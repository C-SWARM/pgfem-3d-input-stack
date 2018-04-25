
#include "Eprimtv.h"

void 
EVMarker3DPrimitive(EView      *v_p,
                    GraphicObj   *g_p, 
		    WCRec      *center,
		    unsigned    size,
		    EMarkerType type,
		    EPixel      pixel,
		    EDrawMode   draw_mode)
{
  int x1, y1;
  Dimension size2;
  VCRec vc;
  BOOLEAN is_visible = YES;
  EV_line_end_point_rec p;
	unsigned width = 0;
	int style = SOLID_STYLE;

  if (v_p->should_clip_by_model_planes && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
      COPY_WCREC(&p.loc.wc, center);
      if (!point_is_in_IN_halfspace(&p, &v_p->model_clip_plane.center,
                                    &v_p->model_clip_plane.normal))
        return;
    }
  }

  EVWCtoVC(v_p, center, &vc);
  EVVCtoDC(v_p, &vc, &x1, &y1);
  size2 = size >2? size/2+1: 1;
   
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
		XSetLineAttributes(EV_display, EV_curr_GC, width, style,
                       CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_UNHILITE) {
    EV_curr_GC    = v_p->erasehiliteGC;
    EV_curr_pixel = v_p->background;
		XSetLineAttributes(EV_display, EV_curr_GC, width, style,
                       CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_XORDRAW) {
    EV_curr_GC    = v_p->defaultXORGC;
    EV_curr_pixel = v_p->foreground;
		XSetLineAttributes(EV_display, EV_curr_GC, width, style,
                       CapButt, JoinMiter);
  }
  EV_render_mode = v_p->render_mode;
  EV_shade_mode  = v_p->shade_mode;
  
  x1 -= size2; y1 -= size2;
  
  switch (type) {
  case FILLED_CIRCLE_MARKER:
    XSetFillStyle(EV_display, EV_curr_GC, FillSolid);
    XFillArc(EV_display, EV_curr_win, EV_curr_GC, x1, y1, size, size, 64*360, 64*360);
    break;
  case X_LETTER_MARKER:
    XDrawLine(EV_display, EV_curr_win, EV_curr_GC, 
	      (int)(x1), (int)(y1),
	      (int)(x1+size), (int)(y1+size));
    XDrawLine(EV_display, EV_curr_win, EV_curr_GC,
	      (int)(x1), (int)(y1+size),
	      (int)(x1+size), (int)(y1));
    break;
  case SQUARE_MARKER:
    XDrawRectangle(EV_display, EV_curr_win, EV_curr_GC,
		   (int)(x1), (int)(y1),
		   (unsigned int)(size), (unsigned int)(size));
    break;
  case CIRCLE_MARKER:
  default:
    XDrawArc(EV_display, EV_curr_win, EV_curr_GC,
             x1, y1, size, size, 64*360, 64*360);
    break;
  }
}

