//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SpinBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/SpinBox.h>
#include <pcl/TextAlign.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### TODO: Create a new class to encapsulate the common behavior of SpinBox
 *           and Slider. Proposed class name: RangeControl
 */

SpinBox::SpinBox( Control& parent )
   : Control( (*API->SpinBox->CreateSpinBox)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateSpinBox" );
}

// ----------------------------------------------------------------------------

int SpinBox::Value() const
{
   return (*API->SpinBox->GetSpinBoxValue)( handle );
}

// ----------------------------------------------------------------------------

void SpinBox::SetValue( int value )
{
   return (*API->SpinBox->SetSpinBoxValue)( handle, value );
}

// ----------------------------------------------------------------------------

double SpinBox::NormalizedValue() const
{
   int v0, v1; GetRange( v0, v1 );
   return (v0 != v1) ? double( Value() - v0 )/(v1 - v0) : 0.0;
}

// ----------------------------------------------------------------------------

void SpinBox::SetNormalizedValue( double f )
{
   int v0, v1; GetRange( v0, v1 );
   SetValue( v0 + RoundInt( Range( f, 0.0, 1.0 )*(v1 - v0) ) );
}

// ----------------------------------------------------------------------------

void SpinBox::GetRange( int& minValue, int& maxValue ) const
{
   (*API->SpinBox->GetSpinBoxRange)( handle, &minValue, &maxValue );
}

// ----------------------------------------------------------------------------

void SpinBox::SetRange( int minValue, int maxValue )
{
   (*API->SpinBox->SetSpinBoxRange)( handle, minValue, maxValue );
}

// ----------------------------------------------------------------------------

int SpinBox::StepSize() const
{
   return (*API->SpinBox->GetSpinBoxStepSize)( handle );
}

// ----------------------------------------------------------------------------

void SpinBox::SetStepSize( int size )
{
   (*API->SpinBox->SetSpinBoxStepSize)( handle, size );
}

// ----------------------------------------------------------------------------

bool SpinBox::IsWrappingEnabled() const
{
   return (*API->SpinBox->GetSpinBoxWrappingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void SpinBox::EnableWrapping( bool enable )
{
   (*API->SpinBox->SetSpinBoxWrappingEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool SpinBox::IsEditable() const
{
   return (*API->SpinBox->GetSpinBoxEditable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void SpinBox::SetEditable( bool enable )
{
   (*API->SpinBox->SetSpinBoxEditable)( handle, enable );
}

// ----------------------------------------------------------------------------

String SpinBox::Prefix() const
{
   size_type len = 0;
   (*API->SpinBox->GetSpinBoxPrefix)( handle, 0, &len );

   String prefix;
   if ( len > 0 )
   {
      prefix.SetLength( len );
      if ( (*API->SpinBox->GetSpinBoxPrefix)( handle, prefix.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetSpinBoxPrefix" );
      prefix.ResizeToNullTerminated();
   }
   return prefix;
}

// ----------------------------------------------------------------------------

void SpinBox::SetPrefix( const String& txt )
{
   (*API->SpinBox->SetSpinBoxPrefix)( handle, txt.c_str() );
}

// ----------------------------------------------------------------------------

String SpinBox::Suffix() const
{
   size_type len = 0;
   (*API->SpinBox->GetSpinBoxSuffix)( handle, 0, &len );

   String suffix;
   if ( len > 0 )
   {
      suffix.SetLength( len );
      if ( (*API->SpinBox->GetSpinBoxSuffix)( handle, suffix.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetSpinBoxSuffix" );
      suffix.ResizeToNullTerminated();
   }
   return suffix;
}

// ----------------------------------------------------------------------------

void SpinBox::SetSuffix( const String& txt )
{
   (*API->SpinBox->SetSpinBoxSuffix)( handle, txt.c_str() );
}

// ----------------------------------------------------------------------------

String SpinBox::MinimumValueText() const
{
   size_type len = 0;
   (*API->SpinBox->GetSpinBoxMinimumValueText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->SpinBox->GetSpinBoxMinimumValueText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetSpinBoxMinimumValueText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void SpinBox::SetMinimumValueText( const String& txt )
{
   (*API->SpinBox->SetSpinBoxMinimumValueText)( handle, txt.c_str() );
}

// ----------------------------------------------------------------------------

bool SpinBox::IsRightAligned() const
{
   return (*API->SpinBox->GetSpinBoxAlignment)( handle ) == TextAlign::Right;
}

// ----------------------------------------------------------------------------

void SpinBox::SetRightAlignment( bool right )
{
   (*API->SpinBox->SetSpinBoxAlignment)( handle, right ? TextAlign::Right : TextAlign::Left );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<SpinBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class SpinBoxEventDispatcher
{
public:

   static void api_func ValueUpdated( control_handle hSender, control_handle hReceiver, int32 value )
   {
      if ( handlers->onValueUpdated != nullptr )
         (receiver->*handlers->onValueUpdated)( *sender, value );
   }

   static void api_func RangeUpdated( control_handle hSender, control_handle hReceiver, int32 minValue, int32 maxValue )
   {
      if ( handlers->onRangeUpdated != nullptr )
         (receiver->*handlers->onRangeUpdated)( *sender, minValue, maxValue );
   }

}; // SpinBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void SpinBox::OnValueUpdated( value_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->SpinBox->SetSpinBoxValueUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? SpinBoxEventDispatcher::ValueUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetSpinBoxValueUpdatedEventRoutine" );
   m_handlers->onValueUpdated = f;
}

void SpinBox::OnRangeUpdated( range_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->SpinBox->SetSpinBoxRangeUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? SpinBoxEventDispatcher::RangeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetSpinBoxRangeUpdatedEventRoutine" );
   m_handlers->onRangeUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SpinBox.cpp - Released 2025-04-07T08:53:32Z
