//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CheckBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/CheckBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CheckBox::CheckBox( const String& text, Control& parent )
   : Button( (*API->Button->CreateCheckBox)( ModuleHandle(), this, text.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateCheckBox" );
}

// ----------------------------------------------------------------------------

bool CheckBox::IsTristateMode() const
{
   return (*API->Button->GetButtonTristateEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CheckBox::SetTristateMode( bool tristate )
{
   (*API->Button->SetButtonTristateEnabled)( handle, tristate );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CheckBox.cpp - Released 2025-04-07T08:53:32Z
