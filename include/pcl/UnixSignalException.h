//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/UnixSignalException.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_UnixSignalException_h
#define __PCL_UnixSignalException_h

/// \file pcl/UnixSignalException.h

#if defined( __PCL_WINDOWS ) || !defined( __PCL_LINUX ) && !defined( __PCL_FREEBSD ) && !defined( __PCL_MACOSX )
#  error UnixSignalException can only be used on Linux, FreeBSD and macOS platforms.
#endif

#include <pcl/Defs.h>

#include <pcl/Exception.h>

#include <signal.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class UnixSignalException
 * \brief A UNIX synchronous signal handler that throws C++ exceptions.
 *
 * This class can only be used on Linux, FreeBSD and macOS platforms. On
 * Windows platforms, including this header file from compilable code raises a
 * compilation error.
 *
 * For this handler to work properly, all code that may raise synchronous
 * signals (SIGSEGV and the like) must be compiled with GCC's
 * '-fnon-call-exceptions' flag. Otherwise the exceptions will be thrown
 * but terminate() will be called, which is the default critical signal
 * management behavior.
 *
 * To generate a backtrace report (see the UnixSignalException::Details()
 * member function), the code must be compiled with the '-rdynamic' GCC flag,
 * which instructs the linker to add all symbols to the dynamic symbol table.
 * In addition, the generated binaries should not be stripped with the '-s'
 * linker flag.
 */
class PCL_CLASS UnixSignalException : public pcl::Exception
{
public:

   /*!
    * Constructs a new %UnixSignalException object with the specified \a signal
    * number and optional backtrace \a details.
    */
   UnixSignalException( int signal, const IsoString& details = IsoString() )
      : m_signal( signal )
      , m_details( details )
   {
   }

   /*!
    * Copy constructor.
    */
   UnixSignalException( const UnixSignalException& ) = default;

   /*!
    * Returns the signal number associated with this object.
    */
   int SignalNumber() const
   {
      return m_signal;
   }

   /*!
    * Returns the backtrace information associated with this exception.
    *
    * The returned string will be empty if no backtrace data were available at
    * the time this signal exception was generated. This happens when the code
    * has not been compiled and linked with the appropriate options (see the
    * -rdynamic compiler flag).
    */
   const IsoString& Details() const
   {
      return m_details;
   }

   /*!
    * Returns an error or warning message corresponding to this signal
    * exception. Typical messages are "segmentation violation", "bus error" and
    * "floating point exception", returned by specific derived classes.
    */
   String Message() const override
   {
      return "Undefined signal";
   }

   /*!
    * Returns a formatted error message with information on this signal
    * exception.
    */
   String FormatInfo() const override
   {
      String info = String().Format( "Critical signal caught (%d): ", SignalNumber() ) + Message();
      if ( !m_details.IsEmpty() )
      {
         info.Append( '\n' );
         info.Append( m_details );
      }
      return info;
   }

   /*!
    * Returns the type of this exception, intended to be used as a caption for
    * a message box. As reimplemented in this class, this member function
    * returns the string "PCL Unix Signal Handler".
    */
   String Caption() const override
   {
      return "PCL Unix Signal Handler";
   }

   /*!
    * Writes a formatted representation of this exception on the platform
    * console. A plain text version of the same textual representation will
    * also be written on stdout.
    *
    * \note UNIX synchronous interrupts are never reported on interactive
    * graphical interfaces, such as message boxes, irrespective of global
    * platform settings or local settings defined through calls to
    * EnableGUIOutput() and similar functions.
    */
   void Show() const override;

   /*!
    * Initializes the UNIX synchronous signal handler. This static member
    * function must be called before the calling process can raise any
    * synchronous signal.
    *
    * \note A module should never call this member function. It is invoked when
    * appropriate by the PixInsight core application and internal PCL routines.
    */
   static void Initialize();

protected:

   int       m_signal;  // signal number
   IsoString m_details; // backtrace information
};

// ----------------------------------------------------------------------------

#define DECLARE_UNIX_SIGNAL_EXCEPTION( className, sigNum, message )           \
   class PCL_CLASS className : public pcl::UnixSignalException                \
   {                                                                          \
   public:                                                                    \
      className( const IsoString& details = IsoString() ) :                   \
         pcl::UnixSignalException( sigNum, details )                          \
      {                                                                       \
      }                                                                       \
      className( const className& ) = default;                                \
      String Message() const override                                         \
      {                                                                       \
         return message;                                                      \
      }                                                                       \
   }

// ----------------------------------------------------------------------------

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixSegmentationViolation, SIGSEGV,
                               "Segmentation violation" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixBusError, SIGBUS,
                               "Bus error" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixFloatingPointException, SIGFPE,
                               "Floating point exception" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixIllegalInstructionException, SIGILL,
                               "Illegal instruction" );

DECLARE_UNIX_SIGNAL_EXCEPTION( EUnixIBrokenPipeException, SIGPIPE,
                               "Broken pipe" );

// ----------------------------------------------------------------------------

#undef DECLARE_UNIX_SIGNAL_EXCEPTION

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_UnixSignalException_h

// ----------------------------------------------------------------------------
// EOF pcl/UnixSignalException.h - Released 2025-04-07T08:52:44Z
