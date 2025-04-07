//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ColorSpace.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ImageColor.h>

namespace pcl
{
   namespace ColorSpace
   {
      String Name( int s )
      {
         static const char* name[] =
         { "Grayscale", "RGB", "CIE XYZ", "CIE L*a*b*", "CIE L*c*h*", "HSV", "HSI" };
         PCL_PRECONDITION( s >= 0 && s < NumberOfColorSpaces )
         if ( s >= 0 && s < NumberOfColorSpaces )
            return String( name[s] );
         return String();
      }

      String ChannelId( int s, int c )
      {
         static const char* id[3][7] = { { "K", "R", "X", "L*", "L*", "H", "H" },
                                         { "",  "G", "Y", "a*", "c*", "S", "S" },
                                         { "",  "B", "Z", "b*", "h*", "V", "I" } };
         PCL_PRECONDITION( s >= 0 && s < NumberOfColorSpaces )
         PCL_PRECONDITION( c >= 0 )
         if ( s >= 0 && s < NumberOfColorSpaces && c >= 0 )
         {
            if ( s == Gray && c > 0 )
               c += 2;
            if ( c < 3 )
               return String( id[c][s] );
            return String().Format( "alpha%02d", c-2 );
         }
         return String();
      }
   } // ColorSpace
} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ColorSpace.cpp - Released 2025-04-07T08:53:32Z
