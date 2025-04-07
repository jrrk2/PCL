//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// RGBWorkingSpaceInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RGBWorkingSpaceInstance_h
#define __RGBWorkingSpaceInstance_h

#include <pcl/ProcessImplementation.h>

namespace pcl
{

class RGBColorSystem;

// ----------------------------------------------------------------------------

class RGBWorkingSpaceInstance : public ProcessImplementation
{
public:

   RGBWorkingSpaceInstance( const MetaProcess* );
   RGBWorkingSpaceInstance( const RGBWorkingSpaceInstance& );
   RGBWorkingSpaceInstance( const MetaProcess*, const RGBColorSystem& );

   bool Validate( pcl::String& info ) override;
   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View& v, const ImageWindow& mask ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   const FVector& LuminanceCoefficients() const
   {
      return Y;
   }

   const FVector& ChromaticityXCoordinates() const
   {
      return x;
   }

   const FVector& ChromaticityYCoordinates() const
   {
      return y;
   }

   float Gamma() const
   {
      return gamma;
   }

   bool IsSRGB() const
   {
      return sRGB != 0;
   }

   bool AppliesGlobalRGBWS() const
   {
      return applyGlobalRGBWS != 0;
   }

   void NormalizeLuminanceCoefficients()
   {
      Y /= Y.Sum();
   }

private:

   FVector Y, x, y;
   float   gamma;
   uint32  sRGB;             // PCL MetaBoolean maps to uint32
   uint32  applyGlobalRGBWS; // ...

   friend class RGBWorkingSpaceInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RGBWorkingSpaceInstance_h

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceInstance.h - Released 2025-04-07T08:53:55Z
