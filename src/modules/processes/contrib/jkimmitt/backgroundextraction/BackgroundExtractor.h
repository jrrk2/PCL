// ****************************************************************************
// PixInsight Class Library - PCL 02.08.05
// Standard BackgroundExtraction Process Module Version 1.0.0
// ****************************************************************************
// BackgroundExtractor.h - Released 2025-12-10
// ****************************************************************************

#ifndef __BackgroundExtractor_h
#define __BackgroundExtractor_h

#include <pcl/Array.h>
#include <pcl/Point.h>
#include <pcl/Image.h>
#include <pcl/Matrix.h>

namespace pcl
{

// Forward declarations
class BackgroundExtractionInstance;

// ============================================================================

struct BackgroundSample
{
   DPoint position;  // Position in image (x, y)
   double value;     // Measured background value at this position
   double weight;    // Sample weight (for outlier rejection)
   
   BackgroundSample() = default;
   
   BackgroundSample( const DPoint& p, double v, double w = 1.0 )
      : position( p ), value( v ), weight( w )
   {
   }
   
   BackgroundSample( double x, double y, double v, double w = 1.0 )
      : position( x, y ), value( v ), weight( w )
   {
   }
};

typedef Array<BackgroundSample> SampleArray;

// ============================================================================

class BackgroundExtractor
{
public:

   BackgroundExtractor( const DImage& image, const BackgroundExtractionInstance& instance );
   
   virtual ~BackgroundExtractor();
   
   // Generate samples automatically
   void GenerateSamples();
   
   // Add a manual sample
   void AddSample( const BackgroundSample& sample );
   
   // Clear all samples
   void ClearSamples();
   
   // Fit the background model to the samples
   void FitBackground();
   
   // Get the fitted background
   const DImage& Background() const
   {
      return m_background;
   }
   
   // Get current samples
   const SampleArray& Samples() const
   {
      return m_samples;
   }
   
   // Get sample count
   size_t SampleCount() const
   {
      return m_samples.Length();
   }

private:

   const DImage&                      m_image;
   const BackgroundExtractionInstance& m_instance;
   DImage                             m_background;
   SampleArray                        m_samples;
   
   // Internal methods
   void GenerateAutomaticSamples();
   void GenerateGridSamples();
   void GenerateGradientSamples();
   
   void RejectOutliers();
   bool IsSampleValid( int x, int y, int size ) const;
   double MeasureSample( int x, int y, int size ) const;
   
   void FitLinear();
   void FitPolynomial( int degree );
   void FitRBF();
   void FitGradientDomain();
   
   // Evaluate fitted model at a point
   double EvaluateModel( double x, double y ) const;
   
   // Model coefficients (for parametric models)
   Vector m_coefficients;
};

// ============================================================================

} // pcl

#endif   // __BackgroundExtractor_h

// ****************************************************************************
// EOF BackgroundExtractor.h - Released 2025-12-10
// ****************************************************************************
