//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ReadoutOptions.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ReadoutOptions.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptions ReadoutOptions::GetCurrentOptions()
{
   api_readout_options a;
   (*API->Global->GetReadoutOptions)( &a );

   ReadoutOptions o;
   o.data                = ReadoutOptions::readout_data( a.data );
   o.mode                = ReadoutOptions::readout_mode( a.mode );
   o.probeSize           = a.probeSize;
   o.previewSize         = a.previewSize;
   o.previewZoom         = a.previewZoom;
   o.precision           = a.precision;
   o.range               = a.range;
   o.showAlpha           = a.showAlpha;
   o.showMask            = a.showMask;
   o.showPreview         = a.showPreview;
   o.previewCenter       = a.previewCenter;
   o.showEquatorial      = a.showEquatorial;
   o.showEcliptic        = a.showEcliptic;
   o.showGalactic        = a.showGalactic;
   o.coordinateItems     = a.coordinateItems;
   o.coordinatePrecision = a.coordinatePrecision;
   o.broadcast           = a.broadcast;
   o.real                = a.real;

   return o;
}

// ----------------------------------------------------------------------------

void ReadoutOptions::SetCurrentOptions( const ReadoutOptions& o )
{
   api_readout_options a;
   a.data                = int32( o.data );
   a.mode                = int32( o.mode );
   a.probeSize           = o.probeSize;
   a.previewSize         = o.previewSize;
   a.previewZoom         = o.previewZoom;
   a.precision           = o.precision;
   a.range               = o.range;
   a.showAlpha           = o.showAlpha;
   a.showMask            = o.showMask;
   a.showPreview         = o.showPreview;
   a.previewCenter       = o.previewCenter;
   a.showEquatorial      = o.showEquatorial;
   a.showEcliptic        = o.showEcliptic;
   a.showGalactic        = o.showGalactic;
   a.coordinateItems     = o.coordinateItems;
   a.coordinatePrecision = o.coordinatePrecision;
   a.broadcast           = o.broadcast;
   a.real                = o.real;
   a.__r__               = 0;

   (*API->Global->SetReadoutOptions)( &a );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ReadoutOptions.cpp - Released 2025-04-07T08:53:32Z
