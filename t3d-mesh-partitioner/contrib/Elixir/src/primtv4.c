
#include "Eprimtv.h"

static char BUF[1024];

void 
EVAnnText3DPrimitive(EView         *v_p,
                     GraphicObj   *g_p, 
		     WCRec         *at,
		     char          *text,
		     Font          font_id,
		     EPixel        pixel,
		     EDrawMode     draw_mode)
{
  int x1, y1, delta;
  float dn;
  char *cp;
  int pos_in_buf;
  XFontStruct *fsp;
  VCRec vc;
  BOOLEAN is_visible = YES;
  EV_line_end_point_rec p;

  if (v_p->should_clip_by_model_planes  && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
      COPY_WCREC(&p.loc.wc, at);
      if (!point_is_in_IN_halfspace(&p, &v_p->model_clip_plane.center,
                                    &v_p->model_clip_plane.normal))
        return;
    }
  }
  
  EVWCtoVC(v_p, at, &vc);
  EVVCtoDC(v_p, &vc, &x1, &y1);
  dn = 2 * (v_p->view_dims_VC.u + v_p->view_dims_VC.v)/
    (v_p->view_dims_DC.x + v_p->view_dims_DC.y);
  
  if (EV_render_mode == FILLED_HIDDEN_RENDERING ||
      EV_render_mode == CONST_SHADING_RENDERING) {
    if (ZBufferDepthAt(x1, y1) <= (float)(vc.n+dn)) {
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
    EV_curr_GC    = v_p->writableGC;
    EV_curr_pixel = v_p->foreground;
    XSetFunction(EV_display, EV_curr_GC, GXxor);
  }
  XSetFont(EV_display, EV_curr_GC, font_id);
  EV_render_mode = v_p->render_mode;
  EV_shade_mode  = v_p->shade_mode;
  
  if ((fsp = FontGetFontInfo(font_id)) == (XFontStruct *)NULL)
    delta = 10;
  else
    delta = (fsp->ascent + fsp->descent)*1.2;
  cp     = text;
  BUF[0] = NULL_CHAR;
  pos_in_buf = 0;
  while (*cp != NULL_CHAR) {
    if (IS_NEWLINE_ESCAPE(cp)) {
      BUF[pos_in_buf] = NULL_CHAR;
      if (is_visible)
	XDrawString(EV_display, EV_curr_win, EV_curr_GC, x1, y1,
		    BUF, (signed)strlen(BUF));
      cp += 2;
      pos_in_buf = 0;
      BUF[0] = NULL_CHAR;
      y1 += delta;
    }
    BUF[pos_in_buf++] = *cp;
    cp++;
  }
  BUF[pos_in_buf] = NULL_CHAR;
  if (is_visible)
    XDrawString(EV_display, EV_curr_win, EV_curr_GC, x1, y1,
		BUF, (signed)strlen(BUF));

  if (draw_mode == DRAW_MODE_XORDRAW)
    XSetFunction(EV_display, EV_curr_GC, GXcopy);
}

