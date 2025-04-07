//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Container.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Container_h
#define __PCL_Container_h

/// \file pcl/Container.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Container
 * \brief Root base class of all PCL containers.
 */
class PCL_CLASS Container
{
};

/*!
 * \class DirectContainer
 * \brief Root base class of all PCL containers of objects.
 */
template <typename T>
class PCL_CLASS DirectContainer : public Container
{
public:

   /*!
    * The type of contained objects.
    */
   using item_type = T;

   /*!
    * The type of contained immutable objects.
    */
   using const_item_type = const T;
};

/*!
 * \class IndirectContainer
 * \brief Root base class of all PCL containers of pointers to objects.
 */
template <typename T>
class PCL_CLASS IndirectContainer : public Container
{
public:

   /*!
    * The type of contained pointers.
    */
   using item_type = T*;

   /*!
    * The type of contained pointers to immutable objects.
    */
   using const_item_type = const T*;
};

/*!
 * \class DirectSortedContainer
 * \brief Root base class of all PCL sorted containers of objects.
 */
template <typename T>
class PCL_CLASS DirectSortedContainer: public DirectContainer<T>
{
public:

   /*!
    * The type of contained objects.
    */
   using item_type = typename DirectContainer<T>::item_type;

   /*!
    * The type of contained immutable objects.
    */
   using const_item_type = typename DirectContainer<T>::const_item_type;
};

/*!
 * \class IndirectSortedContainer
 * \brief Root base class of all PCL sorted containers of pointers to objects.
 */
template <typename T>
class PCL_CLASS IndirectSortedContainer : public IndirectContainer<T>
{
public:

   /*!
    * The type of contained pointers.
    */
   using item_type = typename IndirectContainer<T>::item_type;

   /*!
    * The type of contained pointers to immutable objects.
    */
   using const_item_type = typename IndirectContainer<T>::const_item_type;
};

#define PCL_ASSERT_DIRECT_CONTAINER( C, T )                                   \
   static_assert( std::is_base_of<DirectContainer<T>, C>::value,              \
                  "Argument type must derive from DirectContainer<T>." )

#define PCL_ASSERT_INDIRECT_CONTAINER( C, T )                                 \
   static_assert( std::is_base_of<IndirectContainer<T>, C>::value,            \
                  "Argument type must derive from IndirectContainer<T>." )

#define PCL_ASSERT_CONTAINER( C, T )                                          \
   static_assert( std::is_base_of<DirectContainer<T>, C>::value               \
               || std::is_base_of<IndirectContainer<T>, C>::value,            \
                  "Argument type must derive from DirectContainer<T> or IndirectContainer<T>." )

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Container_h

// ----------------------------------------------------------------------------
// EOF pcl/Container.h - Released 2025-04-07T08:52:44Z
