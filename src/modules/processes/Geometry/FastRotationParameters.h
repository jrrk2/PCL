//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FastRotationParameters_h
#define __FastRotationParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class FRMode : public MetaEnumeration
{
public:

   enum { Rotate180,          // Rotate 180 degrees
          Rotate90CW,         // Rotate 90 degrees, clockwise
          Rotate90CCW,        // Rotate 90 degrees, counter-clockwise
          HorizontalMirror,   // Horizontal mirror
          VerticalMirror,     // Vertical mirror
          NumberOfModes,
          Default = Rotate180 };

   FRMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern FRMode* TheFRModeParameter;

// ----------------------------------------------------------------------------

extern NoGUIMessages* TheFRNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __FastRotationParameters_h

// ----------------------------------------------------------------------------
// EOF FastRotationParameters.h - Released 2025-04-07T08:53:55Z
