
#include "Eprimtv.h"



#define FAN         PI/6.0  /* declination of arrow segment from leading line */
#define EPSILON     0.1     /* difference in rad in projection angle from PI / 2 */
#define DOUBLE_RATE 0.20    /* rate of arrow segment length for shift of double arrow from tip */
#define TRIPLE_RATE 0.75    /* rate of arrow segment length for shift of triple arrow from tip */ 
#define SPEC        0.5     /* reduction of arrow segment length in special case */

#define VEC_MIN_LENGTH 40.0
#define VEC_MAX_LENGTH 1000.0


void 
EVVecMarker3DPrimitive(EView      *v_p,
                    GraphicObj   *g_p, 
		    WCRec      *origin,
		    WCRec      *direction,
		    unsigned    size,
		    int         shift,
				FPNum       rate,
		    EVecMarkerType type,
		    EPixel      pixel,
		    EDrawMode   draw_mode)
{
	double length3d, size3d, length2d, size2d, coef, scale, sizeDC, lengthDC, shiftDC;
	double du, dv, dn, dx, dy;
	double angle, alpha, beta;
	WCRec tail, orig;
  VCRec vo, vt, v1, v2, v_o;
  BOOLEAN is_visible = YES;
	BOOLEAN spec_case = NO;
	int xo, yo, xt, yt, x1, y1, x2, y2, x_o, y_o, x_1, y_1, x_2, y_2, x1t, y1t, x1o, y1o, x2t, y2t, x2o, y2o, d_x, d_y;
	int np, x_shift, y_shift;
	XPoint points[5];
  EV_line_end_point_rec p;
  
  if (v_p->should_clip_by_model_planes && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
      COPY_WCREC(&p.loc.wc, origin);
      if (!point_is_in_IN_halfspace(&p, &v_p->model_clip_plane.center,
                                    &v_p->model_clip_plane.normal))
        return;
    }
  }

	length3d = 1.0;        /* direction is normalized */

	tail.x = (orig.x = origin->x) - direction->x;
	tail.y = (orig.y = origin->y) - direction->y;
	tail.z = (orig.z = origin->z) - direction->z;

  EVWCtoVC(v_p, &orig, &vo);
  EVWCtoVC(v_p, &tail, &vt);

	length2d = sqrt((vo.u - vt.u)*(vo.u - vt.u) + (vo.v - vt.v)*(vo.v - vt.v));

	if(length2d > length3d)length2d = length3d;
	alpha = acos((double)(length2d / length3d));

	EVVCtoDC(v_p, &vo, &xo, &yo);
	EVVCtoDC(v_p, &vt, &xt, &yt);

	sizeDC = size / length3d * length2d;

	scale = 1.0;
	while(YES){
		dx = (double)xt - (double)xo;
		dy = (double)yt - (double)yo;
		lengthDC = sqrt(dx * dx + dy * dy);
		if(lengthDC > 10.0 && lengthDC < 1000.0){
			scale = sizeDC / lengthDC;
			if(shift != 0){
				shiftDC = shift / length3d * length2d;
				
				x_shift = (int)(-shiftDC * dx / lengthDC);
				y_shift = (int)(-shiftDC * dy / lengthDC);
			}
			break;
		}
		
		if(lengthDC == 0.0){
			if(length2d == 0.0)break;
			if(alpha > PI / 2.0 * 0.995)break;
		}

		if(lengthDC < 100.0){
			scale *= 10.0;
			sizeDC *= 10.0;
		}
		else{
			scale /= 10.0;
			sizeDC /= 10.0;
		}
		
		tail.x = orig.x - direction->x * scale;
		tail.y = orig.y - direction->y * scale;
		tail.z = orig.z - direction->z * scale;
		
		EVWCtoVC(v_p, &tail, &vt);
		EVVCtoDC(v_p, &vt, &xt, &yt);
	}

	tail.x = orig.x - direction->x * scale;
	tail.y = orig.y - direction->y * scale;
	tail.z = orig.z - direction->z * scale;

  EVWCtoVC(v_p, &tail, &vt);
	EVVCtoDC(v_p, &vt, &xt, &yt);

	beta = FAN + (1.0 - 2.0 * FAN / PI) * alpha;

	size3d = length3d * scale * rate;
	if(fabs(beta - PI / 2.0) > EPSILON){
		size2d = size3d * tan(FAN) / sin((double)beta);
		
		angle = atan2((double)(vt.v - vo.v), (double)(vt.u - vo.u));

		angle += beta;
		v1.u = vo.u + size2d * cos(angle);
		v1.v = vo.v + size2d * sin(angle);
		v1.n = vo.n;
		EVVCtoDC(v_p, &v1, &x1, &y1);
		
		angle -= 2.0 * beta;
		v2.u = vo.u + size2d * cos(angle);
		v2.v = vo.v + size2d * sin(angle);
		v2.n = vo.n;
		EVVCtoDC(v_p, &v2, &x2, &y2);

		if(type == DOUBLE_ARROW_VECMARKER || type == TRIPLE_ARROW_VECMARKER){
			coef = size3d / length3d / scale;
			du = (vt.u - vo.u) * coef;
			dv = (vt.v - vo.v) * coef;
			dn = (vt.n - vo.n) * coef;
		}
	}
	else{
		spec_case = YES;
		sizeDC = size * rate * SPEC;
		x1o = xo - sizeDC;
		y1o = yo;
		x1t = xo + sizeDC;
		y1t = yo;
		x2o = xo;
		y2o = yo - sizeDC;
		x2t = xo;
		y2t = yo + sizeDC;
	}

	if (EV_render_mode == FILLED_HIDDEN_RENDERING ||
      EV_render_mode == CONST_SHADING_RENDERING) {
    if (ZBufferDepthAt(xo, yo) <= (float)vo.n) {
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

	if(spec_case == NO){
		if(shift != 0){
			xo += x_shift;
			yo += y_shift;
			xt += x_shift;
			yt += y_shift;
			x1 += x_shift;
			y1 += y_shift;
			x2 += x_shift;
			y2 += y_shift;
		}
	}

	XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
						(int)(xo), (int)(yo),
						(int)(xt), (int)(yt));

  switch (type) {
  case TRIPLE_ARROW_VECMARKER:
		if(spec_case == NO){
			v_o.u = vo.u + du * TRIPLE_RATE;
			v_o.v = vo.v + dv * TRIPLE_RATE;
			v_o.n = vo.n + dn * TRIPLE_RATE;
			EVVCtoDC(v_p, &v_o, &x_o, &y_o);
			d_x = x_o - xo;
			d_y = y_o - yo;
			x_1 = x1 + d_x;
			y_1 = y1 + d_y;
			x_2 = x2 + d_x;
			y_2 = y2 + d_y;
			if(shift != 0){
				x_o += x_shift;
				y_o += y_shift;
				x_1 += x_shift;
				y_1 += y_shift;
				x_2 += x_shift;
				y_2 += y_shift;
			}
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x_o), (int)(y_o),
								(int)(x_1), (int)(y_1));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x_o), (int)(y_o),
								(int)(x_2), (int)(y_2));
		}
  case DOUBLE_ARROW_VECMARKER:
		if(spec_case == NO){
			v_o.u = vo.u + du * DOUBLE_RATE;
			v_o.v = vo.v + dv * DOUBLE_RATE;
			v_o.n = vo.n + dn * DOUBLE_RATE;
			EVVCtoDC(v_p, &v_o, &x_o, &y_o);
			d_x = x_o - xo;
			d_y = y_o - yo;
			x_1 = x1 + d_x;
			y_1 = y1 + d_y;
			x_2 = x2 + d_x;
			y_2 = y2 + d_y;
			if(shift != 0){
				x_o += x_shift;
				y_o += y_shift;
				x_1 += x_shift;
				y_1 += y_shift;
				x_2 += x_shift;
				y_2 += y_shift;
			}
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x_o), (int)(y_o),
								(int)(x_1), (int)(y_1));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x_o), (int)(y_o),
								(int)(x_2), (int)(y_2));
		}
  case ARROW_VECMARKER:
		if(spec_case == NO){
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(xo), (int)(yo),
								(int)(x1), (int)(y1));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(xo), (int)(yo),
								(int)(x2), (int)(y2));
		}
		else{
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1o), (int)(y1o),
								(int)(x1t), (int)(y1t));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x2o), (int)(y2o),
								(int)(x2t), (int)(y2t));
		}
		break;
  case TRIANGLE_VECMARKER:
		XSetFillStyle(EV_display, EV_curr_GC, FillSolid);
		if(spec_case == NO){
			points[0].x = xo;
			points[0].y = yo;
			points[1].x = x1;
			points[1].y = y1;
			points[2].x = x2;
			points[2].y = y2;
			points[3].x = xo;
			points[3].y = yo;
			np = 4;
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



