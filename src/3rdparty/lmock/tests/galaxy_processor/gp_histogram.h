// gp_histogram.h -- Histogram computation and distance metrics
#pragma once

#include "gp_common.h"

static constexpr int HIST_BINS = 256;

struct ImageHistogram
{
   float lum[HIST_BINS];      // luminance histogram (normalized)
   float ch[3][HIST_BINS];    // per-channel histograms (normalized)
   float percentiles[11];     // luminance p0,p10,p20,...,p100
};

ImageHistogram compute_histogram( const Image& image );
float histogram_distance( const ImageHistogram& a, const ImageHistogram& b );
