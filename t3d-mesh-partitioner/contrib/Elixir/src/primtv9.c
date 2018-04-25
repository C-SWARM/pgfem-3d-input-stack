#include "Eprimtv.h"



#define INCR(p, dn, n) { p.x += dn * n.x; p.y += dn * n.y; p.z += dn * n.z; }


void 
EVBCBezS3DPrimitive(EView        *v_p,
                    GraphicObj   *g_p, 
										WCRec         wcpoints[4][4],  
										EPixel        pixel,
										int           fill,
                    int           edge_flag,
                    EPixel        edge_pixel,
										int           tessel_intervals,   
										BOOLEAN       show_poly,   
										BOOLEAN       boundary_only,
										EDrawMode     draw_mode)
{
  int x1, x2, y1, y2, x3, y3;
  unsigned int width, height;
  FPNum u, v, delta = 1.0/tessel_intervals;
  FPNum bu[4], bv[4];
  double tmp;
  int i, j, k, m;
  VCRec p1, p2, p3, p4, f, s, points[4][4];
  VCRec vboxll, vboxur, lf, ls;
  XPoint xpoints[3];
  EPixel entity_color = 0;
    
  EV_display                    = XtDisplay(v_p->view_widget);
  EV_curr_win                   = v_p->draw_into;
  EV_should_clip_by_view_planes = v_p->should_clip_by_fg_bg_view_planes;
  EV_fg_plane_depth             = v_p->fg_view_plane_n;
  EV_bg_plane_depth             = v_p->bg_view_plane_n;
  width           = v_p->view_dims_DC.x; 
  height          = v_p->view_dims_DC.y;

  if      (draw_mode == DRAW_MODE_DRAW) {
    EV_curr_GC    = v_p->writableGC;
    EV_curr_pixel = entity_color = pixel;
    XSetForeground(EV_display, EV_curr_GC, pixel);
    XSetLineAttributes(EV_display, EV_curr_GC,
		       (unsigned)0, LineSolid, CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_ERASE) {
    EV_curr_GC    = v_p->eraseGC;
    EV_curr_pixel = entity_color = v_p->background;
    XSetLineAttributes(EV_display, EV_curr_GC,
		       (unsigned)0, LineSolid, CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_HILITE) {
    EV_curr_GC    = v_p->hiliteGC;
    EV_curr_pixel = entity_color = v_p->foreground;
  } else if (draw_mode == DRAW_MODE_UNHILITE) {
    EV_curr_GC    = v_p->erasehiliteGC;
    EV_curr_pixel = entity_color = v_p->background;
  } else if (draw_mode == DRAW_MODE_XORDRAW) {
    EV_curr_GC    = v_p->defaultXORGC;
    EV_curr_pixel = entity_color = v_p->foreground;
    tessel_intervals = 4, delta = 1./tessel_intervals;
  }
  EV_render_mode = v_p->render_mode;
  EV_shade_mode  = v_p->shade_mode;
  EVGetViewBox(v_p, &vboxll, &vboxur);
  
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      EVWCtoVC(v_p, &(wcpoints[i][j]), &(points[i][j]));

  if ((EV_render_mode == FILLED_HIDDEN_RENDERING ||
       EV_render_mode == CONST_SHADING_RENDERING ||
       EV_render_mode == NORMAL_RENDERING) && fill != FILL_HOLLOW) {
    
    for (u = ZERO, i = 0; i < tessel_intervals; u += delta, i++) {
      v = ZERO;
      CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
      CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
      p1.u = p1.v = p1.n = ZERO;
      for (k = 0; k < 4; k++)
				for (m = 0; m < 4; m++) {
					tmp = bu[k] * bv[m];
					p1.u += tmp * points[k][m].u;
					p1.v += tmp * points[k][m].v;
					p1.n += tmp * points[k][m].n;
				}
      CubicBernstein(u+delta, &bu[0], &bu[1], &bu[2], &bu[3]);
      p2.u = p2.v = p2.n = ZERO;
      for (k = 0; k < 4; k++)
				for (m = 0; m < 4; m++) {
					tmp = bu[k] * bv[m];
					p2.u += tmp * points[k][m].u;
					p2.v += tmp * points[k][m].v;
					p2.n += tmp * points[k][m].n;
				}
      for (v = delta, j = 0; j < tessel_intervals; v += delta, j++) {
				CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
				CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
				p3.u = p3.v = p3.n = ZERO;
				for (k = 0; k < 4; k++)
					for (m = 0; m < 4; m++) {
						tmp = bu[k] * bv[m];
						p3.u += tmp * points[k][m].u;
						p3.v += tmp * points[k][m].v;
						p3.n += tmp * points[k][m].n;
					}
				CubicBernstein(u+delta, &bu[0], &bu[1], &bu[2], &bu[3]);
				p4.u = p4.v = p4.n = ZERO;
				for (k = 0; k < 4; k++)
					for (m = 0; m < 4; m++) {
						tmp = bu[k] * bv[m];
						p4.u += tmp * points[k][m].u;
						p4.v += tmp * points[k][m].v;
						p4.n += tmp * points[k][m].n;
					}
	
				if (EV_render_mode == CONST_SHADING_RENDERING) {
					VCRec normal, v1, v2;
					
					v1.u = p2.u - p1.u; v1.v = p2.v - p1.v; v1.n = p2.n - p1.n;
					v2.u = p3.u - p1.u; v2.v = p3.v - p1.v; v2.n = p3.n - p1.n;
					CrossProd3((WCRec *)&normal, (WCRec *)&v1, (WCRec *)&v2);
					if (NormalizeVect3((WCRec *)&normal)) {
						if (normal.n < 0) {
							normal.u = -normal.u; normal.v = -normal.v; normal.n = -normal.n;
						}
						EV_curr_cosine = EVGetLightDirVsNormalCos(v_p, &normal);
						if (EV_shade_mode == COLOR_SHADING) 
							EV_curr_pixel = ColorShadeColor(entity_color, EV_curr_cosine);
					}
				}
				EVVCtoDC(v_p, &p1, &x1, &y1);
				EVVCtoDC(v_p, &p2, &x2, &y2);
				EVVCtoDC(v_p, &p3, &x3, &y3);
				if (EV_render_mode == NORMAL_RENDERING) {
					xpoints[0].x = x1; xpoints[0].y = y1;
					xpoints[1].x = x2; xpoints[1].y = y2;
					xpoints[2].x = x3; xpoints[2].y = y3;
					XFillPolygon(EV_display, EV_curr_win, EV_curr_GC, 
											 xpoints, 3, Convex, CoordModeOrigin);
				} else {
					TF(x1, y1, p1.n, x2, y2, p2.n, x3, y3, p3.n, width, height);
        }
	
				if (EV_render_mode == CONST_SHADING_RENDERING) {
					VCRec normal, v1, v2;
					
					v1.u = p2.u - p3.u; v1.v = p2.v - p3.v; v1.n = p2.n - p3.n;
					v2.u = p4.u - p3.u; v2.v = p4.v - p3.v; v2.n = p4.n - p3.n;
					CrossProd3((WCRec *)&normal, (WCRec *)&v1, (WCRec *)&v2);
					if (NormalizeVect3((WCRec *)&normal)) {
						if (normal.n < 0) {
							normal.u = -normal.u; normal.v = -normal.v; normal.n = -normal.n;
						}
						EV_curr_cosine = EVGetLightDirVsNormalCos(v_p, &normal);
						if (EV_shade_mode == COLOR_SHADING) 
							EV_curr_pixel = ColorShadeColor(entity_color, EV_curr_cosine);
					}
				}
				EVVCtoDC(v_p, &p4, &x1, &y1);
				EVVCtoDC(v_p, &p2, &x2, &y2);
				EVVCtoDC(v_p, &p3, &x3, &y3);
				if (EV_render_mode == NORMAL_RENDERING) {
					xpoints[0].x = x1; xpoints[0].y = y1;
					xpoints[1].x = x2; xpoints[1].y = y2;
					xpoints[2].x = x3; xpoints[2].y = y3;
					XFillPolygon(EV_display, EV_curr_win, EV_curr_GC, 
											 xpoints, 3, Convex, CoordModeOrigin);
				} else {
					TF(x1, y1, p4.n, x2, y2, p2.n, x3, y3, p3.n, width, height);
        }
				p1.u = p3.u; p1.v = p3.v; p1.n = p3.n;
				p2.u = p4.u; p2.v = p4.v; p2.n = p4.n;
      } /* for (v = delta ...)  */
    } /* for (u = 0 ...) */
    /* Should edges be drawn in normal mode? */
    if (edge_flag) {
      EV_curr_pixel = edge_pixel;
			if(EV_render_mode == NORMAL_RENDERING){
				if(draw_mode == DRAW_MODE_DRAW) {
					XSetForeground(EV_display, EV_curr_GC, EV_curr_pixel);
					XSetLineAttributes(EV_display, EV_curr_GC,
														 (unsigned)0, LineSolid, CapButt, JoinMiter);
				}
			}
      for (u = 0, i = 0; i < 2; u += 1, i++) {
        v = ZERO;
        CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
        CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
        f.u = f.v = f.n = ZERO;
        for (k = 0; k < 4; k++)
          for (m = 0; m < 4; m++) {
            tmp = bu[k] * bv[m];
            f.u += tmp * points[k][m].u;
            f.v += tmp * points[k][m].v;
            f.n += tmp * points[k][m].n;
          }
        CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
        for (v = delta, j = 0; j < tessel_intervals; v += delta, j++) {
          CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
          s.u = s.v = s.n = ZERO;
          for (k = 0; k < 4; k++)
            for (m = 0; m < 4; m++) {
              tmp = bu[k] * bv[m];
              s.u += tmp * points[k][m].u;
              s.v += tmp * points[k][m].v;
              s.n += tmp * points[k][m].n;
            }
          lf.u = f.u, lf.v = f.v, lf.n = f.n;
          ls.u = s.u, ls.v = s.v, ls.n = s.n;
          if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                             &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
            double dn;
            
            EVVCtoDC(v_p, &lf, &x1, &y1);
            EVVCtoDC(v_p, &ls, &x2, &y2);
            dn = DELTA_N();
						if(EV_render_mode == FILLED_HIDDEN_RENDERING ||
							 EV_render_mode == CONST_SHADING_RENDERING){
							BHM(x1, y1, lf.n+dn, x2, y2, ls.n+dn);
						}
						else{
							XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
						}
          }
          f.u = s.u; f.v = s.v; f.n = s.n;
        }
      }
      for (v = 0, j = 0; j < 2; v += 1, j++) {
        u = ZERO;
        CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
        CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
        f.u = f.v = f.n = ZERO;
        for (k = 0; k < 4; k++)
          for (m = 0; m < 4; m++) {
            tmp = bu[k] * bv[m];
            f.u += tmp * points[k][m].u;
            f.v += tmp * points[k][m].v;
            f.n += tmp * points[k][m].n;
          }
        CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
        for (u = delta, i = 0; i < tessel_intervals; u += delta, i++) {
          CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
          s.u = s.v = s.n = ZERO;
          for (k = 0; k < 4; k++)
            for (m = 0; m < 4; m++) {
              tmp = bu[k] * bv[m];
              s.u += tmp * points[k][m].u;
              s.v += tmp * points[k][m].v;
              s.n += tmp * points[k][m].n;
            }
          lf.u = f.u, lf.v = f.v, lf.n = f.n;
          ls.u = s.u, ls.v = s.v, ls.n = s.n;
          if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                             &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
            double dn;
            
            EVVCtoDC(v_p, &lf, &x1, &y1);
            EVVCtoDC(v_p, &ls, &x2, &y2);
            dn = DELTA_N();
						if(EV_render_mode == FILLED_HIDDEN_RENDERING ||
							 EV_render_mode == CONST_SHADING_RENDERING){
							BHM(x1, y1, lf.n+dn, x2, y2, ls.n+dn);
						}
						else{
							XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
						}
          }
          f.u = s.u; f.v = s.v; f.n = s.n;
        }
      }
    } /* drawing boundary edges */
  } else { /* Not filled */
    if (EV_render_mode == WIRE_RENDERING)
      tessel_intervals = 4, delta = 1./tessel_intervals;
    
    for (u = 0, i = 0; i < tessel_intervals+1; u += delta, i++) {
      v = ZERO;
      CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
      CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
      f.u = f.v = f.n = ZERO;
      for (k = 0; k < 4; k++)
				for (m = 0; m < 4; m++) {
					tmp = bu[k] * bv[m];
					f.u += tmp * points[k][m].u;
					f.v += tmp * points[k][m].v;
					f.n += tmp * points[k][m].n;
				}
      CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
      for (v = delta, j = 0; j < tessel_intervals; v += delta, j++) {
				CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
				s.u = s.v = s.n = ZERO;
				for (k = 0; k < 4; k++)
					for (m = 0; m < 4; m++) {
						tmp = bu[k] * bv[m];
						s.u += tmp * points[k][m].u;
						s.v += tmp * points[k][m].v;
						s.n += tmp * points[k][m].n;
					}
				lf.u = f.u, lf.v = f.v, lf.n = f.n;
				ls.u = s.u, ls.v = s.v, ls.n = s.n;
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
				f.u = s.u; f.v = s.v; f.n = s.n;
      }
    }
    for (v = 0, j = 0; j < tessel_intervals+1; v += delta, j++) {
      u = ZERO;
      CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
      CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
      f.u = f.v = f.n = ZERO;
      for (k = 0; k < 4; k++)
				for (m = 0; m < 4; m++) {
					tmp = bu[k] * bv[m];
					f.u += tmp * points[k][m].u;
					f.v += tmp * points[k][m].v;
					f.n += tmp * points[k][m].n;
				}
      CubicBernstein(v, &bv[0], &bv[1], &bv[2], &bv[3]);
      for (u = delta, i = 0; i < tessel_intervals; u += delta, i++) {
				CubicBernstein(u, &bu[0], &bu[1], &bu[2], &bu[3]);
				s.u = s.v = s.n = ZERO;
				for (k = 0; k < 4; k++)
					for (m = 0; m < 4; m++) {
						tmp = bu[k] * bv[m];
						s.u += tmp * points[k][m].u;
						s.v += tmp * points[k][m].v;
						s.n += tmp * points[k][m].n;
					}
				lf.u = f.u, lf.v = f.v, lf.n = f.n;
				ls.u = s.u, ls.v = s.v, ls.n = s.n;
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
				f.u = s.u; f.v = s.v; f.n = s.n;
      }
    }
  }
  if (show_poly) {
#define DRAW_POLY_LINE(from, to) \
    lf.u = from.u, lf.v = from.v, lf.n = from.n; \
    ls.u = to.u, ls.v = to.v, ls.n = to.n; \
    if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v, \
		       &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) { \
      EVVCtoDC(v_p, &lf, &x1, &y1); \
      EVVCtoDC(v_p, &ls, &x2, &y2); \
      if (EV_render_mode == NORMAL_RENDERING || \
	  EV_render_mode == WIRE_RENDERING) { \
	XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2); \
      } else { \
	BHM(x1, y1, lf.n, x2, y2, ls.n); \
      } \
    }
    
    if (draw_mode == DRAW_MODE_DRAW)
      EV_curr_GC    = v_p->constrCopyGC, EV_curr_pixel = v_p->foreground;
    else if (draw_mode == DRAW_MODE_XORDRAW)
      EV_curr_GC    = v_p->defaultXORGC, EV_curr_pixel = v_p->foreground;
    else
      EV_curr_GC    = v_p->constrEraseGC, EV_curr_pixel = v_p->background;
		
    if (draw_mode == DRAW_MODE_DRAW || draw_mode == DRAW_MODE_ERASE ||
				draw_mode == DRAW_MODE_XORDRAW) {
      DRAW_POLY_LINE(points[0][0], points[0][1]);
      DRAW_POLY_LINE(points[0][0], points[0][1]);
      DRAW_POLY_LINE(points[0][1], points[0][2]);
      DRAW_POLY_LINE(points[0][2], points[0][3]);
      DRAW_POLY_LINE(points[1][0], points[1][1]);
      DRAW_POLY_LINE(points[1][1], points[1][2]);
      DRAW_POLY_LINE(points[1][2], points[1][3]);
      DRAW_POLY_LINE(points[2][0], points[2][1]);
      DRAW_POLY_LINE(points[2][1], points[2][2]);
      DRAW_POLY_LINE(points[2][2], points[2][3]);
      DRAW_POLY_LINE(points[3][0], points[3][1]);
      DRAW_POLY_LINE(points[3][1], points[3][2]);
      DRAW_POLY_LINE(points[3][2], points[3][3]);
    
      DRAW_POLY_LINE(points[0][0], points[1][0]);
      DRAW_POLY_LINE(points[1][0], points[2][0]);
      DRAW_POLY_LINE(points[2][0], points[3][0]);
      DRAW_POLY_LINE(points[0][1], points[1][1]);
      DRAW_POLY_LINE(points[1][1], points[2][1]);
      DRAW_POLY_LINE(points[2][1], points[3][1]);
      DRAW_POLY_LINE(points[0][2], points[1][2]);
      DRAW_POLY_LINE(points[1][2], points[2][2]);
      DRAW_POLY_LINE(points[2][2], points[3][2]);
      DRAW_POLY_LINE(points[0][3], points[1][3]);
      DRAW_POLY_LINE(points[1][3], points[2][3]);
      DRAW_POLY_LINE(points[2][3], points[3][3]);
    }
  }  
}




