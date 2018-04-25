/*

              ####### #         ###   #     #   ###   ######
              #       #          #     #   #     #    #     #
              #       #          #      # #      #    #     #
              #####   #          #       #       #    ######
              #       #          #      # #      #    #   #
              #       #          #     #   #     #    #    #
              ####### #######   ###   #     #   ###   #     #

                   
                  Copyright: 1994 Petr Krysl

   Czech Technical University in Prague, Faculty of Civil Engineering,
      Dept. Structural Mechanics, 166 29 Prague, Czech Republic,
                  email: pk@power2.fsv.cvut.cz
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/


#ifndef EPVIEWER_H
#define EPVIEWER_H

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_LAYER 40


typedef struct LayerRec {
  BOOLEAN                layer_on; /* YES or NO */
}                                           LayerRec;

struct EView;

typedef void (*EventHandlerP)(Widget w, struct EView *v_p, XEvent *event);
typedef void (*StartProcP)(struct EView *v_p, caddr_t data);
typedef void (*SuspendProcP)(struct EView *v_p, caddr_t data);
typedef void (*ResumeProcP)(struct EView *v_p, caddr_t data);
typedef void (*RemoveProcP)(struct EView *v_p, caddr_t data);

#ifdef AIXV3
struct EModel;
#endif

typedef struct EHandlerRec {
  EventHandlerP         handler;
  StartProcP            start_proc;
  caddr_t               start_proc_data;
  SuspendProcP          suspend_proc;
  caddr_t               suspend_proc_data;
  ResumeProcP           resume_proc;
  caddr_t               resume_proc_data;
  RemoveProcP           remove_proc;
  caddr_t               remove_proc_data;
}                              EHandlerRec;

#define CLEAR_HANDLER_DATA(v_p, the_handler) \
  v_p->the_handler.handler           = NULL; \
  v_p->the_handler.start_proc        = NULL; \
  v_p->the_handler.start_proc_data   = NULL; \
  v_p->the_handler.suspend_proc      = NULL; \
  v_p->the_handler.suspend_proc_data = NULL; \
  v_p->the_handler.resume_proc       = NULL; \
  v_p->the_handler.resume_proc_data  = NULL; \
  v_p->the_handler.remove_proc       = NULL; \
  v_p->the_handler.remove_proc_data  = NULL

#define COPY_HANDLER_DATA(v_p,src,dest) \
  v_p->dest.handler           = v_p->src.handler; \
  v_p->dest.start_proc        = v_p->src.start_proc; \
  v_p->dest.start_proc_data   = v_p->src.start_proc_data; \
  v_p->dest.suspend_proc      = v_p->src.suspend_proc; \
  v_p->dest.suspend_proc_data = v_p->src.suspend_proc_data; \
  v_p->dest.resume_proc       = v_p->src.resume_proc; \
  v_p->dest.resume_proc_data  = v_p->src.resume_proc_data; \
  v_p->dest.remove_proc       = v_p->src.remove_proc; \
  v_p->dest.remove_proc_data  = v_p->src.remove_proc_data
    
typedef struct EModelClipPlane {
  BOOLEAN           clip_is_on;
  WCRec             center;
  WCRec             normal;
}               EModelClipPlane;


typedef struct EView {
  Widget                   view_widget; /* to be drawn into */
  Widget                   top_view_widget; /* the widget to be sent destroy */
                                            /* message */
  Drawable                 draw_into;
  EPixel                   foreground;
  EPixel                   background;
  struct EModel           *model_p;
  EHandlerRec              the_active_handler;
  EHandlerRec              the_suspended_handler;
  GC                       defaultCopyGC;  /* must not be modified */
  GC                       defaultXORGC;   /* must not be modified */
  GC                       defaultEraseGC; /* must not be modified */
  GC                       hiliteGC;       /* must not be modified */
  GC                       erasehiliteGC;  /* must not be modified */
  GC                       eraseGC;        /* this GC may be modified */
  GC                       writableGC;     /* this GC may be modified */
  GC                       gridGC;         /* must not be modified */
  GC                       constrCopyGC;   /* must not be modified */
  GC                       constrEraseGC;  /* must not be modified */
  GC                       constrXORGC;    /* must not be modified */
  WCRec                    VCOrigin;       /* in WC */
  WCRec                    VectUp;	   /* in WC */
  WCRec                    Normal;	   /* in WC */ 
  WCRec                    VectorU;        /* in WC */
  WCRec                    VectorV;        /* in WC */
  WCRec                    ViewRefPoint;   /* in WC */
  VCRec                    ViewRefPointVC; /* in VC */
  VCRec                    view_dims_VC;   /* view dimensions in VC */
  DCRec                    view_dims_DC;   /* actual dimensions in DC */
  BOOLEAN                  preserve_vup;   /* should preserve VUP during mouse-driven rotation*/
  WCRec                    VectUpAtStart;	   /* in WC */
  BOOLEAN                  show_axes; /* Display coordinate axes in the */
                                      /* upper-left corner and at the center */
                                      /* of the scene */
	BOOLEAN                  show_scale; /* Display color scale in the upper-right */
	                                     /* corner of the scene */ 
	BOOLEAN                  show_status; /* Display status at the bottom of the scene */ 
  BOOLEAN                  grid_on;
  VC2DRec                  grid_origin;
  VC2DRec                  grid_deltas;
  BOOLEAN                  grid_lock_on;
  int                      elem_snap_lock_type; /* KEYPOINT | PROJECTION */
  BOOLEAN                  elem_snap_lock_on;
  BOOLEAN                  constrplane_lock_on;
  WCRec                    constrplane_center;
  WCRec                    constrplane_normal;
  WCRec                    constrplane_u;
  WCRec                    constrplane_v;
  LayerRec                 view_layers[MAX_LAYER]; /* layers */
  LIST                     visible_GO;
  LIST                     invisible_GO;
  EFringeTable             fringe_table;
  ERenderingType           render_mode;
	ERenderingType           backup_render_mode;
  EShadingType             shade_mode;
  VCRec                    dir_towards_light;
  Cursor                   cursor;
  BOOLEAN                  should_clip_by_fg_bg_view_planes;
  double                   fg_view_plane_n;
  double                   bg_view_plane_n;
  BOOLEAN                  should_clip_by_model_planes;
  EModelClipPlane          model_clip_plane;
  BOOLEAN                  was_destroyed; /* for debugging */
}                                           EView;


typedef void (*ApplyFuncP)(EView *v_p, caddr_t data, WCRec *point);
typedef void (*SelectActionFuncP)(EView *v_p, caddr_t data_p);
typedef void (*ResetFuncP)(EView *v_p, caddr_t data_p);


#define DEFAULT_LINE_WIDTH (unsigned int)0
#define GRID_LINE_WIDTH    (unsigned int)0
#define GRID_MARK_LENGTH_HALF 6
#define HILITE_LINE_WIDTH  (unsigned int)2

typedef enum {   NO_FAST_VIEWING_ACTION_RUNNING,
		 PANNING,
		 WINDOWING,
		 ZOOMING,
		 ROTATING,
                 STARTING_ROTATION
	     }                FVH_ACTIONS;


#define GET_ROT_ANGLE() (fabs(rot_angle) > 0? rot_angle: PI/72)


#include <X11/keysym.h>

#define TOP_SYMB(s)        (s == XK_t || s == XK_T)
#define BOTTOM_SYMB(s)     (s == XK_o || s == XK_O)
#define LEFT_SYMB(s)       (s == XK_l || s == XK_L)
#define RIGHT_SYMB(s)      (s == XK_r || s == XK_R)
#define FRONT_SYMB(s)      (s == XK_f || s == XK_F)
#define BACK_SYMB(s)       (s == XK_b || s == XK_B)
#define ISO_SYMB(s)        (s == XK_i || s == XK_I)

#define PLUS_SYMB(s)       (s == XK_plus)
#define MINUS_SYMB(s)      (s == XK_minus)
#define LEFTANGLE_SYMB(s)  (s == XK_less)
#define RIGHTANGLE_SYMB(s) (s == XK_greater)
#define ROTATE_SYMB(s)     (s == XK_r || s == XK_R)

#ifdef __cplusplus
}
#endif

#endif
