//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ImageIdentifierParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ImageIdentifierParameters_h
#define __ImageIdentifierParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ImageIdentifier : public MetaString
{
public:

   ImageIdentifier( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern ImageIdentifier* TheImageIdentifierParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ImageIdentifierParameters_h

// ----------------------------------------------------------------------------
// EOF ImageIdentifierParameters.h - Released 2025-04-07T08:53:56Z
