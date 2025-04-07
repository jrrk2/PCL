//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// DynamicPSFInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __DynamicPSFInstance_h
#define __DynamicPSFInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/PSFFit.h>

#include "DynamicPSFParameters.h"
#include "StarDetector.h"

namespace pcl
{

// ----------------------------------------------------------------------------

typedef GenericPoint<int32>   Point32;

class DynamicPSFInstance : public ProcessImplementation
{
public:

   DynamicPSFInstance( const MetaProcess* );
   DynamicPSFInstance( const DynamicPSFInstance& );

   virtual ~DynamicPSFInstance();

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   void UnlockParameter( const MetaParameter* p, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   void AssignOptions( const DynamicPSFInstance& );

private:

   /*
    * View identifiers
    */
   StringList     p_views;

   /*
    * Stars
    */
   struct Star : public StarData
   {
      uint32 view = 0; // index of the view to which this star pertains

      Star() = default;
      Star( const Star& ) = default;

      Star( const StarData& data, uint32 v )
         : StarData( data )
         , view( v )
      {
      }
   };
   Array<Star>    p_stars;

   /*
    * PSF fittings
    */
   struct PSF : public PSFData
   {
      uint32 star = 0; // index of the fitted star

      // Core API-compatible enumerated and Boolean parameters.
      // Used exclusively for parameter lock/unlock calls.
      pcl_enum p_function   = PSFunction::Invalid;
      pcl_bool p_circular   = false;
      pcl_enum p_status     = PSFFitStatus::NotFitted;
      pcl_bool p_celestial  = false;

      // Synthesized mean signal estimate.
      double   p_meanSignal = 0;

      PSF() = default;
      PSF( const PSF& ) = default;

      PSF( const PSFData& data, uint32 s )
         : PSFData( data )
         , star( s )
         , p_function( data.function )
         , p_circular( data.circular )
         , p_status( data.status )
         , p_celestial( data.celestial )
      {
         if ( signalCount > 0 )
            p_meanSignal = signal/signalCount;
      }
   };
   Array<PSF>     p_psfs;

   /*
    * PSF fitting functions
    */
   struct PSFOptions
   {
      pcl_bool  autoPSF;
      pcl_bool  circular;
      pcl_bool  gaussian;
      pcl_bool  moffat;
      pcl_bool  moffatA;
      pcl_bool  moffat8;
      pcl_bool  moffat6;
      pcl_bool  moffat4;
      pcl_bool  moffat25;
      pcl_bool  moffat15;
      pcl_bool  lorentzian;
      pcl_bool  variableShape;
      pcl_bool  autoVariableShapePSF;
      float     betaMin;
      float     betaMax;

      bool Validate()
      {
         if ( !autoPSF )
            if ( !circular )
               if ( !gaussian )
                  if ( !moffat )
                     if ( !moffatA )
                        if ( !moffat8 )
                           if ( !moffat6 )
                              if ( !moffat4 )
                                 if ( !moffat25 )
                                    if ( !moffat15 )
                                       if ( !lorentzian )
                                          if ( !variableShape )
                                          {
                                             autoPSF = true;
                                             return false;
                                          }
         return true;
      }
   };
   PSFOptions     p_psfOptions;
   pcl_bool       p_signedAngles; // show rotation angles in [0,180] or [-90,+90]
   pcl_bool       p_regenerate;   // do a regeneration or a recalculation in ExecuteGlobal()?
   pcl_bool       p_astrometry;   // compute celestial coordinates using existing astrometric solutions
   int32          p_astrometryPrecision; // number of decimal digits shown for declination (R.A. +1)

   /*
    * Star detection parameters
    */
   int32          p_searchRadius;  // star search radius in pixels
   float          p_threshold;     // background threshold, sigma units
   pcl_bool       p_autoAperture;  // automatic sampling aperture

   /*
    * Image scale parameters
    */
   pcl_enum       p_scaleMode;     // std keyword (FOCALLEN), custom keyword or literal value
   float          p_scaleValue;    // image scale in arc seconds per pixel
   String         p_scaleKeyword;  // custom image scale keyword

   /*
    * Interface options
    */
   uint32         p_starColor;              // drawing color, normal stars
   uint32         p_selectedStarColor;      // drawing color, selected star
   uint32         p_selectedStarFillColor;  // fill color, selected star
   uint32         p_badStarColor;           // drawing color, failed fit
   uint32         p_badStarFillColor;       // filling color, failed fit

   friend class DynamicPSFInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicPSFInstance_h

// ----------------------------------------------------------------------------
// EOF DynamicPSFInstance.h - Released 2025-04-07T08:53:56Z
