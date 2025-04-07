//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// InverseFourierTransformProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "InverseFourierTransformProcess.h"
#include "InverseFourierTransformParameters.h"
#include "InverseFourierTransformInstance.h"
#include "InverseFourierTransformInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

InverseFourierTransformProcess* TheInverseFourierTransformProcess = nullptr;

// ----------------------------------------------------------------------------

InverseFourierTransformProcess::InverseFourierTransformProcess()
{
   TheInverseFourierTransformProcess = this;

   new IFTIdOfFirstComponent( this );
   new IFTIdOfSecondComponent( this );
   new IFTOnOutOfRangeResult( this );
}

// ----------------------------------------------------------------------------

IsoString InverseFourierTransformProcess::Id() const
{
   return "InverseFourierTransform";
}

// ----------------------------------------------------------------------------

IsoString InverseFourierTransformProcess::Category() const
{
   return "Fourier";
}

// ----------------------------------------------------------------------------

uint32 InverseFourierTransformProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String InverseFourierTransformProcess::Description() const
{
   return

   "<html>"
   "<p>InverseFourierTransform is a simple implementation of the inverse Fourier transform.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String InverseFourierTransformProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/InverseFourierTransform.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* InverseFourierTransformProcess::DefaultInterface() const
{
   return TheInverseFourierTransformInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* InverseFourierTransformProcess::Create() const
{
   return new InverseFourierTransformInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* InverseFourierTransformProcess::Clone( const ProcessImplementation& p ) const
{
   const InverseFourierTransformInstance* instPtr = dynamic_cast<const InverseFourierTransformInstance*>( &p );
   return (instPtr != nullptr) ? new InverseFourierTransformInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformProcess.cpp - Released 2025-04-07T08:53:55Z
