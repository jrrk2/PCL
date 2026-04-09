// gp_wcs.h -- SimpleTANWCS and WCS utilities
#pragma once

#include "gp_common.h"

// SimpleTANWCS is now defined in stackengine_cli.h (via gp_common.h)
// Additional methods for galaxy_processor use:
bool BuildWCSFromMetadata( SimpleTANWCS& wcs, const PropertyArray& properties, const FITSKeywordArray& keywords );

// Global plate-solve state
extern std::unique_ptr<WCSData> g_solvedWCS;
extern int g_cropOffsetX;
extern int g_cropOffsetY;
extern std::string g_gaiaAstroDbPath;
