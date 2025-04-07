//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// FilterManagerParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FilterManagerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FMTask*          TheFMTaskParameter = nullptr;
FMPath*          TheFMPathParameter = nullptr;
FMNoGUIMessages* TheFMNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

FMTask::FMTask( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheFMTaskParameter = this;
}

IsoString FMTask::Id() const
{
   return "task";
}

size_type FMTask::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString FMTask::ElementId( size_type index ) const
{
   switch ( index )
   {
   default:
   case NoAction:     return "Task_NoAction";
   case ImportXML:    return "Task_ImportXML";
   case MergeXML:     return "Task_MergeXML";
   case ImportCSV:    return "Task_ImportCSV";
   case MergeCSV:     return "Task_MergeCSV";
   case ExportXML:    return "Task_ExportXML";
   case ExportCSV:    return "Task_ExportCSV";
   case MergeDefault: return "Task_MergeDefault";
   case SetDefault:   return "Task_SetDefault";
   }
}

int FMTask::ElementValue( size_type index ) const
{
   return int( index );
}

size_type FMTask::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

FMPath::FMPath( MetaProcess* P )
   : MetaString( P )
{
   TheFMPathParameter = this;
}

IsoString FMPath::Id() const
{
   return "path";
}

// ----------------------------------------------------------------------------

FMNoGUIMessages::FMNoGUIMessages( MetaProcess* P )
   : MetaBoolean( P )
{
   TheFMNoGUIMessagesParameter = this;
}

IsoString FMNoGUIMessages::Id() const
{
   return "noGUIMessages";
}

bool FMNoGUIMessages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterManagerParameters.cpp - Released 2025-04-07T08:53:56Z
