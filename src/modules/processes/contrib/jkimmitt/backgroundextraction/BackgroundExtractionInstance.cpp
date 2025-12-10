// ****************************************************************************
// PixInsight Class Library - PCL 02.08.05
// Standard BackgroundExtraction Process Module Version 1.0.0
// ****************************************************************************
// BackgroundExtractionInstance.cpp - Released 2025-12-10
// ****************************************************************************

#include "BackgroundExtractionInstance.h"
#include "BackgroundExtractionParameters.h"
#include "BackgroundExtractor.h"

#include <pcl/AutoViewLock.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ============================================================================

BackgroundExtractionInstance::BackgroundExtractionInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_modelType( BGModelType::Default )
   , p_sampleGenerationMode( BGSampleGenerationMode::Default )
   , p_sampleSize( TheBGSampleSizeParameter->DefaultValue() )
   , p_sampleTolerance( TheBGSampleToleranceParameter->DefaultValue() )
   , p_minSamples( TheBGMinSamplesParameter->DefaultValue() )
   , p_maxSamples( TheBGMaxSamplesParameter->DefaultValue() )
   , p_gridSpacingX( TheBGGridSpacingXParameter->DefaultValue() )
   , p_gridSpacingY( TheBGGridSpacingYParameter->DefaultValue() )
   , p_enableOutlierRejection( TheBGEnableOutlierRejectionParameter->DefaultValue() )
   , p_outlierRejectionThreshold( TheBGOutlierRejectionThresholdParameter->DefaultValue() )
   , p_outlierRejectionIterations( TheBGOutlierRejectionIterationsParameter->DefaultValue() )
   , p_rbfSmoothness( TheBGRBFSmoothnessParameter->DefaultValue() )
   , p_replaceTarget( TheBGReplaceTargetParameter->DefaultValue() )
   , p_outputBackgroundModel( TheBGOutputBackgroundModelParameter->DefaultValue() )
   , p_applySTFToBackground( TheBGApplySTFToBackgroundParameter->DefaultValue() )
{
}

// ============================================================================

BackgroundExtractionInstance::BackgroundExtractionInstance( const BackgroundExtractionInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ============================================================================

void BackgroundExtractionInstance::Assign( const ProcessImplementation& p )
{
   const BackgroundExtractionInstance* x = dynamic_cast<const BackgroundExtractionInstance*>( &p );
   if ( x != nullptr )
   {
      p_modelType = x->p_modelType;
      p_sampleGenerationMode = x->p_sampleGenerationMode;
      p_sampleSize = x->p_sampleSize;
      p_sampleTolerance = x->p_sampleTolerance;
      p_minSamples = x->p_minSamples;
      p_maxSamples = x->p_maxSamples;
      p_gridSpacingX = x->p_gridSpacingX;
      p_gridSpacingY = x->p_gridSpacingY;
      p_enableOutlierRejection = x->p_enableOutlierRejection;
      p_outlierRejectionThreshold = x->p_outlierRejectionThreshold;
      p_outlierRejectionIterations = x->p_outlierRejectionIterations;
      p_rbfSmoothness = x->p_rbfSmoothness;
      p_replaceTarget = x->p_replaceTarget;
      p_outputBackgroundModel = x->p_outputBackgroundModel;
      p_applySTFToBackground = x->p_applySTFToBackground;
   }
}

// ============================================================================

bool BackgroundExtractionInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "BackgroundExtraction cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ============================================================================

bool BackgroundExtractionInstance::IsHistoryUpdater( const View& view ) const
{
   return true;
}

// ============================================================================

bool BackgroundExtractionInstance::CanExecuteGlobal( String& whyNot ) const
{
   whyNot = "BackgroundExtraction can only be executed on views.";
   return false;
}

// ============================================================================

bool BackgroundExtractionInstance::ExecuteGlobal()
{
   throw Error( "BackgroundExtraction cannot be executed in the global context." );
}

// ============================================================================

void* BackgroundExtractionInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheBGModelTypeParameter )
      return &p_modelType;
   if ( p == TheBGSampleGenerationModeParameter )
      return &p_sampleGenerationMode;
   if ( p == TheBGSampleSizeParameter )
      return &p_sampleSize;
   if ( p == TheBGSampleToleranceParameter )
      return &p_sampleTolerance;
   if ( p == TheBGMinSamplesParameter )
      return &p_minSamples;
   if ( p == TheBGMaxSamplesParameter )
      return &p_maxSamples;
   if ( p == TheBGGridSpacingXParameter )
      return &p_gridSpacingX;
   if ( p == TheBGGridSpacingYParameter )
      return &p_gridSpacingY;
   if ( p == TheBGEnableOutlierRejectionParameter )
      return &p_enableOutlierRejection;
   if ( p == TheBGOutlierRejectionThresholdParameter )
      return &p_outlierRejectionThreshold;
   if ( p == TheBGOutlierRejectionIterationsParameter )
      return &p_outlierRejectionIterations;
   if ( p == TheBGRBFSmoothnessParameter )
      return &p_rbfSmoothness;
   if ( p == TheBGReplaceTargetParameter )
      return &p_replaceTarget;
   if ( p == TheBGOutputBackgroundModelParameter )
      return &p_outputBackgroundModel;
   if ( p == TheBGApplySTFToBackgroundParameter )
      return &p_applySTFToBackground;
      
   return nullptr;
}

// ============================================================================

bool BackgroundExtractionInstance::AllocateParameter( size_type /*sizeOrLength*/, const MetaParameter* /*p*/, size_type /*tableRow*/ )
{
   return false;
}

// ============================================================================

size_type BackgroundExtractionInstance::ParameterLength( const MetaParameter* /*p*/, size_type /*tableRow*/ ) const
{
   return 0;
}

// ============================================================================

// Helper function for MTF calculation
inline double MTF( double m, double x )
{
   if ( x <= 0 )
      return 0;
   if ( x >= 1 )
      return 1;
   if ( m <= 0 )
      return 0;
   if ( m >= 1 )
      return 1;
      
   return (m - 1)*x / ((2*m - 1)*x - m);
}

// ============================================================================
// ExecuteOn - WITH STF SUPPORT
// ============================================================================

bool BackgroundExtractionInstance::ExecuteOn( View& view )
{
   qDebug() << "\n=== BackgroundExtraction::ExecuteOn START ===";
   
   // Lock the view for modification
   view.LockForWrite();
   
   ImageVariant image = view.Image();
   
   // Get image dimensions
   int width = image.Width();
   int height = image.Height();
   int numberOfChannels = image.NumberOfChannels();
   
   qDebug() << "Image dimensions:" << width << "x" << height << "x" << numberOfChannels;
   
   // Create a new Image to hold the background model
   Image backgroundModel( width, height, numberOfChannels );
   
   // Process each channel
   for ( int c = 0; c < numberOfChannels; ++c )
   {
      qDebug() << "Processing channel" << c;
      
      // Extract single channel as DImage
      image.SelectChannel( c );
      DImage channelImage;
      channelImage.Assign( image );
      
      // Sample a few pixels to verify we have data
      qDebug() << "  Sample input pixels:";
      qDebug() << "    [100,100]:" << channelImage(100, 100);
      qDebug() << "    [500,500]:" << channelImage(500, 500);
      qDebug() << "    [1000,1000]:" << channelImage(1000, 1000);
      
      // Create the background extractor
      BackgroundExtractor extractor( channelImage, *this );
      
      // Generate samples if in automatic mode
      if ( p_sampleGenerationMode == BGSampleGenerationMode::Automatic )
      {
         qDebug() << "  Generating samples automatically...";
         extractor.GenerateSamples();
         qDebug() << "  Generated" << extractor.Samples().Length() << "samples";
      }
      
      // Fit the background model
      qDebug() << "  Fitting background model...";
      extractor.FitBackground();
      
      // Get the fitted background for this channel
      const DImage& background = extractor.Background();
      
      // Sample the background to see what we got
      qDebug() << "  Sample background pixels:";
      qDebug() << "    [100,100]:" << background(100, 100);
      qDebug() << "    [500,500]:" << background(500, 500);
      qDebug() << "    [1000,1000]:" << background(1000, 1000);
      
      double minBg = background.MinimumPixelValue();
      double maxBg = background.MaximumPixelValue();
      double meanBg = background.Mean();
      qDebug() << "  Background stats: min=" << minBg << "max=" << maxBg << "mean=" << meanBg;
      
      // Copy background to our model image
      for ( int y = 0; y < height; ++y )
      {
         for ( int x = 0; x < width; ++x )
         {
            backgroundModel( x, y, c ) = background( x, y );
         }
      }
      
      // Subtract background from original if replace mode is enabled
      if ( p_replaceTarget )
      {
         qDebug() << "  Subtracting background from image...";
         
         if ( image.IsFloatSample() )
         {
            Image& img = static_cast<Image&>( *image );
            for ( int y = 0; y < height; ++y )
            {
               for ( int x = 0; x < width; ++x )
               {
                  double value = img( x, y ) - background( x, y );
                  img( x, y ) = Max( 0.0, value );
               }
            }
         }
      }
   }
   
   // Reset channel selection
   image.ResetChannelRange();
   
   qDebug() << "Background model extracted, creating new view...";
   
   // Create a new ImageWindow to display the background model
   if ( p_outputBackgroundModel )
   {
      try
      {
         qDebug() << "Creating ImageWindow for background model...";
         
         // Generate a unique ID for the background window
         IsoString backgroundId = view.FullId() + "_background";
         
         qDebug() << "  Background window ID:" << backgroundId.c_str();
         
         // Create the window
         ImageWindow backgroundWindow( width, height, numberOfChannels,
                                       32/*bits*/, true/*floatSample*/, 
                                       numberOfChannels >= 3/*color*/,
                                       true/*initialProcessing*/,
                                       backgroundId );
         
         if ( backgroundWindow.IsNull() )
         {
            qWarning() << "Failed to create background window!";
         }
         else
         {
            qDebug() << "  Background window created successfully";
            
            // Get the main view of the new window
            View backgroundView = backgroundWindow.MainView();
            qDebug() << "  Got main view:" << backgroundView.FullId().c_str();
            
            // Lock it for writing
            backgroundView.LockForWrite();
            
            // Get the image variant and copy our background model
            ImageVariant backgroundImage = backgroundView.Image();
            qDebug() << "  Copying background model to view...";
            backgroundImage.CopyImage( backgroundModel );
            
            // Unlock
            backgroundView.Unlock();
            
            // Apply STF (auto-stretch) if requested
            if ( p_applySTFToBackground )
            {
               qDebug() << "  Applying STF (auto-stretch) to background...";
               
               try
               {
                  // Compute STF for the background model
                  View::stf_list stfList;
                  
                  for ( int c = 0; c < numberOfChannels; ++c )
                  {
                     backgroundImage.SelectChannel( c );
                     
                     // Get channel statistics
                     double median, mad;
                     if ( backgroundImage.IsFloatSample() )
                     {
                        Image& img = static_cast<Image&>( *backgroundImage );
                        median = img.Median();
                        mad = img.MAD( median );
                     }
                     else
                     {
                        median = backgroundImage.Median();
                        mad = backgroundImage.MAD( median );
                     }
                     
                     qDebug() << "    Channel" << c << "median:" << median << "MAD:" << mad;
                     
                     // Calculate shadow clipping point (median - 2.8*MAD)
                     double c0 = Max( 0.0, median - 2.8 * mad );
                     
                     // Calculate midtones transfer point
                     double m = 0.5;
                     if ( median > c0 )
                     {
                        m = MTF( (median - c0), (1.0 - c0) );
                     }
                     
                     // Calculate highlight clipping
                     double c1 = 1.0;
                     
                     qDebug() << "    STF params: c0=" << c0 << "m=" << m << "c1=" << c1;
                     
                     // Create STF for this channel
                     stfList.Add( View::stf( c0, m, c1, 0.0, 1.0 ) );
                  }
                  
                  backgroundImage.ResetChannelRange();
                  
                  // Apply the STF to the view
                  backgroundView.SetScreenTransferFunctions( stfList );
                  backgroundView.EnableScreenTransferFunctions( true );
                  
                  qDebug() << "  ✓ STF applied";
               }
               catch ( const Exception& ex )
               {
                  qWarning() << "Exception applying STF:" << ex.Message().c_str();
               }
               catch ( ... )
               {
                  qWarning() << "Unknown exception applying STF";
               }
            }
            
            qDebug() << "  Showing background window...";
            
            // Show the window
            backgroundWindow.Show();
            backgroundWindow.ZoomToFit( false/*allowMagnification*/ );
            
            qDebug() << "✓ Background model view created:" << backgroundView.FullId().c_str();
         }
      }
      catch ( const Exception& ex )
      {
         qWarning() << "Exception creating background window:" << ex.Message().c_str();
      }
      catch ( ... )
      {
         qWarning() << "Unknown exception creating background window";
      }
   }
   
   view.Unlock();
   
   qDebug() << "=== BackgroundExtraction::ExecuteOn COMPLETE ===\n";
   
   return true;
}

// ============================================================================

} // pcl

// ****************************************************************************
// EOF BackgroundExtractionInstance.cpp - Released 2025-12-10
// ****************************************************************************
