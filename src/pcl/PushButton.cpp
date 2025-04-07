//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PushButton.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/PushButton.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PushButton::PushButton( const String& text, const pcl::Bitmap& icon, Control& parent )
   : Button( (*API->Button->CreatePushButton)( ModuleHandle(), this, text.c_str(), icon.handle, parent.handle, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreatePushButton" );
}

// ----------------------------------------------------------------------------

bool PushButton::IsDefault() const
{
   return (*API->Button->GetButtonDefaultEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void PushButton::SetDefault( bool defButton )
{
   (*API->Button->SetButtonDefaultEnabled)( handle, defButton );
}

// ----------------------------------------------------------------------------

} //pcl

// ----------------------------------------------------------------------------
// EOF pcl/PushButton.cpp - Released 2025-04-07T08:53:32Z
