//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// MorphologicalTransformationInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "MorphologicalTransformationInstance.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MorphologicalTransformationInstance::MorphologicalTransformationInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , morphologicalOperator( TheMTOperatorParameter->Default )
   , interlacingDistance( uint32( TheMTInterlacingDistanceParameter->DefaultValue() ) )
   , lowThreshold( TheMTLowThresholdParameter->DefaultValue() )
   , highThreshold( TheMTHighThresholdParameter->DefaultValue() )
   , numberOfIterations( uint32( TheMTNumberOfIterationsParameter->DefaultValue() ) )
   , amount( TheMTAmountParameter->DefaultValue() )
   , selectionPoint( TheMTSelectionPointParameter->DefaultValue() )
   , structure( 1, unsigned( TheMTStructureSizeParameter->DefaultValue() ), TheMTStructureNameParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

MorphologicalTransformationInstance::MorphologicalTransformationInstance( const MorphologicalTransformationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void MorphologicalTransformationInstance::Assign( const ProcessImplementation& p )
{
   const MorphologicalTransformationInstance* x = dynamic_cast<const MorphologicalTransformationInstance*>( &p );
   if ( x != nullptr )
   {
      morphologicalOperator = x->morphologicalOperator;
      interlacingDistance = x->interlacingDistance;
      lowThreshold = x->lowThreshold;
      highThreshold = x->highThreshold;
      numberOfIterations = x->numberOfIterations;
      amount = x->amount;
      selectionPoint = x->selectionPoint;
      structure = x->structure;
   }
}

// ----------------------------------------------------------------------------

UndoFlags MorphologicalTransformationInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool MorphologicalTransformationInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "MorphologicalTransformation cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class MTEngine
{
public:

   static
   void Apply( ImageVariant& image, const MorphologicalTransformationInstance& instance )
   {
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: Apply( static_cast<pcl::Image&>( *image ), instance ); break;
         case 64: Apply( static_cast<pcl::DImage&>( *image ), instance ); break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: Apply( static_cast<pcl::UInt8Image&>( *image ), instance ); break;
         case 16: Apply( static_cast<pcl::UInt16Image&>( *image ), instance ); break;
         case 32: Apply( static_cast<pcl::UInt32Image&>( *image ), instance ); break;
         }
   }

private:

   class MTEngineStructure : public StructuringElement
   {
   public:

      MTEngineStructure( const MorphologicalTransformationInstance& instance )
         : StructuringElement( instance.GetStructure().Size(), instance.GetStructure().NumberOfWays() )
         , structure( instance.GetStructure() )
      {
      }

      MTEngineStructure( const MTEngineStructure& ) = default;

      StructuringElement* Clone() const override
      {
         return new MTEngineStructure( *this );
      }

      bool ElementExists( int i, int j, int k ) const override // row, col, way
      {
         return structure.Element( k, j, i );
      }

   private:

      const Structure& structure;
   };

   template <class P> static
   void Apply( GenericImage<P>& image, const MorphologicalTransformationInstance& instance )
   {
      Array<AutoPointer<MorphologicalOperator> > O( 2 );

      switch ( instance.Operator() )
      {
      case MTOperator::Closing:
      case MTOperator::Erosion:   O[0].SetPointer( new ErosionFilter ); break;

      case MTOperator::Opening:
      case MTOperator::Dilation:  O[0].SetPointer( new DilationFilter ); break;

      case MTOperator::Median:    O[0].SetPointer( new MedianFilter ); break;

      case MTOperator::Selection: O[0].SetPointer( new SelectionFilter( instance.SelectionPoint() ) ); break;

      case MTOperator::Midpoint:  O[0].SetPointer( new MidpointFilter ); break;

      default: // ?!
         return;
      }

      switch ( instance.Operator() )
      {
      case MTOperator::Closing:   O[1].SetPointer( new DilationFilter ); break;
      case MTOperator::Opening:   O[1].SetPointer( new ErosionFilter ); break;
      }

      String operatorName;

      switch ( instance.Operator() )
      {
      case MTOperator::Closing:   operatorName = "Closing"; break;
      case MTOperator::Erosion:   operatorName = "Erosion"; break;
      case MTOperator::Opening:   operatorName = "Opening"; break;
      case MTOperator::Dilation:  operatorName = "Dilation"; break;
      case MTOperator::Median:    operatorName = "Median"; break;
      case MTOperator::Selection: operatorName = "Selection"; break;
      case MTOperator::Midpoint:  operatorName = "Midpoint"; break;
      }

      MTEngineStructure S( instance );
      MorphologicalTransformation T;

      T.SetStructure( S );
      T.SetInterlacingDistance( instance.InterlacingDistance() );
      T.SetLowThreshold( instance.LowThreshold() );
      T.SetHighThreshold( instance.HighThreshold() );

      image.Status().Initialize( "Morphological transformation: " + operatorName,
                                 image.NumberOfNominalSamples()*instance.NumberOfIterations() );
      image.Status().DisableInitialization();

      double a = instance.Amount();
      double a0 = 1 - a;

      GenericImage<P> img0;

      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         image.SelectChannel( c );

         for ( int i = 0; i < instance.NumberOfIterations(); ++i )
         {
            if ( a != 1 )
               img0 = image;

            T.SetOperator( **O );
            T >> image;

            if ( O[1] )
            {
               T.SetOperator( *O[1] );
               T >> image;
            }

            if ( a != 1 )
               for ( typename P::sample* f = image[c],
                                       * fN = f + image.NumberOfPixels(),
                                       * f0 = *img0;
                     f < fN; ++f, ++f0 )
               {
                  *f = P::FloatToSample( *f*a + *f0*a0 );
               }
         }
      }
   }
};

// ----------------------------------------------------------------------------

bool MorphologicalTransformationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   MTEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* MorphologicalTransformationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheMTOperatorParameter )
      return &morphologicalOperator;
   if ( p == TheMTInterlacingDistanceParameter )
      return &interlacingDistance;
   if ( p == TheMTLowThresholdParameter )
      return &lowThreshold;
   if ( p == TheMTHighThresholdParameter )
      return &highThreshold;
   if ( p == TheMTNumberOfIterationsParameter )
      return &numberOfIterations;
   if ( p == TheMTAmountParameter )
      return &amount;
   if ( p == TheMTSelectionPointParameter )
      return &selectionPoint;
   if ( p == TheMTStructureNameParameter )
      return structure.name.Begin();
   if ( p == TheMTStructureSizeParameter )
      return &structure.size;
   if ( p == TheMTStructureWayMaskParameter )
      return structure.mask[tableRow].Begin();
   return nullptr;
}

// ----------------------------------------------------------------------------

bool MorphologicalTransformationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheMTStructureNameParameter )
   {
      structure.name.Clear();
      if ( sizeOrLength > 0 )
         structure.name.SetLength( sizeOrLength );
   }
   else if ( p == TheMTStructureWayTableParameter )
   {
      structure.mask.Clear();
      if ( sizeOrLength > 0 )
         structure.mask.Add( ByteArray(), sizeOrLength );
   }
   else if ( p == TheMTStructureWayMaskParameter )
   {
      structure.mask[tableRow].Clear();
      if ( sizeOrLength > 0 )
         structure.mask[tableRow].Add( 0, sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type MorphologicalTransformationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheMTStructureNameParameter )
      return structure.name.Length();
   if ( p == TheMTStructureWayTableParameter )
      return structure.mask.Length();
   if ( p == TheMTStructureWayMaskParameter )
      return structure.mask[tableRow].Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationInstance.cpp - Released 2025-04-07T08:53:56Z
