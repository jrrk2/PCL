//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// GeometryModule.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __GeometryModule_h
#define __GeometryModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class GeometryModule : public MetaModule
{
public:

   GeometryModule();

   const char* Version() const override;
   IsoString Name() const override;
   String Description() const override;
   String Company() const override;
   String Author() const override;
   String Copyright() const override;
   String TradeMarks() const override;
   String OriginalFileName() const override;
   void GetReleaseDate( int& year, int& month, int& day ) const override;
   void OnLoad() override;
};

class PCL_CLASS ImageWindow;

bool WarnOnAstrometryMetadataOrPreviewsOrMask( const ImageWindow&, const IsoString& processId, bool noGUIMessages );
void DeleteAstrometryMetadataAndPreviewsAndMask( ImageWindow&,
                                 bool deleteCenterMetadata, bool deleteScaleMetadata, double pixelSizeScalingFactor = 1 );
void DeleteAstrometryMetadataAndPreviews( ImageWindow&,
                                 bool deleteCenterMetadata, bool deleteScaleMetadata, double pixelSizeScalingFactor = 1 );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GeometryModule_h

// ----------------------------------------------------------------------------
// EOF GeometryModule.h - Released 2025-04-07T08:53:55Z
