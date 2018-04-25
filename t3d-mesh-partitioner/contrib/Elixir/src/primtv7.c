
#include "Eprimtv.h"

void 
EVTriangle3DPrimitive(EView        *v_p,
                      GraphicObj   *g_p,
		      WCRec        *f,
		      WCRec        *s,
		      WCRec        *t,
		      EPixel        pixel,
		      int           fill,
                      int           edge_flag,
                      EPixel        edge_pixel,
		      float         shrink,
		      EDrawMode     draw_mode)
{
  int x1, x2, y1, y2, x3, y3, x4, y4;
  VCRec vc1, vc2, vc3, vc4, ll, ur, tll, tur;
  EBoxVC2DRec vbox, tbox;
  WCRec sf, ss, st;
  double tmp;
  EV_line_end_point_rec p1, p2, p3;
  EV_triangle_clip_result clip_result = EV_TRIANGLE_IS_IN;
  EV_line_end_point_rec  pnts_return[4];
  int npnts;
  VCRec model_clip_plane_center, model_clip_plane_normal;
  
  EV_display                    = XtDisplay(v_p->view_widget);
  EV_curr_win                   = v_p->draw_into;
  EV_should_clip_by_view_planes = v_p->should_clip_by_fg_bg_view_planes;
  EV_fg_plane_depth             = v_p->fg_view_plane_n;
  EV_bg_plane_depth             = v_p->bg_view_plane_n;

	COPY_WCREC(&sf, f);
	COPY_WCREC(&ss, s);
	COPY_WCREC(&st, t);
  if (shrink != 1) {
    SHRINK_TRIANGLE_DATA(shrink, sf.x, ss.x, st.x);
    SHRINK_TRIANGLE_DATA(shrink, sf.y, ss.y, st.y);
    SHRINK_TRIANGLE_DATA(shrink, sf.z, ss.z, st.z);
	}
	EVWCtoVC(v_p, &sf, &vc1);
	EVWCtoVC(v_p, &ss, &vc2);
	EVWCtoVC(v_p, &st, &vc3);
 
  EVGetViewBox(v_p, &ll, &ur);
  tmp = min(vc1.u, vc2.u); tll.u = min(tmp, vc3.u);
  tmp = min(vc1.v, vc2.v); tll.v = min(tmp, vc3.v);
  tmp = min(vc1.n, vc2.n); tll.n = min(tmp, vc3.n);
  tmp = max(vc1.u, vc2.u); tur.u = max(tmp, vc3.u);
  tmp = max(vc1.v, vc2.v); tur.v = max(tmp, vc3.v);
  tmp = max(vc1.n, vc2.n); tur.n = max(tmp, vc3.n);
  vbox.left  = ll.u; vbox.bottom  = ll.v;
  vbox.right = ur.u; vbox.top     = ur.v;
  tbox.left  = tll.u; tbox.bottom = tll.v;
  tbox.right = tur.u; tbox.top    = tur.v;

  if (v_p->should_clip_by_model_planes  && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
      WCRec Zero = {0, 0, 0};

      TransfPntWCToVC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
                      &(v_p->VCOrigin), &v_p->model_clip_plane.center,
                      &model_clip_plane_center);
      TransfPntWCToVC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
                      &Zero, &v_p->model_clip_plane.normal,
                      &model_clip_plane_normal);
      COPY_VCREC(&p1.loc.vc, &vc1);
      COPY_VCREC(&p2.loc.vc, &vc2);
      COPY_VCREC(&p3.loc.vc, &vc3);
      clip_result = clip_triangle_by_a_plane(NO, &p1, &p2, &p3,
                                             (WCRec *)&model_clip_plane_center,
                                             (WCRec *)&model_clip_plane_normal,
                                             pnts_return, &npnts);
      if (clip_result == EV_TRIANGLE_IS_OUT)
        return;
      if (clip_result == EV_TRIANGLE_CLIPPED) {
        if (EGInvisibleWhenClipped(g_p))
          return;
        COPY_VCREC(&vc1, &pnts_return[0].loc.vc);
        COPY_VCREC(&vc2, &pnts_return[1].loc.vc);
        COPY_VCREC(&vc3, &pnts_return[2].loc.vc);
        if (npnts == 4) 
          COPY_VCREC(&vc4, &pnts_return[3].loc.vc);
      }
    }
  }
  
  if (BoxesOverlap(&tbox, &vbox)) {
    EV_render_mode = v_p->render_mode;
    EV_shade_mode  = v_p->shade_mode;
    if      (draw_mode == DRAW_MODE_DRAW) {
      EV_curr_GC    = v_p->writableGC;
      EV_curr_pixel = pixel;
      XSetForeground(EV_display, EV_curr_GC, pixel);
      XSetLineAttributes(EV_display, EV_curr_GC,
			 (unsigned)0, LineSolid, CapButt, JoinMiter);
    } else if (draw_mode == DRAW_MODE_ERASE) {
      EV_curr_GC    = v_p->eraseGC;
      EV_curr_pixel = v_p->background;
      XSetLineAttributes(EV_display, EV_curr_GC,
			 (unsigned)0, LineSolid, CapButt, JoinMiter);
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

    if ((EV_render_mode == FILLED_HIDDEN_RENDERING ||
				 EV_render_mode == CONST_SHADING_RENDERING) && fill != FILL_HOLLOW) {
      if (EV_render_mode == CONST_SHADING_RENDERING) {
				VCRec normal, v1, v2;
	
				v1.u = vc2.u - vc1.u; v1.v = vc2.v - vc1.v; v1.n = vc2.n - vc1.n;
				v2.u = vc3.u - vc1.u; v2.v = vc3.v - vc1.v; v2.n = vc3.n - vc1.n;
				CrossProd3((WCRec *)&normal, (WCRec *)&v1, (WCRec *)&v2);
				if (NormalizeVect3((WCRec *)&normal)) {
					if (normal.n < 0) {
						normal.u = -normal.u;
						normal.v = -normal.v;
						normal.n = -normal.n;
					}
					EV_curr_cosine = EVGetLightDirVsNormalCos(v_p, &normal);
					if (EV_shade_mode == COLOR_SHADING) 
						EV_curr_pixel = ColorShadeColor(EV_curr_pixel, EV_curr_cosine);
				}
			}
      EVVCtoDC(v_p, &vc1, &x1, &y1);
      EVVCtoDC(v_p, &vc2, &x2, &y2);
      EVVCtoDC(v_p, &vc3, &x3, &y3);
      TF(x1, y1, vc1.n, x2, y2, vc2.n, x3, y3, vc3.n,
				 v_p->view_dims_DC.x, v_p->view_dims_DC.y);
      if (clip_result == EV_TRIANGLE_CLIPPED && npnts == 4) {
        EVVCtoDC(v_p, &vc4, &x4, &y4);
        TF(x1, y1, vc1.n, x3, y3, vc3.n, x4, y4, vc4.n, 
           v_p->view_dims_DC.x, v_p->view_dims_DC.y);
      }
      if (edge_flag) {
        double n1, n2, n3, n4, dn;
        EV_curr_pixel = edge_pixel;
        dn = DELTA_N();
        n1 = vc1.n + dn;
        n2 = vc2.n + dn;
        n3 = vc3.n + dn;
        if (clip_result == EV_TRIANGLE_IS_IN) {
          BHM(x1, y1, n1, x2, y2, n2);
          BHM(x1, y1, n1, x3, y3, n3);
          BHM(x2, y2, n2, x3, y3, n3);
        } else if (clip_result == EV_TRIANGLE_CLIPPED && npnts == 3) {
          BHM(x1, y1, n1, x2, y2, n2);
          BHM(x1, y1, n1, x3, y3, n3);
        } else if (clip_result == EV_TRIANGLE_CLIPPED && npnts == 4) {
          n4 = vc4.n + dn;
          BHM(x1, y1, n1, x2, y2, n2);
          BHM(x3, y3, n3, x4, y4, n4);
          BHM(x1, y1, n1, x4, y4, n4);
        }
      }
    } else { 
      XPoint points[4];
      int np =3, ix, iy;
      
      if (EV_render_mode == WIRE_RENDERING || fill == FILL_HOLLOW ||
					draw_mode == DRAW_MODE_XORDRAW) {
				int ix1, ix2, iy1, iy2, ix3, iy3;
				VCRec F, S;
				int draw_line_instead = NO;
				
#define PNTS_IDENTCL(_x1, _y1, _x2, _y2) (_x1==_x2 && _y1==_y2)

				if (draw_mode == DRAW_MODE_XORDRAW) {
					EVVCtoDC(v_p, &vc1, &ix1, &iy1);
					EVVCtoDC(v_p, &vc2, &ix2, &iy2);
					EVVCtoDC(v_p, &vc3, &ix3, &iy3);
					if (PNTS_IDENTCL(ix1, iy1, ix2, iy2)) {
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix1, iy1, ix3, iy3);
						draw_line_instead = YES;
					} else if (PNTS_IDENTCL(ix3, iy3, ix2, iy2)) {
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix3, iy3, ix1, iy1);
						draw_line_instead = YES;
					} else if (PNTS_IDENTCL(ix1, iy1, ix3, iy3)) {
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix2, iy2, ix3, iy3);
						draw_line_instead = YES;
					}
				}
				if (!draw_line_instead) {
					F.u = vc1.u; F.v = vc1.v; F.n = vc1.n;
					S.u = vc2.u; S.v = vc2.v; S.n = vc2.n; 
					if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
														 &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
						EVVCtoDC(v_p, &F, &ix1, &iy1);
						EVVCtoDC(v_p, &S, &ix2, &iy2);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix1, iy1, ix2, iy2);
					}
					F.u = vc2.u; F.v = vc2.v; F.n = vc2.n;
					S.u = vc3.u; S.v = vc3.v; S.n = vc3.n; 
					if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
														 &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
						EVVCtoDC(v_p, &F, &ix1, &iy1);
						EVVCtoDC(v_p, &S, &ix2, &iy2);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix1, iy1, ix2, iy2);
					}
          if (   (clip_result == EV_TRIANGLE_IS_IN)
              || (clip_result == EV_TRIANGLE_CLIPPED && npnts == 3)) {
            F.u = vc1.u; F.v = vc1.v; F.n = vc1.n;
            S.u = vc3.u; S.v = vc3.v; S.n = vc3.n; 
            if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
                               &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
              EVVCtoDC(v_p, &F, &ix1, &iy1);
              EVVCtoDC(v_p, &S, &ix2, &iy2);
              XDrawLine(EV_display, EV_curr_win, EV_curr_GC,
                        ix1, iy1, ix2, iy2);
            }
          } else if (clip_result == EV_TRIANGLE_CLIPPED && npnts == 4) {
            F.u = vc4.u; F.v = vc4.v; F.n = vc4.n;
            S.u = vc3.u; S.v = vc3.v; S.n = vc3.n; 
            if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
                               &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
              EVVCtoDC(v_p, &F, &ix1, &iy1);
              EVVCtoDC(v_p, &S, &ix2, &iy2);
              XDrawLine(EV_display, EV_curr_win, EV_curr_GC,
                        ix1, iy1, ix2, iy2);
            }
            F.u = vc1.u; F.v = vc1.v; F.n = vc1.n;
            S.u = vc4.u; S.v = vc4.v; S.n = vc4.n; 
            if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
                               &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
              EVVCtoDC(v_p, &F, &ix1, &iy1);
              EVVCtoDC(v_p, &S, &ix2, &iy2);
              XDrawLine(EV_display, EV_curr_win, EV_curr_GC,
                        ix1, iy1, ix2, iy2);
            }
					}
				}
      } else { /* fill by plain Xlib */
				EVVCtoDC(v_p, &vc1, &ix, &iy);
				points[0].x = ix; points[0].y = iy;
				EVVCtoDC(v_p, &vc2, &ix, &iy);
				points[1].x = ix; points[1].y = iy;
				EVVCtoDC(v_p, &vc3, &ix, &iy);
				points[2].x = ix; points[2].y = iy;
        if (clip_result == EV_TRIANGLE_CLIPPED && npnts == 4) {
          np = 4;
          EVVCtoDC(v_p, &vc4, &ix, &iy);
          points[3].x = ix; points[3].y = iy;
        }
				XFillPolygon(EV_display, EV_curr_win, EV_curr_GC,
										 points, np, Convex, CoordModeOrigin);
				if (edge_flag) {
					int ls = SOLID_STYLE;
					double dn = DELTA_N();
					WCRec n, p1, p2, p3;
					
#define INCR(p, dn, n) { p.x += dn * n.x; p.y += dn * n.y; p.z += dn * n.z; }
					EVGetNormal(v_p, &n);
					COPY_WCREC(&p1, &sf); INCR(p1, dn, n);
					COPY_WCREC(&p2, &ss); INCR(p2, dn, n);
					COPY_WCREC(&p3, &st); INCR(p3, dn, n);
					EVLine3DPrimitive(v_p, g_p, &p1, &p2, ls, edge_pixel, 0, 1.0, draw_mode);
					EVLine3DPrimitive(v_p, g_p, &p2, &p3, ls, edge_pixel, 0, 1.0, draw_mode);
					EVLine3DPrimitive(v_p, g_p, &p3, &p1, ls, edge_pixel, 0, 1.0, draw_mode);
				}
      }
    }
  } /* if (BoxesOverlap(... */
}


