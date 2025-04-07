//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ReadoutOptionsInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ReadoutOptionsInstance_h
#define __ReadoutOptionsInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/ReadoutOptions.h>

#include "ReadoutOptionsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ReadoutOptionsInstance : public ProcessImplementation
{
public:

   ReadoutOptionsInstance( const MetaProcess* );
   ReadoutOptionsInstance( const ReadoutOptionsInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool CanExecuteGlobal( pcl::String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;
   bool ValidateParameter( void* value, const MetaParameter*, size_type tableRow ) const override;

   pcl::ReadoutOptions Options() const
   {
      ReadoutOptions o;
      o.SetData( ReadoutOptions::readout_data( data ) );
      o.SetMode( ReadoutOptions::readout_mode( mode ) );
      o.SetProbeSize( probeSize );
      o.SetPreviewSize( previewSize );
      o.SetPreviewZoomFactor( previewZoom );
      o.SetPrecision( precision );
      o.SetIntegerRange( range );
      o.EnableAlphaChannel( showAlpha );
      o.EnableMaskChannel( showMask );
      o.EnablePreview( showPreview );
      o.EnablePreviewCenter( previewCenter );
      o.EnableEquatorialCoordinates( showEquatorial );
      o.EnableEclipticCoordinates( showEcliptic );
      o.EnableGalacticCoordinates( showGalactic );
      o.SetCoordinateItems( coordinateItems );
      o.SetCoordinatePrecision( coordinatePrecision );
      o.EnableBroadcast( broadcast );
      o.SetReal( real );
      return o;
   }

   void SetOptions( const pcl::ReadoutOptions& o )
   {
      data = o.Data();
      mode = o.Mode();
      probeSize = o.ProbeSize();
      previewSize = o.PreviewSize();
      previewZoom = o.PreviewZoomFactor();
      precision = o.Precision();
      range = o.IntegerRange();
      showAlpha = o.ShowAlphaChannel();
      showMask = o.ShowMaskChannel();
      showPreview = o.ShowPreview();
      previewCenter = o.ShowPreviewCenter();
      showEquatorial = o.ShowEquatorialCoordinates();
      showEcliptic = o.ShowEclipticCoordinates();
      showGalactic = o.ShowGalacticCoordinates();
      coordinateItems = o.CoordinateItems();
      coordinatePrecision = o.CoordinatePrecision();
      broadcast = o.IsBroadcastEnabled();
      real = o.IsReal();
   }

   void LoadCurrentOptions();

private:

   pcl_enum data;
   pcl_enum mode;
   int32    probeSize;           // size of the square probe - must be an odd number
   int32    previewSize;         // size of the square preview - must be an odd number
   int32    previewZoom;         // readout preview zoom factor >= 1
   int32    precision;           // number of decimal digits if real==true
   uint32   range;               // maximum discrete value if real==false
   pcl_bool showAlpha;           // show alpha channel readouts?
   pcl_bool showMask;            // show mask channel readouts?
   pcl_bool showPreview;         // show real-time readout previews?
   pcl_bool previewCenter;       // plot center hairlines on real-time readout previews?
   pcl_bool showEquatorial;      // show equatorial coordinates
   pcl_bool showEcliptic;        // show ecliptic coordinates
   pcl_bool showGalactic;        // show galactic coordinates
   int32    coordinateItems;     // 1=degrees/hours 2=minutes 3=seconds
   int32    coordinatePrecision; // decimal digits of last represented item
   pcl_bool broadcast;           // broadcast readouts?
   pcl_bool real;                // true=real, false=integer
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ReadoutOptionsInstance_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsInstance.h - Released 2025-04-07T08:53:56Z
