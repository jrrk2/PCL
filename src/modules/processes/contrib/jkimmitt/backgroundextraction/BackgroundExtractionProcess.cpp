// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionProcess.cpp - Process implementation
// ****************************************************************************

#include "BackgroundExtractionProcess.h"
#include "BackgroundExtractionInstance.h"
#include "BackgroundExtractionInterface.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BackgroundExtractionProcess* TheBackgroundExtractionProcess = nullptr;

// ----------------------------------------------------------------------------

BackgroundExtractionProcess::BackgroundExtractionProcess()
{
   TheBackgroundExtractionProcess = this;

   // Instantiate process parameters
   new BEModel( this );
   new BESampleGeneration( this );
   new BETolerance( this );
   new BEDeviation( this );
   new BEMinSamples( this );
   new BEMaxSamples( this );
   new BERejectionEnabled( this );
   new BERejectionLow( this );
   new BERejectionHigh( this );
   new BERejectionIterations( this );
   new BEGridRows( this );
   new BEGridColumns( this );
   new BEReplaceTarget( this );
   new BEOutputBackgroundModel( this );
   new BEManualSamples( this );
   new BEManualSampleX( TheBEManualSamplesParameter );
   new BEManualSampleY( TheBEManualSamplesParameter );
}

// ----------------------------------------------------------------------------

IsoString BackgroundExtractionProcess::Id() const
{
   return "BackgroundExtraction";
}

// ----------------------------------------------------------------------------

IsoString BackgroundExtractionProcess::Category() const
{
   return "BackgroundModeling";
}

// ----------------------------------------------------------------------------

uint32 BackgroundExtractionProcess::Version() const
{
   return 0x100; // Version 1.0.0
}

// ----------------------------------------------------------------------------

String BackgroundExtractionProcess::Description() const
{
   return "<html>"
          "<p>BackgroundExtraction models and subtracts the background from astronomical images "
          "using advanced gradient domain methods and traditional polynomial fitting.</p>"
          
          "<p>The process supports multiple background models including linear, polynomial, "
          "RBF interpolation, and gradient domain solutions. It provides automatic sample "
          "generation with outlier rejection or manual sample placement for precise control.</p>"
          
          "<p><b>Key Features:</b></p>"
          "<ul>"
          "<li>Multiple background models (Linear, Polynomial, RBF, Gradient Domain)</li>"
          "<li>Automatic sample generation with intelligent outlier rejection</li>"
          "<li>Manual sample placement for precise control</li>"
          "<li>Per-channel or combined processing</li>"
          "<li>Quality metrics and validation</li>"
          "</ul>"
          
          "<p>Copyright © 2025 Your Name</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String BackgroundExtractionProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/BackgroundExtraction.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* BackgroundExtractionProcess::DefaultInterface() const
{
   return TheBackgroundExtractionInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* BackgroundExtractionProcess::Create() const
{
   return new BackgroundExtractionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* BackgroundExtractionProcess::Clone( const ProcessImplementation& p ) const
{
   const BackgroundExtractionInstance* instance = dynamic_cast<const BackgroundExtractionInstance*>( &p );
   return (instance != nullptr) ? new BackgroundExtractionInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------
// Process Parameters
// ----------------------------------------------------------------------------

BEModel* TheBEModelParameter = nullptr;

BEModel::BEModel( MetaProcess* P ) : MetaEnumeration( P )
{
   TheBEModelParameter = this;
}

IsoString BEModel::Id() const
{
   return "model";
}

size_type BEModel::NumberOfElements() const
{
   return NumberOfModels;
}

IsoString BEModel::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Linear:          return "Linear";
   case Polynomial2:     return "Polynomial2";
   case Polynomial3:     return "Polynomial3";
   case RBF:             return "RBF";
   case GradientDomain:  return "GradientDomain";
   }
}

int BEModel::ElementValue( size_type i ) const
{
   return int( i );
}

size_type BEModel::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

BESampleGeneration* TheBESampleGenerationParameter = nullptr;

BESampleGeneration::BESampleGeneration( MetaProcess* P ) : MetaEnumeration( P )
{
   TheBESampleGenerationParameter = this;
}

IsoString BESampleGeneration::Id() const
{
   return "sampleGeneration";
}

size_type BESampleGeneration::NumberOfElements() const
{
   return NumberOfMethods;
}

IsoString BESampleGeneration::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Automatic:         return "Automatic";
   case Manual:            return "Manual";
   case Grid:              return "Grid";
   case GradientAnalysis:  return "GradientAnalysis";
   }
}

int BESampleGeneration::ElementValue( size_type i ) const
{
   return int( i );
}

size_type BESampleGeneration::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

BETolerance* TheBEToleranceParameter = nullptr;

BETolerance::BETolerance( MetaProcess* P ) : MetaDouble( P )
{
   TheBEToleranceParameter = this;
}

IsoString BETolerance::Id() const
{
   return "tolerance";
}

int BETolerance::Precision() const
{
   return 2;
}

double BETolerance::DefaultValue() const
{
   return 1.0;
}

double BETolerance::MinimumValue() const
{
   return 0.1;
}

double BETolerance::MaximumValue() const
{
   return 10.0;
}

// ----------------------------------------------------------------------------

BEDeviation* TheBEDeviationParameter = nullptr;

BEDeviation::BEDeviation( MetaProcess* P ) : MetaDouble( P )
{
   TheBEDeviationParameter = this;
}

IsoString BEDeviation::Id() const
{
   return "deviation";
}

int BEDeviation::Precision() const
{
   return 2;
}

double BEDeviation::DefaultValue() const
{
   return 2.0;
}

double BEDeviation::MinimumValue() const
{
   return 0.1;
}

double BEDeviation::MaximumValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

BEMinSamples* TheBEMinSamplesParameter = nullptr;

BEMinSamples::BEMinSamples( MetaProcess* P ) : MetaInt32( P )
{
   TheBEMinSamplesParameter = this;
}

IsoString BEMinSamples::Id() const
{
   return "minSamples";
}

double BEMinSamples::DefaultValue() const
{
   return 100;
}

double BEMinSamples::MinimumValue() const
{
   return 10;
}

double BEMinSamples::MaximumValue() const
{
   return 10000;
}

// ----------------------------------------------------------------------------

BEMaxSamples* TheBEMaxSamplesParameter = nullptr;

BEMaxSamples::BEMaxSamples( MetaProcess* P ) : MetaInt32( P )
{
   TheBEMaxSamplesParameter = this;
}

IsoString BEMaxSamples::Id() const
{
   return "maxSamples";
}

double BEMaxSamples::DefaultValue() const
{
   return 1000;
}

double BEMaxSamples::MinimumValue() const
{
   return 10;
}

double BEMaxSamples::MaximumValue() const
{
   return 10000;
}

// ----------------------------------------------------------------------------

BERejectionEnabled* TheBERejectionEnabledParameter = nullptr;

BERejectionEnabled::BERejectionEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheBERejectionEnabledParameter = this;
}

IsoString BERejectionEnabled::Id() const
{
   return "rejectionEnabled";
}

bool BERejectionEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

BERejectionLow* TheBERejectionLowParameter = nullptr;

BERejectionLow::BERejectionLow( MetaProcess* P ) : MetaDouble( P )
{
   TheBERejectionLowParameter = this;
}

IsoString BERejectionLow::Id() const
{
   return "rejectionLow";
}

int BERejectionLow::Precision() const
{
   return 2;
}

double BERejectionLow::DefaultValue() const
{
   return 3.0;
}

double BERejectionLow::MinimumValue() const
{
   return 0.5;
}

double BERejectionLow::MaximumValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

BERejectionHigh* TheBERejectionHighParameter = nullptr;

BERejectionHigh::BERejectionHigh( MetaProcess* P ) : MetaDouble( P )
{
   TheBERejectionHighParameter = this;
}

IsoString BERejectionHigh::Id() const
{
   return "rejectionHigh";
}

int BERejectionHigh::Precision() const
{
   return 2;
}

double BERejectionHigh::DefaultValue() const
{
   return 2.0;
}

double BERejectionHigh::MinimumValue() const
{
   return 0.5;
}

double BERejectionHigh::MaximumValue() const
{
   return 5.0;
}

// ----------------------------------------------------------------------------

BERejectionIterations* TheBERejectionIterationsParameter = nullptr;

BERejectionIterations::BERejectionIterations( MetaProcess* P ) : MetaInt32( P )
{
   TheBERejectionIterationsParameter = this;
}

IsoString BERejectionIterations::Id() const
{
   return "rejectionIterations";
}

double BERejectionIterations::DefaultValue() const
{
   return 3;
}

double BERejectionIterations::MinimumValue() const
{
   return 1;
}

double BERejectionIterations::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

BEGridRows* TheBEGridRowsParameter = nullptr;

BEGridRows::BEGridRows( MetaProcess* P ) : MetaInt32( P )
{
   TheBEGridRowsParameter = this;
}

IsoString BEGridRows::Id() const
{
   return "gridRows";
}

double BEGridRows::DefaultValue() const
{
   return 8;
}

double BEGridRows::MinimumValue() const
{
   return 2;
}

double BEGridRows::MaximumValue() const
{
   return 32;
}

// ----------------------------------------------------------------------------

BEGridColumns* TheBEGridColumnsParameter = nullptr;

BEGridColumns::BEGridColumns( MetaProcess* P ) : MetaInt32( P )
{
   TheBEGridColumnsParameter = this;
}

IsoString BEGridColumns::Id() const
{
   return "gridColumns";
}

double BEGridColumns::DefaultValue() const
{
   return 8;
}

double BEGridColumns::MinimumValue() const
{
   return 2;
}

double BEGridColumns::MaximumValue() const
{
   return 32;
}

// ----------------------------------------------------------------------------

BEReplaceTarget* TheBEReplaceTargetParameter = nullptr;

BEReplaceTarget::BEReplaceTarget( MetaProcess* P ) : MetaBoolean( P )
{
   TheBEReplaceTargetParameter = this;
}

IsoString BEReplaceTarget::Id() const
{
   return "replaceTarget";
}

bool BEReplaceTarget::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

BEOutputBackgroundModel* TheBEOutputBackgroundModelParameter = nullptr;

BEOutputBackgroundModel::BEOutputBackgroundModel( MetaProcess* P ) : MetaBoolean( P )
{
   TheBEOutputBackgroundModelParameter = this;
}

IsoString BEOutputBackgroundModel::Id() const
{
   return "outputBackgroundModel";
}

bool BEOutputBackgroundModel::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

BEManualSamples* TheBEManualSamplesParameter = nullptr;

BEManualSamples::BEManualSamples( MetaProcess* P ) : MetaTable( P )
{
   TheBEManualSamplesParameter = this;
}

IsoString BEManualSamples::Id() const
{
   return "manualSamples";
}

// ----------------------------------------------------------------------------

BEManualSampleX* TheBEManualSampleXParameter = nullptr;

BEManualSampleX::BEManualSampleX( MetaTable* T ) : MetaInt32( T )
{
   TheBEManualSampleXParameter = this;
}

IsoString BEManualSampleX::Id() const
{
   return "x";
}

// ----------------------------------------------------------------------------

BEManualSampleY* TheBEManualSampleYParameter = nullptr;

BEManualSampleY::BEManualSampleY( MetaTable* T ) : MetaInt32( T )
{
   TheBEManualSampleYParameter = this;
}

IsoString BEManualSampleY::Id() const
{
   return "y";
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
