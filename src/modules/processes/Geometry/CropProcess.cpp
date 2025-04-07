//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// CropProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CropProcess.h"
#include "CropParameters.h"
#include "CropInstance.h"
#include "CropInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CropProcess* TheCropProcess = nullptr;

// ----------------------------------------------------------------------------

CropProcess::CropProcess()
{
   TheCropProcess = this;

   new CRLeftMargin( this );
   new CRTopMargin( this );
   new CRRightMargin( this );
   new CRBottomMargin( this );
   TheCRModeParameter = new CRMode( this );
   TheCRXResolutionParameter = new XResolution( this );
   TheCRYResolutionParameter = new YResolution( this );
   TheCRMetricResolutionParameter = new MetricResolution( this );
   TheCRForceResolutionParameter = new ForceResolution( this );
   TheCRFillRedParameter = new FillRed( this );
   TheCRFillGreenParameter = new FillGreen( this );
   TheCRFillBlueParameter = new FillBlue( this );
   TheCRFillAlphaParameter = new FillAlpha( this );
   TheCRNoGUIMessagesParameter = new NoGUIMessages( this );
}

// ----------------------------------------------------------------------------

IsoString CropProcess::Id() const
{
   return "Crop";
}

// ----------------------------------------------------------------------------

IsoString CropProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 CropProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CropProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String CropProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Crop.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* CropProcess::DefaultInterface() const
{
   return TheCropInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* CropProcess::Create() const
{
   return new CropInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* CropProcess::Clone( const ProcessImplementation& p ) const
{
   const CropInstance* instPtr = dynamic_cast<const CropInstance*>( &p );
   return (instPtr != nullptr) ? new CropInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropProcess.cpp - Released 2025-04-07T08:53:55Z
