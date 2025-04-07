//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Functional.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __Functional_h
#define __Functional_h

#include "Expression.h"

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Abstract base class of all PixelMath functional expression components.
 */
class Functional : public Expression
{
public:

   typedef component_list  argument_list;

   Functional( unsigned t, int p )
      : Expression( t, p )
   {
   }

   Functional( const Functional& x )
      : Expression( x )
   {
      for ( const Expression* arg : x.arguments )
         arguments << arg->Clone();
   }

   virtual ~Functional()
   {
      DestroyArguments();
   }

   virtual Expression* Clone() const = 0;
   virtual Expression* Generate( int ) const = 0;

   virtual String Meta() const = 0;
   virtual String Id() const = 0;
   virtual String Token() const = 0;

   virtual String ToString() const = 0;

   virtual void operator()( Pixel&, PixelList::const_iterator, PixelList::const_iterator ) const = 0;

   virtual bool IsInvariant( ExpressionList::const_iterator, ExpressionList::const_iterator ) const
   {
      return false;
   }

   // Invariant function call
   virtual void operator()( Pixel&, ExpressionList::const_iterator, ExpressionList::const_iterator ) const
   {
   }

   virtual int NumberOfArguments() const
   {
      return int( arguments.Length() );
   }

   virtual bool CanOptimize() const
   {
      return false;
   }

   virtual ExpressionList Optimized() const
   {
      return ExpressionList();
   }

   bool IsParsed() const
   {
      return !arguments.IsEmpty();
   }

   const Expression& operator []( int i ) const
   {
      return *arguments[i];
   }

   void AddArgument( Expression* x )
   {
      arguments.Add( x );
   }

   void DestroyArguments()
   {
      arguments.Destroy();
   }

   bool operator ==( const Functional& x ) const
   {
      return Token() == x.Token();
   }

   bool operator <( const Functional& x ) const
   {
      return Token() < x.Token();
   }

   String PostOrder() const;

   void PostOrder( ExpressionList&, bool optimize = true ) const;

   void CheckParsedGlobalVariables( const String& beingParsed ) const;

protected:

   argument_list arguments;

private:

   void PostOrderRecursive( ExpressionList&, bool optimize ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Functional_h

// ----------------------------------------------------------------------------
// EOF Functional.h - Released 2025-04-07T08:53:56Z
