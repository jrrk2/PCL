//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Version.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Version_h
#define __PCL_Version_h

/// \file pcl/Version.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Version
 * \brief Provides PCL version numbers.
 *
 * The %Version class reads PCL version data stored in static read-only
 * variables. It provides version information for the PCL libraries that have
 * been linked to the current module.
 */
class PCL_CLASS Version
{
public:

   /*!
    * Default constructor. This constructor is disabled because %Version is not
    * an instantiable class.
    */
   Version() = delete;

   /*!
    * Copy constructor. This constructor is disabled because %Version is not an
    * instantiable class.
    */
   Version( const Version& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %Version is not an
    * instantiable class.
    */
   Version& operator =( const Version& ) = delete;

   /*!
    * Destructor. This destructor is disabled because %Version is not an
    * instantiable class.
    */
   ~Version() = delete;

   /*!
    * Returns the current PCL major version number.
    */
   static int Major();

   /*!
    * Returns the current PCL minor version number.
    */
   static int Minor();

   /*!
    * Returns the current PCL release version number.
    */
   static int Release();

   /*!
    * Returns the current PCL build number.
    *
    * \note Since core version 1.8.7 build numbers have been deprecated. They
    * are maintained exclusively for compatibility with previous versions and
    * existing code.
    */
   static int Build();

   /*!
    * Returns the beta version number of PCL, or zero if the current PCL
    * framework is a release (non-beta) version.
    */
   static int BetaRelease();

   /*!
    * Returns an ISO 639.2 language code that identifies the primary language
    * of the current PCL framework.
    */
   static String LanguageCode();  // ISO 639.2 language code

   /*!
    * Returns an integer that corresponds to the full PCL version.
    *
    * The returned value is equal to:
    *
    * \code Major()*1000000 + Minor()*1000 + Release(); \endcode
    */
   static int Number()
   {
      return Major()*1000000 + Minor()*1000 + Release();
   }

   /*!
    * Returns a human-readable representation of the current PCL version.
    */
   static String AsString();
};

// ----------------------------------------------------------------------------

/*!
 * \class PixInsightVersion
 * \brief Provides version information for the PixInsight core application.
 *
 * The %PixInsightVersion class retrieves version data from the PixInsight core
 * application where the current module has been installed.
 */
class PCL_CLASS PixInsightVersion
{
public:

   /*!
    * Default constructor. This constructor is disabled because
    * %PixInsightVersion is not an instantiable class.
    */
   PixInsightVersion() = delete;

   /*!
    * Copy constructor. This constructor is disabled because %PixInsightVersion
    * is not an instantiable class.
    */
   PixInsightVersion( const PixInsightVersion& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %PixInsightVersion is
    * not an instantiable class.
    */
   PixInsightVersion& operator =( const PixInsightVersion& ) = delete;

   /*!
    * Destructor. This destructor is disabled because %PixInsightVersion is not
    * an instantiable class.
    */
   ~PixInsightVersion() = delete;

   /*!
    * Returns the major version number of the PixInsight core application.
    */
   static int Major();

   /*!
    * Returns the minor version number of the PixInsight core application.
    */
   static int Minor();

   /*!
    * Returns the release version number of the PixInsight core application.
    */
   static int Release();

   /*!
    * Returns the revision version number of the PixInsight core application.
    */
   static int Revision();

   /*!
    * Returns the build number of the PixInsight core application.
    *
    * \deprecated Since core version 1.8.7 build numbers have been deprecated.
    * They are maintained exclusively for compatibility with previous versions
    * and existing code.
    */
   static int Build();

   /*!
    * Returns the beta or release candidate version number of the PixInsight
    * core application:
    *
    * \li Returns zero for a release version.
    * \li Returns a positive nonzero integer for a beta version.
    * \li Returns a negative integer for a release candidate (RC) version.
    */
   static int BetaRelease();

   /*!
    * Returns true iff the PixInsight core is a confidential (pre-release)
    * edition.
    */
   static bool Confidential();

   /*!
    * Returns true iff the PixInsight core is a limited edition (LE) release.
    */
   static bool LE();

   /*!
    * Returns an ISO 639.2 language code that identifies the primary language
    * of the PixInsight core application.
    */
   static String LanguageCode();

   /*!
    * Returns the codename for the current PixInsight core application version.
    * Starting from version 1.7, each major PixInsight version is given a
    * codename that uniquely identifies it.
    *
    * Codename history:
    *
    * PixInsight 1.7 Starbuck \n
    * PixInsight 1.8 Ripley
    */
   static String Codename();

   /*!
    * Returns an integer that corresponds to the full PixInsight core version.
    *
    * The returned value is equal to:
    *
    * \code Major()*1000000 + Minor()*1000 + Release(); \endcode
    */
   static int Number()
   {
      return Major()*1000000 + Minor()*1000 + Release();
   }

   /*!
    * Returns a human-readable textual representation of the current PixInsight
    * core version.
    *
    * \param withCodename  If this parameter is true, the returned version
    *                      string will include the codename of the running
    *                      PixInsight core application. The default value of
    *                      this parameter is false.
    */
   static String AsString( bool withCodename = false );

   friend class GlobalContextDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Version_h

// ----------------------------------------------------------------------------
// EOF pcl/Version.h - Released 2025-04-07T08:52:44Z
