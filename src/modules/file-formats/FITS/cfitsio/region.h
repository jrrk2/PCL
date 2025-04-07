//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.3
// ----------------------------------------------------------------------------
// region.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

/***************************************************************/
/*                   REGION STUFF                              */
/***************************************************************/

#include "fitsio.h"
#define myPI  3.1415926535897932385
#define RadToDeg 180.0/myPI

typedef struct {
   int    exists;
   double xrefval, yrefval;
   double xrefpix, yrefpix;
   double xinc,    yinc;
   double rot;
   char   type[6];
} WCSdata;

typedef enum {
   point_rgn,
   line_rgn,
   circle_rgn,
   annulus_rgn,
   ellipse_rgn,
   elliptannulus_rgn,
   box_rgn,
   boxannulus_rgn,
   rectangle_rgn,
   diamond_rgn,
   sector_rgn,
   poly_rgn,
   panda_rgn,
   epanda_rgn,
   bpanda_rgn
} shapeType;

typedef enum { pixel_fmt, degree_fmt, hhmmss_fmt } coordFmt;

typedef struct {
   char      sign;        /*  Include or exclude?        */
   shapeType shape;       /*  Shape of this region       */
   int       comp;        /*  Component number for this region */

   double xmin,xmax;       /*  bounding box    */
   double ymin,ymax;

   union {                /*  Parameters - In pixels     */

      /****   Generic Shape Data   ****/

      struct {
	 double p[11];       /*  Region parameters       */
	 double sinT, cosT;  /*  For rotated shapes      */
	 double a, b;        /*  Extra scratch area      */
      } gen;

      /****      Polygon Data      ****/

      struct {
         int    nPts;        /*  Number of Polygon pts   */
         double *Pts;        /*  Polygon points          */
      } poly;

   } param;

} RgnShape;

typedef struct {
   int       nShapes;
   RgnShape  *Shapes;
   WCSdata   wcs;
} SAORegion;

/*  SAO region file routines */
int  fits_read_rgnfile( const char *filename, WCSdata *wcs, SAORegion **Rgn, int *status );
int  fits_in_region( double X, double Y, SAORegion *Rgn );
void fits_free_region( SAORegion *Rgn );
void fits_set_region_components ( SAORegion *Rgn );
void fits_setup_shape ( RgnShape *shape);
int fits_read_fits_region ( fitsfile *fptr, WCSdata * wcs, SAORegion **Rgn, int *status);
int fits_read_ascii_region ( const char *filename, WCSdata * wcs, SAORegion **Rgn, int *status);

// ----------------------------------------------------------------------------
// EOF region.h - Released 2025-04-07T08:53:45Z
