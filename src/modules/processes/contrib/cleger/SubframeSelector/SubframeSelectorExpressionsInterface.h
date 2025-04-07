//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorExpressionsInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2025 Cameron Leger
// Copyright (c) 2020-2025 Juan Conejero, PTeam
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef e_SubframeSelectorExpressionsInterface_h
#define e_SubframeSelectorExpressionsInterface_h

#include <pcl/BitmapBox.h>
#include <pcl/CodeEditor.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorInstance;

class SubframeSelectorExpressionsInterface : public ProcessInterface
{
public:

   SubframeSelectorExpressionsInterface( SubframeSelectorInstance& );
   virtual ~SubframeSelectorExpressionsInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   using ProcessInterface::Launch;

   void ApplyApprovalExpression();
   void ApplyWeightingExpression();

private:

   SubframeSelectorInstance& m_instance;
   Bitmap* m_okBitmap = nullptr;
   Bitmap* m_errorBitmap = nullptr;

   struct GUIData
   {
      GUIData( SubframeSelectorExpressionsInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   ExpressionParameters_Approval_Sizer;
         VerticalSizer     ExpressionParameters_Approval_Label_Sizer1;
            HorizontalSizer   ExpressionParameters_Approval_Label_Sizer2;
               BitmapBox         ExpressionParameters_Approval_Status;
               Label             ExpressionParameters_Approval_Label;
         CodeEditor        ExpressionParameters_Approval_Editor;
         ToolButton        ExpressionParameters_Approval_ToolButton;
      HorizontalSizer   ExpressionParameters_Weighting_Sizer;
         VerticalSizer     ExpressionParameters_Weighting_Label_Sizer1;
            HorizontalSizer   ExpressionParameters_Weighting_Label_Sizer2;
               BitmapBox         ExpressionParameters_Weighting_Status;
               Label             ExpressionParameters_Weighting_Label;
         CodeEditor        ExpressionParameters_Weighting_Editor;
         ToolButton        ExpressionParameters_Weighting_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_TextUpdated( CodeEditor& sender );
   void e_KeyPressed( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void e_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
   friend class SubframeSelectorInterface;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorExpressionsInterface* TheSubframeSelectorExpressionsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // e_SubframeSelectorExpressionsInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorExpressionsInterface.h - Released 2025-04-07T08:53:56Z
