//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// jconfig.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define HAVE_PROTOTYPES 1
#define HAVE_UNSIGNED_CHAR 1
#define HAVE_UNSIGNED_SHORT 1
#undef CHAR_IS_UNSIGNED
#define HAVE_STDDEF_H 1
#define HAVE_STDLIB_H 1
//#define HAVE_LOCALE_H 1
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS
#undef NEED_SHORT_EXTERNAL_NAMES
#undef INCOMPLETE_TYPES_BROKEN

/* Define "boolean" as unsigned char, not enum, on Windows systems. */
#ifdef _WIN32
#ifndef __RPCNDR_H__		/* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif
#define HAVE_BOOLEAN		/* prevent jmorecfg.h from redefining it */
#endif

#ifdef JPEG_INTERNALS

#undef RIGHT_SHIFT_IS_UNSIGNED

#ifdef _WIN32
#define INLINE __inline
#else
#define INLINE __inline__
#endif

#endif /* JPEG_INTERNALS */

// ----------------------------------------------------------------------------
// EOF jconfig.h - Released 2025-04-07T08:53:45Z
