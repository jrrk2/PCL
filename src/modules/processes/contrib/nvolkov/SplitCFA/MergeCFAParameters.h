//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// MergeCFAParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __MergeCFAParameters_h
#define __MergeCFAParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{
// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class MergeCFASourceImage0 : public MetaString
{
public:

   MergeCFASourceImage0( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage0* TheMergeCFASourceImage0Parameter;

// ----------------------------------------------------------------------------

class MergeCFASourceImage1 : public MetaString
{
public:

   MergeCFASourceImage1( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage1* TheMergeCFASourceImage1Parameter;

// ----------------------------------------------------------------------------

class MergeCFASourceImage2 : public MetaString
{
public:

   MergeCFASourceImage2( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage2* TheMergeCFASourceImage2Parameter;

// ----------------------------------------------------------------------------

class MergeCFASourceImage3 : public MetaString
{
public:

   MergeCFASourceImage3( MetaProcess* );
   IsoString Id() const override;
};

extern MergeCFASourceImage3* TheMergeCFASourceImage3Parameter;

// ----------------------------------------------------------------------------

class MergeCFAOutputViewId : public MetaString
{
public:

   MergeCFAOutputViewId( MetaProcess* );
   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern MergeCFAOutputViewId* TheMergeCFAOutputViewIdParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __MergeCFAParameters_h

// ----------------------------------------------------------------------------
// EOF MergeCFAParameters.h - Released 2025-04-07T08:53:56Z
