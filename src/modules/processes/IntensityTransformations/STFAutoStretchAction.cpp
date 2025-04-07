//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// STFAutoStretchAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "STFAutoStretchAction.h"
#include "ScreenTransferFunctionInterface.h"

#include <pcl/ButtonCodes.h>
#include <pcl/ImageWindow.h>
#include <pcl/KeyCodes.h>
#include <pcl/MessageBox.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

STFAutoStretchActionBase::STFAutoStretchActionBase( bool boost, const String& menuItem, const char* icon )
   : Action( menuItem )
   , m_boost( boost )
{
   SetIcon( Control::Null().ScaledResource( icon ) );
}

// ----------------------------------------------------------------------------

void STFAutoStretchActionBase::Execute()
{
   if ( TheScreenTransferFunctionInterface != nullptr )
   {
      View view = ImageWindow::ActiveWindow().CurrentView();
      if ( view.IsNull() )
      {
         MessageBox( "Cannot apply STF AutoStretch because there is no view available.",
                     "ScreenTransferFunction",
                     StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }

      view.Window().BringToFront();
      view.Window().SelectView( view );

      if ( TheScreenTransferFunctionInterface->IsVisible() )
         TheScreenTransferFunctionInterface->ActivateTrackView();

      TheScreenTransferFunctionInterface->ComputeAutoStretch( view, m_boost );
      TheScreenTransferFunctionInterface->ApplyTo( view );
   }
}

// ----------------------------------------------------------------------------

bool STFAutoStretchActionBase::IsEnabled( ActionInfo info ) const
{
   return info.isImage; // every view can be screen stretched
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

STFAutoStretchAction::STFAutoStretchAction()
   : STFAutoStretchActionBase( false/*boosted*/,
                               "Image > STF AutoStretch",
                               ":/toolbar/image-stf-auto.png" )
{
   SetToolTip( "STF AutoStretch" );
   SetAccelerator( KeyModifier::Control, KeyCode::A );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

STFAutoStretchBoostAction::STFAutoStretchBoostAction()
   : STFAutoStretchActionBase( true/*boosted*/,
                               "Image > STF AutoStretch (boosted)",
                               ":/toolbar/image-stf-auto-boost.png" )
{
   SetToolTip( "STF AutoStretch (Boosted)" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

STFAutoStretchToolBarActionBase::STFAutoStretchToolBarActionBase( bool boost, const char* icon, const String& toolBar )
   : Action( String(), String(), toolBar )
   , m_boost( boost )
{
   SetIcon( Control::Null().ScaledResource( icon ) );
}

// ----------------------------------------------------------------------------

void STFAutoStretchToolBarActionBase::Execute()
{
   if ( TheScreenTransferFunctionInterface != nullptr )
   {
      bool withControlOrCmdPressed =
#ifdef __PCL_MACOSX
      IsCmdPressed();
#else
      IsControlPressed();
#endif
      bool withShiftPressed = !withControlOrCmdPressed && IsShiftPressed();

      View view = ImageWindow::ActiveWindow().CurrentView();
      if ( view.IsNull() )
      {
         MessageBox( "Cannot apply STF AutoStretch because there is no view available.",
                     "ScreenTransferFunction",
                     StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }

      view.Window().BringToFront();
      view.Window().SelectView( view );

      if ( TheScreenTransferFunctionInterface->IsVisible() )
         TheScreenTransferFunctionInterface->ActivateTrackView();

      if ( withControlOrCmdPressed )
         TheScreenTransferFunctionInterface->LinkRGBChannels( false );
      else if ( withShiftPressed )
         TheScreenTransferFunctionInterface->LinkRGBChannels( true );

      TheScreenTransferFunctionInterface->ComputeAutoStretch( view, m_boost );
      TheScreenTransferFunctionInterface->ApplyTo( view );
   }
}

// ----------------------------------------------------------------------------

bool STFAutoStretchToolBarActionBase::IsEnabled( ActionInfo info ) const
{
   return info.isImage; // every view can be screen stretched
}

// ----------------------------------------------------------------------------

String STFAutoStretchToolBarActionBase::AdditionalInformation()
{
   return "<ul><li>"
#ifdef __PCL_MACOSX
          "Cmd-Click"
#else
          "Ctrl-Click"
#endif
          " to apply a different STF to each RGB channel (<b>unlinked</b> mode).</li></ul>"
          "<ul><li>Shift-Click to apply a single STF to all RGB channels (<b>linked</b> mode).</li></ul>"
          "<ul><li>Click to apply current STF tool settings.</li></ul>";
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

STFAutoStretchToolBarAction::STFAutoStretchToolBarAction()
   : STFAutoStretchToolBarActionBase( false,
                                      ":/toolbar/image-stf-auto.png",
                                      "> Screen Transfer Functions" ) // > = add separator
{
   SetToolTip( "<p>STF AutoStretch</p>" + AdditionalInformation() );
}

// ----------------------------------------------------------------------------

STFAutoStretchToolBarBoostAction::STFAutoStretchToolBarBoostAction()
   : STFAutoStretchToolBarActionBase( true,
                                      ":/toolbar/image-stf-auto-boost.png",
                                      "Screen Transfer Functions" )
{
   SetToolTip( "<p>STF AutoStretch (Boosted)</p>" + AdditionalInformation() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF STFAutoStretchAction.cpp - Released 2025-04-07T08:53:56Z
