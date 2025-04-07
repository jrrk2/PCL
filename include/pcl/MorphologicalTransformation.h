//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MorphologicalTransformation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_MorphologicalTransformation_h
#define __PCL_MorphologicalTransformation_h

/// \file pcl/MorphologicalTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AutoPointer.h>
#include <pcl/InterlacedTransformation.h>
#include <pcl/MorphologicalOperator.h>
#include <pcl/ParallelProcess.h>
#include <pcl/StructuringElement.h>
#include <pcl/ThresholdedTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MorphologicalTransformation
 * \brief Generic n-way morphological transformation.
 *
 * ### TODO: Write a detailed description for %MorphologicalTransformation.
 *
 * \sa MorphologicalOperator, StructuringElement
 */
class PCL_CLASS MorphologicalTransformation : public InterlacedTransformation,
                                              public ThresholdedTransformation,
                                              public ParallelProcess
{                          // N.B. ImageTransformation is a virtual base class
public:

   /*!
    * Constructs a default %MorphologicalTransformation object.
    *
    * \note This constructor creates an uninitialized instance. In order to use
    * this object, it must be associated with a particular operator and
    * structuring element by calling the SetOperator() and SetStructure()
    * member functions, respectively, with the appropriate instances of
    * MorphologicalOperator and StructuringElement.
    */
   MorphologicalTransformation() = default;

   /*!
    * Constructs a %MorphologicalTransformation object with the specified
    * operator and structuring element.
    *
    * The specified objects don't have to remain valid while this
    * %MorphologicalTransformation instance is actively used, since it will
    * own private copies of the specified filter operator and structure.
    */
   MorphologicalTransformation( const MorphologicalOperator& op, const StructuringElement& structure )
   {
      m_operator = op.Clone();
      m_structure = structure.Clone();
      m_structure->Initialize();
   }

   /*!
    * Copy constructor.
    */
   MorphologicalTransformation( const MorphologicalTransformation& x )
      : InterlacedTransformation( x )
      , ThresholdedTransformation( x )
      , ParallelProcess( x )
   {
      if ( !x.m_operator.IsNull() )
         if ( !x.m_structure.IsNull() )
         {
            m_operator = x.m_operator->Clone();
            m_structure = x.m_structure->Clone();
         }
   }

   /*!
    * Move constructor.
    */
   MorphologicalTransformation( MorphologicalTransformation&& x ) = default;

   /*!
    * Destroys a %MorphologicalTransformation object.
    */
   ~MorphologicalTransformation() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MorphologicalTransformation& operator =( const MorphologicalTransformation& x )
   {
      if ( &x != this )
      {
         (void)InterlacedTransformation::operator =( x );
         (void)ThresholdedTransformation::operator =( x );
         (void)ParallelProcess::operator =( x );
         if ( !x.m_operator.IsNull() && !x.m_structure.IsNull() )
         {
            m_operator = x.m_operator->Clone();
            m_structure = x.m_structure->Clone();
         }
         else
         {
            m_operator.Destroy();
            m_structure.Destroy();
         }
      }
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MorphologicalTransformation& operator =( MorphologicalTransformation&& ) = default;

   /*!
    * Returns a reference to the morphological operator associated with this
    * transformation.
    */
   const MorphologicalOperator& Operator() const
   {
      PCL_PRECONDITION( !m_operator.IsNull() )
      return *m_operator;
   }

   /*!
    * Causes this transformation to use a duplicate of the specified
    * morphological operator.
    */
   void SetOperator( const MorphologicalOperator& op )
   {
      m_operator = op.Clone();
   }

   /*!
    * Returns a reference to the structuring element associated with this
    * transformation.
    */
   const StructuringElement& Structure() const
   {
      PCL_PRECONDITION( !m_structure.IsNull() )
      return *m_structure;
   }

   /*!
    * Causes this transformation to use a duplicate of the specified
    * structuring element.
    */
   void SetStructure( const StructuringElement& structure )
   {
      m_structure = structure.Clone();
      m_structure->Initialize();
   }

   /*!
    * Returns the size in pixels of the overlapping regions between adjacent
    * areas processed by parallel execution threads. The overlapping distance
    * is a function of the size of the associated structuring element and the
    * current interlacing distance.
    */
   int OverlappingDistance() const
   {
      PCL_PRECONDITION( !m_structure.IsNull() )
      return m_structure->Size() + (m_structure->Size() - 1)*(InterlacingDistance() - 1);
   }

protected:

   AutoPointer<MorphologicalOperator> m_operator;  // morphological operator
   AutoPointer<StructuringElement>    m_structure; // n-way structuring element

   /*
    * In-Place 2-D Morphological Transformation Algorithm.
    */
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;

private:

   void Validate() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MorphologicalTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/MorphologicalTransformation.h - Released 2025-04-07T08:52:44Z
