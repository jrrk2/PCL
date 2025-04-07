//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.5
// ----------------------------------------------------------------------------
// APASSInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __APASSInstance_h
#define __APASSInstance_h

#include <pcl/APASSDatabaseFile.h>
#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class APASSInstance : public ProcessImplementation
{
public:

   APASSInstance( const MetaProcess* );
   APASSInstance( const APASSInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   int OutputDataRelease() const;

private:

   String          p_command;
   int32           p_verbosity;   // Verbosity level: 0=silent 1=minimal 2=full
   pcl_enum        p_dataRelease; // DR9, DR10, BestAvailable
   pcl_enum        p_sortBy;
   pcl_bool        p_generateTextOutput;
   pcl_bool        p_generateBinaryOutput;
   pcl_enum        p_textFormat;
   pcl_enum        p_textHeaders;
   String          p_outputFilePath;
   APASSSearchData p_searchData;
   StringList      p_databaseFilePaths;
   pcl_bool        o_isValid = false;
   pcl_enum        o_dataRelease = -1; // DR9, DR10
   float           o_databaseMagnitudeLow = 0;
   float           o_databaseMagnitudeHigh = 0;

   void Search();
   void GenerateTextOutput() const;
   void GenerateBinaryOutput() const;
   void GetInfo();
   void Configure();

   friend class APASSInterface;
   friend class APASSPreferencesDialog;
};

// ----------------------------------------------------------------------------

struct APASSBinaryHeader
{
   uint32 version = 0;
   uint32 reserved1 = 0;
   uint32 reserved2 = 0;
   uint32 reserved3 = 0;
   uint32 dataRelease = 0;
   uint32 sourceCount = 0;
   uint32 reserved4 = 0;
   uint32 reserved5 = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __APASSInstance_h

// ----------------------------------------------------------------------------
// EOF APASSInstance.h - Released 2025-04-07T08:53:55Z
