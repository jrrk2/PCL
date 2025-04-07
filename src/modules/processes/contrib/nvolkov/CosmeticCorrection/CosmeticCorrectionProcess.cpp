//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.4.1
// ----------------------------------------------------------------------------
// CosmeticCorrectionProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CosmeticCorrectionInstance.h"
#include "CosmeticCorrectionInterface.h"
#include "CosmeticCorrectionParameters.h"
#include "CosmeticCorrectionProcess.h"

namespace pcl
{
// ----------------------------------------------------------------------------

CosmeticCorrectionProcess* TheCosmeticCorrectionProcess = nullptr;

// ----------------------------------------------------------------------------

CosmeticCorrectionProcess::CosmeticCorrectionProcess()
{
   TheCosmeticCorrectionProcess = this;

   new CCTargetFrames( this );
   new CCTargetFrameEnabled( TheTargetFrames );
   new CCTargetFramePath( TheTargetFrames );
   new CCMasterDarkPath( this );
   new CCOutputDir( this );
   new CCOutputExtension( this );
   new CCPrefix( this );
   new CCPostfix( this );
   new CCGenerateHistoryProperties( this );
   new CCOverwrite( this );
   new CCAmount( this );
   new CCCFA( this );
   new CCUseMasterDark( this );
   new CCHotDarkCheck( this );
   new CCHotLevel( this );
   new CCColdDarkCheck( this );
   new CCColdLevel( this );
   new CCUseAutoDetect( this );
   new CCHotAutoCheck( this );
   new CCHotAutoValue( this );
   new CCColdAutoCheck( this );
   new CCColdAutoValue( this );
   new CCUseDefectList( this );
   new CCDefects( this );
   new CCDefectEnabled( TheDefects );
   new CCDefectIsRow( TheDefects );
   new CCDefectAddress( TheDefects );
   new CCDefectIsRange( TheDefects );
   new CCDefectBegin( TheDefects );
   new CCDefectEnd( TheDefects );
}

// ----------------------------------------------------------------------------

IsoString CosmeticCorrectionProcess::Id() const
{
   return "CosmeticCorrection";
}

// ----------------------------------------------------------------------------

IsoString CosmeticCorrectionProcess::Category() const
{
   return "ImageCalibration,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 CosmeticCorrectionProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionProcess::Description() const
{
   return "<html>"
          "<p>The CosmeticCorrection tool replaces defective pixels (hot and cold pixels) "
          "with averaged values from the appropriate neighbor pixels.</p>"
          "<p>The script requires a map of defective pixels or a master dark frame.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/CosmeticCorrection.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* CosmeticCorrectionProcess::DefaultInterface() const
{
   return TheCosmeticCorrectionInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* CosmeticCorrectionProcess::Create() const
{
   return new CosmeticCorrectionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* CosmeticCorrectionProcess::Clone( const ProcessImplementation& p ) const
{
   const CosmeticCorrectionInstance* instPtr = dynamic_cast<const CosmeticCorrectionInstance*>( &p );
   return (instPtr != nullptr) ? new CosmeticCorrectionInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionProcess.cpp - Released 2025-04-07T08:53:56Z
