// ----------------------------------------------------------------------------
// PixInsight JavaScript Runtime API - PJSR Version 1.0
// ----------------------------------------------------------------------------
// Asserts.jsh - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
//
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef _Asserts_jsh
#define _Asserts_jsh

function assertTrue( condition, message )
{
   if ( !condition )
   {
      message = message || "Assertion failed";
      if ( typeof Error !== "undefined" )
         throw new Error( message );
      throw message; // Fallback
   }
}

function assertEquals( expected, actual, message )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               message + ": type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected === actual, message );
}

function expectTrue( condition )
{
   assertTrue( condition, "condition violated: true != " +  condition );
}

function expectEquals( expected, actual )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected === actual,
               "Expected: '" + expected + "' != actual: '" + actual + "'" );
}

function expectEqualsWithPrecision( expected, actual, precision )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( Math.abs( expected - actual ) < precision,
               "Expected: |" + expected + " - " +  actual + "|  smaller than  " + precision );
}

function assertNotEquals( expected, actual, message )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               message + ": type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected !== actual, message );
}

function expectNotEquals( expected, actual )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( expected !== actual,
               "Expected: '" + expected + "' == actual: '" + actual + "'" );
}

function expectNotEqualsWithPrecision( expected, actual, precision )
{
   // check type of arguments
   assertTrue( typeof expected === typeof actual,
               " type of expected !== type of actual: '" + typeof expected + "' !== '" + typeof actual + "'" );
   // check values
   assertTrue( Math.abs( expected - actual ) >= precision,
               "Expected: |" + expected + " - " +  actual + "|  larger than  " + precision );
}

function parseXTPMFile(fileName) {
   let result = {"GeoLat" : 0, "ModelConf": 0, "ModelParamWest" : [], "ModelParamEast" : []};

   let lines = File.readLines(fileName);
   for (var i = 0; i < lines.length ;i++) {
	   let line = lines[i];
	   let tokens1 = line.split('>');
	   if (tokens1.length > 1) {
		 let tokens2 = tokens1[1].split('<');
		 var xmlElement = tokens1[0];
	     var xmlValue = tokens2[0];
	     if (xmlElement.indexOf("GeographicLatitude") !== -1) {
	    	 result.GeoLat = parseFloat(xmlValue);
	     } else if (xmlElement.indexOf("Configuration") !== -1) {
	    	 result.ModelConf = parseInt(xmlValue);
	     } else if (xmlElement.indexOf("ModelParameters PierSide=\"West\"") !== -1) {
	    	 let params = xmlValue.split(',');
	    	 for (var j = 0; j < params.length; ++j){
	    		 result.ModelParamWest[j] = parseFloat(params[j]);
	    	 }
	     } else if (xmlElement.indexOf("ModelParameters PierSide=\"East\"") !== -1) {
	    	 let params = xmlValue.split(',');
	    	 for (var j = 0; j < params.length; ++j){
	    		 result.ModelParamEast[j] = parseFloat(params[j]);
	    	 }
	     }
       }
   }
   return result;
}


#endif // _Asserts_jsh

// ----------------------------------------------------------------------------
// EOF Asserts.jsh - Released 2025-04-07T08:53:56Z
