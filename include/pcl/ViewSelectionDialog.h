//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ViewSelectionDialog.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ViewSelectionDialog_h
#define __PCL_ViewSelectionDialog_h

/// \file pcl/ViewSelectionDialog.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/ViewList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ViewSelectionDialog
 * \brief A simple dialog box to select an existing view.
 *
 * %ViewSelectionDialog consists of a ViewList control populated with all
 * existing views, and standard OK and Cancel push buttons. This class is a
 * useful helper whenever a process needs access to a user-selected view.
 *
 * \sa MultiViewSelectionDialog, PreviewSelectionDialog, Dialog
 */
class PCL_CLASS ViewSelectionDialog : public Dialog
{
public:

   /*!
    * Constructs a %ViewSelectionDialog object.
    *
    * \param id   If specified and a view exists with this identifier, the
    *             dialog will select the corresponding view upon execution.
    *
    * \param allowPreviews    Whether to allow selection of previews, along
    *             with main views. For preview-only selections, see the
    *             PreviewSelectionDialog class. The default value is true.
    */
   ViewSelectionDialog( const IsoString& id = IsoString(), bool allowPreviews = true );

   ViewSelectionDialog( const IsoString::ustring_base& id, bool allowPreviews = true )
      : ViewSelectionDialog( IsoString( id ), allowPreviews )
   {
   }

   /*!
    * Destroys a %ViewSelectionDialog object.
    */
   ~ViewSelectionDialog() override
   {
   }

   /*!
    * Returns the identifier of the selected view.
    */
   IsoString Id() const
   {
      return m_id;
   }

   /*!
    * Returns true if this dialog allows selection of previews along with main
    * views; false if it can only select main views. This option can be
    * controlled with a constructor parameter.
    */
   bool PreviewsAllowed() const
   {
      return m_allowPreviews;
   }

private:

   IsoString m_id;
   bool      m_allowPreviews = true;

   VerticalSizer  Global_Sizer;
      ViewList          Images_ViewList;
      CheckBox          IncludeMainViews_CheckBox;
      CheckBox          IncludePreviews_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void ViewSelected( ViewList& sender, View& view );
   void OptionClick( Button& sender, bool checked );
   void ButtonClick( Button& sender, bool checked );
   void ControlShow( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ViewSelectionDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/ViewSelectionDialog.h - Released 2025-04-07T08:52:44Z
