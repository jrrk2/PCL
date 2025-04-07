//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIMountParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDIMountParameters_h
#define __INDIMountParameters_h

#include <pcl/MetaParameter.h>

#include "ModuleParameterRegistration.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

DECLARE_STRING_PARAMETER_CLASS( IMCDeviceName );

// ----------------------------------------------------------------------------

#define COMMAND_ENUM ENUM_ITEM( Unpark )                                    \
ENUM_ITEM( Park )                                                           \
   ENUM_ITEM( MoveNorthStart ) ENUM_ITEM( MoveNorthStop )                   \
      ENUM_ITEM( MoveSouthStart ) ENUM_ITEM( MoveSouthStop )                \
         ENUM_ITEM( MoveWestStart ) ENUM_ITEM( MoveWestStop )               \
            ENUM_ITEM( MoveEastStart ) ENUM_ITEM( MoveEastStop )            \
               ENUM_ITEM( GoTo ) ENUM_ITEM( Sync ) ENUM_ITEM( ParkDefault ) \
                  ENUM_ITEM( FitPointingModel ) ENUM_ITEM( TestSync )

DECLARE_ENUM_PARAMETER( IMCCommand, COMMAND_ENUM, Unpark );

// ----------------------------------------------------------------------------

#define SLEW_RATE_ENUM ENUM_ITEM( Guide ) \
ENUM_ITEM( Centering )                    \
   ENUM_ITEM( Find ) ENUM_ITEM( Max )

DECLARE_ENUM_PARAMETER( IMCSlewRate, SLEW_RATE_ENUM, Max );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_PARAMETER_CLASS( IMCTargetRA );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_PARAMETER_CLASS( IMCTargetDec );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCCurrentLST );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCCurrentRA );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCCurrentDec );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCApparentTargetRA );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCApparentTargetDec );

// ----------------------------------------------------------------------------

DECLARE_DOUBLE_PARAMETER_CLASS( IMCGeographicLatitude );

// ----------------------------------------------------------------------------

#define PIERSIDE_ENUM ENUM_ITEM( West ) \
ENUM_ITEM( East ) ENUM_ITEM( None )
DECLARE_ENUM_PARAMETER( IMCPierSide, PIERSIDE_ENUM, None );

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncLST );

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncCelestialRA );

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncCelestialDec );

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncTelescopeRA );

DECLARE_DOUBLE_READ_ONLY_PARAMETER_CLASS( IMCSyncTelescopeDec );

// ----------------------------------------------------------------------------

DECLARE_BOOLEAN_PARAMETER_CLASS( IMCEnableAlignmentCorrection );

#define ALIGNMENT_ENUM ENUM_ITEM( None ) \
ENUM_ITEM( AnalyticalModel ) ENUM_ITEM( ServerModel )
DECLARE_ENUM_PARAMETER( IMCAlignmentMethod, ALIGNMENT_ENUM, None );

DECLARE_UINT32_PARAMETER_CLASS( IMCAlignmentConfig );

// ----------------------------------------------------------------------------

DECLARE_STRING_PARAMETER_CLASS( IMCAlignmentFile );

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __INDIMountParameters_h

// ----------------------------------------------------------------------------
// EOF INDIMountParameters.h - Released 2025-04-07T08:53:56Z
