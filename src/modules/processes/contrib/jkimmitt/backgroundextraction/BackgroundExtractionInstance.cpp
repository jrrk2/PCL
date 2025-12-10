// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionInstance.cpp - Process instance implementation
// ****************************************************************************

#include "BackgroundExtractionInstance.h"

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>
#include <pcl/ImageWindow.h>
#include <pcl/ColorSpace.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BackgroundExtractionInstance::BackgroundExtractionInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_model( TheBEModelParameter->Default )
   , p_sampleGeneration( TheBESampleGenerationParameter->Default )
   , p_tolerance( TheBEToleranceParameter->DefaultValue() )
   , p_deviation( TheBEDeviationParameter->DefaultValue() )
   , p_minSamples( TheBEMinSamplesParameter->DefaultValue() )
   , p_maxSamples( TheBEMaxSamplesParameter->DefaultValue() )
   , p_rejectionEnabled( TheBERejectionEnabledParameter->DefaultValue() )
   , p_rejectionLow( TheBERejectionLowParameter->DefaultValue() )
   , p_rejectionHigh( TheBERejectionHighParameter->DefaultValue() )
   , p_rejectionIterations( TheBERejectionIterationsParameter->DefaultValue() )
   , p_gridRows( TheBEGridRowsParameter->DefaultValue() )
   , p_gridColumns( TheBEGridColumnsParameter->DefaultValue() )
   , p_replaceTarget( TheBEReplaceTargetParameter->DefaultValue() )
   , p_outputBackgroundModel( TheBEOutputBackgroundModelParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

BackgroundExtractionInstance::BackgroundExtractionInstance( const BackgroundExtractionInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInstance::Assign( const ProcessImplementation& p )
{
   const BackgroundExtractionInstance* x = dynamic_cast<const BackgroundExtractionInstance*>( &p );
   if ( x != nullptr )
   {
      p_model = x->p_model;
      p_sampleGeneration = x->p_sampleGeneration;
      p_tolerance = x->p_tolerance;
      p_deviation = x->p_deviation;
      p_minSamples = x->p_minSamples;
      p_maxSamples = x->p_maxSamples;
      p_rejectionEnabled = x->p_rejectionEnabled;
      p_rejectionLow = x->p_rejectionLow;
      p_rejectionHigh = x->p_rejectionHigh;
      p_rejectionIterations = x->p_rejectionIterations;
      p_gridRows = x->p_gridRows;
      p_gridColumns = x->p_gridColumns;
      p_replaceTarget = x->p_replaceTarget;
      p_outputBackgroundModel = x->p_outputBackgroundModel;
      p_manualSamples = x->p_manualSamples;
   }
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "BackgroundExtraction cannot be executed on complex images.";
      return false;
   }

   if ( p_sampleGeneration == BESampleGeneration::Manual && p_manualSamples.IsEmpty() )
   {
      whyNot = "No manual samples have been defined.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInstance::ExecuteOn( View& view )
{
   // Lock the view for modification
   view.LockForWrite();

   ImageVariant image = view.Image();

   if ( !image.IsComplexSample() )
   {
      ValidateParameters();
      
      Console console;
      console.EnableAbort();

      StandardStatus status;
      image.SetStatusCallback( &status );

      String info = String().Format( "Background extraction: %dx%d, %d channel(s)",
                                     image.Width(), image.Height(), image.NumberOfChannels() );
      console.WriteLn( "<end><cbr>" + info );

      bool ok = ExecuteOnImage( image );

      image.ResetSelections();

      view.Unlock();

      return ok;
   }

   view.Unlock();
   return false;
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInstance::ExecuteOnImage( ImageVariant& image )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return ExecuteT( static_cast<Image&>( *image ) );
      case 64: return ExecuteT( static_cast<DImage&>( *image ) );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return ExecuteT( static_cast<UInt8Image&>( *image ) );
      case 16: return ExecuteT( static_cast<UInt16Image&>( *image ) );
      case 32: return ExecuteT( static_cast<UInt32Image&>( *image ) );
      }

   return false;
}

// ----------------------------------------------------------------------------

template <class P>
bool BackgroundExtractionInstance::ExecuteT( GenericImage<P>& image )
{
   Console console;
   
   // Generate samples
   Array<Point> samples;
   
   console.WriteLn( String().Format( "<end><cbr>Generating samples using %s method...",
      p_sampleGeneration == BESampleGeneration::Automatic ? "automatic" :
      p_sampleGeneration == BESampleGeneration::Manual ? "manual" :
      p_sampleGeneration == BESampleGeneration::Grid ? "grid" : "gradient analysis" ) );
   
   switch ( p_sampleGeneration )
   {
   case BESampleGeneration::Automatic:
      GenerateAutomaticSamples( image, samples );
      break;
   case BESampleGeneration::Manual:
      for ( const Sample& s : p_manualSamples )
         if ( s.x >= 0 && s.x < image.Width() && s.y >= 0 && s.y < image.Height() )
            samples.Add( Point( s.x, s.y ) );
      break;
   case BESampleGeneration::Grid:
      GenerateGridSamples( image, samples );
      break;
   case BESampleGeneration::GradientAnalysis:
      GenerateGradientBasedSamples( image, samples );
      break;
   }
   
   console.WriteLn( String().Format( "<end><cbr>Generated %d samples", samples.Length() ) );
   
   if ( samples.IsEmpty() )
   {
      console.WarningLn( "** Warning: No valid samples generated" );
      return false;
   }
   
   // Apply outlier rejection if enabled
   if ( p_rejectionEnabled )
   {
      size_type initialCount = samples.Length();
      RejectOutliers( image, samples );
      console.WriteLn( String().Format( "<end><cbr>Rejected %d outliers, %d samples remain",
                                        initialCount - samples.Length(), samples.Length() ) );
   }
   
   if ( samples.Length() < 3 )
   {
      console.WarningLn( "** Warning: Insufficient samples after rejection" );
      return false;
   }
   
   // Create background model
   GenericImage<P> backgroundModel( image.Width(), image.Height(), 
                                    image.NumberOfChannels() == 1 ? ColorSpace::Gray : ColorSpace::RGB );
   
   console.WriteLn( String().Format( "<end><cbr>Fitting %s background model...",
      p_model == BEModel::Linear ? "linear" :
      p_model == BEModel::Polynomial2 ? "2nd degree polynomial" :
      p_model == BEModel::Polynomial3 ? "3rd degree polynomial" :
      p_model == BEModel::RBF ? "RBF" : "gradient domain" ) );
   
   if ( !FitBackgroundModel( image, samples, backgroundModel ) )
   {
      console.WarningLn( "** Warning: Background model fitting failed" );
      return false;
   }
   
   // Output background model as new image if requested
   if ( p_outputBackgroundModel )
   {
      ImageWindow window( image.Width(), image.Height(), image.NumberOfChannels(),
                         P::BitsPerSample(), P::IsFloatSample(),
                         image.IsColor(), true, "background_model" );
      
      if ( !window.IsNull() )
      {
         window.MainView().Image().CopyImage( backgroundModel );
         window.Show();
         console.WriteLn( "<end><cbr>Background model created as new image" );
      }
   }
   
   // Subtract background from target if requested
   if ( p_replaceTarget )
   {
      console.WriteLn( "<end><cbr>Subtracting background..." );
      
      image.SetStatusCallback( nullptr );
      
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         typename GenericImage<P>::sample_iterator i( image, c );
         typename GenericImage<P>::const_sample_iterator b( backgroundModel, c );
         
         for ( ; i; ++i, ++b )
            *i = typename P::sample( Max( 0.0, double(*i) - double(*b) ) );
      }
   }
   
   console.WriteLn( "<end><cbr>Background extraction completed successfully" );
   return true;
}

// ----------------------------------------------------------------------------
// Sample Generation Methods
// ----------------------------------------------------------------------------

template <class P>
void BackgroundExtractionInstance::GenerateAutomaticSamples( const GenericImage<P>& image, Array<Point>& samples )
{
   // Use a stratified sampling approach with outlier detection
   const int gridSize = 32; // Subdivision grid
   const int samplesPerCell = Max( 1, p_maxSamples / (gridSize * gridSize) );
   
   int cellWidth = Max( 1, image.Width() / gridSize );
   int cellHeight = Max( 1, image.Height() / gridSize );
   
   for ( int gy = 0; gy < gridSize && samples.Length() < size_type( p_maxSamples ); ++gy )
   {
      for ( int gx = 0; gx < gridSize && samples.Length() < size_type( p_maxSamples ); ++gx )
      {
         int x0 = gx * cellWidth;
         int y0 = gy * cellHeight;
         int x1 = Min( x0 + cellWidth, image.Width() );
         int y1 = Min( y0 + cellHeight, image.Height() );
         
         // Find darkest pixels in this cell (likely background)
         Array<Point> cellSamples;
         for ( int y = y0; y < y1; y += 2 )
         {
            for ( int x = x0; x < x1; x += 2 )
            {
               cellSamples.Add( Point( x, y ) );
            }
         }
         
         // Sort by brightness and take darkest samples
         cellSamples.Sort( [&]( const Point& a, const Point& b )
         {
            double sumA = 0, sumB = 0;
            for ( int c = 0; c < image.NumberOfChannels(); ++c )
            {
               sumA += image.Pixel( a.x, a.y, c );
               sumB += image.Pixel( b.x, b.y, c );
            }
            return sumA < sumB;
         } );
         
         for ( int i = 0; i < samplesPerCell && i < int( cellSamples.Length() ); ++i )
            samples.Add( cellSamples[i] );
      }
   }
   
   // Ensure minimum sample count
   if ( samples.Length() < size_type( p_minSamples ) )
   {
      // Add more samples if needed
      for ( int i = 0; i < p_minSamples - int( samples.Length() ) && samples.Length() < size_type( p_maxSamples ); ++i )
      {
         int x = rand() % image.Width();
         int y = rand() % image.Height();
         samples.Add( Point( x, y ) );
      }
   }
}

// ----------------------------------------------------------------------------

template <class P>
void BackgroundExtractionInstance::GenerateGridSamples( const GenericImage<P>& image, Array<Point>& samples )
{
   int stepX = Max( 1, image.Width() / p_gridColumns );
   int stepY = Max( 1, image.Height() / p_gridRows );
   
   for ( int gy = 0; gy < p_gridRows; ++gy )
   {
      for ( int gx = 0; gx < p_gridColumns; ++gx )
      {
         int x = Min( gx * stepX + stepX/2, image.Width() - 1 );
         int y = Min( gy * stepY + stepY/2, image.Height() - 1 );
         samples.Add( Point( x, y ) );
      }
   }
}

// ----------------------------------------------------------------------------

template <class P>
void BackgroundExtractionInstance::GenerateGradientBasedSamples( const GenericImage<P>& image, Array<Point>& samples )
{
   // Compute gradient magnitude
   GenericImage<P> dx, dy;
   ComputeGradientField( image, dx, dy );
   
   // Compute gradient magnitude for first channel
   GenericImage<P> gradMag( image.Width(), image.Height(), ColorSpace::Gray );
   
   for ( int y = 0; y < image.Height(); ++y )
   {
      for ( int x = 0; x < image.Width(); ++x )
      {
         double gx = dx.Pixel( x, y, 0 );
         double gy = dy.Pixel( x, y, 0 );
         gradMag.Pixel( x, y, 0 ) = Sqrt( gx*gx + gy*gy );
      }
   }
   
   // Create list of all pixels with their gradient magnitudes
   struct PixelGrad
   {
      Point pos;
      double grad;
      
      bool operator <( const PixelGrad& other ) const { return grad < other.grad; }
   };
   
   Array<PixelGrad> pixels;
   for ( int y = 0; y < image.Height(); y += 2 )
   {
      for ( int x = 0; x < image.Width(); x += 2 )
      {
         PixelGrad pg;
         pg.pos = Point( x, y );
         pg.grad = gradMag.Pixel( x, y, 0 );
         pixels.Add( pg );
      }
   }
   
   // Sort by gradient magnitude (lowest first - these are background)
   pixels.Sort();
   
   // Take samples from low-gradient regions
   size_type numSamples = Min( size_type( p_maxSamples ), pixels.Length() / 2 );
   for ( size_type i = 0; i < numSamples; ++i )
      samples.Add( pixels[i].pos );
}

// ----------------------------------------------------------------------------
// Outlier Rejection
// ----------------------------------------------------------------------------

template <class P>
void BackgroundExtractionInstance::RejectOutliers( const GenericImage<P>& image, Array<Point>& samples )
{
   for ( int iter = 0; iter < p_rejectionIterations; ++iter )
   {
      double mean, stdDev;
      ComputeSampleStatistics( image, samples, mean, stdDev );
      
      Array<Point> accepted;
      for ( const Point& sample : samples )
      {
         double value = 0;
         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            value += image.Pixel( sample.x, sample.y, c );
         value /= image.NumberOfChannels();
         
         double zScore = Abs( value - mean ) / stdDev;
         
         if ( zScore <= p_rejectionHigh )
            accepted.Add( sample );
      }
      
      samples = accepted;
      
      if ( samples.Length() < 3 )
         break;
   }
}

// ----------------------------------------------------------------------------

template <class P>
double BackgroundExtractionInstance::ComputeSampleStatistics( const GenericImage<P>& image,
                                                              const Array<Point>& samples,
                                                              double& mean, double& stdDev )
{
   if ( samples.IsEmpty() )
   {
      mean = stdDev = 0;
      return 0;
   }
   
   // Compute mean
   mean = 0;
   for ( const Point& sample : samples )
   {
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
         mean += image.Pixel( sample.x, sample.y, c );
   }
   mean /= samples.Length() * image.NumberOfChannels();
   
   // Compute standard deviation
   stdDev = 0;
   for ( const Point& sample : samples )
   {
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         double diff = image.Pixel( sample.x, sample.y, c ) - mean;
         stdDev += diff * diff;
      }
   }
   stdDev = Sqrt( stdDev / (samples.Length() * image.NumberOfChannels()) );
   
   return mean;
}

// ----------------------------------------------------------------------------
// Background Model Fitting
// ----------------------------------------------------------------------------

template <class P>
bool BackgroundExtractionInstance::FitBackgroundModel( const GenericImage<P>& image,
                                                       const Array<Point>& samples,
                                                       GenericImage<P>& backgroundModel )
{
   switch ( p_model )
   {
   case BEModel::Linear:
      return FitLinearModel( image, samples, backgroundModel );
      
   case BEModel::Polynomial2:
      return FitPolynomialModel( image, samples, backgroundModel, 2 );
      
   case BEModel::Polynomial3:
      return FitPolynomialModel( image, samples, backgroundModel, 3 );
      
   case BEModel::RBF:
      return FitRBFModel( image, samples, backgroundModel );
      
   case BEModel::GradientDomain:
      return FitGradientDomainModel( image, samples, backgroundModel );
      
   default:
      return false;
   }
}

// ----------------------------------------------------------------------------

template <class P>
bool BackgroundExtractionInstance::FitLinearModel( const GenericImage<P>& image,
                                                   const Array<Point>& samples,
                                                   GenericImage<P>& backgroundModel )
{
   // For each channel, fit plane: z = a*x + b*y + c
   // Using least squares: minimize sum of (z - (a*x + b*y + c))^2
   
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      int n = samples.Length();
      
      // Build normal equations for least squares
      double sumX = 0, sumY = 0, sumZ = 0;
      double sumXX = 0, sumYY = 0, sumXY = 0;
      double sumXZ = 0, sumYZ = 0;
      
      for ( size_type i = 0; i < samples.Length(); ++i )
      {
         double x = samples[i].x;
         double y = samples[i].y;
         double z = image.Pixel( samples[i].x, samples[i].y, c );
         
         sumX += x;
         sumY += y;
         sumZ += z;
         sumXX += x * x;
         sumYY += y * y;
         sumXY += x * y;
         sumXZ += x * z;
         sumYZ += y * z;
      }
      
      // Solve 3x3 system for a, b, c
      // [sumXX  sumXY  sumX ] [a]   [sumXZ]
      // [sumXY  sumYY  sumY ] [b] = [sumYZ]
      // [sumX   sumY   n    ] [c]   [sumZ ]
      
      double det = sumXX * (sumYY * n - sumY * sumY) 
                 - sumXY * (sumXY * n - sumX * sumY)
                 + sumX * (sumXY * sumY - sumYY * sumX);
      
      if ( Abs( det ) < 1e-10 )
      {
         // Degenerate case, use mean value
         double meanZ = sumZ / n;
         backgroundModel.Fill( typename P::sample( meanZ ), c );
         continue;
      }
      
      double a = ((sumXZ * (sumYY * n - sumY * sumY) 
                 - sumXY * (sumYZ * n - sumY * sumZ)
                 + sumX * (sumYZ * sumY - sumYY * sumZ)) / det);
                 
      double b = ((sumXX * (sumYZ * n - sumY * sumZ)
                 - sumXZ * (sumXY * n - sumX * sumY)
                 + sumX * (sumXY * sumZ - sumYZ * sumX)) / det);
                 
      double d = ((sumXX * (sumYY * sumZ - sumY * sumYZ)
                 - sumXY * (sumXY * sumZ - sumX * sumYZ)
                 + sumXZ * (sumXY * sumY - sumYY * sumX)) / det);
      
      // Evaluate plane
      for ( int y = 0; y < image.Height(); ++y )
      {
         for ( int x = 0; x < image.Width(); ++x )
         {
            double value = a * x + b * y + d;
            backgroundModel.Pixel( x, y, c ) = typename P::sample( Range( value, 0.0, 1.0 ) );
         }
      }
   }
   
   return true;
}

// ----------------------------------------------------------------------------

template <class P>
bool BackgroundExtractionInstance::FitPolynomialModel( const GenericImage<P>& image,
                                                       const Array<Point>& samples,
                                                       GenericImage<P>& backgroundModel,
                                                       int degree )
{
   // Simplified polynomial fitting using Gaussian elimination
   // For 2nd degree: z = a + b*x + c*y + d*x² + e*x*y + f*y²
   // For 3rd degree: add g*x³ + h*x²*y + i*x*y² + j*y³
   
   int numCoeffs = (degree == 2) ? 6 : 10;
   
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      int n = samples.Length();
      
      // Build design matrix and observation vector
      DMatrix A( n, numCoeffs );
      DVector b( n );
      
      // Normalize coordinates to [-1, 1] range for numerical stability
      double xNorm = image.Width() / 2.0;
      double yNorm = image.Height() / 2.0;
      
      for ( int i = 0; i < n; ++i )
      {
         double x = (samples[i].x - xNorm) / xNorm;
         double y = (samples[i].y - yNorm) / yNorm;
         
         // Fill design matrix
         A[i][0] = 1.0;
         A[i][1] = x;
         A[i][2] = y;
         A[i][3] = x * x;
         A[i][4] = x * y;
         A[i][5] = y * y;
         
         if ( degree == 3 )
         {
            A[i][6] = x * x * x;
            A[i][7] = x * x * y;
            A[i][8] = x * y * y;
            A[i][9] = y * y * y;
         }
         
         b[i] = image.Pixel( samples[i].x, samples[i].y, c );
      }
      
      // Solve normal equations: A^T * A * x = A^T * b
      DMatrix AtA( numCoeffs, numCoeffs );
      DVector Atb( numCoeffs );
      
      // Compute A^T * A
      for ( int i = 0; i < numCoeffs; ++i )
      {
         for ( int j = 0; j < numCoeffs; ++j )
         {
            double sum = 0;
            for ( int k = 0; k < n; ++k )
               sum += A[k][i] * A[k][j];
            AtA[i][j] = sum;
         }
      }
      
      // Compute A^T * b
      for ( int i = 0; i < numCoeffs; ++i )
      {
         double sum = 0;
         for ( int k = 0; k < n; ++k )
            sum += A[k][i] * b[k];
         Atb[i] = sum;
      }
      
      // Solve using Gaussian elimination with partial pivoting
      DVector coeffs( numCoeffs );
      
      // Forward elimination
      for ( int k = 0; k < numCoeffs - 1; ++k )
      {
         // Find pivot
         int pivotRow = k;
         double maxVal = Abs( AtA[k][k] );
         for ( int i = k + 1; i < numCoeffs; ++i )
         {
            if ( Abs( AtA[i][k] ) > maxVal )
            {
               maxVal = Abs( AtA[i][k] );
               pivotRow = i;
            }
         }
         
         // Swap rows if needed
         if ( pivotRow != k )
         {
            for ( int j = 0; j < numCoeffs; ++j )
               Swap( AtA[k][j], AtA[pivotRow][j] );
            Swap( Atb[k], Atb[pivotRow] );
         }
         
         // Eliminate
         for ( int i = k + 1; i < numCoeffs; ++i )
         {
            if ( Abs( AtA[k][k] ) > 1e-10 )
            {
               double factor = AtA[i][k] / AtA[k][k];
               for ( int j = k; j < numCoeffs; ++j )
                  AtA[i][j] -= factor * AtA[k][j];
               Atb[i] -= factor * Atb[k];
            }
         }
      }
      
      // Back substitution
      for ( int i = numCoeffs - 1; i >= 0; --i )
      {
         double sum = Atb[i];
         for ( int j = i + 1; j < numCoeffs; ++j )
            sum -= AtA[i][j] * coeffs[j];
         coeffs[i] = (Abs( AtA[i][i] ) > 1e-10) ? sum / AtA[i][i] : 0;
      }
      
      // Evaluate polynomial model
      for ( int y = 0; y < image.Height(); ++y )
      {
         for ( int x = 0; x < image.Width(); ++x )
         {
            double xn = (x - xNorm) / xNorm;
            double yn = (y - yNorm) / yNorm;
            
            double value = coeffs[0] + coeffs[1]*xn + coeffs[2]*yn +
                          coeffs[3]*xn*xn + coeffs[4]*xn*yn + coeffs[5]*yn*yn;
            
            if ( degree == 3 )
            {
               value += coeffs[6]*xn*xn*xn + coeffs[7]*xn*xn*yn +
                       coeffs[8]*xn*yn*yn + coeffs[9]*yn*yn*yn;
            }
            
            backgroundModel.Pixel( x, y, c ) = typename P::sample( Range( value, 0.0, 1.0 ) );
         }
      }
   }
   
   return true;
}

// ----------------------------------------------------------------------------

template <class P>
bool BackgroundExtractionInstance::FitRBFModel( const GenericImage<P>& image,
                                               const Array<Point>& samples,
                                               GenericImage<P>& backgroundModel )
{
   // Radial Basis Function interpolation
   // Using thin-plate spline: φ(r) = r² * log(r)
   
   int n = samples.Length();
   
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      // Build interpolation matrix
      DMatrix A( n, n );
      DVector b( n );
      
      for ( int i = 0; i < n; ++i )
      {
         for ( int j = 0; j < n; ++j )
         {
            double dx = samples[i].x - samples[j].x;
            double dy = samples[i].y - samples[j].y;
            double r = Sqrt( dx*dx + dy*dy );
            
            if ( r < 1e-6 )
               A[i][j] = 0;
            else
               A[i][j] = r * r * Log( r ); // Thin-plate spline
         }
         
         b[i] = image.Pixel( samples[i].x, samples[i].y, c );
      }
      
      // Solve for weights
      DVector weights = A.Inverse() * b;
      
      // Evaluate RBF model
      for ( int y = 0; y < image.Height(); ++y )
      {
         for ( int x = 0; x < image.Width(); ++x )
         {
            double value = 0;
            
            for ( int i = 0; i < n; ++i )
            {
               double dx = x - samples[i].x;
               double dy = y - samples[i].y;
               double r = Sqrt( dx*dx + dy*dy );
               
               if ( r > 1e-6 )
                  value += weights[i] * r * r * Log( r );
            }
            
            backgroundModel.Pixel( x, y, c ) = P::FloatToSample( Range( value, 0.0, 1.0 ) );
         }
      }
   }
   
   return true;
}

// ----------------------------------------------------------------------------

template <class P>
bool BackgroundExtractionInstance::FitGradientDomainModel( const GenericImage<P>& image,
                                                          const Array<Point>& samples,
                                                          GenericImage<P>& backgroundModel )
{
   // Gradient domain reconstruction
   // 1. Compute gradient field of original image
   // 2. Solve Poisson equation with boundary conditions from samples
   
   GenericImage<P> dx, dy;
   ComputeGradientField( image, dx, dy );
   
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      GenericImage<P> result( image.Width(), image.Height(), ColorSpace::Gray );
      
      SolveGradientDomain( dx, dy, samples, image, result );
      
      // Copy to background model
      for ( int y = 0; y < image.Height(); ++y )
         for ( int x = 0; x < image.Width(); ++x )
            backgroundModel.Pixel( x, y, c ) = result.Pixel( x, y, 0 );
   }
   
   return true;
}

// ----------------------------------------------------------------------------
// Gradient Domain Utilities
// ----------------------------------------------------------------------------

template <class P>
void BackgroundExtractionInstance::ComputeGradientField( const GenericImage<P>& image,
                                                         GenericImage<P>& dx,
                                                         GenericImage<P>& dy )
{
   dx.AllocateData( image.Width(), image.Height(), image.NumberOfChannels() );
   dy.AllocateData( image.Width(), image.Height(), image.NumberOfChannels() );
   
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
   {
      for ( int y = 0; y < image.Height(); ++y )
      {
         for ( int x = 0; x < image.Width(); ++x )
         {
            // Forward differences
            double gx = (x < image.Width() - 1) ?
                       image.Pixel( x + 1, y, c ) - image.Pixel( x, y, c ) : 0;
                       
            double gy = (y < image.Height() - 1) ?
                       image.Pixel( x, y + 1, c ) - image.Pixel( x, y, c ) : 0;
            
            dx.Pixel( x, y, c ) = gx;
            dy.Pixel( x, y, c ) = gy;
         }
      }
   }
}

// ----------------------------------------------------------------------------

template <class P>
void BackgroundExtractionInstance::SolveGradientDomain( const GenericImage<P>& dx,
                                                        const GenericImage<P>& dy,
                                                        const Array<Point>& samples,
                                                        const GenericImage<P>& sourceImage,
                                                        GenericImage<P>& result )
{
   // Simplified gradient domain solver using iterative method
   // This is a basic implementation - for production, use multigrid or FFT methods
   
   int w = dx.Width();
   int h = dx.Height();
   
   result.Zero();
   
   // Initialize with sample values
   for ( const Point& sample : samples )
   {
      result.Pixel( sample.x, sample.y, 0 ) = sourceImage.Pixel( sample.x, sample.y, 0 );
   }
   
   // Gauss-Seidel iterations
   const int maxIter = 100;
   const double omega = 1.5; // Over-relaxation factor
   
   for ( int iter = 0; iter < maxIter; ++iter )
   {
      double maxChange = 0;
      
      for ( int y = 1; y < h - 1; ++y )
      {
         for ( int x = 1; x < w - 1; ++x )
         {
            // Skip if this is a sample point
            bool isSample = false;
            for ( const Point& sample : samples )
            {
               if ( sample.x == x && sample.y == y )
               {
                  isSample = true;
                  break;
               }
            }
            
            if ( isSample )
               continue;
            
            // Compute Laplacian of result
            double lap = result.Pixel( x - 1, y, 0 ) + result.Pixel( x + 1, y, 0 ) +
                        result.Pixel( x, y - 1, 0 ) + result.Pixel( x, y + 1, 0 ) -
                        4 * result.Pixel( x, y, 0 );
            
            // Compute divergence of gradient field
            double div = (dx.Pixel( x, y, 0 ) - dx.Pixel( x - 1, y, 0 )) +
                        (dy.Pixel( x, y, 0 ) - dy.Pixel( x, y - 1, 0 ));
            
            // Update
            double correction = omega * (div - lap) / 4.0;
            double oldValue = result.Pixel( x, y, 0 );
            double newValue = oldValue + correction;
            
            result.Pixel( x, y, 0 ) = Range( newValue, 0.0, 1.0 );
            
            maxChange = Max( maxChange, Abs( correction ) );
         }
      }
      
      // Check for convergence
      if ( maxChange < 1e-6 )
         break;
   }
}

// ----------------------------------------------------------------------------
// Parameter Interface
// ----------------------------------------------------------------------------

void* BackgroundExtractionInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheBEModelParameter )
      return &p_model;
   if ( p == TheBESampleGenerationParameter )
      return &p_sampleGeneration;
   if ( p == TheBEToleranceParameter )
      return &p_tolerance;
   if ( p == TheBEDeviationParameter )
      return &p_deviation;
   if ( p == TheBEMinSamplesParameter )
      return &p_minSamples;
   if ( p == TheBEMaxSamplesParameter )
      return &p_maxSamples;
   if ( p == TheBERejectionEnabledParameter )
      return &p_rejectionEnabled;
   if ( p == TheBERejectionLowParameter )
      return &p_rejectionLow;
   if ( p == TheBERejectionHighParameter )
      return &p_rejectionHigh;
   if ( p == TheBERejectionIterationsParameter )
      return &p_rejectionIterations;
   if ( p == TheBEGridRowsParameter )
      return &p_gridRows;
   if ( p == TheBEGridColumnsParameter )
      return &p_gridColumns;
   if ( p == TheBEReplaceTargetParameter )
      return &p_replaceTarget;
   if ( p == TheBEOutputBackgroundModelParameter )
      return &p_outputBackgroundModel;
   if ( p == TheBEManualSampleXParameter )
      return &p_manualSamples[tableRow].x;
   if ( p == TheBEManualSampleYParameter )
      return &p_manualSamples[tableRow].y;
   
   return nullptr;
}

// ----------------------------------------------------------------------------

bool BackgroundExtractionInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheBEManualSamplesParameter )
   {
      p_manualSamples.Clear();
      if ( sizeOrLength > 0 )
         p_manualSamples.Add( Sample(), sizeOrLength );
   }
   else
      return false;
   
   return true;
}

// ----------------------------------------------------------------------------

size_type BackgroundExtractionInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheBEManualSamplesParameter )
      return p_manualSamples.Length();
   
   return 0;
}

// ----------------------------------------------------------------------------

void BackgroundExtractionInstance::ValidateParameters()
{
   p_tolerance = Range( p_tolerance, TheBEToleranceParameter->MinimumValue(), TheBEToleranceParameter->MaximumValue() );
   p_deviation = Range( p_deviation, TheBEDeviationParameter->MinimumValue(), TheBEDeviationParameter->MaximumValue() );
   p_minSamples = Range( p_minSamples, int( TheBEMinSamplesParameter->MinimumValue() ), int( TheBEMinSamplesParameter->MaximumValue() ) );
   p_maxSamples = Range( p_maxSamples, int( TheBEMaxSamplesParameter->MinimumValue() ), int( TheBEMaxSamplesParameter->MaximumValue() ) );
   p_rejectionLow = Range( p_rejectionLow, TheBERejectionLowParameter->MinimumValue(), TheBERejectionLowParameter->MaximumValue() );
   p_rejectionHigh = Range( p_rejectionHigh, TheBERejectionHighParameter->MinimumValue(), TheBERejectionHighParameter->MaximumValue() );
   p_rejectionIterations = Range( p_rejectionIterations, int( TheBERejectionIterationsParameter->MinimumValue() ), int( TheBERejectionIterationsParameter->MaximumValue() ) );
   p_gridRows = Range( p_gridRows, int( TheBEGridRowsParameter->MinimumValue() ), int( TheBEGridRowsParameter->MaximumValue() ) );
   p_gridColumns = Range( p_gridColumns, int( TheBEGridColumnsParameter->MinimumValue() ), int( TheBEGridColumnsParameter->MaximumValue() ) );
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
