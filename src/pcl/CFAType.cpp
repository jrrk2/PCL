//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CFAType.cpp - Released 2025-04-07T08:53:32Z
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
   namespace CFAType
   {
      String Name( int cfa )
      {
         static const char* name[] =
         { "None", "BGGR", "GRBG", "GBRG", "RGGB", "CYGM" };
         PCL_PRECONDITION( cfa >= 0 && cfa < NumberOfCFATypes )
         if ( cfa >= 0 && cfa < NumberOfCFATypes )
            return String( name[cfa] );
         return String();
      }
   } // CFAType
} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CFAType.cpp - Released 2025-04-07T08:53:32Z
