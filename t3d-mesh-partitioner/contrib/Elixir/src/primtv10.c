
#include "Eprimtv.h"


static int 
next_point_wd(FPNum sx, FPNum sy, FPNum sz,
              FPNum ex, FPNum ey, FPNum ez,
              FPNum sv, FPNum ev, FPNum lower, FPNum delta,
              FPNum *nx, FPNum *ny, FPNum *nz, FPNum *ns, FPNum *ne);
static int 
fill_triangle_with_colors(EView *v_p,
			  Display *dsp, Window win, GC agc,
			  FPNum *x, FPNum *y, FPNum *z, FPNum *values,
			  EBoxVC2DRec *view_box);
static void 
indices_seq(FPNum *values, int *indx1, int *indx2, int *indx3);
static int 
compute_poly_bbox(FPNum *x, FPNum *y, int npoints, EBoxVC2DRec *box);

static int 
fill_triangle_with_contours(EView *v_p,
			  Display *dsp, Window win, GC agc,
			  FPNum *x, FPNum *y, FPNum *z, FPNum *values,
			  EBoxVC2DRec *view_box, GraphicObj *g_p, 
			  EDrawMode draw_mode, int width, VCRec *normal, double cosine);


#define USE_BHM   0

#define TRANSITION_COSINE   0.25          /* value of contour cosine if current cosine equals 0 */
#define LIMIT_COSINE        0.05          /* value of contour cosine if current cosine equals -1 */



void 
EVTriangleWD3DPrimitive(EView        *v_p,
                        GraphicObj   *g_p, 
                        WCRec        *f,
                        WCRec        *s,
                        WCRec        *t,
                        double        val1,
                        double        val2,
                        double        val3,
                        EPixel        pixel,
                        int           fill,
                        int           edge_flag,
                        EPixel        edge_pixel,
                        float         shrink,
                        EDrawMode     draw_mode)
{
  int x1, x2, y1, y2, x3, y3;
  VCRec vcf, vcs, vct, ll, ur, tll, tur;
  EBoxVC2DRec vbox, tbox;
  WCRec sf, ss, st;
  double tmp, cosine;
  EV_line_end_point_rec p1, p2, p3;
  EV_triangle_clip_result clip_result;
  EV_line_end_point_rec  pnts_return[4];
  int npnts;
	int width;

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
    SHRINK_TRIANGLE_DATA(shrink, val1, val2, val3);
	}
	EVWCtoVC(v_p, &sf, &vcf);
	EVWCtoVC(v_p, &ss, &vcs);
	EVWCtoVC(v_p, &st, &vct);
 
  if (v_p->should_clip_by_model_planes  && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
      COPY_WCREC(&p1.loc.wc, f);
      COPY_WCREC(&p2.loc.wc, s);
      COPY_WCREC(&p3.loc.wc, t);
      clip_result = clip_triangle_by_a_plane(NO, &p1, &p2, &p3,
                                             &v_p->model_clip_plane.center,
                                             &v_p->model_clip_plane.normal,
                                             pnts_return, &npnts);
      if (clip_result == EV_TRIANGLE_IS_OUT)
        return;
      if (clip_result == EV_TRIANGLE_CLIPPED
          && EGInvisibleWhenClipped(g_p))
        return;
/*
      else {
        fprintf(stderr, "RAW in EVTriangleWD3DPrimitive\n");
      }
*/
    }
  }

  EVGetViewBox(v_p, &ll, &ur);
  tmp = min(vcf.u, vcs.u); tll.u = min(tmp, vct.u);
  tmp = min(vcf.v, vcs.v); tll.v = min(tmp, vct.v);
  tmp = min(vcf.n, vcs.n); tll.n = min(tmp, vct.n);
  tmp = max(vcf.u, vcs.u); tur.u = max(tmp, vct.u);
  tmp = max(vcf.v, vcs.v); tur.v = max(tmp, vct.v);
  tmp = max(vcf.n, vcs.n); tur.n = max(tmp, vct.n);
  vbox.left  = ll.u; vbox.bottom  = ll.v;
  vbox.right = ur.u; vbox.top     = ur.v;
  tbox.left  = tll.u; tbox.bottom = tll.v;
  tbox.right = tur.u; tbox.top    = tur.v;

  if (BoxesOverlap(&tbox, &vbox)) {
    EV_render_mode = v_p->render_mode;
    EV_shade_mode  = v_p->shade_mode;
    if      (draw_mode == DRAW_MODE_DRAW) {
      EV_curr_GC    = v_p->writableGC;
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
    
		if(IsFringeTableContourScale()){
			if(EV_render_mode != WIRE_RENDERING && fill != FILL_HOLLOW){
				FPNum x[3], y[3], z[3], v[3];
				VCRec normal = {0.0, 0.0, 0.0};
				double dn = DELTA_N();
				double n1, n2, n3;
				BOOLEAN transparent;

				if((transparent = IsContourBgTransparent()) == NO)EV_curr_pixel = GetContourBgColor();

				width = GetContourWidth();
				if(width > 1 || EV_render_mode == CONST_SHADING_RENDERING){
					VCRec v1, v2;

					v1.u = vcs.u - vcf.u; v1.v = vcs.v - vcf.v; v1.n = vcs.n - vcf.n;
					v2.u = vct.u - vcf.u; v2.v = vct.v - vcf.v; v2.n = vct.n - vcf.n;
					CrossProd3((WCRec *)&normal, (WCRec *)&v1, (WCRec *)&v2);
					if (NormalizeVect3((WCRec *)&normal)) {
						if (EV_render_mode == CONST_SHADING_RENDERING) {
							if (normal.n < 0) {
								normal.u = -normal.u;
								normal.v = -normal.v;
								normal.n = -normal.n;
							}
							EV_curr_cosine = EVGetLightDirVsNormalCos(v_p, &normal);
							if (EV_shade_mode == COLOR_SHADING && transparent == NO)
								EV_curr_pixel = ColorShadeColor(EV_curr_pixel, EV_curr_cosine);

/* modify the cosine to make contours not so dark */
							cosine = EV_curr_cosine + (1.0 - EV_curr_cosine) * TRANSITION_COSINE;
							if(EV_curr_cosine < 0)cosine = TRANSITION_COSINE - cosine * (LIMIT_COSINE - TRANSITION_COSINE);
						}
					}
				}

				if(transparent == NO){
					if ((EV_render_mode == FILLED_HIDDEN_RENDERING ||
							 EV_render_mode == CONST_SHADING_RENDERING) && fill != FILL_HOLLOW) {
						EVVCtoDC(v_p, &vcf, &x1, &y1);
						EVVCtoDC(v_p, &vcs, &x2, &y2);
						EVVCtoDC(v_p, &vct, &x3, &y3);

/* decrease n by small value to make contours continuous */
						n1 = vcf.n - dn;
						n2 = vcs.n - dn;
						n3 = vct.n - dn;
						
						TF(x1, y1, n1, x2, y2, n2, x3, y3, n3,
							 v_p->view_dims_DC.x, v_p->view_dims_DC.y);
					}
				}

				if (edge_flag) {
					int ls = SOLID_STYLE;

					EVLine3DPrimitive(v_p, g_p, &sf, &ss, ls, edge_pixel, 0, 1.0, draw_mode);
					EVLine3DPrimitive(v_p, g_p, &ss, &st, ls, edge_pixel, 0, 1.0, draw_mode);
					EVLine3DPrimitive(v_p, g_p, &st, &sf, ls, edge_pixel, 0, 1.0, draw_mode);

#ifdef ALTERNATIVE
					if(EV_render_mode == NORMAL_RENDERING){
						if (draw_mode == DRAW_MODE_DRAW)XSetForeground(EV_display, EV_curr_GC, edge_pixel);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x2, y2, x3, y3);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x3, y3, x1, y1);
					}
					else{
						BHM(x1, y1, vcf.n, x2, y2, vcs.n);
						BHM(x1, y1, vcf.n, x3, y3, vct.n);
						BHM(x2, y2, vcs.n, x3, y3, vct.n);
					}
#endif
				}

				if(EV_render_mode == NORMAL_RENDERING){
					if (draw_mode == DRAW_MODE_DRAW) {
						XSetLineAttributes(EV_display, EV_curr_GC,
															 (unsigned)width, LineSolid, CapButt, JoinMiter);
					} else if (draw_mode == DRAW_MODE_ERASE) {
						XSetLineAttributes(EV_display, EV_curr_GC,
															 (unsigned)width, LineSolid, CapButt, JoinMiter);
					}
				}

/* increase n by small value to make contours continuous */
				x[0] = vcf.u; y[0] = vcf.v; z[0] = vcf.n + dn; v[0] = val1;
				x[1] = vcs.u; y[1] = vcs.v; z[1] = vcs.n + dn; v[1] = val2;
				x[2] = vct.u; y[2] = vct.v; z[2] = vct.n + dn; v[2] = val3;

				fill_triangle_with_contours(v_p, EV_display, EV_curr_win, EV_curr_GC,
																		x, y, z, v, &vbox, g_p, draw_mode, width, &normal, cosine);
				return;
			}
		}

    if ((EV_render_mode == FILLED_HIDDEN_RENDERING ||
				 EV_render_mode == CONST_SHADING_RENDERING) && fill != FILL_HOLLOW) {
      if (EV_render_mode == CONST_SHADING_RENDERING) {
				VCRec normal, v1, v2;
	
				v1.u = vcs.u - vcf.u; v1.v = vcs.v - vcf.v; v1.n = vcs.n - vcf.n;
				v2.u = vct.u - vcf.u; v2.v = vct.v - vcf.v; v2.n = vct.n - vcf.n;
				CrossProd3((WCRec *)&normal, (WCRec *)&v1, (WCRec *)&v2);
				if (NormalizeVect3((WCRec *)&normal)) {
					if (normal.n < 0) {
						normal.u = -normal.u;
						normal.v = -normal.v;
						normal.n = -normal.n;
					}
					EV_curr_cosine = EVGetLightDirVsNormalCos(v_p, &normal);
        } 
      }  
      
      EVVCtoDC(v_p, &vcf, &x1, &y1);
      EVVCtoDC(v_p, &vcs, &x2, &y2);
      EVVCtoDC(v_p, &vct, &x3, &y3);
      TFCI(x1, y1, vcf.n, x2, y2, vcs.n, x3, y3, vct.n,
           val1, val2, val3, v_p->fringe_table,
           v_p->view_dims_DC.x, v_p->view_dims_DC.y);
      if (edge_flag) {
        double n1, n2, n3, dn;
        EV_curr_pixel = edge_pixel;
        dn = DELTA_N();
        n1 = vcf.n + dn;
        n2 = vcs.n + dn;
        n3 = vct.n + dn;
        BHM(x1, y1, n1, x2, y2, n2);
        BHM(x1, y1, n1, x3, y3, n3);
        BHM(x2, y2, n2, x3, y3, n3);
      }
    } else { /* Not hidden surface/line rendering */
      XPoint points[3];
      int ix, iy;
      
      if (EV_render_mode == WIRE_RENDERING || fill == FILL_HOLLOW ||
					draw_mode == DRAW_MODE_XORDRAW) {
				int ix1, ix2, iy1, iy2, ix3, iy3;
				VCRec F, S;
				int draw_line_instead = NO;
				
#define PNTS_IDENTCL(_x1, _y1, _x2, _y2) (_x1==_x2 && _y1==_y2)

				if (draw_mode == DRAW_MODE_XORDRAW) {
					EVVCtoDC(v_p, &vcf, &ix1, &iy1);
					EVVCtoDC(v_p, &vcs, &ix2, &iy2);
					EVVCtoDC(v_p, &vct, &ix3, &iy3);
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
					F.u = vcf.u; F.v = vcf.v; F.n = vcf.n;
					S.u = vcs.u; S.v = vcs.v; S.n = vcs.n; 
					if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
														 &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
						EVVCtoDC(v_p, &F, &ix1, &iy1);
						EVVCtoDC(v_p, &S, &ix2, &iy2);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix1, iy1, ix2, iy2);
					}
					F.u = vcf.u; F.v = vcf.v; F.n = vcf.n;
					S.u = vct.u; S.v = vct.v; S.n = vct.n; 
					if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
														 &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
						EVVCtoDC(v_p, &F, &ix1, &iy1);
						EVVCtoDC(v_p, &S, &ix2, &iy2);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix1, iy1, ix2, iy2);
					}
					F.u = vcs.u; F.v = vcs.v; F.n = vcs.n;
					S.u = vct.u; S.v = vct.v; S.n = vct.n; 
					if (ClipLineToView(ll.u, ll.v, ur.u, ur.v,
														 &F.u, &F.v, &F.n, &S.u, &S.v, &S.n)) {
						EVVCtoDC(v_p, &F, &ix1, &iy1);
						EVVCtoDC(v_p, &S, &ix2, &iy2);
						XDrawLine(EV_display, EV_curr_win, EV_curr_GC, ix1, iy1, ix2, iy2);
					}
				}
      } else { /* fill by plain Xlib */
        if (1) {
          FPNum x[3], y[3], z[3], v[3];

          x[0] = vcf.u; y[0] = vcf.v; z[0] = vcf.n; v[0] = val1;
          x[1] = vcs.u; y[1] = vcs.v; z[1] = vcs.n; v[1] = val2;
          x[2] = vct.u; y[2] = vct.v; z[2] = vct.n; v[2] = val3;
          fill_triangle_with_colors(v_p, EV_display, EV_curr_win, EV_curr_GC,
                                    x, y, z, v, &vbox);
        } else {
          EVVCtoDC(v_p, &vcf, &ix, &iy);
          points[0].x = ix; points[0].y = iy;
          EVVCtoDC(v_p, &vcs, &ix, &iy);
          points[1].x = ix; points[1].y = iy;
          EVVCtoDC(v_p, &vct, &ix, &iy);
          points[2].x = ix; points[2].y = iy;
          XFillPolygon(EV_display, EV_curr_win, EV_curr_GC,
                       points, 3, Convex, CoordModeOrigin);
        }

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



static int 
fill_triangle_with_colors(EView *v_p,
			  Display *dsp, Window win, GC agc,
			  FPNum *x, FPNum *y, FPNum *z, FPNum *values,
			  EBoxVC2DRec *view_box)
{
  FPNum lisx, liisx, lisy, liisy, lisz,
        liisz, liex, liiex, liey, liiey, liez, liiez;
  FPNum svi, evi, svii, evii;
  FPNum nx, ny, nz, nsv, nev;
  int npoints;
  FPNum lower;
  FPNum value1, value2, value3;
  FPNum delta, val;
  int range;
  EPixel color;
  int indx1, indx2, indx3;
  FPNum tmpx[7], tmpy[7], tmpz[7];
  FPNum *pvx, *pvy, *pvz;
  FPNum eps;
#define EPSILON 0.00001
  Widget w;
  EBoxVC2DRec pbox;
  EFringeTable ft;
	BOOLEAN first = YES;

  w   = EVViewPToWidget(v_p);
  ft  = EVGetAssocFringeTable(v_p);
  
  indices_seq(values, &indx1, &indx2, &indx3);
  lisx   = x[indx1]; lisy   = y[indx1]; lisz   = z[indx1];
  liex   = x[indx2]; liey   = y[indx2]; liez   = z[indx2];
  liisx  = x[indx1]; liisy  = y[indx1]; liisz  = z[indx1];
  liiex  = x[indx3]; liiey  = y[indx3]; liiez  = z[indx3];
  value1 = values[indx1]; value2 = values[indx2]; value3 = values[indx3];
  eps    = EPSILON * (fabs(liex - lisx) + fabs(liiex - liisx) +
		      fabs(liey - lisy) + fabs(liiey - liisy) +
		      fabs(liez - lisz) + fabs(liiez - liisz)); 

  delta = ColorFringesDelta(ft);
  val   = value1;
  range = ColorFringeValueToRange(ft, val);
  color = ColorFringeRangeToColor(range);
  lower = ColorFringeClosestLowerValue(ft, val);
  svi   = value1; evi   = value2;
  svii  = value1; evii  = value3;
  pvx = tmpx; pvx--; pvy = tmpy; pvy--; pvz = tmpz; pvz--;
 continue_1:
  npoints = 0;
  next_point_wd(lisx, lisy, lisz, liex, liey, liez,
	      svi, evi, lower, delta, &nx, &ny, &nz, &nsv, &nev);
  if (PWDAPPOXEQ(liex, liey, liez, evi, nx, ny, nz, nev, eps)) {
    liex = liiex; liey = liiey; liez = liiez; evi  = evii;
    npoints = 2; 
    pvx[npoints] = nx; pvy[npoints] = ny; pvz[npoints] = nz;
    npoints = 3; 
    pvx[npoints] = lisx; pvy[npoints] = lisy; pvz[npoints] = lisz;
    lisx = nx; lisy = ny; lisz = nz; svi  = nsv;
    next_point_wd(lisx, lisy, lisz, liex, liey, liez,
		svi, evi, lower, delta, &nx, &ny, &nz, &nsv, &nev);
    npoints = 1;
    pvx[npoints] = nx; pvy[npoints] = ny; pvz[npoints] = nz;
    npoints = 3;
  } else {
    npoints = 1;
    pvx[npoints] = nx; pvy[npoints] = ny; pvz[npoints] = nz;
    npoints = 2;
    pvx[npoints] = lisx; pvy[npoints] = lisy; pvz[npoints] = lisz;
    npoints = 2;
  }
  lisx = nx; lisy = ny; lisz = nz; svi  = nsv; evi  = nev;
  npoints = npoints + 1;
  pvx[npoints] = liisx; pvy[npoints] = liisy; pvz[npoints] = liisz;
  next_point_wd(liisx, liisy, liisz, liiex, liiey, liiez,
	      svii, evii, lower, delta, &nx, &ny, &nz, &nsv, &nev);
  npoints = npoints + 1;
  pvx[npoints] = nx; pvy[npoints] = ny; pvz[npoints] = nz;
  if (PWDAPPOXEQ(liiex, liiey, liiez, evii, nx, ny, nz, nev, eps)) {
    liiex = liex; liiey = liey; liiez = liez; evii = evi;
    liisx = nx; liisy = ny; liisz = nz; svii = nsv;
    next_point_wd(liisx, liisy, liisz, liiex, liiey, liiez,
		svii, evii, lower, delta, &nx, &ny, &nz, &nsv, &nev);
    npoints = npoints + 1;
    pvx[npoints] = nx; pvy[npoints] = ny; pvz[npoints] = nz;
  }
  liisx = nx; liisy = ny; liisz = nz; svii  = nsv; evii  = nev;
  npoints = npoints + 1;
  pvx[npoints] = pvx[1]; pvy[npoints] = pvy[1]; pvz[npoints] = pvz[1];

  compute_poly_bbox(tmpx, tmpy, npoints, &pbox);
  if (BoxesOverlap(&pbox, view_box)) {
    XPoint points[7];
    int i;
    int ix, iy;
    VCRec ap;

    XSetForeground(dsp, agc, color);
    for (i = 0; i < npoints; i++) {
      ap.u = tmpx[i]; ap.v = tmpy[i]; ap.n = tmpz[i];
      EVVCtoDC(v_p, &ap, &(ix), &(iy));
      points[i].x = ix; points[i].y = iy;
    }
    XFillPolygon(dsp, win, agc, points, npoints, Convex, CoordModeOrigin);
  }
  if (PWDAPPOXEQ(lisx, lisy, lisz, svi, liisx, liisy, liisz, svii, eps)) {
		if(first == NO)return 1;
  }
  val = val + delta;
  range = ColorFringeValueToRange(ft, val);
  color = ColorFringeRangeToColor(range);
  lower = lower + delta;
	first = NO;
  goto continue_1;
}



static int 
compute_poly_bbox(FPNum *x, FPNum *y, int npoints, EBoxVC2DRec *box)
{
  int i;
  
  box->left   = x[0];
  box->right  = x[0];
  box->bottom = y[0];
  box->top    = y[0];
  for (i = 1; i < npoints; i++) {
    box->left   = min(box->left, x[i]);
    box->right  = max(box->right, x[i]);
    box->bottom = min(box->bottom, y[i]);
    box->top    = max(box->top, y[i]);
  }
  return 1;
}



static int 
next_point_wd(FPNum sx, FPNum sy, FPNum sz,
              FPNum ex, FPNum ey, FPNum ez,
              FPNum sv, FPNum ev, FPNum lower, FPNum delta,
              FPNum *nx, FPNum *ny, FPNum *nz, FPNum *ns, FPNum *ne)
{
  double r;
#define EPS 1.0e-9

  if (fabs(ev - sv) < EPS ) {
    *nx = ex; *ny = ey; *nz = ez; *ns = ev; *ne = ev;
    return 1;
  }
  if (lower + delta <= ev)
    *ns = lower + delta;
  else
    *ns = ev;

  r  = 1. / (ev - sv);
  r  *=  (*ns - sv);
  *nx = sx + r * (ex - sx);
  *ny = sy + r * (ey - sy);
  *nz = sz + r * (ez - sz);
  *ne = ev;
  return 1;
}



static void 
indices_seq(FPNum *values, int *indx1, int *indx2, int *indx3)
{
  if    (values[0] < values[1]) {
    if     (values[1] < values[2]) {
      *indx1 = 0; *indx2 = 1; *indx3 = 2;
    } else if (values[2] < values[0]) {
      *indx1 = 2; *indx2 = 0; *indx3 = 1;
    } else {
      *indx1 = 0; *indx2 = 2; *indx3 = 1;
    }
  }else {
    if     (values[0] < values[2]) {
      *indx1 = 1; *indx2 = 0; *indx3 = 2;
    } else if (values[2] < values[1]) {
      *indx1 = 2; *indx2 = 1; *indx3 = 0;
    } else {
      *indx1 = 1; *indx2 = 2; *indx3 = 0;
    }
  }
}



static int 
fill_triangle_with_contours(EView *v_p,
			  Display *dsp, Window win, GC agc,
			  FPNum *x, FPNum *y, FPNum *z, FPNum *values,
			  EBoxVC2DRec *view_box, GraphicObj *g_p, 
        EDrawMode draw_mode, int width, VCRec *normal, double cosine)
{
	double value, val_min, val_max, min_val, max_val, w;
	int indx, inode, jnode, iside, zlevel, labels;
	int isc_color_scale_num_labels = ELIXIR_COLOR_SCALE_NUM_LABELS;
	FPNum delta;
	EFringeTable ft;
	VCRec p[2];
	EPixel pixel;

	labels = isc_color_scale_num_labels;
	if(IsFringeTableSmoothScale() == YES){
		if(EV_render_mode == FILLED_HIDDEN_RENDERING || EV_render_mode == CONST_SHADING_RENDERING){
			labels = GetContourCount();
			if(labels == 0)labels = isc_color_scale_num_labels;
		}
	}

  ft  = EVGetAssocFringeTable(v_p);
	ColorFringesMinMax(ft, &min_val, &max_val);
	delta = (max_val - min_val) / labels;
	value = min_val + delta / 2;
		
	for(zlevel = 1; zlevel <= labels; zlevel++){
		indx = 0;
		if((value <= min_val) || (value >= max_val))return(0);
		for(iside = 1; iside <= 3; iside++){
			inode = iside;
			jnode = (iside == 3) ? 1 : iside + 1;
			val_min = min(values[inode - 1], values[jnode - 1]);
			val_max = max(values[inode - 1], values[jnode - 1]);
			if((val_max > value) && (val_min < value)){
				double t, len;
				double dx, dy, dz;

				dx = x[jnode - 1] - x[inode - 1];
				dy = y[jnode - 1] - y[inode - 1];
				dz = z[jnode - 1] - z[inode - 1];

				t = (value - values[inode - 1]) / (values[jnode - 1] - values[inode - 1]);
					
				p[indx].u = (FPNum)(x[inode - 1] + t * dx);
				p[indx].v = (FPNum)(y[inode - 1] + t * dy);
				p[indx].n = (FPNum)(z[inode - 1] + t * dz);
				indx++;

				if(indx == 2){
					int x1, x2, y1, y2;

					EV_curr_pixel = ColorFringeValueToColorHSV(ft, value);
					
					EVVCtoDC(v_p, &p[0], &x1, &y1);
					EVVCtoDC(v_p, &p[1], &x2, &y2);

					if(EV_render_mode == NORMAL_RENDERING){
						XSetForeground(dsp, agc, EV_curr_pixel);
						XDrawLine(dsp, win, agc, x1, y1, x2, y2);
					}
					else{

/* avoid BHM for shaded rendering because it ignores shades */
						if(width == 0 && EV_render_mode == FILLED_HIDDEN_RENDERING && USE_BHM != 0){
							BHM(x1, y1, p[0].n, x2, y2, p[1].n);
						}
						else{
							VCRec p1, p2, p3, p4, pp, vec;
							WCRec pp1, pp2, pp3, pp4;
							double rate, du, dv;
							unsigned int ddx, ddy;

							if(fabs(normal -> n) < 0.0001){
								BHM(x1, y1, p[0].n, x2, y2, p[1].n);         /* triangle perpendicular to view */
							}
							else{
								
/* check the contour length in pixels */
								ddx = fabs(x2 - x1);
								ddy = fabs(y2 - y1);
								len = ddx * ddx + ddy * ddy;
								if(len == 0.0){
									BHM(x1, y1, p[0].n, x2, y2, p[1].n);         /* just a pixel */
								}
								else{
								
/* calculate the vector (vec) perpendicular to contour (pp) and to the triangle normal (normal) */
									pp.u = p[1].u - p[0].u;
									pp.v = p[1].v - p[0].v;
									pp.n = p[1].n - p[0].n;
									CrossProd3((WCRec *)&vec, (WCRec *)normal, (WCRec *)&pp);
									NormalizeVect3((WCRec *)&vec);

/* calculate plan length of vector vec */
									len = vec.u * vec.u + vec.v * vec.v;
									if(len == 0.0){
										BHM(x1, y1, p[0].n, x2, y2, p[1].n);
									}
									else{

/* take into account the width reduction due to the rotation around the contour */
										w = width * sqrt(1.0 - vec.n * vec.n);
										if((unsigned)(w + 0.5) == 0 && EV_render_mode == FILLED_HIDDEN_RENDERING && USE_BHM != 0){
											BHM(x1, y1, p[0].n, x2, y2, p[1].n);
										}
										else{
											if(w < 1.0)w = 1.0;
											
/* get the view dimensions in DC and VC */
											EVGetDimsDC(v_p, &ddx, &ddy);
											EVGetDimsVC(v_p, &du, &dv);

/* adjust the vector vec to be of width pixels */										
											rate = w / ddx * du / sqrt(len) * 0.5;

/* prevent zero rate */
											if(rate == 0.0)rate = 1.0e-15;
											vec.u *= rate; vec.v *= rate; vec.n *= rate;
									
/* calculate points of quad */
											p1.u = p[0].u - vec.u;
											p1.v = p[0].v - vec.v;
											p1.n = p[0].n - vec.n;
											
											p2.u = p[1].u - vec.u;
											p2.v = p[1].v - vec.v;
											p2.n = p[1].n - vec.n;

											p3.u = p[1].u + vec.u;
											p3.v = p[1].v + vec.v;
											p3.n = p[1].n + vec.n;

											p4.u = p[0].u + vec.u;
											p4.v = p[0].v + vec.v;
											p4.n = p[0].n + vec.n;

											EVVCtoWC(v_p, &p1, &pp1);
											EVVCtoWC(v_p, &p2, &pp2);
											EVVCtoWC(v_p, &p3, &pp3);
											EVVCtoWC(v_p, &p4, &pp4);

											if(EV_render_mode == CONST_SHADING_RENDERING){

/* use hidden rendering with shaded color to prevent contours being so dark */

												pixel = ColorShadeColor(EV_curr_pixel, cosine);
												v_p -> render_mode = FILLED_HIDDEN_RENDERING;
												EVQuad3DPrimitive(v_p, g_p, &pp1, &pp2, &pp3, &pp4, pixel, SOLID_STYLE, 0, 0, 1.0, draw_mode);
												v_p -> render_mode = EV_render_mode = CONST_SHADING_RENDERING;
											}
											else
												EVQuad3DPrimitive(v_p, g_p, &pp1, &pp2, &pp3, &pp4, EV_curr_pixel, SOLID_STYLE, 0, 0, 1.0, draw_mode);
										}
									}
								}
							}
						}
					}
							
					break;
				}
			}
		}
		value += delta;
	}
	return(1);
}









