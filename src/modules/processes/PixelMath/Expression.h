//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Expression.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __Expression_h
#define __Expression_h

#include "Pixel.h"
#include "Symbol.h"
#include "Token.h"

#include <pcl/Point.h>
#include <pcl/Relational.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ImageVariant;
class PCL_CLASS PixelInterpolation;

// ----------------------------------------------------------------------------

#define  XPR_OPERATOR      0x00000001
#define  XPR_FUNCTION      0x00000002
#define  XPR_FUNCTIONAL    0x0000000f

#define  XPR_IMAGEREF      0x00000010
#define  XPR_VARREF        0x00000020
#define  XPR_CONSTREF      0x00000040
#define  XPR_PIXEL         0x00000100
#define  XPR_SAMPLE        0x00000200
#define  XPR_ARGLIST       0x00000800
#define  XPR_DATA          0x00000ff0

#define  XPR_BRANCH        0x00100000
#define  XPR_POINTER       0x00200000

// ----------------------------------------------------------------------------

/*
 * Representation of a generic PixelMath expression.
 */
class Expression
{
public:

   typedef IndirectArray<Expression>   component_list;

   Expression( unsigned t, int p )
      : m_type( t )
      , m_pos( p )
   {
   }

   Expression( const Expression& ) = default;

   virtual ~Expression()
   {
   }

   int TokenPosition() const
   {
      return m_pos;
   }

   unsigned Type() const
   {
      return m_type;
   }

   bool IsFunctional() const
   {
      return (m_type & XPR_FUNCTIONAL) != 0;
   }

   bool IsOperator() const
   {
      return (m_type & XPR_OPERATOR) != 0;
   }

   bool IsFunction() const
   {
      return (m_type & XPR_FUNCTION) != 0;
   }

   bool IsData() const
   {
      return (m_type & XPR_DATA) != 0;
   }

   bool IsImageReference() const
   {
      return (m_type & XPR_IMAGEREF) != 0;
   }

   bool IsVariableReference() const
   {
      return (m_type & XPR_VARREF) != 0;
   }

   bool IsConstantReference() const
   {
      return (m_type & XPR_CONSTREF) != 0;
   }

   bool IsPixel() const
   {
      return (m_type & XPR_PIXEL) != 0;
   }

   bool IsSample() const
   {
      return (m_type & XPR_SAMPLE) != 0;
   }

   bool IsArgumentList() const
   {
      return (m_type & XPR_ARGLIST) != 0;
   }

   bool IsBranch() const
   {
      return (m_type & XPR_BRANCH) != 0;
   }

   bool IsPointer() const
   {
      return (m_type & XPR_POINTER) != 0;
   }

   bool HasLValueRole() const
   {
      return m_isLvalue;
   }

   void SetLValueRole( bool set = true )
   {
      m_isLvalue = set;
   }

   virtual bool IsLValue() const
   {
      return false;
   }

   virtual Expression* Clone() const = 0;

   virtual String ToString() const = 0;

   static void Parse( component_list& expressions, TokenSet& tokens, const String& text );

private:

   unsigned m_type;             // component type
   int      m_pos;              // token position
   bool     m_isLvalue = false; // lvalue role set by the parser

   static Expression* Parse( TokenList& );      // parser stage 1
   static Expression* Parse( component_list& ); // parser stage 2
};

// ----------------------------------------------------------------------------

typedef Expression::component_list  ExpressionList;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Expression_h

// ----------------------------------------------------------------------------
// EOF Expression.h - Released 2025-04-07T08:53:56Z
