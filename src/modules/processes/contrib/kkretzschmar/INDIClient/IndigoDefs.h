//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// IndigoDefs.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __IndigoDefs_h
#define __IndigoDefs_h

#include <pcl/Defs.h>

#ifdef __PCL_FREEBSD
# define INDIGO_FREEBSD 1
#endif

#ifdef __PCL_LINUX
# define INDIGO_LINUX 1
#endif

#if defined( __PCL_MACOS ) || defined( __PCL_MACOSX )
# define INDIGO_MACOS 1
#endif

#ifdef __PCL_WINDOWS
# define INDIGO_WINDOWS 1
#endif

#endif // __IndigoDefs_h

// ----------------------------------------------------------------------------
// EOF IndigoDefs.h - Released 2025-04-07T08:53:56Z
