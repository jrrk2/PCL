//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ProcessInterface.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>
#include <pcl/KeyCodes.h> // for IsControlPressed()/IsCmdPressed()
#include <pcl/MetaModule.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/ProcessInstance.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Settings.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ProcessInterface::ProcessInterface()
   : Control( nullptr )
   , MetaObject( Module )
{
   if ( Module == nullptr )
      throw Error( "ProcessInterface: Module not initialized - illegal ProcessInterface instantiation" );
}

// ----------------------------------------------------------------------------

// Private ctor. to create a null object
ProcessInterface::ProcessInterface( int )
   : Control( nullptr )
   , MetaObject( nullptr )
{
}

class NullInterface : public ProcessInterface
{
public:

   NullInterface()
      : ProcessInterface( -1 )
   {
   }

   IsoString Id() const override
   {
      return IsoString();
   }

   MetaProcess* Process() const override
   {
      return nullptr;
   }
};

ProcessInterface& ProcessInterface::Null()
{
   static NullInterface* nullInterface = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullInterface == nullptr )
      nullInterface = new NullInterface();
   return *nullInterface;
}

// ----------------------------------------------------------------------------

void ProcessInterface::ApplyInstance() const
{
   ProcessImplementation* p = NewProcess();
   if ( p != nullptr )
      p->LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void ProcessInterface::ApplyInstanceGlobal() const
{
   ProcessImplementation* p = NewProcess();
   if ( p != nullptr )
      p->LaunchGlobal();
}

// ----------------------------------------------------------------------------

void ProcessInterface::Cancel()
{
   /*
    * Undocumented feature: Do not close the dynamic interface if the Control
    * key (the Command key on macOS) is being pressed.
    */
   ImageWindow::TerminateDynamicSession( !IsControlOrCmdPressed() );
}

// ----------------------------------------------------------------------------

bool ProcessInterface::BrowseDocumentation() const
{
   const MetaProcess* P = Process();
   if ( P != nullptr && P->CanBrowseDocumentation() )
      return P->BrowseDocumentation();
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessInterface::Launch( unsigned flags )
{
   return API_Global_LaunchProcessInterface( reinterpret_cast<interface_handle>( this ), flags ) != api_false;
}

// ----------------------------------------------------------------------------

void ProcessInterface::BroadcastImageUpdated( const View& v )
{
   API_Global_BroadcastImageUpdated( v.handle, nullptr /*reserved*/ );
}

// ----------------------------------------------------------------------------

void ProcessInterface::ProcessEvents( bool excludeUserInputEvents )
{
   if ( Module != nullptr )
      Module->ProcessEvents( excludeUserInputEvents );
}

// ----------------------------------------------------------------------------

void ProcessInterface::SaveGeometry() const
{
   IsoString key = SettingsKey() + "Geometry/";

   Point p = Position();
   Rect cr = ClientRect();

   Settings::Write( key + "Left", p.x );
   Settings::Write( key + "Top", p.y );
   Settings::Write( key + "Width", cr.Width() );
   Settings::Write( key + "Height", cr.Height() );
}

bool ProcessInterface::RestoreGeometry()
{
   IsoString key = SettingsKey() + "Geometry/";

   Point p0 = Position(), p = p0;

   Rect cr = ClientRect();
   int width = cr.Width();
   int height = cr.Height();

   bool ok = Settings::Read( key + "Left", p.x ) &&
             Settings::Read( key + "Top", p.y ) &&
             Settings::Read( key + "Width", width ) &&
             Settings::Read( key + "Height", height );
   if ( ok )
   {
      bool changeWidth = width != cr.Width() && !IsFixedWidth();
      bool changeHeight = height != cr.Height() && !IsFixedHeight();

      if ( changeWidth || changeHeight )
         Resize( changeWidth ? width : cr.Width(), changeHeight ? height : cr.Height() );

      if ( p != p0 )
         Move( p );
   }

   return ok;
}

void ProcessInterface::SetDefaultPosition()
{
   int x = PixInsightSettings::GlobalInteger( "Workspace/PrimaryScreenCenterX" );
   int y = PixInsightSettings::GlobalInteger( "Workspace/PrimaryScreenCenterY" );
   Rect cr = ClientRect();
   Move( x - (cr.Width() >> 1), y - (cr.Height() >> 1) );
}

// ----------------------------------------------------------------------------
// Process Interface Context
// ----------------------------------------------------------------------------

class InterfaceDispatcher
{
public:

   static void api_func Initialize( interface_handle hi, control_handle hw )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->handle = hw;
         reinterpret_cast<ProcessInterface*>( hi )->Initialize();
      }
      ERROR_HANDLER
   }

   static api_bool api_func Launch( interface_handle hi, meta_process_handle hP,
                                    const_process_handle hp, api_bool* dynamic, uint32* flags )
   {
#define iface  reinterpret_cast<ProcessInterface*>( hi )
      try
      {
         bool myDynamic;
         unsigned myFlags;

         const MetaProcess* P;
         if ( hP != 0 )
            P = reinterpret_cast<const MetaProcess*>( hP );
         else
            P = iface->Process();
         if ( P == nullptr )
            return api_false;

         if ( iface->Launch( *P, reinterpret_cast<const ProcessImplementation*>( hp ), myDynamic, myFlags ) )
         {
            *dynamic = api_bool( myDynamic );
            *flags = uint32( myFlags );

            if ( iface->m_launchCount++ == 0 )
            {
               iface->LoadSettings();

               if ( iface->IsAutoSaveGeometryEnabled() )
                  if ( !iface->RestoreGeometry() )
                     iface->SetDefaultPosition();
            }

            return api_true;
         }

         return api_false;
      }
      ERROR_HANDLER
      return api_false;
#undef iface
   }

   static process_handle api_func NewProcess( const_interface_handle hi, meta_process_handle* hmp )
   {
      try
      {
         ProcessImplementation* p = reinterpret_cast<const ProcessInterface*>( hi )->NewProcess();
         *hmp = (p != nullptr) ? p->Meta() : 0;
         return p;
      }
      ERROR_HANDLER
      return 0;
   }

   static process_handle api_func NewTestProcess( const_interface_handle hi, meta_process_handle* hmp )
   {
      try
      {
         ProcessImplementation* p = reinterpret_cast<const ProcessInterface*>( hi )->NewTestProcess();
         *hmp = (p != nullptr) ? p->Meta() : 0;
         return p;
      }
      ERROR_HANDLER
      return 0;
   }

   static api_bool api_func ValidateProcess( const_interface_handle hi, const_process_handle hp,
                                             char16_type* whyNot, uint32 maxLen )
   {
      try
      {
         String whyNotStr;
         bool ok = reinterpret_cast<const ProcessInterface*>( hi )->ValidateProcess(
                                       *reinterpret_cast<const ProcessImplementation*>( hp ), whyNotStr );
         if ( !ok )
            if ( !whyNotStr.IsEmpty() )
               if ( whyNot != nullptr )
                  if ( maxLen > 0 )
                     whyNotStr.c_copy( whyNot, maxLen );

         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func ImportProcess( interface_handle hi, const_process_handle hp )
   {
      try
      {
         return reinterpret_cast<ProcessInterface*>( hi )->ImportProcess(
                                       *reinterpret_cast<const ProcessImplementation*>( hp ) );
      }
      ERROR_HANDLER
      return api_false;
   }

   static void api_func ApplyInstance( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->ApplyInstance();
      }
      ERROR_HANDLER
   }

   static void api_func ApplyInstanceGlobal( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->ApplyInstanceGlobal();
      }
      ERROR_HANDLER
   }

   static void api_func Execute( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->Execute();
      }
      ERROR_HANDLER
   }

   static void api_func Cancel( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->Cancel();
      }
      ERROR_HANDLER
   }

   static void api_func BrowseDocumentation( interface_handle hi )
   {
      try
      {
         (void)reinterpret_cast<ProcessInterface*>( hi )->BrowseDocumentation();
      }
      ERROR_HANDLER
   }

   static void api_func RealTimePreviewUpdated( interface_handle hi, api_bool active )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->RealTimePreviewUpdated( active != api_false );
      }
      ERROR_HANDLER
   }

   static void api_func TrackViewUpdated( interface_handle hi, api_bool active )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->TrackViewUpdated( active != api_false );
      }
      ERROR_HANDLER
   }

   static void api_func EditPreferences( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->EditPreferences();
      }
      ERROR_HANDLER
   }

   static void api_func ResetInstance( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->ResetInstance();
      }
      ERROR_HANDLER
   }

   static api_bool api_func RequiresRealTimePreviewUpdate( const_interface_handle hi,
                                                           const_image_handle himg, const_view_handle hv,
                                                           int32 x0, int32 y0, int32 x1, int32 y1, int32 z )
   {
      try
      {
         UInt16Image image( const_cast<image_handle>( himg ) );
         View view( hv );
         return reinterpret_cast<const ProcessInterface*>( hi )->RequiresRealTimePreviewUpdate( image, view,
                                                                              Rect( x0, y0, x1, y1 ), z );
      }
      ERROR_HANDLER
      return api_false;
   }

   static uint32 api_func RealTimePreviewGenerationFlags( const_interface_handle hi,
                                                          const_image_handle himg, const_view_handle hv,
                                                          int32 x0, int32 y0, int32 x1, int32 y1, int32 z )
   {
      try
      {
         UInt16Image image( const_cast<image_handle>( himg ) );
         View view( hv );
         return unsigned( reinterpret_cast<const ProcessInterface*>( hi )->RealTimePreviewGenerationFlags( image, view,
                                                                              Rect( x0, y0, x1, y1 ), z ) );
      }
      ERROR_HANDLER
      return RealTimePreviewGenerationFlag::Invalid;
   }

   static api_bool api_func GenerateRealTimePreview( const_interface_handle hi,
                                                     image_handle himg, const_view_handle hv,
                                                     int32 x0, int32 y0, int32 x1, int32 y1, int32 z,
                                                     char16_type* info, uint32 maxLen )
   {
      try
      {
         UInt16Image image( himg );
         View view( hv );
         String infoStr;
         bool ok = reinterpret_cast<const ProcessInterface*>( hi )->GenerateRealTimePreview( image, view,
                                                                              Rect( x0, y0, x1, y1 ), z, infoStr );
         if ( !infoStr.IsEmpty() )
            if ( info != nullptr )
               if ( maxLen > 0 )
                  infoStr.c_copy( info, maxLen );
         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   static void CancelRealTimePreview( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->CancelRealTimePreview();
      }
      ERROR_HANDLER
   }

   static api_bool EnterDynamicMode( interface_handle hi )
   {
      try
      {
         return reinterpret_cast<ProcessInterface*>( hi )->EnterDynamicMode();
      }
      ERROR_HANDLER
      return api_false;
   }

   static void ExitDynamicMode( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->ExitDynamicMode();
      }
      ERROR_HANDLER
   }

   static api_bool api_func DynamicMouseEnter( interface_handle hi, view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->DynamicMouseEnter( v );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicMouseLeave( interface_handle hi, view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->DynamicMouseLeave( v );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicMouseMove( interface_handle hi, view_handle hv,
                                              double x, double y,
                                              api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      try
      {
         View v( hv );
         DPoint p( x, y );
         reinterpret_cast<ProcessInterface*>( hi )->DynamicMouseMove( v, p, buttons, modifiers );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicMousePress( interface_handle hi, view_handle hv,
                                               double x, double y,
                                               api_mouse_button button, api_mouse_buttons buttons,
                                               api_key_modifiers modifiers )
   {
      try
      {
         View v( hv );
         DPoint p( x, y );
         reinterpret_cast<ProcessInterface*>( hi )->DynamicMousePress( v, p, button, buttons, modifiers );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicMouseRelease( interface_handle hi, view_handle hv,
                                                 double x, double y,
                                                 api_mouse_button button, api_mouse_buttons buttons,
                                                 api_key_modifiers modifiers )
   {
      try
      {
         View v( hv );
         DPoint p( x, y );
         reinterpret_cast<ProcessInterface*>( hi )->DynamicMouseRelease( v, p, button, buttons, modifiers );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicMouseDoubleClick( interface_handle hi, view_handle hv,
                                                     double x, double y,
                                                     api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      try
      {
         View v( hv );
         DPoint p( x, y );
         reinterpret_cast<ProcessInterface*>( hi )->DynamicMouseDoubleClick( v, p, buttons, modifiers );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicKeyPress( interface_handle hi, view_handle hv,
                                             api_key_code key, api_key_modifiers modifiers )
   {
      try
      {
         View v( hv );
         return reinterpret_cast<ProcessInterface*>( hi )->DynamicKeyPress( v, key, modifiers );
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicKeyRelease( interface_handle hi, view_handle hv,
                                               api_key_code key, api_key_modifiers modifiers )
   {
      try
      {
         View v( hv );
         return reinterpret_cast<ProcessInterface*>( hi )->DynamicKeyRelease( v, key, modifiers );
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func DynamicMouseWheel( interface_handle hi, view_handle hv,
                                               double x, double y, int32 wheelDelta,
                                               api_mouse_buttons buttons, api_key_modifiers modifiers )
   {
      try
      {
         View v( hv );
         DPoint p( x, y );
         return reinterpret_cast<ProcessInterface*>( hi )->DynamicMouseWheel( v, p, wheelDelta, buttons, modifiers );
      }
      ERROR_HANDLER
      return api_false;
   }

   static api_bool api_func RequiresDynamicUpdate( const_interface_handle hi, const_view_handle hv,
                                                   double x0, double y0, double x1, double y1 )
   {
      try
      {
         View v( hv );
         DRect r( x0, y0, x1, y1 );
         return reinterpret_cast<const ProcessInterface*>( hi )->RequiresDynamicUpdate( v, r );
      }
      ERROR_HANDLER
      return api_false;
   }

   static void api_func DynamicPaint( const_interface_handle hi, const_view_handle hv, graphics_handle hg,
                                      double x0, double y0, double x1, double y1 )
   {
      try
      {
         View v( hv );
         VectorGraphics g( hg );
         DRect r( x0, y0, x1, y1 );
         reinterpret_cast<const ProcessInterface*>( hi )->DynamicPaint( v, g, r );
      }
      ERROR_HANDLER
   }

   // Notifications

   static void api_func ImageCreated( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageCreated( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageUpdated( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageUpdated( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageRenamed( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageRenamed( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageDeleted( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageDeleted( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageFocused( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageFocused( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageLocked( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageLocked( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageUnlocked( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageUnlocked( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageSTFEnabled( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageSTFEnabled( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageSTFDisabled( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageSTFDisabled( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageSTFUpdated( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageSTFUpdated( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageRGBWSUpdated( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageRGBWSUpdated( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageCMEnabled( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageCMEnabled( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageCMDisabled( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageCMDisabled( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageCMUpdated( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageCMUpdated( v );
      }
      ERROR_HANDLER
   }

   static void api_func ImageSaved( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->ImageSaved( v );
      }
      ERROR_HANDLER
   }

   static void api_func MaskUpdated( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->MaskUpdated( v );
      }
      ERROR_HANDLER
   }

   static void api_func MaskEnabled( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->MaskEnabled( v );
      }
      ERROR_HANDLER
   }

   static void api_func MaskDisabled( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->MaskDisabled( v );
      }
      ERROR_HANDLER
   }

   static void api_func MaskShown( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->MaskShown( v );
      }
      ERROR_HANDLER
   }

   static void api_func MaskHidden( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->MaskHidden( v );
      }
      ERROR_HANDLER
   }

   static void api_func TransparencyHidden( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->TransparencyHidden( v );
      }
      ERROR_HANDLER
   }

   static void api_func TransparencyModeUpdated( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->TransparencyModeUpdated( v );
      }
      ERROR_HANDLER
   }

   static void api_func ViewPropertyUpdated( interface_handle hi, const_view_handle hv, const char* property )
   {
      try
      {
         View v( hv );
         IsoString p( property );
         reinterpret_cast<ProcessInterface*>( hi )->ViewPropertyUpdated( v, p );
      }
      ERROR_HANDLER
   }

   static void api_func ViewPropertyDeleted( interface_handle hi, const_view_handle hv, const char* property )
   {
      try
      {
         View v( hv );
         IsoString p( property );
         reinterpret_cast<ProcessInterface*>( hi )->ViewPropertyDeleted( v, p );
      }
      ERROR_HANDLER
   }

   static void api_func BeginReadout( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->BeginReadout( v );
      }
      ERROR_HANDLER
   }

   static void api_func UpdateReadout( interface_handle hi, const_view_handle hv,
                                       double x, double y, double R, double G, double B, double A )
   {
      try
      {
         View v( hv );
         DPoint p( x, y );
         reinterpret_cast<ProcessInterface*>( hi )->UpdateReadout( v, p, R, G, B, A );
      }
      ERROR_HANDLER
   }

   static void api_func EndReadout( interface_handle hi, const_view_handle hv )
   {
      try
      {
         View v( hv );
         reinterpret_cast<ProcessInterface*>( hi )->EndReadout( v );
      }
      ERROR_HANDLER
   }

   static void api_func ProcessCreated( interface_handle hi, const_process_handle hp )
   {
      try
      {
         ProcessInstance instance( hp );
         reinterpret_cast<ProcessInterface*>( hi )->ProcessCreated( instance );
      }
      ERROR_HANDLER
   }

   static void api_func ProcessUpdated( interface_handle hi, const_process_handle hp )
   {
      try
      {
         ProcessInstance instance( hp );
         reinterpret_cast<ProcessInterface*>( hi )->ProcessUpdated( instance );
      }
      ERROR_HANDLER
   }

   static void api_func ProcessDeleted( interface_handle hi, const_process_handle hp )
   {
      try
      {
         ProcessInstance instance( hp );
         reinterpret_cast<ProcessInterface*>( hi )->ProcessDeleted( instance );
      }
      ERROR_HANDLER
   }

   static void api_func ProcessSaved( interface_handle hi, const_process_handle hp )
   {
      try
      {
         ProcessInstance instance( hp );
         reinterpret_cast<ProcessInterface*>( hi )->ProcessSaved( instance );
      }
      ERROR_HANDLER
   }

   static void api_func RealTimePreviewOwnerChanged( interface_handle hi, interface_handle hi1 )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->RealTimePreviewOwnerChanged(
            (hi1 != 0) ? *reinterpret_cast<ProcessInterface*>( hi1 ) : ProcessInterface::Null() );
      }
      ERROR_HANDLER
   }

   static void api_func RealTimePreviewLUTUpdated( interface_handle hi, int32 colorModel )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->RealTimePreviewLUTUpdated( colorModel );
      }
      ERROR_HANDLER
   }

   static void api_func RealTimePreviewGenerationStarted( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->RealTimePreviewGenerationStarted();
      }
      ERROR_HANDLER
   }

   static void api_func RealTimePreviewGenerationFinished( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->RealTimePreviewGenerationFinished();
      }
      ERROR_HANDLER
   }

   static void api_func GlobalRGBWSUpdated( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->GlobalRGBWSUpdated();
      }
      ERROR_HANDLER
   }

   static void api_func GlobalCMEnabled( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->GlobalCMEnabled();
      }
      ERROR_HANDLER
   }

   static void api_func GlobalCMDisabled( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->GlobalCMDisabled();
      }
      ERROR_HANDLER
   }

   static void api_func GlobalCMUpdated( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->GlobalCMUpdated();
      }
      ERROR_HANDLER
   }

   static void api_func ReadoutOptionsUpdated( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->ReadoutOptionsUpdated();
      }
      ERROR_HANDLER
   }

   static void api_func GlobalPreferencesUpdated( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->GlobalPreferencesUpdated();
      }
      ERROR_HANDLER
   }

   static void api_func GlobalFiltersUpdated( interface_handle hi )
   {
      try
      {
         reinterpret_cast<ProcessInterface*>( hi )->GlobalFiltersUpdated();
      }
      ERROR_HANDLER
   }
}; // InterfaceDispatcher

// ----------------------------------------------------------------------------

void ProcessInterface::PerformAPIDefinitions() const
{
   API_InterfaceDefinition_EnterInterfaceDefinitionContext();

   {
      IsoString id = Id();
      API_InterfaceDefinition_BeginInterfaceDefinition( this, id.c_str(), 0 );
   }

   API_InterfaceDefinition_SetInterfaceVersion( Version() );

   {
      IsoString aliases = Aliases().Trimmed();
      if ( !aliases.IsEmpty() )
         API_InterfaceDefinition_SetInterfaceAliasIdentifiers( aliases.c_str() );
   }

   {
      String desc = Description();
      if ( !desc.IsEmpty() )
         API_InterfaceDefinition_SetInterfaceDescription( desc.c_str() );
   }

   {
      IsoString svg = IconImageSVG();
      if ( !svg.IsEmpty() )
         API_InterfaceDefinition_SetInterfaceIconSVG( svg.c_str() );
      else
      {
         String filePath = IconImageSVGFile();
         if ( !filePath.IsEmpty() )
            API_InterfaceDefinition_SetInterfaceIconSVGFile( filePath.c_str() );
         else
         {
            // ### DEPRECATED - Interface icon images in raster bitmap formats.

            const char** xpm = IconImageXPM();
            if ( xpm != nullptr )
               API_InterfaceDefinition_SetInterfaceIconImage( xpm );
            else
            {
               String path = IconImageFile();
               if ( !path.IsEmpty() )
                  API_InterfaceDefinition_SetInterfaceIconImageFile( path.c_str() );
            }

            xpm = SmallIconImageXPM();
            if ( xpm != nullptr )
               API_InterfaceDefinition_SetInterfaceIconSmallImage( xpm );
            else
            {
               String path = SmallIconImageFile();
               if ( !path.IsEmpty() )
                  API_InterfaceDefinition_SetInterfaceIconSmallImageFile( path.c_str() );
            }
         }
      }
   }

   API_InterfaceDefinition_SetInterfaceFeatures( unsigned( Features() ), 0 );

   API_InterfaceDefinition_SetInterfaceInitializationRoutine( InterfaceDispatcher::Initialize );
   API_InterfaceDefinition_SetInterfaceLaunchRoutine( InterfaceDispatcher::Launch );

   if ( IsInstanceGenerator() )
   {
      API_InterfaceDefinition_SetInterfaceProcessInstantiationRoutine( InterfaceDispatcher::NewProcess );

      if ( DistinguishesTestInstances() )
         API_InterfaceDefinition_SetInterfaceProcessTestInstantiationRoutine( InterfaceDispatcher::NewTestProcess );
   }

   if ( CanImportInstances() )
   {
      if ( RequiresInstanceValidation() )
         API_InterfaceDefinition_SetInterfaceProcessValidationRoutine( InterfaceDispatcher::ValidateProcess );

      API_InterfaceDefinition_SetInterfaceProcessImportRoutine( InterfaceDispatcher::ImportProcess );
   }

   API_InterfaceDefinition_SetInterfaceApplyRoutine( InterfaceDispatcher::ApplyInstance );
   API_InterfaceDefinition_SetInterfaceApplyGlobalRoutine( InterfaceDispatcher::ApplyInstanceGlobal );
   API_InterfaceDefinition_SetInterfaceExecuteRoutine( InterfaceDispatcher::Execute );
   API_InterfaceDefinition_SetInterfaceCancelRoutine( InterfaceDispatcher::Cancel );
   API_InterfaceDefinition_SetInterfaceBrowseDocumentationRoutine( InterfaceDispatcher::BrowseDocumentation );
   API_InterfaceDefinition_SetInterfaceRealTimePreviewUpdatedRoutine( InterfaceDispatcher::RealTimePreviewUpdated );
   API_InterfaceDefinition_SetInterfaceTrackViewUpdatedRoutine( InterfaceDispatcher::TrackViewUpdated );
   API_InterfaceDefinition_SetInterfaceEditPreferencesRoutine( InterfaceDispatcher::EditPreferences );
   API_InterfaceDefinition_SetInterfaceResetRoutine( InterfaceDispatcher::ResetInstance );

   API_InterfaceDefinition_SetInterfaceRealTimeUpdateQueryRoutine( InterfaceDispatcher::RequiresRealTimePreviewUpdate );
   API_InterfaceDefinition_SetInterfaceRealTimeGenerationFlagsRoutine( InterfaceDispatcher::RealTimePreviewGenerationFlags );
   API_InterfaceDefinition_SetInterfaceRealTimeGenerationRoutine( InterfaceDispatcher::GenerateRealTimePreview );
   API_InterfaceDefinition_SetInterfaceRealTimeCancelRoutine( InterfaceDispatcher::CancelRealTimePreview );

   if ( IsDynamicInterface() )
   {
      API_InterfaceDefinition_SetInterfaceDynamicModeEnterRoutine( InterfaceDispatcher::EnterDynamicMode );
      API_InterfaceDefinition_SetInterfaceDynamicModeExitRoutine( InterfaceDispatcher::ExitDynamicMode );
      API_InterfaceDefinition_SetInterfaceDynamicMouseEnterRoutine( InterfaceDispatcher::DynamicMouseEnter );
      API_InterfaceDefinition_SetInterfaceDynamicMouseLeaveRoutine( InterfaceDispatcher::DynamicMouseLeave );
      API_InterfaceDefinition_SetInterfaceDynamicMouseMoveRoutine( InterfaceDispatcher::DynamicMouseMove );
      API_InterfaceDefinition_SetInterfaceDynamicMousePressRoutine( InterfaceDispatcher::DynamicMousePress );
      API_InterfaceDefinition_SetInterfaceDynamicMouseReleaseRoutine( InterfaceDispatcher::DynamicMouseRelease );
      API_InterfaceDefinition_SetInterfaceDynamicMouseDoubleClickRoutine( InterfaceDispatcher::DynamicMouseDoubleClick );
      API_InterfaceDefinition_SetInterfaceDynamicKeyPressRoutine( InterfaceDispatcher::DynamicKeyPress );
      API_InterfaceDefinition_SetInterfaceDynamicKeyReleaseRoutine( InterfaceDispatcher::DynamicKeyRelease );
      API_InterfaceDefinition_SetInterfaceDynamicMouseWheelRoutine( InterfaceDispatcher::DynamicMouseWheel );
      API_InterfaceDefinition_SetInterfaceDynamicUpdateQueryRoutine( InterfaceDispatcher::RequiresDynamicUpdate );
      API_InterfaceDefinition_SetInterfaceDynamicPaintRoutine( InterfaceDispatcher::DynamicPaint );
   }

   if ( WantsImageNotifications() )
   {
      API_InterfaceDefinition_SetImageCreatedNotificationRoutine( InterfaceDispatcher::ImageCreated );
      API_InterfaceDefinition_SetImageUpdatedNotificationRoutine( InterfaceDispatcher::ImageUpdated );
      API_InterfaceDefinition_SetImageRenamedNotificationRoutine( InterfaceDispatcher::ImageRenamed );
      API_InterfaceDefinition_SetImageDeletedNotificationRoutine( InterfaceDispatcher::ImageDeleted );
      API_InterfaceDefinition_SetImageFocusedNotificationRoutine( InterfaceDispatcher::ImageFocused );
      API_InterfaceDefinition_SetImageLockedNotificationRoutine( InterfaceDispatcher::ImageLocked );
      API_InterfaceDefinition_SetImageUnlockedNotificationRoutine( InterfaceDispatcher::ImageUnlocked );
      API_InterfaceDefinition_SetImageSTFEnabledNotificationRoutine( InterfaceDispatcher::ImageSTFEnabled );
      API_InterfaceDefinition_SetImageSTFDisabledNotificationRoutine( InterfaceDispatcher::ImageSTFDisabled );
      API_InterfaceDefinition_SetImageSTFUpdatedNotificationRoutine( InterfaceDispatcher::ImageSTFUpdated );
      API_InterfaceDefinition_SetImageRGBWSUpdatedNotificationRoutine( InterfaceDispatcher::ImageRGBWSUpdated );
      API_InterfaceDefinition_SetImageCMEnabledNotificationRoutine( InterfaceDispatcher::ImageCMEnabled );
      API_InterfaceDefinition_SetImageCMDisabledNotificationRoutine( InterfaceDispatcher::ImageCMDisabled );
      API_InterfaceDefinition_SetImageCMUpdatedNotificationRoutine( InterfaceDispatcher::ImageCMUpdated );
      API_InterfaceDefinition_SetImageSavedNotificationRoutine( InterfaceDispatcher::ImageSaved );
   }

   if ( WantsMaskNotifications() )
   {
      API_InterfaceDefinition_SetMaskUpdatedNotificationRoutine( InterfaceDispatcher::MaskUpdated );
      API_InterfaceDefinition_SetMaskEnabledNotificationRoutine( InterfaceDispatcher::MaskEnabled );
      API_InterfaceDefinition_SetMaskDisabledNotificationRoutine( InterfaceDispatcher::MaskDisabled );
      API_InterfaceDefinition_SetMaskShownNotificationRoutine( InterfaceDispatcher::MaskShown );
      API_InterfaceDefinition_SetMaskHiddenNotificationRoutine( InterfaceDispatcher::MaskHidden );
   }

   if ( WantsTransparencyNotifications() )
   {
      API_InterfaceDefinition_SetTransparencyHiddenNotificationRoutine( InterfaceDispatcher::TransparencyHidden );
      API_InterfaceDefinition_SetTransparencyModeUpdatedNotificationRoutine( InterfaceDispatcher::TransparencyModeUpdated );
   }

   if ( WantsViewPropertyNotifications() )
   {
      API_InterfaceDefinition_SetViewPropertyUpdatedNotificationRoutine( InterfaceDispatcher::ViewPropertyUpdated );
      API_InterfaceDefinition_SetViewPropertyDeletedNotificationRoutine( InterfaceDispatcher::ViewPropertyDeleted );
   }

   if ( WantsReadoutNotifications() )
   {
      API_InterfaceDefinition_SetBeginReadoutNotificationRoutine( InterfaceDispatcher::BeginReadout );
      API_InterfaceDefinition_SetUpdateReadoutNotificationRoutine( InterfaceDispatcher::UpdateReadout );
      API_InterfaceDefinition_SetEndReadoutNotificationRoutine( InterfaceDispatcher::EndReadout );
   }

   if ( WantsProcessNotifications() )
   {
      API_InterfaceDefinition_SetProcessCreatedNotificationRoutine( InterfaceDispatcher::ProcessCreated );
      API_InterfaceDefinition_SetProcessUpdatedNotificationRoutine( InterfaceDispatcher::ProcessUpdated );
      API_InterfaceDefinition_SetProcessDeletedNotificationRoutine( InterfaceDispatcher::ProcessDeleted );
      API_InterfaceDefinition_SetProcessSavedNotificationRoutine( InterfaceDispatcher::ProcessSaved );
   }

   if ( WantsRealTimePreviewNotifications() )
   {
      API_InterfaceDefinition_SetRealTimePreviewOwnerChangeNotificationRoutine( InterfaceDispatcher::RealTimePreviewOwnerChanged );
      API_InterfaceDefinition_SetRealTimePreviewLUTUpdatedNotificationRoutine( InterfaceDispatcher::RealTimePreviewLUTUpdated );
      API_InterfaceDefinition_SetRealTimePreviewGenerationStartNotificationRoutine( InterfaceDispatcher::RealTimePreviewGenerationStarted );
      API_InterfaceDefinition_SetRealTimePreviewGenerationFinishNotificationRoutine( InterfaceDispatcher::RealTimePreviewGenerationFinished );
   }

   if ( WantsGlobalNotifications() )
   {
      API_InterfaceDefinition_SetGlobalRGBWSUpdatedNotificationRoutine( InterfaceDispatcher::GlobalRGBWSUpdated );
      API_InterfaceDefinition_SetGlobalCMEnabledNotificationRoutine( InterfaceDispatcher::GlobalCMEnabled );
      API_InterfaceDefinition_SetGlobalCMDisabledNotificationRoutine( InterfaceDispatcher::GlobalCMDisabled );
      API_InterfaceDefinition_SetGlobalCMUpdatedNotificationRoutine( InterfaceDispatcher::GlobalCMUpdated );
      API_InterfaceDefinition_SetReadoutOptionsUpdatedNotificationRoutine( InterfaceDispatcher::ReadoutOptionsUpdated );
      API_InterfaceDefinition_SetGlobalPreferencesUpdatedNotificationRoutine( InterfaceDispatcher::GlobalPreferencesUpdated );
      API_InterfaceDefinition_SetGlobalFiltersUpdatedNotificationRoutine( InterfaceDispatcher::GlobalFiltersUpdated );
   }

   API_InterfaceDefinition_EndInterfaceDefinition();

   API_InterfaceDefinition_ExitInterfaceDefinitionContext();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProcessInterface.cpp - Released 2025-04-07T08:53:32Z
