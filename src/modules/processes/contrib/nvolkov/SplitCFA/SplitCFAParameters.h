//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAParameters.h - Released 2025-04-07T08:53:56Z
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

#ifndef __SplitCFAParameters_h
#define __SplitCFAParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SplitCFATargetFrames : public MetaTable
{
public:

   SplitCFATargetFrames( MetaProcess* );
   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class SplitCFATargetFrameEnabled : public MetaBoolean
{
public:

   SplitCFATargetFrameEnabled( MetaTable* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class SplitCFATargetFramePath : public MetaString
{
public:

   SplitCFATargetFramePath( MetaTable* );
   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class SplitCFATargetFrameFolder : public MetaString
{
public:

   SplitCFATargetFrameFolder( MetaTable* );
   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOutputDir : public MetaString
{
public:

   SplitCFAOutputDir( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOutputTree : public MetaBoolean
{
public:

   SplitCFAOutputTree( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOutputSubDirCFA : public MetaBoolean
{
public:

   SplitCFAOutputSubDirCFA( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAPrefix : public MetaString
{
public:

   SplitCFAPrefix( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAPostfix : public MetaString
{
public:

   SplitCFAPostfix( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOverwrite : public MetaBoolean
{
public:

   SplitCFAOverwrite( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOutputViewId0 : public MetaString
{
public:

   SplitCFAOutputViewId0( MetaProcess* );
   IsoString Id() const override;
   bool IsReadOnly() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOutputViewId1 : public MetaString
{
public:

   SplitCFAOutputViewId1( MetaProcess* );
   IsoString Id() const override;
   bool IsReadOnly() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOutputViewId2 : public MetaString
{
public:

   SplitCFAOutputViewId2( MetaProcess* );
   IsoString Id() const override;
   bool IsReadOnly() const override;
};

// ----------------------------------------------------------------------------

class SplitCFAOutputViewId3 : public MetaString
{
public:

   SplitCFAOutputViewId3( MetaProcess* );
   IsoString Id() const override;
   bool IsReadOnly() const override;
};

// ----------------------------------------------------------------------------

extern SplitCFATargetFrames* TheTargetFramesParameter;
extern SplitCFATargetFrameEnabled* TheTargetFrameEnabledParameter;
extern SplitCFATargetFramePath* TheTargetFramePathParameter;
extern SplitCFATargetFrameFolder* TheTargetFrameFolderParameter;

extern SplitCFAOutputDir* TheOutputDirParameter;
extern SplitCFAOutputTree* TheOutputTreeParameter;
extern SplitCFAOutputSubDirCFA* TheOutputSubDirCFAParameter;
extern SplitCFAPrefix* ThePrefixParameter;
extern SplitCFAPostfix* ThePostfixParameter;
extern SplitCFAOverwrite* TheOverwriteParameter;

extern SplitCFAOutputViewId0* TheSplitCFAOutputViewId0Parameter;
extern SplitCFAOutputViewId1* TheSplitCFAOutputViewId1Parameter;
extern SplitCFAOutputViewId2* TheSplitCFAOutputViewId2Parameter;
extern SplitCFAOutputViewId3* TheSplitCFAOutputViewId3Parameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __SplitCFAParameters_h

// ----------------------------------------------------------------------------
// EOF SplitCFAParameters.h - Released 2025-04-07T08:53:56Z
