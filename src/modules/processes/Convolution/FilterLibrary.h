//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// FilterLibrary.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FilterLibrary_h
#define __FilterLibrary_h

#include <pcl/Array.h>
#include <pcl/Exception.h>
#include <pcl/KernelFilter.h>
#include <pcl/SeparableFilter.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class Filter
{
public:

   Filter() = default;

   Filter( const Filter& x )
   {
      if ( x.IsValid() )
      {
         if ( x.IsSeparable() )
            filter.Set( x.Separable() );
         else
            filter.Set( x.Kernel() );
      }
   }

   Filter( const KernelFilter& k ) : filter( k )
   {
   }

   Filter( const SeparableFilter& s ) : filter( s )
   {
   }

   virtual ~Filter()
   {
   }

   Filter& operator =( const Filter& x )
   {
      if ( &x != this )
         filter = x.filter;
      return *this;
   }

   String Name() const
   {
      if ( !IsValid() )
         return String();
      return IsSeparable() ? Separable().Name() : Kernel().Name();
   }

   bool IsValid() const
   {
      return filter.IsValid();
   }

   bool IsSeparable() const
   {
      return filter.IsSeparable();
   }

   const KernelFilter& Kernel() const
   {
      return filter.AsKernelFilter();
   }

   const SeparableFilter& Separable() const
   {
      return filter.AsSeparableFilter();
   }

   bool IsHighPass() const
   {
      return IsSeparable() ? Separable().IsHighPassFilter() : Kernel().IsHighPassFilter();
   }

   IsoString ToSource() const;

   static Filter FromSource( const IsoString& src );

   template <class S>
   static Filter FromSource( const S& src )
   {
      return FromSource( IsoString( src ) );
   }

   bool operator ==( const Filter& x ) const
   {
      return IsSeparable() == x.IsSeparable() &&
             (IsSeparable() ? Separable() == x.Separable() : Kernel() == x.Kernel());
   }

   bool operator <( const Filter& x ) const
   {
      return Name() < x.Name();
   }

private:

   class FilterVariant
   {
   private:

      void* filter = nullptr;
      bool  separable = false;

#define KERNEL( f )     reinterpret_cast<KernelFilter*>( f )
#define SEPARABLE( f )  reinterpret_cast<SeparableFilter*>( f )

      void Destroy()
      {
         if ( filter != nullptr )
         {
            if ( separable )
               delete SEPARABLE( filter );
            else
               delete KERNEL( filter );
            filter = nullptr;
         }
      }

   public:

      FilterVariant() = default;

      FilterVariant( const KernelFilter& k )
      {
         filter = new KernelFilter( k );
      }

      FilterVariant( const SeparableFilter& s ) : separable( true )
      {
         filter = new SeparableFilter( s );
      }

      ~FilterVariant()
      {
         Destroy();
      }

      void operator =( const FilterVariant& x )
      {
         if ( &x != this )
         {
            Destroy();
            if ( x.IsValid() )
               if ( x.IsSeparable() )
               {
                  filter = new SeparableFilter( *SEPARABLE( x.filter ) );
                  separable = true;
               }
               else
               {
                  filter = new KernelFilter( *KERNEL( x.filter ) );
                  separable = false;
               }
         }
      }

      void Set( const KernelFilter& k )
      {
         Destroy();
         filter = new KernelFilter( k );
         separable = false;
      }

      void Set( const SeparableFilter& s )
      {
         Destroy();
         filter = new SeparableFilter( s );
         separable = true;
      }

      bool IsSeparable() const
      {
         return separable;
      }

      bool IsValid() const
      {
         return filter != nullptr;
      }

      const KernelFilter& AsKernelFilter() const
      {
         if ( !IsValid() )
            throw Error( "Internal error: invalid filter." );
         if ( IsSeparable() )
            throw Error( "Internal error: not a kernel filter." );
         return *KERNEL( filter );
      }

      const SeparableFilter& AsSeparableFilter() const
      {
         if ( !IsValid() )
            throw Error( "Internal error: invalid filter." );
         if ( !IsSeparable() )
            throw Error( "Internal error: not a separable filter." );
         return *SEPARABLE( filter );
      }

      bool IsHighPassFilter() const
      {
         if ( !IsValid() )
            throw Error( "Internal error: invalid filter." );
         return separable ? SEPARABLE( filter )->IsHighPassFilter() :
                            KERNEL( filter )->IsHighPassFilter();
      }

#undef KERNEL
#undef SEPARABLE
   };

   FilterVariant filter;

   // Special constructor for searching in a filter list.
   Filter( const String& name ) : filter( KernelFilter( name ) )
   {
   }

   friend class FilterLibrary;
};

// ----------------------------------------------------------------------------

class FilterParser
{
public:

   typedef SortedArray<Filter> filter_list;

   FilterParser( const IsoStringList& lines )
   {
      Parse( Tokenize( lines ) );
   }

   const filter_list& Filters() const
   {
      return filters;
   }

private:

   filter_list filters;

   struct Token
   {
      Token( const IsoString& s, int l, int c )
         : token( s.UTF8ToUTF16() )
         , line( l )
         , column( c )
      {
      }

      String token;
      int    line, column;
   };

   typedef Array<Token> token_list;

   enum { UnknownFilterType, KernelFilterType, SeparableFilterType };

   enum { UnknownState, FilterTypeState, FilterParameterState };

   token_list Tokenize( const IsoStringList& linesUTF8 );
   void Parse( const token_list& tokens );

   static void CaptureParameterValueTokens( token_list::const_iterator&, token_list::const_iterator& );
};

// ----------------------------------------------------------------------------

class FilterLibrary
{
public:

   typedef SortedArray<Filter> filter_list;

   FilterLibrary() = default;

   FilterLibrary( const String& _filePath )
   {
      Load( _filePath );
   }

   void Load( const String& filePath );
   void LoadDefaultLibrary();
   void Save();
   void SaveAs( const String& filePath );
   void New();

   bool IsNew() const
   {
      return filePath.IsEmpty();
   }

   bool IsWritable() const;

   String FilePath() const
   {
      return filePath;
   }

   size_type Length() const
   {
      return filters.Length();
   }

   bool IsEmpty() const
   {
      return filters.IsEmpty();
   }

   const Filter& operator[]( size_type i ) const
   {
      return filters[i];
   }

   const Filter* FilterByName( const String& filterName ) const
   {
      filter_list::const_iterator i = filters.Search( filterName );
      return (i == filters.End()) ? nullptr : i;
   }

   void Add( const Filter& f )
   {
      const Filter* f0 = FilterByName( f.Name() );
      if ( f0 == nullptr )
         filters.Add( f );
      else
      {
         *const_cast<Filter*>( f0 ) = f;
         filters.Sort();
      }
   }

   bool Remove( const Filter& f )
   {
      const Filter* f0 = FilterByName( f.Name() );
      if ( f0 == nullptr )
         return false;
      filters.Remove( f );
      return true;
   }

   static String DefaultLibraryPath();

private:

   filter_list filters;
   String      filePath;
};

// ----------------------------------------------------------------------------

extern FilterLibrary TheFilterLibrary;

// ----------------------------------------------------------------------------

} // pcl

#endif // __FilterLibrary_h

// ----------------------------------------------------------------------------
// EOF FilterLibrary.h - Released 2025-04-07T08:53:55Z
