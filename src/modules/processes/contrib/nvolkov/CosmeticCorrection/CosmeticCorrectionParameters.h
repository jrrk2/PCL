//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.4.1
// ----------------------------------------------------------------------------
// CosmeticCorrectionParameters.h - Released 2025-04-07T08:53:56Z
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

#ifndef __CosmeticCorrectionParameters_h
#define __CosmeticCorrectionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CCTargetFrames : public MetaTable
{
public:

   CCTargetFrames( MetaProcess* );
   IsoString Id() const override;
};

class CCTargetFrameEnabled : public MetaBoolean
{
public:

   CCTargetFrameEnabled( MetaTable* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCTargetFramePath : public MetaString
{
public:

   CCTargetFramePath( MetaTable* );
   IsoString Id() const override;
};

class CCOutputDir : public MetaString
{
public:

   CCOutputDir( MetaProcess* );
   IsoString Id() const override;
};

class CCOutputExtension : public MetaString
{
public:

   CCOutputExtension( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

class CCPrefix : public MetaString
{
public:

   CCPrefix( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

class CCPostfix : public MetaString
{
public:

   CCPostfix( MetaProcess* );
   IsoString Id() const override;
   String DefaultValue() const override;
};

class CCGenerateHistoryProperties : public MetaBoolean
{
public:

   CCGenerateHistoryProperties( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};
extern CCGenerateHistoryProperties* TheGenerateHistoryProperties;

class CCOverwrite : public MetaBoolean
{
public:

   CCOverwrite( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};
extern CCOverwrite* TheOverwrite;

class CCCFA : public MetaBoolean
{
public:

   CCCFA( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};
class CCAmount : public MetaFloat
{
public:

   CCAmount( MetaProcess* );
   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// Via Master Dark -----------------------------
class CCUseMasterDark : public MetaBoolean
{
public:

   CCUseMasterDark( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCMasterDarkPath : public MetaString
{
public:

   CCMasterDarkPath( MetaProcess* );
   IsoString Id() const override;
};

class CCHotDarkCheck : public MetaBoolean
{
public:

   CCHotDarkCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCHotLevel : public MetaFloat
{
public:

   CCHotLevel( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

class CCColdDarkCheck : public MetaBoolean
{
public:

   CCColdDarkCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCColdLevel : public MetaFloat
{
public:

   CCColdLevel( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

// Via Auto Detect -----------------------------
class CCUseAutoDetect : public MetaBoolean
{
public:

   CCUseAutoDetect( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCHotAutoValue : public MetaFloat
{
public:

   CCHotAutoValue( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

class CCHotAutoCheck : public MetaBoolean
{
public:

   CCHotAutoCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

class CCColdAutoValue : public MetaFloat
{
public:

   CCColdAutoValue( MetaProcess* );
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

class CCColdAutoCheck : public MetaBoolean
{
public:

   CCColdAutoCheck( MetaProcess* );
   IsoString Id() const override;
   bool DefaultValue() const override;
};

// Via Defect List -----------------------------
class CCUseDefectList : public MetaBoolean
{
public:

   CCUseDefectList( MetaProcess* );
   IsoString Id() const override;
};

class CCDefects : public MetaTable
{
public:

   CCDefects( MetaProcess* );
   IsoString Id() const override;
};

class CCDefectEnabled : public MetaBoolean
{
public:

   CCDefectEnabled( MetaTable* );
   IsoString Id() const override;
};

class CCDefectIsRow : public MetaBoolean
{
public:

   CCDefectIsRow( MetaTable* );
   IsoString Id() const override;
};

class CCDefectAddress : public MetaUInt16
{
public:

   CCDefectAddress( MetaTable* );
   IsoString Id() const override;
};

class CCDefectIsRange : public MetaBoolean
{
public:

   CCDefectIsRange( MetaTable* );
   IsoString Id() const override;
};

class CCDefectBegin : public MetaUInt16
{
public:

   CCDefectBegin( MetaTable* );
   IsoString Id() const override;
};

class CCDefectEnd : public MetaUInt16
{
public:

   CCDefectEnd( MetaTable* );
   IsoString Id() const override;
};

// ----------------------------------------------------------------------------

extern CCTargetFrames* TheTargetFrames;
extern CCTargetFrameEnabled* TheTargetFrameEnabled;
extern CCTargetFramePath* TheTargetFramePath;

extern CCOutputDir* TheOutputDir;
extern CCOutputExtension* TheOutputExtension;
extern CCPrefix* ThePrefix;
extern CCPostfix* ThePostfix;

extern CCCFA* TheCFA;
extern CCAmount* TheAmount;

extern CCUseMasterDark* TheUseMasterDark;
extern CCMasterDarkPath* TheMasterPath;
extern CCHotDarkCheck* TheHotDarkCheck;
extern CCHotLevel* TheHotLevel;
extern CCColdDarkCheck* TheColdDarkCheck;
extern CCColdLevel* TheColdLevel;

extern CCUseAutoDetect* TheUseAutoDetect;
extern CCHotAutoCheck* TheHotAutoCheck;
extern CCHotAutoValue* TheHotAutoValue;
extern CCColdAutoCheck* TheColdAutoCheck;
extern CCColdAutoValue* TheColdAutoValue;

extern CCUseDefectList* TheUseDefectList;
extern CCDefects* TheDefects;
extern CCDefectEnabled* TheDefectEnabled;
extern CCDefectIsRow* TheDefectIsRow;
extern CCDefectAddress* TheDefectAddress;
extern CCDefectIsRange* TheDefectIsRange;
extern CCDefectBegin* TheDefectBegin;
extern CCDefectEnd* TheDefectEnd;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __CosmeticCorrectionParameters_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionParameters.h - Released 2025-04-07T08:53:56Z
