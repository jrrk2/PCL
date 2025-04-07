//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageOp.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Image.h>
#include <pcl/String.h>

namespace pcl
{
   namespace ImageOp
   {
      String Id( value_type op )
      {
         static const char* name[] =
         { "Nop", "Mov", "Add", "Sub", "Mul", "Div", "Pow", "Dif", "Min", "Max",
           "Or", "And", "Xor", "Not", "Nor", "Nand", "Xnor",
           "ColorBurn", "LinearBurn", "Screen", "ColorDodge", "Overlay", "SoftLight",
           "HardLight", "VividLight", "LinearLight", "PinLight", "Exclusion" };
         return name[op];
      }
   } // ImageOp
} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageOp.cpp - Released 2025-04-07T08:53:32Z
