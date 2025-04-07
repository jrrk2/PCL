//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorCache.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2025 Cameron Leger
// Copyright (c) 2020-2025 Juan Conejero, PTeam
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SubframeSelectorCache_h
#define __SubframeSelectorCache_h

#include <pcl/FileDataCache.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorCacheItem : public FileDataCacheItem
{
public:

   double    fwhm;
   double    fwhmMeanDev;
   double    eccentricity;
   double    eccentricityMeanDev;
   double    psfSignalWeight;
   double    psfSNR;
   double    psfScale;
   double    psfScaleSNR;
   double    psfFlux;
   double    psfFluxPower;
   double    psfTotalMeanFlux;
   double    psfTotalMeanPowerFlux;
   unsigned  psfCount;
   double    MStar;
   double    NStar;
   double    snrWeight;
   double    median;
   double    medianMeanDev;
   double    noise;
   double    noiseRatio;
   unsigned  stars;
   double    starResidual;
   double    starResidualMeanDev;
   double    azimuth;
   double    altitude;

   SubframeSelectorCacheItem( const String& path = String(), const IsoString& key = IsoString() )
      : FileDataCacheItem( path, key )
   {
   }

private:

   void AssignData( const FileDataCacheItem& ) override;
   IsoStringList SerializedData() const override;
   bool DeserializeData( const IsoStringList& ) override;
};

// ----------------------------------------------------------------------------

class SubframeSelectorCache : public FileDataCache
{
public:

   SubframeSelectorCache();
   virtual ~SubframeSelectorCache();

   String CacheName() const override
   {
      return "SubframeSelector Cache";
   }

   int Version() const override
   {
      return 21;
   }

   int MinSupportedVersion() const override
   {
      return 21;
   }

   bool IsPersistent() const
   {
      return m_persistent;
   }

   void SetPersistent( bool );

   void SetMaxItemDuration( int days ) override;

private:

   bool m_persistent = true;

   FileDataCacheItem* NewItem() const override
   {
      return new SubframeSelectorCacheItem;
   }
};

extern SubframeSelectorCache* TheSubframeSelectorCache;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SubframeSelectorCache_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorCache.h - Released 2025-04-07T08:53:56Z
