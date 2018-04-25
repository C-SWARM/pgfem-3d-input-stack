#include "Eprimtv.h"


void 
EVRBezS3DPrimitive(EView        *v_p,
                   GraphicObj   *g_p, 
                   int           order_u,
                   int           order_v,
                   WCRec         *wcpoints,
                   float         *weights,
                   int           style,
                   EPixel        pixel,
                   unsigned      line_width,
                   int           fill,
                   int           edge_flag,
                   EPixel        edge_pixel,
                   int           tessel_u,   
                   int           tessel_v,   
                   BOOLEAN       show_poly,   
                   BOOLEAN       boundary_only,
                   EDrawMode     draw_mode)
{
  int x1, x2, y1, y2, x3, y3;
  unsigned int width, height;
  FPNum u, v, delta_u = 1.0/tessel_u, delta_v = 1.0/tessel_v;
  double *bu, *bv, weight, tmp;
  int i, j, k, m, n, order = order_u * order_v;
  VCRec p1, p2, p3, p4, f, s, *points;
  VCRec vboxll, vboxur, lf, ls;
  XPoint xpoints[3];
  EPixel entity_color = 0;
  EV_line_end_point_rec pp1, pp2, p1_return, p2_return;
  BOOLEAN test_mclip, to_draw = YES;
  VCRec model_clip_plane_center, model_clip_plane_normal;
  EV_line_clip_result
    clip_result1 = EV_LINE_IS_IN, clip_result2 = EV_LINE_IS_IN,
    clip_result3 = EV_LINE_IS_IN;
  
  if((bu = (double *)calloc(order_u, sizeof(double))) == NULL){
    fprintf(stderr, "Memory allocation in EVRBezS3DPrimitive failed");
    return;
  }
  if((bv = (double *)calloc(order_v, sizeof(double))) == NULL){
    fprintf(stderr, "Memory allocation in EVRBezS3DPrimitive failed");
    free(bu);
    return;
  }
  if((points = (VCRec *)calloc(order, sizeof(VCRec))) == NULL){
    fprintf(stderr, "Memory allocation in EVRBezS3DPrimitive failed");
    free(bu);
    free(bv);
    return;
  }
  
  EV_display                    = XtDisplay(v_p->view_widget);
  EV_curr_win                   = v_p->draw_into;
  EV_should_clip_by_view_planes = v_p->should_clip_by_fg_bg_view_planes;
  EV_fg_plane_depth             = v_p->fg_view_plane_n;
  EV_bg_plane_depth             = v_p->bg_view_plane_n;
  width           = v_p->view_dims_DC.x; 
  height          = v_p->view_dims_DC.y;
  if      (draw_mode == DRAW_MODE_DRAW) {
    EV_curr_GC    = v_p->writableGC;
    EV_curr_pixel = entity_color = pixel;
    XSetForeground(EV_display, EV_curr_GC, pixel);
    XSetLineAttributes(EV_display, EV_curr_GC,
		       line_width, style, CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_ERASE) {
    EV_curr_GC    = v_p->eraseGC;
    EV_curr_pixel = entity_color = v_p->background;
    XSetLineAttributes(EV_display, EV_curr_GC,
		       line_width, style, CapButt, JoinMiter);
  } else if (draw_mode == DRAW_MODE_HILITE) {
    EV_curr_GC    = v_p->hiliteGC;
    EV_curr_pixel = entity_color = v_p->foreground;
  } else if (draw_mode == DRAW_MODE_UNHILITE) {
    EV_curr_GC    = v_p->erasehiliteGC;
    EV_curr_pixel = entity_color = v_p->background;
  } else if (draw_mode == DRAW_MODE_XORDRAW) {
    EV_curr_GC    = v_p->defaultXORGC;
    EV_curr_pixel = entity_color = v_p->foreground;
    tessel_u = order_u, delta_u = 1.0 / tessel_u;
    tessel_v = order_v, delta_v = 1.0 / tessel_v;
  }
  EV_render_mode = v_p->render_mode;
  EV_shade_mode  = v_p->shade_mode;
  EVGetViewBox(v_p, &vboxll, &vboxur);
  
  for (i = 0; i < order; i++)
    EVWCtoVC(v_p, &wcpoints[i], &points[i]);
  
  /* Model clipping */
  test_mclip = (v_p->should_clip_by_model_planes
                && v_p->model_clip_plane.clip_is_on
                && EGIsClippable(g_p));
  if (test_mclip) {
    int how_many_are_in = 0;
    WCRec Zero = {0, 0, 0};
    
    TransfPntWCToVC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
                    &(v_p->VCOrigin), &v_p->model_clip_plane.center,
                    &model_clip_plane_center);
    TransfPntWCToVC(&(v_p->VectorU), &(v_p->VectorV), &(v_p->Normal),
                    &Zero, &v_p->model_clip_plane.normal,
                    &model_clip_plane_normal);
    
    for (i = 0; i < order; i++) {
      COPY_VCREC(&pp1.loc.vc, &points[i]);
      if (point_is_in_IN_halfspace(&pp1, (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal))
        how_many_are_in++;
    }
    if (how_many_are_in == 0){
      free(bu);
      free(bv);
      free(points);
      return;
    }
    if (how_many_are_in == order)
      test_mclip = NO; /* no need to test -- the whole surface is in */
    else
      if (EGInvisibleWhenClipped(g_p)){
        free(bu);
        free(bv);
        free(points);
        return;
      }
  }
  
  if ((EV_render_mode == FILLED_HIDDEN_RENDERING ||
       EV_render_mode == CONST_SHADING_RENDERING ||
       EV_render_mode == NORMAL_RENDERING) && fill != FILL_HOLLOW) {
    
    for (u = ZERO, i = 0; i < tessel_u; u += delta_u, i++) {
      v = ZERO;
      Bernstein(order_u, (double)u, bu);
      Bernstein(order_v, (double)v, bv);
      p1.u = p1.v = p1.n = ZERO;
      weight = 0.0;
      n = 0;
      for (k = 0; k < order_v; k++){
        for (m = 0; m < order_u; m++, n++) {
          weight += (tmp = weights[n] * bv[k] * bu[m]);
          p1.u += points[n].u * tmp;
          p1.v += points[n].v * tmp;
          p1.n += points[n].n * tmp;
        }
      }
      p1.u /= weight;
      p1.v /= weight;
      p1.n /= weight;
      Bernstein(order_u, (double)(u+delta_u), bu);
      p2.u = p2.v = p2.n = ZERO;
      weight = 0.0;
      n = 0;
      for (k = 0; k < order_v; k++){
        for (m = 0; m < order_u; m++, n++) {
          weight += (tmp = weights[n] * bv[k] * bu[m]);
          p2.u += points[n].u * tmp;
          p2.v += points[n].v * tmp;
          p2.n += points[n].n * tmp;
        }
      }
      p2.u /= weight;
      p2.v /= weight;
      p2.n /= weight;
      for (v = delta_v, j = 0; j < tessel_v; v += delta_v, j++) {
        Bernstein(order_u, (double)u, bu);
        Bernstein(order_v, (double)v, bv);
        p3.u = p3.v = p3.n = ZERO;
        weight = 0.0;
        n = 0;
        for (k = 0; k < order_v; k++){
          for (m = 0; m < order_u; m++, n++) {
            weight += (tmp = weights[n] * bv[k] * bu[m]);
            p3.u += points[n].u * tmp;
            p3.v += points[n].v * tmp;
            p3.n += points[n].n * tmp;
          }
        }
        p3.u /= weight;
        p3.v /= weight;
        p3.n /= weight;
        Bernstein(order_u, (double)(u+delta_u), bu);
        p4.u = p4.v = p4.n = ZERO;
        weight = 0.0;
        n = 0;
        for (k = 0; k < order_v; k++){
          for (m = 0; m < order_u; m++, n++) {
            weight += (tmp = weights[n] * bv[k] * bu[m]);
            p4.u += points[n].u * tmp;
            p4.v += points[n].v * tmp;
            p4.n += points[n].n * tmp;
          }
        }
        p4.u /= weight;
        p4.v /= weight;
        p4.n /= weight;
        
        if (test_mclip) {
          COPY_VCREC(&pp1.loc.vc, &p1);
          COPY_VCREC(&pp2.loc.vc, &p2);
          clip_result1
            = clip_line_by_a_plane(NO, &pp1, &pp2,
                                   (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal,
                                   &p1_return, &p2_return);
          COPY_VCREC(&pp1.loc.vc, &p1);
          COPY_VCREC(&pp2.loc.vc, &p3);
          clip_result2
            = clip_line_by_a_plane(NO, &pp1, &pp2,
                                   (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal,
                                   &p1_return, &p2_return);
          COPY_VCREC(&pp1.loc.vc, &p2);
          COPY_VCREC(&pp2.loc.vc, &p3);
          clip_result3
            = clip_line_by_a_plane(NO, &pp1, &pp2,
                                   (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal,
                                   &p1_return, &p2_return);
        }
        
        if(clip_result1 == EV_LINE_IS_IN && 
           clip_result2 == EV_LINE_IS_IN && 
           clip_result3 == EV_LINE_IS_IN){ 
          if (EV_render_mode == CONST_SHADING_RENDERING) {
            VCRec normal, v1, v2;
            
            v1.u = p2.u - p1.u; v1.v = p2.v - p1.v; v1.n = p2.n - p1.n;
            v2.u = p3.u - p1.u; v2.v = p3.v - p1.v; v2.n = p3.n - p1.n;
            CrossProd3((WCRec *)&normal, (WCRec *)&v1, (WCRec *)&v2);
            if (NormalizeVect3((WCRec *)&normal)) {
              if (normal.n < 0) {
                normal.u= -normal.u; normal.v= -normal.v; normal.n= -normal.n;
              }
              EV_curr_cosine = EVGetLightDirVsNormalCos(v_p, &normal);
              if (EV_shade_mode == COLOR_SHADING) 
                EV_curr_pixel = ColorShadeColor(entity_color, EV_curr_cosine);
            }
          }
          EVVCtoDC(v_p, &p1, &x1, &y1);
          EVVCtoDC(v_p, &p2, &x2, &y2);
          EVVCtoDC(v_p, &p3, &x3, &y3);
          if (EV_render_mode == NORMAL_RENDERING) {
            xpoints[0].x = x1; xpoints[0].y = y1;
            xpoints[1].x = x2; xpoints[1].y = y2;
            xpoints[2].x = x3; xpoints[2].y = y3;
            XFillPolygon(EV_display, EV_curr_win, EV_curr_GC, 
                         xpoints, 3, Convex, CoordModeOrigin);
          } else {
            TF(x1, y1, p1.n, x2, y2, p2.n, x3, y3, p3.n, width, height);
          }
        }
        if (test_mclip) {
          COPY_VCREC(&pp1.loc.vc, &p2);
          COPY_VCREC(&pp2.loc.vc, &p3);
          clip_result1
            = clip_line_by_a_plane(NO, &pp1, &pp2,
                                   (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal,
                                   &p1_return, &p2_return);
          COPY_VCREC(&pp1.loc.vc, &p2);
          COPY_VCREC(&pp2.loc.vc, &p4);
          clip_result2
            = clip_line_by_a_plane(NO, &pp1, &pp2,
                                   (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal,
                                   &p1_return, &p2_return);
          COPY_VCREC(&pp1.loc.vc, &p3);
          COPY_VCREC(&pp2.loc.vc, &p4);
          clip_result3
            = clip_line_by_a_plane(NO, &pp1, &pp2,
                                   (WCRec *)&model_clip_plane_center,
                                   (WCRec *)&model_clip_plane_normal,
                                   &p1_return, &p2_return);
        }
        
        if(clip_result1 == EV_LINE_IS_IN && 
           clip_result2 == EV_LINE_IS_IN && 
           clip_result3 == EV_LINE_IS_IN){ 
          if (EV_render_mode == CONST_SHADING_RENDERING) {
            VCRec normal, v1, v2;
            
            v1.u = p2.u - p3.u; v1.v = p2.v - p3.v; v1.n = p2.n - p3.n;
            v2.u = p4.u - p3.u; v2.v = p4.v - p3.v; v2.n = p4.n - p3.n;
            CrossProd3((WCRec *)&normal, (WCRec *)&v1, (WCRec *)&v2);
            if (NormalizeVect3((WCRec *)&normal)) {
              if (normal.n < 0) {
                normal.u= -normal.u; normal.v= -normal.v; normal.n= -normal.n;
              }
              EV_curr_cosine = EVGetLightDirVsNormalCos(v_p, &normal);
              if (EV_shade_mode == COLOR_SHADING) 
                EV_curr_pixel = ColorShadeColor(entity_color, EV_curr_cosine);
            }
          }
          EVVCtoDC(v_p, &p4, &x1, &y1);
          EVVCtoDC(v_p, &p2, &x2, &y2);
          EVVCtoDC(v_p, &p3, &x3, &y3);
          if (EV_render_mode == NORMAL_RENDERING) {
            xpoints[0].x = x1; xpoints[0].y = y1;
            xpoints[1].x = x2; xpoints[1].y = y2;
            xpoints[2].x = x3; xpoints[2].y = y3;
            XFillPolygon(EV_display, EV_curr_win, EV_curr_GC, 
                         xpoints, 3, Convex, CoordModeOrigin);
          } else {
            TF(x1, y1, p4.n, x2, y2, p2.n, x3, y3, p3.n, width, height);
          }
        }
        p1.u = p3.u; p1.v = p3.v; p1.n = p3.n;
        p2.u = p4.u; p2.v = p4.v; p2.n = p4.n;
      } /* for (v = delta ...)  */
    } /* for (u = 0 ...) */
    /* Should edges be drawn in normal mode? */
    if (edge_flag) {
      EV_curr_pixel = edge_pixel;
			if(EV_render_mode == NORMAL_RENDERING){
				if(draw_mode == DRAW_MODE_DRAW) {
					XSetForeground(EV_display, EV_curr_GC, EV_curr_pixel);
					XSetLineAttributes(EV_display, EV_curr_GC,
														 line_width, style, CapButt, JoinMiter);
				}
			}
      for (u = 0, i = 0; i < 2; u += 1, i++) {
        v = ZERO;
        Bernstein(order_u, (double)u, bu);
        Bernstein(order_v, (double)v, bv);
        f.u = f.v = f.n = ZERO;
        weight = 0.0;
        n = 0;
        for (k = 0; k < order_v; k++){
          for (m = 0; m < order_u; m++, n++) {
            weight += (tmp = weights[n] * bv[k] * bu[m]);
            f.u += points[n].u * tmp;
            f.v += points[n].v * tmp;
            f.n += points[n].n * tmp;
          }
        }
        f.u /= weight;
        f.v /= weight;
        f.n /= weight;
        /*			Bernstein(order_u, (double)u, bu); */
        for (v = delta_v, j = 0; j < tessel_v; v += delta_v, j++) {
          Bernstein(order_v, (double)v, bv);
          s.u = s.v = s.n = ZERO;
          weight = 0.0;
          n = 0;
          for (k = 0; k < order_v; k++){
            for (m = 0; m < order_u; m++, n++) {
              weight += (tmp = weights[n] * bv[k] * bu[m]);
              s.u += points[n].u * tmp;
              s.v += points[n].v * tmp;
              s.n += points[n].n * tmp;
            }
          }
          s.u /= weight;
          s.v /= weight;
          s.n /= weight;
          lf.u = f.u, lf.v = f.v, lf.n = f.n;
          ls.u = s.u, ls.v = s.v, ls.n = s.n;
          if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                             &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
            double dn;
            
            EVVCtoDC(v_p, &lf, &x1, &y1);
            EVVCtoDC(v_p, &ls, &x2, &y2);
            dn = DELTA_N();
						if(EV_render_mode == FILLED_HIDDEN_RENDERING ||
							 EV_render_mode == CONST_SHADING_RENDERING){
							BHM(x1, y1, lf.n+dn, x2, y2, ls.n+dn);
						}
						else{
							XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
						}
          }
          f.u = s.u; f.v = s.v; f.n = s.n;
        }
      }
      for (v = 0, j = 0; j < 2; v += 1, j++) {
        u = ZERO;
        Bernstein(order_u, (double)u, bu);
        Bernstein(order_v, (double)v, bv);
        f.u = f.v = f.n = ZERO;
        weight = 0.0;
        n = 0;
        for (k = 0; k < order_v; k++){
          for (m = 0; m < order_u; m++, n++) {
            weight += (tmp = weights[n] * bv[k] * bu[m]);
            f.u += points[n].u * tmp;
            f.v += points[n].v * tmp;
            f.n += points[n].n * tmp;
          }
        }
        f.u /= weight;
        f.v /= weight;
        f.n /= weight;
        /*			Bernstein(order_v, (double)v, bv); */
        for (u = delta_u, i = 0; i < tessel_u; u += delta_u, i++) {
          Bernstein(order_u, (double)u, bu);
          s.u = s.v = s.n = ZERO;
          weight = 0.0;
          n = 0;
          for (k = 0; k < order_v; k++){
            for (m = 0; m < order_u; m++, n++) {
              weight += (tmp = weights[n] * bv[k] * bu[m]);
              s.u += points[n].u * tmp;
              s.v += points[n].v * tmp;
              s.n += points[n].n * tmp;
            }
          }
          s.u /= weight;
          s.v /= weight;
          s.n /= weight;
          lf.u = f.u, lf.v = f.v, lf.n = f.n;
          ls.u = s.u, ls.v = s.v, ls.n = s.n;
          if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                             &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {
            double dn;
            
            EVVCtoDC(v_p, &lf, &x1, &y1);
            EVVCtoDC(v_p, &ls, &x2, &y2);
            dn = DELTA_N();
						if(EV_render_mode == FILLED_HIDDEN_RENDERING ||
							 EV_render_mode == CONST_SHADING_RENDERING){
							BHM(x1, y1, lf.n+dn, x2, y2, ls.n+dn);
						}
						else{
							XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
						}
          }
          f.u = s.u; f.v = s.v; f.n = s.n;
        }
      }
    } /* drawing boundary edges */
  } else { /* Not filled */
    if (EV_render_mode == WIRE_RENDERING){
      tessel_u = (order_u + 1) / 2 * 2; delta_u = 1.0 / tessel_u;
      tessel_v = (order_v + 1) / 2 * 2; delta_v = 1.0 / tessel_v;
    }
    
    for (u = 0, i = 0; i < tessel_u+1; u += delta_u, i++) {
      v = ZERO;
      Bernstein(order_u, (double)u, bu);
      Bernstein(order_v, (double)v, bv);
      f.u = f.v = f.n = ZERO;
      weight = 0.0;
      n = 0;
      for (k = 0; k < order_v; k++){
        for (m = 0; m < order_u; m++, n++) {
          weight += (tmp = weights[n] * bv[k] * bu[m]);
          f.u += points[n].u * tmp;
          f.v += points[n].v * tmp;
          f.n += points[n].n * tmp;
        }
      }
      f.u /= weight;
      f.v /= weight;
      f.n /= weight;
      /*		Bernstein(order_u, (double)u, bu); */
      for (v = delta_v, j = 0; j < tessel_v; v += delta_v, j++) {
        Bernstein(order_v, (double)v, bv);
        s.u = s.v = s.n = ZERO;
        weight = 0.0;
        n = 0;
        for (k = 0; k < order_v; k++){
          for (m = 0; m < order_u; m++, n++) {
            weight += (tmp = weights[n] * bv[k] * bu[m]);
            s.u += points[n].u * tmp;
            s.v += points[n].v * tmp;
            s.n += points[n].n * tmp;
          }
        }
        s.u /= weight;
        s.v /= weight;
        s.n /= weight;
        lf.u = f.u, lf.v = f.v, lf.n = f.n;
        ls.u = s.u, ls.v = s.v, ls.n = s.n;
        if (test_mclip) {
          COPY_VCREC(&pp1.loc.vc, &lf);
          COPY_VCREC(&pp2.loc.vc, &ls);
          to_draw = (clip_line_by_a_plane(NO, &pp1, &pp2,
                                          (WCRec *)&model_clip_plane_center,
                                          (WCRec *)&model_clip_plane_normal,
                                          &p1_return, &p2_return) != 0);
          if (to_draw) {
            COPY_VCREC(&lf, &p1_return.loc.vc);
            COPY_VCREC(&ls, &p2_return.loc.vc);
          }
        }
        if (to_draw && ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                                      &lf.u, &lf.v, &lf.n,
                                      &ls.u, &ls.v, &ls.n)) {
          EVVCtoDC(v_p, &lf, &x1, &y1);
          EVVCtoDC(v_p, &ls, &x2, &y2);
          if (EV_render_mode == NORMAL_RENDERING ||
              EV_render_mode == WIRE_RENDERING) {
            XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
          } else {
            BHM(x1, y1, lf.n, x2, y2, ls.n);
          }
        }
        f.u = s.u; f.v = s.v; f.n = s.n;
      }
    }
    for (v = 0, j = 0; j < tessel_v+1; v += delta_v, j++) {
      u = ZERO;
      Bernstein(order_u, (double)u, bu);
      Bernstein(order_v, (double)v, bv);
      f.u = f.v = f.n = ZERO;
      weight = 0.0;
      n = 0;
      for (k = 0; k < order_v; k++){
        for (m = 0; m < order_u; m++, n++) {
          weight += (tmp = weights[n] * bv[k] * bu[m]);
          f.u += points[n].u * tmp;
          f.v += points[n].v * tmp;
          f.n += points[n].n * tmp;
        }
      }
      f.u /= weight;
      f.v /= weight;
      f.n /= weight;
      /*		Bernstein(order_v, (double)v, bv); */
      for (u = delta_u, i = 0; i < tessel_u; u += delta_u, i++) {
        Bernstein(order_u, (double)u, bu);
        s.u = s.v = s.n = ZERO;
        weight = 0.0;
        n = 0;
        for (k = 0; k < order_v; k++){
          for (m = 0; m < order_u; m++, n++) {
            weight += (tmp = weights[n] * bv[k] * bu[m]);
            s.u += points[n].u * tmp;
            s.v += points[n].v * tmp;
            s.n += points[n].n * tmp;
          }
        }
        s.u /= weight;
        s.v /= weight;
        s.n /= weight;
        lf.u = f.u, lf.v = f.v, lf.n = f.n;
        ls.u = s.u, ls.v = s.v, ls.n = s.n;
        if (test_mclip) {
          COPY_VCREC(&pp1.loc.vc, &lf);
          COPY_VCREC(&pp2.loc.vc, &ls);
          to_draw = (clip_line_by_a_plane(NO, &pp1, &pp2,
                                          (WCRec *)&model_clip_plane_center,
                                          (WCRec *)&model_clip_plane_normal,
                                          &p1_return, &p2_return) != 0);
          if (to_draw) {
            COPY_VCREC(&lf, &p1_return.loc.vc);
            COPY_VCREC(&ls, &p2_return.loc.vc);
          }
        }
        if (to_draw && ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,
                                      &lf.u, &lf.v, &lf.n,
                                      &ls.u, &ls.v, &ls.n)) {
          EVVCtoDC(v_p, &lf, &x1, &y1);
          EVVCtoDC(v_p, &ls, &x2, &y2);
          if (EV_render_mode == NORMAL_RENDERING ||
              EV_render_mode == WIRE_RENDERING) {
            XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);
          } else {
            BHM(x1, y1, lf.n, x2, y2, ls.n);
          }
        }
        f.u = s.u; f.v = s.v; f.n = s.n;
      }
    }
  }
  if (show_poly) {
#define DRAW_POLY_LINE(from, to)                                           \
    {                                                                      \
      lf.u = from.u, lf.v = from.v, lf.n = from.n;                         \
      ls.u = to.u, ls.v = to.v, ls.n = to.n;                               \
      if (ClipLineToView(vboxll.u, vboxll.v, vboxur.u, vboxur.v,           \
                         &lf.u, &lf.v, &lf.n, &ls.u, &ls.v, &ls.n)) {      \
        EVVCtoDC(v_p, &lf, &x1, &y1);                                      \
        EVVCtoDC(v_p, &ls, &x2, &y2);                                      \
        if (EV_render_mode == NORMAL_RENDERING ||                          \
            EV_render_mode == WIRE_RENDERING) {                            \
          XDrawLine(EV_display, EV_curr_win, EV_curr_GC, x1, y1, x2, y2);  \
        } else {                                                           \
          BHM(x1, y1, lf.n, x2, y2, ls.n);                                 \
        }                                                                  \
      }                                                                    \
    }
    
    if (draw_mode == DRAW_MODE_DRAW)
      EV_curr_GC    = v_p->constrCopyGC, EV_curr_pixel = v_p->foreground;
    else if (draw_mode == DRAW_MODE_XORDRAW)
      EV_curr_GC    = v_p->defaultXORGC, EV_curr_pixel = v_p->foreground;
    else
      EV_curr_GC    = v_p->constrEraseGC, EV_curr_pixel = v_p->background;
    
    if (draw_mode == DRAW_MODE_DRAW || draw_mode == DRAW_MODE_ERASE ||
        draw_mode == DRAW_MODE_XORDRAW) {
      
      for(i = 0; i < order_v; i++){
        for(j = 0; j < order_u; j++){
          if(i < order_v - 1){
            DRAW_POLY_LINE(points[i * order_u + j], points[(i + 1) * order_u + j]);
          }
          if(j < order_u - 1){
            DRAW_POLY_LINE(points[i * order_u + j], points[i * order_u + j + 1]);
          }
        }
      }
    }  
  }
  free(bu);
  free(bv);
  free(points);
}








