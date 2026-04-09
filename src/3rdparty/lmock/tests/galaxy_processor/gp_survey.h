// gp_survey.h -- Survey reference fetching and optimization
#pragma once

#include "gp_wcs.h"
#include "gp_histogram.h"
#include "gp_stretch.h"

// Optimization MTF
float opt_mtf( float v, float m );

// Fetch a survey reference image from online surveys (Legacy Survey)
bool FetchSurveyReference( Image& refImage, const SimpleTANWCS& wcs,
                           int inputW, int inputH,
                           double centerRA, double centerDec,
                           double pixscaleArcsec, double targetBg,
                           const char* layer = "sdss" );

// Run iterative coefficient optimization against survey reference
void RunOptimization( Image& image, int maxIterations, const SimpleTANWCS& wcs,
                      double centerRA, double centerDec, double pixscaleArcsec );
