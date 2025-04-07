//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// FilterManagerParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FilterManagerParameters_h
#define __FilterManagerParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class FMTask : public MetaEnumeration
{
public:

   enum { NoAction,
          ImportXML,
          MergeXML,
          ImportCSV,
          MergeCSV,
          ExportXML,
          ExportCSV,
          MergeDefault,
          SetDefault,
          NumberOfItems,
          Default = NoAction };

   FMTask( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern FMTask* TheFMTaskParameter;

// ----------------------------------------------------------------------------

class FMPath : public MetaString
{
public:

   FMPath( MetaProcess* );

   IsoString Id() const override;
};

extern FMPath* TheFMPathParameter;

// ----------------------------------------------------------------------------

class FMNoGUIMessages : public MetaBoolean
{
public:

   FMNoGUIMessages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FMNoGUIMessages* TheFMNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __FilterManagerParameters_h

// ----------------------------------------------------------------------------
// EOF FilterManagerParameters.h - Released 2025-04-07T08:53:56Z
