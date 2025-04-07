//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoOperation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// NoOperationInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoOperation PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "NoOperationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NoOperationInstance::NoOperationInstance( const MetaProcess* m )
   : ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

bool NoOperationInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool NoOperationInstance::IsMaskable( const View&, const ImageWindow& /*mask*/ ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool NoOperationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool NoOperationInstance::ExecuteOn( View& )
{
   return true;
}

// ----------------------------------------------------------------------------

bool NoOperationInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool NoOperationInstance::ExecuteGlobal()
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NoOperationInstance.cpp - Released 2025-04-07T08:53:56Z
