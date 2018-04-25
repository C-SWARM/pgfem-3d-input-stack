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
#include "Eview.h"
#include "Ezbuffer.h"
#include "Edither.h"
#include "Etransfm.h"
#include "Ebezier.h"
#include "Eactset.h"
#include "Ecolors.h"
#include "Eprimtv.h"

/* ======================================================================== */
/* PRIVATE   */
/* ======================================================================== */

/* exported to primitives */
Display        *EV_display;
Window          EV_curr_win;
GC              EV_curr_GC;
EPixel          EV_curr_pixel;
FPNum           EV_curr_cosine;
ERenderingType  EV_render_mode;
EShadingType    EV_shade_mode;
BOOLEAN         EV_should_clip_by_view_planes = NO;
double          EV_fg_plane_depth =  FLT_MAX;
double          EV_bg_plane_depth = -FLT_MAX;

#define VIEW_CLIP()    (EV_should_clip_by_view_planes)
#define FG_N()         (EV_fg_plane_depth)
#define BG_N()         (EV_bg_plane_depth)
#define IS_INSIDE(DEPTH) ((DEPTH < FG_N()) && (DEPTH > BG_N()))

#define Swap(v1, v2, tmp) tmp = v1; v1 = v2; v2 = tmp

#define WZBDR(_Y, _START_x, _END_x, _START_n, _END_n, _PIXEL, _COS_) \
     ZBufferWriteDitheredRow((int)_Y, (int)_START_x, \
                             (int)_END_x, (double)_START_n, (double)_END_n, \
                             (EPixel)_PIXEL, (FPNum)_COS_)

#define WZBR(_Y, _START_x, _END_x, _START_n, _END_n, _PIXEL) \
     ZBufferWriteRow((int)_Y, (int)_START_x, (int)_END_x, (double)_START_n, \
                     (double)_END_n, (EPixel)_PIXEL)

#define WZBRCI(_Y, _START_x, _END_x, _START_n, _END_n, _START_v, _END_v, _FT) \
     ZBufferWriteRowColorInterp((int)_Y, (int)_START_x, (int)_END_x,          \
                     (double)_START_n, (double)_END_n,                        \
                     (double)_START_v, (double)_END_v, (_FT))

#define WZBDRCI(_Y, _START_x, _END_x, _START_n, _END_n, _START_v,             \
                _END_v, _FT, _COS)                                            \
     ZBufferWriteRowDitheredColorInterp((int)_Y, (int)_START_x, (int)_END_x,  \
                     (double)_START_n, (double)_END_n,                        \
                     (double)_START_v, (double)_END_v, (_FT), (_COS))

#define WZBSRCI(_Y, _START_x, _END_x, _START_n, _END_n, _START_v,             \
                _END_v, _FT, _COS)                                            \
     ZBufferWriteRowShadedColorInterp((int)_Y, (int)_START_x, (int)_END_x,    \
                     (double)_START_n, (double)_END_n,                        \
                     (double)_START_v, (double)_END_v, (_FT), (_COS))

#define WZB(_X, _Y, _N, _PIXEL) \
     ZBufferWritePixel((int)_X, (int)_Y, (double)_N, (EPixel)_PIXEL)


#define VIEW_CLIPPING_WZBR(ROW, SCOL, ECOL, SN, EN)                          \
  {                                                                          \
    BOOLEAN draw = NO;                                                       \
    int theSCol = SCOL, theECol = ECOL;                                      \
    double theSN = SN, theEN = EN;                                           \
    if (theSN > theEN) {                                                     \
      double TMP;                                                            \
      TMP = theSN; theSN = theEN; theEN = TMP;                               \
      TMP = theSCol; theSCol = theECol; theECol = TMP;                       \
    }                                                                        \
    if (theSN >= BG_N() && theEN <= FG_N()) {                                \
      draw = YES;                                                            \
    } else {                                                                 \
      if (   (theSN < BG_N() && theEN < BG_N())                              \
          || (theSN > FG_N() && theEN > FG_N())) {                           \
        draw = NO;                                                           \
      } else {                                                               \
        draw = YES;                                                          \
        if (theSN < BG_N()) {                                                \
          theSCol = (theECol - (theEN - BG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          theSN   = BG_N();                                                  \
        }                                                                    \
        if (theEN > FG_N()) {                                                \
          theECol = (theECol - (theEN - FG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          theEN   = FG_N();                                                  \
        }                                                                    \
      }                                                                      \
    }                                                                        \
    if (draw)                                                                \
      WZBR(ROW, theSCol, theECol, theSN, theEN, EV_curr_pixel);              \
  }


#define VIEW_CLIPPING_WZBDR(ROW, SCOL, ECOL, SN, EN)                         \
  {                                                                          \
    BOOLEAN draw = NO;                                                       \
    int theSCol = SCOL, theECol = ECOL;                                      \
    double theSN = SN, theEN = EN;                                           \
    if (theSN > theEN) {                                                     \
      double TMP;                                                            \
      TMP = theSN; theSN = theEN; theEN = TMP;                               \
      TMP = theSCol; theSCol = theECol; theECol = TMP;                       \
    }                                                                        \
    if (theSN >= BG_N() && theEN <= FG_N()) {                                \
      draw = YES;                                                            \
    } else {                                                                 \
      if (   (theSN < BG_N() && theEN < BG_N())                              \
          || (theSN > FG_N() && theEN > FG_N())) {                           \
        draw = NO;                                                           \
      } else {                                                               \
        draw = YES;                                                          \
        if (theSN < BG_N()) {                                                \
          theSCol = (theECol - (theEN - BG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          theSN   = BG_N();                                                  \
        }                                                                    \
        if (theEN > FG_N()) {                                                \
          theECol = (theECol - (theEN - FG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          theEN   = FG_N();                                                  \
        }                                                                    \
      }                                                                      \
    }                                                                        \
    if (draw)                                                                \
      WZBDR(ROW, theSCol, theECol, theSN, theEN, EV_curr_pixel,              \
            EV_curr_cosine);                                                 \
  }




void 
Trifill(int x1, int y1, double n1,
	int x2, int y2, double n2,
	int x3, int y3, double n3,
	unsigned int width, unsigned int height)
{
  double xs, xm, xe, ys, ym, ye, ns, nm, ne;
  double srow, scol, erow, ecol, row;
  double sn, en, t1, t2, dn1, dn2;

  if (y1 < y2)
    if (y1 < y3) {
      xs = x1; ys = y1; ns = n1;
      if (y2 < y3) { xm = x2; ym = y2; nm = n2; xe = x3; ye = y3; ne = n3; }
      else         { xm = x3; ym = y3; nm = n3; xe = x2; ye = y2; ne = n2; }
    } else {
      xs = x3; ys = y3; ns = n3;
      xm = x1; ym = y1; nm = n1;
      xe = x2; ye = y2; ne = n2; 
    }
  else
    if (y2 < y3) {
      xs = x2; ys = y2; ns = n2;
      if (y1 < y3) { xm = x1; ym = y1; nm = n1; xe = x3; ye = y3; ne = n3; }
      else         { xm = x3; ym = y3; nm = n3; xe = x1; ye = y1; ne = n1; }
    } else {
      xs = x3; ys = y3; ns = n3;
      xm = x2; ym = y2; nm = n2;
      xe = x1; ye = y1; ne = n1; 
    }

  if (ye - ys == 0) {
    if (EV_shade_mode == DITHER_SHADING) {
      if (VIEW_CLIP()) {
        VIEW_CLIPPING_WZBDR(ye, xs, xe, ns, ne);
      } else {
        WZBDR(ye, xs, xe, ns, ne, EV_curr_pixel, EV_curr_cosine);
      }
    } else {
      if (VIEW_CLIP()) {
        VIEW_CLIPPING_WZBR(ye, xs, xe, ns, ne);
      } else {
        WZBR(ye, xs, xe, ns, ne, EV_curr_pixel);
      }
    }
  } else {
    t2  = (xe - xs)/(ye - ys);
    dn2 = (ne - ns)/(ye - ys);
    if (ym - ys > 0 && ym > 0) {
      t1   = (xm - xs)/(ym - ys);
      dn1  = (nm - ns)/(ym - ys);
      if (ys < 0) {		/* Rough clipping */
				scol = xs + t1 * (-ys); ecol = xs + t2 * (-ys);
				srow = 0;
				sn   = ns + dn1 * (-ys); en   = ns + dn2 * (-ys);
      } else {
				scol = ecol = xs;
				srow = ys;
				sn   = en = ns;
      }
      erow = ym;
      if (ym > height) /* Rough clipping */
				erow = height;
      for (row = srow; row <= erow; row += 1) {
				if (EV_shade_mode == DITHER_SHADING) {
          if (VIEW_CLIP()) {
            VIEW_CLIPPING_WZBDR(row, scol, ecol, sn, en);
          } else {
            WZBDR(row, scol, ecol, sn, en, EV_curr_pixel, EV_curr_cosine);
          }
				} else {
          if (VIEW_CLIP()) {
            VIEW_CLIPPING_WZBR(row, scol, ecol, sn, en);
          } else {
            WZBR(row, scol, ecol, sn, en, EV_curr_pixel);
          }
        }
				scol += t1; ecol += t2;
				sn += dn1;  en += dn2;
      }
    } /* if (ym - ys > 0 && ym > 0) ... */
    if (ye - ym > 0 && ym < height) {
      t1   = (xe - xm)/(ye - ym);
      dn1  = (ne - nm)/(ye - ym);
      if (ym < 0) {		/* Rough clipping */
				scol = xm + t1 * (-ym); ecol = xs + t2 * (-ys);
				srow = 0; erow = ye;
				sn   = nm + dn1 * (-ym); en   = ns + dn2 * (-ys);
      } else {
				scol = xm; ecol = xs + t2 * (ym - ys);
				srow = ym; erow = ye;
				sn   = nm; en   = ns + (ne - ns)/(ye - ys)*(ym - ys);
      }
      if (ye > height)
				erow = height;
      for (row = srow; row < erow; row += 1) {
				if (EV_shade_mode == DITHER_SHADING) {
          if (VIEW_CLIP()) {
            VIEW_CLIPPING_WZBDR(row, scol, ecol, sn, en);
          } else {
            WZBDR(row, scol, ecol, sn, en, EV_curr_pixel, EV_curr_cosine);
          }
				} else {
          if (VIEW_CLIP()) {
            VIEW_CLIPPING_WZBR(row, scol, ecol, sn, en);
          } else {
            WZBR(row, scol, ecol, sn, en, EV_curr_pixel);
          }
        }
				scol += t1; ecol += t2;
				sn += dn1;  en += dn2;
      }
    } /* if (ye - ym > 0 && ym < height) ... */
  } /* else if (ye - ys != 0) ...  */
}



#define VIEW_CLIPPING_WZBRCI(ROW, SCOL, ECOL, SN, EN, SV, EV, FT)            \
  {                                                                          \
    BOOLEAN draw = NO;                                                       \
    int theSCol = SCOL, theECol = ECOL;                                      \
    double theSN = SN, theEN = EN;                                           \
    double theSV = SV, theEV = EV;                                           \
    double theETA;                                                           \
    double TMP;                                                              \
    if (theSN > theEN) {                                                     \
      TMP = theSN; theSN = theEN; theEN = TMP;                               \
      TMP = theSV; theSV = theEV; theEV = TMP;                               \
      TMP = theSCol; theSCol = theECol; theECol = TMP;                       \
    }                                                                        \
    if (theSN >= BG_N() && theEN <= FG_N()) {                                \
      draw = YES;                                                            \
    } else {                                                                 \
      if (   (theSN < BG_N() && theEN < BG_N())                              \
          || (theSN > FG_N() && theEN > FG_N())) {                           \
        draw = NO;                                                           \
      } else {                                                               \
        draw = YES;                                                          \
        if (theSN < BG_N()) {                                                \
          TMP     = (theECol - (theEN - BG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          if (theECol != theSCol)                                            \
             theETA  = (TMP - theSCol) / (theECol - theSCol);                \
          else                                                               \
             theETA  = 0;                                                    \
          theSCol = theSCol * (1 - theETA) + theECol * theETA;               \
          theSV   = theSV   * (1 - theETA) + theEV   * theETA;               \
          theSN   = BG_N();                                                  \
        }                                                                    \
        if (theEN > FG_N()) {                                                \
          TMP     = (theECol - (theEN - FG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          if (theECol != theSCol)                                            \
             theETA  = (TMP - theSCol) / (theECol - theSCol);                \
          else                                                               \
             theETA  = 0;                                                    \
          theECol = theSCol * (1 - theETA) + theECol * theETA;               \
          theEV   = theSV   * (1 - theETA) + theEV   * theETA;               \
          theEN   = FG_N();                                                  \
        }                                                                    \
      }                                                                      \
    }                                                                        \
    if (draw)                                                                \
      WZBRCI(ROW, theSCol, theECol, theSN, theEN, theSV, theEV, FT);         \
  }



#define VIEW_CLIPPING_WZBDRCI(ROW, SCOL, ECOL, SN, EN, SV, EV, FT, COS)      \
  {                                                                          \
    BOOLEAN draw = NO;                                                       \
    int theSCol = SCOL, theECol = ECOL;                                      \
    double theSN = SN, theEN = EN;                                           \
    double theSV = SV, theEV = EV;                                           \
    double theETA;                                                           \
    double TMP;                                                              \
    if (theSN > theEN) {                                                     \
      TMP = theSN; theSN = theEN; theEN = TMP;                               \
      TMP = theSV; theSV = theEV; theEV = TMP;                               \
      TMP = theSCol; theSCol = theECol; theECol = TMP;                       \
    }                                                                        \
    if (theSN >= BG_N() && theEN <= FG_N()) {                                \
      draw = YES;                                                            \
    } else {                                                                 \
      if (   (theSN < BG_N() && theEN < BG_N())                              \
          || (theSN > FG_N() && theEN > FG_N())) {                           \
        draw = NO;                                                           \
      } else {                                                               \
        draw = YES;                                                          \
        if (theSN < BG_N()) {                                                \
          TMP     = (theECol - (theEN - BG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          if (theECol != theSCol)                                            \
             theETA  = (TMP - theSCol) / (theECol - theSCol);                \
          else                                                               \
             theETA  = 0;                                                    \
          theSCol = theSCol * (1 - theETA) + theECol * theETA;               \
          theSV   = theSV   * (1 - theETA) + theEV   * theETA;               \
          theSN   = BG_N();                                                  \
        }                                                                    \
        if (theEN > FG_N()) {                                                \
          TMP     = (theECol - (theEN - FG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          if (theECol != theSCol)                                            \
             theETA  = (TMP - theSCol) / (theECol - theSCol);                \
          else                                                               \
             theETA  = 0;                                                    \
          theECol = theSCol * (1 - theETA) + theECol * theETA;               \
          theEV   = theSV   * (1 - theETA) + theEV   * theETA;               \
          theEN   = FG_N();                                                  \
        }                                                                    \
      }                                                                      \
    }                                                                        \
    if (draw)                                                                \
      WZBDRCI(ROW, theSCol, theECol, theSN, theEN, theSV, theEV, FT, COS);   \
  }

#define VIEW_CLIPPING_WZBSRCI(ROW, SCOL, ECOL, SN, EN, SV, EV, FT, COS)      \
  {                                                                          \
    BOOLEAN draw = NO;                                                       \
    int theSCol = SCOL, theECol = ECOL;                                      \
    double theSN = SN, theEN = EN;                                           \
    double theSV = SV, theEV = EV;                                           \
    double theETA;                                                           \
    double TMP;                                                              \
    if (theSN > theEN) {                                                     \
      TMP = theSN; theSN = theEN; theEN = TMP;                               \
      TMP = theSV; theSV = theEV; theEV = TMP;                               \
      TMP = theSCol; theSCol = theECol; theECol = TMP;                       \
    }                                                                        \
    if (theSN >= BG_N() && theEN <= FG_N()) {                                \
      draw = YES;                                                            \
    } else {                                                                 \
      if (   (theSN < BG_N() && theEN < BG_N())                              \
          || (theSN > FG_N() && theEN > FG_N())) {                           \
        draw = NO;                                                           \
      } else {                                                               \
        draw = YES;                                                          \
        if (theSN < BG_N()) {                                                \
          TMP     = (theECol - (theEN - BG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          if (theECol != theSCol)                                            \
             theETA  = (TMP - theSCol) / (theECol - theSCol);                \
          else                                                               \
             theETA  = 0;                                                    \
          theSCol = theSCol * (1 - theETA) + theECol * theETA;               \
          theSV   = theSV   * (1 - theETA) + theEV   * theETA;               \
          theSN   = BG_N();                                                  \
        }                                                                    \
        if (theEN > FG_N()) {                                                \
          TMP     = (theECol - (theEN - FG_N())                              \
                     / (theEN - theSN) * (theECol - theSCol));               \
          if (theECol != theSCol)                                            \
             theETA  = (TMP - theSCol) / (theECol - theSCol);                \
          else                                                               \
             theETA  = 0;                                                    \
          theECol = theSCol * (1 - theETA) + theECol * theETA;               \
          theEV   = theSV   * (1 - theETA) + theEV   * theETA;               \
          theEN   = FG_N();                                                  \
        }                                                                    \
      }                                                                      \
    }                                                                        \
    if (draw)                                                                \
      WZBSRCI(ROW, theSCol, theECol, theSN, theEN, theSV, theEV, FT, COS);   \
  }





void 
TrifillColorInterp(int x1, int y1, double n1,
                   int x2, int y2, double n2,
                   int x3, int y3, double n3,
                   double v1, double v2, double v3,
                   EFringeTable ft,
                   unsigned int width, unsigned int height)
{
  double xs, xm, xe, ys, ym, ye, ns, nm, ne, vs, vm, ve;
  double srow, scol, erow, ecol, row;
  double sn, en, sv, ev, t1, t2, dn1, dn2, dv1, dv2;

#define SET_S(aNUM) {xs = x##aNUM; ys = y##aNUM; ns = n##aNUM; vs = v##aNUM;}
#define SET_M(aNUM) {xm = x##aNUM; ym = y##aNUM; nm = n##aNUM; vm = v##aNUM;}
#define SET_E(aNUM) {xe = x##aNUM; ye = y##aNUM; ne = n##aNUM; ve = v##aNUM;}

  if (y1 < y2)
    if (y1 < y3) {
      SET_S(1);
      if (y2 < y3) {
        SET_M(2); SET_E(3);
      } else {
        SET_M(3); SET_E(2);
      }
    } else {
      SET_S(3); SET_M(1); SET_E(2);
    }
  else
    if (y2 < y3) {
      SET_S(2);
      if (y1 < y3) {
        SET_M(1); SET_E(3);
      } else {
        SET_M(3); SET_E(1);
      }
    } else {
      SET_S(3); SET_M(2); SET_E(1);
    }

  if (ye - ys == 0) {
    if (EV_render_mode == FILLED_HIDDEN_RENDERING) {
      if (VIEW_CLIP()) {
        VIEW_CLIPPING_WZBRCI(ye, xs, xe, ns, ne, vs, ve, ft);
      } else {
        WZBRCI(ye, xs, xe, ns, ne, vs, ve, ft);
      }
    } else {
      if        (EV_shade_mode == DITHER_SHADING) {
        if (VIEW_CLIP()) {
          VIEW_CLIPPING_WZBDRCI(ye, xs, xe, ns, ne, vs, ve,
                                ft, EV_curr_cosine);
        } else {
          WZBDRCI(ye, xs, xe, ns, ne, vs, ve, ft, EV_curr_cosine);
        }
      } else  {
        if (VIEW_CLIP()) {
          VIEW_CLIPPING_WZBSRCI(ye, xs, xe, ns, ne, vs, ve,
                                ft, EV_curr_cosine);
        } else {
          WZBSRCI(ye, xs, xe, ns, ne, vs, ve, ft, EV_curr_cosine);
        }
      } 
    }
  } else {
    t2  = (xe - xs)/(ye - ys);
    dn2 = (ne - ns)/(ye - ys);
    dv2 = (ve - vs)/(ye - ys);
    if (ym - ys > 0 && ym > 0) {
      t1   = (xm - xs)/(ym - ys);
      dn1  = (nm - ns)/(ym - ys);
      dv1  = (vm - vs)/(ym - ys);
      if (ys < 0) {		/* Rough clipping */
				scol = xs + t1 * (-ys); ecol = xs + t2 * (-ys);
				srow = 0;
				sn   = ns + dn1 * (-ys); en   = ns + dn2 * (-ys);
        sv   = vs + dv1 * (-ys); ev   = vs + dv2 * (-ys);
      } else {
				scol = ecol = xs;
				srow = ys;
				sn   = en = ns;
        sv   = ev = vs;
      }
      erow = ym;
      if (ym > height) /* Rough clipping */
				erow = height;
      for (row = srow; row <= erow; row++) {
        if (EV_render_mode == FILLED_HIDDEN_RENDERING) {
          if (VIEW_CLIP()) {
            VIEW_CLIPPING_WZBRCI(row, scol, ecol, sn, en, sv, ev, ft);
          } else {
            WZBRCI(row, scol, ecol, sn, en, sv, ev, ft);
          }
        } else {
          if        (EV_shade_mode == DITHER_SHADING) {
            if (VIEW_CLIP()) {
              VIEW_CLIPPING_WZBDRCI(row, scol, ecol, sn, en, sv, ev,
                                    ft, EV_curr_cosine);
            } else {
              WZBDRCI(row, scol, ecol, sn, en, sv, ev, ft, EV_curr_cosine);
            }
          } else  {
            if (VIEW_CLIP()) {
              VIEW_CLIPPING_WZBSRCI(row, scol, ecol, sn, en, sv, ev,
                                    ft, EV_curr_cosine);
            } else {
              WZBSRCI(row, scol, ecol, sn, en, sv, ev, ft, EV_curr_cosine);
            }
          }
        }
				scol += t1; ecol += t2;
				sn += dn1;  en += dn2;
				sv += dv1;  ev += dv2;
      }
    } /* if (ym - ys > 0 && ym > 0) ... */
    if (ye - ym > 0 && ym < height) {
      t1   = (xe - xm)/(ye - ym);
      dn1  = (ne - nm)/(ye - ym);
      dv1  = (ve - vm)/(ye - ym);
      if (ym < 0) {		/* Rough clipping */
				scol = xm + t1 * (-ym); ecol = xs + t2 * (-ys);
				srow = 0; erow = ye;
				sn   = nm + dn1 * (-ym); en   = ns + dn2 * (-ys);
				sv   = vm + dv1 * (-ym); ev   = vs + dv2 * (-ys);
      } else {
				scol = xm; ecol = xs + t2 * (ym - ys);
				srow = ym; erow = ye;
				sn   = nm; en   = ns + (ne - ns)/(ye - ys)*(ym - ys);
				sv   = vm; ev   = vs + (ve - vs)/(ye - ys)*(ym - ys);
      }
      if (ye > height)
				erow = height;
      for (row = srow; row < erow; row++) {
        if (EV_render_mode == FILLED_HIDDEN_RENDERING) {
          if (VIEW_CLIP()) {
            VIEW_CLIPPING_WZBRCI(row, scol, ecol, sn, en, sv, ev, ft);
          } else {
            WZBRCI(row, scol, ecol, sn, en, sv, ev, ft);
          }
        } else {
          if        (EV_shade_mode == DITHER_SHADING) {
            if (VIEW_CLIP()) {
              VIEW_CLIPPING_WZBDRCI(row, scol, ecol, sn, en, sv, ev,
                                    ft, EV_curr_cosine);
            } else {
              WZBDRCI(row, scol, ecol, sn, en, sv, ev, ft, EV_curr_cosine);
            }
          } else  {
            if (VIEW_CLIP()) {
              VIEW_CLIPPING_WZBSRCI(row, scol, ecol, sn, en, sv, ev,
                                    ft, EV_curr_cosine);
            } else {
              WZBSRCI(row, scol, ecol, sn, en, sv, ev, ft, EV_curr_cosine);
            }
          }
        }
				scol += t1; ecol += t2;
				sn += dn1;  en += dn2;
				sv += dv1;  ev += dv2;
      }
    } /* if (ye - ym > 0 && ym < height) ... */
  } /* else if (ye - ys != 0) ...  */
}



void 
BresenhamLine(int x1, int y1, double n1, int x2, int y2, double n2)
{
  int dx, dy, k1, k2;
  int *asx, *asy;
  int x, y, kx, ky, predikce;
  double dn, n;

	if(x1 == x2 && y1 == y2)return;

  if ((y2 - y1)*(x2 - x1) >= 0) {
    if (abs(y2 - y1) < abs(x2 - x1)) {
      if (x2 < x1) {
	Swap(x1, x2, x); Swap(y1, y2, y); Swap(n1, n2, n);
      }
      asx = &x;
      asy = &y;
      x   = x1;
      y   = y1;
      n   = n1;
      kx  = x2;
      ky  = y2;
    } else {
      if (y2 < y1) {
	Swap(x1, x2, x); Swap(y1, y2, y); Swap(n1, n2, n);
      }
      asx = &y;
      asy = &x;
      x   = y1;
      y   = x1;
      n   = n1;
      kx  = y2;
      ky  = x2;
    }
    
    dx       = kx - x;
    dy       = ky - y;
    k1       = 2 * dy;
    k2       = 2 * (dy - dx);
    predikce = k1 - dx;  
    dn       = (n2 - n1)/abs(dx);

    if (VIEW_CLIP()) {
      if (IS_INSIDE(n))
        WZB(*asx, *asy, n, EV_curr_pixel);
    } else {
      WZB(*asx, *asy, n, EV_curr_pixel);
    }
    while (x < kx) {
      ++x;
      n += dn;
      if (predikce < 0)
	predikce += k1;
      else {
	y++;
	predikce += k2;
      }
      if (VIEW_CLIP()) {
        if (IS_INSIDE(n))
          WZB(*asx, *asy, n, EV_curr_pixel);
      } else {
        WZB(*asx, *asy, n, EV_curr_pixel);
      }
    }
  } else {
    if (abs(y2 - y1) <= abs(x2 - x1)) {
      if (x2 > x1) {
	Swap(x1, x2, x); Swap(y1, y2, y); Swap(n1, n2, n);
      }
      asx = &x;
      asy = &y;
      x   = x1;
      y   = y1;
      n   = n1;
      kx  = x2;
      ky  = y2;
      dx  = -kx + x;
      dy  = ky - y;
    } else {
      if (y2 > y1) {
	Swap(x1, x2, x); Swap(y1, y2, y); Swap(n1, n2, n);
      }
      asx = &y;
      asy = &x;
      x   = y1;
      y   = x1;
      n   = n1;
      kx  = y2;
      ky  = x2;
      dx  = -kx + x;
      dy  = ky - y;
    }

    k1       = 2 * dy;
    k2       = 2 * (dy - dx);
    predikce = k1 - dx;  
    dn       = (n2 - n1)/abs(dx);
    
    if (VIEW_CLIP()) {
      if (IS_INSIDE(n))
        WZB(*asx, *asy, n, EV_curr_pixel);
    } else {
      WZB(*asx, *asy, n, EV_curr_pixel);
    }
    while (x > kx) {
      --x;
      n += dn;
      if (predikce < 0)
	predikce += k1;
      else {
	y++;
	predikce += k2;
      }
      if (VIEW_CLIP()) {
        if (IS_INSIDE(n))
          WZB(*asx, *asy, n, EV_curr_pixel);
      } else {
        WZB(*asx, *asy, n, EV_curr_pixel);
      }
    }
  }
}

 

EV_line_clip_result
clip_line_by_a_plane(BOOLEAN have_data, /* IN: whether /data/ field valid */
                     EV_pnt f, EV_pnt s, /* IN: end-points */
                     WCRec *plane_center,  /* IN: clip plane center */
                     WCRec *unit_normal,   /* IN: points toward "out" */
                                         /* half-space; it MUST be of unit */
                                         /* length */
                     EV_pnt f_return, EV_pnt s_return /* OUT: the segment, */
                                                        /* if function */
                                                        /* returned one */
                     )
{
  double d1, d2, t, eta, one_eta;
  WCRec v;
  EV_pnt p1;
  EV_pnt p2;
  
/* distance from the clip plane */
  v.x = f->loc.wc.x - plane_center->x;
  v.y = f->loc.wc.y - plane_center->y;
  v.z = f->loc.wc.z - plane_center->z;
  d1 = DotProd3(&v, unit_normal);
  v.x = s->loc.wc.x - plane_center->x;
  v.y = s->loc.wc.y - plane_center->y;
  v.z = s->loc.wc.z - plane_center->z;
  d2 = DotProd3(&v, unit_normal);
/* It is assumed below that d1 < d2; if it is not so here, swap points */
  if (d2 > d1) {
    p1 = f; p2 = s;
  } else {
    p1 = s; p2 = f;
    Swap(d1, d2, t);
  }
/* Clip */
/* segment is completely in the "out" half-space; data in f_return etc. */
/* NOT valid */
  if (d1 >= 0)
    return EV_LINE_IS_OUT;
/* segment is completely in the "in" half-space */
  if (d2 <= 0) {
    f_return->loc.wc.x = p1->loc.wc.x;
    f_return->loc.wc.y = p1->loc.wc.y;
    f_return->loc.wc.z = p1->loc.wc.z;
    s_return->loc.wc.x = p2->loc.wc.x;
    s_return->loc.wc.y = p2->loc.wc.y;
    s_return->loc.wc.z = p2->loc.wc.z;
    return EV_LINE_IS_IN;
  }
/* segment must be clipped: p1 is "in", p2 is "out" */
  eta     = 1 - d2  / (d2 - d1);
  one_eta = (1 - eta);
  f_return->loc.wc.x = p1->loc.wc.x; 
  f_return->loc.wc.y = p1->loc.wc.y;
  f_return->loc.wc.z = p1->loc.wc.z;
  s_return->loc.wc.x = one_eta * p1->loc.wc.x + eta * p2->loc.wc.x;
  s_return->loc.wc.y = one_eta * p1->loc.wc.y + eta * p2->loc.wc.y;
  s_return->loc.wc.z = one_eta * p1->loc.wc.z + eta * p2->loc.wc.z;
  return EV_LINE_CLIPPED;
}



/* Returns YES when the point is in the "in" half-space (i.e. in the */
/* half-space in which the normal points); otherwise NO. */
BOOLEAN
point_is_in_IN_halfspace(EV_pnt p,             /* IN: point to test */
                         WCRec *plane_center,  /* IN: clip plane center */
                         WCRec *unit_normal    /* IN: points toward "out" */
                                               /* half-space; it MUST be */
                                               /* of unit length */
                     )
{
  WCRec v;
  
/* distance from the clip plane */
  v.x = p->loc.wc.x - plane_center->x;
  v.y = p->loc.wc.y - plane_center->y;
  v.z = p->loc.wc.z - plane_center->z;
  
  return (DotProd3(&v, unit_normal) <= 0);
}



EV_triangle_clip_result
clip_triangle_by_a_plane(BOOLEAN have_data, /* IN: /data/ field valid? */
                         EV_pnt f,
                         EV_pnt s,
                         EV_pnt t, /* IN: end-points */
                         WCRec *plane_center,  /* IN: clip plane center */
                         WCRec *unit_normal, /* IN: points toward "out" */
                                           /* half-space; it MUST be of unit */
                                           /* length */
                         EV_line_end_point_rec pnts_return[],
                         int    *npnts /* OUT: the points of the resulting, */
                                       /* polygon (quadrangle) */
                     )
{
  double d1, d2, d3, tmp, eta, one_eta;
  WCRec v;
  EV_pnt p1;
  EV_pnt p2;
  EV_pnt p3;
  
/* distance from the clip plane */
  v.x = f->loc.wc.x - plane_center->x;
  v.y = f->loc.wc.y - plane_center->y;
  v.z = f->loc.wc.z - plane_center->z;
  d1 = DotProd3(&v, unit_normal);
  v.x = s->loc.wc.x - plane_center->x;
  v.y = s->loc.wc.y - plane_center->y;
  v.z = s->loc.wc.z - plane_center->z;
  d2 = DotProd3(&v, unit_normal);
  v.x = t->loc.wc.x - plane_center->x;
  v.y = t->loc.wc.y - plane_center->y;
  v.z = t->loc.wc.z - plane_center->z;
  d3 = DotProd3(&v, unit_normal);
/* order distances (and points) */
  if (d1 <= d2) {
    if (d2 <= d3) {
      /* 1,2,3 */
      p1 = f;
      p2 = s;
      p3 = t;
    } else {
      if (d1 <= d3) {
        /* 1,3,2 */
        p1 = f; 
        p2 = t; 
        p3 = s;
        Swap(d2, d3, tmp);
      } else {
        /* 3,1,2 */
        p1 = t;
        p2 = f;
        p3 = s;
        Swap(d3, d2, tmp);
        Swap(d1, d2, tmp);
      }
    }
  } else { /* d1 > d2 */
    if (d2 <= d3) {
      if (d1 <= d3) {
        /* 2,1,3 */
        p1 = s;
        p2 = f;
        p3 = t;
        Swap(d2, d1, tmp);
      } else {
        /* 2,3,1 */
        p1 = s;
        p2 = t;
        p3 = f;
        Swap(d2, d1, tmp);
        Swap(d2, d3, tmp);
      }
    } else {
      /* 3,2,1 */
      p1 = t;
      p2 = s;
      p3 = f;
      Swap(d3, d1, tmp);
    }
  }
/* clip */
  if (d1 >= 0)
    return EV_TRIANGLE_IS_OUT;
  if (d3 <= 0) {
    COPY_WCREC(&pnts_return[0].loc.wc, &p1->loc.wc);
    COPY_WCREC(&pnts_return[1].loc.wc, &p2->loc.wc);
    COPY_WCREC(&pnts_return[2].loc.wc, &p3->loc.wc);
    *npnts = 3;
    return EV_TRIANGLE_IS_IN;
  }
  COPY_WCREC(&pnts_return[0].loc.wc, &p1->loc.wc);
  if (d2 < 0) {
    /* point 2 is IN */
    eta     = 1 - d3  / (d3 - d1);
    one_eta = (1 - eta);
    pnts_return[1].loc.wc.x = one_eta * p1->loc.wc.x + eta * p3->loc.wc.x;
    pnts_return[1].loc.wc.y = one_eta * p1->loc.wc.y + eta * p3->loc.wc.y;
    pnts_return[1].loc.wc.z = one_eta * p1->loc.wc.z + eta * p3->loc.wc.z;
    eta     = 1 - d3  / (d3 - d2);
    one_eta = (1 - eta);
    pnts_return[2].loc.wc.x = one_eta * p2->loc.wc.x + eta * p3->loc.wc.x;
    pnts_return[2].loc.wc.y = one_eta * p2->loc.wc.y + eta * p3->loc.wc.y;
    pnts_return[2].loc.wc.z = one_eta * p2->loc.wc.z + eta * p3->loc.wc.z;
    COPY_WCREC(&pnts_return[3].loc.wc, &p2->loc.wc);
    *npnts = 4;
  } else {
    /* point 2 is OUT */
    eta     = 1 - d3  / (d3 - d1);
    one_eta = (1 - eta);
    pnts_return[1].loc.wc.x = one_eta * p1->loc.wc.x + eta * p3->loc.wc.x;
    pnts_return[1].loc.wc.y = one_eta * p1->loc.wc.y + eta * p3->loc.wc.y;
    pnts_return[1].loc.wc.z = one_eta * p1->loc.wc.z + eta * p3->loc.wc.z;
    eta     = 1 - d2  / (d2 - d1);
    one_eta = (1 - eta);
    pnts_return[2].loc.wc.x = one_eta * p1->loc.wc.x + eta * p2->loc.wc.x;
    pnts_return[2].loc.wc.y = one_eta * p1->loc.wc.y + eta * p2->loc.wc.y;
    pnts_return[2].loc.wc.z = one_eta * p1->loc.wc.z + eta * p2->loc.wc.z;
    *npnts = 3;
  }
  return EV_TRIANGLE_CLIPPED;
}
