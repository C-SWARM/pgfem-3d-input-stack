/*

              ####### #         ###   #     #   ###   ######
              #       #          #     #   #     #    #     #
              #       #          #      # #      #    #     #
              #####   #          #       #       #    ######
              #       #          #      # #      #    #   #
              #       #          #     #   #     #    #    #
              ####### #######   ###   #     #   ###   #     #

                   
                  Copyright: 1994 Petr Krysl

   Czech Technical University in Prague, Faculty of Civil Engineering,
      Dept. Structural Mechanics, 166 29 Prague, Czech Republic,
                  email: pk@power2.fsv.cvut.cz
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


#ifndef ETYPES_H
#define ETYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include "Econfig.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>

#include "listP.h"		/* C-toolkit */
#include "hashP.h"		/* C-toolkit */
#include "vectP.h"		/* C-toolkit */
#include "timeP.h"		/* C-toolkit */

#ifndef FILENAME_MAX    /* POSIX constant */
#  define FILENAME_MAX 256
#endif

#ifdef MAXFILENAMECHAR
#  undef MAXFILENAMECHAR
#endif
#if FILENAME_MAX < 256
#   define MAXFILENAMECHAR 256
#else
#   define MAXFILENAMECHAR FILENAME_MAX /* POSIX constant */
#endif

#ifndef LONG_MAX /* no POSIX constants: define them just to have something */
#define	CHAR_BIT	(8)
#define CHAR_MAX	UCHAR_MAX
#define CHAR_MIN	(0)
#define INT_MAX		(2147483647)
#define	INT_MIN		(-(INT_MAX + 1))
#define LONG_MAX	INT_MAX
#define LONG_MIN	INT_MIN
#define SCHAR_MAX	(127)
#define SCHAR_MIN	(-(SCHAR_MAX + 1))
#define SHRT_MAX	(32767)
#define SHRT_MIN	(-(SHRT_MAX + 1))
#define UCHAR_MAX	(255)
#define UINT_MAX	(4294967295)
#define ULONG_MAX	(UINT_MAX)
#define USHRT_MAX	(65535)
#define	MB_LEN_MAX	4 		/* maximum bytes in multibyte char */
#endif

typedef unsigned long EPixel;

typedef struct ERGBColor {
  double R, G, B;
}               ERGBColor;


typedef struct EHSVColor {
  double H, S, V;
}               EHSVColor;

typedef enum { COLOR_APPROX_SPACE, COLOR_APPROX_RANGE } EColorApproxType;

typedef struct EColorApproxRec {
    EColorApproxType    type;
    unsigned short      max1;
    unsigned short      max2;
    unsigned short      max3;
    unsigned long       mult1;
    unsigned long       mult2;
    unsigned long       mult3;
    float               weight1;
    float               weight2;
    float               weight3;
    unsigned long       base_pixel;
} EColorApproxRec;

typedef double FPNum;

/* WARNING: Some routines rely on WCFPNum and VCFPNum being of the same */
/* precision!!! Don't be easy-going about it. */

typedef  FPNum WCFPNum; 

typedef  FPNum VCFPNum;

#define WC2DRec CkitVector2

#define WCRec CkitVector3

typedef struct VCRec {
  VCFPNum  u;
  VCFPNum  v;
  VCFPNum  n;
}                                        VCRec;	/* View coordinates */

typedef struct VC2DRec {
  VCFPNum  u;
  VCFPNum  v;
}                                        VC2DRec;	/* View coordinates */

#ifndef YES
#define NO  FALSE
#define YES TRUE
#endif

#ifndef HAVE_BOOLEAN
typedef int BOOLEAN;
#endif


typedef char EBool;

typedef struct DCRec {
  int  x;
  int  y;
}                                           DCRec; /* Device coordinates */

/*             what it means when the test object (most often */
/*             a box): */

typedef enum {   OVERLAP, /* the box overlaps */
			  /* the convex envelope of the graphic */
		 INSIDE,  /* the convex envelope is inside of the */
			  /* test box */
		 INTERSECT /* the test box boundary intersects the */
			   /* boundary or the path of the graphic */
		   
             } ESelectCriteria;


typedef FPNum *EFringeTable;

typedef enum { NORMAL_RENDERING        = 0, /* Default -- each graphic */
					    /* chooses its own way. */
	       FILLED_HIDDEN_RENDERING = 1, /* Just hidden lines/surfaces */
	       CONST_SHADING_RENDERING = 2, /* Shading (dithering ...) */
	       WIRE_RENDERING          = 3  /* Lines only */
	     } ERenderingType;

typedef enum { NO_SHADING              = 0,
	       DITHER_SHADING          = 1,
	       COLOR_SHADING           = 2
	     } EShadingType;

typedef enum { DRAW_MODE_DRAW     = 0, 
	       DRAW_MODE_ERASE    = 1,
	       DRAW_MODE_HILITE   = 2,
	       DRAW_MODE_UNHILITE = 3,
	       DRAW_MODE_XORDRAW  = 4
	     } EDrawMode;

typedef enum { VIEW_ORIENT_TOP    = 0,
               VIEW_ORIENT_BOTTOM = 1,
               VIEW_ORIENT_LEFT   = 2,
               VIEW_ORIENT_RIGHT  = 3,
               VIEW_ORIENT_FRONT  = 4,
               VIEW_ORIENT_BACK   = 5,
	       VIEW_ORIENT_ISO    = 6
} EViewOrientation;

#define PI 3.1415927

#include "Eutils.h"

#ifdef __cplusplus
}
#endif

#endif
