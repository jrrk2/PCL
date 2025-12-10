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

   // Process parameters
   pcl_enum   p_model;                    // Background model type
   pcl_enum   p_sampleGeneration;         // Sample generation method
   double     p_tolerance;                // Outlier detection tolerance (sigma)
   double     p_deviation;                // Sample deviation threshold
   int32      p_minSamples;               // Minimum number of samples
   int32      p_maxSamples;               // Maximum number of samples
   pcl_bool   p_rejectionEnabled;         // Enable outlier rejection
   double     p_rejectionLow;             // Low rejection threshold
   double     p_rejectionHigh;            // High rejection threshold
   int32      p_rejectionIterations;      // Number of rejection iterations
   int32      p_gridRows;                 // Grid rows for grid sampling
   int32      p_gridColumns;              // Grid columns for grid sampling
   pcl_bool   p_replaceTarget;            // Replace target image with corrected
   pcl_bool   p_outputBackgroundModel;    // Output background model as new image
   
   // Manual sample positions (x, y pairs)
   struct Sample
   {
      int32 x, y;
      
      Sample( int32 x_ = 0, int32 y_ = 0 ) : x( x_ ), y( y_ ) {}
      
      bool operator ==( const Sample& s ) const
      {
         return x == s.x && y == s.y;
      }
   };
   
   Array<Sample> p_manualSamples;

   // Internal execution methods
   bool ExecuteOnImage( ImageVariant& image );
   
   template <class P>
   bool ExecuteT( GenericImage<P>& image );
   
   // Sample generation
   template <class P>
   void GenerateAutomaticSamples( const GenericImage<P>& image, Array<Point>& samples );
   
   template <class P>
   void GenerateGridSamples( const GenericImage<P>& image, Array<Point>& samples );
   
   template <class P>
   void GenerateGradientBasedSamples( const GenericImage<P>& image, Array<Point>& samples );
   
   // Sample validation and rejection
   template <class P>
   void RejectOutliers( const GenericImage<P>& image, Array<Point>& samples );
   
   template <class P>
   double ComputeSampleStatistics( const GenericImage<P>& image, 
                                    const Array<Point>& samples,
                                    double& mean, double& stdDev );
   
   // Background model fitting
   template <class P>
   bool FitBackgroundModel( const GenericImage<P>& image,
                           const Array<Point>& samples,
                           GenericImage<P>& backgroundModel );
   
   template <class P>
   bool FitLinearModel( const GenericImage<P>& image,
                       const Array<Point>& samples,
                       GenericImage<P>& backgroundModel );
   
   template <class P>
   bool FitPolynomialModel( const GenericImage<P>& image,
                           const Array<Point>& samples,
                           GenericImage<P>& backgroundModel,
                           int degree );
   
   template <class P>
   bool FitRBFModel( const GenericImage<P>& image,
                    const Array<Point>& samples,
                    GenericImage<P>& backgroundModel );
   
   template <class P>
   bool FitGradientDomainModel( const GenericImage<P>& image,
                               const Array<Point>& samples,
                               GenericImage<P>& backgroundModel );
   
   // Gradient domain helpers
   template <class P>
   void ComputeGradientField( const GenericImage<P>& image,
                             GenericImage<P>& dx,
                             GenericImage<P>& dy );
   
   template <class P>
   void SolveGradientDomain( const GenericImage<P>& dx,
                            const GenericImage<P>& dy,
                            const Array<Point>& samples,
                            const GenericImage<P>& sourceImage,
                            GenericImage<P>& result );
   
   // Utility methods
   template <class P>
   typename P::sample GetInterpolatedValue( const GenericImage<P>& image,
                                           double x, double y, int channel );
   
   void ValidateParameters();
   
   friend class BackgroundExtractionProcess;
   friend class BackgroundExtractionInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif   // __BackgroundExtractionInstance_h

// ----------------------------------------------------------------------------
