//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// AssignICCProfileParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AssignICCProfileParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ICCATargetProfile* TheICCATargetProfileParameter = nullptr;
ICCAMode*          TheICCAModeParameter = nullptr;

// ----------------------------------------------------------------------------

ICCATargetProfile::ICCATargetProfile( MetaProcess* P ) : MetaString( P )
{
   TheICCATargetProfileParameter = this;
}

IsoString ICCATargetProfile::Id() const
{
   return "targetProfile";
}

// ----------------------------------------------------------------------------

ICCAMode::ICCAMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICCAModeParameter = this;
}

IsoString ICCAMode::Id() const
{
   return "mode";
}

size_type ICCAMode::NumberOfElements() const
{
   return NumberOfAssignModes;
}

IsoString ICCAMode::ElementId( size_type id ) const
{
   switch ( id )
   {
   default:
   case AssignNewProfile:     return "AssignNewProfile";
   case AssignDefaultProfile: return "AssignDefaultProfile";
   case LeaveUntagged:        return "LeaveUntagged";
   }
}

int ICCAMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICCAMode::DefaultValueIndex() const
{
   return int( Default );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssignICCProfileParameters.cpp - Released 2025-04-07T08:53:55Z
