//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAParameters.cpp - Released 2025-04-07T08:53:56Z
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

#include "SplitCFAParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SplitCFATargetFrames* TheTargetFramesParameter = nullptr;
SplitCFATargetFrameEnabled* TheTargetFrameEnabledParameter = nullptr;
SplitCFATargetFramePath* TheTargetFramePathParameter = nullptr;
SplitCFATargetFrameFolder* TheTargetFrameFolderParameter = nullptr;

SplitCFAOutputTree* TheOutputTreeParameter = nullptr;
SplitCFAOutputSubDirCFA* TheOutputSubDirCFAParameter = nullptr;
SplitCFAOutputDir* TheOutputDirParameter = nullptr;
SplitCFAPrefix* ThePrefixParameter = nullptr;
SplitCFAPostfix* ThePostfixParameter = nullptr;
SplitCFAOverwrite* TheOverwriteParameter = nullptr;

SplitCFAOutputViewId0* TheSplitCFAOutputViewId0Parameter = nullptr;
SplitCFAOutputViewId1* TheSplitCFAOutputViewId1Parameter = nullptr;
SplitCFAOutputViewId2* TheSplitCFAOutputViewId2Parameter = nullptr;
SplitCFAOutputViewId3* TheSplitCFAOutputViewId3Parameter = nullptr;

// ----------------------------------------------------------------------------

SplitCFATargetFrames::SplitCFATargetFrames( MetaProcess* P )
   : MetaTable( P )
{
   TheTargetFramesParameter = this;
}

IsoString SplitCFATargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

SplitCFATargetFrameEnabled::SplitCFATargetFrameEnabled( MetaTable* T )
   : MetaBoolean( T )
{
   TheTargetFrameEnabledParameter = this;
}

IsoString SplitCFATargetFrameEnabled::Id() const
{
   return "enabled";
}

bool SplitCFATargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SplitCFATargetFramePath::SplitCFATargetFramePath( MetaTable* T )
   : MetaString( T )
{
   TheTargetFramePathParameter = this;
}

IsoString SplitCFATargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

SplitCFATargetFrameFolder::SplitCFATargetFrameFolder( MetaTable* T )
   : MetaString( T )
{
   TheTargetFrameFolderParameter = this;
}

IsoString SplitCFATargetFrameFolder::Id() const
{
   return "folder";
}

// ----------------------------------------------------------------------------

SplitCFAOutputTree::SplitCFAOutputTree( MetaProcess* P )
   : MetaBoolean( P )
{
   TheOutputTreeParameter = this;
}

IsoString SplitCFAOutputTree::Id() const
{
   return "outputTree";
}

bool SplitCFAOutputTree::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SplitCFAOutputSubDirCFA::SplitCFAOutputSubDirCFA( MetaProcess* P )
   : MetaBoolean( P )
{
   TheOutputSubDirCFAParameter = this;
}

IsoString SplitCFAOutputSubDirCFA::Id() const
{
   return "outputSubDirCFA";
}

bool SplitCFAOutputSubDirCFA::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

SplitCFAOutputDir::SplitCFAOutputDir( MetaProcess* P )
   : MetaString( P )
{
   TheOutputDirParameter = this;
}

IsoString SplitCFAOutputDir::Id() const
{
   return "outputDir";
}

String SplitCFAOutputDir::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

SplitCFAPrefix::SplitCFAPrefix( MetaProcess* P )
   : MetaString( P )
{
   ThePrefixParameter = this;
}

IsoString SplitCFAPrefix::Id() const
{
   return "prefix";
}

String SplitCFAPrefix::DefaultValue() const
{
   return String();
}

// ----------------------------------------------------------------------------

SplitCFAPostfix::SplitCFAPostfix( MetaProcess* P )
   : MetaString( P )
{
   ThePostfixParameter = this;
}

IsoString SplitCFAPostfix::Id() const
{
   return "postfix";
}

String SplitCFAPostfix::DefaultValue() const
{
   return "_CFA";
}

// ----------------------------------------------------------------------------

SplitCFAOverwrite::SplitCFAOverwrite( MetaProcess* P )
   : MetaBoolean( P )
{
   TheOverwriteParameter = this;
}

IsoString SplitCFAOverwrite::Id() const
{
   return "overwrite";
}

bool SplitCFAOverwrite::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SplitCFAOutputViewId0::SplitCFAOutputViewId0( MetaProcess* P )
   : MetaString( P )
{
   TheSplitCFAOutputViewId0Parameter = this;
}

IsoString SplitCFAOutputViewId0::Id() const
{
   return "outputViewId0";
}

bool SplitCFAOutputViewId0::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SplitCFAOutputViewId1::SplitCFAOutputViewId1( MetaProcess* P )
   : MetaString( P )
{
   TheSplitCFAOutputViewId1Parameter = this;
}

IsoString SplitCFAOutputViewId1::Id() const
{
   return "outputViewId1";
}

bool SplitCFAOutputViewId1::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SplitCFAOutputViewId2::SplitCFAOutputViewId2( MetaProcess* P )
   : MetaString( P )
{
   TheSplitCFAOutputViewId2Parameter = this;
}

IsoString SplitCFAOutputViewId2::Id() const
{
   return "outputViewId2";
}

bool SplitCFAOutputViewId2::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

SplitCFAOutputViewId3::SplitCFAOutputViewId3( MetaProcess* P )
   : MetaString( P )
{
   TheSplitCFAOutputViewId3Parameter = this;
}

IsoString SplitCFAOutputViewId3::Id() const
{
   return "outputViewId3";
}

bool SplitCFAOutputViewId3::IsReadOnly() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF SplitCFAParameters.cpp - Released 2025-04-07T08:53:56Z
