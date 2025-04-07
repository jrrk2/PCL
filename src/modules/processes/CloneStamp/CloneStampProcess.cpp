//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// CloneStampProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CloneStampInstance.h"
#include "CloneStampInterface.h"
#include "CloneStampParameters.h"
#include "CloneStampProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CloneStampProcess* TheCloneStampProcess = nullptr;

// ----------------------------------------------------------------------------

CloneStampProcess::CloneStampProcess()
{
   TheCloneStampProcess = this;

   new CSClonerTable( this );
   new CSActionIndex( TheCSClonerTableParameter );
   new CSTargetX( TheCSClonerTableParameter );
   new CSTargetY( TheCSClonerTableParameter );

   new CSActionTable( this );
   new CSSourceIdentifier( TheCSActionTableParameter );
   new CSSourceWidth( TheCSActionTableParameter );
   new CSSourceHeight( TheCSActionTableParameter );
   new CSSourceX( TheCSActionTableParameter );
   new CSSourceY( TheCSActionTableParameter );
   new CSRadius( TheCSActionTableParameter );
   new CSSoftness( TheCSActionTableParameter );
   new CSOpacity( TheCSActionTableParameter );

   new CSWidth( this );
   new CSHeight( this );
   new CSColor( this );
   new CSBoundsColor( this );
}

// ----------------------------------------------------------------------------

IsoString CloneStampProcess::Id() const
{
   return "CloneStamp";
}

// ----------------------------------------------------------------------------

IsoString CloneStampProcess::Category() const
{
   return "Painting";
}

// ----------------------------------------------------------------------------

uint32 CloneStampProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CloneStampProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String CloneStampProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/CloneStamp.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* CloneStampProcess::DefaultInterface() const
{
   return TheCloneStampInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* CloneStampProcess::Create() const
{
   return new CloneStampInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* CloneStampProcess::Clone( const ProcessImplementation& p ) const
{
   const CloneStampInstance* instPtr = dynamic_cast<const CloneStampInstance*>( &p );
   return (instPtr != nullptr) ? new CloneStampInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

ProcessImplementation* CloneStampProcess::TestClone( const ProcessImplementation& p ) const
{
   const CloneStampInstance* instPtr = dynamic_cast<const CloneStampInstance*>( &p );
   if ( instPtr != nullptr )
   {
      CloneStampInstance* i = new CloneStampInstance( TheCloneStampProcess );
      i->TestAssign( *instPtr );
      return i;
   }
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CloneStampProcess.cpp - Released 2025-04-07T08:53:55Z
