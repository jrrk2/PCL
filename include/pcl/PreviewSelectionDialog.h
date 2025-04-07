//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PreviewSelectionDialog.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PreviewSelectionDialog_h
#define __PreviewSelectionDialog_h

/// \file pcl/PreviewSelectionDialog.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/ViewList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PreviewSelectionDialog
 * \brief A simple dialog box to select an existing preview.
 *
 * %PreviewSelectionDialog consists of a ViewList control populated with all
 * existing previews, and standard OK and Cancel push buttons. This class is a
 * useful helper whenever a process needs access to a user-selected preview.
 *
 * \sa ViewSelectionDialog, MultiViewSelectionDialog, Dialog
 */
class PreviewSelectionDialog : public Dialog
{
public:

   /*!
    * Constructs a %PreviewSelectionDialog object. If the optional \a id
    * argument is specified and a preview exists with that identifier, the
    * dialog will select the corresponding preview upon execution.
    */
   PreviewSelectionDialog( const IsoString& id = IsoString() );

   PreviewSelectionDialog( const IsoString::ustring_base& id )
      : PreviewSelectionDialog( IsoString( id ) )
   {
   }

   /*!
    * Destroys a %PreviewSelectionDialog object.
    */
   ~PreviewSelectionDialog() override
   {
   }

   /*!
    * Returns the identifier of the selected preview.
    */
   IsoString Id() const
   {
      return m_id;
   }

private:

   IsoString m_id;

   VerticalSizer  Global_Sizer;
      ViewList          Images_ViewList;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void ViewSelected( ViewList& sender, View& view );
   void ButtonClick( Button& sender, bool checked );
   void ControlShow( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PreviewSelectionDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/PreviewSelectionDialog.h - Released 2025-04-07T08:52:44Z
