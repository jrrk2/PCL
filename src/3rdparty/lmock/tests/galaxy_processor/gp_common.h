// gp_common.h -- Common includes and forward declarations for galaxy_processor
#pragma once

// OpenCV must be included before PCL headers. PCL's APIDefs.h has
// 'using namespace pcl;' which leaks pcl::int64/uint64 into the global
// namespace, conflicting with OpenCV's global typedefs of the same names.
#include "stackengine_cli.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <pcl/XISF.h>
#include <pcl/WCSKeywords.h>
#include <pcl/AstrometricMetadata.h>
#include <pcl/LinearTransformation.h>
#include <pcl/MetaModule.h>
#include <pcl/StarDetector.h>
#include <pcl/GaiaDatabaseFile.h>
#include <pcl/GnomonicProjection.h>
#include <pcl/SurfaceSpline.h>
#include <pcl/ImageVariant.h>

#include "FITS/FITS.h"

#include <fitsio.h>
#include <stellarsolver.h>

#include "lmock/PCLMockAPI.h"
#include <pcl/api/APIInterface.h>

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <numeric>
#include <memory>
#include <sstream>
#include <random>
#include <map>
#include <sys/statvfs.h>
#include <QCoreApplication>
#include <QDir>
#include <QImage>

using namespace pcl;
