// xisf_wcs.cpp — Dump WCS metadata from XISF and FITS files using PCL
//
// Usage: xisf_wcs <file.xisf|file.fits> [file2 ...]
//
// Build (from macosx/g++):
//   make -f makefile-arm64 PCLSRCDIR=... PCLINCDIR=... PCLLIBDIR64=... \
//        $(OBJ_DIR)/xisf_wcs

#include <pcl/XISF.h>
#include <pcl/WCSKeywords.h>
#include <pcl/AstrometricMetadata.h>
#include <pcl/LinearTransformation.h>
#include <pcl/MetaModule.h>

// FITS module reader — exported from libFITS-pxm
#include "FITS/FITS.h"

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace pcl;

static std::string FmtRA( double deg )
{
   double h = deg / 15.0;
   int hh = int( h );
   double m = ( h - hh ) * 60;
   int mm = int( m );
   double ss = ( m - mm ) * 60;
   char buf[64];
   snprintf( buf, sizeof( buf ), "%02dh%02dm%05.2fs", hh, mm, ss );
   return buf;
}

static std::string FmtDec( double deg )
{
   char sign = deg < 0 ? '-' : '+';
   double a = std::abs( deg );
   int dd = int( a );
   double m = ( a - dd ) * 60;
   int mm = int( m );
   double ss = ( m - mm ) * 60;
   char buf[64];
   snprintf( buf, sizeof( buf ), "%c%02d°%02d'%05.2f\"", sign, dd, mm, ss );
   return buf;
}

static void DumpXISF( const String& path )
{
   std::cout << "======================================================================\n";
   std::cout << "File: " << path.ToUTF8() << "\n";
   std::cout << "======================================================================\n\n";

   XISFReader reader;
   reader.Open( path );

   int numImages = reader.NumberOfImages();
   std::cout << "Images: " << numImages << "\n\n";

   for ( int idx = 0; idx < numImages; ++idx )
   {
      if ( numImages > 1 )
         std::cout << "--- Image " << idx << " ---\n";

      reader.SelectImage( idx );

      pcl::ImageInfo info = reader.ImageInfo();
      int w = info.width;
      int h = info.height;
      int ch = info.numberOfChannels;
      std::cout << "Dimensions: " << w << " x " << h << " x " << ch << "\n\n";

      // Read FITS keywords
      FITSKeywordArray keywords = reader.ReadFITSKeywords();

      // Read image properties
      PropertyArray properties = reader.ReadImageProperties();

      // --- Dump FITS keywords ---
      std::cout << "FITS Keywords (" << keywords.Length() << "):\n";
      for ( const FITSHeaderKeyword& kw : keywords )
      {
         IsoString val = kw.StripValueDelimiters();
         std::cout << "  " << std::left << std::setw( 8 ) << kw.name.c_str()
                   << " = " << std::setw( 24 ) << val.c_str();
         if ( !kw.comment.IsEmpty() )
            std::cout << " / " << kw.comment.c_str();
         std::cout << "\n";
      }
      std::cout << "\n";

      // --- Dump XISF properties ---
      std::cout << "XISF Properties (" << properties.Length() << "):\n";
      for ( const Property& p : properties )
      {
         std::cout << "  " << std::left << std::setw( 55 ) << p.Id().c_str();
         const Variant& v = p.Value();
         switch ( v.Type() )
         {
         case VariantType::Boolean:
            std::cout << "Bool    = " << ( v.ToBool() ? "true" : "false" );
            break;
         case VariantType::Int8:
         case VariantType::Int16:
         case VariantType::Int32:
         case VariantType::Int64:
            std::cout << "Int     = " << v.ToInt64();
            break;
         case VariantType::UInt8:
         case VariantType::UInt16:
         case VariantType::UInt32:
         case VariantType::UInt64:
            std::cout << "UInt    = " << v.ToUInt64();
            break;
         case VariantType::Float32:
         case VariantType::Float64:
            std::cout << "Float64 = " << std::setprecision( 12 ) << v.ToDouble();
            break;
         case VariantType::String:
         case VariantType::IsoString:
            {
               IsoString s = v.ToIsoString();
               if ( s.Length() > 80 )
                  s = s.Left( 77 ) + "...";
               std::cout << "String  = \"" << s.c_str() << "\"";
            }
            break;
         case VariantType::TimePoint:
            std::cout << "Time    = " << v.ToTimePoint().ToString().c_str();
            break;
         default:
            if ( v.IsVector() )
            {
               Vector vec = v.ToVector();
               std::cout << "Vec[" << vec.Length() << "] = {";
               for ( int i = 0; i < vec.Length(); ++i )
               {
                  if ( i ) std::cout << ", ";
                  std::cout << std::setprecision( 12 ) << vec[i];
               }
               std::cout << "}";
            }
            else if ( v.IsMatrix() )
            {
               Matrix mat = v.ToMatrix();
               std::cout << "Mat[" << mat.Rows() << "x" << mat.Columns() << "] = {";
               for ( int r = 0; r < mat.Rows(); ++r )
               {
                  if ( r ) std::cout << "; ";
                  for ( int c = 0; c < mat.Columns(); ++c )
                  {
                     if ( c ) std::cout << ", ";
                     std::cout << std::setprecision( 12 ) << mat[r][c];
                  }
               }
               std::cout << "}";
            }
            else
            {
               std::cout << "(type " << v.Type() << ")";
            }
            break;
         }
         std::cout << "\n";
      }
      std::cout << "\n";

      // --- Parse WCS ---
      std::cout << "=== Parsed WCS (via PCL WCSKeywords) ===\n";
      WCSKeywords wcs( properties, keywords );

      auto printOpt = []( const char* label, const Optional<double>& v, const char* unit = "" ) {
         std::cout << "  " << std::left << std::setw( 12 ) << label;
         if ( v.IsDefined() )
            std::cout << "= " << std::setprecision( 12 ) << v() << " " << unit << "\n";
         else
            std::cout << "= (undefined)\n";
      };
      auto printStr = []( const char* label, const IsoString& s ) {
         std::cout << "  " << std::left << std::setw( 12 ) << label;
         if ( s.IsEmpty() )
            std::cout << "= (empty)\n";
         else
            std::cout << "= " << s.c_str() << "\n";
      };

      if ( wcs.objctra.IsDefined() && wcs.objctdec.IsDefined() )
         std::cout << "  Center     = " << FmtRA( wcs.objctra() ) << "  " << FmtDec( wcs.objctdec() )
                   << "  (" << std::setprecision( 8 ) << wcs.objctra() << "°, " << wcs.objctdec() << "°)\n";

      printStr( "CTYPE1", wcs.ctype1 );
      printStr( "CTYPE2", wcs.ctype2 );
      printOpt( "CRVAL1", wcs.crval1, "deg" );
      printOpt( "CRVAL2", wcs.crval2, "deg" );
      printOpt( "CRPIX1", wcs.crpix1, "px" );
      printOpt( "CRPIX2", wcs.crpix2, "px" );

      std::cout << "  CD matrix:\n";
      printOpt( "  CD1_1", wcs.cd1_1, "deg/px" );
      printOpt( "  CD1_2", wcs.cd1_2, "deg/px" );
      printOpt( "  CD2_1", wcs.cd2_1, "deg/px" );
      printOpt( "  CD2_2", wcs.cd2_2, "deg/px" );

      if ( wcs.cd1_1.IsDefined() && wcs.cd2_2.IsDefined() )
      {
         double cd11 = wcs.cd1_1(), cd12 = wcs.cd1_2.OrElse( 0 );
         double cd21 = wcs.cd2_1.OrElse( 0 ), cd22 = wcs.cd2_2();
         double scaleRA = std::sqrt( cd11*cd11 + cd21*cd21 ) * 3600;
         double scaleDec = std::sqrt( cd12*cd12 + cd22*cd22 ) * 3600;
         double rotation = std::atan2( cd21, cd22 ) * 180.0 / M_PI;
         double det = cd11 * cd22 - cd12 * cd21;
         std::cout << "  Pixel scale: " << std::setprecision( 4 )
                   << scaleRA << " x " << scaleDec << " arcsec/px\n";
         std::cout << "  Rotation:    " << std::setprecision( 4 ) << rotation << " deg\n";
         std::cout << "  Parity:      " << ( det < 0 ? "standard" : "flipped" )
                   << " (det=" << std::setprecision( 6 ) << det << ")\n";
      }

      printOpt( "CDELT1", wcs.cdelt1, "deg/px" );
      printOpt( "CDELT2", wcs.cdelt2, "deg/px" );
      printOpt( "CROTA1", wcs.crota1, "deg" );
      printOpt( "CROTA2", wcs.crota2, "deg" );
      printOpt( "PV1_1", wcs.pv1_1 );
      printOpt( "PV1_2", wcs.pv1_2 );
      printOpt( "LONPOLE", wcs.lonpole, "deg" );
      printOpt( "LATPOLE", wcs.latpole, "deg" );
      printOpt( "FOCALLEN", wcs.focallen, "mm" );
      printOpt( "XPIXSZ", wcs.xpixsz, "um" );
      printStr( "RADESYS", wcs.radesys );
      printOpt( "EQUINOX", wcs.equinox );

      // World transformation
      LinearTransformation transIW;
      if ( wcs.ExtractWorldTransformation( transIW, h ) )
      {
         std::cout << "\n  World transformation extracted OK\n";
         DPoint center( w / 2.0, h / 2.0 );
         DPoint world = transIW( center );
         std::cout << "  Center pixel (" << center.x << ", " << center.y
                   << ") -> world (" << std::setprecision( 8 ) << world.x << ", " << world.y << ")\n";
      }
      else
         std::cout << "\n  *** No valid world transformation ***\n";

      // AstrometricMetadata
      std::cout << "\n=== AstrometricMetadata ===\n";
      try
      {
         AstrometricMetadata amd;
         amd.Build( properties, keywords, w, h );
         if ( amd.IsValid() )
         {
            std::cout << "  Valid:      YES\n";
            std::cout << "  Resolution: " << std::setprecision( 6 ) << amd.Resolution() * 3600 << " arcsec/px\n";

            DPoint rd;
            if ( amd.ImageToCelestial( rd, DPoint( w / 2.0, h / 2.0 ) ) )
               std::cout << "  Center:     " << FmtRA( rd.x ) << "  " << FmtDec( rd.y )
                         << "  (" << std::setprecision( 8 ) << rd.x << "°, " << rd.y << "°)\n";

            DPoint corners[4] = { {0,0}, {double(w),0}, {0,double(h)}, {double(w),double(h)} };
            const char* cnames[4] = { "TL", "TR", "BL", "BR" };
            for ( int i = 0; i < 4; ++i )
            {
               DPoint crd;
               if ( amd.ImageToCelestial( crd, corners[i] ) )
                  std::cout << "  Corner " << cnames[i] << ":  " << FmtRA( crd.x ) << "  " << FmtDec( crd.y ) << "\n";
            }
         }
         else
            std::cout << "  Valid:      NO (no plate solution)\n";
      }
      catch ( const Exception& e )
      {
         std::cout << "  Build failed: " << e.Message().ToUTF8() << "\n";
      }

      std::cout << "\n";
   }

   reader.Close();
}

static void DumpFITS( const String& path )
{
   std::cout << "======================================================================\n";
   std::cout << "File: " << path.ToUTF8() << "\n";
   std::cout << "======================================================================\n\n";

   FITSReader reader;
   reader.Open( path );

   size_type numImages = reader.NumberOfImages();
   std::cout << "Images: " << numImages << "\n\n";

   for ( size_t idx = 0; idx < numImages; ++idx )
   {
      if ( numImages > 1 )
         std::cout << "--- Image " << idx << " ---\n";

      reader.SetIndex( int( idx ) );

      const ImageInfo& info = reader.Info();
      int w = info.width;
      int h = info.height;
      int ch = info.numberOfChannels;
      std::cout << "Dimensions: " << w << " x " << h << " x " << ch << "\n\n";

      // Read FITS keywords
      FITSKeywordArray keywords = reader.ReadFITSKeywords();

      // --- Dump FITS keywords ---
      std::cout << "FITS Keywords (" << keywords.Length() << "):\n";
      for ( const FITSHeaderKeyword& kw : keywords )
      {
         IsoString val = kw.StripValueDelimiters();
         std::cout << "  " << std::left << std::setw( 8 ) << kw.name.c_str()
                   << " = " << std::setw( 24 ) << val.c_str();
         if ( !kw.comment.IsEmpty() )
            std::cout << " / " << kw.comment.c_str();
         std::cout << "\n";
      }
      std::cout << "\n";

      // FITS has no XISF properties — pass empty array
      PropertyArray properties;

      // --- Parse WCS ---
      std::cout << "=== Parsed WCS (via PCL WCSKeywords) ===\n";
      WCSKeywords wcs( properties, keywords );

      auto printOpt = []( const char* label, const Optional<double>& v, const char* unit = "" ) {
         std::cout << "  " << std::left << std::setw( 12 ) << label;
         if ( v.IsDefined() )
            std::cout << "= " << std::setprecision( 12 ) << v() << " " << unit << "\n";
         else
            std::cout << "= (undefined)\n";
      };
      auto printStr = []( const char* label, const IsoString& s ) {
         std::cout << "  " << std::left << std::setw( 12 ) << label;
         if ( s.IsEmpty() )
            std::cout << "= (empty)\n";
         else
            std::cout << "= " << s.c_str() << "\n";
      };

      if ( wcs.objctra.IsDefined() && wcs.objctdec.IsDefined() )
         std::cout << "  Center     = " << FmtRA( wcs.objctra() ) << "  " << FmtDec( wcs.objctdec() )
                   << "  (" << std::setprecision( 8 ) << wcs.objctra() << "°, " << wcs.objctdec() << "°)\n";

      printStr( "CTYPE1", wcs.ctype1 );
      printStr( "CTYPE2", wcs.ctype2 );
      printOpt( "CRVAL1", wcs.crval1, "deg" );
      printOpt( "CRVAL2", wcs.crval2, "deg" );
      printOpt( "CRPIX1", wcs.crpix1, "px" );
      printOpt( "CRPIX2", wcs.crpix2, "px" );

      std::cout << "  CD matrix:\n";
      printOpt( "  CD1_1", wcs.cd1_1, "deg/px" );
      printOpt( "  CD1_2", wcs.cd1_2, "deg/px" );
      printOpt( "  CD2_1", wcs.cd2_1, "deg/px" );
      printOpt( "  CD2_2", wcs.cd2_2, "deg/px" );

      if ( wcs.cd1_1.IsDefined() && wcs.cd2_2.IsDefined() )
      {
         double cd11 = wcs.cd1_1(), cd12 = wcs.cd1_2.OrElse( 0 );
         double cd21 = wcs.cd2_1.OrElse( 0 ), cd22 = wcs.cd2_2();
         double scaleRA = std::sqrt( cd11*cd11 + cd21*cd21 ) * 3600;
         double scaleDec = std::sqrt( cd12*cd12 + cd22*cd22 ) * 3600;
         double rotation = std::atan2( cd21, cd22 ) * 180.0 / M_PI;
         double det = cd11 * cd22 - cd12 * cd21;
         std::cout << "  Pixel scale: " << std::setprecision( 4 )
                   << scaleRA << " x " << scaleDec << " arcsec/px\n";
         std::cout << "  Rotation:    " << std::setprecision( 4 ) << rotation << " deg\n";
         std::cout << "  Parity:      " << ( det < 0 ? "standard" : "flipped" )
                   << " (det=" << std::setprecision( 6 ) << det << ")\n";
      }

      printOpt( "CDELT1", wcs.cdelt1, "deg/px" );
      printOpt( "CDELT2", wcs.cdelt2, "deg/px" );
      printOpt( "CROTA1", wcs.crota1, "deg" );
      printOpt( "CROTA2", wcs.crota2, "deg" );
      printOpt( "PV1_1", wcs.pv1_1 );
      printOpt( "PV1_2", wcs.pv1_2 );
      printOpt( "LONPOLE", wcs.lonpole, "deg" );
      printOpt( "LATPOLE", wcs.latpole, "deg" );
      printOpt( "FOCALLEN", wcs.focallen, "mm" );
      printOpt( "XPIXSZ", wcs.xpixsz, "um" );
      printStr( "RADESYS", wcs.radesys );
      printOpt( "EQUINOX", wcs.equinox );

      // World transformation
      LinearTransformation transIW;
      if ( wcs.ExtractWorldTransformation( transIW, h ) )
      {
         std::cout << "\n  World transformation extracted OK\n";
         DPoint center( w / 2.0, h / 2.0 );
         DPoint world = transIW( center );
         std::cout << "  Center pixel (" << center.x << ", " << center.y
                   << ") -> world (" << std::setprecision( 8 ) << world.x << ", " << world.y << ")\n";
      }
      else
         std::cout << "\n  *** No valid world transformation ***\n";

      // AstrometricMetadata
      std::cout << "\n=== AstrometricMetadata ===\n";
      try
      {
         AstrometricMetadata amd;
         amd.Build( properties, keywords, w, h );
         if ( amd.IsValid() )
         {
            std::cout << "  Valid:      YES\n";
            std::cout << "  Resolution: " << std::setprecision( 6 ) << amd.Resolution() * 3600 << " arcsec/px\n";

            DPoint rd;
            if ( amd.ImageToCelestial( rd, DPoint( w / 2.0, h / 2.0 ) ) )
               std::cout << "  Center:     " << FmtRA( rd.x ) << "  " << FmtDec( rd.y )
                         << "  (" << std::setprecision( 8 ) << rd.x << "°, " << rd.y << "°)\n";

            DPoint corners[4] = { {0,0}, {double(w),0}, {0,double(h)}, {double(w),double(h)} };
            const char* cnames[4] = { "TL", "TR", "BL", "BR" };
            for ( int i = 0; i < 4; ++i )
            {
               DPoint crd;
               if ( amd.ImageToCelestial( crd, corners[i] ) )
                  std::cout << "  Corner " << cnames[i] << ":  " << FmtRA( crd.x ) << "  " << FmtDec( crd.y ) << "\n";
            }
         }
         else
            std::cout << "  Valid:      NO (no plate solution)\n";
      }
      catch ( const Exception& e )
      {
         std::cout << "  Build failed: " << e.Message().ToUTF8() << "\n";
      }

      std::cout << "\n";
   }

   reader.Close();
}

// Minimal module stub for PCL
class XISFWCSModule : public MetaModule
{
public:
   XISFWCSModule() : MetaModule() {}
   const char* Version() const override { return "1.0.0"; }
   IsoString Name() const override { return "xisf_wcs"; }
   String Description() const override { return "XISF WCS Inspector"; }
   String Company() const override { return String(); }
   String Author() const override { return "test"; }
   String Copyright() const override { return String(); }
   String TradeMarks() const override { return String(); }
   String OriginalFileName() const override { return "xisf_wcs"; }
   void GetReleaseDate( int& y, int& m, int& d ) const override { y=2026; m=4; d=1; }
};

extern "C" void* IdentifyPixInsightModule()
{
   static XISFWCSModule module;
   return &module;
}

static bool EndsWithCI( const std::string& s, const char* suffix )
{
   size_t slen = s.size(), xlen = strlen( suffix );
   if ( slen < xlen ) return false;
   for ( size_t i = 0; i < xlen; ++i )
      if ( tolower( s[slen - xlen + i] ) != tolower( suffix[i] ) )
         return false;
   return true;
}

int main( int argc, char** argv )
{
   if ( argc < 2 )
   {
      std::cerr << "Usage: xisf_wcs <file.xisf|file.fits|file.fit> [file2 ...]\n";
      return 1;
   }

   for ( int i = 1; i < argc; ++i )
   {
      try
      {
         String path = String::UTF8ToUTF16( argv[i] );
         std::string arg( argv[i] );
         if ( EndsWithCI( arg, ".fits" ) || EndsWithCI( arg, ".fit" ) || EndsWithCI( arg, ".fts" ) )
            DumpFITS( path );
         else
            DumpXISF( path );
      }
      catch ( const Exception& e )
      {
         std::cerr << "ERROR: " << e.Message().ToUTF8() << "\n";
      }
      catch ( const std::exception& e )
      {
         std::cerr << "ERROR: " << e.what() << "\n";
      }
   }
   return 0;
}
