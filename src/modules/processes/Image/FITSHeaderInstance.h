//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// FITSHeaderInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FITSHeaderInstance_h
#define __FITSHeaderInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/FITSHeaderKeyword.h>

#include "FITSHeaderParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSHeaderInstance : public ProcessImplementation
{
public:

   struct Keyword
   {
      String name, value, comment;

      Keyword() = default;
      Keyword( const Keyword& ) = default;

      template <class S>
      Keyword( const S& n, const S& v, const S& c )
         : name( String( n ) )
         , value( String( v ) )
         , comment( String( c ) )
      {
      }
   };

   typedef Array<Keyword>  keyword_list;

   FITSHeaderInstance( const MetaProcess* );
   FITSHeaderInstance( const FITSHeaderInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   void ImportKeywords( const ImageWindow& );

   static bool IsReservedKeyword( const String& id );

private:

   keyword_list keywords;

   friend class FITSHeaderProcess;
   friend class FITSHeaderInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSHeaderInstance_h

// ----------------------------------------------------------------------------
// EOF FITSHeaderInstance.h - Released 2025-04-07T08:53:56Z
