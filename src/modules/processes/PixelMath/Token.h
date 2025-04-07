//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Token.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __Token_h
#define __Token_h

#include <pcl/IndirectArray.h>
#include <pcl/String.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * A token in a raw PixelMath expression analysis. Tokens are generated and
 * consumed by expression parser routines.
 *
 * See Expression::Parse() and the Tokenize() function in Token.cpp.
 */
class Token
{
public:

   enum { Symbol, MetaSymbol, Numeric, Separator };

   Token( const String& str, int pos, bool meta = false )
      : m_type( meta ? MetaSymbol : Symbol )
      , m_pos( pos )
      , m_stringValue( str )
   {
   }

   Token( double num, int pos )
      : m_type( Numeric )
      , m_pos( pos )
      , m_numericValue( num )
   {
   }

   Token( char16_type sep, int pos )
      : m_type( Separator )
      , m_pos( pos )
      , m_separatorValue( sep )
   {
   }

   int Type() const
   {
      return m_type;
   }

   bool IsSymbol() const
   {
      return m_type == Symbol;
   }

   bool IsMetaSymbol() const
   {
      return m_type == MetaSymbol;
   }

   bool IsNumeric() const
   {
      return m_type == Numeric;
   }

   bool IsSeparator() const
   {
      return m_type == Separator;
   }

   int Position() const
   {
      return m_pos;
   }

   String ToString() const
   {
      if ( IsSymbol() || IsMetaSymbol() )
         return m_stringValue;
      if ( IsNumeric() )
         return String( m_numericValue );
      if ( IsSeparator() )
         return String( m_separatorValue, 1 );
      return String();
   }

   const String& AsString() const
   {
      return m_stringValue;
   }

   double AsNumeric() const
   {
      return m_numericValue;
   }

   char16_type AsSeparator() const
   {
      return m_separatorValue;
   }

private:

   int         m_type;
   int         m_pos;
   String      m_stringValue;
   double      m_numericValue   = 0;
   char16_type m_separatorValue = 0;
};

// ----------------------------------------------------------------------------

class Directive
{
public:

   Directive( const String& name, const StringList& arguments, int pos, const Array<int>& argPos )
      : m_name( name )
      , m_arguments( arguments )
      , m_pos( pos )
      , m_argPos( argPos )
   {
   }

   const String& Name() const
   {
      return m_name;
   }

   const StringList& Arguments() const
   {
      return m_arguments;
   }

   int Position() const
   {
      return m_pos;
   }

   int ArgumentPosition( int i ) const
   {
      return m_argPos[Range( i, 0, int( m_argPos.Length() ) )];
   }

private:

   String     m_name;
   StringList m_arguments;
   int        m_pos;
   Array<int> m_argPos;

};

// ----------------------------------------------------------------------------

typedef IndirectArray<Token>     TokenList;
typedef Array<TokenList>         TokenSet;

typedef IndirectArray<Directive> DirectiveList;

void Tokenize( TokenSet& tokens, DirectiveList& directives, const String& text );

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Token_h

// ----------------------------------------------------------------------------
// EOF Token.h - Released 2025-04-07T08:53:56Z
