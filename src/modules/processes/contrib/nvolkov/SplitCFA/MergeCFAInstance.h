//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// MergeCFAInstance.h - Released 2025-04-07T08:53:56Z
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

#ifndef __MergeCFAInstance_h
#define __MergeCFAInstance_h

#include <pcl/ImageWindow.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class MergeCFAInstance : public ProcessImplementation
{
public:

   MergeCFAInstance( const MetaProcess* );
   MergeCFAInstance( const MergeCFAInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   StringList p_viewId = StringList( size_type( 4 ) );
   String o_outputViewId;

   int m_width, m_height, m_bitsPerSample, m_numberOfChannels;
   bool m_isFloatSample, m_isColor;

   View GetView( const int );

   friend class MergeCFAProcess;
   friend class MergeCFAInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __MergeCFAInstance_h

// ----------------------------------------------------------------------------
// EOF MergeCFAInstance.h - Released 2025-04-07T08:53:56Z
