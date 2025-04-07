//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// FITSHeaderParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FITSHeaderParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FITSKeywordTable*   TheFITSKeywordTableParameter = 0;
FITSKeywordName*    TheFITSKeywordNameParameter = 0;
FITSKeywordValue*   TheFITSKeywordValueParameter = 0;
FITSKeywordComment* TheFITSKeywordCommentParameter = 0;

// ----------------------------------------------------------------------------

FITSKeywordTable::FITSKeywordTable( MetaProcess* P ) : MetaTable( P )
{
   TheFITSKeywordTableParameter = this;
}

IsoString FITSKeywordTable::Id() const
{
   return "keywords";
}

// ----------------------------------------------------------------------------

FITSKeywordName::FITSKeywordName( MetaTable* T ) : MetaString( T )
{
   TheFITSKeywordNameParameter = this;
}

IsoString FITSKeywordName::Id() const
{
   return "name";
}

// ----------------------------------------------------------------------------

FITSKeywordValue::FITSKeywordValue( MetaTable* T ) : MetaString( T )
{
   TheFITSKeywordValueParameter = this;
}

IsoString FITSKeywordValue::Id() const
{
   return "value";
}

// ----------------------------------------------------------------------------

FITSKeywordComment::FITSKeywordComment( MetaTable* T ) : MetaString( T )
{
   TheFITSKeywordCommentParameter = this;
}

IsoString FITSKeywordComment::Id() const
{
   return "comment";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSHeaderParameters.cpp - Released 2025-04-07T08:53:56Z
