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
#define CAN_USE_MACROS_INSTEAD_OF_FUNCS
#include "Eview.h"
#include "Ezbuffer.h"
#include "Ereprts.h"
#include "Edither.h"
#include "Elixir.h"
#include "Eutils.h"
#include "Eactset.h"
#include <X11/Xproto.h>


/* ======================================================================== */
/* PRIVATE   */
/* ======================================================================== */

/* #define TIME_BOUNDING_BOX_COMPUTATION */

static BOOLEAN using_back_buffer = NO;
static Pixmap back_buffer = (Pixmap)0;
static DCRec back_buf_dim = {0, 0};
static BOOLEAN use_fast_redraw_for_redisplay = NO;

#define LARGE_COORD 1.e30;

#define MAX_GRID_TICKS 30

#define EVDispatchShow(V_P, G_P)                          \
  {                                                       \
    currently_dispatched_g = (G_P);                       \
    if (EGIsSelected((G_P)) == YES &&                     \
        (V_P)->view_layers[EGGetLayer((G_P))].layer_on) { \
      EGHiliteGraphics((V_P), (G_P));                     \
    } else {                                              \
      EGDrawGraphics((V_P), (G_P));                       \
    }                                                     \
    currently_dispatched_g = NULL;                        \
  } 


#ifdef CAN_USE_MACROS_INSTEAD_OF_FUNCS

#define EVBBoxCutsView(V_P, LL, UR)                                      \
  ((   ((LL)->u > (V_P)->ViewRefPointVC.u + (V_P)->view_dims_VC.u * 0.5) \
    || ((LL)->v > (V_P)->ViewRefPointVC.v + (V_P)->view_dims_VC.v * 0.5) \
    || ((UR)->u < (V_P)->ViewRefPointVC.u - (V_P)->view_dims_VC.u * 0.5) \
    || ((UR)->v < (V_P)->ViewRefPointVC.v - (V_P)->view_dims_VC.v * 0.5) \
    ) ? NO: YES                                                          \
   )

#else  /* !CAN_USE_MACROS_INSTEAD_OF_FUNCS */

BOOLEAN EVBBoxCutsView(EView *v_p, VCRec *fc, VCRec *sc);

#endif

void 
AllocBackBuffer(EView *v_p);
void 
FreeBackBuffer(EView *v_p);
void 
ClearBackBuffer(EView *v_p);
void 
DrawBackBuffer(EView *v_p);
void 
EVFastRedraw(EView *v_p);
void
AddToDelayedEntities(void);
static void 
EVRedrawFixtures(EView *v_p);
static void 
DrawAxes(EView *v_p);
void /* in module ics.c */
DrawIntegratedColorScale (EView *v_p);
void /* in module isa.c */
DrawIntegratedStatusArea (EView *v_p);
static void 
DrawCornerAxes(EView *v_p);
static BOOLEAN 
IsEscapeEvent(XEvent *ev);
static void 
EVBoundingBoxAllVC(EView *v_p, VCRec *ll, VCRec *ur);
void 
EVRedrawDisplayList(EView *v_p);
static void 
EVRedrawGrid(EView *v_p);
static void 
EVRedrawFixtures(EView *v_p);
BOOLEAN 
EscapePressed(Display *d);
void 
EVHandleKeyPress(EView *v_p, XKeyEvent *event,
                 KeySym ksym, char *string_val);
void 
EVHandleKeyRelease(EView *v_p, XKeyEvent *event,
                   KeySym ksym, char *string_val);
void  
recompute_dc_size(EView *v_p);
void 
EVRotateByMouse(EView *v_p, FPNum aboutu, FPNum aboutv);
void 
EVSetVectUpAtStart(WCRec *v);
void
EVRecolorCursor(EView *v_p);

void 
AdjustToConstrPlaneLock(EView *v_p, WCRec *p);
void 
AdjustToConstrPlaneGridLock(EView *v_p, WCRec *p);

static void
generate_input_handler_message(EView *v_p, int flag);

/*  DATA  */

static WCRec last_input_point; /* maintained by EVInputPoint */
FPNum rot_angle = 0;
BOOLEAN force_wire_display = NO;

#define ESCAPE '\033'

/* ========================================================================= */

static void 
EVBoundingBoxAllVC(EView *v_p, VCRec *ll_return, VCRec *ur_return)
{
  VCRec ll, ur;
  BOOLEAN any_modification_to_box = NO;
  
  ll_return->u = ll_return->v = ll_return->n = LARGE_COORD;
  ur_return->u = ur_return->v = ur_return->n = -LARGE_COORD;
	
  if (v_p->visible_GO != NULL) {
    if (!list_empty(v_p->visible_GO)) {
      GraphicObj *g_p;
      
      g_p = (GraphicObj *)get_list_next(v_p->visible_GO, NULL);
      while(g_p != NULL) {
				any_modification_to_box = YES;
				EGGraphicsBoundingBoxVC(v_p, g_p, &ll, &ur);
				ll_return->u = min(ll_return->u, ll.u);
				ll_return->v = min(ll_return->v, ll.v);
				ll_return->n = min(ll_return->n, ll.n);
				ur_return->u = max(ur_return->u, ur.u);
				ur_return->v = max(ur_return->v, ur.v);
				ur_return->n = max(ur_return->n, ur.n);
				g_p = (GraphicObj *)get_list_next(v_p->visible_GO, g_p);
      }
    }
  }

  if (!any_modification_to_box) {
     ll_return->u = ll_return->v = ll_return->n = 1;
     ur_return->u = ur_return->v = ur_return->n = -1;
   }
}


static void 
DrawAxes(EView *v_p)
{
  FPNum x, y;
  WCRec O;
  FPNum L;
  int dcx, dcy, dcxo, dcyo;
  Display *d;
  Window win;
  GC agc;

  d   = XtDisplay(v_p->view_widget);
  win = v_p->draw_into;
  agc = v_p->gridGC;

  EVGetDimsVC(v_p, &x, &y);
  L = (x+y)/15.;
  O.x = O.y = O.z = 0;
  EVWCtoDC(v_p, &O, &dcxo, &dcyo);

  O.x = L; O.y = O.z = 0;
  EVWCtoDC(v_p, &O, &dcx, &dcy);
  XDrawLine(d, win, agc, dcxo, dcyo, dcx, dcy);
  XDrawString(d, win, agc, dcx, dcy, "X", 1);

  O.x = 0; O.y = L; O.z = 0;
  EVWCtoDC(v_p, &O, &dcx, &dcy);
  XDrawLine(d, win, agc, dcxo, dcyo, dcx, dcy);
  XDrawString(d, win, agc, dcx, dcy, "Y", 1);
  
  O.x = O.y = 0; O.z = L;
  EVWCtoDC(v_p, &O, &dcx, &dcy);
  XDrawLine(d, win, agc, dcxo, dcyo, dcx, dcy);
  XDrawString(d, win, agc, dcx, dcy, "Z", 1);
  XDrawString(d, win, agc, dcxo, dcyo, "O", 1);
}




static void 
DrawCornerAxes(EView *v_p)
{
  FPNum x, y;
  WCRec O = {0, 0, 0}, E1 = {1, 0, 0}, E2 = {0, 1, 0}, E3 = {0, 0, 1};
  FPNum L;
  int dcx, dcy, dcx0, dcy0;
  Display *d;
  Window win;
  GC agc;
  int atx = 30, aty = 30;
#define XS (atx+(dcx-dcx0))
#define YS (aty+(dcy-dcy0))
  static EPixel white, black, fg, bg;
  static int first = 1;
  double r, g, b;

  if (first) {
    BOOLEAN suc;
    white = ColorGetPixelFromString("white", &suc);
    if (!suc)
      white = 1;
    black = ColorGetPixelFromString("black", &suc);
    if (!suc)
      black = 0;
    first = 0;
  }
  
  ColorPixelToRGBStandard(v_p->background, &r, &g, &b);
  if (r < 0.5 && g < 0.5 && b < 0.5) {
    fg = black; bg = white;
  } else {
    fg = white; bg = black;
  }

  d   = XtDisplay(v_p->view_widget);
  win = v_p->draw_into;
  agc = v_p->writableGC;

  EVWCtoDC(v_p, &O, &dcx0, &dcy0);
  EVGetDimsVC(v_p, &x, &y);
  L = v_p->view_dims_VC.u / (double)v_p->view_dims_DC.x * 20;

  XSetForeground(d, agc, bg);
  XFillRectangle(d, win, agc, 0, 0, (unsigned)(2*atx), (unsigned)(2*aty));
  XSetForeground(d, agc, fg);
  
  E1.x *= L;
  EVWCtoDC(v_p, &E1, &dcx, &dcy);
  XDrawLine(d, win, agc, atx, aty, XS, YS);
  XDrawString(d, win, agc, XS, YS, "X", 1);

  E2.y *= L;
  EVWCtoDC(v_p, &E2, &dcx, &dcy);
  XDrawLine(d, win, agc, atx, aty, XS, YS);
  XDrawString(d, win, agc, XS, YS, "Y", 1);

  E3.z *= L;
  EVWCtoDC(v_p, &E3, &dcx, &dcy);
  XDrawLine(d, win, agc, atx, aty, XS, YS);
  XDrawString(d, win, agc, XS, YS, "Z", 1);
  
  XSetForeground(d, agc, v_p->foreground); /* reset writable GC */
}





static void 
EVRedrawFixtures(EView *v_p) 
{
  if (v_p->grid_on == YES)
    EVRedrawGrid(v_p);
  if (v_p->show_axes)
    DrawAxes(v_p);
  if (v_p->show_scale)
    DrawIntegratedColorScale(v_p);
  if (v_p->show_status)
    DrawIntegratedStatusArea(v_p);
}


static void
DrawGridInViewPlane(EView *v_p);
static void
DrawGridInConstrPlane(EView *v_p);



static void 
EVRedrawGrid(EView *v_p)
{
  if (v_p->constrplane_lock_on) {
    /* Grid is to be considered in the construction plane */
    DrawGridInConstrPlane(v_p);
  } else {
    /* Grid lies in the view plane */
    DrawGridInViewPlane(v_p);
  }
}
  

static void
DrawGridInConstrPlane(EView *v_p)
{
  double du, dv, du1, dv1, du2, dv2, ou, ov;
  double start_u, start_v, u, v, t;
  FPNum v_d_u, v_d_v;
  int i, j, x1, x2, y1, y2, xc, yc, Tdx1, Tdy1, Tdx2, Tdy2;
  WCRec O, P1, P2, vrpwc;
  VCRec p, Ovc, P1vc, P2vc, vrpvc, startvc;
  unsigned int w, h;
  Display *d;
  Window win;
  GC agc;

  ou = v_p->grid_origin.u;
  ov = v_p->grid_origin.v;
  du = v_p->grid_deltas.u;
  dv = v_p->grid_deltas.v;

  O.x = (v_p->constrplane_center.x
         + ou * v_p->constrplane_u.x
         + ov * v_p->constrplane_v.x);
  O.y = (v_p->constrplane_center.y
         + ou * v_p->constrplane_u.y
         + ov * v_p->constrplane_v.y);
  O.z = (v_p->constrplane_center.z
         + ou * v_p->constrplane_u.z
         + ov * v_p->constrplane_v.z);
/*
  P1.x = v_p->constrplane_center.x + du * v_p->constrplane_u.x;
  P1.y = v_p->constrplane_center.y + du * v_p->constrplane_u.y;
  P1.z = v_p->constrplane_center.z + du * v_p->constrplane_u.z;

  P2.x = v_p->constrplane_center.x + dv * v_p->constrplane_v.x;
  P2.y = v_p->constrplane_center.y + dv * v_p->constrplane_v.y;
  P2.z = v_p->constrplane_center.z + dv * v_p->constrplane_v.z;
*/
  P1.x = O.x + du * v_p->constrplane_u.x;
  P1.y = O.y + du * v_p->constrplane_u.y;
  P1.z = O.z + du * v_p->constrplane_u.z;

  P2.x = O.x + dv * v_p->constrplane_v.x;
  P2.y = O.y + dv * v_p->constrplane_v.y;
  P2.z = O.z + dv * v_p->constrplane_v.z;

  EVWCtoVC(v_p, &O, &Ovc);
  EVWCtoVC(v_p, &P1, &P1vc);
  EVWCtoVC(v_p, &P2, &P2vc);

  EVGetDimsVC(v_p, &v_d_u, &v_d_v);
  EVGetVRP(v_p, &vrpwc);
  EVWCtoVC(v_p, &vrpwc, &vrpvc);
  EVGetDimsDC(v_p, &w, &h);
  d   = XtDisplay(v_p->view_widget);
  win = v_p->draw_into;
  agc = v_p->gridGC;

  AdjustToConstrPlaneLock(v_p, &vrpwc);
  AdjustToConstrPlaneGridLock(v_p, &vrpwc);
  EVWCtoVC(v_p, &vrpwc, &startvc);

  du1 = P1vc.u - Ovc.u;
  dv1 = P1vc.v - Ovc.v;
  du2 = P2vc.u - Ovc.u;
  dv2 = P2vc.v - Ovc.v;
  
  t     = sqrt(du1 * du1 + dv1 * dv1);
  Tdx1  = (double)GRID_MARK_LENGTH_HALF * du1 / t;
  Tdy1  = -(double)GRID_MARK_LENGTH_HALF * dv1 / t;
  t     = sqrt(du2 * du2 + dv2 * dv2);
  Tdx2  = (double)GRID_MARK_LENGTH_HALF * du2 / t;
  Tdy2  = -(double)GRID_MARK_LENGTH_HALF * dv2 / t;
  
  start_u = startvc.u - MAX_GRID_TICKS/2 * (du1 + du2);
  start_v = startvc.v - MAX_GRID_TICKS/2 * (dv1 + dv2);

  for (i = 0; i < MAX_GRID_TICKS; i++) {
    u = start_u;  
    v = start_v;
    for (j = 0; j < MAX_GRID_TICKS; j++) {
      p.u = u; p.v = v;
      EVVCtoDC(v_p, &p, &xc, &yc);
      x1 = xc - Tdx1;
      y1 = yc - Tdy1;
      x2 = xc + Tdx1;
      y2 = yc + Tdy1;
      XDrawLine(d, win, agc, x1, y1, x2, y2);
      x1 = xc - Tdx2;
      y1 = yc - Tdy2;
      x2 = xc + Tdx2;
      y2 = yc + Tdy2;
      XDrawLine(d, win, agc, x1, y1, x2, y2);
      u += du2;
      v += dv2;
    }
    start_u += du1;
    start_v += dv1;
  }
}



static void
DrawGridInViewPlane(EView *v_p)
{
  FPNum du, dv, ou, ov, start_u, start_v, end_u, end_v, u, v;
  FPNum v_d_u, v_d_v;
  int x1, x2, y1, y2, xc, yc;
  long n;
  WCRec vowc, vrpwc;
  VCRec vovc, vrpvc, p = {0, 0, 0};
  Display *d;
  Window win;
  GC agc;

  ou = v_p->grid_origin.u;
  ov = v_p->grid_origin.v;
  du = v_p->grid_deltas.u;
  dv = v_p->grid_deltas.v;

  EVGetDimsVC(v_p, &v_d_u, &v_d_v);
  EVGetOriginVC(v_p, &vowc);
  EVWCtoVC(v_p, &vowc, &vovc);
  EVGetVRP(v_p, &vrpwc);
  EVWCtoVC(v_p, &vrpwc, &vrpvc);

  n = (vrpvc.u - vovc.u)/du - v_d_u/du/2 - 1;
  start_u = vovc.u + ou + n * du;
  end_u   = start_u + v_d_u/du + 2;
  n = (vrpvc.v - vovc.v)/dv - v_d_v/dv/2 - 1;
  start_v = vovc.v + ov + n * dv;
  end_v   = start_v + v_d_v/dv + 2;
  if ((end_v - start_v)/dv > MAX_GRID_TICKS ||
      (end_u - start_u)/du > MAX_GRID_TICKS)
    return;

  d   = XtDisplay(v_p->view_widget);
  win = v_p->draw_into;
  agc = v_p->gridGC;
  u = start_u;
  while(u < end_u) {
    v = start_v;
    while(v < end_v) {
      p.u = u; p.v = v;
      EVVCtoDC(v_p, &p, &xc, &yc);
      x1 = xc - GRID_MARK_LENGTH_HALF;
      y1 = yc;
      x2 = xc + GRID_MARK_LENGTH_HALF;
      y2 = yc;
      XDrawLine(d, win, agc, x1, y1, x2, y2);
      x1 = xc;
      y1 = yc - GRID_MARK_LENGTH_HALF;
      x2 = xc;
      y2 = yc + GRID_MARK_LENGTH_HALF;
      XDrawLine(d, win, agc, x1, y1, x2, y2);
      v += dv;
    }
    u += du;
  }
}



static BOOLEAN 
IsEscapeEvent(XEvent *ev)
{
  char buffer[12];
  KeySym ksym;
  int lngth;
  static XComposeStatus	c_stat;
  
  if (ev->type == KeyPress) {
    lngth = XLookupString(&(ev->xkey), buffer, 12, &ksym, &c_stat);
    buffer[lngth] = '\0';
    if (buffer[0] == ESCAPE) {
      return YES;
    }
  }
  return NO;
}

static LIST delayed_entities = NULL;
static GraphicObj *currently_dispatched_g = NULL;
unsigned long can_delay_display = 0;

static void 
InitDelayedEntityList(void)
{
  can_delay_display = YES;
  if (delayed_entities == (LIST)NULL)
    delayed_entities = make_list();
  if (!list_empty(delayed_entities))
    flush_list(delayed_entities, NOT_NODES);
}

void
AddToDelayedEntities(void)
{
  if (currently_dispatched_g != NULL)
    add_to_tail(delayed_entities, currently_dispatched_g);
}

static void
DrawDelayedEntities(EView *v_p)
{
  GraphicObj *g;

  can_delay_display = NO;
  g = (GraphicObj *)get_list_next(delayed_entities, NULL);
  while (g != NULL) {
    EVDispatchShow(v_p, g);
    g = (GraphicObj *)get_list_next(delayed_entities, g);
  }
}

void 
EVRedrawDisplayList(EView *v_p)
{
  ERenderingType orig_render_mode;
  GraphicObj *prev_g;
  GraphicObj *g;
  LIST aList = v_p->visible_GO;
  unsigned dx, dy;
  int layer;
  VCRec ll, ur;
  int entity_count = 0;

  orig_render_mode = v_p->render_mode;
  
  if (force_wire_display)
    v_p->render_mode = WIRE_RENDERING;
  
  if (v_p->render_mode == CONST_SHADING_RENDERING ||
      v_p->render_mode == FILLED_HIDDEN_RENDERING) {
    EVGetDimsDC(v_p, &dx, &dy);
    if (!ZBufferInit(v_p, (signed)dx, (signed)dy, EVGetBackground(v_p)))
      v_p->render_mode = NORMAL_RENDERING;
  }
  
  if (v_p->visible_GO != NULL) {
    InitDelayedEntityList();
    prev_g = NULL;
    g = (GraphicObj *)get_list_next(aList, prev_g);
    while (g != NULL) {
      layer = EGGetLayer(g);
      if (layer >= 0 && layer < MAX_LAYER) {
				if (v_p->view_layers[layer].layer_on) {
					EGGraphicsBoundingBoxVC(v_p, g, &ll, &ur);
					if (EVBBoxCutsView(v_p, &ll, &ur)) 
						EVDispatchShow(v_p, g);
					prev_g = g;
				} else 
					link_to_tail(v_p->invisible_GO, unlink_current_prev(v_p->visible_GO));
      }
      if (entity_count++ > 2) {
        if (EscapePressed(XtDisplay(v_p->view_widget)))
          goto end_redisplay;
        entity_count = 0;
      }
      g = (GraphicObj *)get_list_next(aList, prev_g);
    }
  }

 end_redisplay:
  
  if (v_p->render_mode == CONST_SHADING_RENDERING ||
      v_p->render_mode == FILLED_HIDDEN_RENDERING)
    ZBufferPaint(XtDisplay(v_p->view_widget), v_p->draw_into,
		 EVWritableGC(v_p));

  DrawDelayedEntities(v_p);

  if (v_p->show_axes)
    DrawCornerAxes(v_p);
  if (v_p->show_scale)
    DrawIntegratedColorScale(v_p);
  if (v_p->show_status)
    DrawIntegratedStatusArea(v_p);
  
  v_p->render_mode = orig_render_mode;
}




/* ======================================================================== */
/* PUBLIC   */
/* ======================================================================== */

Widget 
EVViewPToWidget(EView *v_p)
{
  return (v_p->view_widget);
}


void
EVShowAxes(EView *v_p, BOOLEAN on_off)
{
  v_p->show_axes = on_off;
}


void
EVToggleAxes(EView *v_p)
{
	v_p->show_axes = !v_p->show_axes;
}


void
EVShowScale(EView *v_p, BOOLEAN on_off)
{
  v_p->show_scale = on_off;
}


void
EVToggleScale(EView *v_p)
{
	v_p->show_scale = !v_p->show_scale;
}


void
EVShowStatus(EView *v_p, BOOLEAN on_off)
{
  v_p->show_status = on_off;
}


void
EVToggleStatus(EView *v_p)
{
	v_p->show_status = !v_p->show_status;
}


void 
EVShowGrid(EView *v_p, BOOLEAN on_off)
{
  v_p->grid_on       = on_off;
}

void 
EVSetGridLock(EView *v_p, BOOLEAN on_off)
{
  v_p->grid_lock_on = on_off;
}


void 
EVSetGridOrigin(EView *v_p, FPNum x, FPNum y)
{
  v_p->grid_origin.u = x;
  v_p->grid_origin.v = y;
}

void 
EVSetGridDeltas(EView *v_p, FPNum delta_x, FPNum delta_y)
{
  if (delta_x > 0.0)    v_p->grid_deltas.u = delta_x;
  else                  v_p->grid_deltas.u = 1.0;
  if (delta_y > 0.0)    v_p->grid_deltas.v = delta_y;
  else                  v_p->grid_deltas.v = 1.0;
}
  

void 
EVSetLastInputPoint(WCRec *p)
{
  last_input_point.x = p->x;
  last_input_point.y = p->y;
  last_input_point.z = p->z;
}

void 
EVGetLastInputPoint(WCRec *p)
{
  p->x = last_input_point.x;
  p->y = last_input_point.y;
  p->z = last_input_point.z;
}

GC 
EVHiliteGC(EView *v_p)
{
  return v_p->hiliteGC;
}

GC 
EVEraseGC(EView *v_p)
{
  return v_p->eraseGC;
}

GC 
EVErasehiliteGC(EView *v_p)
{
  return v_p->erasehiliteGC;
}



GC 
EVDefaultCopyGC(EView *v_p)
{
  return v_p->defaultCopyGC;
}



GC 
EVDefaultEraseGC(EView *v_p)
{
  return v_p->defaultEraseGC;
}



GC 
EVDefaultXORGC(EView *v_p)
{
  return v_p->defaultXORGC;
}



GC 
EVWritableGC(EView *v_p)
{
  return v_p->writableGC;
}



GC 
EVConstrCopyGC(EView *v_p)
{
  return v_p->constrCopyGC;
}



GC 
EVConstrEraseGC(EView *v_p)
{
  return v_p->constrEraseGC;
}



GC 
EVConstrXORGC(EView *v_p)
{
  return v_p->constrXORGC;
}



void 
EVGetDimsVC(EView *v_p, FPNum *u, FPNum *v)
{
  *u = v_p->view_dims_VC.u;
  *v = v_p->view_dims_VC.v;
}



void 
EVSetVRP(EView *v_p, WCRec *p)
{
  v_p->ViewRefPoint.x = p->x;
  v_p->ViewRefPoint.y = p->y;
  v_p->ViewRefPoint.z = p->z;
  EVWCtoVC(v_p, &(v_p->ViewRefPoint), &(v_p->ViewRefPointVC));
}



void 
EVGetVRP(EView *v_p, WCRec *p)
{
  p->x = v_p->ViewRefPoint.x;
  p->y = v_p->ViewRefPoint.y;
  p->z = v_p->ViewRefPoint.z;
}



void 
EVSetOriginVC(EView *v_p, WCRec *p)
{
	/* WCRec vrp; */

  v_p->VCOrigin.x = p->x;
  v_p->VCOrigin.y = p->y;
  v_p->VCOrigin.z = p->z;
	/*
	EVGetVRP(v_p, &vrp);
	EVSetVRP(v_p, &vrp);
	*/
  EVWCtoVC(v_p, &(v_p->ViewRefPoint), &(v_p->ViewRefPointVC));
}



void 
EVGetOriginVC(EView *v_p, WCRec *p)
{
  p->x = v_p->VCOrigin.x;
  p->y = v_p->VCOrigin.y;
  p->z = v_p->VCOrigin.z;
}



void 
EVGetViewBox(EView *v_p, VCRec *ll, VCRec *ur)
{
  ll->u = v_p->ViewRefPointVC.u - v_p->view_dims_VC.u * 0.5;
  ll->v = v_p->ViewRefPointVC.v - v_p->view_dims_VC.v * 0.5;
  ll->n = v_p->ViewRefPointVC.n - 1;
  ur->u = v_p->ViewRefPointVC.u + v_p->view_dims_VC.u * 0.5;
  ur->v = v_p->ViewRefPointVC.v + v_p->view_dims_VC.v * 0.5;
  ur->n = v_p->ViewRefPointVC.n + 1;
}


void
EVSetUseFastRedrawRedisplay(BOOLEAN on_off)
{
	use_fast_redraw_for_redisplay = on_off;
}


void 
EVForceRedisplay(EView *v_p)
{

/* fast redraw enables visualization of motion without explicitly
	 prescribing delay after each move - dr */

	if(use_fast_redraw_for_redisplay)
		EVFastRedraw(v_p);
	else
		XClearArea(XtDisplay(v_p->view_widget), XtWindow(v_p->view_widget),
							 0, 0, 0, 0, True);
}



void 
EVFastRedraw(EView *v_p)
{
  Window win;

  if (using_back_buffer) {
    win = v_p->draw_into;
    v_p->draw_into = back_buffer;
    ClearBackBuffer(v_p);
    EVRedrawContents(v_p);
    v_p->draw_into = win;
    DrawBackBuffer(v_p);
  } else {
    XClearArea(XtDisplay(v_p->view_widget), XtWindow(v_p->view_widget),
	       0, 0, 0, 0, False);
    EVRedrawContents(v_p);
  }
}
    


void 
FreeBackBuffer(EView *v_p)
{
  if (back_buffer != (Pixmap)0)
    XFreePixmap(XtDisplay(v_p->view_widget), back_buffer);
  back_buffer = (Pixmap)0;
	using_back_buffer = NO;
}
      


void 
ClearBackBuffer(EView *v_p)
{
  XFillRectangle(XtDisplay(v_p->view_widget), back_buffer,
		 v_p->eraseGC, 0, 0,
		 (unsigned)back_buf_dim.x, (unsigned)back_buf_dim.y);
}



int 
AllocPixmapErrorHandler(Display *display, XErrorEvent *e);

int 
AllocPixmapErrorHandler(Display *display, XErrorEvent *e)
{
  return 0;
}
 


void 
AllocBackBuffer(EView *v_p)
{
  XWindowAttributes xwats;
  Status status;
  Window win;
  int x, y;
  unsigned int w, h, bdw, d;
  

  if (   back_buffer != (Pixmap)0
      && back_buf_dim.x >= v_p->view_dims_DC.x
      && back_buf_dim.y >= v_p->view_dims_DC.y
      )
    return;
  
  FreeBackBuffer(v_p);

  status = XGetWindowAttributes(XtDisplay(v_p->view_widget),
				XtWindow(v_p->view_widget),
				&xwats);

  XSetErrorHandler(AllocPixmapErrorHandler); /* catch so that we don't exit */
  
  back_buffer = XCreatePixmap(XtDisplay(v_p->view_widget),
			      v_p->draw_into,
			      (unsigned int)v_p->view_dims_DC.x,
			      (unsigned int)v_p->view_dims_DC.y,
			      (unsigned int)xwats.depth);
  
  if (XGetGeometry(XtDisplay(v_p->view_widget), back_buffer,
		   &win, &x, &y, &w, &h, &bdw, &d)) { /* to check whether OK */
    back_buf_dim.x  = v_p->view_dims_DC.x;
    back_buf_dim.y  = v_p->view_dims_DC.y;
    using_back_buffer = YES;
  } else {
    back_buffer = (Pixmap)0;
    back_buf_dim.x  = back_buf_dim.y = 0;
    using_back_buffer = NO;
  }
  XSetErrorHandler(NULL); /* now we consumed all possible errors */
}
   

BOOLEAN 
EVUsingBackBuffer()
{
  return using_back_buffer;
}


Pixmap
EVBackBuffer()
{
  return back_buffer;
}

void 
DrawBackBuffer(EView *v_p)
{
  XCopyArea(XtDisplay(v_p->view_widget),
	    back_buffer,
	    v_p->draw_into,
	    v_p->writableGC,
	    0, 0,
	    (unsigned int)v_p->view_dims_DC.x,
	    (unsigned int)v_p->view_dims_DC.y,
	    0, 0);
  XSync(XtDisplay(v_p->view_widget), 0);
}



void 
EVRedrawContents(EView *v_p)
{
  EVRedrawFixtures(v_p);
  EVRedrawDisplayList(v_p); 
}



void 
EVFlushLayerDisplayList(EView *v_p, int layer)
{
  GraphicObj *g, *prev_g;
  
  if (v_p->visible_GO == NULL)
      v_p->visible_GO = make_list();
  else {
    prev_g = NULL;
    g = (GraphicObj *)get_list_next(v_p->visible_GO, prev_g);
    while(g != NULL) {
      if (EGGetLayer(g) == layer) 
				get_from_current_prev(v_p->visible_GO);
      else
				prev_g = g;
      g = (GraphicObj *)get_list_next(v_p->visible_GO, prev_g);
    }
  }
  if (v_p->invisible_GO == NULL)
      v_p->invisible_GO = make_list();
  else {
    prev_g = NULL;
    g = (GraphicObj *)get_list_next(v_p->invisible_GO, prev_g);
    while(g != NULL) {
      if (EGGetLayer(g) == layer) 
				get_from_current_prev(v_p->invisible_GO);
      else
				prev_g = g;
      g = (GraphicObj *)get_list_next(v_p->invisible_GO, prev_g);
    }
  }
}



void 
EVFlushAllDisplayLists(EView *v_p)
{
  flush_list(v_p->visible_GO, NOT_NODES);
  flush_list(v_p->invisible_GO, NOT_NODES);
}



EFringeTable 
EVSetAssocFringeTable(EView *v_p, EFringeTable ft)
{
  EFringeTable old_ft;
  
  old_ft = v_p->fringe_table;
  v_p->fringe_table = ft;
  return old_ft;
}



EFringeTable 
EVGetAssocFringeTable(EView *v_p)
{
  if (v_p->fringe_table == NULL) {
    v_p->fringe_table = ColorCreateFringeTable(); /* resolve the deadlock */
    ColorSetupFringeTableByMinMax(v_p->fringe_table, -1.0, 1.0);
  }
  return v_p->fringe_table;
}



BOOLEAN 
EscapePressed(Display *d)
{
  XEvent event;

  if (XCheckTypedEvent(d, KeyPress, &event))
    return IsEscapeEvent(&event);
  return NO;
}



void 
EVFitAllIntoView(EView *v_p)
{
  VCRec ll, ur, p;
  WCRec vrpwc;

  recompute_dc_size(v_p);
#ifdef TIME_BOUNDING_BOX_COMPUTATION
  msec_timer(YES);
#endif
  EVBoundingBoxAllVC(v_p, &ll, &ur);
#ifdef TIME_BOUNDING_BOX_COMPUTATION
  fprintf(stderr, "Bounding box all: %lu [msec]\n", msec_timer(NO));
#endif
  p.u = (ur.u+ll.u)*0.5;
  p.v = (ur.v+ll.v)*0.5;
  p.n = (ur.n+ll.n)*0.5;
  EVVCtoWC(v_p, &p, &vrpwc);
  EVSetVRP(v_p, &vrpwc);
  EVSetDimsVC(v_p, 1.05*(ur.u-ll.u), 1.05*(ur.v-ll.v));
  EVForceRedisplay(v_p);
}



EPixel 
EVGetForeground(EView *v_p)
{
  return v_p->foreground;
}



EPixel 
EVGetBackground(EView *v_p)
{
  return v_p->background;
}


void
EVSetBackground(EView *v_p, EPixel bkg)
{
  int ac;
  Arg al[1];
  
  ac = 0;
  XtSetArg(al[ac], XtNbackground, bkg);  ac++;
  XtSetValues(v_p->view_widget, al, ac);
  XSetBackground(XtDisplay(v_p->view_widget), v_p->writableGC, bkg);
  XSetBackground(XtDisplay(v_p->view_widget), v_p->eraseGC, bkg);
  XSetForeground(XtDisplay(v_p->view_widget), v_p->eraseGC, bkg);
/*  XSetBackground(XtDisplay(v_p->view_widget), v_p->hiliteGC, bkg); */
  XSetBackground(XtDisplay(v_p->view_widget), v_p->erasehiliteGC, bkg);
  XSetForeground(XtDisplay(v_p->view_widget), v_p->erasehiliteGC, bkg);
  FreeBackBuffer(v_p);
  v_p->background = bkg;
  EVRecolorCursor(v_p);
}

void
EVSetForeground(EView *v_p, EPixel frg)
{
  int ac;
  Arg al[1];
  
  ac = 0;
  XtSetArg(al[ac], XtNforeground, frg);  ac++;
  XtSetValues(v_p->view_widget, al, ac);
/*  XSetForeground(XtDisplay(v_p->view_widget), v_p->writableGC, frg); */
  XSetForeground(XtDisplay(v_p->view_widget), v_p->hiliteGC, frg);
  v_p->foreground = frg;
}


void
EVRecolorCursor(EView *v_p)
{
  XColor fg, bg;
  double r, g, b;

  ColorPixelToRGBStandard(v_p->background, &r, &g, &b);
  if (r < 0.5 && g < 0.5 && b < 0.5) {
    fg.red = 65535; fg.green = 65535; fg.blue = 65535;
    bg.red = 0;     bg.green = 0;     bg.blue = 0;
  } else {
    fg.red = 0;     fg.green = 0;     fg.blue = 0;
    bg.red = 65535; bg.green = 65535; bg.blue = 65535;
  }
  XRecolorCursor(XtDisplay(v_p->view_widget), v_p->cursor, &fg, &bg);
}



void 
EVUnlinkGraphics(EView *v_p, GraphicObj *g_p)
{
  int layer;
  GraphicObj *p;
  LIST to_search;

  layer = EGGetLayer(g_p);
  if (layer >= 0 && layer < MAX_LAYER) {
    if (v_p->view_layers[layer].layer_on)
      to_search = v_p->visible_GO;
    else
      to_search = v_p->invisible_GO;
    
    if (to_search != NULL) {
      p = (GraphicObj *)get_list_next(to_search, NULL);
      while (p != NULL) {
				if (p == g_p) {
					if (v_p->view_layers[layer].layer_on)
						EGEraseGraphics(v_p, g_p);
					get_from_current_prev(to_search);
					break;
				}
				p = (GraphicObj *)get_list_next(to_search, p);
      }
    }
  }
}



void 
EVUnlinkMarkedGraphics(EView *v_p)
{
  GraphicObj *p, *prev_p;
  LIST to_search;

	prev_p = NULL;
	to_search = v_p->visible_GO;
	if (to_search != NULL) {
		p = (GraphicObj *)get_list_next(to_search, prev_p);
		while (p != NULL) {
			if(EGIsMarked(p) == YES){
				EGEraseGraphics(v_p, p);
				get_from_current_prev(to_search);
			}
			else
				prev_p = p;
			p = (GraphicObj *)get_list_next(to_search, prev_p);
		}
	}

	prev_p = NULL;
	to_search = v_p->invisible_GO;
	if (to_search != NULL) {
		p = (GraphicObj *)get_list_next(to_search, prev_p);
		while (p != NULL) {
			if(EGIsMarked(p) == YES)
				get_from_current_prev(to_search);
			else
				prev_p = p;
			p = (GraphicObj *)get_list_next(to_search, prev_p);
		}
	}
}




struct EModel *
EVGetModel(EView *v_p)
{
  return (struct EModel *)v_p->model_p;
}



void 
EVSetModel(EView *v_p, struct EModel *model)
{
  v_p->model_p = (struct EModel *)model;
}



void 
EVDisplayGraphics(EView *v_p, GraphicObj *g_p, BOOLEAN immed_draw)
{
  int layer;

  layer = EGGetLayer(g_p);
  if (layer >= 0 && layer < MAX_LAYER) {
    if (v_p->view_layers[layer].layer_on) {
      add_to_tail(v_p->visible_GO, g_p);
      if (immed_draw)
        EVDrawGraphics(v_p, g_p);
    } else
      add_to_tail(v_p->invisible_GO, g_p);
  }
}



void 
EVUpdateDisplayLists(EView *v_p, GraphicObj *to_check_g_p)
{
  int layer;
  LIST from_list, to_list;
  GraphicObj *g_p, *prev_g_p;

  layer = EGGetLayer(to_check_g_p);
  if (layer >= 0 && layer < MAX_LAYER) {
    if (v_p->view_layers[layer].layer_on)
      from_list = v_p->invisible_GO, to_list = v_p->visible_GO;
    else
      from_list = v_p->visible_GO,   to_list = v_p->invisible_GO;
    prev_g_p = NULL;
    g_p      = (GraphicObj *)get_list_next(from_list, prev_g_p);
    while(g_p != NULL) {
      if (g_p == to_check_g_p) {
				link_to_tail(to_list, unlink_current_prev(from_list));
				break;
      } else
				prev_g_p = g_p;
      g_p = (GraphicObj *)get_list_next(from_list, prev_g_p);
    }
  }
}



void 
EVFastUpdateDisplayLists(EView *v_p)
{
  int old_layer, new_layer;
  LIST to_search;
  GraphicObj *g_p, *prev_g_p;
	BOOLEAN new_visible;

	new_layer = EASValsGetLayer();
  if (new_layer >= 0 && new_layer < MAX_LAYER) {
		new_visible = v_p->view_layers[new_layer].layer_on;

		if(new_visible == NO){
			prev_g_p = NULL;
			to_search = v_p->visible_GO;
			if (to_search != NULL) {
				g_p = (GraphicObj *)get_list_next(to_search, prev_g_p);
				while (g_p != NULL) {
					if(EGIsMarked(g_p) == YES){
						old_layer = EGGetLayer(g_p);
						if(old_layer != new_layer)
							link_to_tail(v_p->invisible_GO, unlink_current_prev(v_p->visible_GO));
						else
							prev_g_p = g_p;
					}
					else
						prev_g_p = g_p;
					g_p = (GraphicObj *)get_list_next(v_p->visible_GO, prev_g_p);
				}
			}
		}
		else{
			prev_g_p = NULL;
			to_search = v_p->invisible_GO;
			if (to_search != NULL) {
				g_p = (GraphicObj *)get_list_next(to_search, prev_g_p);
				while (g_p != NULL) {
					if(EGIsMarked(g_p) == YES){
						old_layer = EGGetLayer(g_p);
						if(old_layer != new_layer)
							link_to_tail(v_p->visible_GO, unlink_current_prev(v_p->invisible_GO));
						else
							prev_g_p = g_p;
					}
					else
						prev_g_p = g_p;
					g_p = (GraphicObj *)get_list_next(v_p->invisible_GO, prev_g_p);
				}
			}
		}
	}				
}



void 
EVToggleLayerOnOff(EView *v_p, int layer)
{
  if (layer >= 0 && layer < MAX_LAYER) {
    if (v_p->view_layers[layer].layer_on)
      EVSetLayerOnOff(v_p, layer, NO);
    else
      EVSetLayerOnOff(v_p, layer, YES);
    EVForceRedisplay(v_p);
  }
}



void 
EVSetLayerOnOff(EView *v_p, int layer, BOOLEAN on)
{
  GraphicObj *g_p, *prev_g_p;
  LIST from_list, to_list;
  
  if (layer >= 0 && layer < MAX_LAYER) {
		if(v_p->view_layers[layer].layer_on != on){
			v_p->view_layers[layer].layer_on = on;
			if (on)
				from_list = v_p->invisible_GO, to_list = v_p->visible_GO;
			else
				from_list = v_p->visible_GO, to_list = v_p->invisible_GO;
			prev_g_p = NULL;
			g_p = (GraphicObj *)get_list_next(from_list, prev_g_p);
			while(g_p != NULL) {
				if (EGGetLayer(g_p) == layer)
					link_to_tail(to_list, unlink_current_prev(from_list));
				else
					prev_g_p = g_p;
				g_p = (GraphicObj *)get_list_next(from_list, prev_g_p);
			}
			EVForceRedisplay(v_p);
		}
  }
}




void 
EVSetMultLayersOnOff(EView *v_p,
                     int layers[],  /* IN layer numbers */
                     BOOLEAN on[], /* IN flags whether layer[i] is to be */
                                   /* set on or off */
                     int dim    /* IN dimension of /layers/ and /on/ */
                     )
{
  GraphicObj *g_p, *prev_g_p;
  LIST from_list, to_list;
  int L;
  
  for (L = 0; L < dim; L++) {
    if (layers[L] >= 0 && layers[L] < MAX_LAYER) {
			if(v_p->view_layers[layers[L]].layer_on != on[L]){
				v_p->view_layers[layers[L]].layer_on = on[L];
				if (on[L])
					from_list = v_p->invisible_GO, to_list = v_p->visible_GO;
				else
					from_list = v_p->visible_GO, to_list = v_p->invisible_GO;
				prev_g_p = NULL;
				g_p = (GraphicObj *)get_list_next(from_list, prev_g_p);
				while(g_p != NULL) {
					if (EGGetLayer(g_p) == layers[L]) 
						link_to_tail(to_list, unlink_current_prev(from_list));
					else
						prev_g_p = g_p;
					g_p = (GraphicObj *)get_list_next(from_list, prev_g_p);
				}
      }
    }
  }
  EVForceRedisplay(v_p);
}





void 
EVFastSetMultLayersOnOff(EView *v_p,
												 int layers[],  /* IN layer numbers */
												 BOOLEAN on[], /* IN flags whether layer[i] is to be */
                                       /* set on or off */
												 int dim    /* IN dimension of /layers/ and /on/ */
												 )
{
  GraphicObj *g_p_vis, *g_p_invis, *prev_g_p;
  LIST visible_list, invisible_list;
	BOOLEAN layers_to_show[MAX_LAYER], layers_to_hide[MAX_LAYER], show = NO, hide = NO;
  int L, layer;
  
	if(dim <= 0)return;

	for (L = 0; L < MAX_LAYER; L++)
		layers_to_show[L] = layers_to_hide[L] = NO;

  for (L = 0; L < dim; L++) {
		layer = layers[L];
    if (layer >= 0 && layer < MAX_LAYER){
			v_p->view_layers[layer].layer_on = on[L];
			if(on[L] == YES)
				layers_to_show[layer] = show = YES;
			else
				layers_to_hide[layer] = hide = YES;
		}
	}

	if(show == NO && hide == NO)return;

	visible_list = v_p->visible_GO;
	invisible_list = v_p->invisible_GO;

	prev_g_p = NULL;
	g_p_vis = (GraphicObj *)get_list_next(visible_list, prev_g_p);
	g_p_invis = (GraphicObj *)get_list_next(invisible_list, prev_g_p);

	if(hide == YES && g_p_vis != NULL){
		while(g_p_vis != NULL) {
			if(layers_to_hide[EGGetLayer(g_p_vis)] == YES)
				link_to_tail(invisible_list, unlink_current_prev(visible_list));
			else
				prev_g_p = g_p_vis;
			g_p_vis = (GraphicObj *)get_list_next(visible_list, prev_g_p);
		}
	}

	if(show == YES && g_p_invis != NULL){
		prev_g_p = (GraphicObj *)get_list_prev(invisible_list, g_p_invis);
		g_p_invis = (GraphicObj *)get_list_next(invisible_list, prev_g_p);

		while(g_p_invis != NULL) {
			if(layers_to_show[EGGetLayer(g_p_invis)] == YES)
				link_to_tail(visible_list, unlink_current_prev(invisible_list));
			else
				prev_g_p = g_p_invis;
			g_p_invis = (GraphicObj *)get_list_next(invisible_list, prev_g_p);
		}
	}

  EVForceRedisplay(v_p);
}






BOOLEAN
EVGetLayerOnOff(EView *v_p, int layer)
{
  if (layer >= 0 && layer < MAX_LAYER) {
    return v_p->view_layers[layer].layer_on;
  } else
    return NO;
}


void 
EVInstallHandler(EView *v_p, EventHandlerP handler,
		 StartProcP   start_proc,
		 caddr_t        start_proc_data,
		 SuspendProcP suspend_proc,
		 caddr_t        suspend_proc_data,
		 ResumeProcP  resume_proc,
		 caddr_t        resume_proc_data,
		 RemoveProcP  remove_proc,
		 caddr_t        remove_proc_data)
{
  v_p->the_active_handler.handler           = handler;
  v_p->the_active_handler.start_proc        = start_proc;
  v_p->the_active_handler.start_proc_data   = start_proc_data;
  v_p->the_active_handler.suspend_proc      = suspend_proc;
  v_p->the_active_handler.suspend_proc_data = suspend_proc_data;
  v_p->the_active_handler.resume_proc       = resume_proc;
  v_p->the_active_handler.resume_proc_data  = resume_proc_data;
  v_p->the_active_handler.remove_proc       = remove_proc;
  v_p->the_active_handler.remove_proc_data  = remove_proc_data;
  generate_input_handler_message(v_p, STARTING_INPUT_HANDLER);
  if (v_p->the_active_handler.start_proc != NULL)
    (*(v_p->the_active_handler.start_proc))(v_p, start_proc_data);
}



void 
EVUninstallHandler(EView *v_p)
{
  RemoveProcP  remove_proc;
  caddr_t        remove_proc_data;

  remove_proc      = v_p->the_active_handler.remove_proc;
  remove_proc_data = v_p->the_active_handler.remove_proc_data;
  CLEAR_HANDLER_DATA(v_p, the_active_handler);
  ERptPrompt (ELIXIR_PROMPT_CLASS,
              MESSAGE_CLEAR_PROMPT,
              elixir_default_prompts[MESSAGE_CLEAR_PROMPT]);
  if (remove_proc != NULL)
    (*remove_proc)(v_p, remove_proc_data);
}



void 
EVSuspendActiveHandler(EView *v_p)
{
  SuspendProcP      suspend_proc;
  caddr_t        suspend_proc_data;

  suspend_proc      = v_p->the_active_handler.suspend_proc;
  suspend_proc_data = v_p->the_active_handler.suspend_proc_data;
  ERptPrompt (ELIXIR_PROMPT_CLASS,
              MESSAGE_CLEAR_PROMPT,
              elixir_default_prompts[MESSAGE_CLEAR_PROMPT]); 
  if (v_p->the_active_handler.handler != NULL) {
    COPY_HANDLER_DATA(v_p, the_active_handler, the_suspended_handler);
    CLEAR_HANDLER_DATA(v_p, the_active_handler);
  }
  if (suspend_proc != NULL)
    (*suspend_proc)(v_p, suspend_proc_data);
}



void 
EVResumeSuspendedHandler(EView *v_p)
{
  ResumeProcP      resume_proc;
  caddr_t        resume_proc_data;

  resume_proc      = v_p->the_active_handler.resume_proc;
  resume_proc_data = v_p->the_active_handler.resume_proc_data;
  ERptPrompt (ELIXIR_PROMPT_CLASS,
              MESSAGE_CLEAR_PROMPT,
              elixir_default_prompts[MESSAGE_CLEAR_PROMPT]); 
  if (v_p->the_suspended_handler.handler != NULL) {
    COPY_HANDLER_DATA(v_p, the_suspended_handler, the_active_handler);
    CLEAR_HANDLER_DATA(v_p, the_suspended_handler);
  }
  generate_input_handler_message(v_p, RESUMING_INPUT_HANDLER);
  if (resume_proc != NULL)
    (*resume_proc)(v_p, resume_proc_data);
}

static void
generate_input_handler_message(EView *v_p, int flag)
{
  XEvent ev;
  int i;

  ev.xclient.type         = ClientMessage;
  ev.xclient.send_event   = True;
  ev.xclient.display      = XtDisplay(v_p->top_view_widget);
  ev.xclient.window       = XtWindow(v_p->top_view_widget);
  ev.xclient.message_type = flag;
  ev.xclient.format       = 32;
  for (i = 0; i < 5; i++)
    ev.xclient.data.l[i] = flag;
  if (XSendEvent(XtDisplay(v_p->view_widget), XtWindow(v_p->view_widget),
                 True, NoEventMask, &ev) == 0)
    fprintf(stderr, "generate_input_handler_message failed\n");
}


void 
EVWCtoVC(EView *v_p, WCRec *w, VCRec *v)
{
  TransfPntWCToVC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
		  &(v_p->VCOrigin), w, v);
}



void 
EVVCtoWC(EView *v_p, VCRec *v, WCRec *w)
{
  TransfPntVCToWC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
		  &(v_p->VCOrigin), v, w);  
}



void 
EVSetVectUp(EView *v_p, WCRec *v)
{
  v_p->VectUp.x = v->x;
  v_p->VectUp.y = v->y;
  v_p->VectUp.z = v->z;
  EVSetupViewOrientation(v_p);
}



void
EVGetVectUp(EView *v_p, WCRec *v)
{
  v->x = v_p->VectUp.x;
  v->y = v_p->VectUp.y;
  v->z = v_p->VectUp.z;
}



void
EVSetNormal(EView *v_p, WCRec *v)
{
  v_p->Normal.x = v->x;
  v_p->Normal.y = v->y;
  v_p->Normal.z = v->z;
  EVSetupViewOrientation(v_p);
}



void 
EVGetNormal(EView *v_p, WCRec *v)
{
  v->x = v_p->Normal.x;
  v->y = v_p->Normal.y;
  v->z = v_p->Normal.z;
}



void 
EVRotateByMouse(EView *v_p, FPNum aboutu, FPNum aboutv)
{
  WCRec v, oldv;

  oldv.x = v_p->VectorV.x; oldv.y = v_p->VectorV.y; oldv.z = v_p->VectorV.z;
  v.x = aboutu*v_p->VectorU.x;
  v.y = aboutu*v_p->VectorU.y;
  v.z = aboutu*v_p->VectorU.z;
  RotVectAboutVect(&v, &(v_p->Normal));
  RotVectAboutVect(&v, &(v_p->VectUp));
  v.x = aboutv*v_p->VectUpAtStart.x;
  v.y = aboutv*v_p->VectUpAtStart.y;
  v.z = aboutv*v_p->VectUpAtStart.z;
  RotVectAboutVect(&v, &(v_p->Normal));
  RotVectAboutVect(&v, &(v_p->VectUp));
  EVSetupViewOrientation(v_p);   
  EVFastRedraw(v_p);
}


void 
EVPreserveVectUp (EView *v_p, BOOLEAN preserve)
{
  v_p->preserve_vup = preserve;
} 


static BOOLEAN inhibit_view_orientation_setup = NO;

void 
EVInhibitViewOrientationSetup(void)
{
  inhibit_view_orientation_setup = YES;
}



void 
EVSetupViewOrientation(EView *v_p)
{
  if (inhibit_view_orientation_setup) 
    inhibit_view_orientation_setup = NO;
  else {
    if (VectorsColinear((double)1.e-9, &(v_p->VectUp), &(v_p->Normal))) {
      v_p->Normal.x       = 0;	/* Setup top view */
      v_p->Normal.y       = 0;
      v_p->Normal.z       = 1;
      v_p->VectUp.x       = 0;
      v_p->VectUp.y       = 1;
      v_p->VectUp.z       = 0;
    }
    ComputeRotMatVects(&(v_p->VectUp), &(v_p->Normal),
		       &(v_p->VectorU), &(v_p->VectorV));
  }
}



void 
EVSetViewOrientation(EView *v_p, EViewOrientation vo)
{
	/* WCRec vrp; */

  switch (vo) {
  case VIEW_ORIENT_TOP:
    v_p->Normal.x = 0; v_p->Normal.y = 0; v_p->Normal.z = 1;
    v_p->VectUp.x = 0; v_p->VectUp.y = 1; v_p->VectUp.z = 0;
    break;
  case VIEW_ORIENT_BOTTOM:
    v_p->Normal.x = 0; v_p->Normal.y = 0; v_p->Normal.z = -1;
    v_p->VectUp.x = 0; v_p->VectUp.y = 1; v_p->VectUp.z = 0;
    break;
  case VIEW_ORIENT_LEFT:
    v_p->Normal.x = 0; v_p->Normal.y = -1; v_p->Normal.z = 0;
    v_p->VectUp.x = 0; v_p->VectUp.y = 0; v_p->VectUp.z = 1;
    break;
  case VIEW_ORIENT_RIGHT:
    v_p->Normal.x = 0; v_p->Normal.y = 1; v_p->Normal.z = 0;
    v_p->VectUp.x = 0; v_p->VectUp.y = 0; v_p->VectUp.z = 1;
    break;
  case VIEW_ORIENT_FRONT:
    v_p->Normal.x = 1; v_p->Normal.y = 0; v_p->Normal.z = 0;
    v_p->VectUp.x = 0; v_p->VectUp.y = 0; v_p->VectUp.z = 1;
    break;
  case VIEW_ORIENT_BACK:
    v_p->Normal.x = -1; v_p->Normal.y = 0; v_p->Normal.z = 0;
    v_p->VectUp.x = 0; v_p->VectUp.y = 0; v_p->VectUp.z = 1;
    break;
  case VIEW_ORIENT_ISO:
    v_p->Normal.x = 3; v_p->Normal.y = 2; v_p->Normal.z = 1;
    v_p->VectUp.x = 0; v_p->VectUp.y = 0; v_p->VectUp.z = 1;
    break;
  default:
    break;
  }
  EVSetupViewOrientation(v_p);
	/*
	EVGetVRP(v_p, &vrp);
	EVSetVRP(v_p, &vrp);
	*/
  EVWCtoVC(v_p, &(v_p->ViewRefPoint), &(v_p->ViewRefPointVC));
}




void 
EVSetRenderMode(EView *v_p, ERenderingType mode)
{
  if (mode != NORMAL_RENDERING &&
      mode != CONST_SHADING_RENDERING &&
      mode != FILLED_HIDDEN_RENDERING &&
      mode != WIRE_RENDERING)
    mode = NORMAL_RENDERING;
  v_p->render_mode = mode;
}



ERenderingType 
EVGetRenderMode(EView *v_p)
{
  return v_p->render_mode;
}



void 
EVSetShadeMode(EView *v_p, EShadingType mode)
{
  if (mode != DITHER_SHADING &&
      mode != COLOR_SHADING &&
      mode != NO_SHADING)
    mode = NO_SHADING;
  v_p->shade_mode = mode;
}



EShadingType EVGetShadeMode(EView *v_p)
{
  return v_p->shade_mode;
}


void 
EVStoreRenderMode(EView *v_p)
{

/* prevent next call to EVStoreRenderMode without call to EVRestoreRenderMode */

	if(v_p->backup_render_mode == -1)v_p->backup_render_mode = v_p->render_mode;
}


void 
EVRestoreRenderMode(EView *v_p)
{
	if(v_p->backup_render_mode != -1){
		v_p->render_mode = v_p->backup_render_mode;
		v_p->backup_render_mode=-1;
	}
}




void 
EVSetDirTowardsLight(EView *v_p, VCRec *vect)
{
  if (fabs(vect->u) == 0 && fabs(vect->v) == 0 && fabs(vect->n) == 0) {
    v_p->dir_towards_light.u = 1;
    v_p->dir_towards_light.v = 2;
    v_p->dir_towards_light.n = 3;
  } else {
    v_p->dir_towards_light.u = vect->u;
    v_p->dir_towards_light.v = vect->v;
    v_p->dir_towards_light.n = vect->n;
  }
  NormalizeVect3((WCRec *)&(v_p->dir_towards_light));
}



FPNum 
EVGetLightDirVsNormalCos(EView *v_p, VCRec *normal)
{
  if (fabs(v_p->dir_towards_light.u) == 0 &&
      fabs(v_p->dir_towards_light.v) == 0 &&
      fabs(v_p->dir_towards_light.n) == 0) {
    v_p->dir_towards_light.u = 1;
    v_p->dir_towards_light.v = 1;
    v_p->dir_towards_light.n = 1;
    NormalizeVect3((WCRec *)&(v_p->dir_towards_light));
  }
  NormalizeVect3((WCRec *)normal);
  return DotProd3((WCRec *)&(v_p->dir_towards_light), (WCRec *)normal);
}



void  
recompute_dc_size(EView *v_p)
{
  unsigned int wdth, hght;
  FPNum actual_ratio;
  
  EVGetDimsDC(v_p, &wdth, &hght);

  actual_ratio = (FPNum)hght/(FPNum)wdth;
  if (actual_ratio > (v_p->view_dims_VC.v / v_p->view_dims_VC.u))
    v_p->view_dims_VC.v = v_p->view_dims_VC.u * actual_ratio;
  else
    v_p->view_dims_VC.u = v_p->view_dims_VC.v / actual_ratio;
  v_p->view_dims_DC.x = wdth;
  v_p->view_dims_DC.y = hght;
}



void
EVGetDimsDC(EView *v_p, unsigned int *width, unsigned int *height)
{
  Widget w;
  Window root;
  int x, y;
  unsigned int depth, bwidth;

  w = EVViewPToWidget(v_p);
  XMapWindow(XtDisplay(w), XtWindow(w));
  XGetGeometry(XtDisplay(w), XtWindow(w), &root, &x, &y, width, height,
	       &bwidth, &depth);
}



void 
EVDrawGraphics(EView *v_p, GraphicObj *g_p)
{
  if (v_p->view_layers[EGGetLayer(g_p)].layer_on) {
    currently_dispatched_g = g_p;
    EGDrawGraphics(v_p, g_p);

    if (v_p->render_mode == FILLED_HIDDEN_RENDERING ||
	v_p->render_mode == CONST_SHADING_RENDERING) {
      if (ZBufferInitedFor() == v_p)
	ZBufferPaint(XtDisplay(v_p->view_widget), v_p->draw_into,
		     v_p->writableGC);
    }
    currently_dispatched_g = NULL;
  }
}



void 
EVXORDrawGraphics(EView *v_p, GraphicObj *g_p)
{
  EGXORDrawGraphics(v_p, g_p);
}



void 
EVHiliteGraphics(EView *v_p, GraphicObj *g_p)
{
  if (v_p->view_layers[EGGetLayer(g_p)].layer_on)
    EGHiliteGraphics(v_p, g_p);
}



void 
EVUnhiliteGraphics(EView *v_p, GraphicObj *g_p)
{
  if (v_p->view_layers[EGGetLayer(g_p)].layer_on)
    EGUnhiliteGraphics(v_p, g_p); 
}



void 
EVEraseGraphics(EView *v_p, GraphicObj *g_p)
{
  if (v_p->view_layers[EGGetLayer(g_p)].layer_on)
    EGEraseGraphics(v_p, g_p); 
}



void 
EVSetConstrPlaneLock(EView *v_p, BOOLEAN on)
{
  v_p->constrplane_lock_on = on;
}



void
EVSetConstrPlaneCenter(EView *v_p, WCRec *center)
{
  v_p->constrplane_center.x = center->x;
  v_p->constrplane_center.y = center->y;
  v_p->constrplane_center.z = center->z;
}


void
EVGetConstrPlaneCenter(EView *v_p, WCRec *center)
{
  center->x = v_p->constrplane_center.x;
  center->y = v_p->constrplane_center.y;
  center->z = v_p->constrplane_center.z;
}

void
EVSetConstrPlaneOrientation(EView *v_p, WCRec *normal, WCRec *vup)
{
  WCRec Z = {0, 0, 1}, VUP, X = {1, 0, 0}, NORMAL, U, V;
  
  NORMAL.x = normal->x; NORMAL.y = normal->y; NORMAL.z = normal->z;
  VUP.x = vup->x; VUP.y = vup->y; VUP.z = vup->z;
  if (VectorsColinear(0.001, &NORMAL, vup)) {
    if (VectorsColinear(0.001, &NORMAL, &Z)) {
      VUP.x = X.x; VUP.y = X.y; VUP.z = X.z;
    } else {
      VUP.x = Z.x; VUP.y = Z.y; VUP.z = Z.z;
    }
  }
  if (!ComputeRotMatVects(&VUP, &NORMAL, &U, &V)) {
    v_p->constrplane_normal.x = 0;
    v_p->constrplane_normal.y = 0;
    v_p->constrplane_normal.z = 1;
    v_p->constrplane_u.x      = 1;
    v_p->constrplane_u.y      = 0;
    v_p->constrplane_u.z      = 0;
    v_p->constrplane_v.x      = 0;
    v_p->constrplane_v.y      = 1;
    v_p->constrplane_v.z      = 0;
  } else {
    v_p->constrplane_normal.x = NORMAL.x;
    v_p->constrplane_normal.y = NORMAL.y;
    v_p->constrplane_normal.z = NORMAL.z;
    v_p->constrplane_u.x      = U.x;
    v_p->constrplane_u.y      = U.y;
    v_p->constrplane_u.z      = U.z;
    v_p->constrplane_v.x      = V.x;
    v_p->constrplane_v.y      = V.y;
    v_p->constrplane_v.z      = V.z;
  }
}


void
EVGetConstrPlaneOrientation(EView *v_p, WCRec *u, WCRec *v, WCRec *normal)
{
  u->x      = v_p->constrplane_u.x;
  u->y      = v_p->constrplane_u.y;
  u->z      = v_p->constrplane_u.z;
  v->x      = v_p->constrplane_v.x;
  v->y      = v_p->constrplane_v.y;
  v->z      = v_p->constrplane_v.z;
  normal->x = v_p->constrplane_normal.x;
  normal->y = v_p->constrplane_normal.y;
  normal->z = v_p->constrplane_normal.z;
}



void 
AdjustToConstrPlaneLock(EView *v_p, WCRec *p)
{
  FPNum d;
  WCRec Lp0, Lp1, v, diff;
  
  Lp0.x = p->x;   Lp0.y = p->y;  Lp0.z = p->z;
  Lp1.x = Lp0.x + v_p->Normal.x;
  Lp1.y = Lp0.y + v_p->Normal.y;
  Lp1.z = Lp0.z + v_p->Normal.z;

  v.x = Lp1.x - Lp0.x;  v.y = Lp1.y - Lp0.y;  v.z = Lp1.z - Lp0.z;
  d = DotProd3(&(v_p->constrplane_normal), &v);

  if (d == 0)
    return; /* no intersection of line and constrplane */

  diff.x = v_p->constrplane_center.x - Lp0.x;
  diff.y = v_p->constrplane_center.y - Lp0.y;
  diff.z = v_p->constrplane_center.z - Lp0.z;

  d = DotProd3(&diff, &(v_p->constrplane_normal))/d;

  p->x = Lp0.x + d * v.x;
  p->y = Lp0.y + d * v.y;
  p->z = Lp0.z + d * v.z;
}



/* It is assumed that the point P has been adjusted to the construction */
/* plane before by AdjustToConstrPlaneLock. */
void 
AdjustToConstrPlaneGridLock(EView *v_p, WCRec *p)
{
  double floor_coord;
  double dx, dy, ox, oy;
  double pu, pv;
  WCRec v;
  
  ox = v_p->grid_origin.u;
  oy = v_p->grid_origin.v;
  dx = v_p->grid_deltas.u;
  dy = v_p->grid_deltas.v;

  v.x = p->x - v_p->constrplane_center.x;
  v.y = p->y - v_p->constrplane_center.y;
  v.z = p->z - v_p->constrplane_center.z;
  pu = DotProd3(&v, &v_p->constrplane_u);
  pv = DotProd3(&v, &v_p->constrplane_v);
  
  floor_coord = (FPNum)floor((double)((pu - ox)/dx)) * dx + ox;
  if ((pu - 0.5 * dx) < floor_coord)
    pu = floor_coord;
  else
    pu = floor_coord + dx;
  floor_coord = (FPNum)floor((double)((pv - oy)/dy)) * dy + oy;
  if ((pv - 0.5 * dy) < floor_coord)
    pv = floor_coord;
  else
    pv = floor_coord + dy;

  p->x = (v_p->constrplane_center.x
          + pu * v_p->constrplane_u.x
          + pv * v_p->constrplane_v.x);
  p->y = (v_p->constrplane_center.y
          + pu * v_p->constrplane_u.y
          + pv * v_p->constrplane_v.y);
  p->z = (v_p->constrplane_center.z
          + pu * v_p->constrplane_u.z
          + pv * v_p->constrplane_v.z);
}


void
EVSetFGBGViewPlaneDepth(EView *v_p, FPNum fg_depth, FPNum bg_depth)
{
  if (fg_depth <= bg_depth) {
    v_p->bg_view_plane_n = -FLT_MAX; /* -- far away */
    v_p->fg_view_plane_n =  FLT_MAX; /* -- toward the viewer */
  } else {
    v_p->bg_view_plane_n = bg_depth;
    v_p->fg_view_plane_n = fg_depth;
  }
}


void
EVSetFGViewPlaneDepth(EView *v_p, FPNum fg_depth)
{
	v_p->fg_view_plane_n = fg_depth;
}


void
EVSetBGViewPlaneDepth(EView *v_p, FPNum bg_depth)
{
	v_p->bg_view_plane_n = bg_depth;
}


void
EVGetFGBGViewPlaneDepth(EView *v_p, FPNum *fg_depth, FPNum *bg_depth)
{
  *bg_depth = v_p->bg_view_plane_n;
  *fg_depth = v_p->fg_view_plane_n;
}



void
EVSetFGBGViewPlaneClipOnOff(EView *v_p, BOOLEAN flag)
{
  v_p->should_clip_by_fg_bg_view_planes = flag;
}



BOOLEAN
EVGetFGBGViewPlaneClipOnOff(EView *v_p)
{
  return v_p->should_clip_by_fg_bg_view_planes;
}



void
EVSetModelClipPlane(EView *v_p, int which_plane,
                    WCRec *center, WCRec *normal, BOOLEAN on_flag)
{
  WCRec NORMAL;
  
  NORMAL.x = normal->x; NORMAL.y = normal->y; NORMAL.z = normal->z;

  v_p->model_clip_plane.clip_is_on = on_flag;
  v_p->model_clip_plane.center.x   = center->x;
  v_p->model_clip_plane.center.y   = center->y;
  v_p->model_clip_plane.center.z   = center->z;
  if (!NormalizeVect3(&NORMAL)) {
    v_p->model_clip_plane.normal.x   = 0;
    v_p->model_clip_plane.normal.y   = 0;
    v_p->model_clip_plane.normal.z   = 1;
  } else {
    v_p->model_clip_plane.normal.x   = NORMAL.x;
    v_p->model_clip_plane.normal.y   = NORMAL.y;
    v_p->model_clip_plane.normal.z   = NORMAL.z;
  }
}


void
EVGetModelClipPlane(EView *v_p, int which_plane,
                    WCRec *center, WCRec *normal, BOOLEAN *on)
{
  *on = v_p->model_clip_plane.clip_is_on;
  center->x = v_p->model_clip_plane.center.x;
  center->y = v_p->model_clip_plane.center.y;
  center->z = v_p->model_clip_plane.center.z;
  normal->x = v_p->model_clip_plane.normal.x;
  normal->y = v_p->model_clip_plane.normal.y;
  normal->z = v_p->model_clip_plane.normal.z;
  
}

void
EVSetModelPlaneClipOnOff(EView *v_p, BOOLEAN flag)
{
  v_p->should_clip_by_model_planes = flag;
}


BOOLEAN
EVGetModelPlaneClipOnOff(EView *v_p)
{
  return v_p->should_clip_by_model_planes;
}
