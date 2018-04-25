
#include "Eprimtv.h"
#include "Eline3d.h"


#define FAN         PI/6.0  /* declination of arrow segment from leading line */
#define EPSILON     0.1     /* difference in rad in projection angle from PI / 2 */
#define DOUBLE_RATE 0.20    /* rate of arrow segment length for shift of double arrow from tip */
#define TRIPLE_RATE 0.75    /* rate of arrow segment length for shift of triple arrow from tip */ 
#define SPEC        0.5     /* reduction of arrow segment length in special case */


void 
EVVector3DPrimitive(EView      *v_p,
                    GraphicObj   *g_p, 
		    WCRec      *origin,
		    WCRec      *comp,
		    EVecMarkerType type,
		    FPNum       scale,
		    FPNum       rate,
		    EPixel      pixel,
        BOOLEAN     use_ftable,
				BOOLEAN     shift_flag,
		    EDrawMode   draw_mode)
{
	FPNum length3d, size3d, length2d, size2d, coef;
	FPNum du, dv, dn;
	double angle, alpha, beta;
  EFringeTable ft;
	WCRec tail, orig;
  VCRec  vboxll, vboxur, vo, vt, v1, v2, lo, lt;
  BOOLEAN is_visible = YES;
	BOOLEAN spec_case = NO, line0, side1, side2, side3;
	int xo, yo, xt, yt, x1t, y1t, x1o, y1o, x2t, y2t, x2o, y2o, x3t, y3t, x3o, y3o, np;
	XPoint points[7];
  
  EVGetViewBox(v_p, &vboxll, &vboxur);

	if(shift_flag == NO){
		tail.x = (orig.x = origin->x) - comp->x * scale;
		tail.y = (orig.y = origin->y) - comp->y * scale;
		tail.z = (orig.z = origin->z) - comp->z * scale;
	}
	else{
		orig.x = (tail.x = origin->x) + comp->x * scale;
		orig.y = (tail.y = origin->y) + comp->y * scale;
		orig.z = (tail.z = origin->z) + comp->z * scale;
	}

  EVWCtoVC(v_p, &orig, &vo);
  EVWCtoVC(v_p, &tail, &vt);

	lo.u = vo.u; lt.u = vt.u;
	lo.v = vo.v; lt.v = vt.v;
	lo.n = vo.n; lt.n = vt.n;

  line0 = ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
												 &lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n);
	EVVCtoDC(v_p, &lo, &xo, &yo);
	EVVCtoDC(v_p, &lt, &xt, &yt);

	length3d = sqrt(comp->x * comp->x + comp->y * comp->y + comp->z * comp->z);
	if(use_ftable){
		ft  = EVGetAssocFringeTable(v_p);
		pixel = ColorFringeValueToColorHSV(ft, length3d);
/*		pixel = ColorFringeRangeToColor(ColorFringeValueToRange(ft, length3d)); */
	}

	length2d = sqrt((vo.u - vt.u)*(vo.u - vt.u) + (vo.v - vt.v)*(vo.v - vt.v)) / scale;
	if(length2d > length3d)length2d = length3d;
	alpha = acos((double)(length2d / length3d));
	beta = FAN + (1.0 - 2.0 * FAN / PI) * alpha;

	size3d = length3d * scale * rate;
	if(fabs(beta - PI / 2.0) > EPSILON){
		size2d = size3d * tan(FAN) / sin((double)beta);
		
		angle = atan2((double)(vt.v - vo.v), (double)(vt.u - vo.u));
		
		angle += beta;
		lt.u = v1.u = (lo.u = vo.u) + size2d * cos(angle);
		lt.v = v1.v = (lo.v = vo.v) + size2d * sin(angle);
		lt.n = v1.n = (lo.n = vo.n);
		side1 = ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
													 &lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n);
		EVVCtoDC(v_p, &lo, &x1o, &y1o);
		EVVCtoDC(v_p, &lt, &x1t, &y1t);
		
		angle -= 2.0 * beta;
		lt.u = v2.u = (lo.u = vo.u) + size2d * cos(angle);
		lt.v = v2.v = (lo.v = vo.v) + size2d * sin(angle);
		lt.n = v2.n = (lo.n = vo.n);
		side2 = ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
													 &lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n);
		EVVCtoDC(v_p, &lo, &x2o, &y2o);
		EVVCtoDC(v_p, &lt, &x2t, &y2t);

		if(type == DOUBLE_ARROW_VECMARKER || type == TRIPLE_ARROW_VECMARKER){
			coef = scale * size3d / length3d;
/*			fprintf(stderr, "%f %f %f %f\n", scale, size3d, length3d, coef); */
			du = (vt.u - vo.u) * coef;
			dv = (vt.v - vo.v) * coef;
			dn = (vt.n - vo.n) * coef;
		}
		if(type == TRIANGLE_VECMARKER){
			side3 = ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
														 &v1.u, &v1.v, &v1.n, &v2.u, &v2.v, &v2.n);
			EVVCtoDC(v_p, &v1, &x3o, &y3o);
			EVVCtoDC(v_p, &v2, &x3t, &y3t);
		}
	}
	else{
		spec_case = YES;
		size2d = size3d * tan(FAN) * SPEC;
		lo.u = vo.u + size2d;
		lo.v = vo.v;
		lo.n = vo.n;
		lt.u = vo.u - size2d;
		lt.v = vo.v;
		lt.n = vo.n;
		side1 = ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
													 &lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n);
		EVVCtoDC(v_p, &lo, &x1o, &y1o);
		EVVCtoDC(v_p, &lt, &x1t, &y1t);
		lo.u = vo.u;
		lo.v = vo.v + size2d;
		lo.n = vo.n;
		lt.u = vo.u;
		lt.v = vo.v - size2d;
		lt.n = vo.n;
		side2 = ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
													 &lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n);
		EVVCtoDC(v_p, &lo, &x2o, &y2o);
		EVVCtoDC(v_p, &lt, &x2t, &y2t);
	}

	if (EV_render_mode == FILLED_HIDDEN_RENDERING ||
      EV_render_mode == CONST_SHADING_RENDERING) {
    if (ZBufferDepthAt(xo, yo) <= (float)lo.n) {
      if (can_delay_display) 
				AddToDelayedEntities();
      else
				is_visible = YES;
    } else
      return;
  }
  EV_display                    = XtDisplay(v_p->view_widget);
  EV_curr_win                   = v_p->draw_into;
  EV_should_clip_by_view_planes = v_p->should_clip_by_fg_bg_view_planes;
  EV_fg_plane_depth             = v_p->fg_view_plane_n;
  EV_bg_plane_depth             = v_p->bg_view_plane_n;
  
  if      (draw_mode == DRAW_MODE_DRAW) {
    EV_curr_GC    = v_p->writableGC;
    EV_curr_pixel = pixel;
    XSetForeground(EV_display, EV_curr_GC, pixel);
  } else if (draw_mode == DRAW_MODE_ERASE) {
    EV_curr_GC    = v_p->eraseGC;
    EV_curr_pixel = v_p->background;
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

	if(line0 == YES)
		XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
							(int)(xo), (int)(yo),
							(int)(xt), (int)(yt));

  switch (type) {
  case TRIPLE_ARROW_VECMARKER:
		if(spec_case == NO){
			lo.u = vo.u + du * TRIPLE_RATE;
			lo.v = vo.v + dv * TRIPLE_RATE;
			lo.n = vo.n + dn * TRIPLE_RATE;
			lt.u = v1.u + du * TRIPLE_RATE;
			lt.v = v1.v + dv * TRIPLE_RATE;
			lt.n = v1.n + dn * TRIPLE_RATE;
			if(ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
												&lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n) == YES){
				EVVCtoDC(v_p, &lo, &xo, &yo);
				EVVCtoDC(v_p, &lt, &xt, &yt);
				XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
									(int)(xo), (int)(yo),
									(int)(xt), (int)(yt));
			}
			lo.u = vo.u + du * TRIPLE_RATE;
			lo.v = vo.v + dv * TRIPLE_RATE;
			lo.n = vo.n + dn * TRIPLE_RATE;
			lt.u = v2.u + du * TRIPLE_RATE;
			lt.v = v2.v + dv * TRIPLE_RATE;
			lt.n = v2.n + dn * TRIPLE_RATE;
			if(ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
												&lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n) == YES){
				EVVCtoDC(v_p, &lo, &xo, &yo);
				EVVCtoDC(v_p, &lt, &xt, &yt);
				XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
									(int)(xo), (int)(yo),
									(int)(xt), (int)(yt));
			}			
		}
  case DOUBLE_ARROW_VECMARKER:
		if(spec_case == NO){
			lo.u = vo.u + du * DOUBLE_RATE;
			lo.v = vo.v + dv * DOUBLE_RATE;
			lo.n = vo.n + dn * DOUBLE_RATE;
			lt.u = v1.u + du * DOUBLE_RATE;
			lt.v = v1.v + dv * DOUBLE_RATE;
			lt.n = v1.n + dn * DOUBLE_RATE;
			if(ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
												&lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n) == YES){
				EVVCtoDC(v_p, &lo, &xo, &yo);
				EVVCtoDC(v_p, &lt, &xt, &yt);
				XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
									(int)(xo), (int)(yo),
									(int)(xt), (int)(yt));
			}
			lo.u = vo.u + du * DOUBLE_RATE;
			lo.v = vo.v + dv * DOUBLE_RATE;
			lo.n = vo.n + dn * DOUBLE_RATE;
			lt.u = v2.u + du * DOUBLE_RATE;
			lt.v = v2.v + dv * DOUBLE_RATE;
			lt.n = v2.n + dn * DOUBLE_RATE;
			if(ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
												&lo.u, &lo.v, &lo.n, &lt.u, &lt.v, &lt.n) == YES){
				EVVCtoDC(v_p, &lo, &xo, &yo);
				EVVCtoDC(v_p, &lt, &xt, &yt);
				XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
									(int)(xo), (int)(yo),
									(int)(xt), (int)(yt));
			}	
		}
  case ARROW_VECMARKER:
		if(side1 == YES)
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1o), (int)(y1o),
								(int)(x1t), (int)(y1t));
		if(side2 == YES)
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x2o), (int)(y2o),
								(int)(x2t), (int)(y2t));
		break;
  case TRIANGLE_VECMARKER:
		XSetFillStyle(EV_display, EV_curr_GC, FillSolid);
		if(spec_case == NO){
			points[0].x = x1t;
			points[0].y = y1t;
			points[1].x = x1o;
			points[1].y = y1o;
			points[2].x = x2o;
			points[2].y = y2o;
			points[3].x = x2t;
			points[3].y = y2t;
			points[4].x = x3t;
			points[4].y = y3t;
			points[5].x = x3o;
			points[5].y = y3o;
			points[6].x = x1t;
			points[6].y = y1t;
			np = 7;
		}
		else{
			points[0].x = x1t;
			points[0].y = y1t;
			points[1].x = x2t;
			points[1].y = y2t;
			points[2].x = x1o;
			points[2].y = y1o;
			points[3].x = x2o;
			points[3].y = y2o;
			points[4].x = x1t;
			points[4].y = y1t;
			np = 5;
		}
		XFillPolygon(EV_display, EV_curr_win, EV_curr_GC,
								 points, np, Convex, CoordModeOrigin);
    break;
	default:
		break;
	}
}
