//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// InverseFourierTransformInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __InverseFourierTransformInterface_h
#define __InverseFourierTransformInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/ToolButton.h>
#include <pcl/ComboBox.h>

#include "InverseFourierTransformInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class InverseFourierTransformInterface : public ProcessInterface
{
public:

   InverseFourierTransformInterface();
   virtual ~InverseFourierTransformInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   InverseFourierTransformInstance instance;

   struct GUIData
   {
      GUIData( InverseFourierTransformInterface& );

      VerticalSizer  Global_Sizer;
      HorizontalSizer   FirstComponent_Sizer;
         Label             FirstComponent_Label;
         Edit              FirstComponent_Edit;
         ToolButton        FirstComponent_ToolButton;
      HorizontalSizer   SecondComponent_Sizer;
         Label             SecondComponent_Label;
         Edit              SecondComponent_Edit;
         ToolButton        SecondComponent_ToolButton;
      HorizontalSizer   OnOutOfRangeResult_Sizer;
         Label             OnOutOfRangeResult_Label;
         ComboBox          OnOutOfRangeResult_ComboBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __EditCompleted( Edit& sender );
   void __Click( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

extern InverseFourierTransformInterface* TheInverseFourierTransformInterface;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __InverseFourierTransformInterface_h

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformInterface.h - Released 2025-04-07T08:53:55Z
