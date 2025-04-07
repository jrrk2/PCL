//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Label.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Label.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Label::Label( const String& text, Control& parent )
   : Frame( (*API->Label->CreateLabel)( ModuleHandle(), this, text.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateLabel" );
}

// ----------------------------------------------------------------------------

String Label::Text() const
{
   size_type len = 0;
   (*API->Label->GetLabelText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->Label->GetLabelText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetLabelText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void Label::SetText( const String& text )
{
   (*API->Label->SetLabelText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

int Label::Margin() const
{
   return (*API->Label->GetLabelMargin)( handle );
}

// ----------------------------------------------------------------------------

void Label::SetMargin( int px )
{
   (*API->Label->SetLabelMargin)( handle, px );
}

// ----------------------------------------------------------------------------

int Label::TextAlignment() const
{
   return (*API->Label->GetLabelAlignment)( handle );
}

// ----------------------------------------------------------------------------

void Label::SetTextAlignment( int align )
{
   (*API->Label->SetLabelAlignment)( handle, align );
}

// ----------------------------------------------------------------------------

bool Label::IsWordWrappingEnabled() const
{
   return (*API->Label->GetLabelWordWrappingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Label::EnableWordWrapping( bool enable )
{
   (*API->Label->SetLabelWordWrappingEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Label::IsRichTextEnabled() const
{
   return (*API->Label->GetLabelRichTextEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Label::EnableRichText( bool enable )
{
   (*API->Label->SetLabelRichTextEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Label.cpp - Released 2025-04-07T08:53:32Z
