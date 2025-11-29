//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/UIControlObject.cpp - Released 2025-04-07T08:53:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pcl/Array.h>
#include <pcl/AutoLock.h>
#include <pcl/UIControlObject.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

//#define __PCL_DEBUG_UIOBJECT_INDEX 1

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
#  include <iostream>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class UIEventDispatcher
{
public:

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
   static void api_func HandleDestroyed( api_handle );
#endif
};

// ----------------------------------------------------------------------------

class UIControlObjectIndex
{
public:

   struct IndexItem
   {
      api_handle  handle;
      size_type   count = 0;

      IndexItem( UIControlObject* object )
         : handle( object->handle )
      {
         Add( object );
      }

      IndexItem( api_handle h, int )
         : handle( h )
      {
      }

      IndexItem( const IndexItem& ) = default;

      bool operator ==( const IndexItem& item ) const
      {
         return handle == item.handle;
      }

      bool operator <( const IndexItem& item ) const
      {
         return handle < item.handle;
      }

      bool IsEmpty() const
      {
         return count == 0;
      }

      void Add( UIControlObject* object )
      {
         if ( object == nullptr )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIControlObjectIndex::IndexItem::Add() : Null object pointer.\n"
                      << std::flush;
#endif
            return;
         }

         if ( object->handle != handle )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIControlObjectIndex::IndexItem::Add() : Invalid object (" << object->ObjectType() << ") "
                      << IsoString().Format( "at address %p with handle %p, expected %p\n", (void*)object, object->handle, handle )
                      << std::flush;
#endif
            return;
         }

         object->alias = count > 0;
         ++count;
      }

      void Remove( UIControlObject* object )
      {
         if ( object == nullptr )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIControlObjectIndex::IndexItem::Remove() : Null object pointer.\n"
                      << std::flush;
#endif
            return;
         }

         if ( object->handle != handle )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIControlObjectIndex::IndexItem::Remove() : Invalid object (" << object->ObjectType() << ") "
                      << IsoString().Format( "at address %p with handle %p, expected %p\n", (void*)object, object->handle, handle )
                      << std::flush;
#endif
            return;
         }

         object->handle = nullptr;
         object->alias = false;

         if ( count == 0 )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIControlObjectIndex::IndexItem::Remove() : Empty index item.\n"
                      << std::flush;
#endif
            return;
         }

         --count;
      }
   };

   using index_implementation = SortedArray<IndexItem>;
   using iterator             = index_implementation::iterator;
   using const_iterator       = index_implementation::const_iterator;

   UIControlObjectIndex() = default;

   void Add( UIControlObject* object )
   {
      if ( object == nullptr )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIControlObjectIndex::Add() : Null object pointer.\n"
                   << std::flush;
#endif
         return;
      }

      if ( object->IsNull() )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIControlObjectIndex::Add() : Invalid null object (" << object->ObjectType() << ") "
                   << IsoString().Format( "at address %p\n", (void*)object )
                   << std::flush;
#endif
         return;
      }

      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( object->handle, 0 ) );
         if ( i != m_index.End() )
            m_index.MutableIterator( i )->Add( object );
         else
            m_index.Add( IndexItem( object ) );
      }

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
      (API->UI->SetHandleDestroyedEventRoutine)( object->handle, UIEventDispatcher::HandleDestroyed );
#endif
   }

   void Remove( UIControlObject* object )
   {
      if ( object == nullptr )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIControlObjectIndex::Remove() : Null object pointer.\n"
                   << std::flush;
#endif
         return;
      }

      if ( object->IsNull() )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIControlObjectIndex::Remove() : Invalid null object (" << object->ObjectType() << ") "
                   << IsoString().Format( "at address %p\n", (void*)object )
                   << std::flush;
#endif
         return;
      }

      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( object->handle, 0 ) );
         if ( i != m_index.End() )
         {
            iterator m = m_index.MutableIterator( i );
            m->Remove( object );
            if ( m->IsEmpty() )
               m_index.Remove( m );
         }
      }
   }

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
   void HandleDestroyed( api_handle handle )
   {
      if ( handle == nullptr )
      {
         std::cerr << "\n** Warning: UIControlObjectIndex::HandleDestroyed() : Invalid zero handle.\n" << std::flush;
         return;
      }

      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( handle, 0 ) );
         if ( i != m_index.End() )
         {
            std::cerr << "\n** Warning: UIControlObjectIndex::HandleDestroyed() : "
                      << IsoString().Format( "handle %p invalidates %u existing object(s)\n", handle, i->count )
                      << std::flush;
            m_index.Remove( m_index.MutableIterator( i ) );
         }
      }
   }
#endif

private:

           index_implementation m_index;
   mutable Mutex                m_mutex;
};

static UIControlObjectIndex s_objects;

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
void api_func UIEventDispatcher::HandleDestroyed( api_handle handle )
{
   s_objects.HandleDestroyed( handle );
}
#endif

// ----------------------------------------------------------------------------

UIControlObject::UIControlObject( control_handle h )
   : handle( h )
{
   if ( handle != nullptr )
      s_objects.Add( this );
}

UIControlObject::UIControlObject( const_control_handle h )
   : handle( const_cast<control_handle>( h ) )
{
   if ( handle != nullptr )
      s_objects.Add( this );
}

// ----------------------------------------------------------------------------

UIControlObject::UIControlObject( const UIControlObject& x )
   : handle( x.handle )
{
   if ( handle != nullptr )
   {
      if ( (API->UI->AttachToUIControlObject)( ModuleHandle(), handle ) == api_false )
      {
         handle = nullptr;
         throw APIFunctionError( "AttachToUIControlObject" );
      }

      s_objects.Add( this );
   }
}

// ----------------------------------------------------------------------------

UIControlObject::~UIControlObject() noexcept( false )
{
   if ( handle != nullptr )
   {
      api_handle theHandle = handle;

      s_objects.Remove( this );

      if ( (API->UI->GetUIControlObjectRefCount)( theHandle ) > 0 )
         if ( (API->UI->DetachFromUIControlObject)( ModuleHandle(), theHandle ) == api_false )
            throw APIFunctionError( "DetachFromUIControlObject" );
   }
}

// ----------------------------------------------------------------------------

UIControlObject& UIControlObject::Null()
{
   static UIControlObject* nullUIControlObject = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullUIControlObject == nullptr )
      nullUIControlObject = new UIControlObject( nullptr );
   return *nullUIControlObject;
}

// ----------------------------------------------------------------------------

size_type UIControlObject::RefCount() const
{
   return (handle != nullptr) ? (API->UI->GetUIControlObjectRefCount)( handle ) : size_type( 0 );
}

// ----------------------------------------------------------------------------

void UIControlObject::EnsureUnique()
{
   if ( handle != nullptr )
      if ( !IsUnique() )
         SetHandle( CloneHandle() );
}

// ----------------------------------------------------------------------------

IsoString UIControlObject::ObjectType() const
{
   if ( handle == nullptr )
      return "Null";

   size_type len = 0;
   (API->UI->GetUIControlObjectType)( handle, nullptr, &len );
   if ( len == 0 )
      throw APIFunctionError( "GetUIControlObjectType" );

   IsoString objType;
   objType.SetLength( len );
   if ( (API->UI->GetUIControlObjectType)( handle, objType.Begin(), &len ) == api_false )
      throw APIFunctionError( "GetUIControlObjectType" );
   objType.ResizeToNullTerminated();
   return objType;
}

// ----------------------------------------------------------------------------

String UIControlObject::ObjectId() const
{
   if ( handle == nullptr )
      return String();

   size_type len = 0;
   (API->UI->GetUIControlObjectId)( handle, nullptr, &len );

   String objId;
   if ( len > 0 )
   {
      objId.SetLength( len );
      if ( (API->UI->GetUIControlObjectId)( handle, objId.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetUIControlObjectId" );
      objId.ResizeToNullTerminated();
   }
   return objId;
}

// ----------------------------------------------------------------------------

void UIControlObject::SetObjectId( const String& id )
{
   if ( (API->UI->SetUIControlObjectId)( handle, id.c_str() ) == api_false )
      throw APIFunctionError( "SetUIControlObjectId" );
}

// ----------------------------------------------------------------------------

void UIControlObject::SetHandle( void* newHandle )
{
   if ( newHandle != handle )
   {
      if ( handle != nullptr )
      {
         api_handle oldHandle = handle;

         s_objects.Remove( this );

         if ( (API->UI->GetUIControlObjectRefCount)( oldHandle ) > 0 )
            if ( (API->UI->DetachFromUIControlObject)( ModuleHandle(), oldHandle ) == api_false )
               throw APIFunctionError( "DetachFromUIControlObject" );
      }

      if ( newHandle != nullptr )
      {
         if ( (API->UI->AttachToUIControlObject)( ModuleHandle(), newHandle ) == api_false )
            throw APIFunctionError( "AttachToUIControlObject" );

         handle = newHandle;
         s_objects.Add( this );
      }
   }
}

// ----------------------------------------------------------------------------

void UIControlObject::TransferHandle( void* newHandle )
{
   if ( newHandle != handle )
   {
      if ( handle != nullptr )
      {
         api_handle oldHandle = handle;

         s_objects.Remove( this );

         if ( (API->UI->GetUIControlObjectRefCount)( oldHandle ) > 0 )
            if ( (API->UI->DetachFromUIControlObject)( ModuleHandle(), oldHandle ) == api_false )
               throw APIFunctionError( "DetachFromUIControlObject" );
      }

      if ( newHandle != nullptr )
      {
         handle = newHandle;
         s_objects.Add( this );
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/UIControlObject.cpp - Released 2025-04-07T08:53:32Z
