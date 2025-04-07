//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// ExclusiveAccess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef EXCLUSIVEACCESS_H
#define EXCLUSIVEACCESS_H

#include <pcl/AutoLock.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <class T>
class ExclusiveAccess
{
public:

   typedef T   item_type;

   ExclusiveAccess( Mutex& mutex, item_type& item ) :
      m_lock( mutex ),
      m_item( item )
   {
   }

   operator item_type&()
   {
      return m_item;
   }

   operator const item_type&() const
   {
      return m_item;
   }

private:

   AutoLock   m_lock;
   item_type& m_item;
};

template <class T>
class ExclusiveConstAccess
{
public:

   typedef T   item_type;

   ExclusiveConstAccess( Mutex& mutex, const item_type& item ) :
      m_lock( mutex ),
      m_item( item )
   {
   }

   operator const item_type&() const
   {
      return m_item;
   }

private:

         AutoLock   m_lock;
   const item_type& m_item;
};

}
#endif // EXCLUSIVEACCESS_H

// ----------------------------------------------------------------------------
// EOF ExclusiveAccess.h - Released 2025-04-07T08:53:56Z
