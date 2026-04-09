// gp_stretch.h -- VeraLux HyperMetric stretch and wavelet enhancement
#pragma once

#include "gp_common.h"

struct VeraLuxParams
{
   double weights[3]    = { 0.2126, 0.7152, 0.0722 }; // Rec.709 luminance
   double log_d         = 0.0;    // 0 = auto-solve to target_bg
   double protect_b     = 5.0;    // highlight protection
   double convergence   = 3.5;    // colour → white blend speed
   double color_grip    = 1.0;    // 1.0 = luminance-preserving
   double shadow_conv   = 0.0;    // shadow convergence fade
   double target_bg     = 0.10;   // target background level
   bool   adaptive      = false;  // adaptive histogram anchor
   bool   add_pedestal  = true;   // 0.5% pedestal
   int    wavelet_scales    = 5;
   double wavelet_gain[5]  = { 0.0, 0.7, 1.0, 1.3, 0.5 };
   double wavelet_residual = 1.3;
   double wavelet_mask_lo_mad = 1.0;
   double wavelet_mask_hi_mad = 3.0;
};

// Core stretch functions
float vl_sorted_percentile( const std::vector<float>& sorted, float p );
float vl_hyperbolic( float v, float D, float b );
float vl_mtf( float v, float m );
float vl_soft_clip( float v, float threshold = 0.98f, float rolloff = 2.0f );
float vl_anchor_stats( const Image& img, const double weights[3] );
float vl_anchor_adaptive( const Image& img, const double weights[3] );
double vl_solve_log_d( float medianIn, float targetMedian, float protectB );
void vl_adaptive_scaling( Image& img, const double weights[3], float targetBg );
void VeraLuxStretch( Image& img, const VeraLuxParams& p );

// Wavelet (starlet) decomposition and enhancement
struct WaveletLayers
{
   std::vector<std::vector<float>> detail;
   std::vector<float> residual;
};
WaveletLayers starlet_decompose( const std::vector<float>& data, int w, int h, int nLayers );
std::vector<float> starlet_reconstruct( const WaveletLayers& wl, const std::vector<float>& gain, float residualScale );
void WaveletEnhance( Image& img, const VeraLuxParams& p );
