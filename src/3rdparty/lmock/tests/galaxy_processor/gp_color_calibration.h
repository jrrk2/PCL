// gp_color_calibration.h -- Background neutralization and color calibration
#pragma once

#include "gp_wcs.h"

// Filter response curve interpolation
struct FilterPoint { double wavelength; double transmission; };
extern const FilterPoint s_filterR[];
extern const FilterPoint s_filterG[];
extern const FilterPoint s_filterB[];
extern const int s_filterRCount, s_filterGCount, s_filterBCount;

double InterpolateFilter( const FilterPoint* filter, int count, double wavelength );
double ConvolveSpectrumWithFilter(
   const FVector& spectrum, float spectrumStart, float spectrumStep, int spectrumCount,
   const FilterPoint* filter, int filterCount );

// Statistics helpers
double Median( std::vector<double>& v );
double MAD( std::vector<double>& v, double median );

// Smart crop (remove black stacking edges)
void SmartCropStackingEdges( Image& image, int& cropX0, int& cropY0 );

// Background neutralization
void BackgroundNeutralize( Image& image );

// Spectrophotometric color calibration (Gaia)
void SpectrophotometricColorCalibrate(
      Image& image,
      const FITSKeywordArray& keywords,
      const PropertyArray& properties,
      const std::vector<std::string>& gaiaDbPaths );

// SDSS color calibration
struct SDSSstar
{
   double ra, dec;
   double psfMag_u, psfMag_g, psfMag_r, psfMag_i;
};
std::vector<SDSSstar> FetchSDSSPhotometry( double centerRA, double centerDec, double radiusDeg );
void SDSSColorCalibrate( Image& image, const FITSKeywordArray& keywords );
