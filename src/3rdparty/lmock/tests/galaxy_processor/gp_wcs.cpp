#include "gp_wcs.h"

// ============================================================================
// SimpleTANWCS helper: build from PCL metadata
// (pixelToWorld/worldToPixel/pixelScale are now in stackengine_cli.cpp)
// ============================================================================

bool BuildWCSFromMetadata( SimpleTANWCS& w, const PropertyArray& properties, const FITSKeywordArray& keywords )
{
   w.valid = false;

   WCSKeywords wcsKeys( properties, keywords );

   if ( !wcsKeys.crval1.IsDefined() || !wcsKeys.crval2.IsDefined() ||
        !wcsKeys.crpix1.IsDefined() || !wcsKeys.crpix2.IsDefined() ||
        !wcsKeys.cd1_1.IsDefined() || !wcsKeys.cd1_2.IsDefined() ||
        !wcsKeys.cd2_1.IsDefined() || !wcsKeys.cd2_2.IsDefined() )
      return false;

   w.crval1 = wcsKeys.crval1();
   w.crval2 = wcsKeys.crval2();
   w.crpix1 = wcsKeys.crpix1();
   w.crpix2 = wcsKeys.crpix2();
   w.cd11 = wcsKeys.cd1_1();
   w.cd12 = wcsKeys.cd1_2();
   w.cd21 = wcsKeys.cd2_1();
   w.cd22 = wcsKeys.cd2_2();

   double det = w.cd11 * w.cd22 - w.cd12 * w.cd21;
   if ( std::abs( det ) < 1e-20 )
      return false;

   w.valid = true;
   return true;
}

// ============================================================================
// Global WCS state definitions
// ============================================================================

std::unique_ptr<WCSData> g_solvedWCS;
int g_cropOffsetX = 0;
int g_cropOffsetY = 0;
std::string g_gaiaAstroDbPath;
