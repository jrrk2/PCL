//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/BitmapBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/BitmapBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BitmapBox::BitmapBox( const Bitmap& bm, Control& parent )
   : Frame( API_BitmapBox_CreateBitmapBox( ModuleHandle(), this, bm.handle, parent.handle, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapBox" );
}

// ----------------------------------------------------------------------------

Bitmap BitmapBox::CurrentBitmap() const
{
   return Bitmap( API_BitmapBox_GetBitmapBoxBitmap( handle ) );
}

// ----------------------------------------------------------------------------

void BitmapBox::SetBitmap( const Bitmap& pm )
{
   API_BitmapBox_SetBitmapBoxBitmap( handle, pm.handle );
}

// ----------------------------------------------------------------------------

int BitmapBox::Margin() const
{
   return API_BitmapBox_GetBitmapBoxMargin( handle );
}

// ----------------------------------------------------------------------------

void BitmapBox::SetMargin( int px )
{
   API_BitmapBox_SetBitmapBoxMargin( handle, px );
}

// ----------------------------------------------------------------------------

bool BitmapBox::IsAutoFitEnabled() const
{
   return API_BitmapBox_GetBitmapBoxAutoFitEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void BitmapBox::EnableAutoFit( bool enable )
{
   API_BitmapBox_SetBitmapBoxAutoFitEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/BitmapBox.cpp - Released 2025-04-07T08:53:32Z
