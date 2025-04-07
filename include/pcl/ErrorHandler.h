//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ErrorHandler.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ErrorHandler_h
#define __PCL_ErrorHandler_h

/// \file pcl/ErrorHandler.h

#include <pcl/Defs.h>

#include <pcl/Exception.h>

#include <new> // std::bad_alloc

// ----------------------------------------------------------------------------

#ifdef __PCL_SILENTLY_REPLACE_USER_ERROR_MACROS

#undef ERROR_HANDLER
#undef ERROR_CLEANUP

#else

#ifdef ERROR_HANDLER
#error ERROR_HANDLER macro: Already defined!
#endif

#ifdef ERROR_CLEANUP
#error ERROR_CLEANUP macro: Already defined!
#endif

#endif

// ----------------------------------------------------------------------------

/*!
 * \defgroup error_handling_macros Error Handling Macros
 */

// ----------------------------------------------------------------------------

/*!
 * \def ERROR_HANDLER
 * \brief Standard PCL error handler macro
 *
 * Use the ERROR_HANDLER macro after try blocks in all PCL programs and
 * modules.
 *
 * \ingroup error_handling_macros
 */
#define ERROR_HANDLER                                                         \
   catch ( pcl::FatalError& )                                                 \
   {                                                                          \
      /* Launch fatal errors to the hyperspace... */                          \
      throw;                                                                  \
   }                                                                          \
   catch ( pcl::ProcessAborted& )                                             \
   {                                                                          \
      /* Abort process: Let the PI application do the GUI output */           \
   }                                                                          \
   catch ( pcl::CaughtException& )                                            \
   {                                                                          \
      /* Exceptions already caught and managed: simply ignore them */         \
   }                                                                          \
   catch ( pcl::Exception& x )                                                \
   {                                                                          \
      /* Other PCL exceptions */                                              \
      x.Show();                                                               \
   }                                                                          \
   catch ( pcl::String& s )                                                   \
   {                                                                          \
      /* Nonstandard ways of throwing things... */                            \
      try                                                                     \
      {                                                                       \
         throw pcl::Error( s );                                               \
      }                                                                       \
      catch ( pcl::Exception& x )                                             \
      {                                                                       \
         x.Show();                                                            \
      }                                                                       \
   }                                                                          \
   catch ( std::bad_alloc& )                                                  \
   {                                                                          \
      /* Standard bad allocation exception */                                 \
      try                                                                     \
      {                                                                       \
         throw pcl::Error( "Out of memory" );                                 \
      }                                                                       \
      catch ( pcl::Exception& x )                                             \
      {                                                                       \
         x.Show();                                                            \
      }                                                                       \
   }                                                                          \
   catch ( ... )                                                              \
   {                                                                          \
      /* Catch-all */                                                         \
      try                                                                     \
      {                                                                       \
         throw pcl::Error( "Unknown exception" );                             \
      }                                                                       \
      catch ( pcl::Exception& x )                                             \
      {                                                                       \
         x.Show();                                                            \
      }                                                                       \
   }

// ----------------------------------------------------------------------------

/*!
 * \def ERROR_CLEANUP
 * \brief Standard PCL error handler macro with cleanup code
 *
 * Use ERROR_CLEANUP in the same way as the ERROR_HANDLER macro, when you need
 * to specify a local clean up code, e.g. to delete dynamically allocated local
 * variables.
 *
 * \ingroup error_handling_macros
 */
#define ERROR_CLEANUP( cleanup_code )                                         \
   catch ( ... )                                                              \
   {                                                                          \
      cleanup_code;                                                           \
                                                                              \
      try                                                                     \
      {                                                                       \
         throw;                                                               \
      }                                                                       \
      ERROR_HANDLER                                                           \
   }

// ----------------------------------------------------------------------------

#endif   // __PCL_ErrorHandler_h

// ----------------------------------------------------------------------------
// EOF pcl/ErrorHandler.h - Released 2025-04-07T08:52:44Z
