//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ImageIdentifierInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ImageIdentifierInstance.h"

#include <pcl/View.h>
#include <pcl/ImageWindow.h> // for UndoFlags
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIdentifierInstance::ImageIdentifierInstance( const MetaProcess* m )
   : ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

ImageIdentifierInstance::ImageIdentifierInstance( const ImageIdentifierInstance& x )
   : ProcessImplementation( x )
   , id( x.id )
{
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInstance::Validate( pcl::String& info )
{
   id.Trim();

   /*
    * The identifier can either be empty, meaning that a default image
    * identifier will be automatically assigned by the PixInsight core
    * application, or a valid C identifier.
    *
    * We only have to check validity of our identifier here. Uniqueness in any
    * particular naming context is enforced by the core application.
    */
   if ( !id.IsEmpty() )
      if ( !id.IsValidIdentifier() ) // String::IsValidIdentifier() does the job
      {
         info = '\'' + id + "' is not a valid identifier.";
         return false;
      }

   info.Clear();
   return true;
}

// ----------------------------------------------------------------------------

void ImageIdentifierInstance::Assign( const ProcessImplementation& p )
{
   const ImageIdentifierInstance* x = dynamic_cast<const ImageIdentifierInstance*>( &p );
   if ( x != nullptr )
      id = x->id;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInstance::IsMaskable( const View&, const ImageWindow& /*mask*/ ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags ImageIdentifierInstance::UndoMode( const View& ) const
{
   return UndoFlag::ImageId;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( !v.IsMainView() )
   {
      whyNot = "ImageIdentifier can only be executed on main views, not on previews.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInstance::ExecuteOn( View& view )
{
   Console().WriteLn( "id = " + id );
   view.Rename( id );
   return true;
}

// ----------------------------------------------------------------------------

void* ImageIdentifierInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheImageIdentifierParameter )
      return id.Begin();
   return nullptr;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheImageIdentifierParameter )
   {
      id.Clear();
      if ( sizeOrLength > 0 )
         id.SetLength( sizeOrLength );
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

size_type ImageIdentifierInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheImageIdentifierParameter )
      return id.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIdentifierInstance.cpp - Released 2025-04-07T08:53:56Z
