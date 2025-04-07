//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// CloneStampInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CloneStampInstance_h
#define __CloneStampInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Rectangle.h>
#include <pcl/KernelFilter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class CloneStampInstance : public ProcessImplementation
{
public:

   CloneStampInstance( const MetaProcess* );
   CloneStampInstance( const CloneStampInstance& );

   void Assign( const ProcessImplementation& ) override;
   void TestAssign( const ProcessImplementation& );
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool IsMaskable( const View& v, const ImageWindow& mask ) const override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   struct BrushData
   {
      /*
       * Brush radius. Radius=0 means a single-point brush.
       */
      int32 radius = 5;

      /*
       * Brush softness. From 0=hard to 1=very_soft. 0.5=Gaussian.
       */
      float softness = 0.5F;

      /*
       * Brush opacity. 0=transparent (no action), 1=opaque
       * Opacity multiplies mask items before mixing cloned pixels.
       */
      float opacity = 1.0F;

      BrushData() = default;
      BrushData( const BrushData& ) = default;

      bool operator ==( const BrushData& b ) const
      {
         return radius == b.radius && softness == b.softness && opacity == b.opacity;
      }

      KernelFilter CreateBrush() const;
   };

   struct ClonerData
   {
      uint32   actionIdx = uint32_max; // action index (on the actions array)
      I32Point targetPos; // target location

      ClonerData( const Point& p = Point( 0 ) ) : targetPos( p )
      {
      }

      ClonerData( const ClonerData& ) = default;
   };

   struct ActionData
   {
      String    sourceId;         // source image
      int32     sourceWidth = 0;  // source width in pixels
      int32     sourceHeight = 0; // source height in pixels
      I32Point  sourcePos = 0;    // initial source position
      BrushData brush;            // action brush

      ActionData() = default;
      ActionData( const ActionData& ) = default;
   };

   typedef Array<ClonerData>  cloner_sequence;
   typedef Array<ActionData>  action_sequence;

   action_sequence actions; // cloner actions
   cloner_sequence cloner;  // cloner items

   int32 width = 0;  // target width in pixels
   int32 height = 0; // target height in pixels

   static void Apply( ImageVariant& img, const ImageVariant& src, const ImageVariant& mask, bool maskInverted,
                      const BrushData&, cloner_sequence::const_iterator, const Point& delta );

   /*
    * Interface data
    */
   uint32 color;       // cloner color
   uint32 boundsColor; // bounds color

   // Private flag to work in coordination with CloneStampInterface::Execute()
   bool   isInterfaceInstance = false;

   friend class CloneStampEngine;
   friend class CloneStampInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CloneStampInstance_h

// ----------------------------------------------------------------------------
// EOF CloneStampInstance.h - Released 2025-04-07T08:53:55Z
