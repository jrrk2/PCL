// ****************************************************************************
// PixInsight Class Library - PCL 02.08.05
// Standard BackgroundExtraction Process Module Version 1.0.0
// ****************************************************************************
// BackgroundExtractor.cpp - Released 2025-12-10
// ****************************************************************************

#include "BackgroundExtractor.h"
#include "BackgroundExtractionInstance.h"
#include "BackgroundExtractionParameters.h"

#include <pcl/LinearFit.h>
#include <pcl/Math.h>
#include <pcl/Random.h>

namespace pcl
{

// ============================================================================

BackgroundExtractor::BackgroundExtractor( const DImage& image, const BackgroundExtractionInstance& instance )
   : m_image( image )
   , m_instance( instance )
   , m_background( image.Width(), image.Height() )
{
}

// ============================================================================

BackgroundExtractor::~BackgroundExtractor()
{
}

// ============================================================================

void BackgroundExtractor::GenerateSamples()
{
   ClearSamples();
   
   switch ( m_instance.p_sampleGenerationMode )
   {
   case BGSampleGenerationMode::Automatic:
      GenerateAutomaticSamples();
      break;
   case BGSampleGenerationMode::Grid:
      GenerateGridSamples();
      break;
   case BGSampleGenerationMode::GradientAnalysis:
      GenerateGradientSamples();
      break;
   case BGSampleGenerationMode::Manual:
      // Manual samples are added via AddSample()
      break;
   }
   
   // Apply outlier rejection if enabled
   if ( m_instance.p_enableOutlierRejection && m_samples.Length() > 3 )
   {
      RejectOutliers();
   }
}

// ============================================================================

void BackgroundExtractor::GenerateAutomaticSamples()
{
   int width = m_image.Width();
   int height = m_image.Height();
   int sampleSize = RoundInt( m_instance.p_sampleSize * Min( width, height ) );
   
   // Make sure sample size is odd
   if ( sampleSize % 2 == 0 )
      ++sampleSize;
   
   int halfSize = sampleSize / 2;
   
   // Random number generator
   RandomNumberGenerator rng;
   
   int targetSamples = m_instance.p_maxSamples;
   int attempts = 0;
   int maxAttempts = targetSamples * 10;
   
   while ( m_samples.Length() < size_t( targetSamples ) && attempts < maxAttempts )
   {
      // Generate random position
      int x = halfSize + (rng.Rand32() % (width - 2*halfSize));
      int y = halfSize + (rng.Rand32() % (height - 2*halfSize));
      
      // Check if this is a valid sample location
      if ( IsSampleValid( x, y, sampleSize ) )
      {
         double value = MeasureSample( x, y, sampleSize );
         m_samples.Add( BackgroundSample( x, y, value ) );
      }
      
      ++attempts;
   }
   
   // Make sure we have minimum samples
   if ( m_samples.Length() < size_t( m_instance.p_minSamples ) )
   {
      throw Error( String().Format( "Could not generate minimum number of samples (%d generated, %d required)",
                                   m_samples.Length(), m_instance.p_minSamples ) );
   }
}

// ============================================================================

void BackgroundExtractor::GenerateGridSamples()
{
   int width = m_image.Width();
   int height = m_image.Height();
   int sampleSize = RoundInt( m_instance.p_sampleSize * Min( width, height ) );
   
   if ( sampleSize % 2 == 0 )
      ++sampleSize;
   
   int halfSize = sampleSize / 2;
   
   int spacingX = m_instance.p_gridSpacingX;
   int spacingY = m_instance.p_gridSpacingY;
   
   for ( int y = halfSize; y < height - halfSize; y += spacingY )
   {
      for ( int x = halfSize; x < width - halfSize; x += spacingX )
      {
         if ( IsSampleValid( x, y, sampleSize ) )
         {
            double value = MeasureSample( x, y, sampleSize );
            m_samples.Add( BackgroundSample( x, y, value ) );
         }
      }
   }
}

// ============================================================================

void BackgroundExtractor::GenerateGradientSamples()
{
   // For now, just use automatic sampling
   // TODO: Implement gradient analysis to place samples in low-gradient areas
   GenerateAutomaticSamples();
}

// ============================================================================

bool BackgroundExtractor::IsSampleValid( int x, int y, int size ) const
{
   int halfSize = size / 2;
   
   // Check bounds
   if ( x - halfSize < 0 || x + halfSize >= m_image.Width() ||
        y - halfSize < 0 || y + halfSize >= m_image.Height() )
      return false;
   
   // Sample area and check for stars (high values or high variance)
   double sum = 0;
   double sum2 = 0;
   int count = 0;
   
   for ( int dy = -halfSize; dy <= halfSize; ++dy )
   {
      for ( int dx = -halfSize; dx <= halfSize; ++dx )
      {
         double v = m_image( x + dx, y + dy );
         sum += v;
         sum2 += v * v;
         ++count;
      }
   }
   
   double mean = sum / count;
   double variance = (sum2 / count) - (mean * mean);
   double stddev = Sqrt( variance );
   
   // Reject if too bright (likely contains stars)
   if ( mean > 0.1 )
      return false;
   
   // Reject if too much variance (not uniform background)
   if ( stddev > m_instance.p_sampleTolerance * mean )
      return false;
   
   return true;
}

// ============================================================================

double BackgroundExtractor::MeasureSample( int x, int y, int size ) const
{
   int halfSize = size / 2;
   
   // Use median instead of mean for robustness
   Array<double> values;
   
   for ( int dy = -halfSize; dy <= halfSize; ++dy )
   {
      for ( int dx = -halfSize; dx <= halfSize; ++dx )
      {
         values.Add( m_image( x + dx, y + dy ) );
      }
   }
   
   values.Sort();
   return values[values.Length() / 2]; // Median
}

// ============================================================================

void BackgroundExtractor::RejectOutliers()
{
   if ( m_samples.Length() < 4 )
      return;
   
   int iterations = m_instance.p_outlierRejectionIterations;
   double threshold = m_instance.p_outlierRejectionThreshold;
   
   for ( int iter = 0; iter < iterations; ++iter )
   {
      // Calculate median and MAD of sample values
      Array<double> values;
      for ( const auto& sample : m_samples )
         if ( sample.weight > 0 )
            values.Add( sample.value );
      
      if ( values.Length() < 4 )
         break;
      
      values.Sort();
      double median = values[values.Length() / 2];
      
      // Calculate MAD
      Array<double> deviations;
      for ( double v : values )
         deviations.Add( Abs( v - median ) );
      deviations.Sort();
      double mad = deviations[deviations.Length() / 2];
      
      if ( mad < 1e-10 )
         break; // All samples are identical
      
      // Mark outliers
      int rejectedCount = 0;
      for ( auto& sample : m_samples )
      {
         if ( sample.weight > 0 )
         {
            double deviation = Abs( sample.value - median ) / mad;
            if ( deviation > threshold )
            {
               sample.weight = 0; // Reject this sample
               ++rejectedCount;
            }
         }
      }
      
      if ( rejectedCount == 0 )
         break; // No more outliers
   }
   
   // Remove rejected samples
   SampleArray validSamples;
   for ( const auto& sample : m_samples )
      if ( sample.weight > 0 )
         validSamples.Add( sample );
   
   m_samples = validSamples;
}

// ============================================================================

void BackgroundExtractor::AddSample( const BackgroundSample& sample )
{
   m_samples.Add( sample );
}

// ============================================================================

void BackgroundExtractor::ClearSamples()
{
   m_samples.Clear();
}

// ============================================================================

void BackgroundExtractor::FitBackground()
{
   if ( m_samples.IsEmpty() )
      throw Error( "No samples available for background fitting" );
   
   // Choose fitting method based on model type
   switch ( m_instance.p_modelType )
   {
   case BGModelType::Linear:
      FitLinear();
      break;
   case BGModelType::Polynomial2:
      FitPolynomial( 2 );
      break;
   case BGModelType::Polynomial3:
      FitPolynomial( 3 );
      break;
   case BGModelType::RBF:
      FitRBF();
      break;
   case BGModelType::GradientDomain:
      FitGradientDomain();
      break;
   }
   
   // Evaluate model at all pixels
   int width = m_background.Width();
   int height = m_background.Height();
   
   for ( int y = 0; y < height; ++y )
   {
      for ( int x = 0; x < width; ++x )
      {
         m_background( x, y ) = EvaluateModel( x, y );
      }
   }
}

// ============================================================================

void BackgroundExtractor::FitLinear()
{
   // Fit plane: z = a*x + b*y + c
   
   int n = m_samples.Length();
   m_coefficients = Vector( 3 );
   
   // Use simple least squares approach
   double sumX = 0, sumY = 0, sumZ = 0;
   double sumX2 = 0, sumY2 = 0, sumXY = 0;
   double sumXZ = 0, sumYZ = 0;
   
   for ( int i = 0; i < n; ++i )
   {
      double x = m_samples[i].position.x;
      double y = m_samples[i].position.y;
      double z = m_samples[i].value;
      sumX += x;
      sumY += y;
      sumZ += z;
      sumX2 += x*x;
      sumY2 += y*y;
      sumXY += x*y;
      sumXZ += x*z;
      sumYZ += y*z;
   }
   
   // Solve normal equations for plane fitting
   // n*a*x^2 + n*b*xy + n*c*x = n*xz
   // n*a*xy + n*b*y^2 + n*c*y = n*yz  
   // n*a*x + n*b*y + n*c = n*z
   
   double det = n*(sumX2*sumY2 - sumXY*sumXY) - sumX*(sumX*sumY2 - sumY*sumXY) + sumY*(sumX*sumXY - sumY*sumX2);
   
   if ( Abs(det) > 1e-10 )
   {
      double a = (n*(sumXZ*sumY2 - sumYZ*sumXY) - sumX*(sumXZ*sumY - sumYZ*sumX) + sumY*(sumXZ*sumXY - sumYZ*sumX2)) / det;
      double b = (n*(sumX2*sumYZ - sumXY*sumXZ) - sumX*(sumX*sumYZ - sumY*sumXZ) + sumY*(sumX*sumXY - sumY*sumX2)) / det;
      double c = (sumZ - a*sumX - b*sumY) / n;
      
      m_coefficients[0] = a;
      m_coefficients[1] = b;
      m_coefficients[2] = c;
   }
   else
   {
      // Degenerate case - use mean
      m_coefficients[0] = 0;
      m_coefficients[1] = 0;
      m_coefficients[2] = sumZ / n;
   }
}

// ============================================================================

void BackgroundExtractor::FitPolynomial( int degree )
{
   // For simplicity, use linear fit for now
   // TODO: Implement full polynomial fitting
   FitLinear();
}

// ============================================================================

void BackgroundExtractor::FitRBF()
{
   // For simplicity, use linear fit for now
   // TODO: Implement RBF interpolation
   FitLinear();
}

// ============================================================================

void BackgroundExtractor::FitGradientDomain()
{
   // For simplicity, use linear fit for now
   // TODO: Implement gradient domain solver
   FitLinear();
}

// ============================================================================

double BackgroundExtractor::EvaluateModel( double x, double y ) const
{
   // Evaluate linear model: z = a*x + b*y + c
   if ( m_coefficients.Length() >= 3 )
   {
      return m_coefficients[0] * x + m_coefficients[1] * y + m_coefficients[2];
   }
   
   return 0.0;
}

// ============================================================================

} // pcl

// ****************************************************************************
// EOF BackgroundExtractor.cpp - Released 2025-12-10
// ****************************************************************************
