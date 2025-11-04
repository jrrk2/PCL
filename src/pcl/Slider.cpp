//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Slider.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Math.h>
#include <pcl/Slider.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### TODO: Create a new class to encapsulate the common behavior of SpinBox
 *           and Slider. Proposed class name: RangeControl
 */

Slider::Slider( Control& parent, bool vertical )
   : Control( API_Slider_CreateSlider( ModuleHandle(), this, vertical, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateSlider" );
}

// ----------------------------------------------------------------------------

int Slider::Value() const
{
   return API_Slider_GetSliderValue( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetValue( int value )
{
   API_Slider_SetSliderValue( handle, value );
}

// ----------------------------------------------------------------------------

double Slider::NormalizedValue() const
{
   int v0, v1; GetRange( v0, v1 );
   return (v0 != v1) ? double( Value() - v0 )/(v1 - v0) : 0.0;
}

// ----------------------------------------------------------------------------

void Slider::SetNormalizedValue( double f )
{
   int v0, v1; GetRange( v0, v1 );
   SetValue( v0 + RoundInt( Range( f, 0.0, 1.0 )*(v1 - v0) ) );
}

// ----------------------------------------------------------------------------

void Slider::GetRange( int& minValue, int& maxValue ) const
{
   API_Slider_GetSliderRange( handle, &minValue, &maxValue );
}

// ----------------------------------------------------------------------------

void Slider::SetRange( int minValue, int maxValue )
{
   API_Slider_SetSliderRange( handle, minValue, maxValue );
}

// ----------------------------------------------------------------------------

int Slider::StepSize() const
{
   return API_Slider_GetSliderStepSize( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetStepSize( int stepSize )
{
   API_Slider_SetSliderStepSize( handle, stepSize );
}

// ----------------------------------------------------------------------------

int Slider::PageSize() const
{
   return API_Slider_GetSliderPageSize( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetPageSize( int pageSize )
{
   API_Slider_SetSliderPageSize( handle, pageSize );
}

// ----------------------------------------------------------------------------

int Slider::TickInterval() const
{
   return API_Slider_GetSliderTickInterval( handle );
}

// ----------------------------------------------------------------------------

void Slider::SetTickInterval( int interval )
{
   API_Slider_SetSliderTickInterval( handle, interval );
}

// ----------------------------------------------------------------------------

TickStyles Slider::TickStyle() const
{
   return TickStyle::mask_type( API_Slider_GetSliderTickStyle( handle ) );
}

// ----------------------------------------------------------------------------

void Slider::SetTickStyle( TickStyles style )
{
   API_Slider_SetSliderTickStyle( handle, style );
}

// ----------------------------------------------------------------------------

bool Slider::IsTrackingEnabled() const
{
   return API_Slider_GetSliderTrackingEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Slider::EnableTracking ( bool enable )
{
   API_Slider_SetSliderTrackingEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Slider*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class SliderEventDispatcher
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

}; // SliderEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void Slider::OnValueUpdated( value_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Slider_SetSliderValueUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? SliderEventDispatcher::ValueUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetSliderValueUpdatedEventRoutine" );
   m_handlers->onValueUpdated = f;
}

void Slider::OnRangeUpdated( range_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Slider_SetSliderRangeUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? SliderEventDispatcher::RangeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetSliderRangeUpdatedEventRoutine" );
   m_handlers->onRangeUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Slider.cpp - Released 2025-04-07T08:53:32Z
