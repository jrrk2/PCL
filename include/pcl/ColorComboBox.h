//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ColorComboBox.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ColorComboBox_h
#define __PCL_ColorComboBox_h

/// \file pcl/ColorComboBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Color.h>
#include <pcl/ComboBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ColorComboBox
 * \brief A %ComboBox descendant class to select RGB colors.
 *
 * %ColorComboBox is a utility class that provides a simple list with the full
 * set of CSS3 standard colors. This includes 143 %ComboBox items that are
 * automatically shared by all existing %ColorComboBox instances. Thanks to
 * this implicit sharing mechanism, a module can define a large number of
 * %ColorComboBox controls without consuming too much resources on any
 * supported platform.
 *
 * The list of items corresponding to CSS3 colors is sorted by \e hue value (in
 * the HSV or HSI color ordering systems). This makes it much easier the task
 * of selecting colors since similar hues are grouped visually.
 *
 * In addition to CSS3 standard colors, a single custom color item can be
 * defined for a %ColorComboBox object. The custom color item is automatically
 * managed by %ColorComboBox and appended to the end of standard items.
 *
 * \sa pcl/Color.h, ComboBox
 */
class PCL_CLASS ColorComboBox : public ComboBox
{
public:

   /*!
    * Constructs a %ColorComboBox as a child control of \a parent.
    *
    * A standard set of %ComboBox items is created for the full set of CSS3
    * colors. This includes 143 color items.
    */
   ColorComboBox( Control& parent = Control::Null() );

   /*!
    * Destroys a %ColorComboBox control.
    */
   ~ColorComboBox() override
   {
   }

   /*!
    * Returns the RGB color value that corresponds to the currently selected
    * %ComboBox item, or zero if no item is currently selected.
    *
    * \sa SetCurrentColor(), CustomColor(), HasCustomColor()
    */
   RGBA CurrentColor() const;

   /*!
    * Selects a %ComboBox item corresponding to the specified \a color.
    *
    * If the specified \a color doesn't correspond to a standard CSS color, it
    * replaces the existing <em>custom color item</em> in this %ColorComboBox,
    * or a new one is created if there is no custom color item yet.
    *
    * \note The alpha (transparency) component of the specified \a color is
    * always ignored. %ColorComboBox only includes opaque colors (alpha=0xff).
    *
    * \sa CurrentColor(), CustomColor(), HasCustomColor()
    */
   void SetCurrentColor( RGBA color );

   /*!
    * Returns the current <em>custom color</em> in this %ColorComboBox, or zero
    * if no custom color has been defined.
    *
    * To set a new custom color, call SetCurrentColor() with the desired custom
    * color value.
    *
    * \sa HasCustomColor(), CurrentColor(), SetCurrentColor()
    */
   RGBA CustomColor() const
   {
      return m_customColor;
   }

   /*!
    * Returns true iff a <em>custom color item</em> has been defined in this
    * %ColorComboBox control.
    *
    * This is a convenience member function, equivalent to:
    * CustomColor() != 0.
    *
    * \sa CustomColor(), CurrentColor(), SetCurrentColor()
    */
   bool HasCustomColor() const
   {
      return CustomColor() != 0;
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnColorSelected( ColorComboBox& sender, RGBA color );
   // void OnColorHighlighted( ColorComboBox& sender, RGBA color );
   // void OnCustomColorDefined( ColorComboBox& sender, RGBA color );

   /*!
    * \defgroup colorcombobox_event_handlers ColorComboBox Event Handlers
    */

   /*!
    * Defines the prototype of a <em>color combo box event handler</em>.
    *
    * A color combo box event is generated when an item is selected or
    * highlighted in a color combo box, or when a new <em>custom color</em> has
    * been defined.
    *
    * \param sender  The %ColorComboBox control that sends a color combo box
    *                event.
    *
    * \param color   The RGBA value of the color that has been selected or
    *                highlighted in the \a sender color combo box.
    *
    * \ingroup colorcombobox_event_handlers
    */
   using color_event_handler = void (Control::*)( ColorComboBox& sender, RGBA color );

   /*!
    * Sets the <em>color selected</em> event handler for this color combo box.
    *
    * \param handler    The color combo box event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever a color is selected in this
    *                   color combo box control.
    *
    * \param receiver   The control that will receive <em>color selected</em>
    *                   events from this color combo box.
    *
    * \ingroup colorcombobox_event_handlers
    */
   void OnColorSelected( color_event_handler handler, Control& receiver );

   /*!
    * Sets the <em>color highlighted</em> event handler for this color combo
    * box control.
    *
    * \param handler    The color combo box event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever a color is highlighted in this
    *                   color combo box control.
    *
    * \param receiver   The control that will receive <em>color
    *                   highlighted</em> events from this color combo box.
    *
    * \ingroup colorcombobox_event_handlers
    */
   void OnColorHighlighted( color_event_handler handler, Control& receiver );

   /*!
    * Sets the <em>custom color defined</em> event handler for this color combo
    * box control.
    *
    * \param handler    The color combo box event handler. Must be a member
    *                   function of the receiver object's class. This handler
    *                   will be called whenever a new custom color is defined
    *                   in this color combo box control.
    *
    * \param receiver   The control that will receive <em>custom color
    *                   defined</em> events from this color combo box.
    *
    * \ingroup colorcombobox_event_handlers
    */
   void OnCustomColorDefined( color_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      color_event_handler onColorSelected              = nullptr;
      Control*            onColorSelectedReceiver      = nullptr;

      color_event_handler onColorHighlighted           = nullptr;
      Control*            onColorHighlightedReceiver   = nullptr;

      color_event_handler onCustomColorDefined         = nullptr;
      Control*            onCustomColorDefinedReceiver = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;
   RGBA                       m_customColor;

   void ItemSelected( ComboBox&, int );
   void ItemHighlighted( ComboBox&, int );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ColorComboBox_h

// ----------------------------------------------------------------------------
// EOF pcl/ColorComboBox.h - Released 2025-04-07T08:52:44Z
