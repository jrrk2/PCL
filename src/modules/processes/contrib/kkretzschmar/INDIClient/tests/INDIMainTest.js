// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// INDIMainTest.js - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
//
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pjsr/Test.jsh>

#include "INDICCDControllerTests.js"

#define host "localhost"

function INDICCDTestSuite()
{
   this.__base__ = Test;
   this.__base__( "INDICCDTestSuite" );

   this.deviceController = new IndigoDeviceController;
   this.deviceController.serverHostName = "localhost"
   this.timer = new ElapsedTime;

   this.add( new INDICCDControllerTests( this ) );

   this.executeController = function()
   {
      if ( !this.deviceController.executeGlobal() )
         throw new Error( "IndigoDeviceController.executeGlobal() failed" );
   };

   this.restartTimer = function()
   {
      this.timer.reset();
   };

   this.stopTimer = function ()
   {
      this.timer.stop();
   };
   this.timeout = function()
   {
      if ( this.timer.value > 10 )
      {
         console.criticalln( "Timeout reached" );
         return true;
      }
      return false;
   };

   // N.B. begin() and end() methods reimplemented from Test

   this.begin = function()
   {
      // Connect to INDI server
      this.deviceController.serverHostName = host
      this.deviceController.serverConnect = true;
      this.executeController();

      // Wait until device names are received from server
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( indexOfDevice( (new IndigoDeviceController).devices, CCD_DEVICE_NAME ) >= 0 )
            break;
      }
      this.stopTime();

      // Wait for another 500 ms to allow for all device and property lists to
      // update completely.
      msleep( 500 );

      // Connect to CCD device
      let propertyKey = "/" + CCD_DEVICE_NAME + "/CONNECTION/CONNECTED";
      this.deviceController.newProperties = [[propertyKey, "INDI_SWITCH", "ON"]];
      this.deviceController.serverCommand = "SET";
      this.executeController();
      this.deviceController.serverCommand = "";

      // Wait until device is connected
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( propertyEquals( (new IndigoDeviceController).properties, propertyKey, "ON" ) )
            break;
      }
   };

   this.end = function()
   {
      // Disconnect from INDI server
      this.deviceController.serverConnect = false;
      this.executeController();
   };
};

#include "INDIMountControllerTests.js"

function INDIMountTestSuite()
{
   this.__base__ = Test;
   this.__base__( "INDIMountTestSuite" );

<<<<<<< HEAD
   this.deviceController = new IndigoDeviceController;
=======
   this.deviceController = new INDIDeviceController;
   this.deviceController.serverHostName = "klaus-Inspiron-1720"
>>>>>>> 7c912548af71aec64089d296987ff43fbbe876ba
   this.timer = new ElapsedTime;

   this.add( new INDIMountControllerTests( this ) );

   this.executeController = function()
   {
      if ( !this.deviceController.executeGlobal() )
         throw new Error( "IndigoDeviceController.executeGlobal() failed" );
   };

   this.restartTimer = function()
   {
      this.timer.reset();
   };

   this.timeout = function()
   {
      if ( this.timer.value > 10 )
      {
         console.criticalln( "Timeout reached" );
         return true;
      }
      return false;
   };

   // N.B. begin() and end() methods reimplemented from Test

   this.begin = function()
   {
      // Connect to INDI server
      this.deviceController.serverConnect = true;
      this.executeController();

      // Wait until device names are received from server
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( indexOfDevice( (new IndigoDeviceController).devices, MOUNT_DEVICE_NAME ) >= 0 )
            break;
      }

      // Wait for another 500 ms to allow for all device and property lists to
      // update completely.
      msleep( 500 );

      // Connect to Mount device
      let propertyKey = "/" + MOUNT_DEVICE_NAME + "/CONNECTION/CONNECTED";
      this.deviceController.newProperties = [[propertyKey, "INDI_SWITCH", "ON"]];
      this.deviceController.serverCommand = "SET";
      this.executeController();
      this.deviceController.serverCommand = "";

      // Wait until device is connected
      for ( this.restartTimer(); !this.timeout(); )
      {
         msleep( 100 );
         processEvents();
         if ( propertyEquals( (new IndigoDeviceController).properties, propertyKey, "ON" ) )
            break;
      }
   };

   this.end = function()
   {
      // Disconnect from INDI server
      this.deviceController.serverConnect = false;
      this.executeController();
   };
};

// run CCD tests
//INDICCDTestSuite.prototype = new Test;

//(new INDICCDTestSuite).run();

// run Mount tests
INDIMountTestSuite.prototype = new Test;

(new INDIMountTestSuite).run();

// ----------------------------------------------------------------------------
// EOF INDIMainTest.js - Released 2025-04-07T08:53:56Z
