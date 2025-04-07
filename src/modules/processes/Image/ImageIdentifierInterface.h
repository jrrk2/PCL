//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ImageIdentifierInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ImageIdentifierInterface_h
#define __ImageIdentifierInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>

#include "ImageIdentifierInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ImageIdentifierInterface : public ProcessInterface
{
public:

   ImageIdentifierInterface();
   virtual ~ImageIdentifierInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool RequiresInstanceValidation() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   ImageIdentifierInstance instance;

   struct GUIData
   {
      GUIData( ImageIdentifierInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   Identifier_Sizer;
            Label             Identifier_Label;
            Edit              Identifier_Edit;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_EditCompleted( Edit& sender );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ImageIdentifierInterface* TheImageIdentifierInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageIdentifierInterface_h

// ----------------------------------------------------------------------------
// EOF ImageIdentifierInterface.h - Released 2025-04-07T08:53:56Z
