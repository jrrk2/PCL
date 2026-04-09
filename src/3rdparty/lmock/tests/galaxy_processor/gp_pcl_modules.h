// gp_pcl_modules.h -- Integration wrappers for open-source PCL process modules
//
// Each function instantiates the module's *Instance class, sets parameters,
// and calls ExecuteOn() on the image.
#pragma once

#include "gp_common.h"

// HistogramTransformation: midtones transfer function (MTF) stretch
// m = midtones balance (0-1, default 0.5 = no change)
// shadows = black point clipping (0-1)
// highlights = white point clipping (0-1)
void ApplyHistogramTransformation( Image& image, float midtones = 0.5f,
                                   float shadows = 0.0f, float highlights = 1.0f );

// CurvesTransformation: apply curve to each channel
// curves[c] is a vector of {x,y} control points for channel c (0=R,1=G,2=B,3=RGB combined)
// If curves[3] is provided, it's applied to all channels
struct CurvePoint { double x, y; };
void ApplyCurvesTransformation( Image& image,
                                const std::vector<CurvePoint>& rgbCurve );

// UnsharpMask: sharpen via unsharp masking
// sigma = Gaussian blur sigma (pixels)
// amount = strength (0-1, typical 0.5)
void ApplyUnsharpMask( Image& image, float sigma = 2.0f, float amount = 0.5f );

// MorphologicalTransformation: erosion, dilation, median, etc.
// op: 0=erosion, 1=dilation, 2=opening, 3=closing, 4=median
// size = structuring element radius in pixels
void ApplyMorphologicalTransformation( Image& image, int op = 4, int size = 1 );

// AutoHistogram: automatic histogram stretch
void ApplyAutoHistogram( Image& image );

// Convolution: general convolution with custom kernel
void ApplyConvolution( Image& image, float sigma = 2.0f );

// Resample: resize image by a scale factor
void ApplyResample( Image& image, double scaleFactor );

// Crop: crop image to specified rectangle
void ApplyCrop( Image& image, int left, int top, int right, int bottom );

// Rotation: rotate image by angle in degrees
void ApplyRotation( Image& image, double angleDeg );

// PixelMath: evaluate a pixel math expression
// expr = expression like "$T * 0.5 + 0.1" where $T = current pixel
void ApplyPixelMath( Image& image, const std::string& expr );

// Register all PCL module functions in a Lua state's galaxy table
struct lua_State;
void RegisterPCLModulesLua( lua_State* L );
