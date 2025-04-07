//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ColorDialog.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ColorDialog_h
#define __PCL_ColorDialog_h

/// \file pcl/ColorDialog.h

#include <pcl/Defs.h>

#include <pcl/Color.h>
#include <pcl/Dialog.h>
#include <pcl/NumericControl.h>
#include <pcl/PushButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### TODO: Implement a fully-functional color dialog box in PCL.
 */

// ----------------------------------------------------------------------------

class PCL_INTERNAL SimpleColorDialogPrivate;

/*!
 * \class SimpleColorDialog
 * \brief A simple color selection dialog
 *
 * %SimpleColorDialog is a simplified color selection dialog box. This class is
 * suitable to implement quick definition of 32-bit RGBA color items as part of
 * a user preferences definition interface. It implements optional support for
 * an alpha (transparency) component. Color components are specified in the
 * eight-bit range [0,255].
 *
 * \sa Dialog
 */
class PCL_CLASS SimpleColorDialog : public Dialog
{
public:

   enum
   {
     AlphaEnabled = 0x0080,   //!< Enable definition of the alpha (transparency) component.
     Grayscale    = 0x0020    //!< Define a grayscale AARRGGBB color, where RR=GG=BB.
   };

   /*!
    * Constructs a %SimpleColorDialog object.
    *
    * \param color   Address of a 32-bit unsigned integer variable (a RGBA
    *                color value). If a non-null pointer is specified, when the
    *                user accepts the dialog the selected color value will be
    *                written to the specified variable.
    *
    * \param flags   A combination of flags that define the behavior of the
    *                color selection dialog. Currently the following flags are
    *                supported:\n
    *                \n
    *                SimpleColorDialog::AlphaEnabled: Enable definition of the
    *                alpha (transparency) component\n
    *                \n
    *                SimpleColorDialog::Grayscale: Define a grayscale AARRGGBB
    *                color, where RR=GG=BB.
    */
   SimpleColorDialog( RGBA* color = nullptr, uint32 flags = 0 );

   /*!
    * Destroys a %SimpleColorDialog object.
    */
   ~SimpleColorDialog() override
   {
   }

   /*!
    * Sets the current RGBA color value.
    *
    * This function can be called before executing the dialog to set an initial
    * color value, as an alternative to passing a non-null pointer to the
    * constructor.
    */
   void SetColor( RGBA color );

   /*!
    * Returns the RGBA color value that has been defined by the user.
    *
    * If a non-null pointer was passed to the constructor, this function
    * returns the same color value that will be (or has been) written to the
    * corresponding variable.
    */
   RGBA Color() const
   {
      return m_workingColor;
   }

   /*!
    * Returns true iff this dialog allows definition of an alpha (transparency)
    * color component.
    */
   bool IsAlphaEnabled() const
   {
      return (m_flags & AlphaEnabled) != 0;
   }

   /*!
    * Returns true iff this dialog defines a grayscale color, where the three
    * individual RGB components are equal.
    */
   bool IsGrayscale() const
   {
      return (m_flags & Grayscale) != 0;
   }

protected:

   RGBA*  m_color;
   RGBA   m_workingColor;
   uint32 m_flags;

   VerticalSizer  Global_Sizer;
      HorizontalSizer   Color_Sizer;
         VerticalSizer     Sliders_Sizer;
            NumericControl    V0_NumericControl;
            NumericControl    V1_NumericControl;
            NumericControl    V2_NumericControl;
            NumericControl    V3_NumericControl;
         Control           ColorSample_Control;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void Color_ValueUpdated( NumericEdit& sender, double value );
   void ColorSample_Paint( Control& sender, const Rect& updateRect );
   void Done_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ColorDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/ColorDialog.h - Released 2025-04-07T08:52:44Z
