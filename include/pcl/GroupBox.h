//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/GroupBox.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_GroupBox_h
#define __PCL_GroupBox_h

/// \file pcl/GroupBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GroupBox
 * \brief Client-side interface to a PixInsight %GroupBox control
 *
 * ### TODO: Write a detailed description for %GroupBox.
 */
class PCL_CLASS GroupBox : public Control
{
public:

   /*!
    * Constructs a %GroupBox object with the specified \a title, as a child
    * control of \a parent.
    */
   GroupBox( const String& title = String(), Control& parent = Control::Null() );

   /*!
    * Destroys a %GroupBox object.
    */
   ~GroupBox() override
   {
   }

   /*! #
    */
   String Title() const;

   /*! #
    */
   void SetTitle( const String& );

   /*! #
    */
   bool HasTitleCheckBox() const;

   /*! #
    */
   void EnableTitleCheckBox( bool = true );

   /*! #
    */
   void DisableTitleCheckBox( bool disable = true )
   {
      EnableTitleCheckBox( !disable );
   }

   /*! #
    */
   bool IsChecked() const;

   /*! #
    */
   void SetChecked( bool = true );

   /*! #
    */
   void Check()
   {
      SetChecked( true );
   }

   /*! #
    */
   void Uncheck()
   {
      SetChecked( false );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnCheck( GroupBox& sender, bool checked );

   /*! #
    */
   using check_event_handler = void (Control::*)( GroupBox& sender, bool checked );

   /*! #
    */
   void OnCheck( check_event_handler, Control& );

private:

   struct EventHandlers
   {
      check_event_handler onCheck = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class GroupBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_GroupBox_h

// ----------------------------------------------------------------------------
// EOF pcl/GroupBox.h - Released 2025-04-07T08:52:44Z
