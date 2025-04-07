//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ButtonCodes.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ButtonCodes_h
#define __PCL_ButtonCodes_h

/// \file pcl/ButtonCodes.h

#include <pcl/Defs.h>

#include <pcl/Flags.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::MouseButton
 * \brief Defines PCL mouse button codes
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>MouseButton::Left</td>    <td>Primary mouse button</td></tr>
 * <tr><td>MouseButton::Right</td>   <td>Secondary mouse button</td></tr>
 * <tr><td>MouseButton::Middle</td>  <td>Middle mouse button</td></tr>
 * <tr><td>MouseButton::X1</td>      <td>First X button</td></tr>
 * <tr><td>MouseButton::X2</td>      <td>Second X button</td></tr>
 * <tr><td>MouseButton::Unknown</td> <td>Unknown/unsupported mouse button</td></tr>
 * </table>
 */
namespace MouseButton
{
   enum mask_type
   {
      Unknown  = 0,     // Unknown/unsupported mouse button
      Left     = 0x01,  // Primary mouse button
      Right    = 0x02,  // Secondary mouse button
      Middle   = 0x04,  // Middle mouse button
      X1       = 0x10,  // First X button
      X2       = 0x20   // Second X button
   };
}

/*!
 * A combination of mouse button codes.
 */
using MouseButtons = Flags<MouseButton::mask_type>;

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::KeyModifier
 * \brief Defines PCL key modifier codes
 *
 * The %KeyModifier namespace defines platform-independent key modifiers on the
 * PixInsight/PCL framework. Key modifiers are sent to event handlers (such as
 * Control::OnKeyPress(), Control::OnMousePress(), etc.) and used to specify
 * keyboard shortcuts; see for example Action::SetAccelerator().
 *
 * To query the current state of supported keyboard modifier keys in real time,
 * see the pcl::KeyboardModifier namespace and the CurrentKeyboardModifiers()
 * global function, defined and declared, respectively, in the pcl/KeyCodes.h
 * header file.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>KeyModifier::Shift</td>    <td>Shift key</td></tr>
 * <tr><td>KeyModifier::Control</td>  <td>Control key (Command key on macOS)</td></tr>
 * <tr><td>KeyModifier::Alt</td>      <td>Alt key</td></tr>
 * <tr><td>KeyModifier::SpaceBar</td> <td>Space bar</td></tr>
 * <tr><td>KeyModifier::Meta</td>     <td>Meta key (Control key on macOS)</td></tr>
 * </table>
 */
namespace KeyModifier
{
   enum mask_type
   {
      Shift    = 0x01,  // Shift key
      Control  = 0x02,  // Control key (= Command on macOS)
      Alt      = 0x04,  // Alt key
      SpaceBar = 0x08,  // Space bar
      Meta     = 0x10   // Meta key (= Control on macOS)
   };
}

/*!
 * A combination of keyboard modifiers.
 */
using KeyModifiers = Flags<KeyModifier::mask_type>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ButtonCodes_h

// ----------------------------------------------------------------------------
// EOF pcl/ButtonCodes.h - Released 2025-04-07T08:52:44Z
