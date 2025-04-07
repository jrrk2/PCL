//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// Ephemerides.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __Ephemerides_h
#define __Ephemerides_h

#include "EphemerisGeneratorInstance.h"

#include <pcl/EphemerisFile.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Fundamental and massive asteroid ephemerides.
 */
class Ephemerides
{
public:

   Ephemerides( const EphemerisGeneratorInstance&, bool fundamentalOnly = false );

   virtual ~Ephemerides();

   /*
    * Fundamental ephemerides.
    */
   EphemerisFile& Fundamental() const
   {
      return *m_eph;
   }

   /*
    * Massive asteroid ephemerides.
    */
   EphemerisFile& Asteroids() const
   {
      return *m_ast;
   }

   /*
    * Massive Kuiper Belt object (KBO) ephemerides.
    */
   EphemerisFile& KBOs() const
   {
      return *m_kbo;
   }

   /*
    * The solar gravitational mass parameter, acquired directly from
    * fundamental ephemerides.
    */
   double GMS() const;

   /*
    * Total time span covered by the set of installed ephemerides.
    */
   TimePoint StartTime() const;
   TimePoint EndTime() const;

   /*
    * KBO mass parameters from DE440.
    */
   static double KBOMass( const EphemerisObject& );

private:

   EphemerisFile* m_eph = nullptr; // fundamental ephemerides
   EphemerisFile* m_ast = nullptr; // asteroid ephemerides
   EphemerisFile* m_kbo = nullptr; // KBO ephemerides
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Ephemerides_h

// ----------------------------------------------------------------------------
// EOF Ephemerides.h - Released 2025-04-07T08:53:55Z
