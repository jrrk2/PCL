//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// BlinkVideoDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BlinkVideoDialog_h
#define __BlinkVideoDialog_h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/ExternalProcess.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Rectangle.h>
#include <pcl/Sizer.h>
#include <pcl/TextBox.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BlinkInterface;

class BlinkVideoDialog : public Dialog
{
public:

   BlinkVideoDialog( BlinkInterface* parent );

private:

   VerticalSizer     Global_Sizer;
      TextBox           StdOut_TextBox;
      Edit              StdIn_Edit;
      HorizontalSizer   PreferencesLine1_Sizer;
         Label             Program_Label;
         Edit              Program_Edit;
      HorizontalSizer   PreferencesLine2_Sizer;
         Label             Arguments_Label;
         Edit              Arguments_Edit;
      HorizontalSizer   PreferencesLine3_Sizer;
         Label             OutputDir_Label;
         Edit              OutputDir_Edit;
         ToolButton        OutputDir_SelectButton;
      HorizontalSizer   PreferencesLine4_Sizer;
         Label             FrameExtension_Label;
         Edit              FrameExtension_Edit;
         CheckBox          DeleteVideoFrames_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Load_PushButton;
         PushButton        Save_PushButton;
         PushButton        WriteLog_PushButton;
         PushButton        Run_PushButton;
         PushButton        Cancel_PushButton;

   BlinkInterface*   m_parent = nullptr; // the parent interface
   ExternalProcess   m_videoEncoder;     // the external video encoder process
   int               m_frameCount = 0;   // file counter in the output sequence
   StringList        m_framesDone;       // already generated video frames in a given format ?
   String            m_command;          // command executed
   IsoString         m_timestamp;        // time of execution

   void  DisableGUI( bool disable = true );
   void  EnableGUI( bool enable = true ) { DisableGUI( !enable ); }
   void  Init();
   void  CreateFrames(); // create sequence with format: "dir/Blink00001.jpg"
   void  DeleteFrames();
   void  ExecuteVideoEncoder();
   bool  TerminateVideoEncoder();
   void  WriteLogFile();
   void  AddLog( const String& );

   void  __Process_Started( ExternalProcess& );
   void  __Process_Finished( ExternalProcess&, int exitCode, bool exitOk );
   void  __Process_StandardOutputDataAvailable( ExternalProcess& );
   void  __Process_Error( ExternalProcess&, ExternalProcess::error_code error );
   void  __Button_Click( Button& sender, bool checked );
   void  __Edit_Completed( Edit& sender );
   void  __Dialog_Close( Control& sender, bool& allowClose );
   void  __Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BlinkVideoDialog_h

// ----------------------------------------------------------------------------
// EOF BlinkVideoDialog.h - Released 2025-04-07T08:53:56Z
