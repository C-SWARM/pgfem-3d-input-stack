
#include "Eprimtv.h"

#define POINTS_IDENTICAL(P1, P2) \
     ((P1.x == P2.x) && (P1.y == P2.y) && (P1.z == P2.z))

#define SHRINK_VALUES(SHRINK, V1, V2, V3, V4, VC)          \
  {                                                        \
    (V1) = SHRINK * (V1) + (1 - SHRINK) * (VC);            \
    (V2) = SHRINK * (V2) + (1 - SHRINK) * (VC);            \
    (V3) = SHRINK * (V3) + (1 - SHRINK) * (VC);            \
    (V4) = SHRINK * (V4) + (1 - SHRINK) * (VC);            \
  }


#define SHRINK_RAPRISM_DATA(theSHRINK, UPPER, LOWER)            \
  {                                                             \
     double theCV;                                              \
                                                                \
     theCV = (UPPER) + (LOWER);                                 \
     theCV *= 0.5;                                              \
     (UPPER) = theCV + (theSHRINK) * ((UPPER)  - theCV);        \
     (LOWER) = theCV + (theSHRINK) * ((LOWER)  - theCV);        \
   }


void 
EVRAPrismPrimitive(EView        *v_p,
                   GraphicObj   *g_p,
                   WCRec        *ll,
                   WCRec        *ur,
                   EPixel        pixel,
                   int           fill,
                   int           edge_flag,
                   EPixel        edge_pixel,
                   float         shrink,
                   EDrawMode     draw_mode)
{
  WCRec points[8], vpn;
  int how_many_in = 0;
  EV_line_end_point_rec ap;
  double b, c;
  int i;
  BOOLEAN mclip;

  COPY_WCREC(&points[0], ll);
  COPY_WCREC(&points[1], ur);
  if (shrink != 1) {
    SHRINK_RAPRISM_DATA(shrink, points[1].x, points[0].x);
    SHRINK_RAPRISM_DATA(shrink, points[1].y, points[0].y);
    SHRINK_RAPRISM_DATA(shrink, points[1].z, points[0].z);
  }
  b = (ur->y - ll->y) * shrink;
  c = (ur->z - ll->z) * shrink;
  COPY_WCREC(&points[2], &points[0]);
  points[2].y += b;
  COPY_WCREC(&points[6], &points[0]);
  points[6].z += c;
  COPY_WCREC(&points[4], &points[2]);
  points[4].z += c;
  COPY_WCREC(&points[3], &points[1]);
  points[3].y -= b;
  COPY_WCREC(&points[5], &points[3]);
  points[5].z -= c;
  COPY_WCREC(&points[7], &points[5]);
  points[7].y += b;

  mclip = (v_p->should_clip_by_model_planes
           && EGIsClippable(g_p)
           && v_p->model_clip_plane.clip_is_on);
  if (mclip) {
    if (EGInvisibleWhenClipped(g_p)) {
      for (i = 0; i < 8; i++) {
        COPY_WCREC(&ap.loc.wc, &points[i]);
        if (point_is_in_IN_halfspace(&ap,
                                     &v_p->model_clip_plane.center,
                                     &v_p->model_clip_plane.normal))
          how_many_in++;
      }
      if (how_many_in == 0)
        return;
      if (how_many_in != 8)
        return;
    }
  }
      
  if (v_p->render_mode == WIRE_RENDERING || fill == FILL_HOLLOW) {
    if (draw_mode == DRAW_MODE_XORDRAW
        && POINTS_IDENTICAL(points[1], points[7]))
      EVLine3DPrimitive(v_p, g_p, &points[0], &points[1],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    else {
      EVLine3DPrimitive(v_p, g_p, &points[0], &points[2],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[0], &points[6],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[0], &points[5],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[1], &points[3],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[1], &points[7],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[1], &points[4],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[7], &points[2],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[4], &points[2],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[6], &points[3],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[5], &points[3],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[5], &points[7],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &points[6], &points[4],
                        SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    }
  } else {
    float s = 1;

    
#define BACKFACE_CULL(FACE_NORMAL, VIEW_PLANE_NORMAL) \
   ((FACE_NORMAL) * (VIEW_PLANE_NORMAL) > 0               \
    || mclip)
     
    EVGetNormal(v_p, &vpn);

    if (BACKFACE_CULL(1, vpn.x))
      EVQuad3DPrimitive(v_p, g_p,
                        &points[1], &points[3], &points[5], &points[7],
                        pixel, fill, edge_flag, edge_pixel, s, draw_mode);
    if (BACKFACE_CULL(-1, vpn.x))
      EVQuad3DPrimitive(v_p, g_p,
                        &points[0], &points[2], &points[4], &points[6],
                        pixel, fill, edge_flag, edge_pixel, s, draw_mode);
    if (BACKFACE_CULL(1, vpn.y))
      EVQuad3DPrimitive(v_p, g_p,
                        &points[1], &points[7], &points[2], &points[4],
                        pixel, fill, edge_flag, edge_pixel, s, draw_mode);
    if (BACKFACE_CULL(-1, vpn.y))
      EVQuad3DPrimitive(v_p, g_p,
                        &points[0], &points[5], &points[3], &points[6],
                        pixel, fill, edge_flag, edge_pixel, s, draw_mode);
    if (BACKFACE_CULL(1, vpn.z))
      EVQuad3DPrimitive(v_p, g_p,
                        &points[1], &points[4], &points[6], &points[3],
                        pixel, fill, edge_flag, edge_pixel, s, draw_mode);
    if (BACKFACE_CULL(-1, vpn.z))
      EVQuad3DPrimitive(v_p, g_p,
                        &points[0], &points[5], &points[7], &points[2],
                        pixel, fill, edge_flag, edge_pixel, s, draw_mode);
    
    
    if (edge_flag) {
      /* RAW: needs to be completed yet */
    }
  }
}
