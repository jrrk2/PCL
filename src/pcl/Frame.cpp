//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Frame.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Frame.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Frame::Frame( Control& parent )
   : Control( (*API->Frame->CreateFrame)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateFrame" );
}

// ----------------------------------------------------------------------------

Frame::style Frame::Style() const
{
   return style( (*API->Frame->GetFrameStyle)( handle ) );
}

// ----------------------------------------------------------------------------

void Frame::SetStyle( Frame::style style )
{
   (*API->Frame->SetFrameStyle)( handle, style );
}

// ----------------------------------------------------------------------------

int Frame::LineWidth() const
{
   return (*API->Frame->GetFrameLineWidth)( handle );
}

// ----------------------------------------------------------------------------

void Frame::SetLineWidth( int px )
{
   (*API->Frame->SetFrameLineWidth)( handle, px );
}

// ----------------------------------------------------------------------------

int Frame::BorderWidth() const
{
   return (*API->Frame->GetFrameBorderWidth)( handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Frame.cpp - Released 2025-04-07T08:53:32Z
