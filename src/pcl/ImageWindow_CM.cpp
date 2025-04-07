//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageWindow_CM.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ImageWindow.h>
#include <pcl/ICCProfile.h>
#include <pcl/ErrorHandler.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

bool ImageWindow::IsColorManagementEnabled() const
{
   api_bool enableCM;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, &enableCM, 0, 0 );
   return enableCM != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableColorManagement( bool enable )
{
   api_bool proofing, gamutCheck;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, 0, &proofing, &gamutCheck );
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, enable, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsProofingEnabled() const
{
   api_bool proofing;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, 0, &proofing, 0 );
   return proofing != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableProofing( bool enable )
{
   api_bool enableCM, gamutCheck;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, &enableCM, 0, &gamutCheck );
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, enableCM, enable, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsGamutCheckEnabled() const
{
   api_bool gamutCheck;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, 0, 0, &gamutCheck );
   return gamutCheck != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableGamutCheck( bool enable )
{
   api_bool enableCM, proofing;
   (*API->ImageWindow->GetImageWindowCMEnabled)( handle, &enableCM, &proofing, 0 );
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, enableCM, enable || proofing, enable );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetColorManagementFeatures( bool cmEnabled, bool proofing, bool gamutCheck )
{
   (*API->ImageWindow->SetImageWindowCMEnabled)( handle, cmEnabled, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageWindow::GetICCProfile( ICCProfile& profile ) const
{
   profile.Clear();

   size_type profileSize = (*API->ImageWindow->GetImageWindowICCProfileLength)( handle );
   if ( profileSize == 0 )
      return false;

   try
   {
      ByteArray profileData( profileSize );
      (*API->ImageWindow->GetImageWindowICCProfile)( handle, profileData.Begin() );
      profile.Set( profileData );
      return true;
   }
   ERROR_HANDLER

   return false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetICCProfile( const ICCProfile& profile )
{
   if ( profile.IsProfile() )
      (*API->ImageWindow->SetImageWindowICCProfile)( handle, profile.ProfileData().Begin() );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetICCProfile( const String& filePath )
{
   (*API->ImageWindow->LoadImageWindowICCProfile)( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

void ImageWindow::DeleteICCProfile()
{
   (*API->ImageWindow->DeleteImageWindowICCProfile)( handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageWindow_CM.cpp - Released 2025-04-07T08:53:32Z
