#include "Eprimtv.h"

#define SHRINK_LINE_DATA(theSHRINK, fstV, scndV) \
  {                                                         \
     double theCV;                                          \
                                                            \
     theCV   = ((fstV) + (scndV)) / 2.0;                    \
     (fstV)  = theCV + (theSHRINK) * ((fstV)  - theCV);     \
     (scndV) = theCV + (theSHRINK) * ((scndV) - theCV);     \
   }

 
void 
EVLine3DPrimitive(EView        *v_p,	
                  GraphicObj   *g_p,        
		  WCRec        *f,
		  WCRec        *s,
		  int           style,
		  EPixel        pixel,
		  unsigned      width,
			float         shrink,
		  EDrawMode     draw_mode) 
{
  int x1, x2, y1, y2;
  EV_line_clip_result clip_result;
  VCRec vboxll, vboxur, lf, ls, vcf, vcs;
  EV_line_end_point_rec p1, p2, p1_return, p2_return;
	WCRec sf, ss;
  
	if(shrink != 1){
		COPY_WCREC(&sf, f);
		COPY_WCREC(&ss, s);
    SHRINK_LINE_DATA(shrink, sf.x, ss.x);
    SHRINK_LINE_DATA(shrink, sf.y, ss.y);
    SHRINK_LINE_DATA(shrink, sf.z, ss.z);
		f = &sf;
		s = &ss;
	}

  if (v_p->should_clip_by_model_planes && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
      COPY_WCREC(&p1.loc.wc, f);
      COPY_WCREC(&p2.loc.wc, s);
      clip_result = clip_line_by_a_plane(NO, &p1, &p2,
                                         &v_p->model_clip_plane.center,
                                         &v_p->model_clip_plane.normal,
                                         &p1_return, &p2_return);
      if (clip_result == EV_LINE_IS_OUT)
        return;
      if (clip_result == EV_LINE_CLIPPED
          && EGInvisibleWhenClipped(g_p))
        return;
      f = &p1_return.loc.wc;
      s = &p2_return.loc.wc;
    }
  }
  
  EVGetViewBox(v_p, &vboxll, &vboxur);

	EVWCtoVC(v_p, f, &vcf);
	EVWCtoVC(v_p, s, &vcs);
  lf.u = vcf.u, lf.v = vcf.v, lf.n = vcf.n;
  ls.u = vcs.u, ls.v = vcs.v, ls.n = vcs.n;
  if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
		     &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n) == YES) {
    EV_display                    = XtDisplay(v_p->view_widget);
    EV_curr_win                   = v_p->draw_into;
    EV_should_clip_by_view_planes = v_p->should_clip_by_fg_bg_view_planes;
    EV_fg_plane_depth             = v_p->fg_view_plane_n;
    EV_bg_plane_depth             = v_p->bg_view_plane_n;

    if      (draw_mode == DRAW_MODE_DRAW) {
      EV_curr_GC    = v_p->writableGC;
      EV_curr_pixel = pixel;
      XSetForeground(EV_display, EV_curr_GC, pixel);
      XSetLineAttributes(EV_display, EV_curr_GC,
			 width, style, CapButt, JoinMiter);
    } else if (draw_mode == DRAW_MODE_ERASE) {
      EV_curr_GC    = v_p->eraseGC;
      EV_curr_pixel = v_p->background;
      XSetLineAttributes(EV_display, EV_curr_GC,
			 width, style, CapButt, JoinMiter);
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
    
    EVVCtoDC(v_p, &lf, &x1, &y1);
    EVVCtoDC(v_p, &ls, &x2, &y2);
    if (EV_render_mode == NORMAL_RENDERING ||
				EV_render_mode == WIRE_RENDERING) {
      XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
    } else {
      BHM(x1, y1, lf.n, x2, y2, ls.n);
    }
  }
}


