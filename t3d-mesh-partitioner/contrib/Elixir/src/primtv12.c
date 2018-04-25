
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
EVTetraPrimitive(EView        *v_p,
                 GraphicObj   *g_p,
                 WCRec        *first,
                 WCRec        *second,
                 WCRec        *third,
                 WCRec        *fourth,
                 EPixel        pixel,
                 int           fill,
                 int           edge_flag,
                 EPixel        edge_color,
                 float         shrink,
                 EDrawMode     draw_mode)
{
  WCRec s1, s2, s3, s4;
  int how_many_in = 0;
  EV_line_end_point_rec ap;
  BOOLEAN mclip;

  COPY_WCREC(&s1, first);
  COPY_WCREC(&s2, second);
  COPY_WCREC(&s3, third);
  COPY_WCREC(&s4, fourth);
  if (shrink != 1) {
    SHRINK_TETRA_DATA(shrink, s1.x, s2.x, s3.x, s4.x);
    SHRINK_TETRA_DATA(shrink, s1.y, s2.y, s3.y, s4.y);
    SHRINK_TETRA_DATA(shrink, s1.z, s2.z, s3.z, s4.z);
  }

  mclip = (v_p->should_clip_by_model_planes
           && EGIsClippable(g_p)
           && v_p->model_clip_plane.clip_is_on);
  if (mclip) {
    if (EGInvisibleWhenClipped(g_p)) {
      COPY_WCREC(&ap.loc.wc, &s1);
      if (point_is_in_IN_halfspace(&ap, &v_p->model_clip_plane.center,
                                   &v_p->model_clip_plane.normal))
        how_many_in++;
      COPY_WCREC(&ap.loc.wc, &s2);
      if (point_is_in_IN_halfspace(&ap, &v_p->model_clip_plane.center,
                                   &v_p->model_clip_plane.normal))
        how_many_in++;
      COPY_WCREC(&ap.loc.wc, &s3);
      if (point_is_in_IN_halfspace(&ap, &v_p->model_clip_plane.center,
                                   &v_p->model_clip_plane.normal))
        how_many_in++;
      COPY_WCREC(&ap.loc.wc, &s4);
      if (point_is_in_IN_halfspace(&ap, &v_p->model_clip_plane.center,
                                   &v_p->model_clip_plane.normal))
        how_many_in++;
      if (how_many_in == 0)
        return;
      if (how_many_in != 4)
        return;
    }
  }
  
  if (v_p->render_mode == WIRE_RENDERING || fill == FILL_HOLLOW) {
    EVLine3DPrimitive(v_p, g_p, &s1, &s2, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s2, &s3, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s3, &s1, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s1, &s4, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s2, &s4, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    EVLine3DPrimitive(v_p, g_p, &s3, &s4, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
  } else {
    WCRec vpn, n, V1, V2, V3, *v1 = &s1, *v2 = &s2, *v3 = &s3, *v4 = &s4;
    
    /* Cull the back-facing triangles */

#define TRI_NORMAL(RESLT_VEC, VEC1, VEC2)                            \
    {                                                                \
    (RESLT_VEC)->x = (VEC1)->y * (VEC2)->z - (VEC1)->z * (VEC2)->y;  \
    (RESLT_VEC)->y = (VEC1)->z * (VEC2)->x - (VEC1)->x * (VEC2)->z;  \
    (RESLT_VEC)->z = (VEC1)->x * (VEC2)->y - (VEC1)->y * (VEC2)->x;  \
    }

#define SUBTR_WCRECS(RESLT_VEC, VEC1, VEC2)                \
    {                                                      \
    (RESLT_VEC)->x = (VEC1)->x - (VEC2)->x;                \
    (RESLT_VEC)->y = (VEC1)->y - (VEC2)->y;                \
    (RESLT_VEC)->z = (VEC1)->z - (VEC2)->z;                \
    }

#define DPROD(VEC1, VEC2) \
   ((VEC1)->x * (VEC2)->x + (VEC1)->y * (VEC2)->y + (VEC1)->z * (VEC2)->z) 

    SUBTR_WCRECS(&V1, v1, v2);
    SUBTR_WCRECS(&V2, v1, v3);
    SUBTR_WCRECS(&V3, v1, v4);
    TRI_NORMAL(&n, &V1, &V2);
    if (DPROD(&V3, &n) < 0) {
      /* Swap 2nd and 3rd vertices */
      v2 = &s3; v3 = &s2;
    }

#define BACKFACE_CULL(TRI_NORMAL_PTR, VIEW_PLANE_NORMAL_PTR) \
   (DPROD(TRI_NORMAL_PTR, VIEW_PLANE_NORMAL_PTR) > 0         \
    || mclip)
     
    EVGetNormal(v_p, &vpn);

    SUBTR_WCRECS(&V1, v1, v2);
    SUBTR_WCRECS(&V2, v1, v3);
    TRI_NORMAL(&n, &V1, &V2);
    if (BACKFACE_CULL(&n, &vpn)) 
      EVTriangle3DPrimitive(v_p, g_p, v1, v2, v3, pixel, fill,
                            edge_flag, edge_color, 1.0, draw_mode);

    SUBTR_WCRECS(&V1, v1, v3);
    SUBTR_WCRECS(&V2, v1, v4);
    TRI_NORMAL(&n, &V1, &V2);
    if (BACKFACE_CULL(&n, &vpn)) 
      EVTriangle3DPrimitive(v_p, g_p, v1, v4, v3, pixel, fill,
                            edge_flag, edge_color, 1.0, draw_mode);

    SUBTR_WCRECS(&V1, v3, v2);
    SUBTR_WCRECS(&V2, v3, v4);
    TRI_NORMAL(&n, &V1, &V2);
    if (BACKFACE_CULL(&n, &vpn)) 
      EVTriangle3DPrimitive(v_p, g_p, v2, v4, v3, pixel, fill,
                            edge_flag, edge_color, 1.0, draw_mode);

    SUBTR_WCRECS(&V1, v2, v1);
    SUBTR_WCRECS(&V2, v2, v4);
    TRI_NORMAL(&n, &V1, &V2);
    if (BACKFACE_CULL(&n, &vpn)) 
      EVTriangle3DPrimitive(v_p, g_p, v1, v4, v2, pixel, fill,
                            edge_flag, edge_color, 1.0, draw_mode);

  }
}

