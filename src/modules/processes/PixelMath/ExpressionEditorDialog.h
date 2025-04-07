//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// ExpressionEditorDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ExpressionEditorDialog_h
#define __ExpressionEditorDialog_h

#include <pcl/CodeEditor.h>
#include <pcl/Dialog.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/TabBox.h>
#include <pcl/TextBox.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PixelMathInstance;

class ExpressionEditorDialog : public Dialog
{
public:

   ExpressionEditorDialog();

   int Execute( PixelMathInstance& );

private:

   int m_splitterPressed = 0;
   int m_splitterPos = 0;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   Top_Sizer;
         Control           TopLeft_Control;
            VerticalSizer     TopLeft_Sizer;
               TabBox            Editors_TabBox;
                  Control           RK_Control;
                  HorizontalSizer   RK_Sizer;
                     CodeEditor        RK_CodeEditor;
                  Control           G_Control;
                  HorizontalSizer   G_Sizer;
                     CodeEditor        G_CodeEditor;
                  Control           B_Control;
                  HorizontalSizer   B_Sizer;
                     CodeEditor        B_CodeEditor;
                  Control           A_Control;
                  HorizontalSizer   A_Sizer;
                     CodeEditor        A_CodeEditor;
                  Control           Symbols_Control;
                  HorizontalSizer   Symbols_Sizer;
                     CodeEditor        Symbols_CodeEditor;
               Control           VertSplitter_Control;
               TextBox           Syntax_TextBox;
               TextBox           Parser_TextBox;
         Control           HorzSplitter_Control;
         Control           TopRight_Control;
            VerticalSizer     TopRight_Sizer;
               Label             Images_Label;
               TreeBox           Images_TreeBox;
               Label             Symbols_Label;
               TreeBox           Symbols_TreeBox;
               Label             Syntax_Label;
               TreeBox           Syntax_TreeBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Parse_PushButton;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void RebuildImagesTree();
   void RebuildSymbolsTree();
   void RebuildSyntaxTree();

   void ParseExpression();
   void ParseSymbols();

   CodeEditor* CurrentEditor();

   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_Click( Button& sender, bool checked );
   void e_KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey );
   void e_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExpressionEditorDialog_h

// ----------------------------------------------------------------------------
// EOF ExpressionEditorDialog.h - Released 2025-04-07T08:53:56Z
