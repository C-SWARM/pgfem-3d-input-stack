
#include "Eprimtv.h"


#define SHRINK_TETRA_DATA(theSHRINK, fstV, scndV, thrdV, frthV)             \
  {                                                                         \
     double theCV;                                                          \
                                                                            \
     theCV   = (fstV); theCV += (scndV); theCV += (thrdV); theCV += (frthV);\
     theCV   *= 0.25;                                                       \
     (fstV)  = theCV + (theSHRINK) * ((fstV)  - theCV);                     \
     (scndV) = theCV + (theSHRINK) * ((scndV) - theCV);                     \
     (thrdV) = theCV + (theSHRINK) * ((thrdV) - theCV);                     \
     (frthV) = theCV + (theSHRINK) * ((frthV) - theCV);                     \
   }


void 
EVTetraWDPrimitive(EView        *v_p,
                   GraphicObj   *g_p,
                   WCRec        *first,
                   WCRec        *second,
                   WCRec        *third,
                   WCRec        *fourth,
                   FPNum         values[4],
                   EPixel        pixel,
                   int           fill,
                   int           edge_flag,
                   EPixel        edge_color,
                   float         shrink,
                   EDrawMode     draw_mode)
{
  WCRec s[4];
  int how_many_in = 0;
  EV_line_end_point_rec ap;
  BOOLEAN mclip;
  int i;

  COPY_WCREC(&s[0], first);
  COPY_WCREC(&s[1], second);
  COPY_WCREC(&s[2], third);
  COPY_WCREC(&s[3], fourth);
  if (shrink != 1) {
    SHRINK_TETRA_DATA(shrink, s[0].x, s[1].x, s[2].x, s[3].x);
    SHRINK_TETRA_DATA(shrink, s[0].y, s[1].y, s[2].y, s[3].y);
    SHRINK_TETRA_DATA(shrink, s[0].z, s[1].z, s[2].z, s[3].z);
  }

  mclip = (v_p->should_clip_by_model_planes
           && EGIsClippable(g_p)
           && v_p->model_clip_plane.clip_is_on);
  if (mclip) {
    if (EGInvisibleWhenClipped(g_p)) {
      for (i = 0; i < 4; i++) {
        COPY_WCREC(&ap.loc.wc, &s[i]);
        if (point_is_in_IN_halfspace(&ap, &v_p->model_clip_plane.center,
                                     &v_p->model_clip_plane.normal))
          how_many_in++;
      }
      if (how_many_in != 4)
        return;
    }
  }
  
  if (v_p->render_mode == WIRE_RENDERING || fill == FILL_HOLLOW) {
    int ls = SOLID_STYLE;
    
    EVLine3DPrimitive(v_p, g_p, &s[0], &s[1], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[1], &s[2], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[2], &s[0], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[3], &s[0], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[3], &s[1], ls, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s[3], &s[2], ls, pixel, 0, 1.0, draw_mode);

  } else {
#define INVOKE_TetraWD3D_PRIM(n0, n1, n2)                                \
    EVTriangleWD3DPrimitive(v_p, g_p, &s[n0], &s[n1], &s[n2],            \
                        values[n0], values[n1], values[n2],              \
                        pixel, fill,                                     \
                        edge_flag, edge_color, 1.0, draw_mode)

    INVOKE_TetraWD3D_PRIM(0, 1, 2);
    INVOKE_TetraWD3D_PRIM(0, 1, 3); 
    INVOKE_TetraWD3D_PRIM(1, 2, 3);
    INVOKE_TetraWD3D_PRIM(2, 0, 3);
  }
} 
