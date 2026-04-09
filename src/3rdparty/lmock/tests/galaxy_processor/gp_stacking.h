// gp_stacking.h -- Stacking frontend (LRGB and RGB)
#pragma once

#include "gp_common.h"

// Utility: case-insensitive string suffix check
bool EndsWithCI( const std::string& s, const char* suffix );

// Convert between OpenCV Mat and PCL Image
Image CvMatToPCLImage( const cv::Mat& mat );
cv::Mat PCLImageToCvMat( const Image& img );

// Stack a single channel from FITS/XISF files
cv::Mat StackChannel( const QStringList& files, const StackParams& params, const char* channelName );

// Stack LRGB from multiple FITS/XISF files
Image StackFramesLRGB( const QStringList& files, const StackParams& params );

// Stack RGB from multiple FITS/XISF files
Image StackFramesRGB( const QStringList& files, const StackParams& params );
