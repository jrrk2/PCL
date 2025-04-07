//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ParallelProcess.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ParallelProcess_h
#define __PCL_ParallelProcess_h

/// \file pcl/ParallelProcess.h

#include <pcl/Defs.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ParallelProcess
 * \brief A process using multiple concurrent execution threads.
 *
 * %ParallelProcess is a base class for classes implementing multithreaded
 * processes in PCL.
 */
class PCL_CLASS ParallelProcess
{
public:

   /*!
    * Default constructor.
    *
    * By default, parallel processing is always enabled, and any process can
    * use the maximum possible number of parallel execution threads.
    */
   ParallelProcess() = default;

   /*!
    * Copy constructor.
    */
   ParallelProcess( const ParallelProcess& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~ParallelProcess()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ParallelProcess& operator =( const ParallelProcess& ) = default;

   /*!
    * Returns true iff this process is allowed to use multiple parallel
    * execution threads, when multiple threads are permitted and available.
    */
   bool IsParallelProcessingEnabled() const noexcept
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this process.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance. If \a enable is false this parameter is ignored.
    *                A value &le; 0 is ignored. The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 ) noexcept
   {
      m_parallel = enable;
      if ( m_parallel )
         if ( maxProcessors > 0 )
            SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this process.
    *
    * This is a convenience function, equivalent to:
    *
    * \code EnableParallelProcessing( !disable ) \endcode
    */
   void DisableParallelProcessing( bool disable = true ) noexcept
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this process.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable. Refer to the GlobalSettings
    * class for information on global variables.
    */
   int MaxProcessors() const noexcept
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this process.
    *
    * A value of \a maxProcessors &le; 0 is ignored and causes the call to this
    * function to have no effect.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processors actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable. Refer to the GlobalSettings
    * class for information on global variables.
    */
   void SetMaxProcessors( int maxProcessors ) noexcept
   {
      if ( maxProcessors > 0 )
         m_maxProcessors = Min( maxProcessors, PCL_MAX_PROCESSORS );
   }

   /*!
    * Exchanges two %ParallelProcess instances.
    */
   void Swap( ParallelProcess& process ) noexcept
   {
      pcl::Swap( m_maxProcessors, process.m_maxProcessors );
      pcl::Swap( m_parallel, process.m_parallel );
   }

protected:

   int  m_maxProcessors = PCL_MAX_PROCESSORS; // maximum number of processors allowed
   bool m_parallel = true; // use multiple threads?
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ParallelProcess_h

// ----------------------------------------------------------------------------
// EOF pcl/ParallelProcess.h - Released 2025-04-07T08:52:44Z
