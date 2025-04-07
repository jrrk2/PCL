//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Action.cpp - Released 2025-04-07T08:53:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pcl/Action.h>
#include <pcl/AutoLock.h>
#include <pcl/ErrorHandler.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void MandatoryError( const char* funcName )
{
   throw Error( String( "Action::" ) + funcName + "() must be reimplemented in descendant class." );
}

// ----------------------------------------------------------------------------

class ActionDispatcher
{
public:

   static void api_func Execute( action_handle hSender )
   {
      try
      {
         reinterpret_cast<Action*>( hSender )->Execute();
      }
      ERROR_HANDLER
   }

   static api_bool api_func QueryState( const_action_handle hSender, const void* info )
   {
      try
      {
         return api_bool( reinterpret_cast<const Action*>( hSender )->IsEnabled( *reinterpret_cast<const ActionInfo*>( info ) ) );
      }
      ERROR_HANDLER
      return api_false;
   }
};

// ----------------------------------------------------------------------------

Action::Action( const String& menuItem, const String& iconSVGFile, const String& toolBar )
   : UIObject( (*API->Action->CreateActionSVGFile)( ModuleHandle(), this, menuItem.c_str(), toolBar.c_str(), iconSVGFile.c_str(), 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateActionSVGFile" );

   (*API->Action->SetActionExecutionRoutine)( handle, ActionDispatcher::Execute );
   (*API->Action->SetActionStateQueryRoutine)( handle, ActionDispatcher::QueryState );
}

// ----------------------------------------------------------------------------

Action::Action( const String& menuItem, const IsoString& iconSVGSource, int, const String& toolBar )
   : UIObject( (*API->Action->CreateActionSVG)( ModuleHandle(), this, menuItem.c_str(), toolBar.c_str(), iconSVGSource.c_str(), 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateActionSVG" );

   (*API->Action->SetActionExecutionRoutine)( handle, ActionDispatcher::Execute );
   (*API->Action->SetActionStateQueryRoutine)( handle, ActionDispatcher::QueryState );
}

// ----------------------------------------------------------------------------

#ifdef __PCL_ACTION_DEPRECATED_CTOR
// ### DEPRECATED
Action::Action( const String& menuItem, const Bitmap& icon, const String& toolBar )
   : UIObject( (*API->Action->CreateAction)( ModuleHandle(), this, menuItem.c_str(), toolBar.c_str(), icon.handle, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateAction" );

   (*API->Action->SetActionExecutionRoutine)( handle, ActionDispatcher::Execute );
   (*API->Action->SetActionStateQueryRoutine)( handle, ActionDispatcher::QueryState );
}
#endif

// ----------------------------------------------------------------------------

Action& Action::Null()
{
   static Action* nullAction = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullAction == nullptr )
      nullAction = new Action( reinterpret_cast<void*>( 0 ) );
   return *nullAction;
}

// ----------------------------------------------------------------------------

String Action::MenuItem() const
{
   size_type len = 0;
   (*API->Action->GetActionMenuItem)( handle, 0, &len );

   String menuItem;
   if ( len > 0 )
   {
      menuItem.SetLength( len );
      if ( (*API->Action->GetActionMenuItem)( handle, menuItem.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionMenuItem" );
      menuItem.ResizeToNullTerminated();
   }
   return menuItem;
}

// ----------------------------------------------------------------------------

String Action::MenuText() const
{
   size_type len = 0;
   (*API->Action->GetActionMenuText)( handle, 0, &len );

   String menuText;
   if ( len > 0 )
   {
      menuText.SetLength( len );
      if ( (*API->Action->GetActionMenuText)( handle, menuText.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionMenuText" );
      menuText.ResizeToNullTerminated();
   }
   return menuText;
}

// ----------------------------------------------------------------------------

void Action::SetMenuText( const String& item )
{
   (*API->Action->SetActionMenuText)( handle, item.c_str() );
}

// ----------------------------------------------------------------------------

String Action::ToolBar() const
{
   size_type len = 0;
   (*API->Action->GetActionToolBar)( handle, 0, &len );

   String toolBarName;
   if ( len > 0 )
   {
      toolBarName.SetLength( len );
      if ( (*API->Action->GetActionToolBar)( handle, toolBarName.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionToolBar" );
      toolBarName.ResizeToNullTerminated();
   }
   return toolBarName;
}

// ----------------------------------------------------------------------------

void Action::GetAccelerator( int& keyModifiers, int& keyCode ) const
{
   (*API->Action->GetActionAccelerator)( handle, &keyModifiers, &keyCode );
}

// ----------------------------------------------------------------------------

void Action::SetAccelerator( int keyModifiers, int keyCode )
{
   (*API->Action->SetActionAccelerator)( handle, keyModifiers, keyCode );
}

// ----------------------------------------------------------------------------

String Action::ToolTip() const
{
   size_type len = 0;
   (*API->Action->GetActionToolTip)( handle, 0, &len );

   String toolTip;
   if ( len > 0 )
   {
      toolTip.SetLength( len );
      if ( (*API->Action->GetActionToolTip)( handle, toolTip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetActionToolTip" );
      toolTip.ResizeToNullTerminated();
   }
   return toolTip;
}

// ----------------------------------------------------------------------------

void Action::SetToolTip( const String& tip )
{
   (*API->Action->SetActionToolTip)( handle, tip.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap Action::Icon() const
{
   return Bitmap( (*API->Action->GetActionIcon)( handle ) );
}

// ----------------------------------------------------------------------------

void Action::SetIconSVGFile( const String& filePath )
{
   (*API->Action->SetActionIconSVGFile)( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

void Action::SetIconSVG( const IsoString& svgSource )
{
   (*API->Action->SetActionIconSVG)( handle, svgSource.c_str() );
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
void Action::SetIcon( const Bitmap& bmp )
{
   (*API->Action->SetActionIcon)( handle, bmp.handle );
}

// ----------------------------------------------------------------------------

void Action::Execute()
{
   MandatoryError( "Execute" );
}

// ----------------------------------------------------------------------------

void* Action::CloneHandle() const
{
   throw Error( "Cannot clone an Action handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Action.cpp - Released 2025-04-07T08:53:32Z
