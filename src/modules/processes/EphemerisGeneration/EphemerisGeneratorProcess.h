//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorProcess.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __EphemerisGeneratorProcess_h
#define __EphemerisGeneratorProcess_h

#include <pcl/MetaProcess.h>

#include "TextDatabase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class EphemerisGeneratorProcess : public MetaProcess
{
public:

   EphemerisGeneratorProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;

   static void InitializeDatabaseFormats();
   static bool HasValidDatabaseFormats();
   static const Array<TextDatabase::FormatDescription>& DatabaseFormats();
   static const TextDatabase::FormatDescription& DefaultDatabaseFormat();
   static int FindDatabaseFormatByName( const String& );
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern EphemerisGeneratorProcess* TheEphemerisGeneratorProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __EphemerisGeneratorProcess_h

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorProcess.h - Released 2025-04-07T08:53:55Z
