//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// Structure.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __Structure_h
#define __Structure_h

#include <pcl/ByteArray.h>
#include <pcl/IndirectSortedArray.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class File;

class Structure
{
public:

   typedef Array<ByteArray>   structure_mask_list;

   Structure( int nw = 1, int sz = 3, const String& nm = String() );

   Structure( const Structure& ) = default;

   Structure( File& file )
   {
      Read( file );
   }

   Structure( const IsoString& key )
   {
      Load( key );
   }

   String Name() const
   {
      return name;
   }

   void Rename( const String& n )
   {
      name = n;
      name.Trim();
   }

   bool operator ==( const Structure& x ) const
   {
      return name == x.name && SameStructure( x );
   }

   bool operator <( const Structure& x ) const
   {
      return name.CompareIC( x.name ) < 0;
   }

   bool SameStructure( const Structure& ) const;

   int Size() const
   {
      return int( size );
   }

   void Resize( int sz );

   int NumberOfWays() const
   {
      return int( mask.Length() );
   }

   int NumberOfElements() const
   {
      return int( size*size );
   }

   bool IsEmptyWay( int w )
   {
      return *MaxItem( mask[w].Begin(), mask[w].End() ) == 0;
   }

   bool Element( int w, int x, int y ) const
   {
      return mask[w][y*size + x] != 0;
   }

   void SetElement( int w, int x, int y, bool set = true )
   {
      mask[w][y*size + x] = set ? 1 : 0;
   }

   void ClearElement( int w, int x, int y, bool clr = true )
   {
      SetElement( w, x, y, !clr );
   }

   void ClearWay( int w )
   {
      Fill( mask[w].Begin(), mask[w].End(), uint8( 0 ) );
   }

   void SetWay( int w )
   {
      Fill( mask[w].Begin(), mask[w].End(), uint8( 1 ) );
   }

   void GetWay( int w, ByteArray& b ) const
   {
      b = mask[w];
   }

   void SetWay( int w, const ByteArray& b )
   {
      mask[w] = b;
   }

   void RemoveWay( int w, ByteArray& b )
   {
      GetWay( w, b );
      RemoveWay( w );
   }

   void RemoveWay( int w )
   {
      mask.Remove( mask.At( w ) );
   }

   void AddWay( const ByteArray& b )
   {
      mask.Add( b );
   }

   void AddWay( bool set = false )
   {
      mask.Add( ByteArray() );
      (*mask.ReverseBegin()).Add( uint8( set ? 1 : 0 ), size*size );
   }

   void InsertWay( int w, const ByteArray& b )
   {
      mask.Insert( mask.At( w ), b );
   }

   void InsertWay( int w, bool set = false )
   {
      mask.Insert( mask.At( w ), ByteArray() );
      mask[w].Add( uint8( set ? 1 : 0 ), size*size );
   }

   void Clear()
   {
      for ( int i = 0; i < NumberOfWays(); ++i )
         ClearWay( i );
   }

   void Set()
   {
      for ( int i = 0; i < NumberOfWays(); ++i )
         SetWay( i );
   }

   void Invert( int w );
   void Rotate( int w );
   void Circular( int w );
   void Diamond( int w );
   void Orthogonal( int w );
   void Diagonal( int w );

   void Read( File& );
   void Write( File& ) const;

   void Load( const IsoString& key );
   void Save( const IsoString& key ) const;

private:

   String              name;
   uint32              size = 0;
   structure_mask_list mask;

   friend class MorphologicalTransformationInstance;
   friend class MorphologicalTransformationInterface;
};

// ----------------------------------------------------------------------------

class StructureCollection
{
public:

   struct IndexNode
   {
      String    name;
      size_type index;

      IndexNode( const String& s, size_type i = 0 ) : name( s ), index( i )
      {
      }

      IndexNode( const IndexNode& ) = default;

      bool operator ==( const IndexNode& n ) const
      {
         return name == n.name;
      }

      bool operator <( const IndexNode& n ) const
      {
         return name < n.name;
      }
   };

   typedef IndirectArray<Structure>       structure_list;
   typedef IndirectSortedArray<IndexNode> index_list;

   static const size_type notFound = ~size_type( 0 );

   StructureCollection() = default;

   virtual ~StructureCollection()
   {
      Destroy();
   }

   void Read( const String& filePath );
   void Write( const String& filePath = String() );

   void Load( const IsoString& key );
   void Save( const IsoString& key ) const;

   String Path() const
   {
      return path;
   }

   size_type Length() const
   {
      return index.Length();
   }

   bool IsEmpty() const
   {
      return index.IsEmpty();
   }

   const Structure& operator[]( size_type i ) const
   {
      return *collection[index[i]->index];
   }

   Structure& operator[]( size_type i )
   {
      return *collection[index[i]->index];
   }

   void AddStructure( const Structure& s )
   {
      collection.Add( new Structure( s ) );
      index.Add( new IndexNode( s.Name(), collection.UpperBound() ) );
   }

   void DeleteStructure( size_type i )
   {
      collection.Delete( collection.At( index[i]->index ) );
      index.Destroy( index.MutableAt( i ) );
   }

   void Reindex()
   {
      index.Destroy();
      collection.Pack();
      for ( size_type i = 0; i < collection.Length(); ++i )
         index.Add( new IndexNode( collection[i]->Name(), i ) );
   }

   void Destroy()
   {
      path.Clear();
      collection.Destroy();
      index.Destroy();
   }

   size_type FindStructure( const String& name ) const
   {
      index_list::const_iterator i = index.Search( IndexNode( name ) );
      return (i != index.End()) ? i-index.Begin() : notFound;
   }

   size_type FindStructure( const Structure& s ) const
   {
      return FindStructure( s.Name() );
   }

   bool HasStructure( const String& name ) const
   {
      return FindStructure( name ) != notFound;
   }

   bool HasStructure( const Structure& s ) const
   {
      return FindStructure( s ) != notFound;
   }

private:

   String         path;
   structure_list collection;
   index_list     index;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Structure_h

// ----------------------------------------------------------------------------
// EOF Structure.h - Released 2025-04-07T08:53:56Z
