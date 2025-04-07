//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageType.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ImageOptions.h>

namespace pcl
{
   namespace ImageType
   {
      String Name( int type )
      {
         static const char* name[] =
         {
            "Unknown",
            "Bias",
            "Dark",
            "Flat",
            "Light",
            "Master Bias",
            "Master Dark",
            "Master Flat",
            "Master Light",
            "Defect Map",
            "High Rejection Map",
            "Low Rejection Map",
            "Binary High Rejection Map",
            "Binary Low Rejection Map",
            "Slope Map",
            "Weight Map"
         };
         PCL_PRECONDITION( type >= 0 && type < NumberOfImageTypes )
         if ( type >= 0 && type < NumberOfImageTypes )
            return String( name[type] );
         return String();
      }
   } // ImageType
} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageType.cpp - Released 2025-04-07T08:53:32Z
