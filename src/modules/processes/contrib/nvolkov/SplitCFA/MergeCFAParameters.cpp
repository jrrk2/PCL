//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// MergeCFAParameters.cpp - Released 2025-04-07T08:53:56Z
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

#include "MergeCFAParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

MergeCFASourceImage0* TheMergeCFASourceImage0Parameter = nullptr;
MergeCFASourceImage1* TheMergeCFASourceImage1Parameter = nullptr;
MergeCFASourceImage2* TheMergeCFASourceImage2Parameter = nullptr;
MergeCFASourceImage3* TheMergeCFASourceImage3Parameter = nullptr;
MergeCFAOutputViewId* TheMergeCFAOutputViewIdParameter = nullptr;

// ----------------------------------------------------------------------------

MergeCFASourceImage0::MergeCFASourceImage0( MetaProcess* P )
   : MetaString( P )
{
   TheMergeCFASourceImage0Parameter = this;
}

IsoString MergeCFASourceImage0::Id() const
{
   return "sourceCFAImage0";
}

// ----------------------------------------------------------------------------

MergeCFASourceImage1::MergeCFASourceImage1( MetaProcess* P )
   : MetaString( P )
{
   TheMergeCFASourceImage1Parameter = this;
}

IsoString MergeCFASourceImage1::Id() const
{
   return "sourceCFAImage1";
}

// ----------------------------------------------------------------------------

MergeCFASourceImage2::MergeCFASourceImage2( MetaProcess* P )
   : MetaString( P )
{
   TheMergeCFASourceImage2Parameter = this;
}

IsoString MergeCFASourceImage2::Id() const
{
   return "sourceCFAImage2";
}

// ----------------------------------------------------------------------------

MergeCFASourceImage3::MergeCFASourceImage3( MetaProcess* P )
   : MetaString( P )
{
   TheMergeCFASourceImage3Parameter = this;
}

IsoString MergeCFASourceImage3::Id() const
{
   return "sourceCFAImage3";
}

// ----------------------------------------------------------------------------

MergeCFAOutputViewId::MergeCFAOutputViewId( MetaProcess* P )
   : MetaString( P )
{
   TheMergeCFAOutputViewIdParameter = this;
}

IsoString MergeCFAOutputViewId::Id() const
{
   return "outputViewId";
}

bool MergeCFAOutputViewId::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF MergeCFAParameters.cpp - Released 2025-04-07T08:53:56Z
