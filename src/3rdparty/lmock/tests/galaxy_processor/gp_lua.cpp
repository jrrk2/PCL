// gp_lua.cpp -- Lua scripting interface for galaxy_processor
//
// Exposes the galaxy_processor pipeline as a "galaxy" Lua module:
//   galaxy.load(path)            → image userdata
//   galaxy.save(image, path)
//   galaxy.plate_solve(image)    → wcs userdata
//   galaxy.bg_neutralize(image)
//   galaxy.spcc(image, {gaia_db=...})
//   galaxy.sdss_calibrate(image)
//   galaxy.stretch(image, {params...})
//   galaxy.wavelet(image, {params...})
//   galaxy.smart_crop(image)     → cropX, cropY
//   galaxy.stack_lrgb(files, {params...}) → image
//   galaxy.stack_rgb(files, {params...})  → image
//   galaxy.histogram(image)      → table
//   galaxy.info(image)           → table {width, height, channels}

// sol2 must be included BEFORE PCL headers because PCL's Relational.h
// leaks template operator<= into the global namespace, breaking sol2's
// template metaprogramming.
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "gp_common.h"
#include "gp_lua.h"
#include "gp_plate_solver.h"
#include "gp_color_calibration.h"
#include "gp_stretch.h"
#include "gp_histogram.h"
#include "gp_survey.h"
#include "gp_stacking.h"
#include "gp_pcl_modules.h"
#include "galaxy_processor.h"

// ============================================================================
// Lua-visible wrapper types
// ============================================================================

struct LuaImage
{
   Image img;
   FITSKeywordArray keywords;
   PropertyArray properties;
};

struct LuaWCS
{
   SimpleTANWCS wcs;
};

// ============================================================================
// Helper: parse VeraLuxParams from a Lua table
// ============================================================================

static VeraLuxParams parseStretchParams( sol::optional<sol::table> tbl )
{
   VeraLuxParams p;
   if ( !tbl ) return p;
   auto& t = *tbl;

   if ( auto v = t["target_bg"]; v.valid() ) p.target_bg = v.get<double>();
   if ( auto v = t["protect_b"]; v.valid() ) p.protect_b = v.get<double>();
   if ( auto v = t["log_d"]; v.valid() ) p.log_d = v.get<double>();
   if ( auto v = t["convergence"]; v.valid() ) p.convergence = v.get<double>();
   if ( auto v = t["color_grip"]; v.valid() ) p.color_grip = v.get<double>();
   if ( auto v = t["shadow_conv"]; v.valid() ) p.shadow_conv = v.get<double>();
   if ( auto v = t["adaptive"]; v.valid() ) p.adaptive = v.get<bool>();
   if ( auto v = t["add_pedestal"]; v.valid() ) p.add_pedestal = v.get<bool>();
   if ( auto v = t["wavelet_scales"]; v.valid() ) p.wavelet_scales = v.get<int>();
   if ( auto v = t["wavelet_residual"]; v.valid() ) p.wavelet_residual = v.get<double>();
   if ( auto v = t["wavelet_mask_lo_mad"]; v.valid() ) p.wavelet_mask_lo_mad = v.get<double>();
   if ( auto v = t["wavelet_mask_hi_mad"]; v.valid() ) p.wavelet_mask_hi_mad = v.get<double>();

   if ( auto wg = t["wavelet_gain"]; wg.valid() && wg.get_type() == sol::type::table )
   {
      sol::table gains = wg;
      for ( int i = 1; i <= 5 && i <= (int)gains.size(); ++i )
         p.wavelet_gain[i - 1] = gains[i].get<double>();
   }

   if ( auto w = t["weights"]; w.valid() && w.get_type() == sol::type::table )
   {
      sol::table wts = w;
      if ( wts.size() >= 3 )
      {
         p.weights[0] = wts[1].get<double>();
         p.weights[1] = wts[2].get<double>();
         p.weights[2] = wts[3].get<double>();
      }
   }

   return p;
}

// ============================================================================
// Helper: parse StackParams from a Lua table
// ============================================================================

static StackParams parseStackParams( sol::optional<sol::table> tbl )
{
   StackParams p;
   if ( !tbl ) return p;
   auto& t = *tbl;

   if ( auto v = t["gradient"]; v.valid() ) p.gradientDegree = v.get<int>();
   if ( auto v = t["sigma"]; v.valid() )
   {
      p.sigmaLow = v.get<double>();
      p.sigmaHigh = p.sigmaLow;
   }
   if ( auto v = t["sigma_low"]; v.valid() ) p.sigmaLow = v.get<double>();
   if ( auto v = t["sigma_high"]; v.valid() ) p.sigmaHigh = v.get<double>();
   if ( auto v = t["clip_iters"]; v.valid() ) p.clipIterations = v.get<int>();
   if ( auto v = t["psf_weighting"]; v.valid() ) p.psfWeighting = v.get<bool>();
   if ( auto v = t["local_norm"]; v.valid() ) p.localNormalization = v.get<bool>();
   if ( auto v = t["local_norm_grid"]; v.valid() ) p.localNormGridSize = v.get<int>();
   if ( auto v = t["distortion"]; v.valid() ) p.distortionCorrection = v.get<bool>();

   return p;
}

// ============================================================================
// RunLuaScript — main entry point
// ============================================================================

int RunLuaScript( const std::string& scriptPath )
{
   sol::state lua;
   lua.open_libraries( sol::lib::base, sol::lib::math, sol::lib::string,
                       sol::lib::table, sol::lib::io, sol::lib::os,
                       sol::lib::package );

   // Register LuaImage usertype
   lua.new_usertype<LuaImage>( "Image",
      sol::no_constructor,
      "width", sol::property( []( const LuaImage& li ) { return li.img.Width(); } ),
      "height", sol::property( []( const LuaImage& li ) { return li.img.Height(); } ),
      "channels", sol::property( []( const LuaImage& li ) { return li.img.NumberOfChannels(); } ),
      "pixel", []( const LuaImage& li, int x, int y, int c ) -> float {
         if ( x < 0 || x >= li.img.Width() || y < 0 || y >= li.img.Height() ||
              c < 0 || c >= li.img.NumberOfChannels() )
            return 0.0f;
         return li.img( x, y, c );
      },
      "set_pixel", []( LuaImage& li, int x, int y, int c, float v ) {
         if ( x >= 0 && x < li.img.Width() && y >= 0 && y < li.img.Height() &&
              c >= 0 && c < li.img.NumberOfChannels() )
            li.img( x, y, c ) = v;
      }
   );

   // Register LuaWCS usertype
   lua.new_usertype<LuaWCS>( "WCS",
      sol::no_constructor,
      "valid", sol::property( []( const LuaWCS& w ) { return w.wcs.valid; } ),
      "ra", sol::property( []( const LuaWCS& w ) { return w.wcs.crval1; } ),
      "dec", sol::property( []( const LuaWCS& w ) { return w.wcs.crval2; } ),
      "resolution", sol::property( []( const LuaWCS& w ) { return w.wcs.Resolution() * 3600.0; } ),
      "pixel_to_world", []( const LuaWCS& w, double px, double py ) -> sol::object {
         double ra, dec;
         if ( w.wcs.PixelToWorld( px, py, ra, dec ) )
            return sol::make_object( sol::state_view( nullptr ), std::make_tuple( ra, dec ) );
         return sol::nil;
      },
      "world_to_pixel", []( const LuaWCS& w, double ra, double dec ) -> sol::object {
         double px, py;
         if ( w.wcs.WorldToPixel( ra, dec, px, py ) )
            return sol::make_object( sol::state_view( nullptr ), std::make_tuple( px, py ) );
         return sol::nil;
      }
   );

   // Create galaxy namespace
   sol::table galaxy = lua.create_named_table( "galaxy" );

   // galaxy.load(path) → LuaImage
   galaxy["load"] = [&lua]( const std::string& path ) -> std::shared_ptr<LuaImage>
   {
      auto li = std::make_shared<LuaImage>();
      String pclPath = String::UTF8ToUTF16( path.c_str() );

      bool isFITS = EndsWithCI( path, ".fits" ) || EndsWithCI( path, ".fit" )
                 || EndsWithCI( path, ".fts" );

      if ( isFITS )
      {
         std::cout << "Loading FITS: " << path << "\n";
         FITSReader reader;
         reader.Open( pclPath );
         reader.SetIndex( 0 );
         li->keywords = reader.ReadFITSKeywords();
         reader.ReadImage( li->img );
         reader.Close();
      }
      else
      {
         std::cout << "Loading XISF: " << path << "\n";
         XISFReader reader;
         reader.Open( pclPath );
         reader.SelectImage( 0 );
         li->keywords = reader.ReadFITSKeywords();
         li->properties = reader.ReadImageProperties();
         reader.ReadImage( li->img );

         // Handle UInt16 fallback
         bool allZero = true;
         if ( li->img.Width() > 0 && li->img.PixelData( 0 ) != nullptr )
         {
            const float* p = li->img.PixelData( 0 );
            for ( int i = 0; i < std::min( li->img.Width() * li->img.Height(), 10000 ); ++i )
               if ( p[i] != 0.0f ) { allZero = false; break; }
         }
         if ( allZero && li->img.Width() > 0 )
         {
            std::cout << "  Float32 zero, trying UInt16...\n";
            reader.Close();
            reader.Open( pclPath );
            reader.SelectImage( 0 );
            UInt16Image u16;
            reader.ReadImage( u16 );
            reader.Close();
            if ( u16.Width() > 0 && u16.PixelData( 0 ) )
            {
               int iw = u16.Width(), ih = u16.Height(), ic = u16.NumberOfChannels();
               li->img.AllocateData( iw, ih, ic, u16.ColorSpace() );
               for ( int c = 0; c < ic; ++c )
               {
                  const uint16* src = u16.PixelData( c );
                  float* dst = li->img.PixelData( c );
                  for ( int i = 0; i < iw * ih; ++i )
                     dst[i] = float( src[i] ) / 65535.0f;
               }
            }
         }
         else
            reader.Close();
      }

      std::cout << "  Loaded: " << li->img.Width() << "x" << li->img.Height()
                << "x" << li->img.NumberOfChannels() << "\n";
      return li;
   };

   // galaxy.save(image, path)
   galaxy["save"] = []( std::shared_ptr<LuaImage> li, const std::string& path )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      String pclPath = String::UTF8ToUTF16( path.c_str() );

      bool isFITS = EndsWithCI( path, ".fits" ) || EndsWithCI( path, ".fit" );
      if ( isFITS )
      {
         FITSWriter writer;
         writer.Create( pclPath, 1 );
         ImageOptions opts;
         opts.bitsPerSample = 32;
         opts.ieeefpSampleFormat = true;
         writer.SetOptions( opts );
         writer.WriteFITSKeywords( li->keywords );
         writer.WriteImage( li->img );
         writer.Close();
      }
      else
      {
         XISFWriter writer;
         writer.Create( pclPath, 1 );
         ImageOptions opts;
         opts.bitsPerSample = 32;
         opts.ieeefpSampleFormat = true;
         writer.SetImageOptions( opts );
         writer.WriteFITSKeywords( li->keywords );
         for ( const auto& p : li->properties )
            writer.WriteImageProperty( p.Id(), p.Value() );
         writer.WriteImage( li->img );
         writer.Close();
      }
      std::cout << "Saved: " << path << "\n";
   };

   // galaxy.info(image) → {width, height, channels}
   galaxy["info"] = [&lua]( std::shared_ptr<LuaImage> li ) -> sol::table
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      sol::table t = lua.create_table();
      t["width"] = li->img.Width();
      t["height"] = li->img.Height();
      t["channels"] = li->img.NumberOfChannels();
      return t;
   };

   // galaxy.plate_solve(image) → LuaWCS
   galaxy["plate_solve"] = [&lua]( std::shared_ptr<LuaImage> li ) -> std::shared_ptr<LuaWCS>
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      auto lw = std::make_shared<LuaWCS>();
      BuildWCSFromMetadata( lw->wcs, li->properties, li->keywords );
      if ( PlateSolve( li->img, lw->wcs, li->keywords ) )
         std::cout << "Plate solve successful.\n";
      else
         std::cerr << "Plate solve failed.\n";
      return lw;
   };

   // galaxy.bg_neutralize(image)
   galaxy["bg_neutralize"] = []( std::shared_ptr<LuaImage> li )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      BackgroundNeutralize( li->img );
   };

   // galaxy.spcc(image, {gaia_db={...}})
   galaxy["spcc"] = []( std::shared_ptr<LuaImage> li, sol::optional<sol::table> opts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      std::vector<std::string> gaiaDbPaths;
      if ( opts )
      {
         if ( auto db = (*opts)["gaia_db"]; db.valid() )
         {
            if ( db.get_type() == sol::type::string )
               gaiaDbPaths.push_back( db.get<std::string>() );
            else if ( db.get_type() == sol::type::table )
            {
               sol::table dbt = db;
               for ( auto& kv : dbt )
                  gaiaDbPaths.push_back( kv.second.as<std::string>() );
            }
         }
      }
      SpectrophotometricColorCalibrate( li->img, li->keywords, li->properties, gaiaDbPaths );
   };

   // galaxy.sdss_calibrate(image)
   galaxy["sdss_calibrate"] = []( std::shared_ptr<LuaImage> li )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      SDSSColorCalibrate( li->img, li->keywords );
   };

   // galaxy.smart_crop(image) → cropX, cropY
   galaxy["smart_crop"] = []( std::shared_ptr<LuaImage> li ) -> std::tuple<int, int>
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      int cropX = 0, cropY = 0;
      SmartCropStackingEdges( li->img, cropX, cropY );
      return { cropX, cropY };
   };

   // galaxy.stretch(image, {params...})
   galaxy["stretch"] = []( std::shared_ptr<LuaImage> li, sol::optional<sol::table> opts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      VeraLuxParams p = parseStretchParams( opts );
      VeraLuxStretch( li->img, p );
   };

   // galaxy.wavelet(image, {params...})
   galaxy["wavelet"] = []( std::shared_ptr<LuaImage> li, sol::optional<sol::table> opts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      VeraLuxParams p = parseStretchParams( opts );
      WaveletEnhance( li->img, p );
   };

   // galaxy.histogram(image) → table
   galaxy["histogram"] = [&lua]( std::shared_ptr<LuaImage> li ) -> sol::table
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      ImageHistogram h = compute_histogram( li->img );
      sol::table t = lua.create_table();

      sol::table lum = lua.create_table();
      for ( int i = 0; i < HIST_BINS; ++i )
         lum[i + 1] = h.lum[i];
      t["luminance"] = lum;

      sol::table pct = lua.create_table();
      for ( int i = 0; i <= 10; ++i )
         pct[i] = h.percentiles[i];
      t["percentiles"] = pct;

      const char* chNames[] = { "red", "green", "blue" };
      for ( int c = 0; c < 3; ++c )
      {
         sol::table ch = lua.create_table();
         for ( int i = 0; i < HIST_BINS; ++i )
            ch[i + 1] = h.ch[c][i];
         t[chNames[c]] = ch;
      }
      return t;
   };

   // galaxy.stack_lrgb(files, {params...}) → LuaImage
   galaxy["stack_lrgb"] = [&lua]( sol::table files, sol::optional<sol::table> opts ) -> std::shared_ptr<LuaImage>
   {
      QStringList qPaths;
      for ( auto& kv : files )
         qPaths.append( QString::fromStdString( kv.second.as<std::string>() ) );
      StackParams sp = parseStackParams( opts );
      auto li = std::make_shared<LuaImage>();
      li->img = StackFramesLRGB( qPaths, sp );
      return li;
   };

   // galaxy.stack_rgb(files, {params...}) → LuaImage
   galaxy["stack_rgb"] = [&lua]( sol::table files, sol::optional<sol::table> opts ) -> std::shared_ptr<LuaImage>
   {
      QStringList qPaths;
      for ( auto& kv : files )
         qPaths.append( QString::fromStdString( kv.second.as<std::string>() ) );
      StackParams sp = parseStackParams( opts );
      auto li = std::make_shared<LuaImage>();
      li->img = StackFramesRGB( qPaths, sp );
      return li;
   };

   // galaxy.set_gaia_db(path) — set the Gaia astrometry database path
   galaxy["set_gaia_db"] = []( const std::string& path )
   {
      g_gaiaAstroDbPath = path;
      std::cout << "Gaia astrometry DB: " << path << "\n";
   };

   // galaxy.optimize(image, wcs, {iterations=50})
   galaxy["optimize"] = []( std::shared_ptr<LuaImage> li, std::shared_ptr<LuaWCS> lw,
                            sol::optional<sol::table> opts )
   {
      if ( !li || !lw ) throw std::runtime_error( "nil image or wcs" );
      int maxIter = 50;
      std::string layer = "sdss";
      if ( opts )
      {
         if ( auto v = (*opts)["iterations"]; v.valid() ) maxIter = v.get<int>();
         if ( auto v = (*opts)["survey"]; v.valid() ) layer = v.get<std::string>();
      }
      double centerRA = lw->wcs.crval1;
      double centerDec = lw->wcs.crval2;
      double pixscale = lw->wcs.valid ? lw->wcs.Resolution() * 3600.0 : 0;
      RunOptimization( li->img, maxIter, lw->wcs, centerRA, centerDec, pixscale, layer.c_str() );
   };

   // ---- PCL module wrappers ------------------------------------------------

   // galaxy.histogram_transform(image, {midtones=0.5, shadows=0, highlights=1})
   galaxy["histogram_transform"] = []( std::shared_ptr<LuaImage> li, sol::optional<sol::table> opts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      float m = 0.5f, s = 0.0f, h = 1.0f;
      if ( opts )
      {
         if ( auto v = (*opts)["midtones"]; v.valid() ) m = v.get<float>();
         if ( auto v = (*opts)["shadows"]; v.valid() ) s = v.get<float>();
         if ( auto v = (*opts)["highlights"]; v.valid() ) h = v.get<float>();
      }
      ApplyHistogramTransformation( li->img, m, s, h );
   };

   // galaxy.curves(image, {{x,y}, {x,y}, ...})
   galaxy["curves"] = []( std::shared_ptr<LuaImage> li, sol::table pts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      std::vector<CurvePoint> curve;
      for ( auto& kv : pts )
      {
         sol::table pt = kv.second.as<sol::table>();
         curve.push_back( { pt[1].get<double>(), pt[2].get<double>() } );
      }
      ApplyCurvesTransformation( li->img, curve );
   };

   // galaxy.unsharp_mask(image, {sigma=2, amount=0.5})
   galaxy["unsharp_mask"] = []( std::shared_ptr<LuaImage> li, sol::optional<sol::table> opts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      float sigma = 2.0f, amount = 0.5f;
      if ( opts )
      {
         if ( auto v = (*opts)["sigma"]; v.valid() ) sigma = v.get<float>();
         if ( auto v = (*opts)["amount"]; v.valid() ) amount = v.get<float>();
      }
      ApplyUnsharpMask( li->img, sigma, amount );
   };

   // galaxy.morphology(image, {op=4, size=1})
   galaxy["morphology"] = []( std::shared_ptr<LuaImage> li, sol::optional<sol::table> opts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      int op = 4, size = 1;
      if ( opts )
      {
         if ( auto v = (*opts)["op"]; v.valid() ) op = v.get<int>();
         if ( auto v = (*opts)["size"]; v.valid() ) size = v.get<int>();
      }
      ApplyMorphologicalTransformation( li->img, op, size );
   };

   // galaxy.auto_histogram(image)
   galaxy["auto_histogram"] = []( std::shared_ptr<LuaImage> li )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      ApplyAutoHistogram( li->img );
   };

   // galaxy.convolve(image, {sigma=2})
   galaxy["convolve"] = []( std::shared_ptr<LuaImage> li, sol::optional<sol::table> opts )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      float sigma = 2.0f;
      if ( opts ) { if ( auto v = (*opts)["sigma"]; v.valid() ) sigma = v.get<float>(); }
      ApplyConvolution( li->img, sigma );
   };

   // galaxy.resample(image, scale)
   galaxy["resample"] = []( std::shared_ptr<LuaImage> li, double scale )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      ApplyResample( li->img, scale );
   };

   // galaxy.crop(image, left, top, right, bottom)
   galaxy["crop"] = []( std::shared_ptr<LuaImage> li, int l, int t, int r, int b )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      ApplyCrop( li->img, l, t, r, b );
   };

   // galaxy.rotate(image, angle_degrees)
   galaxy["rotate"] = []( std::shared_ptr<LuaImage> li, double angle )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      ApplyRotation( li->img, angle );
   };

   // galaxy.pixel_math(image, expression)
   galaxy["pixel_math"] = []( std::shared_ptr<LuaImage> li, const std::string& expr )
   {
      if ( !li ) throw std::runtime_error( "nil image" );
      ApplyPixelMath( li->img, expr );
   };

   // Run the script
   std::cout << "Running Lua script: " << scriptPath << "\n";
   try
   {
      auto result = lua.safe_script_file( scriptPath );
      if ( !result.valid() )
      {
         sol::error err = result;
         std::cerr << "Lua error: " << err.what() << "\n";
         return 1;
      }
   }
   catch ( const sol::error& e )
   {
      std::cerr << "Lua error: " << e.what() << "\n";
      return 1;
   }
   catch ( const std::exception& e )
   {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
   }

   std::cout << "Lua script completed.\n";
   return 0;
}
