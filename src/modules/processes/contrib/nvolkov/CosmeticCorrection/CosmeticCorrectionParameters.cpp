//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.4.1
// ----------------------------------------------------------------------------
// CosmeticCorrectionParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CosmeticCorrectionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CCTargetFrames* TheTargetFrames = nullptr;
CCTargetFrameEnabled* TheTargetFrameEnabled = nullptr;
CCTargetFramePath* TheTargetFramePath = nullptr;

CCOutputDir* TheOutputDir = nullptr;
CCOutputExtension* TheOutputExtension = nullptr;
CCPrefix* ThePrefix = nullptr;
CCPostfix* ThePostfix = nullptr;
CCGenerateHistoryProperties* TheGenerateHistoryProperties = nullptr;
CCOverwrite* TheOverwrite = nullptr;

CCCFA* TheCFA = nullptr;
CCAmount* TheAmount = nullptr;

CCUseMasterDark* TheUseMasterDark = nullptr;
CCMasterDarkPath* TheMasterPath = nullptr;
CCHotDarkCheck* TheHotDarkCheck = nullptr;
CCColdLevel* TheColdLevel = nullptr;
CCColdDarkCheck* TheColdDarkCheck = nullptr;
CCHotLevel* TheHotLevel = nullptr;

CCUseAutoDetect* TheUseAutoDetect = nullptr;
CCHotAutoCheck* TheHotAutoCheck = nullptr;
CCHotAutoValue* TheHotAutoValue = nullptr;
CCColdAutoCheck* TheColdAutoCheck = nullptr;
CCColdAutoValue* TheColdAutoValue = nullptr;

CCUseDefectList* TheUseDefectList = nullptr;
CCDefects* TheDefects = nullptr;
CCDefectEnabled* TheDefectEnabled = nullptr;
CCDefectIsRow* TheDefectIsRow = nullptr;
CCDefectAddress* TheDefectAddress = nullptr;
CCDefectIsRange* TheDefectIsRange = nullptr;
CCDefectBegin* TheDefectBegin = nullptr;
CCDefectEnd* TheDefectEnd = nullptr;

// ----------------------------------------------------------------------------

CCTargetFrames::CCTargetFrames( MetaProcess* P )
   : MetaTable( P )
{
   TheTargetFrames = this;
}

IsoString CCTargetFrames::Id() const
{
   return "targetFrames";
}

// ----------------------------------------------------------------------------

CCTargetFrameEnabled::CCTargetFrameEnabled( MetaTable* T )
   : MetaBoolean( T )
{
   TheTargetFrameEnabled = this;
}

IsoString CCTargetFrameEnabled::Id() const
{
   return "enabled";
}

bool CCTargetFrameEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CCTargetFramePath::CCTargetFramePath( MetaTable* T )
   : MetaString( T )
{
   TheTargetFramePath = this;
}

IsoString CCTargetFramePath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

CCOutputDir::CCOutputDir( MetaProcess* P )
   : MetaString( P )
{
   TheOutputDir = this;
}

IsoString CCOutputDir::Id() const
{
   return "outputDir";
}

// ----------------------------------------------------------------------------

CCOutputExtension::CCOutputExtension( MetaProcess* P )
   : MetaString( P )
{
   TheOutputExtension = this;
}

IsoString CCOutputExtension::Id() const
{
   return "outputExtension";
}

String CCOutputExtension::DefaultValue() const
{
   return ".xisf";
}

// ----------------------------------------------------------------------------

CCPrefix::CCPrefix( MetaProcess* P )
   : MetaString( P )
{
   ThePrefix = this;
}

IsoString CCPrefix::Id() const
{
   return "prefix";
}

String CCPrefix::DefaultValue() const
{
   return "";
}

// ----------------------------------------------------------------------------

CCPostfix::CCPostfix( MetaProcess* P )
   : MetaString( P )
{
   ThePostfix = this;
}

IsoString CCPostfix::Id() const
{
   return "postfix";
}

String CCPostfix::DefaultValue() const
{
   return "_cc";
}

// ----------------------------------------------------------------------------

CCGenerateHistoryProperties::CCGenerateHistoryProperties( MetaProcess* P )
   : MetaBoolean( P )
{
   TheGenerateHistoryProperties = this;
}

IsoString CCGenerateHistoryProperties::Id() const
{
   return "generateHistoryProperties";
}

bool CCGenerateHistoryProperties::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCOverwrite::CCOverwrite( MetaProcess* P )
   : MetaBoolean( P )
{
   TheOverwrite = this;
}

IsoString CCOverwrite::Id() const
{
   return "overwrite";
}

bool CCOverwrite::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCCFA::CCCFA( MetaProcess* P )
   : MetaBoolean( P )
{
   TheCFA = this;
}

IsoString CCCFA::Id() const
{
   return "cfa";
}

bool CCCFA::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCAmount::CCAmount( MetaProcess* P )
   : MetaFloat( P )
{
   TheAmount = this;
}

IsoString CCAmount::Id() const
{
   return "amount";
}

IsoString CCAmount::Aliases() const
{
   return "transferFn";
}

int CCAmount::Precision() const
{
   return 2;
}

double CCAmount::DefaultValue() const
{
   return 1;
}

double CCAmount::MinimumValue() const
{
   return 0;
}

double CCAmount::MaximumValue() const
{
   return 1;
}

// -----------------------------------------------------------Via Master Dark-----------------

CCUseMasterDark::CCUseMasterDark( MetaProcess* P )
   : MetaBoolean( P )
{
   TheUseMasterDark = this;
}

IsoString CCUseMasterDark::Id() const
{
   return "useMasterDark";
}

bool CCUseMasterDark::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCMasterDarkPath::CCMasterDarkPath( MetaProcess* P )
   : MetaString( P )
{
   TheMasterPath = this;
}

IsoString CCMasterDarkPath::Id() const
{
   return "masterDarkPath";
}

// ----------------------------------------------------------------------------

CCHotDarkCheck::CCHotDarkCheck( MetaProcess* P )
   : MetaBoolean( P )
{
   TheHotDarkCheck = this;
}

IsoString CCHotDarkCheck::Id() const
{
   return "hotDarkCheck";
}

bool CCHotDarkCheck::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCHotLevel::CCHotLevel( MetaProcess* P )
   : MetaFloat( P )
{
   TheHotLevel = this;
}

IsoString CCHotLevel::Id() const
{
   return "hotDarkLevel";
}

int CCHotLevel::Precision() const
{
   return 10;
}

double CCHotLevel::DefaultValue() const
{
   return 1;
}

double CCHotLevel::MinimumValue() const
{
   return 0;
}

double CCHotLevel::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CCColdDarkCheck::CCColdDarkCheck( MetaProcess* P )
   : MetaBoolean( P )
{
   TheColdDarkCheck = this;
}

IsoString CCColdDarkCheck::Id() const
{
   return "coldDarkCheck";
}

bool CCColdDarkCheck::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCColdLevel::CCColdLevel( MetaProcess* P )
   : MetaFloat( P )
{
   TheColdLevel = this;
}

IsoString CCColdLevel::Id() const
{
   return "coldDarkLevel";
}

int CCColdLevel::Precision() const
{
   return 10;
}

double CCColdLevel::DefaultValue() const
{
   return 0;
}

double CCColdLevel::MinimumValue() const
{
   return 0;
}

double CCColdLevel::MaximumValue() const
{
   return 1;
}

// -----------------------------------------------------------Via Auto Detect-----------------

CCUseAutoDetect::CCUseAutoDetect( MetaProcess* P )
   : MetaBoolean( P )
{
   TheUseAutoDetect = this;
}

IsoString CCUseAutoDetect::Id() const
{
   return "useAutoDetect";
}

bool CCUseAutoDetect::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCHotAutoCheck::CCHotAutoCheck( MetaProcess* P )
   : MetaBoolean( P )
{
   TheHotAutoCheck = this;
}

IsoString CCHotAutoCheck::Id() const
{
   return "hotAutoCheck";
}

bool CCHotAutoCheck::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCHotAutoValue::CCHotAutoValue( MetaProcess* P )
   : MetaFloat( P )
{
   TheHotAutoValue = this;
}

IsoString CCHotAutoValue::Id() const
{
   return "hotAutoValue";
}

int CCHotAutoValue::Precision() const
{
   return 1;
}

double CCHotAutoValue::DefaultValue() const
{
   return 3;
}

double CCHotAutoValue::MinimumValue() const
{
   return 0;
}

double CCHotAutoValue::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

CCColdAutoCheck::CCColdAutoCheck( MetaProcess* P )
   : MetaBoolean( P )
{
   TheColdAutoCheck = this;
}

IsoString CCColdAutoCheck::Id() const
{
   return "coldAutoCheck";
}

bool CCColdAutoCheck::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CCColdAutoValue::CCColdAutoValue( MetaProcess* P )
   : MetaFloat( P )
{
   TheColdAutoValue = this;
}

IsoString CCColdAutoValue::Id() const
{
   return "coldAutoValue";
}

int CCColdAutoValue::Precision() const
{
   return 1;
}

double CCColdAutoValue::DefaultValue() const
{
   return 3;
}

double CCColdAutoValue::MinimumValue() const
{
   return 0;
}

double CCColdAutoValue::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

CCUseDefectList::CCUseDefectList( MetaProcess* P )
   : MetaBoolean( P )
{
   TheUseDefectList = this;
}

IsoString CCUseDefectList::Id() const
{
   return "useDefectList";
}

// ----------------------------------------------------------------------------

CCDefects::CCDefects( MetaProcess* P )
   : MetaTable( P )
{
   TheDefects = this;
}

IsoString CCDefects::Id() const
{
   return "defects";
}

// ----------------------------------------------------------------------------

CCDefectEnabled::CCDefectEnabled( MetaTable* T )
   : MetaBoolean( T )
{
   TheDefectEnabled = this;
}

IsoString CCDefectEnabled::Id() const
{
   return "defectEnabled";
}

// ----------------------------------------------------------------------------

CCDefectIsRow::CCDefectIsRow( MetaTable* T )
   : MetaBoolean( T )
{
   TheDefectIsRow = this;
}

IsoString CCDefectIsRow::Id() const
{
   return "defectIsRow";
}

// ----------------------------------------------------------------------------

CCDefectAddress::CCDefectAddress( MetaTable* T )
   : MetaUInt16( T )
{
   TheDefectAddress = this;
}

IsoString CCDefectAddress::Id() const
{
   return "defectAddress";
}

// ----------------------------------------------------------------------------

CCDefectIsRange::CCDefectIsRange( MetaTable* T )
   : MetaBoolean( T )
{
   TheDefectIsRange = this;
}

IsoString CCDefectIsRange::Id() const
{
   return "defectIsRange";
}

// ----------------------------------------------------------------------------

CCDefectBegin::CCDefectBegin( MetaTable* T )
   : MetaUInt16( T )
{
   TheDefectBegin = this;
}

IsoString CCDefectBegin::Id() const
{
   return "defectBegin";
}

// ----------------------------------------------------------------------------

CCDefectEnd::CCDefectEnd( MetaTable* T )
   : MetaUInt16( T )
{
   TheDefectEnd = this;
}

IsoString CCDefectEnd::Id() const
{
   return "defectEnd";
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionParameters.cpp - Released 2025-04-07T08:53:56Z
