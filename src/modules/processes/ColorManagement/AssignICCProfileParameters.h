//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// AssignICCProfileParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AssignICCProfileParameters_h
#define __AssignICCProfileParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ICCATargetProfile : public MetaString
{
public:

   ICCATargetProfile( MetaProcess* );

   IsoString Id() const override;
};

extern ICCATargetProfile* TheICCATargetProfileParameter;

// ----------------------------------------------------------------------------

class ICCAMode : public MetaEnumeration
{
public:

   enum { AssignNewProfile,      // specify a new ICC profile
          AssignDefaultProfile,  // assign current default RGB or grayscale profile
          LeaveUntagged,         // leave the image untagged (with no profile assigned)
          NumberOfAssignModes,
          Default = AssignDefaultProfile };

   ICCAMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICCAMode* TheICCAModeParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __AssignICCProfileParameters_h

// ----------------------------------------------------------------------------
// EOF AssignICCProfileParameters.h - Released 2025-04-07T08:53:55Z
