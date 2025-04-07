//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// FluxCalibrationInterface.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FluxCalibrationInterface.h"
#include "FluxCalibrationParameters.h"
#include "FluxCalibrationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FluxCalibrationInterface* TheFluxCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

int FluxCalibrationInterface::CalibrationParameter::m_valueLabelWidth;
int FluxCalibrationInterface::CalibrationParameter::m_valueEditWidth;
int FluxCalibrationInterface::CalibrationParameter::m_keywordEditWidth;

// ----------------------------------------------------------------------------

FluxCalibrationInterface::FluxCalibrationInterface()
   : m_instance( TheFluxCalibrationProcess )
{
   TheFluxCalibrationInterface = this;
}

// ----------------------------------------------------------------------------

FluxCalibrationInterface::~FluxCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString FluxCalibrationInterface::Id() const
{
   return "FluxCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* FluxCalibrationInterface::Process() const
{
   return TheFluxCalibrationProcess;
}

// ----------------------------------------------------------------------------

String FluxCalibrationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/FluxCalibration.svg";
}

// ----------------------------------------------------------------------------

void FluxCalibrationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void FluxCalibrationInterface::ResetInstance()
{
   FluxCalibrationInstance defaultInstance( TheFluxCalibrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FluxCalibration" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheFluxCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FluxCalibrationInterface::NewProcess() const
{
   return new FluxCalibrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const FluxCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a FluxCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool FluxCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FluxCalibrationInterface::UpdateControls()
{
   GUI->Wavelength_Parameter.Update();
   GUI->Transmissivity_Parameter.Update();
   GUI->FilterWidth_Parameter.Update();
   GUI->Aperture_Parameter.Update();
   GUI->CentralObstruction_Parameter.Update();
   GUI->ExposureTime_Parameter.Update();
   GUI->AtmosphericExtinction_Parameter.Update();
   GUI->SensorGain_Parameter.Update();
   GUI->QuantumEfficiency_Parameter.Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FluxCalibrationInterface::GUIData::GUIData( FluxCalibrationInterface& w )
{
   pcl::Font fnt = w.Font();

   FluxCalibrationInterface::CalibrationParameter::m_valueLabelWidth =
         fnt.Width( String( "Central obstruction (mm):" ) + 'M' ); // the longest label text;
   FluxCalibrationInterface::CalibrationParameter::m_valueEditWidth =
         fnt.Width( String( '0', 15 ) );
   FluxCalibrationInterface::CalibrationParameter::m_keywordEditWidth =
         fnt.Width( String( 'M', 10 ) );

   Wavelength_Parameter.Build(            w.m_instance.p_wavelength,            "Wavelength (nm):",
      "<p>Effective filter wavelenth in nm (mandatory parameter).</p>",    true /* required parameter */ );

   Transmissivity_Parameter.Build(        w.m_instance.p_transmissivity,        "Transmissivity:",
      "<p>Filter transmissivity in the range (0,1].</p>"                                                 );

   FilterWidth_Parameter.Build(           w.m_instance.p_filterWidth,           "Filter width (nm):",
      "<p>Filter bandwith in nm (mandatory parameter).</p>",               true /* required parameter */ );

   Aperture_Parameter.Build(              w.m_instance.p_aperture,              "Aperture (mm):",
      "<p>Telescope aperture diameter in mm (mandatory parameter).</p>",   true /* required parameter */ );

   CentralObstruction_Parameter.Build(    w.m_instance.p_centralObstruction,    "Central obstruction (mm):",
      "<p>Telescope central obstruction diameter in mm.</p>"                                             );

   ExposureTime_Parameter.Build(          w.m_instance.p_exposureTime,          "Exposure time (s):",
      "<p>Exposure time in seconds (mandatory parameter).</p>",            true /* required parameter */ );

   AtmosphericExtinction_Parameter.Build( w.m_instance.p_atmosphericExtinction, "Atmospheric extinction:",
      "<p>Atmospheric extinction in the range [0,1].</p>"                                                );

   SensorGain_Parameter.Build(            w.m_instance.p_sensorGain,            "Sensor gain (e-/ADU):",
      "<p>Sensor gain (e-/ADU). The gain must be &ge; 0.</p>"                                           );

   QuantumEfficiency_Parameter.Build(     w.m_instance.p_quantumEfficiency,     "Quantum efficiency:",
      "<p>Sensor quantum efficiency in the range [0,1).</p>"                                             );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );

   Global_Sizer.Add( Wavelength_Parameter );
   Global_Sizer.Add( Transmissivity_Parameter );
   Global_Sizer.Add( FilterWidth_Parameter );
   Global_Sizer.Add( Aperture_Parameter );
   Global_Sizer.Add( CentralObstruction_Parameter );
   Global_Sizer.Add( ExposureTime_Parameter );
   Global_Sizer.Add( AtmosphericExtinction_Parameter );
   Global_Sizer.Add( SensorGain_Parameter );
   Global_Sizer.Add( QuantumEfficiency_Parameter );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FluxCalibrationInterface.cpp - Released 2025-04-07T08:53:55Z
