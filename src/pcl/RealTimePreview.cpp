//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/RealTimePreview.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/RealTimePreview.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Exception.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

bool RealTimePreview::SetOwner( ProcessInterface& iface )
{
   interface_handle h = iface.IsNull() ? 0 : reinterpret_cast<interface_handle>( &iface );
   return API_RealTimePreview_SetRealTimePreviewOwner( h, 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

void RealTimePreview::Update()
{
   API_RealTimePreview_UpdateRealTimePreview();
}

// ----------------------------------------------------------------------------

bool RealTimePreview::IsUpdating()
{
   return API_RealTimePreview_IsRealTimePreviewUpdating() != api_false;
}

// ----------------------------------------------------------------------------

void RealTimePreview::ShowProgressDialog( const String& title, const String& text, size_type totalCount )
{
   API_RealTimePreview_ShowRealTimePreviewProgressDialog( title.c_str(), text.c_str(), totalCount, 0/*flags*/ );
}

// ----------------------------------------------------------------------------

void RealTimePreview::CloseProgressDialog()
{
   API_RealTimePreview_CloseRealTimePreviewProgressDialog();
}

// ----------------------------------------------------------------------------

bool RealTimePreview::IsProgressDialogVisible()
{
   return API_RealTimePreview_IsRealTimePreviewProgressDialogVisible() != api_false;
}

// ----------------------------------------------------------------------------

void RealTimePreview::SetProgressCount( size_type count )
{
   API_RealTimePreview_SetRealTimePreviewProgressCount( count, 0/*flags*/ );
}

// ----------------------------------------------------------------------------

void RealTimePreview::SetProgressText( const String& text )
{
   API_RealTimePreview_SetRealTimePreviewProgressText( text.c_str(), 0/*flags*/ );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/RealTimePreview.cpp - Released 2025-04-07T08:53:32Z
