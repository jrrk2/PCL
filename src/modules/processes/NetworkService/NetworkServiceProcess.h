//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NetworkServiceProcess_h
#define __NetworkServiceProcess_h

#include <pcl/MetaProcess.h>
#include <pcl/Mutex.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct NetworkServiceLogger;
class PCL_CLASS NetworkTransfer;

class NetworkServiceProcess : public MetaProcess
{
public:

   NetworkServiceProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool IsAssignable() const override;

   bool CanProcessIPCMessages() const override;
   void IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const override;
   void IPCStop( int instance, const IsoString& messageUID ) const override;
   void IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const override;
   int IPCStatus( int instance, const IsoString& messageUID ) const override;

   void Start( const String& parameters, NetworkServiceLogger* = nullptr ) const;
   void Stop() const;

   bool IsRunning() const
   {
      return m_worker.IsRunning();
   }

   bool StopRequested() const
   {
      return m_worker.StopRequested();
   }

private:

   class Worker
   {
   public:

      Worker() = default;
      Worker( const Worker& ) = delete;

      void Start( const String& parameters, NetworkServiceLogger* = nullptr );
      void Stop();
      int Status() const;
      bool IsRunning() const;
      bool IsError() const;
      bool StopRequested() const;

   private:

      NetworkServiceLogger* m_log = nullptr;
      String                m_taskProviderURL;
      String                m_workerName;
      String                m_taskType;
      String                m_userName;
      String                m_userPassword;
      String                m_logFilePath;
      ByteArray             m_downloadData;
      unsigned              m_waitTimeSecs;
      bool                  m_running = false;
      bool                  m_error = false;
      bool                  m_stop = false;
      bool                  m_debug = false;
      mutable Mutex         m_mutex;

      void GetParameters( const String& );
      void ValidateParameters();

      void QueryTask();
      void PerformTask();
      void Wait();

      void SetRunning();
      void SetNotRunning();
      void SetError();
      void ClearStatus();
   };

   mutable Worker m_worker;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern NetworkServiceProcess* TheNetworkServiceProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NetworkServiceProcess_h

// ----------------------------------------------------------------------------
// EOF NetworkServiceProcess.h - Released 2025-04-07T08:53:56Z
