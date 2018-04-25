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

#include "Econfig.h"
#include "Ezbuffer.h"
#include "Edither.h"
#include "Ecolors.h"
#include "Eutils.h"
#include "Ereprts.h"
#include <stdlib.h>


static ZBuffCellRec *zbuf = NULL;
static int max_buf_row = 0, max_buf_col = 0;
static EPixel the_background = 0x0;
static EView *inited_for_v_p = NULL;
static unsigned long time_to_init, time_to_write, time_to_paint;

#define ZBUF_PTR_AT(_X, _Y) (zbuf + (_Y) * max_buf_col + (_X))


#undef TIME_ZBUFFERING

/* ========================================================================= */

BOOLEAN 
ZBufferInit(EView *v_p, int maxcol, int maxrow, EPixel background)
{ 
  int i, n;
  ZBuffCellRec *ptr;

  if (   (zbuf == NULL)
      || (maxrow*maxcol > max_buf_row*max_buf_col)) { /* alloc z-buffer */
    if (zbuf != NULL) {  
      zbuf = (ZBuffCellRec *)
        realloc((void *)zbuf, (size_t)sizeof(ZBuffCellRec) * (maxrow*maxcol));
    } else {             
      zbuf = (ZBuffCellRec *)
        malloc(sizeof(ZBuffCellRec) * (size_t)(maxrow*maxcol));
    }
    if (zbuf == NULL) {
      ERptErrMessage(ELIXIR_ERROR_CLASS, 4, ELIXIR_ERROR_4, ERROR_GRADE);
      fprintf(stderr, "InitZBuffer failed. Not enough memory");
      return NO;
    }
  }
  max_buf_row    = maxrow;
  max_buf_col    = maxcol;
  the_background = background;
  inited_for_v_p = v_p;
  
#ifdef TIME_ZBUFFERING
  msec_timer(YES);
#endif
  for (ptr = zbuf, n = max_buf_row * max_buf_col, i = 0; i < n; i++) {
    ptr->pixel   = the_background;
    ptr->n_coord = -FLT_MAX;
    ptr++; 
  }
  
  DitherInit(SMALLEST_COS_IN_DITHER, the_background);
  ColorShadeInit(SMALLEST_COS_IN_DITHER);
  
#ifdef TIME_ZBUFFERING
  time_to_init = msec_timer(NO);
  msec_timer(YES);
#endif
  return YES;
}



void 
ZBufferWritePixel(int x, int y, double n, EPixel pixel)
{
  if (x < 0 || x > max_buf_col-1 ||
      y < 0 || y > max_buf_row-1)
    return;

  if (ZBUF_PTR_AT(x, y)->n_coord < n) {
    ZBUF_PTR_AT(x, y)->n_coord = n;
    ZBUF_PTR_AT(x, y)->pixel   = pixel;
  }
}

float 
ZBufferDepthAt(int x, int y)
{
  if (x < 0 || x > max_buf_col-1 || y < 0 || y > max_buf_row-1)
    return 0;
  else
    return ZBUF_PTR_AT(x, y)->n_coord;
}

 

void 
ZBufferWriteRow(int y,
		int start_x, int end_x, 
		double start_n, double end_n, EPixel pixel)
{
  int xs, xe, dx;
  double ns, ne, dn, n;
  int col;

  if (y < 0 || y > max_buf_row-1)
    return;
  
  if (start_x > end_x)
    xs = end_x, xe = start_x, ns = end_n, ne = start_n;
  else
    xe = end_x, xs = start_x, ne = end_n, ns = start_n;

	dx = xe - xs;
  if (xs < 0) {
		if(dx == 0)return;
    ns = ns - xs * (ne - ns)/dx;
    xs = 0;
		dx = xe;
  }
  if (xe > max_buf_col-1) {
		if(dx == 0)return;
    ne = ns + (max_buf_col-1 - xs)*(ne - ns)/dx;
    xe = max_buf_col-1;
		dx = xe - xs;
  }
  if (dx == 0) {
    n = min(start_n, end_n);
    if (ZBUF_PTR_AT(xs, y)->n_coord < n) {
      ZBUF_PTR_AT(xs, y)->n_coord = n;
      ZBUF_PTR_AT(xs, y)->pixel   = pixel;
    }
  } else {
    dn = (ne - ns)/dx;
    n = ns;
    for (col = xs; col <= xe; col++) {
      if (ZBUF_PTR_AT(col, y)->n_coord < n) {
	ZBUF_PTR_AT(col, y)->n_coord = n;
	ZBUF_PTR_AT(col, y)->pixel   = pixel;
      }
      n += dn;
    }
  }
}





void 
ZBufferWriteRowColorInterp(int y,
                           int start_x, int end_x, 
                           double start_n, double end_n,
                           double start_v, double end_v, EFringeTable ft)
{
  int xs, xe, dx;
  double ns, ne, dn, n, vs, ve, dv, v;
  int col;

  if (y < 0 || y > max_buf_row-1)
    return;
  
  if (start_x > end_x) {
    xs = end_x; xe = start_x;
    ns = end_n; ne = start_n;
    vs = end_v; ve = start_v;
  } else {
    xe = end_x; xs = start_x;
    ne = end_n; ns = start_n;
    ve = end_v; vs = start_v;
  }

	dx = xe - xs;
  if (xs < 0) {
		if(dx == 0)return;
    ns    = ns - xs * (ne - ns)/dx;
    vs    = vs - xs * (ve - vs)/dx;
    xs    = 0;
		dx    = xe;
  }
  if (xe > max_buf_col-1) {
		if(dx == 0)return;
    ne    = ns + (max_buf_col-1 - xs)*(ne - ns)/dx;
    ve    = vs + (max_buf_col-1 - xs)*(ve - vs)/dx;
    xe = max_buf_col-1;
		dx = xe - xs;
  }
  if (dx == 0) {
    n = min(start_n, end_n);
    v = min(start_v, end_v);
    if (ZBUF_PTR_AT(xs, y)->n_coord < n) {
      ZBUF_PTR_AT(xs, y)->n_coord = n;
      ZBUF_PTR_AT(xs, y)->pixel   = ColorFringeValueToColorHSV(ft, v);
    }
  } else {
    dn = (ne - ns)/dx;
    dv = (ve - vs)/dx;
    n = ns;
    v = vs;
    for (col = xs; col <= xe; col++) {
      if (ZBUF_PTR_AT(col, y)->n_coord < n) {
	ZBUF_PTR_AT(col, y)->n_coord = n;
	ZBUF_PTR_AT(col, y)->pixel   = ColorFringeValueToColorHSV(ft, v);
      }
      n += dn;
      v += dv;
    }
  }
}




void 
ZBufferWriteRowShadedColorInterp(int y,
                                 int start_x, int end_x, 
                                 double start_n, double end_n,
                                 double start_v, double end_v,
                                 EFringeTable ft, FPNum cosine)
{
  int xs, xe, dx;
  double ns, ne, dn, n, vs, ve, dv, v;
  int col;

  if (y < 0 || y > max_buf_row-1)
    return;
  
  if (start_x > end_x) {
    xs = end_x; xe = start_x;
    ns = end_n; ne = start_n;
    vs = end_v; ve = start_v;
  } else {
    xe = end_x; xs = start_x;
    ne = end_n; ns = start_n;
    ve = end_v; vs = start_v;
  }

	dx = xe - xs;
  if (xs < 0) {
		if(dx == 0)return;
    ns    = ns - xs * (ne - ns)/dx;
    vs    = vs - xs * (ve - vs)/dx;
    xs    = 0;
		dx    = xe;
  }
  if (xe > max_buf_col-1) {
		if(dx == 0)return;
    ne    = ns + (max_buf_col-1 - xs)*(ne - ns)/dx;
    ve    = vs + (max_buf_col-1 - xs)*(ve - vs)/dx;
    xe = max_buf_col-1;
		dx = xe - xs;
  }
  if (dx == 0) {
    n = min(start_n, end_n);
    v = min(start_v, end_v);
    if (ZBUF_PTR_AT(xs, y)->n_coord < n) {
      ZBUF_PTR_AT(xs, y)->n_coord = n;
      ZBUF_PTR_AT(xs, y)->pixel
        = ColorShadeColor(ColorFringeValueToColorHSV(ft, v), cosine);
    }
  } else {
    dn = (ne - ns)/dx;
    dv = (ve - vs)/dx;
    n = ns;
    v = vs;
    for (col = xs; col <= xe; col++) {
      if (ZBUF_PTR_AT(col, y)->n_coord < n) {
	ZBUF_PTR_AT(col, y)->n_coord = n;
	ZBUF_PTR_AT(col, y)->pixel
          = ColorShadeColor(ColorFringeValueToColorHSV(ft, v), cosine);
      }
      n += dn;
      v += dv;
    }
  }
}



void 
ZBufferWriteRowDitheredColorInterp(int y,
                                   int start_x, int end_x, 
                                   double start_n, double end_n,
                                   double start_v, double end_v,
                                   EFringeTable ft, FPNum cosine)
{
  int xs, xe, dx;
  double ns, ne, dn, n, vs, ve, dv, v;
  int col;

  if (y < 0 || y > max_buf_row-1)
    return;
  
  if (start_x > end_x) {
    xs = end_x; xe = start_x;
    ns = end_n; ne = start_n;
    vs = end_v; ve = start_v;
  } else {
    xe = end_x; xs = start_x;
    ne = end_n; ns = start_n;
    ve = end_v; vs = start_v;
  }

	dx = xe - xs;
  if (xs < 0) {
		if(dx == 0)return;
    ns    = ns - xs * (ne - ns)/dx;
    vs    = vs - xs * (ve - vs)/dx;
    xs    = 0;
		dx    = xe;
  }
  if (xe > max_buf_col-1) {
		if(dx == 0)return;
    ne    = ns + (max_buf_col-1 - xs)*(ne - ns)/dx;
    ve    = vs + (max_buf_col-1 - xs)*(ve - vs)/dx;
    xe = max_buf_col-1;
    dx = xe - xs;
  }
  if (dx == 0) {
    n = min(start_n, end_n);
    v = min(start_v, end_v);
    if (ZBUF_PTR_AT(xs, y)->n_coord < n) {
      ZBUF_PTR_AT(xs, y)->n_coord = n;
      ZBUF_PTR_AT(xs, y)->pixel =
        DitherColorValue(cosine, ColorFringeValueToColorHSV(ft, v));
    }
  } else {
    dn = (ne - ns)/dx;
    dv = (ve - vs)/dx;
    n = ns;
    v = vs;
    for (col = xs; col <= xe; col++) {
      if (ZBUF_PTR_AT(col, y)->n_coord < n) {
	ZBUF_PTR_AT(col, y)->n_coord = n;
	ZBUF_PTR_AT(col, y)->pixel =
          DitherColorValue(cosine, ColorFringeValueToColorHSV(ft, v));
      }
      n += dn;
      v += dv;
    }
  }
}



void 
ZBufferWriteDitheredRow(int y,
			int start_x, int end_x, 
			double start_n, double end_n, EPixel pixel,
			FPNum cos_)
{
  int xs, xe, dx;
  double ns, ne, dn, n;
  int col;

  if (y < 0 || y > max_buf_row-1)
    return;
  
  if (start_x > end_x)
    xs = end_x, xe = start_x, ns = end_n, ne = start_n;
  else
    xe = end_x, xs = start_x, ne = end_n, ns = start_n;

	dx = xe - xs;
  if (xs < 0) {
		if(dx == 0)return;
    ns = ns - xs * (ne - ns)/dx;
    xs = 0;
		dx = xe;
  }
  if (xe > max_buf_col-1) {
		if(dx == 0)return;
    ne = ns + (max_buf_col-1 - xs)*(ne - ns)/dx;
    xe = max_buf_col-1;
		dx = xe - xs;
  }
  if (dx == 0) {
    n = min(start_n, end_n);
    if (ZBUF_PTR_AT(xs, y)->n_coord < n) {
      ZBUF_PTR_AT(xs, y)->n_coord = n;
      ZBUF_PTR_AT(xs, y)->pixel   = DitherColorValue(cos_, pixel);
    }
  } else {
    dn = (ne - ns)/dx;
    n = ns;
    for (col = xs; col <= xe; col++) {
      if (ZBUF_PTR_AT(col, y)->n_coord < n) {
	ZBUF_PTR_AT(col, y)->n_coord = n;
        ZBUF_PTR_AT(col, y)->pixel   = DitherColorValue(cos_, pixel);
      }
      n += dn;
    }
  }
}

void 
ZBufferPaint(Display *display, Drawable drawable, GC gc)
{
  int row, col;
  ZBuffCellRec *ptr;
  EPixel pixel;
  static EPixel previous_pixel;

#ifdef TIME_ZBUFFERING
  time_to_write = msec_timer(NO);
  msec_timer(YES);
#endif
  XSetLineAttributes(display, gc, (unsigned)0, LineSolid, CapButt, JoinMiter);
  previous_pixel = the_background;
  XSetForeground(display, gc, previous_pixel);
  XFillRectangle(display, drawable, gc, 0, 0, max_buf_col, max_buf_row);
  for (row = 0; row < max_buf_row; row++) {
    col = 0;
    ptr = ZBUF_PTR_AT(col, row);
    while (col++ < max_buf_col) {
      pixel = ptr->pixel;
      if (pixel != the_background)  {
        int start = col;
        
        if (previous_pixel != pixel) {
          XSetForeground(display, gc, pixel);
          previous_pixel = pixel;
        }
        
        while ((++ptr)->pixel == pixel)
          if (++col > max_buf_col)
            break;
        ptr--;
        if (start == col)
          XDrawPoint(display, drawable, gc, col, row);
        else 
          XDrawLine(display, drawable, gc, start, row, col, row);
      }
      ptr++; 
    }
  }
#ifdef TIME_ZBUFFERING
  fprintf(stderr,
          "ZBuffer (I %lu, W %lu, P %lu, T %lu [msec])\n",
          time_to_init, time_to_write, (time_to_paint = msec_timer(NO)),
          time_to_init + time_to_write + time_to_paint);
#endif 
}

EView *
ZBufferInitedFor(void)
{
  return inited_for_v_p;
}
