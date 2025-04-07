//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Functional.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "Data.h"
#include "Functional.h"

namespace pcl
{

// ----------------------------------------------------------------------------

String Functional::PostOrder() const
{
   String s;
   if ( !arguments.IsEmpty() )
   {
      for ( argument_list::const_iterator i = arguments.Begin(); ; )
      {
         Functional* f = dynamic_cast<Functional*>( *i );
         if ( f != nullptr )
            s << f->PostOrder();
         else
            s << (*i)->ToString();
         if ( ++i == arguments.End() )
            break;
         s << ',';
      }

      s << ',';
   }
   return s << Token() << '(' + String( arguments.Length() ) << ')';
}

// ----------------------------------------------------------------------------

void Functional::PostOrder( ExpressionList& components, bool optimize ) const
{
   components.Destroy();
   PostOrderRecursive( components, optimize );
}

void Functional::PostOrderRecursive( ExpressionList& components, bool optimize ) const
{
   if ( optimize && CanOptimize() )
   {
      ExpressionList optimized = Optimized();
      for ( const Expression* xpr : optimized )
      {
         const Functional* f = dynamic_cast<const Functional*>( xpr );
         if ( f != nullptr )
            f->PostOrderRecursive( components, optimize );
         else
            components << xpr->Clone();
      }
      optimized.Destroy();
   }
   else
   {
      for ( const Expression* arg : arguments )
      {
         const Functional* f = dynamic_cast<const Functional*>( arg );
         if ( f != nullptr )
            f->PostOrderRecursive( components, optimize );
         else
            components << arg->Clone();
      }
      const Functional* f = dynamic_cast<const Functional*>( this );
      if ( f != nullptr )
         components << f->Clone();
      else
         components << Clone();
   }
}

// ----------------------------------------------------------------------------

void Functional::CheckParsedGlobalVariables( const String& beingParsed ) const
{
   for ( const Expression* arg : arguments )
   {
      const Functional* f = dynamic_cast<const Functional*>( arg );
      if ( f != nullptr )
         f->CheckParsedGlobalVariables( beingParsed );
      else if ( arg->IsVariableReference() )
      {
         const Variable& v = static_cast<const VariableReference*>( arg )->Reference();
         if ( v.IsGlobalVariable() )
            if ( !arg->HasLValueRole() )
               throw ParseError( "Illegal rvalue role of global variable '"
                                 + static_cast<const GlobalVariable&>( v ).QualifiedId()
                                 + "'"
                                 , beingParsed
                                 , arg->TokenPosition() );
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Functional.cpp - Released 2025-04-07T08:53:56Z
