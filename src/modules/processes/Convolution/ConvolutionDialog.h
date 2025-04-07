//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// ConvolutionDialog.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ConvolutionDialog_h
#define __ConvolutionDialog_h

#include "ConvolutionInstance.h"
#include "FilterLibrary.h"

#include <pcl/KernelFilter.h>
#include <pcl/SeparableFilter.h>
#include <pcl/Label.h>
#include <pcl/Dialog.h>
#include <pcl/Array.h>
#include <pcl/String.h>
#include <pcl/TextBox.h>
#include <pcl/PushButton.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvolutionFilterCodeDialog : public Dialog
{
public:

   ConvolutionFilterCodeDialog( String mode, const Filter& f );

   const Filter& BeingEdited() const
   {
      return m_filter;
   }

private:

   Filter m_filter;

   VerticalSizer  Global_Sizer;
      TextBox           FilterCode_TextBox;
      HorizontalSizer   LineCol_Sizer;
         Label          LineCol_Label;
      HorizontalSizer   DialogButtons_Sizer;
         PushButton     Save_PushButton;
         PushButton     Cancel_PushButton;

   void __Button_Click( Button& sender, bool checked );
   void __TextBox_CaretPosition( TextBox& sender, int oldPos, int newPos );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ConvolutionDialog_h

// ----------------------------------------------------------------------------
// EOF ConvolutionDialog.h - Released 2025-04-07T08:53:55Z
