//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Frame.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Frame_h
#define __PCL_Frame_h

/// \file pcl/Frame.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Control.h>

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FrameStyle
 * \brief Frame styles
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FrameStyle::Flat</td>   <td>No frame is drawn</td></tr>
 * <tr><td>FrameStyle::Box</td>    <td>Simple rectangular frame</td></tr>
 * <tr><td>FrameStyle::Raised</td> <td>Raised 3-D panel</td></tr>
 * <tr><td>FrameStyle::Sunken</td> <td>Sunken (lowered) 3-D panel</td></tr>
 * <tr><td>FrameStyle::Styled</td> <td>The appearance depends on the current platform and GUI style</td></tr>
 * </table>
 */
namespace FrameStyle
{
   enum value_type
   {
      Flat,    // No frame is drawn
      Box,     // Simple rectangular frame
      Raised,  // Raised 3-D panel
      Sunken,  // Sunken (lowered) 3-D panel
      Styled   // The appearance depends on the current platform and GUI style
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class Frame
 * \brief Client-side interface to a PixInsight %Frame control
 *
 * ### TODO: Write a detailed description for %Frame.
 */
class PCL_CLASS Frame : public Control
{
public:

   /*!
    * Represents a frame style.
    */
   using style = FrameStyle::value_type;

   /*!
    * Constructs a %Frame as a child control of \a parent.
    */
   Frame( Control& parent = Control::Null() );

   /*!
    * Destroys a %Frame object.
    */
   ~Frame() override
   {
   }

   /*!
    * Returns the current frame style.
    */
   style Style() const;

   /*!
    * Sets the frame style.
    */
   void SetStyle( style );

   /*!
    * Returns the current frame's line width in pixels.
    */
   int LineWidth() const;

   /*!
    * Sets the frame's line width in pixels.
    */
   void SetLineWidth( int );

   /*!
    * Returns the border width of this frame.
    *
    * The border width is the distance in pixels from the outer frame rectangle
    * to the inner client rectangle.
    */
   int BorderWidth() const;

   /*! #
    */
   int ScaledLineWidth() const
   {
      return PhysicalPixelsToLogical( LineWidth() );
   }

   /*! #
    */
   void SetScaledLineWidth( int width )
   {
      SetLineWidth( LogicalPixelsToPhysical( width ) );
   }

   /*! #
    */
   int ScaledBorderWidth() const
   {
      return PhysicalPixelsToLogical( BorderWidth() );
   }

protected:

   /*!
    * \internal
    */
   Frame( void* h ) : Control( h )
   {
   }
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Frame_h

// ----------------------------------------------------------------------------
// EOF pcl/Frame.h - Released 2025-04-07T08:52:44Z
