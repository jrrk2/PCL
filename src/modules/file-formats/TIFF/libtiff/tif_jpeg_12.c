//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 1.0.10
// ----------------------------------------------------------------------------
// tif_jpeg_12.c - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "tiffiop.h"

#if defined(HAVE_JPEGTURBO_DUAL_MODE_8_12)
#define JPEG_DUAL_MODE_8_12
#endif

#if defined(JPEG_DUAL_MODE_8_12)

#define FROM_TIF_JPEG_12

#ifdef TIFFInitJPEG
#undef TIFFInitJPEG
#endif
#define TIFFInitJPEG TIFFInitJPEG_12

#ifdef TIFFJPEGIsFullStripRequired
#undef TIFFJPEGIsFullStripRequired
#endif
#define TIFFJPEGIsFullStripRequired TIFFJPEGIsFullStripRequired_12

int TIFFInitJPEG_12(TIFF *tif, int scheme);

#if !defined(HAVE_JPEGTURBO_DUAL_MODE_8_12)
#include LIBJPEG_12_PATH
#endif

#include "tif_jpeg.c"

int TIFFReInitJPEG_12(TIFF *tif, const JPEGOtherSettings *otherSettings,
                      int scheme, int is_encode)
{
    JPEGState *sp;
    uint8_t *new_tif_data;

    (void)scheme;
    assert(scheme == COMPRESSION_JPEG);

    new_tif_data =
        (uint8_t *)_TIFFreallocExt(tif, tif->tif_data, sizeof(JPEGState));

    if (new_tif_data == NULL)
    {
        TIFFErrorExtR(tif, "TIFFReInitJPEG_12",
                      "No space for JPEG state block");
        return 0;
    }

    tif->tif_data = new_tif_data;
    _TIFFmemset(tif->tif_data, 0, sizeof(JPEGState));

    TIFFInitJPEGCommon(tif);

    sp = JState(tif);
    sp->otherSettings = *otherSettings;

    if (is_encode)
        return JPEGSetupEncode(tif);
    else
        return JPEGSetupDecode(tif);
}

#endif /* defined(JPEG_DUAL_MODE_8_12) */

// ----------------------------------------------------------------------------
// EOF tif_jpeg_12.c - Released 2025-04-07T08:53:45Z
