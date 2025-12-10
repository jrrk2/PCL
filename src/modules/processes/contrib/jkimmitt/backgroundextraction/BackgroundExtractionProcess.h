// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionProcess.h - Process definition
// ****************************************************************************

#ifndef __BackgroundExtractionProcess_h
#define __BackgroundExtractionProcess_h

#include <pcl/MetaProcess.h>
#include <pcl/MetaParameter.h>
#include <pcl/ImageVariant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BackgroundExtractionProcess : public MetaProcess
{
public:

   BackgroundExtractionProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
};

// ----------------------------------------------------------------------------

extern BackgroundExtractionProcess* TheBackgroundExtractionProcess;

// ----------------------------------------------------------------------------
// Process Parameters
// ----------------------------------------------------------------------------

class BEModel : public MetaEnumeration
{
public:
   enum { Linear,
          Polynomial2,
          Polynomial3,
          RBF,
          GradientDomain,
          NumberOfModels,
          Default = Polynomial2 };

   BEModel( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern BEModel* TheBEModelParameter;

// ----------------------------------------------------------------------------

class BESampleGeneration : public MetaEnumeration
{
public:
   enum { Automatic,
          Manual,
          Grid,
          GradientAnalysis,
          NumberOfMethods,
          Default = Automatic };

   BESampleGeneration( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern BESampleGeneration* TheBESampleGenerationParameter;

// ----------------------------------------------------------------------------

class BETolerance : public MetaDouble
{
public:
   BETolerance( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BETolerance* TheBEToleranceParameter;

// ----------------------------------------------------------------------------

class BEDeviation : public MetaDouble
{
public:
   BEDeviation( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BEDeviation* TheBEDeviationParameter;

// ----------------------------------------------------------------------------

class BEMinSamples : public MetaInt32
{
public:
   BEMinSamples( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BEMinSamples* TheBEMinSamplesParameter;

// ----------------------------------------------------------------------------

class BEMaxSamples : public MetaInt32
{
public:
   BEMaxSamples( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BEMaxSamples* TheBEMaxSamplesParameter;

// ----------------------------------------------------------------------------

class BERejectionEnabled : public MetaBoolean
{
public:
   BERejectionEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BERejectionEnabled* TheBERejectionEnabledParameter;

// ----------------------------------------------------------------------------

class BERejectionLow : public MetaDouble
{
public:
   BERejectionLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BERejectionLow* TheBERejectionLowParameter;

// ----------------------------------------------------------------------------

class BERejectionHigh : public MetaDouble
{
public:
   BERejectionHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BERejectionHigh* TheBERejectionHighParameter;

// ----------------------------------------------------------------------------

class BERejectionIterations : public MetaInt32
{
public:
   BERejectionIterations( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BERejectionIterations* TheBERejectionIterationsParameter;

// ----------------------------------------------------------------------------

class BEGridRows : public MetaInt32
{
public:
   BEGridRows( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BEGridRows* TheBEGridRowsParameter;

// ----------------------------------------------------------------------------

class BEGridColumns : public MetaInt32
{
public:
   BEGridColumns( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern BEGridColumns* TheBEGridColumnsParameter;

// ----------------------------------------------------------------------------

class BEReplaceTarget : public MetaBoolean
{
public:
   BEReplaceTarget( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BEReplaceTarget* TheBEReplaceTargetParameter;

// ----------------------------------------------------------------------------

class BEOutputBackgroundModel : public MetaBoolean
{
public:
   BEOutputBackgroundModel( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BEOutputBackgroundModel* TheBEOutputBackgroundModelParameter;

// ----------------------------------------------------------------------------

class BEManualSamples : public MetaTable
{
public:
   BEManualSamples( MetaProcess* );

   IsoString Id() const override;
};

extern BEManualSamples* TheBEManualSamplesParameter;

// ----------------------------------------------------------------------------

class BEManualSampleX : public MetaInt32
{
public:
   BEManualSampleX( MetaTable* );

   IsoString Id() const override;
};

extern BEManualSampleX* TheBEManualSampleXParameter;

// ----------------------------------------------------------------------------

class BEManualSampleY : public MetaInt32
{
public:
   BEManualSampleY( MetaTable* );

   IsoString Id() const override;
};

extern BEManualSampleY* TheBEManualSampleYParameter;

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern BackgroundExtractionProcess* TheBackgroundExtractionProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif   // __BackgroundExtractionProcess_h

// ----------------------------------------------------------------------------
