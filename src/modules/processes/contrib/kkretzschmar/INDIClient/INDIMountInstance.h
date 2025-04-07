//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIMountInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDIMountInstance_h
#define __INDIMountInstance_h

#include <pcl/Array.h>
#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "Alignment.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIMountInstance : public ProcessImplementation
{
public:

   INDIMountInstance( const MetaProcess* );
   INDIMountInstance( const INDIMountInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& v ) const override;
   bool ExecuteOn( View& view ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter* p, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   bool ValidateDevice( bool throwErrors = true ) const;
   void SendDeviceProperties( bool asynchronous = true ) const;

   static String MountSlewRatePropertyString( int slewRateIdx );

   void GetCurrentCoordinates();
   void GetTargetCoordinates( double& ra, double& dec ) const;
   void GetPierSide();
   bool isForceCounterWeightUp() const;

   void AddSyncDataPoint( const SyncDataPoint& syncDataPoint );

   static void loadSyncData( Array<SyncDataPoint>& syncDataList, String syncDataFile );

private:

   String p_deviceName;
   pcl_enum p_command;
   pcl_enum p_slewRate;
   pcl_enum p_alignmentMethod;
   pcl_enum p_pierSide;
   double p_targetRA;
   double p_targetDec;
   pcl_bool p_enableAlignmentCorrection;
   String p_alignmentFile;
   uint32 p_alignmentConfig;

   double o_currentLST;
   double o_currentRA;
   double o_currentDec;
   mutable double o_apparentTargetRA;
   mutable double o_apparentTargetDec;
   double o_geographicLatitude;
   double o_syncLST;
   double o_syncCelestialRA;
   double o_syncCelestialDEC;
   double o_syncTelescopeRA;
   double o_syncTelescopeDEC;

   Array<SyncDataPoint> syncDataArray;

   friend class INDIMountInterface;
   friend class AbstractINDIMountExecution;
};

// ----------------------------------------------------------------------------

class AbstractINDIMountExecution
{
public:

   AbstractINDIMountExecution( INDIMountInstance& instance )
      : m_instance( instance )
   {
   }

   virtual ~AbstractINDIMountExecution()
   {
   }

   virtual void Perform();
   virtual bool Perform(View& view);

   void Perform( pcl_enum command )
   {
      m_instance.p_command = command;
      Perform();
   }

   virtual void Abort();

   const INDIMountInstance& Instance() const
   {
      return m_instance;
   }

   bool IsRunning() const
   {
      return m_running;
   }

   bool WasAborted() const
   {
      return m_aborted;
   }

protected:

   INDIMountInstance& m_instance;

   virtual void StartMountEvent( double targetRA, double currentRA, double targetDec, double currentDec, pcl_enum command ) = 0;
   virtual void MountEvent( double targetRA, double currentRA, double targetDec, double currentDec ) = 0;
   virtual void EndMountEvent() = 0;
   virtual void WaitEvent() = 0;
   virtual void AbortEvent() = 0;

private:

   bool m_running = false;
   bool m_aborted = false;

   void ApplyPointingModelCorrection( AlignmentModel* aModel, double& targetRA, double& targetDec );
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDIMountInstance_h

// ----------------------------------------------------------------------------
// EOF INDIMountInstance.h - Released 2025-04-07T08:53:56Z
