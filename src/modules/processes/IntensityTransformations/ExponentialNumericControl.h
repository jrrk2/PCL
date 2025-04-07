//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialNumericControl.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __PCL_ExponentialNumericControl_h
#define __PCL_ExponentialNumericControl_h

/// \file pcl/ExponentialNumericControl.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_NumericControl_h
#include <pcl/NumericControl.h>
#endif

#ifndef __PCL_SpinBox_h
#include <pcl/SpinBox.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ExponentialNumericControl : public NumericEdit
{
public:

   ExponentialNumericControl( Control& parent = Null() );

   virtual ~ExponentialNumericControl()
   {
   }

private:

   NumericControl m_coefficient;
   SpinBox        m_exponent;

   void __CoefficientValueUpdated( NumericEdit& sender, double value );
   void __ExponentValueUpdated( SpinBox& sender, int value );

   virtual void UpdateControls();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ExponentialNumericControl_h

// ----------------------------------------------------------------------------
// EOF ExponentialNumericControl.h - Released 2025-04-07T08:53:56Z
