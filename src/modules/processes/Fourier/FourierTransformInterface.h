//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// FourierTransformInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FourierTransformInterface_h
#define __FourierTransformInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>

#include "FourierTransformInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FourierTransformInterface : public ProcessInterface
{
public:

   FourierTransformInterface();
   virtual ~FourierTransformInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   FourierTransformInstance instance;

   struct GUIData
   {
      GUIData( FourierTransformInterface& );

      VerticalSizer  Global_Sizer;
         CheckBox       Shift_CheckBox;
         CheckBox       Radial_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

extern FourierTransformInterface* TheFourierTransformInterface;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FourierTransformInterface_h

// ----------------------------------------------------------------------------
// EOF FourierTransformInterface.h - Released 2025-04-07T08:53:55Z
