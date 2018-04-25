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


#ifndef EPRIMTV_H
#define EPRIMTV_H

#ifdef __cplusplus
extern "C" {
#endif

#define CAN_USE_MACROS_INSTEAD_OF_FUNCS
#include "Eview.h"
#include "Egraphic.h"
#undef CAN_USE_MACROS_INSTEAD_OF_FUNCS
#include "Ezbuffer.h"
#include "Edither.h"
#include "Etransfm.h"
#include "Ebezier.h"
#include "Eactset.h"
#include "Ecolors.h"


/* imported from viewb.c */
extern unsigned long can_delay_display;

void
AddToDelayedEntities(void);


#define IS_NEWLINE_ESCAPE(at_p) ((*at_p == '\\') && ( *(at_p+1) == 'n'))
#define NULL_CHAR '\0'

#define PWDAPPOXEQ(X1, Y1, Z1, V1, X2, Y2, Z2, V2, EPS) \
   (   fabs(X2 - X1) <= EPS                                      \
    && fabs(Y2 - Y1) <= EPS                                      \
    && fabs(Z2 - Z1) <= EPS                                      \
    && fabs(V2 - V1) <= EPS)

#define DIST1(ax, ay, bx, by) ((abs(ax-bx) <=1) && (abs(ay-by) <=1))

#define ZERO 0.0

/* Small number to add to the depth (i.e. make the depth smaller) */
#define DELTA_N() (2*(v_p->view_dims_VC.u / v_p->view_dims_DC.x))

#define COPY_WCREC(destP, srcP) \
    { (destP)->x = (srcP)->x; (destP)->y = (srcP)->y; (destP)->z = (srcP)->z; }

#define COPY_VCREC(destP, srcP) \
    { (destP)->u = (srcP)->u; (destP)->v = (srcP)->v; (destP)->n = (srcP)->n; }

#define SHRINK_TRIANGLE_DATA(theSHRINK, fstV, scndV, thrdV) \
  {                                                         \
     double theCV;                                          \
                                                            \
     theCV   = (fstV); theCV += (scndV); theCV += (thrdV);  \
     theCV   *= 0.3333333333333333333333333333333333333333; \
     (fstV)  = theCV + (theSHRINK) * ((fstV)  - theCV);     \
     (scndV) = theCV + (theSHRINK) * ((scndV) - theCV);     \
     (thrdV) = theCV + (theSHRINK) * ((thrdV) - theCV);     \
   }

#define TF(_X1, _Y1, _N1, _X2, _Y2, _N2, _X3, _Y3, _N3, _WIDTH, _HEIGHT) \
     Trifill((int)_X1, (int)_Y1, (double)_N1, \
	     (int)_X2, (int)_Y2, (double)_N2, \
	     (int)_X3, (int)_Y3, (double)_N3, \
	     (unsigned int)_WIDTH, (unsigned int)_HEIGHT)

#define TFCI(_X1, _Y1, _N1, _X2, _Y2, _N2, _X3, _Y3, _N3, _V1, _V2, _V3, \
             _FT, _WIDTH, _HEIGHT)                                       \
     TrifillColorInterp((int)_X1, (int)_Y1, (double)_N1,                 \
	     (int)_X2, (int)_Y2, (double)_N2,                            \
	     (int)_X3, (int)_Y3, (double)_N3,                            \
             (_V1), (_V2), (_V3), (_FT),                                 \
	     (unsigned int)_WIDTH, (unsigned int)_HEIGHT)

#define BHM(_X1, _Y1, _N1, _X2, _Y2, _N2) \
     BresenhamLine((int)_X1, (int)_Y1, (double)_N1, \
                   (int)_X2, (int)_Y2, (double)_N2)

/* imported from viewc.c */
extern Display        *EV_display;
extern Window          EV_curr_win;
extern GC              EV_curr_GC;
extern EPixel          EV_curr_pixel;
extern FPNum           EV_curr_cosine;
extern ERenderingType  EV_render_mode;
extern EShadingType    EV_shade_mode;
extern BOOLEAN         EV_should_clip_by_view_planes;
extern double          EV_fg_plane_depth;
extern double          EV_bg_plane_depth;


void 
BresenhamLine(int x1, int y1, double n1, int x2, int y2, double n2);

void 
Trifill(int x1, int y1, double n1,
        int x2, int y2, double n2,
        int x3, int y3, double n3,
        unsigned int width, unsigned int height);

void 
TrifillColorInterp(int x1, int y1, double n1,
                   int x2, int y2, double n2,
                   int x3, int y3, double n3,
                   double v1, double v2, double v3,
                   EFringeTable ft,
                   unsigned int width, unsigned int height);


typedef struct EV_line_end_point_rec {
  union {
    WCRec wc;
    VCRec vc;
  } loc;
  double data;
}              EV_line_end_point_rec;  

typedef EV_line_end_point_rec  *EV_pnt;

#define EV_ABS(V1, V2) (((V1) > (V2))? ((V1) - (V2)): ((V2) - (V1)))

typedef enum { EV_LINE_IS_OUT = 0,
               EV_LINE_IS_IN  = 1,
               EV_LINE_CLIPPED= 2
             }  EV_line_clip_result;

/* Returns either EV_LINE_IS_OUT (line is clipped off), EV_LINE_IS_IN */
/* (line is inside the displayed volume), or EV_LINE_CLIPPED (line was */
/* clipped -- it is partially visible). */
EV_line_clip_result
clip_line_by_a_plane(BOOLEAN have_data, /* IN: whether /data/ field valid */
                     EV_pnt f, EV_pnt s, /* IN: end-points */
                     WCRec *plane_center,  /* IN: clip plane center */
                     WCRec *unit_normal, /* IN: points toward "out" */
                                         /* half-space; it MUST be of unit */
                                         /* length */
                     EV_pnt f_return, EV_pnt s_return /* OUT: the segment, */
                                                        /* if function */
                                                        /* returned one */
                     );

/* Returns YES when the point is in the "in" half-space (i.e. in the */
/* half-space in which the normal points); otherwise NO. */
BOOLEAN
point_is_in_IN_halfspace(EV_pnt p,             /* IN: point to test */
                         WCRec *plane_center,  /* IN: clip plane center */
                         WCRec *unit_normal    /* IN: points toward "out" */
                                               /* half-space; it MUST be */
                                               /* of unit length */
                     );

typedef enum { EV_TRIANGLE_IS_OUT  = 0,
               EV_TRIANGLE_IS_IN   = 1,
               EV_TRIANGLE_CLIPPED = 2
             }  EV_triangle_clip_result;


EV_triangle_clip_result
clip_triangle_by_a_plane(BOOLEAN have_data, /* IN: /data/ field valid? */
                         EV_pnt f,
                         EV_pnt s,
                         EV_pnt t, /* IN: end-points */
                         WCRec *plane_center,  /* IN: clip plane center */
                         WCRec *unit_normal, /* IN: points toward "out" */
                                           /* half-space; it MUST be of unit */
                                           /* length */
                         EV_line_end_point_rec  pnts_return[],
                         int    *npnts /* OUT: the points of the resulting, */
                                       /* polygon (quadrangle) */

                     );

#ifdef __cplusplus
}
#endif

#endif /* EPRIMTV_H */
