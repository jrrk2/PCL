//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/TreeBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/AutoLock.h>
#include <pcl/TreeBox.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

TreeBox::TreeBox( Control& parent )
   : ScrollBox( nullptr )
{
   TransferHandle( API_TreeBox_CreateTreeBox( ModuleHandle(), this, parent.handle, 0/*flags*/ ) );
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBox" );

   m_viewport.TransferHandle( API_TreeBox_CreateTreeBoxViewport( handle, &m_viewport ) );
   if ( m_viewport.IsNull() )
      throw APIFunctionError( "CreateTreeBoxViewport" );
}

TreeBox::TreeBox( void* h )
   : ScrollBox( nullptr )
{
   TransferHandle( h );
   if ( !IsNull() )
   {
      m_viewport.TransferHandle( API_TreeBox_CreateTreeBoxViewport( handle, &m_viewport ) );
      if ( m_viewport.IsNull() )
         throw APIFunctionError( "CreateTreeBoxViewport" );
   }
}

TreeBox::~TreeBox()
{
   child_node_list children( m_children );
   m_children.Clear();
   for ( auto node : children )
   {
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

TreeBox& TreeBox::NullTree()
{
   static TreeBox* nullTree = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullTree == nullptr )
      nullTree = new TreeBox( nullptr );
   return *nullTree;
}

// ----------------------------------------------------------------------------

int TreeBox::NumberOfChildren() const
{
   return API_TreeBox_GetTreeBoxChildCount( handle );
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::Child( int idx ) const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxChild( handle, idx ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::Child( int idx )
{
   return reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxChild( handle, idx ) );
}

// ----------------------------------------------------------------------------

int TreeBox::ChildIndex( const TreeBox::Node* node ) const
{
   if ( node == nullptr )
      return -1;
   return API_TreeBox_GetTreeBoxChildIndex( handle, node->handle );
}

// ----------------------------------------------------------------------------

void TreeBox::Insert( int idx, TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      PCL_CHECK( node->ParentTree().IsNull() )
      PCL_CHECK( node->Parent() == nullptr )
      if ( !m_children.Contains( node ) )
      {
         API_TreeBox_InsertTreeBoxNode( handle, idx, node->handle );
         m_children << node;
      }
   }
}

// ----------------------------------------------------------------------------

void TreeBox::Remove( int idx )
{
   Node* node = Child( idx );
   if ( node != nullptr )
   {
      API_TreeBox_RemoveTreeBoxNode( handle, idx );
      m_children.Remove( node );
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

void TreeBox::Clear()
{
   API_TreeBox_ClearTreeBox( handle );
   child_node_list children( m_children );
   m_children.Clear();
   for ( auto node : children )
   {
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::CurrentNode() const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxCurrentNode( handle ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::CurrentNode()
{
   return reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxCurrentNode( handle ) );
}

// ----------------------------------------------------------------------------

void TreeBox::SetCurrentNode( TreeBox::Node* node )
{
   if ( node != nullptr )
      API_TreeBox_SetTreeBoxCurrentNode( handle, node->handle );
}

// ----------------------------------------------------------------------------

bool TreeBox::AreMultipleSelectionsEnabled() const
{
   return API_TreeBox_GetTreeBoxMultipleNodeSelectionEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableMultipleSelections( bool enable )
{
   API_TreeBox_SetTreeBoxMultipleNodeSelectionEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

IndirectArray<TreeBox::Node> TreeBox::SelectedNodes() const
{
   IndirectArray<TreeBox::Node> nodes;
   size_type n = 0;
   API_TreeBox_GetTreeBoxSelectedNodes( handle, 0, &n );
   if ( n > 0 )
   {
      nodes.Add( nullptr, n );
      if ( API_TreeBox_GetTreeBoxSelectedNodes( handle,
                     reinterpret_cast< ::api_handle*>( nodes.Begin() ), &n ) == api_false )
         throw APIFunctionError( "GetTreeBoxSelectedNodes" );
      nodes.Pack();
   }
   return nodes;
}

// ----------------------------------------------------------------------------

bool TreeBox::HasSelectedTopLevelNodes() const
{
   for ( int i = 0, n = NumberOfChildren(); i < n; ++i )
      if ( Child( i )->IsSelected() )
         return true;
   return false;
}

// ----------------------------------------------------------------------------

void TreeBox::SelectAllNodes()
{
   API_TreeBox_SelectAllTreeBoxNodes( handle );
}

// ----------------------------------------------------------------------------

/*
 * ### TODO: Implement inline node editors.
 */

/*
void TreeBox::BeginNodeEdition( TreeBox::Node* node, int col )
{
   if ( node != nullptr )
      API_TreeBox_BeginTreeBoxNodeEdition( handle, node->handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::EndNodeEdition( TreeBox::Node* node, int col )
{
   if ( node != nullptr )
      API_TreeBox_EndTreeBoxNodeEdition( handle, node->handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::EditNode( TreeBox::Node* node, int col )
{
   if ( node != nullptr )
      API_TreeBox_EditTreeBoxNode( handle, node->handle, col );
}
*/

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::NodeByPosition( int x, int y ) const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxNodeByPos( handle, x, y ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::NodeByPosition( int x, int y )
{
   return reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxNodeByPos( handle, x, y ) );
}

// ----------------------------------------------------------------------------

void TreeBox::SetNodeIntoView( TreeBox::Node* node )
{
   if ( node != nullptr )
      API_TreeBox_SetTreeBoxNodeIntoView( handle, node->handle );
}

// ----------------------------------------------------------------------------

pcl::Rect TreeBox::NodeRect( const TreeBox::Node* node ) const
{
   if ( node == nullptr )
      return pcl::Rect( 0 );
   pcl::Rect r;
   API_TreeBox_GetTreeBoxNodeRect( handle, node->handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

int TreeBox::NumberOfColumns() const
{
   return API_TreeBox_GetTreeBoxColumnCount( handle );
}

// ----------------------------------------------------------------------------

void TreeBox::SetNumberOfColumns( int nCols )
{
   API_TreeBox_SetTreeBoxColumnCount( handle, nCols );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsColumnVisible( int col ) const
{
   return API_TreeBox_GetTreeBoxColumnVisible( handle, col ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::ShowColumn( int col, bool show )
{
   API_TreeBox_SetTreeBoxColumnVisible( handle, col, show );
}

// ----------------------------------------------------------------------------

int TreeBox::ColumnWidth( int col ) const
{
   return API_TreeBox_GetTreeBoxColumnWidth( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::SetColumnWidth( int col, int width )
{
   API_TreeBox_SetTreeBoxColumnWidth( handle, col, width );
}

// ----------------------------------------------------------------------------

void TreeBox::AdjustColumnWidthToContents( int col )
{
   API_TreeBox_AdjustTreeBoxColumnWidthToContents( handle, col );
}

// ----------------------------------------------------------------------------

String TreeBox::HeaderText( int col ) const
{
   size_type len = 0;
   API_TreeBox_GetTreeBoxHeaderText( handle, col, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_TreeBox_GetTreeBoxHeaderText( handle, col, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTreeBoxHeaderText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void TreeBox::SetHeaderText( int col, const String& text )
{
   API_TreeBox_SetTreeBoxHeaderText( handle, col, text.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap TreeBox::HeaderIcon( int col ) const
{
   return Bitmap( API_TreeBox_GetTreeBoxHeaderIcon( handle, col ) );
}

// ----------------------------------------------------------------------------

void TreeBox::SetHeaderIcon( int col, const Bitmap& icon )
{
   API_TreeBox_SetTreeBoxHeaderIcon( handle, col, icon.handle );
}

// ----------------------------------------------------------------------------

int TreeBox::HeaderAlignment( int col ) const
{
   return API_TreeBox_GetTreeBoxHeaderAlignment( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::SetHeaderAlignment( int col, int align )
{
   API_TreeBox_SetTreeBoxHeaderAlignment( handle, col, align );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsHeaderVisible() const
{
   return API_TreeBox_GetTreeBoxHeaderVisible( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::ShowHeader( bool show )
{
   API_TreeBox_SetTreeBoxHeaderVisible( handle, show );
}

// ----------------------------------------------------------------------------

int TreeBox::IndentSize() const
{
   return API_TreeBox_GetTreeBoxIndentSize( handle );
}

// ----------------------------------------------------------------------------

void TreeBox::SetIndentSize( int szPx )
{
   API_TreeBox_SetTreeBoxIndentSize( handle, szPx );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsNodeExpansionEnabled() const
{
   return API_TreeBox_GetTreeBoxNodeExpansionEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableNodeExpansion( bool enable )
{
   API_TreeBox_SetTreeBoxNodeExpansionEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsRootDecorationEnabled() const
{
   return API_TreeBox_GetTreeBoxRootDecorationEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableRootDecoration( bool enable )
{
   API_TreeBox_SetTreeBoxRootDecorationEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsAlternateRowColorEnabled() const
{
   return API_TreeBox_GetTreeBoxAlternateRowColorEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableAlternateRowColor( bool enable )
{
   API_TreeBox_SetTreeBoxAlternateRowColorEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsUniformRowHeightEnabled() const
{
   return API_TreeBox_GetTreeBoxUniformRowHeightEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableUniformRowHeight( bool enable )
{
   API_TreeBox_SetTreeBoxUniformRowHeightEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

void TreeBox::GetIconSize( int& width, int& height ) const
{
   API_TreeBox_GetTreeBoxIconSize( handle, &width, &height );
}

// ----------------------------------------------------------------------------

void TreeBox::SetIconSize( int width, int height )
{
   API_TreeBox_SetTreeBoxIconSize( handle, width, height );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsHeaderSortingEnabled() const
{
   return API_TreeBox_GetTreeBoxHeaderSortingEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableHeaderSorting( bool enable )
{
   API_TreeBox_SetTreeBoxHeaderSortingEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

void TreeBox::Sort( int col, bool ascending )
{
   API_TreeBox_SortTreeBox( handle, col, ascending );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsNodeDraggingEnabled() const
{
   return API_TreeBox_GetTreeBoxNodeDraggingEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableNodeDragging( bool enable )
{
   API_TreeBox_SetTreeBoxNodeDraggingEnabled( handle, enable );
}

// ----------------------------------------------------------------------------
// TreeBox::Node Implementation
// ----------------------------------------------------------------------------

TreeBox::Node::Node()
   : UIObject( API_TreeBox_CreateTreeBoxNode( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBoxNode" );
}

TreeBox::Node::Node( TreeBox::Node& parent, int index )
   : UIObject( API_TreeBox_CreateTreeBoxNode( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBoxNode" );
   if ( !parent.IsNull() )
      parent.Insert( index, this );
}

TreeBox::Node::Node( TreeBox& parentTree, int index )
   : UIObject( API_TreeBox_CreateTreeBoxNode( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBoxNode" );
   if ( !parentTree.IsNull() )
      parentTree.Insert( index, this );
}

// ----------------------------------------------------------------------------

TreeBox::Node::~Node()
{
   if ( !m_removed )
   {
      m_removed = true;
      Node* parentNode = reinterpret_cast<Node*>( API_TreeBox_GetTreeBoxNodeParent( handle ) );
      if ( parentNode != nullptr )
      {
         parentNode->m_children.Remove( this );
         int idx = parentNode->ChildIndex( this );
         if ( idx >= 0 )
            API_TreeBox_RemoveTreeBoxNodeChild( parentNode->handle, idx );
      }
      else
      {
         TreeBox* parentTree = reinterpret_cast<TreeBox*>( API_TreeBox_GetTreeBoxNodeParentBox( handle ) );
         if ( parentTree != nullptr )
         {
            parentTree->m_children.Remove( this );
            int idx = parentTree->ChildIndex( this );
            if ( idx >= 0 )
               API_TreeBox_RemoveTreeBoxNode( parentTree->handle, idx );
         }
      }
   }

   child_node_list children( m_children );
   m_children.Clear();
   for ( auto node : children )
   {
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

const TreeBox& TreeBox::Node::ParentTree() const
{
   TreeBox* tree = reinterpret_cast<TreeBox*>( API_TreeBox_GetTreeBoxNodeParentBox( handle ) );
   return (tree != nullptr) ? *tree : NullTree();
}

// ----------------------------------------------------------------------------

TreeBox& TreeBox::Node::ParentTree()
{
   TreeBox* tree = reinterpret_cast<TreeBox*>( API_TreeBox_GetTreeBoxNodeParentBox( handle ) );
   return (tree != nullptr) ? *tree : NullTree();
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::Node::Parent() const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxNodeParent( handle ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::Node::Parent()
{
   return reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxNodeParent( handle ) );
}

// ----------------------------------------------------------------------------

int TreeBox::Node::NumberOfChildren() const
{
   return API_TreeBox_GetTreeBoxNodeChildCount( handle );
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::Node::Child( int idx ) const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxNodeChild( handle, idx ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::Node::Child( int idx )
{
   return reinterpret_cast<TreeBox::Node*>( API_TreeBox_GetTreeBoxNodeChild( handle, idx ) );
}

// ----------------------------------------------------------------------------

int TreeBox::Node::ChildIndex( const TreeBox::Node* node ) const
{
   for ( int i = 0, n = NumberOfChildren(); i < n; ++i )
      if ( Child( i ) == node )
         return i;
   return -1;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Insert( int idx, TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      PCL_CHECK( node->ParentTree().IsNull() )
      PCL_CHECK( node->Parent() == nullptr )
      if ( !m_children.Contains( node ) )
      {
         API_TreeBox_InsertTreeBoxNodeChild( handle, idx, node->handle );
         m_children << node;
      }
   }
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Remove( int idx )
{
   TreeBox::Node* node = Child( idx );
   if ( node != nullptr )
   {
      API_TreeBox_RemoveTreeBoxNodeChild( handle, idx );
      m_children.Remove( node );
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsEnabled() const
{
   return API_TreeBox_GetTreeBoxNodeEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Enable( bool enable )
{
   API_TreeBox_SetTreeBoxNodeEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsExpanded() const
{
   return API_TreeBox_GetTreeBoxNodeExpanded( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Expand( bool expand )
{
   API_TreeBox_SetTreeBoxNodeExpanded( handle, expand );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsSelectable() const
{
   return API_TreeBox_GetTreeBoxNodeSelectable( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetSelectable( bool enable )
{
   API_TreeBox_SetTreeBoxNodeSelectable( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsSelected() const
{
   return API_TreeBox_GetTreeBoxNodeSelected( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Select( bool select )
{
   API_TreeBox_SetTreeBoxNodeSelected( handle, select );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsCheckable() const
{
   return API_TreeBox_GetTreeBoxNodeCheckable( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetCheckable( bool enable )
{
   API_TreeBox_SetTreeBoxNodeCheckable( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsChecked() const
{
   return API_TreeBox_GetTreeBoxNodeChecked( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Check( bool check )
{
   API_TreeBox_SetTreeBoxNodeChecked( handle, check );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsFirstColumnSpanned() const
{
   return API_TreeBox_GetTreeBoxNodeFirstColumnSpanned( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetFirstColumnSpanned( bool spanned )
{
   API_TreeBox_SetTreeBoxNodeFirstColumnSpanned( handle, spanned );
}

// ----------------------------------------------------------------------------

/*
 * ### TODO: Implement inline node editors.
 */

/*
bool TreeBox::Node::IsEditable() const
{
   return API_TreeBox_GetTreeBoxNodeEditable( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetEditable( bool enable )
{
   API_TreeBox_SetTreeBoxNodeEditable( handle, enable );
}
*/
// ----------------------------------------------------------------------------

String TreeBox::Node::Text( int col ) const
{
   size_type len = 0;
   API_TreeBox_GetTreeBoxNodeColText( handle, col, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_TreeBox_GetTreeBoxNodeColText( handle, col, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTreeBoxNodeColText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetText( int col, const String& text )
{
   API_TreeBox_SetTreeBoxNodeColText( handle, col, text.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap TreeBox::Node::Icon( int col ) const
{
   return TreeBox::BitmapFromHandle( API_TreeBox_GetTreeBoxNodeColIcon( handle, col ) );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetIcon( int col, const Bitmap& icon )
{
   API_TreeBox_SetTreeBoxNodeColIcon( handle, col, TreeBox::HandleFromBitmap( icon ) );
}

// ----------------------------------------------------------------------------

int TreeBox::Node::Alignment( int col ) const
{
   return API_TreeBox_GetTreeBoxNodeColAlignment( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetAlignment( int col, int align )
{
   API_TreeBox_SetTreeBoxNodeColAlignment( handle, col, align );
}

// ----------------------------------------------------------------------------

String TreeBox::Node::ToolTip( int col ) const
{
   size_type len = 0;
   API_TreeBox_GetTreeBoxNodeColToolTip( handle, col, 0, &len );

   String tip;
   if ( len > 0 )
   {
      tip.SetLength( len );
      if ( API_TreeBox_GetTreeBoxNodeColToolTip( handle, col, tip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTreeBoxNodeColToolTip" );
      tip.ResizeToNullTerminated();
   }
   return tip;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetToolTip( int col, const String& tip )
{
   API_TreeBox_SetTreeBoxNodeColToolTip( handle, col, tip.c_str() );
}

// ----------------------------------------------------------------------------

pcl::Font TreeBox::Node::Font( int col ) const
{
   return TreeBox::FontFromHandle( API_TreeBox_GetTreeBoxNodeColFont( handle, col ) );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetFont( int col, const pcl::Font& font )
{
   API_TreeBox_SetTreeBoxNodeColFont( handle, col, TreeBox::HandleFromFont( font ) );
}

// ----------------------------------------------------------------------------

RGBA TreeBox::Node::BackgroundColor( int col ) const
{
   return API_TreeBox_GetTreeBoxNodeColBackgroundColor( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetBackgroundColor( int col, RGBA color )
{
   API_TreeBox_SetTreeBoxNodeColBackgroundColor( handle, col, color );
}

// ----------------------------------------------------------------------------

RGBA TreeBox::Node::TextColor( int col ) const
{
   return API_TreeBox_GetTreeBoxNodeColTextColor( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetTextColor( int col, RGBA color )
{
   API_TreeBox_SetTreeBoxNodeColTextColor( handle, col, color );
}

// ----------------------------------------------------------------------------

void* TreeBox::Node::CloneHandle() const
{
   throw Error( "Cannot clone a TreeBox::Node handle" );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<TreeBox*>( hSender ))
#define node( n ) (reinterpret_cast<TreeBox::Node*>( n ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class TreeBoxEventDispatcher
{
public:

   static void CurrentNodeUpdated( control_handle hSender, control_handle hReceiver, api_handle hNNew, api_handle hNOld )
   {
      if ( handlers->onCurrentNodeUpdated != nullptr )
         (receiver->*handlers->onCurrentNodeUpdated)( *sender, *node( hNNew ), *node( hNOld ) );
   }

   static void NodeActivated( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeActivated != nullptr )
         (receiver->*handlers->onNodeActivated)( *sender, *node( hN ), col );
   }

   static void NodeUpdated( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeUpdated != nullptr )
         (receiver->*handlers->onNodeUpdated)( *sender, *node( hN ), col );
   }

   static void NodeEntered( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeEntered != nullptr )
         (receiver->*handlers->onNodeEntered)( *sender, *node( hN ), col );
   }

   static void NodeClicked( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeClicked != nullptr )
         (receiver->*handlers->onNodeClicked)( *sender, *node( hN ), col );
   }

   static void NodeDoubleClicked( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeDoubleClicked != nullptr )
         (receiver->*handlers->onNodeDoubleClicked)( *sender, *node( hN ), col );
   }

   static void NodeExpanded( control_handle hSender, control_handle hReceiver, api_handle hN )
   {
      if ( handlers->onNodeExpanded != nullptr )
         (receiver->*handlers->onNodeExpanded)( *sender, *node( hN ) );
   }

   static void NodeCollapsed( control_handle hSender, control_handle hReceiver, api_handle hN )
   {
      if ( handlers->onNodeCollapsed != nullptr )
         (receiver->*handlers->onNodeCollapsed)( *sender, *node( hN ) );
   }

   static void NodeSelectionUpdated( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onNodeSelectionUpdated != nullptr )
         (receiver->*handlers->onNodeSelectionUpdated)( *sender );
   }
}; // TreeBoxEventDispatcher

#undef sender
#undef node
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void TreeBox::OnCurrentNodeUpdated( node_navigation_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxCurrentNodeUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::CurrentNodeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxCurrentNodeUpdatedEventRoutine" );
   m_handlers->onCurrentNodeUpdated = f;
}

void TreeBox::OnNodeActivated( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeActivatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeActivated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeActivatedEventRoutine" );
   m_handlers->onNodeActivated = f;
}

void TreeBox::OnNodeUpdated( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeUpdatedEventRoutine" );
   m_handlers->onNodeUpdated = f;
}

void TreeBox::OnNodeEntered( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeEnteredEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeEntered : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeEnteredEventRoutine" );
   m_handlers->onNodeEntered = f;
}

void TreeBox::OnNodeClicked( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeClickedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeClicked : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeClickedEventRoutine" );
   m_handlers->onNodeClicked = f;
}

void TreeBox::OnNodeDoubleClicked( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeDoubleClickedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeDoubleClicked : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeDoubleClickedEventRoutine" );
   m_handlers->onNodeDoubleClicked = f;
}

void TreeBox::OnNodeExpanded( node_expand_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeExpandedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeExpanded : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeExpandedEventRoutine" );
   m_handlers->onNodeExpanded = f;
}

void TreeBox::OnNodeCollapsed( node_expand_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeCollapsedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeCollapsed : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeCollapsedEventRoutine" );
   m_handlers->onNodeCollapsed = f;
}

void TreeBox::OnNodeSelectionUpdated( tree_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TreeBox_SetTreeBoxNodeSelectionUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeSelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeSelectionUpdatedEventRoutine" );
   m_handlers->onNodeSelectionUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/TreeBox.cpp - Released 2025-04-07T08:53:32Z
