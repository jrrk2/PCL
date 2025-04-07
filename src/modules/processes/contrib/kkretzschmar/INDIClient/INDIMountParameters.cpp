//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIMountParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "INDIMountParameters.h"

namespace pcl
{
// ----------------------------------------------------------------------------

REGISTER_MODULE_PARAMETER( IMCDeviceName );
REGISTER_MODULE_PARAMETER( IMCCommand );
REGISTER_MODULE_PARAMETER( IMCSlewRate );
REGISTER_MODULE_PARAMETER( IMCTargetRA );
REGISTER_MODULE_PARAMETER( IMCTargetDec );
REGISTER_MODULE_PARAMETER( IMCCurrentLST );
REGISTER_MODULE_PARAMETER( IMCCurrentRA );
REGISTER_MODULE_PARAMETER( IMCCurrentDec );
REGISTER_MODULE_PARAMETER( IMCApparentTargetRA );
REGISTER_MODULE_PARAMETER( IMCApparentTargetDec );
REGISTER_MODULE_PARAMETER( IMCGeographicLatitude );
REGISTER_MODULE_PARAMETER( IMCPierSide );
REGISTER_MODULE_PARAMETER( IMCSyncLST );
REGISTER_MODULE_PARAMETER( IMCSyncCelestialRA );
REGISTER_MODULE_PARAMETER( IMCSyncCelestialDec );
REGISTER_MODULE_PARAMETER( IMCSyncTelescopeRA );
REGISTER_MODULE_PARAMETER( IMCSyncTelescopeDec );
REGISTER_MODULE_PARAMETER( IMCAlignmentMethod );
REGISTER_MODULE_PARAMETER( IMCEnableAlignmentCorrection );
REGISTER_MODULE_PARAMETER( IMCAlignmentFile );
REGISTER_MODULE_PARAMETER( IMCAlignmentConfig );

// ----------------------------------------------------------------------------

DEFINE_STRING_PARAMETER_CLASS( IMCDeviceName, "deviceName" );

// ----------------------------------------------------------------------------

#undef ENUM_ITEM
#define ENUM_ITEM( X ) \
   case X:             \
      return "Command_" #X;
DEFINE_ENUM_PARAMETER_CLASS( IMCCommand, "Command", COMMAND_ENUM, Unpark );

// ----------------------------------------------------------------------------

#undef ENUM_ITEM
#define ENUM_ITEM( X ) \
   case X:             \
      return "SlewRate_" #X;
DEFINE_ENUM_PARAMETER_CLASS( IMCSlewRate, "SlewRate", SLEW_RATE_ENUM, Max );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_PARAMETER_CLASS( IMCTargetRA, "targetRA", 8, 0, 0, 24 );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_PARAMETER_CLASS( IMCTargetDec, "targetDec", 7, 0, -90, 90 );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCCurrentLST, "currentLST", 8, 0, 0, 24 );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCCurrentRA, "currentRA", 8, -1, -1, 24 );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCCurrentDec, "currentDec", 7, -91, -91, 90 );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCApparentTargetRA, "apparentTargetRA", 8, -1, -1, 24 );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCApparentTargetDec, "apparentTargetDec", 7, -91, -91, 90 );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_PARAMETER_CLASS( IMCGeographicLatitude, "geographicLatitude", 7, -91, -91, 90 );

// ----------------------------------------------------------------------------

#undef ENUM_ITEM
#define ENUM_ITEM( X ) \
   case X:             \
      return "PierSide_" #X;
DEFINE_ENUM_PARAMETER_CLASS( IMCPierSide, "PierSide", PIERSIDE_ENUM, None );

// ----------------------------------------------------------------------------

DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncLST, "syncLST", 8, 0, 0, 24 );
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncCelestialRA, "syncCelestialRA", 8, 0, 0, 24 );
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncCelestialDec, "syncCelestialDec", 7, 0, -90, 90 );
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncTelescopeRA, "syncTelescopeRA", 8, 0, 0, 24 );
DEFINE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncTelescopeDec, "syncTelescopeDec", 7, 0, -90, 90 );

// ----------------------------------------------------------------------------

DEFINE_BOOLEAN_PARAMETER_CLASS( IMCEnableAlignmentCorrection, "enableAlignmentCorrection", false );

#undef ENUM_ITEM
#define ENUM_ITEM( X ) \
   case X:             \
      return "AlignmentMethod_" #X;
DEFINE_ENUM_PARAMETER_CLASS( IMCAlignmentMethod, "AlignmentMethod", ALIGNMENT_ENUM, None );

DEFINE_STRING_PARAMETER_CLASS( IMCAlignmentFile, "alignmentModelFile" );

DEFINE_UINT32_PARAMETER_CLASS( IMCAlignmentConfig, "alignmentConfig", 0, 1, 2047 );

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF INDIMountParameters.cpp - Released 2025-04-07T08:53:56Z
