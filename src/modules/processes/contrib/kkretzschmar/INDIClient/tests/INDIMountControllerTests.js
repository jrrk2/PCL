// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// INDIMountControllerTests.js - Released 2025-04-07T08:53:56Z
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

#include "Asserts.jsh"
#include "INDI-helper.jsh"
#include "CoordUtils.jsh"

#define MOUNT_DEVICE_NAME "Mount Simulator @ localhost" // Indigo simulator device name

function INDIMountControllerTests( parent )
{
   this.__base__ = Test;
   this.__base__( "INDIMountControllerTests", parent );

   this.add(
      function testUnpark()
      {
         let mountController = new IndigoMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
      }
   );

 /*  this.add(
      function testMoveNorthSouth()
      {
         let mountController = new IndigoMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         var origPosRA = mountController.currentRA;
         var origPosDEC = mountController.currentDec;
         // set command type
         mountController.Command = 2; // MoveNorth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 3; // MoveNorth_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( origPosRA, mountController.currentRA, 0.1 );
         //expectTrue( origPosDEC < mountController.currentDec );


         origPosRA = mountController.currentRA;
         origPosDEC = mountController.currentDec;
         // set command type
         mountController.Command = 4; // MoveSouth_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 5; // MoveSouth_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( origPosRA, mountController.currentRA, 0.1 );
         //expectTrue( origPosDEC > mountController.currentDec );
      }
   );

   this.add(
      function testMoveWestEast()
      {
         let mountController = new IndigoMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         var origPosRA = mountController.currentRA;
         var origPosDEC = mountController.currentDec;
         // set command type
         mountController.Command = 6; // MoveWest_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 7; // MoveWest_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         console.writeln("origPosRA: " + origPosRA);
         console.writeln("currentRA: " + mountController.currentRA);
         expectTrue( origPosRA < mountController.currentRA );
         expectEqualsWithPrecision( origPosDEC, mountController.currentDec, 0.1 );
         origPosRA = mountController.currentRA;
         origPosDEC = mountController.currentDec;
         // set command type
         mountController.Command = 8; // MoveEast_Start
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         msleep( 2000 );
         mountController.Command = 9; // MoveEast_Stop
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         //expectTrue( origPosRA > mountController.currentRA );
         expectEqualsWithPrecision( origPosDEC, mountController.currentDec, 0.1 );
      }
   );

   this.add(
      function testGoto()
      {
         let mountController = new IndigoMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // execute in the global context to unpark
         assertTrue( mountController.executeGlobal() );
         // set target coordinates
         mountController.targetRA = 1;
         mountController.targetDec = 15;
         mountController.Command = 10; // Goto
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( 1.0, mountController.currentRA, 0.1 );
         expectEqualsWithPrecision( 15.0, mountController.currentDec, 0.1 );
      }
   );
*/


   this.add(
      function testPointingModel()
      {
         let currentDir = File.extractDirectory( #__FILE__ );
         let pointingModelFileOrig = currentDir + "/TestPointingModel.xtpm";
         let pointingModelFile = currentDir + "/TestPointingModelTmp.xtpm";
         let pointingModelFileCreated = currentDir + "/TestPointingModelCreated.xtpm";

         let mountController=new IndigoMount;
         mountController.deviceName=MOUNT_DEVICE_NAME;
         mountController.alignmentModelFile=pointingModelFile;
         assertTrue(File.exists(mountController.alignmentModelFile));
         mountController.AlignmentMethod=1; // Analytical Model

         if (File.exists(pointingModelFile)){
            File.remove(pointingModelFile);
            File.copyFile(pointingModelFile, pointingModelFileOrig);
         }

         // collect sync points
          var  ra_lower    = 0.2;
	      var  ra_upper    = 23.7;
	      var  dec_lower   = -10;
	      var  dec_upper   = 80;
	      var  ra_step_deg = 10;
	      var  ra_step  = Math.round(ra_step_deg / 15 * 1000000000000) / 1000000000000;

	      var  dec_step_deg = 20;

	      var num_of_ra_steps  = (ra_upper - ra_lower) /  ra_step;
	      var num_of_dec_steps = (dec_upper - dec_lower) /  dec_step_deg;

	      var count=0;
	      for (var ra=ra_lower ; ra<=ra_upper ; ra+=ra_step ){
		      for (var dec=dec_lower ; dec<=dec_upper ; dec+=dec_step_deg ) {
               // set target coordinates
               mountController.targetRA = ra;
               mountController.targetDec = dec;
               // set pier side - only necessary in testing
               mountController.PierSide = hourAngleRangeShift(-ra) <= 0 ? 0 : 1; // west:0, east:1
               console.writeln( mountController.PierSide);
               // set synch command
               mountController.Command = 14; // TestSync
               assertTrue( mountController.executeGlobal() );
            }
         }

         // Fit the pointing model
         if (File.exists(pointingModelFileCreated)){
            File.remove(pointingModelFileCreated);
            File.copyFile(pointingModelFileCreated, pointingModelFile);
         }
         mountController.alignmentModelFile=pointingModelFileCreated;
         mountController.alignmentConfig = 1919;
         mountController.geographicLatitude = 49.261872611;
         mountController.Command = 13; // FitPointingModel
         assertTrue( mountController.executeGlobal());


         // Check the paramters
         let modelValuesExpected = parseXTPMFile(pointingModelFileOrig);
         let modelValues         = parseXTPMFile(pointingModelFileCreated);

         // model config
         expectEquals(modelValuesExpected.ModelConf, modelValues.ModelConf,"model configuration not equal");

         // geo latitude
         expectEqualsWithPrecision( modelValuesExpected.GeoLat, modelValues.GeoLat, 0.0001 );

         // model parameters (west)
         expectEquals(modelValuesExpected.ModelParamWest.length, modelValues.ModelParamWest.length, "model (west) paramaters not equal" )
         for (var i=0; i < modelValuesExpected.ModelParamWest.length; ++i){
        	   let paramExpected = modelValuesExpected.ModelParamWest[i];
        	   let param = modelValues.ModelParamWest[i];
            expectEqualsWithPrecision( paramExpected, param, 0.01 );
         }
         // model parameters (east)
         expectEquals(modelValuesExpected.ModelParamEast.length, modelValues.ModelParamEast.length, "model (east) paramaters not equal" )
         for (var i=0; i < modelValuesExpected.ModelParamEast.length; ++i){
        	   let paramExpected = modelValuesExpected.ModelParamEast[i];
         	let param = modelValues.ModelParamEast[i];
            expectEqualsWithPrecision( paramExpected, param, 0.01 );
         }
      }
   );

   this.add(
      function testParking()
      {
         let mountController = new IndigoMount;
         mountController.deviceName = MOUNT_DEVICE_NAME;
         // set park command
         mountController.Command = 1; // Park
         // execute in the global context
         assertTrue( mountController.executeGlobal() );
         // check current coordinates
         expectEqualsWithPrecision( 12.0, mountController.currentLST - mountController.currentRA, 0.1 );
         expectEqualsWithPrecision( 0.0, mountController.currentDec, 0.1 );
      }
   );
}

INDIMountControllerTests.prototype = new Test;

// ----------------------------------------------------------------------------
// EOF INDIMountControllerTests.js - Released 2025-04-07T08:53:56Z
