//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// FlowControl.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FlowControl_h
#define __FlowControl_h

#include "Expression.h"

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Representation of a flow control operation in an optimized PixelMath
 * expression.
 */
class Branch : public Expression
{
public:

   /*
    * Implemented flow control primitives.
    *
    * Branch operations that pop stack elements are 'consumers'. A pop
    * operation is necessary when a conditional expression is evaluated and its
    * value is no longer part of the post-order expression evaluation sequence.
    */
   enum opcode { JMP,    // Unconditional branch
                 JZ,     // Branch if zero
                 JNZ,    // Branch if nonzero
                 JZP,    // Branch if zero and pop
                 JNZP }; // Branch if nonzero and pop

   /*
    * Construction with the specified branch primitive operation, pointer
    * identifier, and token position.
    */
   Branch( opcode op, unsigned targetId, int p )
      : Expression( XPR_BRANCH, p )
      , m_opcode( op )
      , m_targetId( targetId )
   {
   }

   /*
    * Copy constructor.
    */
   Branch( const Branch& ) = default;

   /*
    * Returns the type of flow control operation.
    */
   opcode OpCode() const
   {
      return m_opcode;
   }

   /*
    * Returns the identifier of a pointer structure for intermediate optimized
    * representation.
    */
   unsigned TargetId() const
   {
      return m_targetId;
   }

   /*
    * Returns the branch offset (in stack elements) for optimized execution.
    */
   distance_type TargetOffset() const
   {
      return m_targetOffset;
   }

   /*
    * Sets the branch offset (in stack elements) for optimized execution.
    */
   void SetTargetOffset( distance_type offset )
   {
      m_targetOffset = offset;
   }

   virtual Expression* Clone() const = 0;
   virtual String ToString() const = 0;

private:

   opcode        m_opcode;
   unsigned      m_targetId;
   distance_type m_targetOffset = 0;
};

// ----------------------------------------------------------------------------

class BranchOnZero : public Branch
{
public:

   BranchOnZero( unsigned targetId, int p )
      : Branch( JZ, targetId, p )
   {
   }

   BranchOnZero( const BranchOnZero& ) = default;

   Expression* Clone() const override
   {
      return new BranchOnZero( *this );
   }

   String ToString() const override
   {
      return String().Format( "jz L%03u(%+ld)", TargetId(), TargetOffset() );
   }
};

// ----------------------------------------------------------------------------

class BranchOnZeroAndPop : public Branch
{
public:

   BranchOnZeroAndPop( unsigned targetId, int p )
      : Branch( JZP, targetId, p )
   {
   }

   BranchOnZeroAndPop( const BranchOnZeroAndPop& ) = default;

   Expression* Clone() const override
   {
      return new BranchOnZeroAndPop( *this );
   }

   String ToString() const override
   {
      return String().Format( "jzp L%03u(%+ld)", TargetId(), TargetOffset() );
   }
};

// ----------------------------------------------------------------------------

class BranchOnNonZero : public Branch
{
public:

   BranchOnNonZero( unsigned targetId, int p )
      : Branch( JNZ, targetId, p )
   {
   }

   BranchOnNonZero( const BranchOnNonZero& ) = default;

   Expression* Clone() const override
   {
      return new BranchOnNonZero( *this );
   }

   String ToString() const override
   {
      return String().Format( "jnz L%03u(%+ld)", TargetId(), TargetOffset() );
   }
};

// ----------------------------------------------------------------------------

class BranchOnNonZeroAndPop : public Branch
{
public:

   BranchOnNonZeroAndPop( unsigned targetId, int p )
      : Branch( JNZP, targetId, p )
   {
   }

   BranchOnNonZeroAndPop( const BranchOnNonZeroAndPop& ) = default;

   Expression* Clone() const override
   {
      return new BranchOnNonZeroAndPop( *this );
   }

   String ToString() const override
   {
      return String().Format( "jnzp L%03u(%+ld)", TargetId(), TargetOffset() );
   }
};

// ----------------------------------------------------------------------------

class UnconditionalBranch : public Branch
{
public:

   UnconditionalBranch( unsigned targetId, int p )
      : Branch( JMP, targetId, p )
   {
   }

   UnconditionalBranch( const UnconditionalBranch& ) = default;

   Expression* Clone() const override
   {
      return new UnconditionalBranch( *this );
   }

   String ToString() const override
   {
      return String().Format( "jmp L%03u(%+ld)", TargetId(), TargetOffset() );
   }
};

// ----------------------------------------------------------------------------

/*
 * A pointer is a temporary element used to reference brach target locations
 * for code optimization. Pointers are only used by expression optimization
 * routines. For optimized execution, all pointers are replaced by their
 * corresponding branch offsets in branch elements.
 */
class Pointer : public Expression
{
public:

   Pointer( unsigned id, int p = 0 )
      : Expression( XPR_POINTER, p )
      , m_id( id )
   {
   }

   Pointer( const Pointer& ) = default;

   unsigned Id() const
   {
      return m_id;
   }

   Expression* Clone() const override
   {
      return new Pointer( *this );
   }

   String ToString() const override
   {
      return String().Format( "L%03u", m_id );
   }

   static unsigned NextId()
   {
      static unsigned s_id = 0;
      return ++s_id;
   }

private:

   unsigned m_id;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FlowControl_h

// ----------------------------------------------------------------------------
// EOF FlowControl.h - Released 2025-04-07T08:53:56Z
