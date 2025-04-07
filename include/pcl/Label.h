//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Label.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Label_h
#define __PCL_Label_h

/// \file pcl/Label.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Frame.h>
#include <pcl/TextAlign.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Label
 * \brief Client-side interface to a PixInsight %Label control.
 *
 * ### TODO: Write a detailed description for %Label.
 */
class PCL_CLASS Label : public Frame
{
public:

   /*!
    * Constructs a %Label object with the specified \a text, as a child control
    * of \a parent.
    */
   Label( const String& text = String(), Control& parent = Control::Null() );

   /*!
    * Destroys a %Label object.
    */
   ~Label() override
   {
   }

   /*! #
    */
   String Text() const;

   /*! #
    */
   void SetText( const String& );

   /*! #
    */
   void Clear()
   {
      SetText( String() );
   }

   /*! #
    */
   int Margin() const;

   /*! #
    */
   void SetMargin( int );

   /*! #
    */
   void ClearMargin()
   {
      SetMargin( 0 );
   }

   /*! #
    */
   int TextAlignment() const;

   /*! #
    */
   void SetTextAlignment( int );

   /*! #
    */
   bool IsWordWrappingEnabled() const;

   /*! #
    */
   void EnableWordWrapping( bool = true );

   /*! #
    */
   void DisableWordWrapping( bool disable = true )
   {
      EnableWordWrapping( !disable );
   }

   /*! #
    */
   bool IsRichTextEnabled() const;

   /*! #
    */
   void EnableRichText( bool = true );

   /*! #
    */
   void DisableRichText( bool disable = true )
   {
      EnableRichText( !disable );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_Label_h

// ----------------------------------------------------------------------------
// EOF pcl/Label.h - Released 2025-04-07T08:52:44Z
