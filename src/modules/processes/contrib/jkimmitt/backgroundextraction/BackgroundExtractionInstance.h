// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionInstance.h - Process instance definition
// ****************************************************************************

#ifndef __BackgroundExtractionInstance_h
#define __BackgroundExtractionInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h>
#include <pcl/Point.h>
#include <pcl/Vector.h>
#include <pcl/Matrix.h>

#include "BackgroundExtractionProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class BackgroundExtractionInstance : public ProcessImplementation
{
public:

   BackgroundExtractionInstance( const MetaProcess* );
   BackgroundExtractionInstance( const BackgroundExtractionInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

private:

   // Process parameters - MATCHING BackgroundExtractionParameters.h (BG* parameters)
   pcl_enum   p_modelType;                    // BGModelType
   pcl_enum   p_sampleGenerationMode;         // BGSampleGenerationMode  
   double     p_sampleSize;                   // Sample size as fraction
   double     p_sampleTolerance;              // Sample tolerance (sigma)
   int32      p_minSamples;                   // Minimum number of samples
   int32      p_maxSamples;                   // Maximum number of samples
   int32      p_gridSpacingX;                 // Grid spacing X
   int32      p_gridSpacingY;                 // Grid spacing Y
   pcl_bool   p_enableOutlierRejection;       // Enable outlier rejection
   double     p_outlierRejectionThreshold;    // Outlier threshold (sigma)
   int32      p_outlierRejectionIterations;   // Rejection iterations
   int32      p_rbfSmoothness;                // RBF smoothness (1-100)
   pcl_bool   p_replaceTarget;                // Replace target image
   pcl_bool   p_outputBackgroundModel;        // Output background model
   pcl_bool   p_applySTFToBackground;         // Apply auto-stretch to background
   
   friend class BackgroundExtractionProcess;
   friend class BackgroundExtractionInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif   // __BackgroundExtractionInstance_h

// ----------------------------------------------------------------------------
