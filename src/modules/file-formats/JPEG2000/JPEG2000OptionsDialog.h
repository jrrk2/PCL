//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 1.0.3
// ----------------------------------------------------------------------------
// JPEG2000OptionsDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __JPEG2000OptionsDialog_h
#define __JPEG2000OptionsDialog_h

#include "JPEG2000Instance.h"

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/CheckBox.h>
#include <pcl/SpinBox.h>
#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class JPEG2000OptionsDialog : public Dialog
{
public:

   JPEG2000OptionsDialog( const ImageOptions&, const JPEG2000ImageOptions&, bool isJPC );

   ImageOptions         options;
   JPEG2000ImageOptions jp2Options;

protected:

   VerticalSizer     Global_Sizer;

   GroupBox          Compression_GroupBox;
   VerticalSizer     Compression_Sizer;
      RadioButton       Lossless_RadioButton;
      RadioButton       Lossy_RadioButton;
      NumericControl    CompressionRate_NumericControl;
      CheckBox          UnsignedSample_CheckBox;

   GroupBox          Progressive_GroupBox;
   VerticalSizer     Progressive_Sizer;
      HorizontalSizer   DecompressionLayers_Sizer;
         Label             DecompressionLayers_Label;
         SpinBox           DecompressionLayers_SpinBox;
      HorizontalSizer   ProgressionOrder_Sizer;
         Label             ProgressionOrder_Label;
         ComboBox          ProgressionOrder_ComboBox;

   GroupBox          Tiled_GroupBox;
   HorizontalSizer   Tiled_Sizer;
      NumericEdit       TileWidth_NumericEdit;
      NumericEdit       TileHeight_NumericEdit;

   GroupBox          EmbeddedData_GroupBox;
   HorizontalSizer   EmbeddedData_Sizer;
      CheckBox          ICCProfile_CheckBox;
      CheckBox          Resolution_CheckBox;

   HorizontalSizer   BottomSection_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void CompressionMode_Click( Button& sender, bool checked );
   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEG2000OptionsDialog_h

// ----------------------------------------------------------------------------
// EOF JPEG2000OptionsDialog.h - Released 2025-04-07T08:53:45Z
