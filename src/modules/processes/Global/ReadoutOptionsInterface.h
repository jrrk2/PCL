//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ReadoutOptionsInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ReadoutOptionsInterface_h
#define __ReadoutOptionsInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "ReadoutOptionsInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ReadoutOptionsInterface : public ProcessInterface
{
public:

   ReadoutOptionsInterface();
   virtual ~ReadoutOptionsInterface();

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

   ReadoutOptionsInstance m_instance;

   struct GUIData
   {
      GUIData( ReadoutOptionsInterface& );

      HorizontalSizer   Global_Sizer;

         VerticalSizer     Left_Sizer;
            HorizontalSizer   Data_Sizer;
               Label             Data_Label;
               ComboBox          Data_ComboBox;
            HorizontalSizer   Mode_Sizer;
               Label             Mode_Label;
               ComboBox          Mode_ComboBox;
            HorizontalSizer   ProbeSize_Sizer;
               Label             ProbeSize_Label;
               ComboBox          ProbeSize_ComboBox;
            HorizontalSizer   Alpha_Sizer;
               CheckBox          ShowAlphaChannel_CheckBox;
            HorizontalSizer   Mask_Sizer;
               CheckBox          ShowMaskChannel_CheckBox;
            HorizontalSizer   Preview_Sizer;
               CheckBox          ShowPreview_CheckBox;
            HorizontalSizer   PreviewCenter_Sizer;
               CheckBox          PreviewCenter_CheckBox;
            HorizontalSizer   PreviewSize_Sizer;
               Label             PreviewSize_Label;
               SpinBox           PreviewSize_SpinBox;
            HorizontalSizer   PreviewZoom_Sizer;
               Label             PreviewZoom_Label;
               ComboBox          PreviewZoom_ComboBox;

         VerticalSizer     Right_Sizer;
            HorizontalSizer   Real_Sizer;
               Label             Precision_Label;
               ComboBox          Precision_ComboBox;
               CheckBox          Real_CheckBox;
            HorizontalSizer   BinaryInteger_Sizer;
               Label             BinaryInteger_Label;
               ComboBox          BinaryInteger_ComboBox;
               CheckBox          BinaryInteger_CheckBox;
            HorizontalSizer   ArbitraryInteger_Sizer;
               Label             ArbitraryInteger_Label;
               SpinBox           ArbitraryInteger_SpinBox;
               CheckBox          ArbitraryInteger_CheckBox;
            HorizontalSizer   ShowEquatorial_Sizer;
               CheckBox          ShowEquatorial_CheckBox;
            HorizontalSizer   ShowEcliptic_Sizer;
               CheckBox          ShowEcliptic_CheckBox;
            HorizontalSizer   ShowGalactic_Sizer;
               CheckBox          ShowGalactic_CheckBox;
            HorizontalSizer   CoordinateItems_Sizer;
               Label             CoordinateItems_Label;
               ComboBox          CoordinateItems_ComboBox;
            HorizontalSizer   CoordinatePrecision_Sizer;
               Label             CoordinatePrecision_Label;
               SpinBox           CoordinatePrecision_SpinBox;
            HorizontalSizer   Broadcast_Sizer;
               CheckBox          Broadcast_CheckBox;
            HorizontalSizer   LoadCurrentOptions_Sizer;
               PushButton        LoadCurrentOptions_PushButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ButtonClick( Button& sender, bool checked );
   void __ValueUpdated( SpinBox& sender, int value );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ReadoutOptionsInterface* TheReadoutOptionsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ReadoutOptionsInterface_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsInterface.h - Released 2025-04-07T08:53:56Z
