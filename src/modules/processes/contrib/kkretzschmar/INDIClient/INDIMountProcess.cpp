//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIMountProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "INDIMountProcess.h"
#include "INDIMountInstance.h"
#include "INDIMountInterface.h"
#include "INDIMountParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

INDIMountProcess* TheINDIMountProcess = nullptr;

// ----------------------------------------------------------------------------

INDIMountProcess::INDIMountProcess()
{
   TheINDIMountProcess = this;
   new IMCDeviceName( this );
   new IMCCommand( this );
   new IMCSlewRate( this );
   new IMCTargetRA( this );
   new IMCTargetDec( this );
   new IMCCurrentLST( this );
   new IMCCurrentRA( this );
   new IMCCurrentDec( this );
   new IMCApparentTargetRA( this );
   new IMCApparentTargetDec( this );
   new IMCPierSide( this );
   new IMCSyncLST( this );
   new IMCSyncCelestialRA( this );
   new IMCSyncCelestialDec( this );
   new IMCSyncTelescopeRA( this );
   new IMCSyncTelescopeDec( this );
   new IMCEnableAlignmentCorrection( this );
   new IMCAlignmentMethod( this );
   new IMCAlignmentFile( this );
   new IMCAlignmentConfig( this );
   new IMCGeographicLatitude( this );
}

// ----------------------------------------------------------------------------

IsoString INDIMountProcess::Id() const
{
   return "IndigoMount";
}

// ----------------------------------------------------------------------------

IsoString INDIMountProcess::Category() const
{
   return "Instrumentation";
}

// ----------------------------------------------------------------------------

uint32 INDIMountProcess::Version() const
{
   return 0x200; // required
}

// ----------------------------------------------------------------------------

String INDIMountProcess::Description() const
{
   return "<html>"
          "<p>Control Indigo Mount devices.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String INDIMountProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/IndigoMount.svg";
}

// ----------------------------------------------------------------------------

bool INDIMountProcess::PrefersGlobalExecution() const
{
   return true;
}

// ----------------------------------------------------------------------------

ProcessInterface* INDIMountProcess::DefaultInterface() const
{
   return TheINDIMountInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDIMountProcess::Create() const
{
   return new INDIMountInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDIMountProcess::Clone( const ProcessImplementation& p ) const
{
   const INDIMountInstance* instPtr = dynamic_cast<const INDIMountInstance*>( &p );
   return ( instPtr != nullptr ) ? new INDIMountInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool INDIMountProcess::CanProcessCommandLines() const
{
   return false;
}

// ----------------------------------------------------------------------------

#if 0
static void ShowHelp()
{
   Console().Write(
"<raw>"
"Nothing to show."
"</raw>" );
}
#endif

int INDIMountProcess::ProcessCommandLine( const StringList& argv ) const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF INDIMountProcess.cpp - Released 2025-04-07T08:53:56Z
