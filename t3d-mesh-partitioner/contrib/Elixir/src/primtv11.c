
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



#define COMPUTE_CENTER(CENTER, P1, P2, P3, P4)  \
  {                                             \
    (CENTER)->x =  (P1)->x;                     \
    (CENTER)->x += (P2)->x;                     \
    (CENTER)->x += (P3)->x;                     \
    (CENTER)->x += (P4)->x;                     \
    (CENTER)->x *= 0.25;                        \
    (CENTER)->y =  (P1)->y;                     \
    (CENTER)->y += (P2)->y;                     \
    (CENTER)->y += (P3)->y;                     \
    (CENTER)->y += (P4)->y;                     \
    (CENTER)->y *= 0.25;                        \
    (CENTER)->z =  (P1)->z;                     \
    (CENTER)->z += (P2)->z;                     \
    (CENTER)->z += (P3)->z;                     \
    (CENTER)->z += (P4)->z;                     \
    (CENTER)->z *= 0.25;                        \
  }

void 
EVQuadWD3DPrimitive(EView        *v_p,
                    GraphicObj   *g_p,
                    WCRec        *first,
                    WCRec        *second,
                    WCRec        *third,
                    WCRec        *fourth,
                    double        val1,
                    double        val2,
                    double        val3,
                    double        val4,
                    EPixel        pixel,
                    int           fill,
                    int           edge_flag,
                    EPixel        edge_pixel,
                    float         shrink,
                    EDrawMode     draw_mode)
{
  WCRec s1, s2, s3, s4, center;
  double v1 = val1, v2 = val2, v3 = val3, v4 = val4;
  double vc = 0.25*(v1+v2+v3+v4);
  int how_many_in = 0;
  EV_line_end_point_rec ap;

  COPY_WCREC(&s1, first);
  COPY_WCREC(&s2, second);
  COPY_WCREC(&s3, third);
  COPY_WCREC(&s4, fourth);
  if (shrink != 1) {
    SHRINK_TETRA_DATA(shrink, s1.x, s2.x, s3.x, s4.x);
    SHRINK_TETRA_DATA(shrink, s1.y, s2.y, s3.y, s4.y);
    SHRINK_TETRA_DATA(shrink, s1.z, s2.z, s3.z, s4.z);
    SHRINK_TETRA_DATA(shrink, v1, v2, v3, v4);
  }

  if (v_p->should_clip_by_model_planes  && EGIsClippable(g_p)) {
    if (v_p->model_clip_plane.clip_is_on) {
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
  }

	if(IsFringeTableContourScale()){
		if(EV_render_mode != WIRE_RENDERING && fill != FILL_HOLLOW){
			if (edge_flag) {
				int ls = SOLID_STYLE;
				double dn = DELTA_N();
				WCRec n, p1, p2, p3, p4;

/* decrease n by small value to make contours continuous */
#define DECR(p, dn, n) { p.x -= dn * n.x; p.y -= dn * n.y; p.z -= dn * n.z; }
				EVGetNormal(v_p, &n);
				COPY_WCREC(&p1, &s1); DECR(p1, dn, n);
				COPY_WCREC(&p2, &s2); DECR(p2, dn, n);
				COPY_WCREC(&p3, &s3); DECR(p3, dn, n);
				COPY_WCREC(&p4, &s4); DECR(p4, dn, n);
				EVLine3DPrimitive(v_p, g_p, &p1, &p2, ls, edge_pixel, 0, 1.0, draw_mode);
				EVLine3DPrimitive(v_p, g_p, &p2, &p3, ls, edge_pixel, 0, 1.0, draw_mode);
				EVLine3DPrimitive(v_p, g_p, &p3, &p4, ls, edge_pixel, 0, 1.0, draw_mode);
				EVLine3DPrimitive(v_p, g_p, &p4, &p1, ls, edge_pixel, 0, 1.0, draw_mode);
			}
					
			COMPUTE_CENTER(&center, &s1, &s2, &s3, &s4);
			EVTriangleWD3DPrimitive(v_p, g_p, &s1, &s2, &center, v1, v2, vc, pixel, fill, NO, 0, 1.0, draw_mode);
			EVTriangleWD3DPrimitive(v_p, g_p, &s2, &s3, &center, v2, v3, vc, pixel, fill, NO, 0, 1.0, draw_mode);
			EVTriangleWD3DPrimitive(v_p, g_p, &s3, &s4, &center, v3, v4, vc, pixel, fill, NO, 0, 1.0, draw_mode);
			EVTriangleWD3DPrimitive(v_p, g_p, &s4, &s1, &center, v4, v1, vc, pixel, fill, NO, 0, 1.0, draw_mode);
			return;
		}
	}
      
  if (v_p->render_mode == WIRE_RENDERING || fill == FILL_HOLLOW) {
    if (draw_mode == DRAW_MODE_XORDRAW
        && POINTS_IDENTICAL(s3, s4) && POINTS_IDENTICAL(s3, s2))
      EVLine3DPrimitive(v_p, g_p, &s1, &s2, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    else {
      EVLine3DPrimitive(v_p, g_p, &s1, &s2, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &s2, &s3, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &s3, &s4, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &s4, &s1, SOLID_STYLE, pixel, 0, 1.0, draw_mode);
    }
  } else {
    float s = 1;
    COMPUTE_CENTER(&center, &s1, &s2, &s3, &s4);
    EVTriangleWD3DPrimitive(v_p, g_p, &s1, &s2, &center,
                            v1, v2, vc, pixel, fill,
                            NO, edge_pixel, s, draw_mode);
    EVTriangleWD3DPrimitive(v_p, g_p, &s2, &s3, &center,
                            v2, v3, vc, pixel, fill,
                            NO, edge_pixel, s, draw_mode);
    EVTriangleWD3DPrimitive(v_p, g_p, &s3, &s4, &center,
                            v3, v4, vc, pixel, fill,
                            NO, edge_pixel, s, draw_mode);
    EVTriangleWD3DPrimitive(v_p, g_p, &s4, &s1, &center,
                            v4, v1, vc, pixel, fill,
                            NO, edge_pixel, s, draw_mode);
    if (edge_flag) {
      int ls = SOLID_STYLE;
      double dn = DELTA_N();
      WCRec n, p1, p2, p3, p4;

#define INCR(p, dn, n) { p.x += dn * n.x; p.y += dn * n.y; p.z += dn * n.z; }
      EVGetNormal(v_p, &n);
      COPY_WCREC(&p1, &s1); INCR(p1, dn, n);
      COPY_WCREC(&p2, &s2); INCR(p2, dn, n);
      COPY_WCREC(&p3, &s3); INCR(p3, dn, n);
      COPY_WCREC(&p4, &s4); INCR(p4, dn, n);
      EVLine3DPrimitive(v_p, g_p, &p1, &p2, ls, edge_pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &p2, &p3, ls, edge_pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &p3, &p4, ls, edge_pixel, 0, 1.0, draw_mode);
      EVLine3DPrimitive(v_p, g_p, &p4, &p1, ls, edge_pixel, 0, 1.0, draw_mode);
    }
  }
}
