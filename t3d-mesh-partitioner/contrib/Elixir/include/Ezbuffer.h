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


#ifndef EZBUFFER_H
#define EZBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Ecolors.h"
#include "Egraphic.h"

typedef struct ZBuffCellRec {
  EPixel  pixel;
  float        n_coord;
} ZBuffCellRec;

#define MAX_ZBUF_DIM 1024
#define  EMPTY_PIXEL 0x0

void 
ZBufferWritePixel(int x, int y, double n, EPixel pixel);

void
ZBufferWriteRow(int y,
                int start_x, int end_x, 
                double start_n, double end_n, EPixel pixel);

void 
ZBufferWriteDitheredRow(int y,
                        int start_x, int end_x, 
                        double start_n, double end_n, EPixel pixel,
                        FPNum cos_);

void 
ZBufferWriteRowColorInterp(int y,
                           int start_x, int end_x, 
                           double start_n, double end_n,
                           double start_v, double end_v, EFringeTable ft);

void 
ZBufferWriteRowDitheredColorInterp(int y,
                                   int start_x, int end_x, 
                                   double start_n, double end_n,
                                   double start_v, double end_v,
                                   EFringeTable ft,
                                   FPNum cos_);

void 
ZBufferWriteRowShadedColorInterp(int y,
                                 int start_x, int end_x, 
                                 double start_n, double end_n,
                                 double start_v, double end_v,
                                 EFringeTable ft, FPNum cosine);

void 
ZBufferPaint(Display *display, Drawable drawable, GC gc);

BOOLEAN 
ZBufferInit(EView *v_p, int maxcol, int maxrow, EPixel background);

EView *
ZBufferInitedFor(void);

float 
ZBufferDepthAt(int x, int y);

#ifdef __cplusplus
}
#endif

#endif
