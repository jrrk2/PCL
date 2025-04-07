//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// SampleFormatConversionInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SampleFormatConversionInterface_h
#define __SampleFormatConversionInterface_h

#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>

#include "SampleFormatConversionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SampleFormatConversionInterface : public ProcessInterface
{
public:

   SampleFormatConversionInterface();
   virtual ~SampleFormatConversionInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageCreated( const View& ) override;
   void ImageUpdated( const View& ) override;
   void ImageRenamed( const View& ) override;
   void ImageDeleted( const View& ) override;
   void ImageFocused( const View& ) override;
   void ImageLocked( const View& ) override;
   void ImageUnlocked( const View& ) override;

private:

   SampleFormatConversionInstance instance;

   struct GUIData
   {
      GUIData( SampleFormatConversionInterface& );

      void SetupTrackViewControls( SampleFormatConversionInterface&, bool );

      VerticalSizer     Global_Sizer;

      GroupBox          Conversion_GroupBox;
      VerticalSizer     Conversion_Sizer;
         RadioButton       Integer8Bit_RadioButton;
         RadioButton       Integer16Bit_RadioButton;
         RadioButton       Integer32Bit_RadioButton;
         RadioButton       FloatingPoint32Bit_RadioButton;
         RadioButton       FloatingPoint64Bit_RadioButton;

      GroupBox          CurrentViewInfo_GroupBox;
      HorizontalSizer   CurrentViewInfo_Sizer;
         Label             CurrentViewInfo_Label;
   };

   GUIData* GUI = nullptr;

   void UpdateConversionControls();
   void UpdateCurrentViewInfoControls();

   void __ButtonClick( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SampleFormatConversionInterface* TheSampleFormatConversionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SampleFormatConversionInterface_h

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionInterface.h - Released 2025-04-07T08:53:56Z
