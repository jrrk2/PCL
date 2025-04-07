//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorCache.cpp - Released 2025-04-07T08:53:56Z
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

#include "SubframeSelectorCache.h"

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorCache* TheSubframeSelectorCache = nullptr;

// ----------------------------------------------------------------------------

void SubframeSelectorCacheItem::AssignData( const FileDataCacheItem& item )
{
#define src static_cast<const SubframeSelectorCacheItem&>( item )
   fwhm                  = src.fwhm;
   fwhmMeanDev           = src.fwhmMeanDev;
   eccentricity          = src.eccentricity;
   eccentricityMeanDev   = src.eccentricityMeanDev;
   psfSignalWeight       = src.psfSignalWeight;
   psfSNR                = src.psfSNR;
   psfScale              = src.psfScale;
   psfScaleSNR           = src.psfScaleSNR;
   psfFlux               = src.psfFlux;
   psfFluxPower          = src.psfFluxPower;
   psfTotalMeanFlux      = src.psfTotalMeanFlux;
   psfTotalMeanPowerFlux = src.psfTotalMeanPowerFlux;
   psfCount              = src.psfCount;
   MStar                 = src.MStar;
   NStar                 = src.NStar;
   snrWeight             = src.snrWeight;
   median                = src.median;
   medianMeanDev         = src.medianMeanDev;
   noise                 = src.noise;
   noiseRatio            = src.noiseRatio;
   stars                 = src.stars;
   starResidual          = src.starResidual;
   starResidualMeanDev   = src.starResidualMeanDev;
   azimuth               = src.azimuth;
   altitude              = src.altitude;
#undef src
}

// ----------------------------------------------------------------------------

IsoStringList SubframeSelectorCacheItem::SerializedData() const
{
   IsoStringList tokens;
   tokens << "fwhm"                  << IsoString().Format( "%.4f", fwhm )
          << "fwhmMeanDev"           << IsoString().Format( "%.4e", fwhmMeanDev )
          << "eccentricity"          << IsoString().Format( "%.4f", eccentricity )
          << "eccentricityMeanDev"   << IsoString().Format( "%.4e", eccentricityMeanDev )
          << "psfSignalWeight"       << IsoString().Format( "%.4e", psfSignalWeight )
          << "psfSNR"                << IsoString().Format( "%.4e", psfSNR )
          << "psfScale"              << IsoString().Format( "%.4e", psfScale )
          << "psfScaleSNR"           << IsoString().Format( "%.4e", psfScaleSNR )
          << "psfFlux"               << IsoString().Format( "%.4e", psfFlux )
          << "psfFluxPower"          << IsoString().Format( "%.4e", psfFluxPower )
          << "psfTotalMeanFlux"      << IsoString().Format( "%.4e", psfTotalMeanFlux )
          << "psfTotalMeanPowerFlux" << IsoString().Format( "%.4e", psfTotalMeanPowerFlux )
          << "psfCount"              << IsoString().Format( "%u", psfCount )
          << "MStar"                 << IsoString().Format( "%.4e", MStar )
          << "NStar"                 << IsoString().Format( "%.4e", NStar )
          << "snrWeight"             << IsoString().Format( "%.4e", snrWeight )
          << "median"                << IsoString().Format( "%.4e", median )
          << "medianMeanDev"         << IsoString().Format( "%.4e", medianMeanDev )
          << "noise"                 << IsoString().Format( "%.4e", noise )
          << "noiseRatio"            << IsoString().Format( "%.4f", noiseRatio )
          << "stars"                 << IsoString().Format( "%u", stars )
          << "starResidual"          << IsoString().Format( "%.4e", starResidual )
          << "starResidualMeanDev"   << IsoString().Format( "%.4e", starResidualMeanDev )
          << "azimuth"               << IsoString().Format( "%.4f", azimuth )
          << "altitude"              << IsoString().Format( "%.4f", altitude );
   return tokens;
}

// ----------------------------------------------------------------------------

bool SubframeSelectorCacheItem::DeserializeData( const IsoStringList& tokens )
{
   AssignData( SubframeSelectorCacheItem() );

   for ( IsoStringList::const_iterator i = tokens.Begin(); i != tokens.End(); )
      if ( *i == "fwhm" )
      {
         if ( !(++i)->TryToDouble( fwhm ) )
            return false;
      }
      else if ( *i == "fwhmMeanDev" )
      {
         if ( !(++i)->TryToDouble( fwhmMeanDev ) )
            return false;
      }
      else if ( *i == "eccentricity" )
      {
         if ( !(++i)->TryToDouble( eccentricity ) )
            return false;
      }
      else if ( *i == "eccentricityMeanDev" )
      {
         if ( !(++i)->TryToDouble( eccentricityMeanDev ) )
            return false;
      }
      else if ( *i == "psfSignalWeight" )
      {
         if ( !(++i)->TryToDouble( psfSignalWeight ) )
            return false;
      }
      else if ( *i == "psfSNR" )
      {
         if ( !(++i)->TryToDouble( psfSNR ) )
            return false;
      }
      else if ( *i == "psfScale" )
      {
         if ( !(++i)->TryToDouble( psfScale ) )
            return false;
      }
      else if ( *i == "psfScaleSNR" )
      {
         if ( !(++i)->TryToDouble( psfScaleSNR ) )
            return false;
      }
      else if ( *i == "psfFlux" )
      {
         if ( !(++i)->TryToDouble( psfFlux ) )
            return false;
      }
      else if ( *i == "psfFluxPower" )
      {
         if ( !(++i)->TryToDouble( psfFluxPower ) )
            return false;
      }
      else if ( *i == "psfTotalMeanFlux" )
      {
         if ( !(++i)->TryToDouble( psfTotalMeanFlux ) )
            return false;
      }
      else if ( *i == "psfTotalMeanPowerFlux" )
      {
         if ( !(++i)->TryToDouble( psfTotalMeanPowerFlux ) )
            return false;
      }
      else if ( *i == "psfCount" )
      {
         if ( !(++i)->TryToUInt( psfCount ) )
            return false;
      }
      else if ( *i == "MStar" )
      {
         if ( !(++i)->TryToDouble( MStar ) )
            return false;
      }
      else if ( *i == "NStar" )
      {
         if ( !(++i)->TryToDouble( NStar ) )
            return false;
      }
      else if ( *i == "snrWeight" )
      {
         if ( !(++i)->TryToDouble( snrWeight ) )
            return false;
      }
      else if ( *i == "median" )
      {
         if ( !(++i)->TryToDouble( median ) )
            return false;
      }
      else if ( *i == "medianMeanDev" )
      {
         if ( !(++i)->TryToDouble( medianMeanDev ) )
            return false;
      }
      else if ( *i == "noise" )
      {
         if ( !(++i)->TryToDouble( noise ) )
            return false;
      }
      else if ( *i == "noiseRatio" )
      {
         if ( !(++i)->TryToDouble( noiseRatio ) )
            return false;
      }
      else if ( *i == "stars" )
      {
         if ( !(++i)->TryToUInt( stars ) )
            return false;
      }
      else if ( *i == "starResidual" )
      {
         if ( !(++i)->TryToDouble( starResidual ) )
            return false;
      }
      else if ( *i == "starResidualMeanDev" )
      {
         if ( !(++i)->TryToDouble( starResidualMeanDev ) )
            return false;
      }
      else if ( *i == "azimuth" )
      {
         if ( !(++i)->TryToDouble( azimuth ) )
            return false;
      }
      else if ( *i == "altitude" )
      {
         if ( !(++i)->TryToDouble( altitude ) )
            return false;
      }
      else
      {
         ++i;
      }

   return true;
}

// ----------------------------------------------------------------------------

SubframeSelectorCache::SubframeSelectorCache()
   : FileDataCache( "SubframeSelector" )
{
   if ( TheSubframeSelectorCache == nullptr )
      TheSubframeSelectorCache = this;

   int maxItemDuration = 30;
   Settings::Read( "/Cache/MaxItemDuration", maxItemDuration );
   SetMaxItemDuration( maxItemDuration );
   Settings::Read( "/Cache/Persistent", m_persistent );
}

// ----------------------------------------------------------------------------

SubframeSelectorCache::~SubframeSelectorCache()
{
   if ( TheSubframeSelectorCache == this )
      TheSubframeSelectorCache = nullptr;
}

// ----------------------------------------------------------------------------

void SubframeSelectorCache::SetPersistent( bool persistent )
{
   if ( persistent != m_persistent )
   {
      if ( persistent )
         if ( IsEmpty() )
            Load();

      m_persistent = persistent;
      Settings::Write( "/Cache/Persistent", m_persistent );
   }
}

// ----------------------------------------------------------------------------

void SubframeSelectorCache::SetMaxItemDuration( int days )
{
   if ( days != MaxItemDuration() )
   {
      FileDataCache::SetMaxItemDuration( days );
      Settings::Write( "/Cache/MaxItemDuration", MaxItemDuration() );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorCache.cpp - Released 2025-04-07T08:53:56Z
