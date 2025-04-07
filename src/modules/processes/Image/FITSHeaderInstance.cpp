//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// FITSHeaderInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FITSHeaderInstance.h"

#include <pcl/View.h>
#include <pcl/ImageWindow.h> // for UndoFlags

namespace pcl
{

// ----------------------------------------------------------------------------

FITSHeaderInstance::FITSHeaderInstance( const MetaProcess* m )
   : ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

FITSHeaderInstance::FITSHeaderInstance( const FITSHeaderInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void FITSHeaderInstance::Assign( const ProcessImplementation& p )
{
   const FITSHeaderInstance* x = dynamic_cast<const FITSHeaderInstance*>( &p );
   if ( x != nullptr )
      keywords = x->keywords;
}

// ----------------------------------------------------------------------------

UndoFlags FITSHeaderInstance::UndoMode( const View& ) const
{
   return UndoFlag::Keywords;
}

// ----------------------------------------------------------------------------

bool FITSHeaderInstance::IsMaskable( const View&, const ImageWindow& /*mask*/ ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool FITSHeaderInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( !v.IsMainView() )
   {
      whyNot = "FITSHeader can only be executed on main views, not on previews.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool FITSHeaderInstance::ExecuteOn( View& view )
{
   FITSKeywordArray K, K0;

   // Collect current reserved keywords
   view.Window().GetKeywords( K0 );
   for ( FITSKeywordArray::iterator i = K0.Begin(); i != K0.End(); ++i )
      if ( IsReservedKeyword( i->name ) )
         K << *i;

   // Append new non-reserved keywords
   for ( keyword_list::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
      if ( !IsReservedKeyword( i->name ) )
         K << FITSHeaderKeyword( IsoString( i->name ), IsoString( i->value ), IsoString( i->comment ) );

   // Update keyword set
   view.Window().SetKeywords( K );

   // Done Ok
   return true;
}

// ----------------------------------------------------------------------------

void* FITSHeaderInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheFITSKeywordNameParameter )
      return keywords[tableRow].name.Begin();
   if ( p == TheFITSKeywordValueParameter )
      return keywords[tableRow].value.Begin();
   if ( p == TheFITSKeywordCommentParameter )
      return keywords[tableRow].comment.Begin();
   return nullptr;
}

// ----------------------------------------------------------------------------

bool FITSHeaderInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheFITSKeywordNameParameter )
   {
      keywords[tableRow].name.Clear();
      if ( sizeOrLength > 0 )
         keywords[tableRow].name.SetLength( sizeOrLength );
   }
   else if ( p == TheFITSKeywordValueParameter )
   {
      keywords[tableRow].value.Clear();
      if ( sizeOrLength > 0 )
         keywords[tableRow].value.SetLength( sizeOrLength );
   }
   else if ( p == TheFITSKeywordCommentParameter )
   {
      keywords[tableRow].comment.Clear();
      if ( sizeOrLength > 0 )
         keywords[tableRow].comment.SetLength( sizeOrLength );
   }
   else if ( p == TheFITSKeywordTableParameter )
   {
      keywords.Clear();
      if ( sizeOrLength > 0 )
         keywords.Add( Keyword(), sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type FITSHeaderInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheFITSKeywordNameParameter )
      return keywords[tableRow].name.Length();
   if ( p == TheFITSKeywordValueParameter )
      return keywords[tableRow].value.Length();
   if ( p == TheFITSKeywordCommentParameter )
      return keywords[tableRow].comment.Length();
   if ( p == TheFITSKeywordTableParameter )
      return keywords.Length();
   return 0;
}

// ----------------------------------------------------------------------------

void FITSHeaderInstance::ImportKeywords( const ImageWindow& w )
{
   keywords.Clear();

   FITSKeywordArray K;
   w.GetKeywords( K );

   for ( FITSKeywordArray::const_iterator i = K.Begin(); i != K.End(); ++i )
      keywords << Keyword( i->name, i->value, i->comment );
}

// ----------------------------------------------------------------------------

bool FITSHeaderInstance::IsReservedKeyword( const String& id )
{
   static const char* cReserved[] =
   {
      // Structural/Critical keywords
      "SIMPLE",
      "BITPIX",
      "NAXIS",
      "NAXIS1",
      "NAXIS2",
      "NAXIS3",
      "NAXIS4",
      "NAXIS5",
      "NAXIS6",
      "NAXIS7",
      "NAXIS8",
      "NAXIS9",
      "NAXIS10",
      "EXTEND",
      "BSCALE",
      "BZERO",
      "EXTNAME",

      // PixInsight reserved keywords
      "PINSIGHT",
      "COLORSPC",
      "ALPHACHN",
      "RESOUNIT",
      "RESOLUTN",
      "XRESOLTN",
      "YRESOLTN",
      "ICCPROFL",
      "THUMBIMG",

      nullptr
   };

   static SortedStringList reserved;
   if ( reserved.IsEmpty() )
      for ( const char** i = cReserved; *i != nullptr; ++i )
         reserved << *i;

   return reserved.Contains( id.Uppercase() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSHeaderInstance.cpp - Released 2025-04-07T08:53:56Z
