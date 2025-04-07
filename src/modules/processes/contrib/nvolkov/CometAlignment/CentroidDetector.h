//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.9
// ----------------------------------------------------------------------------
// CentroidDetector.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CentroidDetector_h
#define __CentroidDetector_h

#include <pcl/ImageVariant.h>
#include <pcl/PSFFit.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class CentroidDetector
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
      NoPSFFit,
      UnknownError
   };

   PSFData psf;
   Rect    rect = 0;
   Status  status = NotDetected;

   CentroidDetector( const ImageVariant& image, int channel,
                     const DPoint& pos, int searchRadius = 8, float threshold = 1.0F );

   operator bool() const
   {
      return psf;
   }

   String StatusText() const
   {
      switch ( status )
      {
      case NotDetected:       return "Not detected";
      case DetectedOk:        return "Detected Ok";
      case NoSignificantData: return "No significant data";
      case CrossingEdges:     return "Crossing edges";
      case OutsideImage:      return "Outside image";
      case NoConvergence:     return "No convergence";
      case NoPSFFit:          return "PSF fitting failure";
      default:
      case UnknownError:      return "Unknown error";
      }
   }
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __CentroidDetector_h

// ----------------------------------------------------------------------------
// EOF CentroidDetector.h - Released 2025-04-07T08:53:56Z
