//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ViewList.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ViewList.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ViewList::ViewList( Control& parent )
   : Control( (*API->ViewList->CreateViewList)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateViewList" );
}

// ----------------------------------------------------------------------------

bool ViewList::IncludesMainViews() const
{
   api_bool mainViews;
   (*API->ViewList->GetViewListContents)( handle, &mainViews, 0, 0 );
   return mainViews != api_false;
}

// ----------------------------------------------------------------------------

bool ViewList::IncludesPreviews() const
{
   api_bool previews;
   (*API->ViewList->GetViewListContents)( handle, 0, &previews, 0 );
   return previews != api_false;
}

// ----------------------------------------------------------------------------

bool ViewList::IncludesRealTimePreview() const
{
   api_bool realTimePreview;
   (*API->ViewList->GetViewListContents)( handle, 0, 0, &realTimePreview );
   return realTimePreview != api_false;
}

// ----------------------------------------------------------------------------

void ViewList::Regenerate( bool mainViews, bool previews, bool realTimePreview )
{
   (*API->ViewList->RegenerateViewList)( handle, mainViews, previews, realTimePreview );
}

// ----------------------------------------------------------------------------

View ViewList::ExcludedView() const
{
   return View( (*API->ViewList->GetViewListExcludedView)( handle ) );
}

// ----------------------------------------------------------------------------

void ViewList::ExcludeView( const View& v )
{
   (*API->ViewList->SetViewListExcludedView)( handle, v.handle );
}

// ----------------------------------------------------------------------------

View ViewList::CurrentView() const
{
   return View( (*API->ViewList->GetViewListCurrentView)( handle ) );
}

// ----------------------------------------------------------------------------

void ViewList::SelectView( const View& v )
{
   (*API->ViewList->SetViewListCurrentView)( handle, v.handle );
}

// ----------------------------------------------------------------------------

bool ViewList::HasView( const View& v )
{
   return (*API->ViewList->FindViewListView)( handle, v.handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ViewList::RemoveView( const View& v )
{
   (*API->ViewList->RemoveViewListView)( handle, v.handle );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ViewList*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ViewListEventDispatcher
{
public:

   static void api_func ViewSelected( control_handle hSender, control_handle hReceiver, view_handle hView )
   {
      if ( handlers->onViewSelected != nullptr )
      {
         View view( hView );
         (receiver->*handlers->onViewSelected)( *sender, view );
      }
   }

   static void api_func CurrentViewUpdated( control_handle hSender, control_handle hReceiver, view_handle hView )
   {
      if ( handlers->onCurrentViewUpdated != nullptr )
      {
         View view( hView );
         (receiver->*handlers->onCurrentViewUpdated)( *sender, view );
      }
   }

}; // ViewListEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void ViewList::OnViewSelected( view_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ViewList->SetViewListViewSelectedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ViewListEventDispatcher::ViewSelected : nullptr ) == api_false )
      throw APIFunctionError( "SetViewListViewSelectedEventRoutine" );
   m_handlers->onViewSelected = f;
}

void ViewList::OnCurrentViewUpdated( view_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ViewList->SetViewListCurrentViewUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ViewListEventDispatcher::CurrentViewUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetViewListCurrentViewUpdatedEventRoutine" );
   m_handlers->onCurrentViewUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ViewList.cpp - Released 2025-04-07T08:53:32Z
