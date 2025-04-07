//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ToolButton.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ToolButton.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ToolButton::ToolButton( const String& text, const pcl::Bitmap& icon, bool checkable, Control& parent )
   : Button( (*API->Button->CreateToolButton)( ModuleHandle(), this, text.c_str(), icon.handle, checkable, parent.handle, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateToolButton" );
}

// ----------------------------------------------------------------------------

bool ToolButton::IsCheckable() const
{
   return (*API->Button->GetToolButtonCheckable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ToolButton::SetCheckable( bool checkable )
{
   (*API->Button->SetToolButtonCheckable)( handle, checkable );
}

// ----------------------------------------------------------------------------

} //pcl

// ----------------------------------------------------------------------------
// EOF pcl/ToolButton.cpp - Released 2025-04-07T08:53:32Z
