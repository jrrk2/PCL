// ****************************************************************************
// PixInsight Class Library - PCL 02.08.05
// Standard BackgroundExtraction Process Module Version 1.0.0
// ****************************************************************************
// BackgroundExtractionParameters.cpp - Released 2025-12-10
// ****************************************************************************

#include "BackgroundExtractionParameters.h"

namespace pcl
{

// ============================================================================

BGModelType*                        TheBGModelTypeParameter = nullptr;
BGSampleGenerationMode*             TheBGSampleGenerationModeParameter = nullptr;
BGSampleSize*                       TheBGSampleSizeParameter = nullptr;
BGSampleTolerance*                  TheBGSampleToleranceParameter = nullptr;
BGMinSamples*                       TheBGMinSamplesParameter = nullptr;
BGMaxSamples*                       TheBGMaxSamplesParameter = nullptr;
BGGridSpacingX*                     TheBGGridSpacingXParameter = nullptr;
BGGridSpacingY*                     TheBGGridSpacingYParameter = nullptr;
BGEnableOutlierRejection*           TheBGEnableOutlierRejectionParameter = nullptr;
BGOutlierRejectionThreshold*        TheBGOutlierRejectionThresholdParameter = nullptr;
BGOutlierRejectionIterations*       TheBGOutlierRejectionIterationsParameter = nullptr;
BGRBFSmoothness*                    TheBGRBFSmoothnessParameter = nullptr;
BGReplaceTarget*                    TheBGReplaceTargetParameter = nullptr;
BGOutputBackgroundModel*            TheBGOutputBackgroundModelParameter = nullptr;
BGApplySTFToBackground*             TheBGApplySTFToBackgroundParameter = nullptr;

// ============================================================================

BGModelType::BGModelType( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheBGModelTypeParameter = this;
}

IsoString BGModelType::Id() const
{
   return "modelType";
}

size_type BGModelType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString BGModelType::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Linear:           return "Linear";
   case Polynomial2:      return "Polynomial2";
   case Polynomial3:      return "Polynomial3";
   case RBF:              return "RBF";
   case GradientDomain:   return "GradientDomain";
   default:
   case NumberOfItems:    return "Unknown";
   }
}

int BGModelType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type BGModelType::DefaultValueIndex() const
{
   return size_type( Default );
}

// ============================================================================

BGSampleGenerationMode::BGSampleGenerationMode( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheBGSampleGenerationModeParameter = this;
}

IsoString BGSampleGenerationMode::Id() const
{
   return "sampleGenerationMode";
}

size_type BGSampleGenerationMode::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString BGSampleGenerationMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   case Automatic:         return "Automatic";
   case Manual:            return "Manual";
   case Grid:              return "Grid";
   case GradientAnalysis:  return "GradientAnalysis";
   default:
   case NumberOfItems:     return "Unknown";
   }
}

int BGSampleGenerationMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type BGSampleGenerationMode::DefaultValueIndex() const
{
   return size_type( Default );
}

// ============================================================================

BGSampleSize::BGSampleSize( MetaProcess* P )
   : MetaDouble( P )
{
   TheBGSampleSizeParameter = this;
}

IsoString BGSampleSize::Id() const
{
   return "sampleSize";
}

int BGSampleSize::Precision() const
{
   return 2;
}

double BGSampleSize::DefaultValue() const
{
   return 0.05; // 5% of image dimension
}

double BGSampleSize::MinimumValue() const
{
   return 0.01; // 1%
}

double BGSampleSize::MaximumValue() const
{
   return 0.20; // 20%
}

// ============================================================================

BGSampleTolerance::BGSampleTolerance( MetaProcess* P )
   : MetaDouble( P )
{
   TheBGSampleToleranceParameter = this;
}

IsoString BGSampleTolerance::Id() const
{
   return "sampleTolerance";
}

int BGSampleTolerance::Precision() const
{
   return 2;
}

double BGSampleTolerance::DefaultValue() const
{
   return 2.5; // 2.5 sigma outlier rejection
}

double BGSampleTolerance::MinimumValue() const
{
   return 1.0;
}

double BGSampleTolerance::MaximumValue() const
{
   return 5.0;
}

// ============================================================================

BGMinSamples::BGMinSamples( MetaProcess* P )
   : MetaInt32( P )
{
   TheBGMinSamplesParameter = this;
}

IsoString BGMinSamples::Id() const
{
   return "minSamples";
}

double BGMinSamples::DefaultValue() const
{
   return 10;
}

double BGMinSamples::MinimumValue() const
{
   return 3;
}

double BGMinSamples::MaximumValue() const
{
   return 100;
}

// ============================================================================

BGMaxSamples::BGMaxSamples( MetaProcess* P )
   : MetaInt32( P )
{
   TheBGMaxSamplesParameter = this;
}

IsoString BGMaxSamples::Id() const
{
   return "maxSamples";
}

double BGMaxSamples::DefaultValue() const
{
   return 100;
}

double BGMaxSamples::MinimumValue() const
{
   return 10;
}

double BGMaxSamples::MaximumValue() const
{
   return 1000;
}

// ============================================================================

BGGridSpacingX::BGGridSpacingX( MetaProcess* P )
   : MetaInt32( P )
{
   TheBGGridSpacingXParameter = this;
}

IsoString BGGridSpacingX::Id() const
{
   return "gridSpacingX";
}

double BGGridSpacingX::DefaultValue() const
{
   return 200;
}

double BGGridSpacingX::MinimumValue() const
{
   return 50;
}

double BGGridSpacingX::MaximumValue() const
{
   return 1000;
}

// ============================================================================

BGGridSpacingY::BGGridSpacingY( MetaProcess* P )
   : MetaInt32( P )
{
   TheBGGridSpacingYParameter = this;
}

IsoString BGGridSpacingY::Id() const
{
   return "gridSpacingY";
}

double BGGridSpacingY::DefaultValue() const
{
   return 200;
}

double BGGridSpacingY::MinimumValue() const
{
   return 50;
}

double BGGridSpacingY::MaximumValue() const
{
   return 1000;
}

// ============================================================================

BGEnableOutlierRejection::BGEnableOutlierRejection( MetaProcess* P )
   : MetaBoolean( P )
{
   TheBGEnableOutlierRejectionParameter = this;
}

IsoString BGEnableOutlierRejection::Id() const
{
   return "enableOutlierRejection";
}

bool BGEnableOutlierRejection::DefaultValue() const
{
   return true;
}

// ============================================================================

BGOutlierRejectionThreshold::BGOutlierRejectionThreshold( MetaProcess* P )
   : MetaDouble( P )
{
   TheBGOutlierRejectionThresholdParameter = this;
}

IsoString BGOutlierRejectionThreshold::Id() const
{
   return "outlierRejectionThreshold";
}

int BGOutlierRejectionThreshold::Precision() const
{
   return 2;
}

double BGOutlierRejectionThreshold::DefaultValue() const
{
   return 3.0; // 3 sigma
}

double BGOutlierRejectionThreshold::MinimumValue() const
{
   return 1.0;
}

double BGOutlierRejectionThreshold::MaximumValue() const
{
   return 5.0;
}

// ============================================================================

BGOutlierRejectionIterations::BGOutlierRejectionIterations( MetaProcess* P )
   : MetaInt32( P )
{
   TheBGOutlierRejectionIterationsParameter = this;
}

IsoString BGOutlierRejectionIterations::Id() const
{
   return "outlierRejectionIterations";
}

double BGOutlierRejectionIterations::DefaultValue() const
{
   return 3;
}

double BGOutlierRejectionIterations::MinimumValue() const
{
   return 1;
}

double BGOutlierRejectionIterations::MaximumValue() const
{
   return 10;
}

// ============================================================================

BGRBFSmoothness::BGRBFSmoothness( MetaProcess* P )
   : MetaInt32( P )
{
   TheBGRBFSmoothnessParameter = this;
}

IsoString BGRBFSmoothness::Id() const
{
   return "rbfSmoothness";
}

double BGRBFSmoothness::DefaultValue() const
{
   return 50; // Medium smoothness
}

double BGRBFSmoothness::MinimumValue() const
{
   return 1;
}

double BGRBFSmoothness::MaximumValue() const
{
   return 100;
}

// ============================================================================

BGReplaceTarget::BGReplaceTarget( MetaProcess* P )
   : MetaBoolean( P )
{
   TheBGReplaceTargetParameter = this;
}

IsoString BGReplaceTarget::Id() const
{
   return "replaceTarget";
}

bool BGReplaceTarget::DefaultValue() const
{
   return true;
}

// ============================================================================

BGOutputBackgroundModel::BGOutputBackgroundModel( MetaProcess* P )
   : MetaBoolean( P )
{
   TheBGOutputBackgroundModelParameter = this;
}

IsoString BGOutputBackgroundModel::Id() const
{
   return "outputBackgroundModel";
}

bool BGOutputBackgroundModel::DefaultValue() const
{
   return false;
}

// ============================================================================

BGApplySTFToBackground::BGApplySTFToBackground( MetaProcess* P )
   : MetaBoolean( P )
{
   TheBGApplySTFToBackgroundParameter = this;
}

IsoString BGApplySTFToBackground::Id() const
{
   return "applySTFToBackground";
}

bool BGApplySTFToBackground::DefaultValue() const
{
   return true; // Auto-stretch ON by default to make gradient visible
}

// ============================================================================

} // pcl

// ****************************************************************************
// EOF BackgroundExtractionParameters.cpp - Released 2025-12-10
// ****************************************************************************
