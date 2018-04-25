 
#include "Eprimtv.h"

 
void 
EVCBezC3DPrimitive(EView        *v_p,
                   GraphicObj   *g_p,
		   WCRec         wcpoints[4],
		   int           style,
		   EPixel   pixel,
		   unsigned      width,
		   int           tessel_intervals,   
		   BOOLEAN       show_poly,
		   EDrawMode     draw_mode)   
{
  VCRec f, s, lf, ls;
  FPNum t, deltat = 1.0/tessel_intervals;
  FPNum b0, b1, b2, b3;
  int i;
  VCRec vboxll, vboxur, points[4];
  int x1, x2, y1, y2;
  EV_line_end_point_rec p1, p2, p1_return, p2_return;
  BOOLEAN test_mclip, to_draw = YES;
  VCRec model_clip_plane_center, model_clip_plane_normal;
  
  EV_display                    = XtDisplay(v_p->view_widget);
  EV_curr_win                   = v_p->draw_into;
  EV_should_clip_by_view_planes = v_p->should_clip_by_fg_bg_view_planes;
  EV_fg_plane_depth             = v_p->fg_view_plane_n;
  EV_bg_plane_depth             = v_p->bg_view_plane_n;

  if      (draw_mode == DRAW_MODE_DRAW) {
    EV_curr_GC    = v_p->writableGC;
    EV_curr_pixel = pixel;
    XSetForeground(EV_display, EV_curr_GC, pixel);
    XSetLineAttributes(EV_display, EV_curr_GC, width, style,
                       CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_ERASE) {
    EV_curr_GC    = v_p->eraseGC;
    EV_curr_pixel = v_p->background;
    XSetLineAttributes(EV_display, EV_curr_GC, width, style,
                       CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_HILITE) {
    EV_curr_GC    = v_p->hiliteGC;
    EV_curr_pixel = v_p->foreground;
  } else if (draw_mode == DRAW_MODE_UNHILITE) {
    EV_curr_GC    = v_p->erasehiliteGC;
    EV_curr_pixel = v_p->background;
  } else if (draw_mode == DRAW_MODE_XORDRAW) {
    EV_curr_GC    = v_p->defaultXORGC;
    EV_curr_pixel = v_p->foreground;
  }
  EV_render_mode = v_p->render_mode;
  EV_shade_mode  = v_p->shade_mode;
  EVGetViewBox(v_p, &vboxll, &vboxur);

  EVWCtoVC(v_p, &wcpoints[0], &points[0]);
  EVWCtoVC(v_p, &wcpoints[1], &points[1]);
  EVWCtoVC(v_p, &wcpoints[2], &points[2]); 
  EVWCtoVC(v_p, &wcpoints[3], &points[3]);

  
  /* Model clipping */
  test_mclip = (v_p->should_clip_by_model_planes
                && v_p->model_clip_plane.clip_is_on
                && EGIsClippable(g_p));
  if (test_mclip) {
    int how_many_are_in = 0;
    WCRec Zero = {0, 0, 0};

    TransfPntWCToVC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
                    &(v_p->VCOrigin), &v_p->model_clip_plane.center,
                    &model_clip_plane_center);
    TransfPntWCToVC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
                    &Zero, &v_p->model_clip_plane.normal,
                    &model_clip_plane_normal);
                    
    for (i = 0; i < 4; i++) {
      COPY_VCREC(&p1.loc.vc, &points[i]);
      if (point_is_in_IN_halfspace(&p1, (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal))
        how_many_are_in++;
    }
    if (how_many_are_in == 0)
      return;
    if (how_many_are_in == 4)
      test_mclip = NO; /* no need to test -- the whole curve is in */
    else
      if (EGInvisibleWhenClipped(g_p))
        return;
  }
      
  if (EV_render_mode == WIRE_RENDERING)
    tessel_intervals = 4, deltat = 1./tessel_intervals;
  
  f.u = points[0].u; f.v = points[0].v; f.n = points[0].n;
  for (t = deltat, i = 0; i < tessel_intervals-1; t += deltat, i++) {
    CubicBernstein(t, &b0, &b1, &b2, &b3);
    s.u =
      b0 * points[0].u +
	b1 * points[1].u +
	  b2 * points[2].u +
	    b3 * points[3].u;
    s.v =
      b0 * points[0].v +
	b1 * points[1].v +
	  b2 * points[2].v +
	    b3 * points[3].v;
    s.n =
      b0 * points[0].n +
	b1 * points[1].n +
	  b2 * points[2].n +
	    b3 * points[3].n;
    lf.u = f.u, lf.v = f.v, lf.n = f.n;
    ls.u = s.u, ls.v = s.v, ls.n = s.n;
    if (test_mclip) {
      COPY_VCREC(&p1.loc.vc, &lf);
      COPY_VCREC(&p2.loc.vc, &ls);
      to_draw = (clip_line_by_a_plane(NO, &p1, &p2,
                                      (WCRec *)&model_clip_plane_center,
                                      (WCRec *)&model_clip_plane_normal,
                                      &p1_return, &p2_return) != 0);
      if (to_draw) {
        COPY_VCREC(&lf, &p1_return.loc.vc);
        COPY_VCREC(&ls, &p2_return.loc.vc);
      }
    }
    if (to_draw && ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                                  &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
      EVVCtoDC(v_p, &lf, &x1, &y1);
      EVVCtoDC(v_p, &ls, &x2, &y2);
      if (EV_render_mode == NORMAL_RENDERING ||
	  EV_render_mode == WIRE_RENDERING) {
	XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
      } else {
	BHM(x1, y1, lf.n, x2, y2, ls.n);
      }
    }
    f.u = s.u; f.v = s.v; f.n = s.n;
  }
  s.u = points[3].u; s.v = points[3].v; s.n = points[3].n;
  lf.u = f.u, lf.v = f.v, lf.n = f.n;
  ls.u = s.u, ls.v = s.v, ls.n = s.n;
  if (test_mclip) {
    COPY_VCREC(&p1.loc.vc, &lf);
    COPY_VCREC(&p2.loc.vc, &ls);
    to_draw = (clip_line_by_a_plane(NO, &p1, &p2,
                                    (WCRec *)&model_clip_plane_center,
                                    (WCRec *)&model_clip_plane_normal,
                                    &p1_return, &p2_return) != 0);
    if (to_draw) {
      COPY_VCREC(&lf, &p1_return.loc.vc);
      COPY_VCREC(&ls, &p2_return.loc.vc);
    }
  }
  if (to_draw && ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                                &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
    EVVCtoDC(v_p, &lf, &x1, &y1);
    EVVCtoDC(v_p, &ls, &x2, &y2);
    if (EV_render_mode == NORMAL_RENDERING ||
	EV_render_mode == WIRE_RENDERING) {
      XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
    } else {
      BHM(x1, y1, lf.n, x2, y2, ls.n);
    }
  }
  if (show_poly) {
    if (draw_mode == DRAW_MODE_DRAW)
      EV_curr_GC    = v_p->constrCopyGC, EV_curr_pixel = v_p->foreground;
    else if (draw_mode == DRAW_MODE_XORDRAW)
      EV_curr_GC    = v_p->defaultXORGC, EV_curr_pixel = v_p->foreground;
    else
      EV_curr_GC    = v_p->constrEraseGC, EV_curr_pixel = v_p->background;
    
    if (draw_mode == DRAW_MODE_DRAW || draw_mode == DRAW_MODE_ERASE ||
	draw_mode == DRAW_MODE_XORDRAW) {
      lf.u = points[0].u, lf.v = points[0].v, lf.n = points[0].n;
      ls.u = points[1].u, ls.v = points[1].v, ls.n = points[1].n;
      if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
			 &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
	EVVCtoDC(v_p, &lf, &x1, &y1);
	EVVCtoDC(v_p, &ls, &x2, &y2);
	if (EV_render_mode == NORMAL_RENDERING ||
	    EV_render_mode == WIRE_RENDERING) {
	  XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
	} else {
	  BHM(x1, y1, lf.n, x2, y2, ls.n);
	}
      }
      lf.u = points[2].u, lf.v = points[2].v, lf.n = points[2].n;
      ls.u = points[1].u, ls.v = points[1].v, ls.n = points[1].n;
      if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
			 &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
	EVVCtoDC(v_p, &lf, &x1, &y1);
	EVVCtoDC(v_p, &ls, &x2, &y2);
	if (EV_render_mode == NORMAL_RENDERING ||
	    EV_render_mode == WIRE_RENDERING) {
	  XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
	} else {
	  BHM(x1, y1, lf.n, x2, y2, ls.n);
	}
      }
      lf.u = points[2].u, lf.v = points[2].v, lf.n = points[2].n;
      ls.u = points[3].u, ls.v = points[3].v, ls.n = points[3].n;
      if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
			 &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
	EVVCtoDC(v_p, &lf, &x1, &y1);
	EVVCtoDC(v_p, &ls, &x2, &y2);
	if (EV_render_mode == NORMAL_RENDERING ||
	    EV_render_mode == WIRE_RENDERING) {
	  XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
	} else {
	  BHM(x1, y1, lf.n, x2, y2, ls.n);
	}
      }
    } /* if (draw_mode...) */
  } /* if (show_poly) */
}

