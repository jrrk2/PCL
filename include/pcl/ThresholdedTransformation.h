//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ThresholdedTransformation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ThresholdedTransformation_h
#define __PCL_ThresholdedTransformation_h

/// \file pcl/ThresholdedTransformation.h

#include <pcl/Defs.h>

#include <pcl/ImageTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ThresholdedTransformation
 * \brief Thresholded image transformation.
 *
 * %ThresholdedTransformation represents a <em>thresholded image
 * transformation</em> in PCL. A thresholded transformation modifies its effect
 * when applied to pixels within a given range of pixel sample values.
 *
 * Instead of defining a fixed range of sample values, a thresholded
 * transformation defines two \e threshold values in PCL. These thresholds are
 * normalized \e distances measured from a \e floating reference value that can
 * be specified on a per-pixel basis.
 *
 * In PCL, convolutions in the spatial domain and morphological transformations
 * have been implemented as thresholded transformations.
 */
class PCL_CLASS ThresholdedTransformation : public virtual ImageTransformation
{
public:

   /*!
    * Constructs a %ThresholdedTransformation object with the specified \a low
    * and \a high threshold values in the normalized real range [0,1].
    */
   ThresholdedTransformation( double low = 0, double high = 0 )
      : m_lowThreshold( low )
      , m_highThreshold( high )
   {
   }

   /*!
    * Copy constructor.
    */
   ThresholdedTransformation( const ThresholdedTransformation& ) = default;

   /*!
    * Destroys a %ThresholdedTransformation object.
    */
   ~ThresholdedTransformation() override
   {
   }

   /*!
    * Returns true iff this transformation is currently thresholded.
    *
    * The transformation is thresholded when either threshold (low or high) is
    * nonzero. When both thresholds are zero, the transformation applies no
    * thresholding at all.
    */
   bool IsThresholded() const
   {
      return 1 + m_lowThreshold != 1 || 1 + m_highThreshold != 1;
   }

   /*!
    * Returns the current low threshold in the normalized real range.
    */
   double LowThreshold() const
   {
      return m_lowThreshold;
   }

   /*!
    * Sets the low threshold to the specified sample value \a t in the
    * normalized real range.
    */
   void SetLowThreshold( double t )
   {
      m_lowThreshold = t;
   }

   /*!
    * Returns the current high threshold in the normalized real range.
    */
   double HighThreshold() const
   {
      return m_highThreshold;
   }

   /*!
    * Sets the high threshold to the specified sample value \a t in the
    * normalized real range.
    */
   void SetHighThreshold( double t )
   {
      m_highThreshold = t;
   }

private:

   /*
    * Normalized threshold values.
    */
   double m_lowThreshold = 0;
   double m_highThreshold = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ThresholdedTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/ThresholdedTransformation.h - Released 2025-04-07T08:52:44Z
