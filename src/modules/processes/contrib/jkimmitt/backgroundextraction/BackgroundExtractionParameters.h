// ****************************************************************************
// PixInsight Class Library - PCL 02.08.05
// Standard BackgroundExtraction Process Module Version 1.0.0
// ****************************************************************************
// BackgroundExtractionParameters.h - Released 2025-12-10
// ****************************************************************************

#ifndef __BackgroundExtractionParameters_h
#define __BackgroundExtractionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ============================================================================

class BGModelType : public MetaEnumeration
{
public:
   enum { Linear,
          Polynomial2,
          Polynomial3,
          RBF,
          GradientDomain,
          NumberOfItems,
          Default = Linear };

   BGModelType( MetaProcess* );
   
   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern BGModelType* TheBGModelTypeParameter;

// ============================================================================

class BGSampleGenerationMode : public MetaEnumeration
{
public:
   enum { Automatic,
          Manual,
          Grid,
          GradientAnalysis,
          NumberOfItems,
          Default = Automatic };

   BGSampleGenerationMode( MetaProcess* );
   
   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern BGSampleGenerationMode* TheBGSampleGenerationModeParameter;

// ============================================================================

class BGSampleSize : public MetaDouble
{
public:
   BGSampleSize( MetaProcess* );
   
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGSampleSize* TheBGSampleSizeParameter;

// ============================================================================

class BGSampleTolerance : public MetaDouble
{
public:
   BGSampleTolerance( MetaProcess* );
   
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGSampleTolerance* TheBGSampleToleranceParameter;

// ============================================================================

class BGMinSamples : public MetaInt32
{
public:
   BGMinSamples( MetaProcess* );
   
   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGMinSamples* TheBGMinSamplesParameter;

// ============================================================================

class BGMaxSamples : public MetaInt32
{
public:
   BGMaxSamples( MetaProcess* );
   
   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGMaxSamples* TheBGMaxSamplesParameter;

// ============================================================================

class BGGridSpacingX : public MetaInt32
{
public:
   BGGridSpacingX( MetaProcess* );
   
   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGGridSpacingX* TheBGGridSpacingXParameter;

// ============================================================================

class BGGridSpacingY : public MetaInt32
{
public:
   BGGridSpacingY( MetaProcess* );
   
   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGGridSpacingY* TheBGGridSpacingYParameter;

// ============================================================================

class BGEnableOutlierRejection : public MetaBoolean
{
public:
   BGEnableOutlierRejection( MetaProcess* );
   
   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BGEnableOutlierRejection* TheBGEnableOutlierRejectionParameter;

// ============================================================================

class BGOutlierRejectionThreshold : public MetaDouble
{
public:
   BGOutlierRejectionThreshold( MetaProcess* );
   
   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGOutlierRejectionThreshold* TheBGOutlierRejectionThresholdParameter;

// ============================================================================

class BGOutlierRejectionIterations : public MetaInt32
{
public:
   BGOutlierRejectionIterations( MetaProcess* );
   
   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGOutlierRejectionIterations* TheBGOutlierRejectionIterationsParameter;

// ============================================================================

class BGRBFSmoothness : public MetaInt32
{
public:
   BGRBFSmoothness( MetaProcess* );
   
   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BGRBFSmoothness* TheBGRBFSmoothnessParameter;

// ============================================================================

class BGReplaceTarget : public MetaBoolean
{
public:
   BGReplaceTarget( MetaProcess* );
   
   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BGReplaceTarget* TheBGReplaceTargetParameter;

// ============================================================================

class BGOutputBackgroundModel : public MetaBoolean
{
public:
   BGOutputBackgroundModel( MetaProcess* );
   
   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BGOutputBackgroundModel* TheBGOutputBackgroundModelParameter;

// ============================================================================

class BGApplySTFToBackground : public MetaBoolean
{
public:
   BGApplySTFToBackground( MetaProcess* );
   
   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BGApplySTFToBackground* TheBGApplySTFToBackgroundParameter;

// ============================================================================

PCL_END_LOCAL

} // pcl

#endif   // __BackgroundExtractionParameters_h

// ****************************************************************************
// EOF BackgroundExtractionParameters.h - Released 2025-12-10
// ****************************************************************************
