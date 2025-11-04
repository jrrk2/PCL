//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Control.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/AutoLock.h>
#include <pcl/Control.h>
#include <pcl/View.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Control::Control( Control& parent, uint32 flags )
   : UIObject( API_Control_CreateControl( ModuleHandle(), this, parent.handle, flags ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateControl" );
}

// ----------------------------------------------------------------------------

Control& Control::Null()
{
   static Control* nullControl = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullControl == nullptr )
      nullControl = new Control( nullptr );
   return *nullControl;
}

// ----------------------------------------------------------------------------

pcl::Rect Control::FrameRect() const
{
   pcl::Rect r;
   API_Control_GetFrameRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

pcl::Rect Control::ClientRect() const
{
   pcl::Rect r;
   API_Control_GetClientRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void Control::SetClientRect( int x0, int y0, int x1, int y1 )
{
   API_Control_SetClientRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Control::Resize( int w, int h )
{
   API_Control_SetControlSize( handle, w, h );
}

// ----------------------------------------------------------------------------

void Control::AdjustToContents()
{
   API_Control_AdjustControlToContents( handle );
}

// ----------------------------------------------------------------------------

void Control::GetMinSize( int& w, int& h ) const
{
   API_Control_GetControlMinSize( handle, &w, &h );
}

// ----------------------------------------------------------------------------

void Control::SetMinSize( int w, int h )
{
   API_Control_SetControlMinSize( handle, w, h );
}

// ----------------------------------------------------------------------------

void Control::GetMaxSize( int& w, int& h ) const
{
   API_Control_GetControlMaxSize( handle, &w, &h );
}

// ----------------------------------------------------------------------------

void Control::SetMaxSize( int w, int h )
{
   API_Control_SetControlMaxSize( handle, w, h );
}

// ----------------------------------------------------------------------------

void Control::SetFixedSize( int w, int h )
{
   API_Control_SetControlFixedSize( handle, w, h );
}

// ----------------------------------------------------------------------------

bool Control::IsHorizontalExpansionEnabled() const
{
   api_bool h;
   API_Control_GetControlExpansionEnabled( handle, &h, 0 );
   return h != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsVerticalExpansionEnabled() const
{
   api_bool v;
   API_Control_GetControlExpansionEnabled( handle, 0, &v );
   return v != api_false;
}

// ----------------------------------------------------------------------------

void Control::EnableExpansion( bool horzEnable, bool vertEnable )
{
   API_Control_SetControlExpansionEnabled( handle, horzEnable, vertEnable );
}

// ----------------------------------------------------------------------------

pcl::Point Control::Position() const
{
   pcl::Point p;
   API_Control_GetControlPosition( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void Control::Move( int x, int y )
{
   API_Control_SetControlPosition( handle, x, y );
}

// ----------------------------------------------------------------------------

bool Control::IsUnderMouse() const
{
   return API_Control_GetControlUnderMouseStatus( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::BringToFront()
{
   API_Control_BringControlToFront( handle );
}

// ----------------------------------------------------------------------------

void Control::SendToBack()
{
   API_Control_SendControlToBack( handle );
}

// ----------------------------------------------------------------------------

void Control::StackUnder( Control& w )
{
   API_Control_StackControls( handle, w.handle );
}

// ----------------------------------------------------------------------------

pcl::Sizer Control::Sizer() const
{
   return pcl::Sizer( API_Control_GetControlSizer( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetSizer( pcl::Sizer& s )
{
   API_Control_SetControlSizer( handle, s.handle );
}

// ----------------------------------------------------------------------------

void Control::GlobalToLocal( int& x, int& y ) const
{
   API_Control_GlobalToLocal( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::LocalToGlobal( int& x, int& y ) const
{
   API_Control_LocalToGlobal( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::ParentToLocal( int& x, int& y ) const
{
   API_Control_ParentToLocal( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::LocalToParent( int& x, int& y ) const
{
   API_Control_LocalToParent( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::ControlToLocal( const Control& w, int& x, int& y ) const
{
   API_Control_ControlToLocal( handle, w.handle, &x, &y );
}

// ----------------------------------------------------------------------------

void Control::LocalToControl( const Control& w, int& x, int& y ) const
{
   API_Control_LocalToControl( handle, w.handle, &x, &y );
}

// ----------------------------------------------------------------------------

Control& Control::ChildByPos( int x, int y ) const
{
   Control* w = reinterpret_cast<Control*>( API_Control_GetChildByPos( handle, x, y ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

pcl::Rect Control::ChildrenRect() const
{
   pcl::Rect r;
   API_Control_GetChildrenRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

bool Control::IsAncestorOf( const Control& w ) const
{
   return API_Control_GetControlAncestry( handle, w.handle ) != api_false;
}

// ----------------------------------------------------------------------------

Control& Control::Parent() const
{
   Control* w = reinterpret_cast<Control*>( API_Control_GetControlParent( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

void Control::SetParent( Control& w )
{
   API_Control_SetControlParent( handle, w.handle );
}

// ----------------------------------------------------------------------------

Control& Control::Window() const
{
   Control* w = reinterpret_cast<Control*>( API_Control_GetControlWindow( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

bool Control::IsEnabled() const
{
   return API_Control_GetControlEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::Enable( bool enabled )
{
   API_Control_SetControlEnabled( handle, enabled );
}

// ----------------------------------------------------------------------------

bool Control::IsMouseTrackingEnabled() const
{
   return API_Control_GetControlMouseTrackingEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::EnableMouseTracking( bool enable )
{
   API_Control_SetControlMouseTrackingEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

bool Control::IsVisible() const
{
   return API_Control_GetControlVisible( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::Show()
{
   API_Control_SetControlVisible( handle, api_true );
}

// ----------------------------------------------------------------------------

pcl::Rect Control::VisibleRect() const
{
   pcl::Rect r;
   API_Control_GetControlVisibleRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void Control::Hide()
{
   API_Control_SetControlVisible( handle, api_false );
}

// ----------------------------------------------------------------------------

bool Control::IsMaximized() const
{
   api_bool m;
   (void)API_Control_GetWindowState( handle, 0, 0, &m, 0 );
   return m != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsMinimized() const
{
   api_bool m;
   (void)API_Control_GetWindowState( handle, 0, 0, 0, &m );
   return m != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsModal() const
{
   api_bool m;
   (void)API_Control_GetWindowState( handle, 0, &m, 0, 0 );
   return m != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsWindow() const
{
   return API_Control_GetWindowState( handle, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

bool Control::IsActiveWindow() const
{
   api_bool a;
   (void)API_Control_GetWindowState( handle, &a, 0, 0, 0 );
   return a != api_false;
}

// ----------------------------------------------------------------------------

void Control::ActivateWindow()
{
   API_Control_ActivateWindow( handle );
}

// ----------------------------------------------------------------------------

bool Control::IsFocused() const
{
   return API_Control_GetControlFocus( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::Focus( bool focus )
{
   API_Control_SetControlFocus( handle, focus );
}

// ----------------------------------------------------------------------------

FocusStyles Control::FocusStyle() const
{
   return FocusStyle::mask_type( API_Control_GetControlFocusStyle( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetFocusStyle( FocusStyles style )
{
   API_Control_SetControlFocusStyle( handle, style );
}

// ----------------------------------------------------------------------------

Control& Control::FocusedChild() const
{
   Control* w = reinterpret_cast<Control*>( API_Control_GetFocusChildControl( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

Control& Control::ChildToFocus() const
{
   Control* w = reinterpret_cast<Control*>( API_Control_GetChildControlToFocus( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

void Control::SetChildToFocus( Control& w )
{
   API_Control_SetChildControlToFocus( handle, w.handle );
}

// ----------------------------------------------------------------------------

Control& Control::NextSiblingToFocus() const
{
   Control* w = reinterpret_cast<Control*>( API_Control_GetNextSiblingControlToFocus( handle ) );
   return (w != nullptr) ? *w : Null();
}

// ----------------------------------------------------------------------------

void Control::SetNextSiblingToFocus( Control& w )
{
   API_Control_SetNextSiblingControlToFocus( handle, w.handle );
}

// ----------------------------------------------------------------------------

bool Control::CanUpdate() const
{
   return API_Control_GetControlUpdatesEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::EnableUpdates( bool enable )
{
   API_Control_SetControlUpdatesEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

void Control::Update()
{
   API_Control_UpdateControl( handle );
}

// ----------------------------------------------------------------------------

void Control::Update( int x0, int y0, int x1, int y1 )
{
   API_Control_UpdateControlRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Control::Repaint()
{
   API_Control_RepaintControl( handle );
}

// ----------------------------------------------------------------------------

void Control::Repaint( int x0, int y0, int x1, int y1 )
{
   API_Control_RepaintControlRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void Control::Restyle()
{
   API_Control_RestyleControl( handle );
}

// ----------------------------------------------------------------------------

void Control::EnsureLayoutUpdated()
{
   API_Control_EnsureControlLayoutUpdated( handle );
}

// ----------------------------------------------------------------------------

void Control::Scroll( int dx, int dy )
{
   API_Control_ScrollControl( handle, dx, dy );
}

// ----------------------------------------------------------------------------

void Control::Scroll( int dx, int dy, int x0, int y0, int x1, int y1 )
{
   API_Control_ScrollControlRect( handle, dx, dy, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

pcl::Cursor Control::Cursor() const
{
   return pcl::Cursor( API_Control_GetControlCursor( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetCursor( const pcl::Cursor& c )
{
   API_Control_SetControlCursor( handle, c.handle );
}
// ----------------------------------------------------------------------------

void Control::SetCursorToParent()
{
   API_Control_SetControlCursorToParent( handle );
}

// ----------------------------------------------------------------------------

String Control::StyleSheet() const
{
   size_type len = 0;
   API_Control_GetControlStyleSheet( handle, 0, &len );

   String css;
   if ( len > 0 )
   {
      css.SetLength( len );
      if ( API_Control_GetControlStyleSheet( handle, css.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetControlStyleSheet" );
      css.ResizeToNullTerminated();
   }
   return css;
}

// ----------------------------------------------------------------------------

void Control::SetStyleSheet( const String& css )
{
   API_Control_SetControlStyleSheet( handle, css.c_str() );
}

// ----------------------------------------------------------------------------

RGBA Control::BackgroundColor() const
{
   return API_Control_GetControlBackgroundColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetBackgroundColor( RGBA color )
{
   API_Control_SetControlBackgroundColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::ForegroundColor() const
{
   return API_Control_GetControlForegroundColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetForegroundColor( RGBA color )
{
   API_Control_SetControlForegroundColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::CanvasColor()
{
   return API_Control_GetControlCanvasColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetCanvasColor( RGBA color )
{
   API_Control_SetControlCanvasColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::AlternateCanvasColor() const
{
   return API_Control_GetControlAlternateCanvasColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetAlternateCanvasColor( RGBA color )
{
   API_Control_SetControlAlternateCanvasColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::TextColor() const
{
   return API_Control_GetControlTextColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetTextColor( RGBA color )
{
   API_Control_SetControlTextColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::ButtonColor() const
{
   return API_Control_GetControlButtonColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetButtonColor( RGBA color )
{
   API_Control_SetControlButtonColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::ButtonTextColor() const
{
   return API_Control_GetControlTextColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetButtonTextColor( RGBA color )
{
   API_Control_SetControlTextColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::HighlightColor() const
{
   return API_Control_GetControlHighlightColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetHighlightColor( RGBA color )
{
   API_Control_SetControlHighlightColor( handle, color );
}

// ----------------------------------------------------------------------------

RGBA Control::HighlightedTextColor() const
{
   return API_Control_GetControlHighlightedTextColor( handle );
}

// ----------------------------------------------------------------------------

void Control::SetHighlightedTextColor( RGBA color )
{
   API_Control_SetControlHighlightedTextColor( handle, color );
}

// ----------------------------------------------------------------------------

pcl::Font Control::Font() const
{
   return pcl::Font( API_Control_GetControlFont( handle ) );
}

// ----------------------------------------------------------------------------

void Control::SetFont( const pcl::Font& f )
{
   API_Control_SetControlFont( handle, f.handle );
}

// ----------------------------------------------------------------------------

double Control::WindowOpacity() const
{
   double op = 1.0;
   API_Control_GetWindowOpacity( handle, &op );
   return op;
}

// ----------------------------------------------------------------------------

void Control::SetWindowOpacity( double op )
{
   API_Control_SetWindowOpacity( handle, op );
}

// ----------------------------------------------------------------------------

String Control::WindowTitle() const
{
   size_type len = 0;
   API_Control_GetWindowTitle( handle, 0, &len );

   String title;
   if ( len > 0 )
   {
      title.SetLength( len );
      if ( API_Control_GetWindowTitle( handle, title.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetWindowTitle" );
      title.ResizeToNullTerminated();
   }
   return title;
}

// ----------------------------------------------------------------------------

void Control::SetWindowTitle( const String& title )
{
   API_Control_SetWindowTitle( handle, title.c_str() );
}

// ----------------------------------------------------------------------------

String Control::InfoText() const
{
   size_type len = 0;
   API_Control_GetInfoText( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_Control_GetInfoText( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetInfoText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void Control::SetInfoText( const String& text )
{
   API_Control_SetInfoText( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool Control::IsTrackViewActive() const
{
   return API_Control_GetTrackViewActive( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::SetTrackViewActive( bool active )
{
   API_Control_SetTrackViewActive( handle, api_bool( active ) );
}

// ----------------------------------------------------------------------------

bool Control::IsRealTimePreviewActive() const
{
   return API_Control_GetRealTimePreviewActive( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Control::SetRealTimePreviewActive( bool active )
{
   API_Control_SetRealTimePreviewActive( handle, api_bool( active ) );
}

// ----------------------------------------------------------------------------

String Control::ToolTip() const
{
   size_type len = 0;
   API_Control_GetWindowToolTip( handle, 0, &len );

   String tip;
   if ( len > 0 )
   {
      tip.SetLength( len );
      if ( API_Control_GetWindowToolTip( handle, tip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetWindowToolTip" );
      tip.ResizeToNullTerminated();
   }
   return tip;
}

// ----------------------------------------------------------------------------

void Control::SetToolTip( const String& tip )
{
   API_Control_SetWindowToolTip( handle, tip.c_str() );
}

// ----------------------------------------------------------------------------

void Control::ShowToolTip( int x, int y, const String& text, const Control& control, const Rect& rect )
{
   API_Global_ShowToolTipWindow( x, y, text.c_str(), control.handle, rect.x0, rect.y0, rect.x1, rect.y1 );
}

// ----------------------------------------------------------------------------

void Control::HideToolTip()
{
   API_Global_HideToolTipWindow();
}

// ----------------------------------------------------------------------------

String Control::ToolTipText()
{
   size_type len = 0;
   API_Global_GetToolTipWindowText( 0, &len );

   String tip;
   if ( len > 0 )
   {
      tip.SetLength( len );
      if ( API_Global_GetToolTipWindowText( tip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetToolTipWindowText" );
      tip.ResizeToNullTerminated();
   }
   return tip;
}

// ----------------------------------------------------------------------------

double Control::DisplayPixelRatio() const
{
   double r;
   if ( API_Control_GetControlDisplayPixelRatio( handle, &r ) == api_false )
      throw APIFunctionError( "GetControlDisplayPixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

double Control::ResourcePixelRatio() const
{
   double r;
   if ( API_Control_GetControlResourcePixelRatio( handle, &r ) == api_false )
      throw APIFunctionError( "GetControlResourcePixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

void* Control::CloneHandle() const
{
   throw Error( "Cannot clone a Control handle" );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Control*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ControlEventDispatcher
{
public:

   static api_bool api_func Destroy( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onDestroy != nullptr )
      {
         (receiver->*handlers->onDestroy)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Show( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onShow != nullptr )
      {
         (receiver->*handlers->onShow)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Hide( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onHide != nullptr )
      {
         (receiver->*handlers->onHide)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Close( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onClose != nullptr )
      {
         bool allowClose = true;
         (receiver->*handlers->onClose)( *sender, allowClose );
         return api_bool( allowClose );
      }
      return api_true;
   }

   static api_bool api_func GetFocus( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onGetFocus != nullptr )
      {
         (receiver->*handlers->onGetFocus)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func LoseFocus( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onLoseFocus != nullptr )
      {
         (receiver->*handlers->onLoseFocus)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Enter( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onEnter != nullptr )
      {
         (receiver->*handlers->onEnter)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Leave( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onLeave != nullptr )
      {
         (receiver->*handlers->onLeave)( *sender );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Move( control_handle hSender, control_handle hReceiver,
                                    int32 x, int32 y, int32 oldX, int32 oldY )
   {
      if ( handlers->onMove != nullptr )
      {
         pcl::Point pos( x, y );
         pcl::Point oldPos( oldX, oldY );
         (receiver->*handlers->onMove)( *sender, pos, oldPos );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Resize( control_handle hSender, control_handle hReceiver,
                                    int32 w, int32 h, int32 oldW, int32 oldH )
   {
      if ( handlers->onResize != nullptr )
      {
         (receiver->*handlers->onResize)( *sender, w, h, oldW, oldH );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func Paint( control_handle hSender, control_handle hReceiver,
                                    int32 x0, int32 y0, int32 x1, int32 y1 )
   {
      if ( handlers->onPaint != nullptr )
      {
         pcl::Rect r( x0, y0, x1, y1 );
         (receiver->*handlers->onPaint)( *sender, r );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func KeyPress( control_handle hSender, control_handle hReceiver,
                                    api_key_code key, api_key_modifiers modifiers )
   {
      if ( handlers->onKeyPress != nullptr )
      {
         bool wantsKey = false;
         (receiver->*handlers->onKeyPress)( *sender, key, modifiers, wantsKey );
         return wantsKey;
      }
      return api_false;
   }

   static api_bool api_func KeyRelease( control_handle hSender, control_handle hReceiver,
                                    api_key_code key, api_key_modifiers modifiers )
   {
      if ( handlers->onKeyRelease != nullptr )
      {
         bool wantsKey = false;
         (receiver->*handlers->onKeyRelease)( *sender, key, modifiers, wantsKey );
         return wantsKey;
      }
      return api_false;
   }

   static api_bool api_func MouseMove( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseMove != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseMove)( *sender, pos, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MouseDoubleClick( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseDoubleClick != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseDoubleClick)( *sender, pos, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MousePress( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_button button,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMousePress != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMousePress)( *sender, pos, button, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MouseRelease( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, api_mouse_button button,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseRelease != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseRelease)( *sender, pos, button, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func MouseWheel( control_handle hSender, control_handle hReceiver,
      int32 x, int32 y, int32 delta,
      api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      if ( handlers->onMouseWheel != nullptr )
      {
         pcl::Point pos( x, y );
         (receiver->*handlers->onMouseWheel)( *sender, pos, delta, buttons, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func ChildCreate( control_handle hSender, control_handle hReceiver, control_handle hChild )
   {
      if ( handlers->onChildCreate != nullptr )
      {
         (receiver->*handlers->onChildCreate)( *sender, *reinterpret_cast<Control*>( hChild ) );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func ChildDestroy( control_handle hSender, control_handle hReceiver, control_handle hChild )
   {
      if ( handlers->onChildDestroy != nullptr )
      {
         (receiver->*handlers->onChildDestroy)( *sender, *reinterpret_cast<Control*>( hChild ) );
         return api_true;
      }
      return api_false;
   }

   static StringList MakeFileList( const char16_type* files, const size_type* lengths, size_type count )
   {
      StringList fileList;
      for ( String::const_iterator i = files; count > 0; --count, ++lengths )
      {
         String::const_iterator j = i + *lengths;
         fileList << String( i, j );
         i = j;
      }
      return fileList;
   }

   static api_bool api_func FileDrag( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const char16_type* files, const size_type* lengths, size_type count,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onFileDrag != nullptr )
      {
         StringList fileList = MakeFileList( files, lengths, count );
         bool wantsFiles = false;
         (receiver->*handlers->onFileDrag)( *sender, pcl::Point( x, y ), fileList, modifiers, wantsFiles );
         return wantsFiles;
      }
      return api_false;
   }

   static api_bool api_func FileDrop( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const char16_type* files, const size_type* lengths, size_type count,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onFileDrop != nullptr )
      {
         StringList fileList = MakeFileList( files, lengths, count );
         (receiver->*handlers->onFileDrop)( *sender, pcl::Point( x, y ), fileList, modifiers );
         return api_true;
      }
      return api_false;
   }

   static api_bool api_func ViewDrag( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const_view_handle hView,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onViewDrag != nullptr )
      {
         View view( hView );
         bool wantsView = false;
         (receiver->*handlers->onViewDrag)( *sender, pcl::Point( x, y ), view, modifiers, wantsView );
         return wantsView;
      }
      return api_false;
   }

   static api_bool api_func ViewDrop( control_handle hSender, control_handle hReceiver,
                                      int32 x, int32 y,
                                      const_view_handle hView,
                                      api_key_modifiers modifiers )
   {
      if ( handlers->onViewDrop != nullptr )
      {
         View view( hView );
         (receiver->*handlers->onViewDrop)( *sender, pcl::Point( x, y ), view, modifiers );
         return api_true;
      }
      return api_false;
   }
}; // ControlEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new Control::EventHandlers

void Control::OnDestroy( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetDestroyEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Destroy : nullptr ) == api_false )
      throw APIFunctionError( "SetDestroyEventRoutine" );
   m_handlers->onDestroy = f;
}

void Control::OnShow( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetShowEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Show : nullptr ) == api_false )
      throw APIFunctionError( "SetShowEventRoutine" );
   m_handlers->onShow = f;
}

void Control::OnHide( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetHideEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Hide : nullptr ) == api_false )
      throw APIFunctionError( "SetHideEventRoutine" );
   m_handlers->onHide = f;
}

void Control::OnClose( close_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetCloseEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Close : nullptr ) == api_false )
      throw APIFunctionError( "SetCloseEventRoutine" );
   m_handlers->onClose = f;
}

void Control::OnGetFocus( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetGetFocusEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::GetFocus : nullptr ) == api_false )
      throw APIFunctionError( "SetGetFocusEventRoutine" );
   m_handlers->onGetFocus = f;
}

void Control::OnLoseFocus( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetLoseFocusEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::LoseFocus : nullptr ) == api_false )
      throw APIFunctionError( "SetLoseFocusEventRoutine" );
   m_handlers->onLoseFocus = f;
}

void Control::OnEnter( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetEnterEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Enter : nullptr ) == api_false )
      throw APIFunctionError( "SetEnterEventRoutine" );
   m_handlers->onEnter = f;
}

void Control::OnLeave( event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetLeaveEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Leave : nullptr ) == api_false )
      throw APIFunctionError( "SetLeaveEventRoutine" );
   m_handlers->onLeave = f;
}

void Control::OnMove( move_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetMoveEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Move : nullptr ) == api_false )
      throw APIFunctionError( "SetMoveEventRoutine" );
   m_handlers->onMove = f;
}

void Control::OnResize( resize_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetResizeEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Resize : nullptr ) == api_false )
      throw APIFunctionError( "SetResizeEventRoutine" );
   m_handlers->onResize = f;
}

void Control::OnPaint( paint_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetPaintEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::Paint : nullptr ) == api_false )
      throw APIFunctionError( "SetPaintEventRoutine" );
   m_handlers->onPaint = f;
}

void Control::OnKeyPress( keyboard_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetKeyPressEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::KeyPress : nullptr ) == api_false )
      throw APIFunctionError( "SetKeyPressEventRoutine" );
   m_handlers->onKeyPress = f;
}

void Control::OnKeyRelease( keyboard_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetKeyReleaseEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::KeyRelease : nullptr ) == api_false )
      throw APIFunctionError( "SetKeyReleaseEventRoutine" );
   m_handlers->onKeyRelease = f;
}

void Control::OnMouseMove( mouse_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetMouseMoveEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseMove : nullptr ) == api_false )
      throw APIFunctionError( "SetMouseMoveEventRoutine" );
   m_handlers->onMouseMove = f;
}

void Control::OnMouseDoubleClick( mouse_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetMouseDoubleClickEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseDoubleClick : nullptr ) == api_false )
      throw APIFunctionError( "SetMouseDoubleClickEventRoutine" );
   m_handlers->onMouseDoubleClick = f;
}

void Control::OnMousePress( mouse_button_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetMousePressEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MousePress : nullptr ) == api_false )
      throw APIFunctionError( "SetMousePressEventRoutine" );
   m_handlers->onMousePress = f;
}

void Control::OnMouseRelease( mouse_button_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetMouseReleaseEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseRelease : nullptr ) == api_false )
      throw APIFunctionError( "SetMouseReleaseEventRoutine" );
   m_handlers->onMouseRelease = f;
}

void Control::OnMouseWheel( mouse_wheel_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetWheelEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::MouseWheel : nullptr ) == api_false )
      throw APIFunctionError( "SetWheelEventRoutine" );
   m_handlers->onMouseWheel = f;
}

void Control::OnChildCreate( child_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetChildCreateEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ChildCreate : nullptr ) == api_false )
      throw APIFunctionError( "SetChildCreateEventRoutine" );
   m_handlers->onChildCreate = f;
}

void Control::OnChildDestroy( child_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetChildDestroyEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ChildDestroy : nullptr ) == api_false )
      throw APIFunctionError( "SetChildDestroyEventRoutine" );
   m_handlers->onChildDestroy = f;
}

void Control::OnFileDrag( file_drag_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetFileDragEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::FileDrag : nullptr ) == api_false )
      throw APIFunctionError( "SetFileDragEventRoutine" );
   m_handlers->onFileDrag = f;
}

void Control::OnFileDrop( file_drop_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetFileDropEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::FileDrop : nullptr ) == api_false )
      throw APIFunctionError( "SetFileDropEventRoutine" );
   m_handlers->onFileDrop = f;
}

void Control::OnViewDrag( view_drag_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetViewDragEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ViewDrag : nullptr ) == api_false )
      throw APIFunctionError( "SetViewDragEventRoutine" );
   m_handlers->onViewDrag = f;
}

void Control::OnViewDrop( view_drop_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Control_SetViewDropEventRoutine( handle, &receiver,
                  (f != nullptr) ? ControlEventDispatcher::ViewDrop : nullptr ) == api_false )
      throw APIFunctionError( "SetViewDropEventRoutine" );
   m_handlers->onViewDrop = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Control.cpp - Released 2025-04-07T08:53:32Z
