//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MultiscaleLinearTransform.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_MultiscaleLinearTransform_h
#define __PCL_MultiscaleLinearTransform_h

/// \file pcl/MultiscaleLinearTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/RedundantMultiscaleTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MultiscaleLinearTransform
 * \brief A redundant multiscale transform using separable convolutions.
 *
 * The multiscale linear transform algorithm produces a set {w1,w2,...,wN,cN},
 * where each wj is a set of coefficients at scale j, which we call <em>detail
 * layer</em>, and cN is a large-scale smoothed residual, which we call
 * <em>residual layer</em>. Each layer has the same dimensions as the input
 * image, hence the generated multiscale transform is redundant.
 *
 * The algorithm applies successive convolutions with separable filter kernels
 * of increasing size 2*s + 1, where s grows following a monotonically
 * increasing sequence (the dyadic sequence 1, 2, 4, ... is used by default).
 * Multiscale coefficients are the differences between each pair of successive
 * convolved images. By default Gaussian filters are used, but block average
 * filters can also be used (see the class constructor) for special
 * applications.
 *
 * The reconstruction algorithm consists of the sum of all wj multiscale layers
 * for 1 <= j <= N, plus the residual layer cN.
 *
 * In our implementation, each layer in a multiscale linear transform is a
 * floating-point image with the same dimensions as the transformed image.
 * Layers are indexed from 0 to N. Layers at indexes from 0 to N-1 are detail
 * layers, whose elements are actually convolved difference coefficients.
 * Pixels in a detail layer can be negative, zero or positive real values.
 *
 * The last layer, at index N, is the large-scale <em>residual layer</em>.
 * Pixels in the residual layer image can only be positive or zero real values.
 *
 * \ingroup multiscale_transforms
 */
class PCL_CLASS MultiscaleLinearTransform : public RedundantMultiscaleTransform
{
public:

   /*!
    * Represents a multiscale transform layer.
    */
   using layer = RedundantMultiscaleTransform::layer;

   /*!
    * Represents a set of multiscale transform layers, or multiscale transform.
    */
   using transform = RedundantMultiscaleTransform::transform;

   /*!
    * Represents a set of layer enabled/disabled states.
    */
   using layer_state_set = RedundantMultiscaleTransform::layer_state_set;

   /*!
    * Constructs a %MultiscaleLinearTransform instance.
    *
    * \param n    Number of detail layers. The transform will consist of \a n
    *             detail layers plus a residual layer, that is n+1 total
    *             layers. The default value is 4.
    *
    * \param d    Scaling sequence. If \a d <= 0, the transform will use the
    *             dyadic sequence: 1, 2, 4, ... 2^i. If \a d > 0, its value is
    *             the distance in pixels between two successive scales.
    *
    * \param useMeanFilters   If true, the transformation will use block
    *             average filters (mean) instead of Gaussian filters. Mean
    *             filters have important special applications, such as
    *             computation of multiscale local variances. Gaussian filters
    *             are always used by default.
    *
    * The default values for \a n and \a d are 4 and 0, respectively (four
    * layers and the dyadic scaling sequence).
    *
    * Successive layers are computed by applying separable convolutions with
    * kernel filters of size 2*s + 1. The scaling sequence parameter \a d
    * is interpreted as follows:
    *
    * - If the specified sequence parameter \a d is zero 0, then the transform
    *   uses the dyadic sequence: s = 1, 2, 4, ..., 2^j for 0 <= j < n.
    *
    * - If \a d > 0, then \a d is the constant increment in pixels between two
    *   successive scales (linear scaling sequence): s = d*j for 1 <= j < n.
    */
   MultiscaleLinearTransform( int n = 4, int d = 0, bool useMeanFilters = false )
      : RedundantMultiscaleTransform( n, d )
      , m_useMeanFilters( useMeanFilters )
   {
   }

   /*!
    * Copy constructor.
    */
   MultiscaleLinearTransform( const MultiscaleLinearTransform& ) = default;

   /*!
    * Move constructor.
    */
   MultiscaleLinearTransform( MultiscaleLinearTransform&& ) = default;

   /*!
    * Destroys a %MultiscaleLinearTransform object. All existing transform
    * layers are destroyed and deallocated.
    */
   ~MultiscaleLinearTransform() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MultiscaleLinearTransform& operator =( const MultiscaleLinearTransform& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MultiscaleLinearTransform& operator =( MultiscaleLinearTransform&& ) = default;

   /*!
    * Returns true iff this transform applies block average filters instead of
    * Gaussian filters. See the class constructor for more information.
    */
   bool UsesMeanFilters() const
   {
      return m_useMeanFilters;
   }

   /*!
    * Returns true iff this transform applies Gaussian filters instead of block
    * average filters. See the class constructor for more information.
    */
   bool UsesGaussianFilters() const
   {
      return !m_useMeanFilters;
   }

protected:

   /*
    * Whether we should use mean (block average) or Gaussian separable filters.
    */
   bool m_useMeanFilters = false;

   /*
    * Transform (decomposition)
    */
   void Transform( const pcl::Image& ) override;
   void Transform( const pcl::DImage& ) override;
   void Transform( const pcl::ComplexImage& ) override;
   void Transform( const pcl::DComplexImage& ) override;
   void Transform( const pcl::UInt8Image& ) override;
   void Transform( const pcl::UInt16Image& ) override;
   void Transform( const pcl::UInt32Image& ) override;

   friend class MLTDecomposition;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MultiscaleLinearTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/MultiscaleLinearTransform.h - Released 2025-04-07T08:52:44Z
