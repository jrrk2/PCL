//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// CreateAlphaChannelsInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CreateAlphaChannelsInterface_h
#define __CreateAlphaChannelsInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "CreateAlphaChannelsInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CreateAlphaChannelsInterface : public ProcessInterface
{
public:

   CreateAlphaChannelsInterface();
   virtual ~CreateAlphaChannelsInterface();

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

   CreateAlphaChannelsInstance instance;

   struct GUIData
   {
      GUIData( CreateAlphaChannelsInterface& );

      VerticalSizer      Global_Sizer;
         GroupBox          FromImage_GroupBox;
         VerticalSizer     FromImage_Sizer;
            HorizontalSizer   ImageId_Sizer;
               Edit              ImageId_Edit;
               ToolButton        SelectSource_ToolButton;
            CheckBox          InvertSource_CheckBox;
            CheckBox          CloseSource_CheckBox;
         GroupBox          FromTransparency_GroupBox;
         HorizontalSizer   FromTransparency_Sizer;
            NumericControl    Transparency_NumericControl;
            Control           ColorSample_Control;
         CheckBox          Replace_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __SourceMode_Check( GroupBox& sender, bool checked );
   void __ImageId_GetFocus( Control& sender );
   void __ImageId_EditCompleted( Edit& sender );
   void __SelectSource_Click( Button& sender, bool checked );
   void __SourceOption_Click( Button& sender, bool checked );
   void __Transparency_ValueUpdated( NumericEdit& sender, double value );
   void __ColorSample_Paint( Control& sender, const Rect& updateRect );
   void __Replace_Click( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CreateAlphaChannelsInterface* TheCreateAlphaChannelsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CreateAlphaChannelsInterface_h

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsInterface.h - Released 2025-04-07T08:53:56Z
