
#include "Eprimtv.h"


void 
EVHexahPrimitive(EView        *v_p,
                 GraphicObj   *g_p,
                 WCRec         points[8],
                 EPixel        pixel,
                 int           fill,
                 int           edge_flag,
                 EPixel        edge_color,
                 float         shrink,
                 EDrawMode     draw_mode)
{
  WCRec s[8];
  int how_many_in = 0;
  EV_line_end_point_rec ap;
  BOOLEAN mclip;
  int i;
  WCRec c = {0, 0, 0}; 

  for (i = 0; i < 8; i++) {
    COPY_WCREC(&s[i], &points[i]);
  }
  if (shrink != 1) {
    for (i = 0; i < 8; i++) {
      c.x += points[i].x;
      c.y += points[i].y;
      c.z += points[i].z;
    }
    c.x /= 8;
    c.y /= 8;
    c.z /= 8;
    for (i = 0; i < 8; i++) {
      s[i].x = c.x + shrink * (points[i].x - c.x);
      s[i].y = c.y + shrink * (points[i].y - c.y);
      s[i].z = c.z + shrink * (points[i].z - c.z);
    }
  }

  mclip = (v_p->should_clip_by_model_planes
           && EGIsClippable(g_p)
           && v_p->model_clip_plane.clip_is_on);
  if (mclip) {
    if (EGInvisibleWhenClipped(g_p)) {
      for (i = 0; i < 8; i++) {
        COPY_WCREC(&ap.loc.wc, &s[i]);
        if (point_is_in_IN_halfspace(&ap, &v_p->model_clip_plane.center,
                                     &v_p->model_clip_plane.normal))
          how_many_in++;
      }
      if (how_many_in != 8)
        return;
    }
  }
  
  if (v_p->render_mode == WIRE_RENDERING || fill == FILL_HOLLOW) {
    int ls = SOLID_STYLE;
    
    EVLine3DPrimitive(v_p, g_p, &s[0], &s[1], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[1], &s[2], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[2], &s[3], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[3], &s[0], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[4], &s[5], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[5], &s[6], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[6], &s[7], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[7], &s[4], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[4], &s[0], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[5], &s[1], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[6], &s[2], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[7], &s[3], ls, pixel, 0, 1.0, draw_mode);
    
  } else {
    EVQuad3DPrimitive(v_p, g_p, &s[0], &s[1], &s[2], &s[3], pixel, fill,
                      edge_flag, edge_color, 1.0, draw_mode);
    EVQuad3DPrimitive(v_p, g_p, &s[1], &s[5], &s[6], &s[2], pixel, fill,
                      edge_flag, edge_color, 1.0, draw_mode);
    EVQuad3DPrimitive(v_p, g_p, &s[2], &s[6], &s[7], &s[3], pixel, fill,
                      edge_flag, edge_color, 1.0, draw_mode);
    EVQuad3DPrimitive(v_p, g_p, &s[3], &s[7], &s[4], &s[0], pixel, fill,
                      edge_flag, edge_color, 1.0, draw_mode);
    EVQuad3DPrimitive(v_p, g_p, &s[0], &s[4], &s[5], &s[1], pixel, fill,
                      edge_flag, edge_color, 1.0, draw_mode);
    EVQuad3DPrimitive(v_p, g_p, &s[5], &s[4], &s[7], &s[6], pixel, fill,
                      edge_flag, edge_color, 1.0, draw_mode);
  }
}

