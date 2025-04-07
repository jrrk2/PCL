//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// GrayscaleRGBInstances.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __GrayscaleRGBInstances_h
#define __GrayscaleRGBInstances_h

#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvertToGrayscaleInstance : public ProcessImplementation
{
public:

   ConvertToGrayscaleInstance( const MetaProcess* );
   ConvertToGrayscaleInstance( const ConvertToGrayscaleInstance& );

   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
};

// ----------------------------------------------------------------------------

class ConvertToRGBColorInstance : public ProcessImplementation
{
public:

   ConvertToRGBColorInstance( const MetaProcess* );
   ConvertToRGBColorInstance( const ConvertToRGBColorInstance& );

   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GrayscaleRGBInstances_h

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBInstances.h - Released 2025-04-07T08:53:55Z
