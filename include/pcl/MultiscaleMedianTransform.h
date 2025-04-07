//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MultiscaleMedianTransform.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_MultiscaleMedianTransform_h
#define __PCL_MultiscaleMedianTransform_h

/// \file pcl/MultiscaleMedianTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/RedundantMultiscaleTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MultiscaleMedianTransform
 * \brief Multiscale median transform / hybrid median-wavelet transform.
 *
 * The multiscale median transform algorithm produces a set {w1,w2,...,wN,cN},
 * where each wj is a set of coefficients at scale j, which we call <em>detail
 * layer</em>, and cN is a large-scale smoothed residual, which we call
 * <em>residual layer</em>. Each layer has the same dimensions as the input
 * image, hence the generated multiscale transform is redundant.
 *
 * The algorithm applies successive median filters with a structuring element
 * of increasing size 2*s + 1, where s grows following a monotonically
 * increasing sequence (the dyadic sequence 1, 2, 4, ... is used by default).
 * Multiscale coefficients are the differences between each pair of successive
 * median filtered images.
 *
 * The hybrid median-wavelet transform merges the multiscale median and wavelet
 * transforms in a single high-level operation. Wavelets are used to represent
 * nonsignificant structures, such as noise and smooth regions, while median
 * filtering is used to represent strong significant structures. This hybrid
 * transform provides an optimal representation of the image by combining the
 * strongest points of both techniques: wavelets are good to support smooth
 * structures with weak variations, while the median transform is better at
 * isolating significant, high-contrast structures.
 *
 * The reconstruction algorithm consists of the sum of all wj multiscale layers
 * for 1 <= j <= N, plus the residual layer cN.
 *
 * <b>References</b>
 *
 * \li Starck, J.-L., Murtagh, F. and J. Fadili, A. (2010), <em>Sparse %Image
 * and Signal Processing: Wavelets, Curvelets, Morphological Diversity</em>,
 * Cambridge University Press.
 *
 * \li Barth, Timothy J., Chan, Tony, Haimes, Robert (Eds.) (2002),
 * <em>Multiscale and Multiresolution Methods: Theory and Applications</em>,
 * Springer. invited paper: Jean-Luc Starck, <em>Nonlinear Multiscale
 * Transforms</em>, pp. 239-279.
 *
 * In our implementation, each layer in a multiscale median transform is a
 * floating-point image with the same dimensions as the transformed image.
 * Layers are indexed from 0 to N. Layers at indexes from 0 to N-1 are detail
 * layers, whose elements are actually median difference coefficients. Pixels
 * in a multiscale layer can be negative, zero or positive real values.
 *
 * The last layer, at index N, is the large-scale residual layer. Pixels in the
 * residual layer image can only be positive or zero real values.
 *
 * The original algorithm uses square structuring elements. Square structures
 * lead to relatively simple and efficient implementations, but unfortunately
 * they tend to generate objectionable artifacts around round shapes. In our
 * implementation we use special multiway structures to minimize these
 * artifacts and to improve the behavior of the algorithm to isolate isotropic
 * image structures.
 *
 * \ingroup multiscale_transforms
 */
class PCL_CLASS MultiscaleMedianTransform : public RedundantMultiscaleTransform
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
    * Constructs a %MultiscaleMedianTransform instance.
    *
    * \param n    Number of detail layers. The transform will consist of \a n
    *             detail layers plus a residual layer, that is n+1 total
    *             layers. The default value is 4.
    *
    * \param d    Scaling sequence. If \a d <= 0, the transform will use the
    *             dyadic sequence: 1, 2, 4, ... 2^i. If \a d > 0, its value is
    *             the distance in pixels between two successive scales.
    *
    * The default values for \a n and \a d are 4 and 0, respectively (four
    * layers and the dyadic scaling sequence).
    *
    * Successive layers are computed by applying median filters with
    * structuring elements of size 2*s + 1. The scaling sequence parameter \a d
    * is interpreted as follows:
    *
    * - If the specified sequence parameter \a d is zero 0, then the transform
    *   uses the dyadic sequence: s = 1, 2, 4, ..., 2^j for 0 <= j < n.
    *
    * - If \a d > 0, then \a d is the constant increment in pixels between two
    *   successive scales (linear scaling sequence): s = d*j for 1 <= j < n.
    */
   MultiscaleMedianTransform( int n = 4, int d = 0 )
      : RedundantMultiscaleTransform( n, d )
   {
   }

   /*!
    * Copy constructor.
    */
   MultiscaleMedianTransform( const MultiscaleMedianTransform& ) = default;

   /*!
    * Move constructor.
    */
   MultiscaleMedianTransform( MultiscaleMedianTransform&& ) = default;

   /*!
    * Destroys a %MultiscaleMedianTransform object. All existing transform
    * layers are destroyed and deallocated.
    */
   ~MultiscaleMedianTransform() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MultiscaleMedianTransform& operator =( const MultiscaleMedianTransform& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MultiscaleMedianTransform& operator =( MultiscaleMedianTransform&& ) = default;

   /*!
    * Returns true if this transform applies special multiway structuring
    * elements for improved isotropic behavior. Returns false if simple
    * structures are used instead for improved execution speed, at the cost of
    * some performance degradation in the isotropic behavior of the transform.
    */
   bool UsingMultiwayStructures() const
   {
      return m_multiwayStructures;
   }

   /*!
    * Enables the use of multiway structuring elements. See
    * UsingMultiwayStructures() for more information.
    *
    * \note Calling this member function implicitly deletes all existing
    * transform layers.
    */
   void EnableMultiwayStructures( bool enable = true )
   {
      DestroyLayers();
      m_multiwayStructures = enable;
   }

   /*!
    * Disables the use of multiway structuring elements. See
    * UsingMultiwayStructures() for more information.
    *
    * \note Calling this member function implicitly deletes all existing
    * transform layers.
    */
   void DisableMultiwayStructures( bool disable = true )
   {
      EnableMultiwayStructures( !disable );
   }

   /*!
    * Returns true iff this object performs a hybrid median-wavelet transform.
    * Returns false if this is a pure multiscale median transform.
    */
   bool IsMedianWaveletTransform() const
   {
      return m_medianWaveletTransform;
   }

   /*!
    * Causes this object to perform a hybrid wavelet-median transform.
    *
    * \param threshold  Threshold in sigma units for per-layer suppression of
    *                   significant median transform coefficients. Image
    *                   structures represented by median coefficients with
    *                   absolute values smaller than this threshold will be
    *                   supported by wavelet transform coefficients. The
    *                   default value is 5 sigma.
    *
    * \note Calling this member function implicitly deletes all existing
    * transform layers.
    */
   void SetMedianWaveletTransform( float threshold = 5 )
   {
      DestroyLayers();
      m_medianWaveletTransform = true;
      m_medianWaveletThreshold = Max( 0.F, threshold );
   }

   /*!
    * Sets a threshold in sigma units for per-layer suppression of significant
    * median transform coefficients. Image structures represented by median
    * coefficients with absolute values smaller than this threshold will be
    * supported by wavelet transform coefficients. The larger this value, the
    * more image structures will be supported by wavelet coefficients.
    *
    * By default the median-wavelet threshold is 5 sigma. This is normally
    * large enough to prevent inclusion of the noise in median transform
    * coefficients.
    *
    * \note Calling this member function implicitly deletes all existing
    * transform layers.
    */
   void SetMedianWaveletThreshold( float threshold )
   {
      DestroyLayers();
      m_medianWaveletThreshold = Max( 0.F, threshold );
   }

   /*!
    * Returns the current median-wavelet threshold in sigma units. See the
    * documentation for SetMedianWaveletThreshold() for more information.
    */
   float MedianWaveletThreshold() const
   {
      return m_medianWaveletThreshold;
   }

   /*!
    * Causes this object to perform a pure multiscale median transform.
    *
    * \note Calling this member function implicitly deletes all existing
    * transform layers.
    */
   void SetMultiscaleMedianTransform()
   {
      DestroyLayers();
      m_medianWaveletTransform = false;
   }

private:

   /*
    * Use multiway structural elements for improved isotropy.
    */
   bool m_multiwayStructures = true;

   /*
    * Compute a wavelet-median transform.
    */
   bool m_medianWaveletTransform = false;

   /*
    * Median-wavelet threshold in sigma units.
    */
   float m_medianWaveletThreshold = 5.0F;

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

   friend class MMTDecomposition;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MultiscaleMedianTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/MultiscaleMedianTransform.h - Released 2025-04-07T08:52:44Z
