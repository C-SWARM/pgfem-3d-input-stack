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

#include "Econfig.h"
#include "Eview.h"
#include "Eactset.h"
#include "Emodel.h"
#include "Elixir.h"
#include "Ereprts.h"
#include "Esimple.h"
#include <X11/cursorfont.h>

/* ======================================================================== */
/* PRIVATE   */
/* ======================================================================== */

/* one-time initialization done one for all views */

static BOOLEAN started = NO;
static XContext save_context_id = (XContext)0;
static Atom delete_widget_atom;

static void EVOneTimeInit(Widget wgt);
static void EVSaveViewP(Widget w, EView *v_p);

/* callbacks */

static void EVResizeEH(Widget w, XtPointer v_p, XEvent *event,
		       Boolean *cont_to_dispatch); 
static void EVExposeEH(Widget w, XtPointer v_p, XEvent *event,
		       Boolean *cont_to_dispatch); 
static void EVInputEH(Widget w, XtPointer v_p, XEvent *event,
		      Boolean *cont_to_dispatch);
static void EVNonMaskableEH(Widget w, XtPointer v_p, XEvent *event,
			    Boolean *cont_to_dispatch);
BOOLEAN EscapePressed(Display *d);
void  recompute_dc_size(EView *v_p);
void 
EVFastRedraw(EView *v_p);

/* fast-viewing handler */

void EVFastViewHMotion(Widget w, EView *v_p, XMotionEvent *event);
void EVFastViewHButtonPress(Widget w, EView *v_p, XButtonEvent *event);
void EVFastViewHButtonRelease(Widget w, EView *v_p, XButtonEvent *event);
void EVFastViewHKey(Widget w, EView *v_p, XKeyEvent *event);
static void EVSetPointerIn(BOOLEAN flag);
void EVHandleKeyPress(EView *v_p, XKeyEvent *event,
		      KeySym ksym, char *string_val);
void EVHandleKeyRelease(EView *v_p, XKeyEvent *event,
			KeySym ksym, char *string_val);
void EVFastRedraw(EView *v_p);

/* selection handlers */

void EVSelectGraphicsBPressHandler(Widget w, EView *v_p,
			      XButtonEvent *event);
void EVSelectGraphicsMNotifyHandler(Widget w, EView *v_p,
			      XMotionEvent *event);
static void InitFixedSizeAperture(EView *v_p, WCRec *p);
static void ModifyFixedSizeAperture(EView *v_p, WCRec *p);
static void DestroyFixedSizeAperture(EView *v_p);
static BOOLEAN EVPreprocessInput(EView *v_p, XEvent *e);
/* static void EVStartViewSelection(void); */
/* static EView *EVNextSelectedView(void); */
int EVDispatchHitTest(EView *v_p, GraphicObj *g_p);
static int EVHitListNumberOfHits(void);
static int EVMarkUnmarkGraphics(EView *v_p, GraphicObj *g_p);
static void EVUnmarkGraphics(EView *v_p, GraphicObj *g_p);
static void EVMarkGraphics(EView *v_p, GraphicObj *g_p);
static GraphicObj *EVGetFromHitList(void);
static GraphicObj *EVHitGraphics(void);
static void EVUnhiliteSelectionList(EView *v_p);
BOOLEAN
PassAttributeCriteria(GraphicObj *g_p);

/* display-list functions */

void AllocBackBuffer(EView *v_p);
void EVRedrawDisplayList(EView *v_p);
BOOLEAN EVBBoxCutsView(EView *v_p, VCRec *ll, VCRec *ur);
int EVDispatchShow(EView *v_p, GraphicObj *g_p);
static void EVAdjustToLocks(EView *v_p, WCRec *p);
static void 
AdjustToGridLock(EView *v_p, WCRec *p);
void 
AdjustToConstrPlaneLock(EView *v_p, WCRec *p);
void 
AdjustToConstrPlaneGridLock(EView *v_p, WCRec *p);
void EMHiliteGraphics(EModel *m_p, GraphicObj *g_p);
void EMUnhiliteGraphics(EModel *m_p, GraphicObj *g_p);
void EMUninstallHandler(EModel *m_p );

/* selections */

static void EVInitSelection(EView *v_p, WCRec *p);
static void ClearSelectionList(EView *v_p);
static GraphicObj *EVHitGraphics(void);
static GraphicObj *EVGetFromHitList(void);
static int EVMarkUnmarkGraphics(EView *v_p, GraphicObj *g_p);
static int EVHitListNumberOfHits(void);
static void ClearHitList(EView *);
static void EVInitHitList(EView *v_p, WCRec *hit);
static void EVTestHitOnVisibleGraphics(EView *v_p, LIST hit_list);

/* Snap point */

static BOOLEAN EVGetSnapPoint(WCRec *p);
static void EVSetSnapPoint(WCRec *kp);
static void UndrawSnapPoint(EView *v_p);
/* static void RedrawSnapPoint(EView *v_p); */
static void DrawSnapPoint(EView *v_p, WCRec *p);
static void InvalidateSnapPoint(EView *v_p);
static void EVProcessSnapPoint(EView *v_p, XButtonEvent *event);

/* Key points */

static BOOLEAN NextFromKeyPointList(EView *v_p, WCRec *kp);
static int DispatchKeyPList(EView *v_p, GraphicObj *g_p);
static int CompareKeypointDistances(KeyPointRec *p1, KeyPointRec *p2);
static void EliminateDuplicateKeyPoints(void);
static void MakeNewKeyPointList(EView *v_p, WCRec *kp);

static void RedrawTP(EView *v_p, WCRec *p);

void EVRotateByMouse(EView *v_p, FPNum aboutu, FPNum aboutv);
void EVRecolorCursor(EView *v_p);

/* ======================================================================== */
/* PRIVATE DEFS */
/* ======================================================================== */

#define LEFT_BUTTON   Button1
#define MIDDLE_BUTTON Button2
#define RIGHT_BUTTON  Button3

#define ZOOM_COEFF 0.5

#define PROCESSED     YES	/* used by EVPreprocessInput */

#define NOPE      0
#define MARKED    1
#define UNMARKED -1

#define DEFAULT_APERTURE_SIZE_RATIO  0.02

#define SMALL_NUMBER 1.0e-12
#define APPROX_EQUAL(n1,n2) fabs((n2)-(n1))<SMALL_NUMBER

#define ROT_MAGNIFACTOR (4)

#define SNAP_BOX_DIM_RATIO 0.1
 
typedef enum {   NO_SELECTION_PROCESS_RUNNING,
		 FIXED_SIZE_APERTURE_SELECTION,
		 FENCING_SELECTION,
		 DEFINING_FENCE
	       } SEL_ACTIONS;

typedef struct _FVH_DATA_REC {
  FVH_ACTIONS           action;
  WCRec                 last_point;
  DCRec                 last_point_DC;
  struct { Dimension x;
	   Dimension y; }    DC_dims;
  WCRec                 origin_at_start;
  DCRec                 origin_at_start_DC;
  ERectVCRec            *rubber_band_echo;
} FVH_DATA_REC;

typedef struct _SEL_DATA_REC {
  SEL_ACTIONS               action;
  WCRec                     last_point;
  DCRec                     last_point_DC;
  struct { Dimension x;
	   Dimension y; }   DC_dims;
  WCRec                     origin_at_start;
  FPNum                     aperture_size_ratio;
  FPNum                     aperture_size;
  EBoxVC2DRec               aperture;
  ERectVCRec                *rubber_band_echo;
  unsigned long             attrib_val_mask;
  BOOLEAN                   take_as_NOT;
  unsigned long             pickable_entity_mask;
}   SEL_DATA_REC;

/* ======================================================================== */
/* PRIVATE DATA */
/* ======================================================================== */

FVH_DATA_REC fv_data;

static struct {
  BOOLEAN        point_is_valid;
  WCRec          coords;
}   sent_point;

static struct {
  BOOLEAN        point_is_valid;
  WCRec          coords;
}   snap_point;

static struct {
  ApplyFuncP    func_p;
  caddr_t       data_p;
  ResetFuncP    reset_func_p;
  BOOLEAN       prevent_redisplay;
  int           prompt_class;
  int           prompt_serial;
  char         *prompt;
}  apply_func = {NULL, NULL, NULL, NO, 0, 0, NULL};

static struct {
  SelectActionFuncP     func_p;
  caddr_t               data_p;
  ResetFuncP            reset_func_p;
}  select_action_func;

static LIST hit_list = NULL;
static LIST selection_list = NULL;
static BOOLEAN selection_started = NO;
static GraphicObj *current_from_hit_list;
static ESelectCriteria select_criterion = INTERSECT;

static SEL_DATA_REC sel_data;

static BOOLEAN pointer_in = NO; /* is pointer in any view or not? */

extern BOOLEAN force_wire_display;

extern FPNum rot_angle;

/* ========================================================================= */

static LIST key_p_list = NULL;
static WCRec prev_hit_at = {0, 0, 0};

#ifdef NEED_PRINT_SELECTION
void print_selection()
{
  GraphicObj *g_p;

  g_p = (GraphicObj *)get_list_next(selection_list, NULL);
  while (g_p != NULL) {
    printf("g_id %d\n",EGGraphicSerialID(g_p));
    g_p = (GraphicObj *)get_list_next(selection_list, g_p);
  }
}
void print_hit_list()
{
  GraphicObj *g_p;

  g_p = (GraphicObj *)get_list_next(hit_list, NULL);
  while (g_p != NULL) {
    printf("g_id %d\n",EGGraphicSerialID(g_p));
    g_p = (GraphicObj *)get_list_next(hit_list, g_p);
  }
}
#endif

static void 
EVUnhiliteSelectionList(EView *v_p)
{
  GraphicObj *g_p;
  Display *d;
  BOOLEAN redraw;

  d      = XtDisplay(EVViewPToWidget(v_p));
  redraw = YES;
  g_p    = (GraphicObj *)get_list_next(selection_list, NULL);
  while (g_p != NULL) {
    if (redraw) {
      redraw = !EscapePressed(d);
      EMUnhiliteGraphics(EVGetModel(v_p), g_p); 
    } else
      EGSetSelected(g_p, NO);
    g_p = (GraphicObj *)get_list_next(selection_list, g_p);
  }
}

static BOOLEAN 
NextFromKeyPointList(EView *v_p, WCRec *kp)
{
  KeyPointRec *p;

  p = (KeyPointRec *)get_from_head(key_p_list);
  if (p == NULL)
    return NO;
  else {
    kp->x = p->coords.x;
    kp->y = p->coords.y;
    kp->z = p->coords.z;
    UndrawSnapPoint(v_p);
    return YES;
  }
}

static int 
DispatchKeyPList(EView *v_p, GraphicObj *g_p)
{
  LIST temp_list;
  KeyPointRec *p;
  VCRec prev_hit_vc, pvc;

  EVWCtoVC(v_p, &prev_hit_at, &prev_hit_vc);
  temp_list = EGGraphicKeyPointList(g_p);
  if (temp_list != NULL) {
    p = (KeyPointRec *)get_list_next(temp_list, NULL);
    while(p != NULL) {
      add_to_tail(key_p_list, p);
      EVWCtoVC(v_p, &(p->coords), &pvc);
      p->dist_from_hit =
	DistOfPnts(prev_hit_vc.u, prev_hit_vc.v, pvc.u, pvc.v);
      p = (KeyPointRec *)get_list_next(temp_list, p);
    }
    flush_list(temp_list, NOT_NODES);
  }
  return 1;
}

static int 
CompareKeypointDistances(KeyPointRec *p1, KeyPointRec *p2)
{
  if (p1->dist_from_hit < p2->dist_from_hit)
    return TRUE;
  else
    return FALSE;
}

static void 
EVSetPointerIn(BOOLEAN flag)
{
  pointer_in = flag;
}

static void 
EVSaveViewP(Widget w, EView *v_p)
{
  if (XSaveContext(XtDisplay(w), XtWindow(w),
		    save_context_id, (caddr_t)v_p) != 0) {
    fprintf(stderr, "Failed to save context in EVSaveViewP\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
}

static void 
EVAdjustToLocks(EView *v_p, WCRec *p)
{
  if (v_p->constrplane_lock_on) {
    /* Should lock to the construction plane? */
    AdjustToConstrPlaneLock(v_p, p);
    if (v_p->grid_lock_on)
      /* Should lock to the grid in the construction plane? */
      AdjustToConstrPlaneGridLock(v_p, p);
  } else if (v_p->grid_lock_on) {
    /* Should lock to the grid in the view plane? */
    AdjustToGridLock(v_p, p);
  }
}


static void 
AdjustToGridLock(EView *v_p, WCRec *p)
{
  FPNum floor_coord;
  FPNum dx, dy, ox, oy;
  VCRec vcp;
  
  ox = v_p->grid_origin.u;
  oy = v_p->grid_origin.v;
  dx = v_p->grid_deltas.u;
  dy = v_p->grid_deltas.v;
  
  EVWCtoVC(v_p, p, &vcp);
  
  floor_coord = (FPNum)floor((double)((vcp.u - ox)/dx)) * dx + ox;
  if ((vcp.u - 0.5 * dx) < floor_coord)
    vcp.u = floor_coord;
  else
    vcp.u = floor_coord + dx;
  floor_coord = (FPNum)floor((double)((vcp.v - oy)/dy)) * dy + oy;
  if ((vcp.v - 0.5 * dy) < floor_coord)
    vcp.v = floor_coord;
  else
    vcp.v = floor_coord + dy;
  vcp.n = 0;
  EVVCtoWC(v_p, &vcp, p);
}



static void 
EliminateDuplicateKeyPoints()
{
  KeyPointRec *p /*, *pp */;
  FPNum prev_dist, dist;
  
  p = (KeyPointRec *)get_list_next(key_p_list, NULL);
  prev_dist = 0.0;
  while(p != NULL) {
    dist = p->dist_from_hit;
    if (APPROX_EQUAL(dist,prev_dist)) {
      p  = (KeyPointRec *)get_from_current_prev(key_p_list);
			free_node(p);
      p = (KeyPointRec *)get_list_current(key_p_list);
    }
    prev_dist = dist;
    p = (KeyPointRec *)get_list_next(key_p_list, p);
  }
}

static void 
MakeNewKeyPointList(EView *v_p, WCRec *kp)
{
  VCRec vc;

  if (key_p_list == NULL)
    key_p_list = make_list();
  else
    flush_list(key_p_list, AND_NODES);
  
  if (v_p->visible_GO != NULL) {
    GraphicObj *g_p;
    LIST  a_list = v_p->visible_GO;
    EBoxVC2DRec box;
    FPNum view_dim_x, view_dim_y, dim;
    
    EVGetDimsVC(v_p, &view_dim_x, &view_dim_y);
    dim        = min(view_dim_x, view_dim_y);
    dim        = 0.5 * SNAP_BOX_DIM_RATIO * dim;
    EVWCtoVC(v_p, &prev_hit_at, &vc);
    box.left   = vc.u - dim;
    box.right  = vc.u + dim;
    box.bottom = vc.v - dim;
    box.top    = vc.v + dim;
    g_p = (GraphicObj *)get_list_next(a_list, NULL);
    while (g_p != NULL) {
      if (EGGraphicHitByBox(v_p, &box, OVERLAP, g_p) == YES)
	DispatchKeyPList(v_p, g_p);
      g_p = (GraphicObj *)get_list_next(a_list, g_p);
    }
  }
  sort_list_func(key_p_list, CompareKeypointDistances);
  EliminateDuplicateKeyPoints();
  NextFromKeyPointList(v_p, kp);
}

static void 
EVProcessSnapPoint(EView *v_p, XButtonEvent *event)
{
  WCRec hit_at;
  WCRec kp;

  EVDCtoWC(v_p, event->x, event->y, &hit_at);
  if (hit_at.x == prev_hit_at.x &&
      hit_at.y == prev_hit_at.y &&
      hit_at.z == prev_hit_at.z) { 
    if (NextFromKeyPointList(v_p, &kp) == NO) 
      MakeNewKeyPointList(v_p, &kp);
  } else { 
    prev_hit_at.x = hit_at.x;
    prev_hit_at.y = hit_at.y;
    prev_hit_at.z = hit_at.z;
    MakeNewKeyPointList(v_p, &kp);
  }
  EVSetSnapPoint(&kp);
  EVSetLastInputPoint(&kp);
  DrawSnapPoint(v_p, &kp);
}

/* ========================================================================= */

static BOOLEAN snap_point_drawn = NO;
static WCRec prev_TP = {0, 0, 0};



static void 
RedrawTP(EView *v_p, WCRec *p)
{
  int DCx, DCy;
  unsigned int width, height;
  long cross_dim;
  
  EVGetDimsDC(v_p, &width, &height);
  cross_dim = (width+height)/20.;
  cross_dim = min(cross_dim, 40);
  EVWCtoDC(v_p, p, &DCx, &DCy);
  XDrawLine(XtDisplay(v_p->view_widget), XtWindow(v_p->view_widget), 
	    v_p->defaultXORGC,
	    (DCx - cross_dim/2), DCy, (DCx + cross_dim/2), DCy);
  XDrawLine(XtDisplay(v_p->view_widget), XtWindow(v_p->view_widget), 
	    v_p->defaultXORGC,
	    DCx, (DCy - cross_dim/2), DCx, (DCy + cross_dim/2));
}


static void 
DrawSnapPoint(EView *v_p, WCRec *p)
{
  if (snap_point.point_is_valid) {
    if (!snap_point_drawn) {
      prev_TP.x = p->x; prev_TP.y = p->y; prev_TP.z = p->z;
      RedrawTP(v_p, p); /* draw new */
      snap_point_drawn = YES;
    }
  }
}

static void 
UndrawSnapPoint(EView *v_p)
{
  if (snap_point_drawn) {
    RedrawTP(v_p, &prev_TP);
    snap_point_drawn = NO;
  }
}



static BOOLEAN 
EVPreprocessInput(EView *v_p, XEvent *e)
{
  switch(e->type) {
  case ButtonPress:
    if((e->xbutton.button == LEFT_BUTTON) &&
       (e->xbutton.state & ShiftMask) &&
       !(e->xbutton.state & ControlMask)) {
      EVProcessSnapPoint(v_p, (XButtonEvent *)e); /* find snap point */
      return PROCESSED;
    } else if ((e->xbutton.button == RIGHT_BUTTON) &&
	       (e->xbutton.state & ControlMask)) {
      EVSuspendActiveHandler(v_p); /* use fast-viewing */
      return PROCESSED;
    } else if ((e->xbutton.button == RIGHT_BUTTON) &&
	       (e->xbutton.state & ShiftMask)) {
      InvalidateSnapPoint(v_p); /* cancel snap point */
      return PROCESSED;
    }
    break;
  case LeaveNotify:
      /*InvalidateSnapPoint(v_p); Does this have a sense? */ 
    EVSetPointerIn(NO);
    return !PROCESSED; /* we still want to process the event outside */
    break;
  case EnterNotify:
    /* XSetInputFocus: to force the view window to be activated when the */
    /* pointer enters it (this is to facilitate constructions, in fact */
    /* to avoid the user having to explicitly raise the window by clicking */
    /* on its border or title bar */
    XSetInputFocus(XtDisplay(EVViewPToWidget(v_p)),
                   XtWindow(EVViewPToWidget(v_p)), RevertToPointerRoot,
                   CurrentTime);
    /* Notify internals that the pointer is in the view */
    EVSetPointerIn(YES);
    return !PROCESSED; /* we still want to process the event outside */
    break;
  }
  return !PROCESSED; /* this is default; otherwise return */
			/* from within the code above */
}



static void 
EVOneTimeInit(Widget wgt)
{
  if (started == NO) {
    
    save_context_id = XUniqueContext();

    delete_widget_atom =
      XInternAtom(XtDisplay(wgt), "WM_DELETE_WINDOW", False);

    started = YES;
  }
}


BOOLEAN 
EVBBoxCutsView(EView *v_p, VCRec *ll, VCRec *ur)
{
  if (ll->u > v_p->ViewRefPointVC.u + v_p->view_dims_VC.u * 0.5)
    return NO;
  if (ll->v > v_p->ViewRefPointVC.v + v_p->view_dims_VC.v * 0.5)
    return NO;
  if (ur->u < v_p->ViewRefPointVC.u - v_p->view_dims_VC.u * 0.5)
    return NO;
  if (ur->v < v_p->ViewRefPointVC.v - v_p->view_dims_VC.v * 0.5)
    return NO;
  return YES;
}

static void 
InvalidateSnapPoint(EView *v_p)
{
  UndrawSnapPoint(v_p);
  snap_point.point_is_valid = NO;
}



static BOOLEAN 
EVGetSnapPoint(WCRec *p)
{
  if (snap_point.point_is_valid) {
    p->x = snap_point.coords.x;
    p->y = snap_point.coords.y;
    p->z = snap_point.coords.z;
    snap_point.point_is_valid = NO; /* invalidate this point */
    return YES;
  } else {
    p->x = p->y = p->z = 0;
    return NO;
  }
}




static void 
EVSetSnapPoint(WCRec *kp)
{
  char message[132];
  
  snap_point.coords.x       = kp->x;
  snap_point.coords.y       = kp->y;
  snap_point.coords.z       = kp->z;
  sprintf (message, "Keypoint at [%f;%f;%f]", kp->x, kp->y, kp->z);
  ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 2, message);
  snap_point.point_is_valid = YES;
}



static void 
InitFixedSizeAperture(EView *v_p, WCRec *p)
{
  FPNum dx, dy;
  VC2DRec p1, p2;
  VCRec vcp;
  
  EVGetDimsVC(v_p, &dx, &dy);
  sel_data.aperture_size_ratio = DEFAULT_APERTURE_SIZE_RATIO;
  sel_data.aperture_size = min(dx, dy) * sel_data.aperture_size_ratio;
  sel_data.origin_at_start.x = sel_data.last_point.x = p->x;
  sel_data.origin_at_start.y = sel_data.last_point.y = p->y;
  sel_data.origin_at_start.z = sel_data.last_point.z = p->z;
  EVWCtoVC(v_p, p, &vcp);
  sel_data.aperture.left   = vcp.u - 0.5 * sel_data.aperture_size;
  sel_data.aperture.bottom = vcp.v - 0.5 * sel_data.aperture_size;
  sel_data.aperture.right  = vcp.u + 0.5 * sel_data.aperture_size;
  sel_data.aperture.top    = vcp.v + 0.5 * sel_data.aperture_size;
  p1.u = sel_data.aperture.left;  p1.v = sel_data.aperture.bottom;
  p2.u = sel_data.aperture.right; p2.v = sel_data.aperture.top;
  XorDrawRectVC(v_p, sel_data.rubber_band_echo = CreateRectVC(&p1, &p2));
}



static void 
ModifyFixedSizeAperture(EView *v_p, WCRec *p)
{
  VC2DRec p1, p2;
  VCRec vp;
  
  sel_data.last_point.x = p->x;
  sel_data.last_point.y = p->y;
  sel_data.last_point.z = p->z;
  XorDrawRectVC(v_p, sel_data.rubber_band_echo);
  EVWCtoVC(v_p, p, &vp);
  sel_data.aperture.left   = vp.u - 0.5 * sel_data.aperture_size;
  sel_data.aperture.bottom = vp.v - 0.5 * sel_data.aperture_size;
  sel_data.aperture.right  = vp.u + 0.5 * sel_data.aperture_size;
  sel_data.aperture.top    = vp.v + 0.5 * sel_data.aperture_size;
  p1.u = sel_data.aperture.left; p1.v = sel_data.aperture.bottom; 
  p2.u = sel_data.aperture.right; p2.v = sel_data.aperture.top; 
  XorDrawRectVC(v_p, ModifyRectVC(sel_data.rubber_band_echo, &p1, &p2));
}



static void 
DestroyFixedSizeAperture(EView *v_p)
{
  if (sel_data.rubber_band_echo != NULL) {
    XorDrawRectVC(v_p, sel_data.rubber_band_echo);
    DestroyRectVC(sel_data.rubber_band_echo);
  }
  sel_data.rubber_band_echo = NULL;
}



static void 
ClearHitList(EView *v_p)
{
  if (hit_list == NULL)
    hit_list = make_list();
  else {
    GraphicObj *g;
    BOOLEAN redraw;
    Display *d;
    
    d      = XtDisplay(EVViewPToWidget(v_p));
    redraw = YES;
    if (current_from_hit_list)
      EMUnhiliteGraphics(EVGetModel(v_p), current_from_hit_list);
    while ((g = (GraphicObj *)get_from_head(hit_list)) != NULL) {
      if (redraw) {
	redraw = !EscapePressed(d);
	EMUnhiliteGraphics(EVGetModel(v_p), g);
      } else
	EGSetSelected(g, NO);
    }
  }
}


BOOLEAN
PassAttributeCriteria(GraphicObj *g_p)
{
  BOOLEAN result = YES;
  
  if (sel_data.attrib_val_mask != (unsigned long)0) {
    result = EGAttribValAsInAS(g_p, sel_data.attrib_val_mask);
  }
  if (sel_data.pickable_entity_mask != (unsigned long)0) {
    if (!(sel_data.pickable_entity_mask & EGGetPickableMask(g_p)))
      result = NO;
  }
  
  return result;
}


int 
EVDispatchHitTest(EView *v_p, GraphicObj *g_p)
{
  VCRec ll, ur;

  EGGraphicsBoundingBoxVC(v_p, g_p, &ll, &ur);
  if (EVBBoxCutsView(v_p, &ll, &ur) == YES) {
    if (EGGraphicHitByBox(v_p, &(sel_data.aperture), select_criterion, g_p)){
      if (sel_data.take_as_NOT) {
				if (!PassAttributeCriteria(g_p))
					add_to_tail(hit_list, g_p);
      }else {
				if (PassAttributeCriteria(g_p))
					add_to_tail(hit_list, g_p);
      }
		}
  }
  return 1;
}



static int 
EVHitListNumberOfHits(void)
{
  return list_length(hit_list);
}



static int 
EVMarkUnmarkGraphics(EView *v_p, GraphicObj *g_p)
{
  if (g_p == NULL)
    return NOPE;
	if(EGIsMarked(g_p) == NO){
    add_to_tail(selection_list, g_p);
		EGSetMarked(g_p, YES);
    return MARKED;
	} else {
    delete_list_node(selection_list, (NODE)g_p);
		EGSetMarked(g_p, NO);
    return UNMARKED;
  }
}



static void 
EVUnmarkGraphics(EView *v_p, GraphicObj *g_p)
{
  if (g_p == NULL)
    return;
	if(EGIsMarked(g_p) == YES){
    delete_list_node(selection_list, (NODE)g_p);
		EGSetMarked(g_p, NO);
  } 
}



static void 
EVMarkGraphics(EView *v_p, GraphicObj *g_p)
{
  if (g_p == NULL)
    return;
	if(EGIsMarked(g_p) == NO){
    add_to_tail(selection_list, g_p);
		EGSetMarked(g_p, YES);
  } 
}



static GraphicObj *
EVGetFromHitList()
{
  current_from_hit_list = (GraphicObj *)get_from_head(hit_list);
  return current_from_hit_list;
}



static GraphicObj *
EVHitGraphics()
{
  return current_from_hit_list;
}



static void 
EVExposeEH(Widget w, XtPointer p, XEvent *event, Boolean *ctd)
{
  EView *v_p = (EView *)p;
  
  if (event->xexpose.count == 0) {
    recompute_dc_size(v_p);
    AllocBackBuffer(v_p);
    EVFastRedraw(v_p);
  }
  *ctd = TRUE;
}


/* PUBLIC */


EView  *
EVInitView(Widget drawing_area,
	   Widget top_widget,
	   EPixel bg_pixel,
	   EPixel fg_pixel,
	   WCRec *vrp,
	   WCRec *normal,
	   WCRec *vup,
	   WCRec *vorigin,
	   FPNum view_x_dim, FPNum view_y_dim)
{
  EView  *v_p;
  int  i;
  EPixel foreground = fg_pixel, background = bg_pixel;
  XGCValues vals;
  unsigned long aMask;

/* Was ELIXIR initialized?  */

  if (!ElixirInitialized()) {
    fprintf(stderr, "ELIXIR not initialized\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }

/* One-time initialization: */

  EVOneTimeInit(drawing_area);
   
/* Alloc */
  
  if ((v_p = (EView  *) make_node(sizeof(EView))) == NULL)
    return NULL;

/* Widget */

  v_p->view_widget = drawing_area;
  v_p->top_view_widget = top_widget;
  EVSaveViewP(v_p->view_widget, v_p);

/* Drawable */

  v_p->draw_into     = XtWindow(v_p->view_widget);

/* Geometry: */

  v_p->VCOrigin.x     = vorigin->x;
  v_p->VCOrigin.y     = vorigin->y;
  v_p->VCOrigin.z     = vorigin->z;
  v_p->VectUp.x       = vup->x;
  v_p->VectUp.y       = vup->y;
  v_p->VectUp.z       = vup->z;
  v_p->Normal.x       = normal->x;
  v_p->Normal.y       = normal->y;
  v_p->Normal.z       = normal->z;
	v_p->VectorU.x      = 0.0;
	v_p->VectorU.y      = 0.0;
	v_p->VectorU.z      = 0.0;
	v_p->VectorV.x      = 0.0;
	v_p->VectorV.y      = 0.0;
	v_p->VectorV.z      = 0.0;
  EVSetVRP(v_p, vrp);		/* sets also VRP in VC */
  EVSetupViewOrientation(v_p);
  EVSetDimsVC(v_p, view_x_dim, view_y_dim);

/* Colors and cursor */

  v_p->foreground    = foreground;
  v_p->background    = background;
  v_p->cursor = XCreateFontCursor(XtDisplay(v_p->view_widget), XC_draft_small);
  XDefineCursor(XtDisplay(v_p->view_widget), XtWindow(v_p->view_widget),
                v_p->cursor);
  EVRecolorCursor(v_p);

/* Graphic Model */
  v_p->model_p = NULL;

/* Graphic contexts */

  aMask = 0;
  vals.foreground    = foreground; aMask |= GCForeground;
  vals.background    = background; aMask |= GCBackground;
  vals.function      = GXcopy;  aMask |= GCFunction;
  vals.line_width    = DEFAULT_LINE_WIDTH; aMask |= GCLineWidth;
  vals.line_style    = LineSolid; aMask |= GCLineStyle;
  vals.fill_style    = FillSolid; aMask |= GCFillStyle;
  vals.fill_rule     = WindingRule;  aMask |= GCFillRule;
  vals.font          = FontDefaultFont();  aMask |= GCFont;
  v_p->defaultCopyGC = XtGetGC(drawing_area, aMask, &vals);

  aMask = 0;
  vals.foreground    = foreground; aMask |= GCForeground;
  vals.background    = background; aMask |= GCBackground;
  vals.function      = GXcopy; aMask |= GCFunction;
  vals.line_width    = DEFAULT_LINE_WIDTH; aMask |= GCLineWidth;
  vals.line_style    = LineSolid; aMask |= GCLineStyle;
  vals.fill_style    = FillSolid; aMask |= GCFillStyle;
  vals.fill_rule     = WindingRule;  aMask |= GCFillRule;
  v_p->writableGC    = XCreateGC(XtDisplay(drawing_area),
				 XtWindow(drawing_area), aMask, &vals);

  aMask = 0;
  vals.foreground    = foreground; aMask |= GCForeground;
  vals.background    = background; aMask |= GCBackground;
  vals.function      = GXcopy; aMask |= GCFunction;
  vals.line_width    = 0; aMask |= GCLineWidth;
  vals.line_style    = LineOnOffDash; aMask |= GCLineStyle;
  v_p->constrCopyGC  = XtGetGC(drawing_area, aMask, &vals);
  
  aMask = 0;
  vals.foreground    = background; aMask |= GCForeground;
  vals.background    = background; aMask |= GCBackground;
  vals.function      = GXcopy; aMask |= GCFunction;
  vals.line_width    = 0; aMask |= GCLineWidth;
  vals.line_style    = LineOnOffDash; aMask |= GCLineStyle;
  v_p->constrEraseGC = XtGetGC(drawing_area, aMask, &vals);

    aMask = 0;
  vals.foreground    = foreground; aMask |= GCForeground;
  vals.background    = background; aMask |= GCBackground;
  vals.function      = GXxor; aMask |= GCFunction;
  vals.line_width    = 0; aMask |= GCLineWidth;
  vals.line_style    = LineOnOffDash; aMask |= GCLineStyle;
  v_p->constrXORGC   = XtGetGC(drawing_area, aMask, &vals);

  aMask = 0;
  vals.foreground     = background; aMask |= GCForeground;
  vals.background     = background; aMask |= GCBackground;
  vals.function       = GXcopy; aMask |= GCFunction;
  vals.line_width     = DEFAULT_LINE_WIDTH; aMask |= GCLineWidth;
  vals.line_style     = LineSolid; aMask |= GCLineStyle;
  vals.fill_style     = FillSolid; aMask |= GCFillStyle;
  vals.fill_rule      = WindingRule;  aMask |= GCFillRule;
  v_p->defaultEraseGC = XtGetGC(drawing_area, aMask, &vals);

  aMask = 0;
  vals.foreground    = background; aMask |= GCForeground;
  vals.background    = background; aMask |= GCBackground;
  vals.function      = GXcopy; aMask |= GCFunction;
  vals.line_width    = DEFAULT_LINE_WIDTH; aMask |= GCLineWidth;
  vals.line_style    = LineSolid; aMask |= GCLineStyle;
  vals.fill_style    = FillSolid; aMask |= GCFillStyle;
  vals.fill_rule     = WindingRule;  aMask |= GCFillRule;
  v_p->eraseGC       = XCreateGC(XtDisplay(drawing_area),
				 XtWindow(drawing_area), aMask, &vals);

  aMask = 0;
  vals.foreground   = foreground ^ background; aMask |= GCForeground;
  vals.background   = background; aMask |= GCBackground;
  vals.function     = GXxor; aMask |= GCFunction;
  v_p->defaultXORGC = XtGetGC(drawing_area, aMask, &vals);

  aMask = 0;
  vals.foreground = foreground; aMask |= GCForeground;
  vals.background = background; aMask |= GCBackground;
  vals.line_style = LineSolid; aMask |= GCLineStyle;
  vals.line_width = GRID_LINE_WIDTH; aMask |= GCLineWidth;
  v_p->gridGC     = XtGetGC(drawing_area, aMask, &vals);

  aMask = 0;
  vals.foreground = foreground; aMask |= GCForeground;
  vals.background = background; aMask |= GCBackground;
  vals.function   = GXcopy; aMask |= GCFunction;
  vals.line_width = HILITE_LINE_WIDTH; aMask |= GCLineWidth;
  vals.line_style = LineSolid; aMask |= GCLineStyle;
  vals.fill_style = FillSolid; aMask |= GCFillStyle;
  vals.fill_rule = WindingRule;  aMask |= GCFillRule;
  v_p->hiliteGC   = XtGetGC(drawing_area, aMask, &vals);

  aMask = 0;
  vals.foreground    = background; aMask |= GCForeground;
  vals.background    = background; aMask |= GCBackground;
  vals.function      = GXcopy; aMask |= GCFunction;
  vals.line_width    = HILITE_LINE_WIDTH; aMask |= GCLineWidth;
  vals.line_style    = LineSolid; aMask |= GCLineStyle;
  vals.fill_style    = FillSolid; aMask |= GCFillStyle;
  vals.fill_rule     = WindingRule; aMask |= GCFillRule;
  v_p->erasehiliteGC = XtGetGC(drawing_area, aMask, &vals);
  XCopyGC(XtDisplay(drawing_area), v_p->hiliteGC,
	  GCLineStyle | GCLineWidth | GCCapStyle | GCJoinStyle,
	  v_p->erasehiliteGC);
  
/* Coordinate axes, color scale and status area display */

  v_p->show_axes = YES;
	v_p->show_scale = NO;
	v_p->show_status = NO;

/* should preserve vup during mouse-driven rotation */
	v_p->preserve_vup = NO;

/* Vector up at the start of rotation */
  v_p->VectUpAtStart = v_p->VectUp;

/* Grid */

  v_p->grid_on       = NO;
  v_p->grid_origin.u = 0.0;
  v_p->grid_origin.v = 0.0;
  v_p->grid_deltas.u = 1.0;
  v_p->grid_deltas.v = 1.0;
  v_p->grid_lock_on  = NO;

/* Construction plane */

  v_p->constrplane_lock_on = NO;
  v_p->constrplane_center.x = 0;
  v_p->constrplane_center.y = 0;
  v_p->constrplane_center.z = 0;
  v_p->constrplane_u.x      = 1;
  v_p->constrplane_u.y      = 0;
  v_p->constrplane_u.z      = 0;
  v_p->constrplane_v.x      = 0;
  v_p->constrplane_v.y      = 1;
  v_p->constrplane_v.z      = 0;
  v_p->constrplane_normal.x = 0;
  v_p->constrplane_normal.y = 0;
  v_p->constrplane_normal.z = 1;
  
/* Layers: */
  
  for (i = 0; i < MAX_LAYER; i++) {
    v_p->view_layers[i].layer_on     = NO;
  }
  v_p->view_layers[0].layer_on = YES; /* This one is ON by default */

/* Display lists */
  
  v_p->visible_GO = make_list();
  v_p->invisible_GO = make_list();

/* Fringe-table  */

  v_p->fringe_table = NULL;

/* Rendering */

  v_p->render_mode = NORMAL_RENDERING;
  v_p->backup_render_mode = -1;
  v_p->dir_towards_light.u =
    v_p->dir_towards_light.v =
      v_p->dir_towards_light.n =  0;
  v_p->shade_mode = NO_SHADING;

/* Clipping by view planes */
  
  v_p->should_clip_by_fg_bg_view_planes = NO;
  v_p->bg_view_plane_n = -FLT_MAX; /* -- far away */
  v_p->fg_view_plane_n =  FLT_MAX; /* -- toward the viewer */

/* Clipping by model planes */

  v_p->should_clip_by_model_planes = NO;
  v_p->model_clip_plane.clip_is_on = NO;
  v_p->model_clip_plane.center.x   = 0;
  v_p->model_clip_plane.center.y   = 0;
  v_p->model_clip_plane.center.z   = 0;
  v_p->model_clip_plane.normal.x   = 0;
  v_p->model_clip_plane.normal.y   = 0;
  v_p->model_clip_plane.normal.z   = 1;
  
/* Callbacks: */

  XtAddEventHandler(drawing_area,
		    ExposureMask,
		    FALSE, EVExposeEH, v_p);
  XtAddEventHandler(drawing_area,
		    StructureNotifyMask,
		    FALSE, EVResizeEH, v_p);
  XtAddEventHandler(drawing_area,
		    ButtonPressMask | ButtonReleaseMask |
                    PointerMotionMask |
		    EnterWindowMask | LeaveWindowMask |
		    KeyPressMask | KeyReleaseMask,
		    FALSE, EVInputEH, v_p);
  XSetWMProtocols(XtDisplay(drawing_area), XtWindow(v_p->top_view_widget),
		  &delete_widget_atom, 1);
  XtAddEventHandler(v_p->top_view_widget,
		    StructureNotifyMask /* | SubstructureNotifyMask */
                    /* and all the other non-maskable events */,
		    TRUE, EVNonMaskableEH, v_p);
  CLEAR_HANDLER_DATA(v_p, the_active_handler);
  CLEAR_HANDLER_DATA(v_p, the_suspended_handler);
  v_p->the_active_handler.handler = (EventHandlerP)EVFastViewHandler;

  v_p->was_destroyed = NO;
  
  return v_p;
}



EView *
EVRetrieveViewP(Widget w)
{
  EView *v_p;
  
  if (XFindContext(XtDisplay(w), XtWindow(w),
		   save_context_id, (caddr_t *)&v_p) != 0) {
    fprintf(stderr, "Failed to retrieve context in EVRetrieveViewP\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  } else
    return (v_p);
}



void 
EVInputPoint(EView *v_p, XEvent *event, WCRec *p)
{
  if (event->xany.send_event == True) {
    EVRetrieveSentPoint(p); /* precision point */
  } else {
    if (event->type == ButtonPress) {
      if (EVGetSnapPoint(p) == YES) {
				UndrawSnapPoint(v_p);
      } else {
				EVConvertInputPoint(v_p, event->xbutton.x, event->xbutton.y, p);
      }
    } else {
      EVConvertInputPoint(v_p, event->xbutton.x, event->xbutton.y, p);
    }
  }
}



void 
EVConvertInputPoint(EView *v_p, int DCx, int DCy, WCRec *p)
{
  EVDCtoWC(v_p, DCx, DCy, p);
  EVAdjustToLocks(v_p, p);
}



void 
EVSetDimsVC(EView *v_p, FPNum x_dim, FPNum y_dim)
{
  
  FPNum actual_ratio;
  unsigned int width, height;
  
  EVGetDimsDC(v_p, &width, &height);

  v_p->view_dims_DC.x   = width;
  v_p->view_dims_DC.y   = height;
  if (y_dim <= 0) y_dim = 1;
  if (x_dim <= 0) x_dim = y_dim;
  v_p->view_dims_VC.u   = x_dim;
  v_p->view_dims_VC.v   = y_dim;
  actual_ratio          = (FPNum)height/(FPNum)width;
  if (actual_ratio > (v_p->view_dims_VC.v / v_p->view_dims_VC.u))
    v_p->view_dims_VC.v = v_p->view_dims_VC.u * actual_ratio;
  else
    v_p->view_dims_VC.u = v_p->view_dims_VC.v / actual_ratio;
}



static void 
EVInputEH(Widget w, XtPointer p, XEvent *event, Boolean *ctd)
{
  EView *v_p = (EView *)p;

  if (EVPreprocessInput(v_p, event) == NO) {
    if (v_p->the_active_handler.handler == NULL) {
      CLEAR_HANDLER_DATA(v_p, the_active_handler);
      v_p->the_active_handler.handler = (EventHandlerP)EVFastViewHandler;
    }
    (*(v_p->the_active_handler.handler))(w, v_p, event);
  }
  *ctd = TRUE; 
}



static void 
EVNonMaskableEH(Widget w, XtPointer p, XEvent *event, Boolean *ctd)
{
  EView *v_p = (EView *)p;
  Widget wdgt;

  switch (event->type) {
  case ClientMessage:
    {
      XClientMessageEvent *cm = &event->xclient;
      
      if (   (cm->message_type == STARTING_INPUT_HANDLER)
          || (cm->message_type == RESUMING_INPUT_HANDLER)) {
        EVInputEH(v_p->view_widget, p, event, ctd);
      } else {
        if (cm->format == 32) {
          if (cm->data.l[0] == delete_widget_atom) {
            wdgt = EVFreeView(v_p);
            XtDestroyWidget(v_p->top_view_widget); /* hide the view */
          } 
        }
      }
    }
    break;
  default:
    break;
  }
  *ctd = TRUE;
}



void 
EVFastViewHandler(Widget w, EView *v_p, XEvent *event)
{ 
  switch(event->type) {
  case ButtonPress: 
    EVFastViewHButtonPress(w, v_p, (XButtonEvent *)event);
    break;
  case MotionNotify:
    EVFastViewHMotion(w, v_p, (XMotionEvent *)event);
    break;
  case ButtonRelease:
    EVFastViewHButtonRelease(w, v_p, (XButtonEvent *)event);
    break;
  case KeyPress:
  case KeyRelease:
    EVFastViewHKey(w, v_p, (XKeyEvent *)event);
    break;
  case ClientMessage:
    /* Handler has been (re)installed */
    ERptPrompt(ELIXIR_PROMPT_CLASS, MESSAGE_FV1,
               elixir_default_prompts[MESSAGE_FV1]);
    break;
  }
}


static WCRec current_pointer_pos_in_WC;

void
EVLastPointerWCPos(WCRec *wcp)
{
  wcp->x = current_pointer_pos_in_WC.x;
  wcp->y = current_pointer_pos_in_WC.y;
  wcp->z = current_pointer_pos_in_WC.z;
}



void 
EVFastViewHMotion(Widget w, EView *v_p, XMotionEvent *event)
{
  WCRec wcp, newwcp;
  FPNum xdim, ydim, faCtor, udist, vdist;
  VC2DRec p1, p2;
  VCRec vp;

  EVDCtoWC(v_p, event->x, event->y, &wcp);
      
  if(event->state & ControlMask) {
    if (!(event->state & ShiftMask)) {
      if (fv_data.action == PANNING) { 
				newwcp.x = fv_data.origin_at_start.x - (wcp.x - fv_data.last_point.x);
				newwcp.y = fv_data.origin_at_start.y - (wcp.y - fv_data.last_point.y);
				newwcp.z = fv_data.origin_at_start.z - (wcp.z - fv_data.last_point.z);
				EVSetVRP(v_p, &newwcp);
				fv_data.origin_at_start = newwcp;
				EVFastRedraw(v_p);
        ERptPrompt(ELIXIR_PROMPT_CLASS,
									 MESSAGE_FV4, elixir_default_prompts[MESSAGE_FV4]);
      } else if (fv_data.action == ZOOMING) { 
				faCtor  = 1. -
          ZOOM_COEFF * ((FPNum)(event->y - fv_data.last_point_DC.y)
                        /(FPNum)fv_data.DC_dims.y);
				EVGetDimsVC(v_p, &xdim, &ydim);
				EVSetDimsVC(v_p, xdim*faCtor, ydim*faCtor);
				EVFastRedraw(v_p);
        ERptPrompt(ELIXIR_PROMPT_CLASS,
									 MESSAGE_FV5, elixir_default_prompts[MESSAGE_FV5]);
      }
    } else if ((event->state & ShiftMask)) {
      if (fv_data.action == ROTATING) {
				fv_data.last_point_DC.x = event->x;
				fv_data.last_point_DC.y = event->y;
				udist = fv_data.last_point_DC.x - fv_data.origin_at_start_DC.x;
				vdist = fv_data.last_point_DC.y - fv_data.origin_at_start_DC.y;
				EVRotateByMouse(v_p, -vdist/fv_data.DC_dims.x*ROT_MAGNIFACTOR,
												-udist/fv_data.DC_dims.y*ROT_MAGNIFACTOR);
				fv_data.origin_at_start_DC = fv_data.last_point_DC;
        ERptPrompt(ELIXIR_PROMPT_CLASS,
									 MESSAGE_FV3, elixir_default_prompts[MESSAGE_FV3]);
      }
    }
  } else { /* !(event->state & ControlMask) */
    if (fv_data.action == WINDOWING) {
      EVDCtoWC(v_p, event->x, event->y, &(fv_data.last_point));
      XorDrawRectVC(v_p, fv_data.rubber_band_echo);
      EVWCtoVC(v_p, &(fv_data.origin_at_start), &vp);
      p1.u = vp.u,       p1.v = vp.v;
      EVWCtoVC(v_p, &(fv_data.last_point), &vp);
      p2.u = vp.u,       p2.v = vp.v;
      XorDrawRectVC(v_p, ModifyRectVC(fv_data.rubber_band_echo, &p1, &p2));
      ERptPrompt(ELIXIR_PROMPT_CLASS,
                 MESSAGE_FV2, elixir_default_prompts[MESSAGE_FV2]);
    } else {
      ERptPrompt(ELIXIR_PROMPT_CLASS,
                 MESSAGE_FV1, elixir_default_prompts[MESSAGE_FV1]);
      current_pointer_pos_in_WC.x = wcp.x;
      current_pointer_pos_in_WC.y = wcp.y;
      current_pointer_pos_in_WC.z = wcp.z;

      ERptTrackPointer(&wcp);
    }
  }
}



void 
EVFastViewHButtonPress(Widget w, EView *v_p, XButtonEvent *event)
{
  unsigned int width, height;
  VCRec p1, p2;
  WCRec wp1, wp2, p;
	
  switch(event->button) {
  case LEFT_BUTTON:
    if(event->state & ControlMask) {
      if (!(event->state & ShiftMask)) {
	fv_data.action = PANNING;
	force_wire_display = YES;
	EVDCtoWC(v_p, event->x, event->y, &(fv_data.last_point));
	EVGetVRP(v_p, &(fv_data.origin_at_start));
	/* EVSetVRP(v_p, &(fv_data.origin_at_start)); */
  EVWCtoVC(v_p, &(v_p->ViewRefPoint), &(v_p->ViewRefPointVC));
      } else if (event->state & ShiftMask) {
	fv_data.action = ROTATING;
	force_wire_display = YES;
	fv_data.origin_at_start_DC.x = event->x;
 	fv_data.origin_at_start_DC.y = event->y;
	EVGetDimsDC(v_p, &width, &height);
	fv_data.DC_dims.x = width;
	fv_data.DC_dims.y = height;
	v_p->VectUpAtStart = v_p->VectUp;
	ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_FV3, elixir_default_prompts[MESSAGE_FV3]);
      }
    } else { 
      if (fv_data.action == WINDOWING) { 
	EVDCtoWC(v_p, event->x, event->y, &(fv_data.last_point));
	if (!(fv_data.last_point.x == fv_data.origin_at_start.x &&
	      fv_data.last_point.y == fv_data.origin_at_start.y &&
	      fv_data.last_point.z == fv_data.origin_at_start.z)) {
	  EVWCtoVC(v_p, &(fv_data.last_point), &p1);
	  EVWCtoVC(v_p, &(fv_data.origin_at_start), &p2);
	  p1.n = p2.n = 0;
	  EVVCtoWC(v_p, &p1, &wp1);
	  EVVCtoWC(v_p, &p2, &wp2);
	  p.x = 0.5*(wp1.x+wp2.x);
	  p.y = 0.5*(wp1.y+wp2.y);
	  p.z = 0.5*(wp1.z+wp2.z);
	  EVSetVRP(v_p, &p);
	  EVSetDimsVC(v_p, fabs(p1.u-p2.u), fabs(p1.v-p2.v));
	}
	XorDrawRectVC(v_p, fv_data.rubber_band_echo);
	DestroyRectVC(fv_data.rubber_band_echo);
	fv_data.rubber_band_echo = NULL;
	EVForceRedisplay(v_p);
	fv_data.action = NO_FAST_VIEWING_ACTION_RUNNING;
	ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_FV1, elixir_default_prompts[MESSAGE_FV1]);
      } else if (fv_data.action == NO_FAST_VIEWING_ACTION_RUNNING) {
	VC2DRec vp = {0, 0};
	
	fv_data.action = WINDOWING;
	EVDCtoWC(v_p, event->x, event->y, &(fv_data.origin_at_start));
	fv_data.last_point.x     = fv_data.origin_at_start.x;
	fv_data.last_point.y     = fv_data.origin_at_start.y;
	fv_data.last_point.z     = fv_data.origin_at_start.z;
	XorDrawRectVC(v_p, fv_data.rubber_band_echo = CreateRectVC(&vp, &vp));
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_FV2, elixir_default_prompts[MESSAGE_FV2]);
      }
    }
    break;
  case MIDDLE_BUTTON:
    if(event->state & ShiftMask) { /* fit all graphics to view */
      EVFitAllIntoView(v_p);
    }else if(event->state & ControlMask) { /* start zooming */
      unsigned int xdim, ydim;
      
      fv_data.action = ZOOMING;
      force_wire_display = YES;
      fv_data.last_point_DC.x = event->x;
      fv_data.last_point_DC.y = event->y;
      EVGetDimsDC(v_p, &xdim, &ydim);
      fv_data.DC_dims.x = xdim; fv_data.DC_dims.y = ydim;
    }
    break;
  case RIGHT_BUTTON:
    /* cancel any action */
    if (fv_data.action == WINDOWING) {
      XorDrawRectVC(v_p, fv_data.rubber_band_echo);
      DestroyRectVC(fv_data.rubber_band_echo);
      fv_data.rubber_band_echo = NULL;
    }
    if (fv_data.action == NO_FAST_VIEWING_ACTION_RUNNING)
      EVResumeSuspendedHandler(v_p);
    else
      ERptPrompt(ELIXIR_PROMPT_CLASS,
                 MESSAGE_FV1, elixir_default_prompts[MESSAGE_FV1]);
    fv_data.action = NO_FAST_VIEWING_ACTION_RUNNING;
    break;
  }
}



void 
EVFastViewHKey(Widget w, EView *v_p, XKeyEvent *event)
{
  char buffer[12];
  KeySym ksym;
  int lngth;
  static XComposeStatus	c_stat;
  
  switch(event->type) {
  case KeyPress:
    lngth = XLookupString(event, buffer, 12, &ksym, &c_stat);
    buffer[lngth] = '\0';
    EVHandleKeyPress(v_p, event, ksym, buffer);
    break;
  case KeyRelease:
    EVHandleKeyRelease(v_p, event, ksym, buffer);
    break;
  }
}


void 
EVFastViewHButtonRelease(Widget w, EView *v_p, XButtonEvent *event)
{
	WCRec vrp, wcp, newwcp;

  if (fv_data.action == PANNING)
    fv_data.action = NO_FAST_VIEWING_ACTION_RUNNING;
  if (fv_data.action == ZOOMING)
    fv_data.action = NO_FAST_VIEWING_ACTION_RUNNING;
  if (fv_data.action == ROTATING) {

		EVDCtoWC(v_p, event->x, event->y, &wcp);
		EVGetVRP(v_p, &vrp);
		/* EVSetVRP(v_p, &vrp); */
		EVWCtoVC(v_p, &(v_p->ViewRefPoint), &(v_p->ViewRefPointVC));
		EVDCtoWC(v_p, event->x, event->y, &newwcp);
		newwcp.x = vrp.x - (newwcp.x - wcp.x);
		newwcp.y = vrp.y - (newwcp.y - wcp.y);
		newwcp.z = vrp.z - (newwcp.z - wcp.z);
		EVSetVRP(v_p, &newwcp);

    fv_data.action = NO_FAST_VIEWING_ACTION_RUNNING;
    ERptPrompt(ELIXIR_PROMPT_CLASS,
               MESSAGE_FV1, elixir_default_prompts[MESSAGE_FV1]);
		if(v_p->preserve_vup)v_p->VectUp = v_p->VectUpAtStart;
  }
  if (force_wire_display) {
    force_wire_display = NO;
    EVFastRedraw(v_p);
  }
}



void 
EVWCtoDC(EView *v_p, WCRec *wp, int *DCx, int *DCy)
{
  VCRec vp;

  EVWCtoVC(v_p, wp, &vp);
  EVVCtoDC(v_p, &vp, DCx, DCy);
}



void 
EVVCtoDC(EView *v_p, VCRec *vp, int *DCx, int *DCy)
{
  FPNum DC_to_VC;

  DC_to_VC = (FPNum)(v_p->view_dims_DC.x) / v_p->view_dims_VC.u;
  *DCx     =
    (int)(DC_to_VC *( vp->u - v_p->ViewRefPointVC.u +
		     v_p->view_dims_VC.u*0.5));
  *DCy     =
    (int)(DC_to_VC *(-vp->v + v_p->ViewRefPointVC.v +
		     v_p->view_dims_VC.v*0.5));
}



void 
EVDCtoWC(EView *v_p, int DCx, int DCy, WCRec *wp)
{
  VCRec vp;
  
  EVDCtoVC(v_p,DCx, DCy, &vp);
  EVVCtoWC(v_p, &vp, wp);
}



void 
EVDCtoVC(EView *v_p, int DCx, int DCy, VCRec *vp)
{
  FPNum VC_to_DC;

  VC_to_DC = v_p->view_dims_VC.u / (FPNum)(v_p->view_dims_DC.x);
  vp->u = VC_to_DC * ( DCx) +
    (v_p->ViewRefPointVC.u - v_p->view_dims_VC.u*0.5);
  vp->v = VC_to_DC * (-DCy) +
    (v_p->ViewRefPointVC.v + v_p->view_dims_VC.v*0.5);
  vp->n = 0;
}


void 
EVRetrieveSentPoint(WCRec *p)
{
  if (sent_point.point_is_valid == YES) {
    p->x = sent_point.coords.x;
    p->y = sent_point.coords.y;
    p->z = sent_point.coords.z;
    sent_point.point_is_valid = NO; /* invalidate this point */
  } else {
    fprintf(stderr, "Retrieved point is not valid\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
}


static int 
undraw_snap_point(NODE data, NODE v_p)            /* added by dr */
{
	UndrawSnapPoint((EView *)v_p);
	return(1);
}

void 
EVSaveSentPoint(WCRec *p)
{
  sent_point.coords.x = p->x;
  sent_point.coords.y = p->y;
  sent_point.coords.z = p->z;
  sent_point.point_is_valid = YES;

/* invalidate snap point to prevent its use after next B1 click - by dr;
	 this phenomenon occured when an operation was done using sent_point (i.e. snap_point was
	 set (SB1) and then xy or dx command was used);
	 then during the next operation the next B1 click used the last snap_point already utilized
	 by the sent_point */

	snap_point.point_is_valid = NO;            

/* there is however problem, that I undraw the snap_point here, because I have 
	 no access to view anymore;
	 therefore dispatch is used - by dr */

	EMDispatchToDependentViews(ESIModel(), undraw_snap_point, NULL);
}



void 
EVSetApplyToViewFunction(ApplyFuncP func_p, caddr_t data_p,
                         ResetFuncP reset_func_p)
{
  apply_func.func_p            = func_p;
  apply_func.data_p            = data_p;
  apply_func.reset_func_p      = reset_func_p;
  apply_func.prevent_redisplay = NO; /* do redisplay by default */
  EVSetApplyToViewPrompt(ELIXIR_PROMPT_CLASS, MESSAGE_ATV1,
                         elixir_default_prompts[MESSAGE_ATV1]);
}


void 
EVSetApplyToViewPreventRedisplay(BOOLEAN flag)
{
  apply_func.prevent_redisplay = flag;
}


void 
EVSetApplyToViewPrompt(int class_number, int serial_number, char *p)
{
  apply_func.prompt_class      = class_number;
  apply_func.prompt_serial     = serial_number;
  if (apply_func.prompt != NULL) free(apply_func.prompt);
  apply_func.prompt            = strdup(p);
}



void 
EVApplyToViewHandler(Widget w, EView *v_p, XEvent *e)
{
  XButtonEvent *event;
  WCRec p;

  event = (XButtonEvent *)e;
  switch(e->type) {
  case ButtonPress:
    switch(event->button) {
    case Button1:
      EVInputPoint(v_p, e, &p);
      if (apply_func.func_p != NULL)
	(*apply_func.func_p)(v_p, apply_func.data_p, &p);
      if (!apply_func.prevent_redisplay)EVForceRedisplay(v_p);
      break;
    case Button3:
      if (apply_func.reset_func_p != NULL)
	(*apply_func.reset_func_p)(v_p, apply_func.data_p);
      else
	EVUninstallHandler(v_p);
      break;
    }
    break;
  case MotionNotify:
    if (apply_func.prompt != NULL)
      ERptPrompt(apply_func.prompt_class,
                 apply_func.prompt_serial, apply_func.prompt);
    break;
  case ClientMessage:
    /* Handler has been (re)installed */
    if (apply_func.prompt != NULL)
      ERptPrompt(apply_func.prompt_class,
                 apply_func.prompt_serial, apply_func.prompt);
    break;
  }
}



void 
EVSelectGraphicsHandler(Widget w, EView *v_p,
			      XEvent *event)
{
  switch(event->type) {
  case ButtonPress:
    EVSelectGraphicsBPressHandler(w, v_p, (XButtonEvent *)event);
    break;
  case MotionNotify:
    EVSelectGraphicsMNotifyHandler(w, v_p, (XMotionEvent *)event);
    break;
  case ClientMessage:
    ERptPrompt(ELIXIR_PROMPT_CLASS,
                     MESSAGE_SEL1, elixir_default_prompts[MESSAGE_SEL1]);
    break;
  }
}



static void 
EVInitHitList(EView *v_p, WCRec *hit)
{
  GraphicObj *g_p;
  BOOLEAN redraw;
  Display *d;

  ClearHitList(v_p);
  EVTestHitOnVisibleGraphics(v_p, hit_list);
  redraw = YES;
  d      = XtDisplay(EVViewPToWidget(v_p));
  g_p    = (GraphicObj *)get_list_next(hit_list, NULL);
  while (g_p != NULL) {
    if (redraw) {
      redraw = !EscapePressed(d);
      EMUnhiliteGraphics(EVGetModel(v_p), g_p);
    } else
      EGSetSelected(g_p, NO);
    g_p = (GraphicObj *)get_list_next(hit_list, g_p);
  }
}



static void 
EVTestHitOnVisibleGraphics(EView *v_p, LIST hit_list)
{
  if (v_p->visible_GO != NULL)
    for_all_list(v_p->visible_GO, v_p, EVDispatchHitTest);
}



static void 
EVInitSelection(EView *v_p, WCRec *p)
{
  ClearSelectionList(v_p);
  ClearHitList(v_p);
  sel_data.action = FIXED_SIZE_APERTURE_SELECTION;
  InitFixedSizeAperture(v_p, p);
  ERptPrompt(ELIXIR_PROMPT_CLASS,
             MESSAGE_SEL1, elixir_default_prompts[MESSAGE_SEL1]);
  selection_started = YES;
}



static void 
ClearSelectionList(EView *v_p)
{
  if (selection_list == NULL)
    selection_list = make_list();
  else {
    GraphicObj *g;
    BOOLEAN redraw;
    Display *d;

    d      = XtDisplay(EVViewPToWidget(v_p));
    redraw = YES;
    while ((g = (GraphicObj *)get_from_head(selection_list)) != NULL) {
			EGSetMarked(g, NO);
      if (redraw) {
	redraw = !EscapePressed(d);
	EMUnhiliteGraphics(EVGetModel(v_p), g);
      } else
	EGSetSelected(g, NO);
    }
  }
}



void 
EVClearSelection(void)
{
	GraphicObj *g_p = NULL;

  if (selection_list == NULL)
    selection_list = make_list();
  else{
		g_p = (GraphicObj *)get_list_next(selection_list, NULL);
		while (g_p != NULL) {
      EGSetMarked(g_p, NO);
			g_p = (GraphicObj *)get_list_next(selection_list, g_p);
		}
    flush_list(selection_list, NOT_NODES);
	}
}



int 
EVItemsInSelectionList(void)
{
  if (selection_list != NULL)
    return list_length(selection_list);
  else
    return 0;
}



GraphicObj *
EVFirstInSelectionList(void)
{
  return (GraphicObj *)get_list_next(selection_list, NULL);
}



GraphicObj *
EVNextInSelectionList(GraphicObj *g_p)
{
  return (GraphicObj *)get_list_next(selection_list, g_p);
}



GraphicObj *
EVGetFromSelectionList(void)
{
	GraphicObj *g_p;

  if (list_empty(selection_list) == TRUE)
    return NULL;

	g_p = (GraphicObj *)get_from_head(selection_list);
	EGSetMarked(g_p, NO);
	return g_p;
}



void 
EVSetSelectActionFunction(SelectActionFuncP func_p,
                          caddr_t data_p,
                          ResetFuncP reset_func_p)
{
  select_action_func.func_p       = func_p;
  select_action_func.data_p       = data_p;
  select_action_func.reset_func_p = reset_func_p;
  selection_started = NO;       /* make sure the handler is started properly */
}



void 
EVSelectGraphicsBPressHandler(Widget w, EView *v_p,
			      XButtonEvent *event)
{
  WCRec wcp;
  int op;
  BOOLEAN redraw;
  VCRec vp1, vp2;
  
  switch(event->button) {
  case Button1:
    if (event->state & ControlMask) { /* fence aperture */
      if (sel_data.action == FENCING_SELECTION) {
				EVDCtoWC(v_p, event->x, event->y, &(sel_data.last_point));
				XorDrawRectVC(v_p, sel_data.rubber_band_echo);
				EVWCtoVC(v_p, &(sel_data.last_point), &vp1);
				EVWCtoVC(v_p, &(sel_data.origin_at_start), &vp2);
				sel_data.aperture.left   = min(vp1.u, vp2.u);
				sel_data.aperture.right  = max(vp1.u, vp2.u);
				sel_data.aperture.bottom = min(vp1.v, vp2.v);
				sel_data.aperture.top    = max(vp1.v, vp2.v);
				if (!selection_started) {
					EVInitSelection(v_p, &(sel_data.last_point));
				}
				redraw = YES;
				select_criterion = EASValsGetSelectCriterion();
				EVInitHitList(v_p, &(sel_data.last_point));
				while (EVGetFromHitList() != NULL) {
					EVMarkGraphics(v_p, EVHitGraphics());
					if (redraw) {
						redraw = !EscapePressed(XtDisplay(w));
						EMHiliteGraphics(EVGetModel(v_p), EVHitGraphics());
					}
				}
				InitFixedSizeAperture(v_p, &(sel_data.last_point));
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_SEL1, elixir_default_prompts[MESSAGE_SEL1]);
				select_criterion = INTERSECT;
				sel_data.action  = FIXED_SIZE_APERTURE_SELECTION;
      } else { /* !FENCING_SELECTION */
				EVDCtoWC(v_p, event->x, event->y, &(sel_data.origin_at_start));
				sel_data.last_point.x = sel_data.origin_at_start.x;
				sel_data.last_point.y = sel_data.origin_at_start.y;
				sel_data.last_point.z = sel_data.origin_at_start.z;
				if (sel_data.rubber_band_echo == NULL) {
					VC2DRec p = {0, 0};
					XorDrawRectVC(v_p, sel_data.rubber_band_echo = CreateRectVC(&p, &p));
				} else {
					VC2DRec p1, p2;
					VCRec vp;
					
					XorDrawRectVC(v_p, sel_data.rubber_band_echo);
					EVWCtoVC(v_p, &(sel_data.origin_at_start), &vp);
					p1.u = vp.u,       p1.v = vp.v;
					EVWCtoVC(v_p, &(sel_data.last_point), &vp);
					p2.u = vp.u,       p2.v = vp.v;
					XorDrawRectVC(v_p, ModifyRectVC(sel_data.rubber_band_echo,
																					&p1, &p2));
				}
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_SEL4, elixir_default_prompts[MESSAGE_SEL4]);
				sel_data.action = FENCING_SELECTION;
      }
    } else { /* fixed-size aperture */
      EVDCtoWC(v_p, event->x, event->y, &wcp);
      if (selection_started == NO) {
				EVInitSelection(v_p, &wcp);
      }
      select_criterion = INTERSECT;
      EVInitHitList(v_p, &wcp);
      if (EVHitListNumberOfHits() == 1) {
				op = EVMarkUnmarkGraphics(v_p, EVGetFromHitList());
				if (op == MARKED)
					EMHiliteGraphics(EVGetModel(v_p), EVHitGraphics());
				else if (op == UNMARKED)
					EMUnhiliteGraphics(EVGetModel(v_p), EVHitGraphics());
				EVGetFromHitList();
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_SEL1, elixir_default_prompts[MESSAGE_SEL1]);
      } else if (EVHitListNumberOfHits() > 1) {
				if (EVGetFromHitList() != NULL)
					EMHiliteGraphics(EVGetModel(v_p), EVHitGraphics());
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_SEL3, elixir_default_prompts[MESSAGE_SEL3]);
      } else
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_SEL1, elixir_default_prompts[MESSAGE_SEL1]);
    }
    break;
  case Button2:
		ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");          /* added by dr */
    if (EVHitGraphics() != NULL) {
      EVMarkGraphics(v_p, EVHitGraphics());
      EMHiliteGraphics(EVGetModel(v_p), EVHitGraphics());
      if (EVGetFromHitList() != NULL) {
				EMHiliteGraphics(EVGetModel(v_p), EVHitGraphics());
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_SEL2, elixir_default_prompts[MESSAGE_SEL2]);
      } else
        ERptPrompt(ELIXIR_PROMPT_CLASS,
                   MESSAGE_SEL1, elixir_default_prompts[MESSAGE_SEL1]);
    } else {
      DestroyFixedSizeAperture(v_p);
      EVUnhiliteSelectionList(v_p);
      selection_started = NO;
      if (select_action_func.func_p != NULL)
				(*select_action_func.func_p)(v_p, select_action_func.data_p);
      else
				EMUninstallHandler(EVGetModel(v_p)); /* EXIT POINT */
    }
    break;  
  case Button3:
		ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");          /* added by dr */
    if (sel_data.action == FENCING_SELECTION) {
      XorDrawRectVC(v_p, sel_data.rubber_band_echo);
      DestroyRectVC(sel_data.rubber_band_echo);
      sel_data.rubber_band_echo = NULL;
      sel_data.action = FIXED_SIZE_APERTURE_SELECTION;
			EMUninstallHandler(EVGetModel(v_p)); /* EXIT POINT */
    } else {
      if (EVHitGraphics() != NULL) {
				EVUnmarkGraphics(v_p, EVHitGraphics());
				EMUnhiliteGraphics(EVGetModel(v_p), EVHitGraphics());
				if (EVGetFromHitList() != NULL) {
					EMHiliteGraphics(EVGetModel(v_p), EVHitGraphics());
          ERptPrompt(ELIXIR_PROMPT_CLASS,
                     MESSAGE_SEL2, elixir_default_prompts[MESSAGE_SEL2]);
				} else
          ERptPrompt(ELIXIR_PROMPT_CLASS,
                     MESSAGE_SEL1, elixir_default_prompts[MESSAGE_SEL1]);
      } else {
				DestroyFixedSizeAperture(v_p);
				EVUnhiliteSelectionList(v_p);
				EVClearSelection();
				selection_started = NO;
				if (select_action_func.reset_func_p != NULL)
					(*select_action_func.reset_func_p)(v_p, select_action_func.data_p);
				else
					EMUninstallHandler(EVGetModel(v_p)); /* EXIT POINT */
      }
    }
    break;
  }
}



void 
EVSelectGraphicsMNotifyHandler(Widget w, EView *v_p,
			      XMotionEvent *event)
{
  WCRec wcp;
  VC2DRec p1, p2;
  VCRec vp;
      
  switch(sel_data.action) {
  case FENCING_SELECTION:
    EVDCtoWC(v_p, event->x, event->y, &(sel_data.last_point));
    XorDrawRectVC(v_p, sel_data.rubber_band_echo);
    EVWCtoVC(v_p, &(sel_data.origin_at_start), &vp);
    p1.u = vp.u,       p1.v = vp.v;
    EVWCtoVC(v_p, &(sel_data.last_point), &vp);
    p2.u = vp.u,       p2.v = vp.v;
    XorDrawRectVC(v_p, ModifyRectVC(sel_data.rubber_band_echo, &p1, &p2));
    break;
  case FIXED_SIZE_APERTURE_SELECTION:
    EVDCtoWC(v_p, event->x, event->y, &wcp);
    if (!selection_started) 
      EVInitSelection(v_p, &wcp);
/*    EVDCtoWC(v_p, event->x, event->y, &wcp); */
    ModifyFixedSizeAperture(v_p, &wcp);
    break;
  default:
    EVDCtoWC(v_p, event->x, event->y, &wcp);
    if (!selection_started) 
      EVInitSelection(v_p, &wcp);
    break;
  }
}



BOOLEAN 
EVIsPointerIn(void)
{
  return pointer_in;
}




static void 
EVResizeEH(Widget w, XtPointer p, XEvent *event, Boolean *ctd)
{
  EView *v_p = (EView *)p;

  AllocBackBuffer(v_p);
  EVForceRedisplay(v_p);
  *ctd = TRUE;
}



void 
EVHandleKeyPress(EView *v_p, XKeyEvent *event,
			  KeySym ksym, char *string_val)
{
  if        (TOP_SYMB(ksym)) {
    EVSetViewOrientation(v_p, VIEW_ORIENT_TOP); 
  } else if (BOTTOM_SYMB(ksym)) {
    EVSetViewOrientation(v_p, VIEW_ORIENT_BOTTOM);
  } else if (RIGHT_SYMB(ksym)) {
    EVSetViewOrientation(v_p, VIEW_ORIENT_RIGHT);
  } else if (LEFT_SYMB(ksym)) {
    EVSetViewOrientation(v_p, VIEW_ORIENT_LEFT);
  } else if (FRONT_SYMB(ksym)) {
    EVSetViewOrientation(v_p, VIEW_ORIENT_FRONT);
  } else if (BACK_SYMB(ksym)) {
    EVSetViewOrientation(v_p, VIEW_ORIENT_BACK);
  } else if (ISO_SYMB(ksym)) {
    EVSetViewOrientation(v_p, VIEW_ORIENT_ISO);
  } else {
    return;
  }
	EVForceRedisplay(v_p);
}



void 
EVHandleKeyRelease(EView *v_p, XKeyEvent *event,
			KeySym ksym, char *string_val)
{
}



Widget 
EVFreeView(EView *v_p)
{
  if (v_p->was_destroyed) {
    fprintf(stderr, "EVFreeView: Trying it second time\n");
    return NULL;
  }
  
  XFreeGC(XtDisplay(v_p->view_widget), v_p->writableGC);
  XFreeGC(XtDisplay(v_p->view_widget), v_p->eraseGC);

  XtReleaseGC(v_p->view_widget, v_p->defaultCopyGC);
  XtReleaseGC(v_p->view_widget, v_p->constrCopyGC);
  XtReleaseGC(v_p->view_widget, v_p->constrXORGC);
  XtReleaseGC(v_p->view_widget, v_p->constrEraseGC);
  XtReleaseGC(v_p->view_widget, v_p->defaultEraseGC);
  XtReleaseGC(v_p->view_widget, v_p->defaultXORGC);
  XtReleaseGC(v_p->view_widget, v_p->gridGC);
  XtReleaseGC(v_p->view_widget, v_p->hiliteGC); 
  XtReleaseGC(v_p->view_widget, v_p->erasehiliteGC);

  XFreeCursor(XtDisplay(v_p->view_widget), v_p->cursor);
  
  if (v_p->visible_GO != NULL)
    free_list(v_p->visible_GO, NOT_NODES);
  if (v_p->invisible_GO != NULL)
    free_list(v_p->invisible_GO, NOT_NODES);

  EMNotifyDestroyView(v_p->model_p, v_p);
  free_node(v_p);

  v_p->was_destroyed = YES;
  
  return v_p->view_widget;
}

void 
EVSetAttribMask(unsigned long mask)
{
  sel_data.attrib_val_mask = mask;
}
 
unsigned long
EVGetAttribMask(void)
{
  return sel_data.attrib_val_mask;
}


void 
EVSetPickableMask(unsigned long mask)
{
  sel_data.pickable_entity_mask = mask;
}
 
unsigned long
EVGetPickableMask(void)
{
  return sel_data.pickable_entity_mask;
}


void 
EVSetTakeAsNOT(BOOLEAN flag)
{
  sel_data.take_as_NOT = flag;
}
