//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/FileDataCachePreferencesDialog.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_FileDataCachePreferencesDialog_h
#define __PCL_FileDataCachePreferencesDialog_h

/// \file pcl/FileDataCache.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileDataCache;

/*!
 * \class FileDataCachePreferencesDialog
 * \brief A dialog to edit file data cache preferences settings
 *
 * This dialog allows to define persistence and maximum duration for file data
 * cache items associated with a given FileDataCache instance. The dialog also
 * allows to save and clear all cache items in memory and persistent storage.
 */
class PCL_CLASS FileDataCachePreferencesDialog : public Dialog
{
public:

   /*!
    * Constructs a dialog to edit preferences settings for a file data cache.
    *
    * \param cache         Reference to a file data cache object that will be
    *                      managed by this cache preferences dialog.
    *
    * \param persistent    Reference to a variable storing a cache persistence
    *                      state. Persistent caches should be written to
    *                      permanent storage. Non-persistent caches are only
    *                      kept in volatile memory. This variable will be
    *                      updated with the user selection if the dialog is
    *                      accepted.
    */
   FileDataCachePreferencesDialog( FileDataCache& cache, bool& persistent );

private:

   FileDataCache& m_cache;
   bool&          m_persistent;

   VerticalSizer  Global_Sizer;
      HorizontalSizer   PersistentCache_Sizer;
         CheckBox          PersistentCache_CheckBox;
      HorizontalSizer   CacheDuration_Sizer;
         Label             CacheDuration_Label;
         SpinBox           CacheDuration_SpinBox;
      HorizontalSizer   ClearCache_Sizer;
         PushButton        ClearCache_PushButton;
      HorizontalSizer   PurgeCache_Sizer;
         PushButton        PurgeCache_PushButton;
      HorizontalSizer   SaveCache_Sizer;
         PushButton        SaveCache_PushButton;
      HorizontalSizer   Buttons_Sizer;
         Label             Info_Label;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void e_Click( Button& sender, bool checked );
   void e_Return( Dialog& sender, int retVal );

   void UpdateInfo();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FileDataCachePreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/FileDataCachePreferencesDialog.h - Released 2025-04-07T08:52:44Z
