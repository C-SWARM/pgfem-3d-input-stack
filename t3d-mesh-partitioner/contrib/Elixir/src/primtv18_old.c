
#include "Eprimtv.h"


#define VEC_MIN_LENGTH 40.0
#define VEC_MAX_LENGTH 1000.0
#define DOUBLE_RATE    0.30       /* rate of arrow segment length for shift of double arrow from tip */
#define TRIPLE_RATE    1.00       /* rate of arrow segment length for shift of triple arrow from tip */ 
#define FAN            PI/6.0     /* declination of arrow segment from leading line */


void 
EVVecMarker3DPrimitive(EView      *v_p,
                    GraphicObj   *g_p, 
		    WCRec      *origin,
		    WCRec      *direction,
		    unsigned    size,
		    unsigned    shift,
				FPNum       rate,
		    EVecMarkerType type,
		    EPixel      pixel,
		    EDrawMode   draw_mode)
{
  int x1, y1, x2, y2, x3, y3, x4, y4, x_shift, y_shift;
	int dx, dy, xx, yy, np;
	FPNum arrow_size, coef, arrow_shift;
  Dimension size2;
  VCRec vc, vd;
	WCRec vv, vvv;
  BOOLEAN is_visible = YES;
  EV_line_end_point_rec p;
	double angle, length;
	XPoint points[5];

	arrow_size = size * rate;

  if (v_p->should_clip_by_model_planes && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
      COPY_WCREC(&p.loc.wc, origin);
      if (!point_is_in_IN_halfspace(&p, &v_p->model_clip_plane.center,
                                    &v_p->model_clip_plane.normal))
        return;
    }
  }

	vv.x = origin -> x - (vvv.x = direction -> x);
	vv.y = origin -> y - (vvv.y = direction -> y);
	vv.z = origin -> z - (vvv.z = direction -> z);

  EVWCtoVC(v_p, origin, &vc);
	EVWCtoVC(v_p, &vv, &vd);
	EVVCtoDC(v_p, &vc, &x1, &y1);
	EVVCtoDC(v_p, &vd, &x2, &y2);

	length = sqrt((double)(x2 - x1) * (double)(x2 - x1) + (double)(y2 - y1) * (double)(y2 - y1));

	if(length > 3.0){
		if(length < VEC_MIN_LENGTH){
			vv.x = origin -> x - direction -> x / length * VEC_MIN_LENGTH;
			vv.y = origin -> y - direction -> y / length * VEC_MIN_LENGTH;
			vv.z = origin -> z - direction -> z / length * VEC_MIN_LENGTH;
			EVWCtoVC(v_p, &vv, &vd);
			EVVCtoDC(v_p, &vd, &x2, &y2);
			length = sqrt((double)(x2 - x1) * (double)(x2 - x1) + (double)(y2 - y1) * (double)(y2 - y1));
		}
		while(length > VEC_MAX_LENGTH){
			vvv.x = vvv.x / length * VEC_MAX_LENGTH;
			vvv.y = vvv.y / length * VEC_MAX_LENGTH;
			vvv.z = vvv.z / length * VEC_MAX_LENGTH;
			vv.x = origin -> x - vvv.x;
			vv.y = origin -> y - vvv.y;
			vv.z = origin -> z - vvv.z;
			EVWCtoVC(v_p, &vv, &vd);
			EVVCtoDC(v_p, &vd, &x2, &y2);
			length = sqrt((double)(x2 - x1) * (double)(x2 - x1) + (double)(y2 - y1) * (double)(y2 - y1));
		}

		angle = atan2((double)(y2 - y1), (double)(x2 - x1));
		angle += FAN;
		
		if(shift != 0.0){
			x_shift = (int)((x2 - x1) / length * shift);
			y_shift = (int)((y2 - y1) / length * shift);
		}

		x3 = x1 + (int)(arrow_size * cos(angle)); 
		y3 = y1 + (int)(arrow_size * sin(angle)); 

		angle -= 2.0 * FAN;

		x4 = x1 + (int)(arrow_size * cos(angle)); 
		y4 = y1 + (int)(arrow_size * sin(angle)); 

		x2 = x1 + (int)((x2 - x1) / length * size);
		y2 = y1 + (int)((y2 - y1) / length * size);
		
		if(shift != 0.0){
			x1 += x_shift;
			y1 += y_shift;
			x2 += x_shift;
			y2 += y_shift;
			x3 += x_shift;
			y3 += y_shift;
			x4 += x_shift;
			y4 += y_shift;
		}
	}

  if (EV_render_mode == FILLED_HIDDEN_RENDERING ||
      EV_render_mode == CONST_SHADING_RENDERING) {
    if (ZBufferDepthAt(x1, y1) <= (float)vc.n) {
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
  
	if(type == DOUBLE_ARROW_VECMARKER || type == TRIPLE_ARROW_VECMARKER){
		coef = arrow_size * DOUBLE_RATE / size;
		dx = (int)(x2 - x1) * coef;
		dy = (int)(y2 - y1) * coef;
	}

  switch (type) {
  case TRIPLE_ARROW_VECMARKER:
		if(length > 3.0){
			coef = TRIPLE_RATE / DOUBLE_RATE;
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1 + coef * dx), (int)(y1 + coef * dy),
								(int)(x3 + coef * dx), (int)(y3 + coef * dy));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1 + coef * dx), (int)(y1 + coef * dy),
								(int)(x4 + coef * dx), (int)(y4 + coef * dy));
		}
  case DOUBLE_ARROW_VECMARKER:
		if(length > 3.0){
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1 + dx), (int)(y1 + dy),
								(int)(x3 + dx), (int)(y3 + dy));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1 + dx), (int)(y1 + dy),
								(int)(x4 + dx), (int)(y4 + dy));
		}
  case ARROW_VECMARKER:
		if(length > 3.0){
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1), (int)(y1),
								(int)(x2), (int)(y2));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1), (int)(y1),
								(int)(x3), (int)(y3));
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1), (int)(y1),
								(int)(x4), (int)(y4));
		}
		else{
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC,
								(int)(x1 - 0.7 * arrow_size), (int)y1,
								(int)(x1 + 0.7 * arrow_size), (int)y1);
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC,
								(int)x1, (int)(y1 - 0.7 * arrow_size),
								(int)x1, (int)(y1 + 0.7 * arrow_size));
		}
		
		break;
  case TRIANGLE_VECMARKER:
		XSetFillStyle(EV_display, EV_curr_GC, FillSolid);
		if(length > 3.0){
			XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
								(int)(x1), (int)(y1),
								(int)(x2), (int)(y2));
			points[0].x = x1;
			points[0].y = y1;
			points[1].x = x3;
			points[1].y = y3;
			points[2].x = x4;
			points[2].y = y4;
			points[3].x = x1;
			points[3].y = y1;
			np = 4;
		}
		else{
			points[0].x = (int)(x1 - 0.7 * arrow_size);
			points[0].y = y1;
			points[1].x = x1; 
			points[1].y = (int)(y1 - 0.7 * arrow_size); 
			points[2].x = (int)(x1 + 0.7 * arrow_size);
			points[2].y = y1; 
			points[3].x = x1;
			points[3].y = (int)(y1 + 0.7 * arrow_size);
			points[4].x = points[0].x;
			points[4].y = points[0].y;
			np = 5;
		}
		XFillPolygon(EV_display, EV_curr_win, EV_curr_GC,
								 points, np, Convex, CoordModeOrigin);
    break;
	default:
    XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
	      (int)(x1), (int)(y1),
	      (int)(x2), (int)(y2));

		break;
  }
}
