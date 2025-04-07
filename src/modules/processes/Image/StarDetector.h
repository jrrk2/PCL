//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// StarDetector.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __StarDetector_h
#define __StarDetector_h

#include <pcl/Image.h>
#include <pcl/MetaParameter.h>   // pcl_enum, pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

struct StarData
{
   pcl_enum status;  // error code (StarDetector::Status)
   int32    channel; // channel index (0=R/K, 1=G, 2=B)
   DRect    rect;    // selection rectangle
   DPoint   pos;     // barycenter position

   mutable DRect drawRect; // drawing region

   operator bool() const;

   String StatusText() const;
};

// ----------------------------------------------------------------------------

class StarDetector
{
public:

   enum Status
   {
      NotDetected,
      DetectedOk,
      NoSignificantData,
      CrossingEdges,
      OutsideImage,
      NoConvergence,
      UnknownError
   };

   StarData star;

   StarDetector( const Image& img, int channel,
                 const DPoint& pos, int searchRadius = 8, float bkgThreshold = 1.0F,
                 bool autoAperture = true );

   operator bool() const
   {
      return star;
   }
};

// ----------------------------------------------------------------------------

inline StarData::operator bool() const
{
   return status == StarDetector::DetectedOk;
}

// ----------------------------------------------------------------------------

inline String StarData::StatusText() const
{
   switch ( status )
   {
   case StarDetector::NotDetected:       return "Not detected";
   case StarDetector::DetectedOk:        return "Detected Ok";
   case StarDetector::NoSignificantData: return "No significant data";
   case StarDetector::CrossingEdges:     return "Crossing edges";
   case StarDetector::OutsideImage:      return "Outside image";
   case StarDetector::NoConvergence:     return "No convergence";
   default:
   case StarDetector::UnknownError:      return "Unknown error";
   }
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarDetector_h

// ----------------------------------------------------------------------------
// EOF StarDetector.h - Released 2025-04-07T08:53:56Z
