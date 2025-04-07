//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.5
// ----------------------------------------------------------------------------
// APASSProcess.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __APASSProcess_h
#define __APASSProcess_h

#include <pcl/APASSDatabaseFile.h>
#include <pcl/MetaProcess.h>
#include <pcl/Mutex.h>
#include <pcl/ReferenceArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class APASSProcess : public MetaProcess
{
public:

   APASSProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool PrefersGlobalExecution() const override;
   bool CanProcessCommandLines() const override;
   int ProcessCommandLine( const StringList& ) const override;

   bool CanProcessIPCMessages() const override;
   void IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const override;
   void IPCStop( int instance, const IsoString& messageUID ) const override;
   void IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const override;
   int IPCStatus( int instance, const IsoString& messageUID ) const override;

   void SetDatabaseFilePaths( int dr, const StringList& );
   const StringList& DatabaseFilePaths( int dr ) const;

   bool IsValid( int dr ) const;
   bool HasDatabaseFiles( int dr ) const;

   void EnsureDatabasesInitialized( int dr, int verbosity = 0 ); // thread-safe
   void CloseDatabases( int dr );
   const ReferenceArray<APASSDatabaseFile>& Databases( int dr ) const;
   float MagnitudeLow( int dr ) const;
   float MagnitudeHigh( int dr ) const;

   bool PreferencesLoaded( int dr ) const;
   void LoadPreferences( int dr );
   void SavePreferences( int dr );

private:

   class IPCWorker
   {
   public:

      IPCWorker() = default;
      IPCWorker( const IPCWorker& ) = delete;

      void Start( const String& parameters );
      void Stop();
      int Status() const;
      bool IsRunning() const;
      bool IsError() const;

   private:

      String        m_task;
      pcl_enum      m_dataRelease;
      StringList    m_xpsdFilePaths;
      int           m_verbosity; // 0=silent 1=minimal 2=full
      bool          m_running = false;
      bool          m_error = false;
      mutable Mutex m_mutex;

      void GetParameters( const String& );
      void SetRunning();
      void SetNotRunning();
      void SetError();
      void ClearData();
      void ClearStatus();
   };

   mutable IPCWorker m_ipcWorker;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern APASSProcess* TheAPASSProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __APASSProcess_h

// ----------------------------------------------------------------------------
// EOF APASSProcess.h - Released 2025-04-07T08:53:55Z
