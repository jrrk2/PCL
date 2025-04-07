//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MultiViewSelectionDialog.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_MultiViewSelectionDialog_h
#define __PCL_MultiViewSelectionDialog_h

/// \file pcl/MultiViewSelectionDialog.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/TreeBox.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MultiViewSelectionDialog
 * \brief A simple dialog box to select a set of existing views.
 *
 * %MultiViewSelectionDialog consists of a TreeBox control, populated with all
 * existing views, plus standard OK and Cancel push buttons. This class is a
 * useful helper whenever a process needs access to several user-selected views
 * in a single operation.
 *
 * \sa ViewSelectionDialog, PreviewSelectionDialog, Dialog
 */
class MultiViewSelectionDialog : public Dialog
{
public:

   /*!
    * Constructs a %MultiViewSelectionDialog object.
    *
    * \param allowPreviews    Whether to allow selection of previews, along
    *                         with main views. The default value is true.
    */
   MultiViewSelectionDialog( bool allowPreviews = true );

   /*!
    * Destroys a %MultiViewSelectionDialog object.
    */
   ~MultiViewSelectionDialog() override
   {
   }

   /*!
    * Returns the set of selected views.
    */
   const Array<View>& Views() const
   {
      return m_selectedViews;
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

   Array<View> m_selectedViews;
   bool        m_allowPreviews = true;

   void Regenerate();

   VerticalSizer  Global_Sizer;
      TreeBox           Views_TreeBox;
      HorizontalSizer   Row2_Sizer;
         PushButton        SelectAll_PushButton;
         PushButton        UnselectAll_PushButton;
         CheckBox          IncludeMainViews_CheckBox;
         CheckBox          IncludePreviews_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void OptionClick( Button& sender, bool checked );
   void ButtonClick( Button& sender, bool checked );
   void ControlShow( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MultiViewSelectionDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/MultiViewSelectionDialog.h - Released 2025-04-07T08:52:44Z
