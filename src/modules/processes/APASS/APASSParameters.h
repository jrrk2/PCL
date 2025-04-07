//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.5
// ----------------------------------------------------------------------------
// APASSParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __APASSParameters_h
#define __APASSParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ACommand : public MetaString
{
public:

   ACommand( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern ACommand* TheACommandParameter;

// ----------------------------------------------------------------------------

class ACenterRA : public MetaDouble
{
public:

   ACenterRA( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern ACenterRA* TheACenterRAParameter;

// ----------------------------------------------------------------------------

class ACenterDec : public MetaDouble
{
public:

   ACenterDec( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern ACenterDec* TheACenterDecParameter;

// ----------------------------------------------------------------------------

class ARadius : public MetaDouble
{
public:

   ARadius( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern ARadius* TheARadiusParameter;

// ----------------------------------------------------------------------------

class AMagnitudeLow : public MetaFloat
{
public:

   AMagnitudeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern AMagnitudeLow* TheAMagnitudeLowParameter;

// ----------------------------------------------------------------------------

class AMagnitudeHigh : public MetaFloat
{
public:

   AMagnitudeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern AMagnitudeHigh* TheAMagnitudeHighParameter;

// ----------------------------------------------------------------------------

class ASourceLimit : public MetaUInt32
{
public:

   ASourceLimit( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern ASourceLimit* TheASourceLimitParameter;

// ----------------------------------------------------------------------------

class ARequiredFlags : public MetaUInt32
{
public:

   ARequiredFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern ARequiredFlags* TheARequiredFlagsParameter;

// ----------------------------------------------------------------------------

class AInclusionFlags : public MetaUInt32
{
public:

   AInclusionFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AInclusionFlags* TheAInclusionFlagsParameter;

// ----------------------------------------------------------------------------

class AExclusionFlags : public MetaUInt32
{
public:

   AExclusionFlags( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AExclusionFlags* TheAExclusionFlagsParameter;

// ----------------------------------------------------------------------------

class AVerbosity : public MetaInt32
{
public:

   AVerbosity( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AVerbosity* TheAVerbosityParameter;

// ----------------------------------------------------------------------------

class ADataRelease : public MetaEnumeration
{
public:

   enum { BestAvailable,
          DR9,
          DR10,
          NumberOfItems,
          Default = DR10 };

   ADataRelease( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static IsoString ReleaseName( pcl_enum dr )
   {
      switch ( dr )
      {
      case DR9:  return "DR9";
      default:
      case DR10: return "DR10";
      }
   }
};

extern ADataRelease* TheADataReleaseParameter;

// ----------------------------------------------------------------------------

class ASortBy : public MetaEnumeration
{
public:

   enum { DontSort,
          RA,
          Dec,
          V,
          B,
          g,
          r,
          i,
          z_s,
          NumberOfItems,
          Default = V };

   ASortBy( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ASortBy* TheASortByParameter;

// ----------------------------------------------------------------------------

class AGenerateTextOutput : public MetaBoolean
{
public:

   AGenerateTextOutput( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern AGenerateTextOutput* TheAGenerateTextOutputParameter;

// ----------------------------------------------------------------------------

class AGenerateBinaryOutput : public MetaBoolean
{
public:

   AGenerateBinaryOutput( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern AGenerateBinaryOutput* TheAGenerateBinaryOutputParameter;

// ----------------------------------------------------------------------------

class ATextFormat : public MetaEnumeration
{
public:

   enum { CSV,
          Tabular,
          TabularCompound,
          NumberOfItems,
          Default = TabularCompound };

   ATextFormat( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ATextFormat* TheATextFormatParameter;

// ----------------------------------------------------------------------------

class ATextHeaders : public MetaEnumeration
{
public:

   enum { None,
          TableColumns,
          SearchParameters,
          SearchParametersAndTableColumns,
          NumberOfItems,
          Default = SearchParametersAndTableColumns };

   ATextHeaders( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ATextHeaders* TheATextHeadersParameter;

// ----------------------------------------------------------------------------

class AOutputFilePath : public MetaString
{
public:

   AOutputFilePath( MetaProcess* );

   IsoString Id() const override;
};

extern AOutputFilePath* TheAOutputFilePathParameter;

// ----------------------------------------------------------------------------

class ADatabaseFilePaths : public MetaTable
{
public:

   ADatabaseFilePaths( MetaProcess* );

   IsoString Id() const override;
};

extern ADatabaseFilePaths* TheADatabaseFilePathsParameter;

// ----------------------------------------------------------------------------

class ADatabaseFilePath : public MetaString
{
public:

   ADatabaseFilePath( MetaTable* );

   IsoString Id() const override;
};

extern ADatabaseFilePath* TheADatabaseFilePathParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class ASources : public MetaTable
{
public:

   ASources( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ASources* TheASourcesParameter;

// ----------------------------------------------------------------------------

class ASourceRA : public MetaDouble
{
public:

   ASourceRA( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceRA* TheASourceRAParameter;

// ----------------------------------------------------------------------------

class ASourceDec : public MetaDouble
{
public:

   ASourceDec( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceDec* TheASourceDecParameter;

// ----------------------------------------------------------------------------

class ASourceMag_V : public MetaFloat
{
public:

   ASourceMag_V( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_V* TheASourceMag_VParameter;

// ----------------------------------------------------------------------------

class ASourceMag_B : public MetaFloat
{
public:

   ASourceMag_B( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_B* TheASourceMag_BParameter;

// ----------------------------------------------------------------------------

class ASourceMag_g : public MetaFloat
{
public:

   ASourceMag_g( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_g* TheASourceMag_gParameter;

// ----------------------------------------------------------------------------

class ASourceMag_r : public MetaFloat
{
public:

   ASourceMag_r( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_r* TheASourceMag_rParameter;

// ----------------------------------------------------------------------------

class ASourceMag_i : public MetaFloat
{
public:

   ASourceMag_i( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_i* TheASourceMag_iParameter;

// ----------------------------------------------------------------------------

class ASourceMag_z_s : public MetaFloat
{
public:

   ASourceMag_z_s( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_z_s* TheASourceMag_z_sParameter;

// ----------------------------------------------------------------------------

class ASourceMag_V_Error : public MetaFloat
{
public:

   ASourceMag_V_Error( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_V_Error* TheASourceMag_V_ErrorParameter;

// ----------------------------------------------------------------------------

class ASourceMag_B_Error : public MetaFloat
{
public:

   ASourceMag_B_Error( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_B_Error* TheASourceMag_B_ErrorParameter;

// ----------------------------------------------------------------------------

class ASourceMag_g_Error : public MetaFloat
{
public:

   ASourceMag_g_Error( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_g_Error* TheASourceMag_g_ErrorParameter;

// ----------------------------------------------------------------------------

class ASourceMag_r_Error : public MetaFloat
{
public:

   ASourceMag_r_Error( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_r_Error* TheASourceMag_r_ErrorParameter;

// ----------------------------------------------------------------------------

class ASourceMag_i_Error : public MetaFloat
{
public:

   ASourceMag_i_Error( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_i_Error* TheASourceMag_i_ErrorParameter;

// ----------------------------------------------------------------------------

class ASourceMag_z_s_Error : public MetaFloat
{
public:

   ASourceMag_z_s_Error( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ASourceMag_z_s_Error* TheASourceMag_z_s_ErrorParameter;

// ----------------------------------------------------------------------------

class ASourceFlags : public MetaUInt32
{
public:

   ASourceFlags( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ASourceFlags* TheASourceFlagsParameter;

// ----------------------------------------------------------------------------

class AExcessCount : public MetaUInt32
{
public:

   AExcessCount( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern AExcessCount* TheAExcessCountParameter;

// ----------------------------------------------------------------------------

class ARejectCount : public MetaUInt32
{
public:

   ARejectCount( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ARejectCount* TheARejectCountParameter;

// ----------------------------------------------------------------------------

class ATimeTotal : public MetaDouble
{
public:

   ATimeTotal( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ATimeTotal* TheATimeTotalParameter;

// ----------------------------------------------------------------------------

class ATimeIO : public MetaDouble
{
public:

   ATimeIO( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ATimeIO* TheATimeIOParameter;

// ----------------------------------------------------------------------------

class ACountIO : public MetaUInt32
{
public:

   ACountIO( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern ACountIO* TheACountIOParameter;

// ----------------------------------------------------------------------------

class ATimeUncompress : public MetaDouble
{
public:

   ATimeUncompress( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ATimeUncompress* TheATimeUncompressParameter;

// ----------------------------------------------------------------------------

class ATimeDecode : public MetaDouble
{
public:

   ATimeDecode( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ATimeDecode* TheATimeDecodeParameter;

// ----------------------------------------------------------------------------

class AIsValid : public MetaBoolean
{
public:

   AIsValid( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern AIsValid* TheAIsValidParameter;

// ----------------------------------------------------------------------------

class AOutputDataRelease : public MetaInt32
{
public:

   AOutputDataRelease( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern AOutputDataRelease* TheAOutputDataReleaseParameter;

// ----------------------------------------------------------------------------

class ADatabaseMagnitudeLow : public MetaFloat
{
public:

   ADatabaseMagnitudeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ADatabaseMagnitudeLow* TheADatabaseMagnitudeLowParameter;

// ----------------------------------------------------------------------------

class ADatabaseMagnitudeHigh : public MetaFloat
{
public:

   ADatabaseMagnitudeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   bool IsReadOnly() const override;
};

extern ADatabaseMagnitudeHigh* TheADatabaseMagnitudeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __APASSParameters_h

// ----------------------------------------------------------------------------
// EOF APASSParameters.h - Released 2025-04-07T08:53:55Z
