//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// FITSHeaderParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FITSHeaderParameters_h
#define __FITSHeaderParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class FITSKeywordTable : public MetaTable
{
public:

   FITSKeywordTable( MetaProcess* );

   IsoString Id() const override;
};

extern FITSKeywordTable* TheFITSKeywordTableParameter;

// ----------------------------------------------------------------------------

class FITSKeywordName : public MetaString
{
public:

   FITSKeywordName( MetaTable* );

   IsoString Id() const override;
};

extern FITSKeywordName* TheFITSKeywordNameParameter;

// ----------------------------------------------------------------------------

class FITSKeywordValue : public MetaString
{
public:

   FITSKeywordValue( MetaTable* );

   IsoString Id() const override;
};

extern FITSKeywordValue* TheFITSKeywordValueParameter;

// ----------------------------------------------------------------------------

class FITSKeywordComment : public MetaString
{
public:

   FITSKeywordComment( MetaTable* );

   IsoString Id() const override;
};

extern FITSKeywordComment* TheFITSKeywordCommentParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __FITSHeaderParameters_h

// ----------------------------------------------------------------------------
// EOF FITSHeaderParameters.h - Released 2025-04-07T08:53:56Z
