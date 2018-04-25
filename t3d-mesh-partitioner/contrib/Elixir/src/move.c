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


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <stdlib.h>
#include <math.h>

#include "Esimple.h"
#include "Egecmds.h"
#include "Ege.h"
#include "Egecb.h"
#include "Emove.h"


/* Local variables of the graphics-repositioning routines */


static GraphicObj *echo = NULL;
static BOOLEAN move_started = NO; 
static WCRec start, end, delta, axial;
static MoveMode mode = MOVE_COPY;
static LIST glist = NULL;
static float rot_ang = 0;

void translate_resume(EView *v_p, caddr_t);
void translate_start(EView *v_p, caddr_t data);
void scale_resume(EView *v_p, caddr_t);
void scale_start(EView *v_p, caddr_t data);
void rotate_start_by_point(EView *v_p, caddr_t data);
void rotate_start_by_angle(EView *v_p, caddr_t data);
void rotate_resume(EView *v_p, caddr_t data);
void init_translate(void);
void init_scale(void);
void init_rotate_by_angle(void);
void init_rotate_by_point(void);
void init_mirror_center(void);
void init_mirror_plane(void);
void mirror_start_center(EView *v_p, caddr_t data);
void mirror_start_plane(EView *v_p, caddr_t data);
void mirror_resume(EView *v_p, caddr_t data);
float compute_rot_ang(WCRec *start, WCRec *end, WCRec *axis1, WCRec *axis2);
static GraphicObj *create_echo(EView *v_p);
static void create_glist(EView *v_p);


/* ======================================================================== */
  
static WCRec *minus(WCRec *p)
{
  static WCRec tmp;

  tmp.x = -p->x;
  tmp.y = -p->y;
  tmp.z = -p->z;
  return &tmp;
}

/* ======================================================================== */

static int 
set_view(NODE data, NODE v_p)            /* added by dr to prevent blinking */
{
	EVStoreRenderMode((EView *)v_p);
	EVSetRenderMode((EView *)v_p, WIRE_RENDERING);
	return(1);
}

	
static int 
reset_view(NODE data, NODE v_p)          /* added by dr to prevent blinking */
{
	EVRestoreRenderMode((EView *)v_p);
	EVForceRedisplay((EView *)v_p);
	return(1);
}



static void
init_reset(EView *v_p, caddr_t data_p);
static void
init_reset(EView *v_p, caddr_t data_p)
{
  EMUninstallHandler(EVGetModel(v_p));
	EMDispatchToDependentViews(ESIModel(), reset_view, NULL);   /* added by dr to prevent blinking */
}

/* ======================================================================== */
  
void init_translate(void)
{
	if(move_started == NO)
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  move_started = NO;
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		   NULL, NULL,
		   NULL, NULL,
		   translate_resume, NULL,
			 init_reset, NULL);                                     /* added by dr to prevent blinking */

	/* alternatively init_reset may be passed as last argument to EVSetSelectActionFunction */

  EVSetSelectActionFunction(translate_start, NULL, NULL);
	ERptStatusMessage(MOVE_STATUS_CLASS, 3, MESSAGE_TR3);
}

void init_scale(void)
{
	if(move_started == NO)
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  move_started = NO;
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		   NULL, NULL,
		   NULL, NULL,
		   scale_resume, NULL,
		   init_reset, NULL);                                     /* added by dr to prevent blinking */
  EVSetSelectActionFunction(scale_start, NULL, NULL);
  ERptStatusMessage(MOVE_STATUS_CLASS, 3, MESSAGE_SC2);
}

/* ======================================================================== */
  
void 
init_rotate_by_point(void)
{
	if(move_started == NO)
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  move_started = NO;
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      rotate_resume, NULL,
		      init_reset, NULL);                                     /* added by dr to prevent blinking */
  EVSetSelectActionFunction(rotate_start_by_point, NULL, NULL);
  ERptStatusMessage(MOVE_STATUS_CLASS, 7, MESSAGE_RO3);
}

/* ======================================================================== */
  
void 
init_mirror_center(void)
{
	if(move_started == NO)
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  move_started = NO;
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      mirror_resume, NULL,
		      init_reset, NULL);                                     /* added by dr to prevent blinking */
  EVSetSelectActionFunction(mirror_start_center, NULL, NULL);
  ERptStatusMessage(MOVE_STATUS_CLASS, 14, MESSAGE_MC3);
}

/* ======================================================================== */
  
void 
init_rotate_by_angle(void)
{
	if(move_started == NO)
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  move_started = NO;
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      rotate_resume, NULL,
		      init_reset, NULL);                                     /* added by dr to prevent blinking */
  EVSetSelectActionFunction(rotate_start_by_angle, NULL, NULL);
  ERptStatusMessage(MOVE_STATUS_CLASS, 7, MESSAGE_RO3);
}

#if 0
void print_echo(void)
{
  LIST l;
  GraphicObj *p;

  if (echo != NULL) {
    l = GgroupGObjects(echo);
    p = (GraphicObj *)get_list_next(l, NULL);
    while (p != NULL) {
      printf("%s\n", EGStringGraphicRepresentation(p));
      p = (GraphicObj *)get_list_next(l, p);
    }
  }
}
#endif
  
/* ======================================================================== */
  
void 
TranslateHandler(Widget w, EView *v_p, XEvent *event)
{
  WCRec curr;
  
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      if (move_started == YES) {
				ERptStatusMessage (MOVE_STATUS_CLASS, 4, MESSAGE_TR4);
				EVInputPoint(v_p, event, &end);
				EVSetLastInputPoint(&end);
				delta.x = end.x - start.x;
				delta.y = end.y - start.y;
				delta.z = end.z - start.z;
				CmdFlipState(AGECreateTranslateCommand(glist, mode, &delta));
				if (mode == MOVE_ORIGINAL) {
					start.x = end.x; start.y = end.y; start.z = end.z;
				} 
				move_started = YES;
      } else {
				EVInputPoint(v_p, event, &start);
				EVSetLastInputPoint(&start);
				end.x = start.x; end.y = start.y; end.z = start.z;
				EGXORDrawGraphics(v_p, (echo=create_echo(v_p)));
				move_started = YES;
      }
      ERptPrompt(MOVE_PROMPT_CLASS, 1, MESSAGE_TR1);
      break;
    case Button3:
      if (move_started == YES) {
				ERptStatusMessage (MOVE_STATUS_CLASS, 4, MESSAGE_TR4);
				if (echo != NULL) 
					EGXORDrawGraphics(v_p, echo);
				if (echo != NULL) 
					EGDeepDestroyGraphics(echo);
				echo = NULL;
				init_translate();        
      } else { /* this code is reachable only if B3 is pressed just after selection ???
									then it seems that init_reset is not called and therefore reset_view 
									is called explicitly - dr */
				if (echo != NULL) 
					EGDeepDestroyGraphics(echo);
				echo = NULL;
				ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
//				ERptStatusMessage (MOVE_STATUS_CLASS, 4, MESSAGE_TR4);
				EVClearSelection();
				EMDispatchToDependentViews(ESIModel(), reset_view, NULL);  /* by dr */
				EMUninstallHandler(EVGetModel(v_p));
      }
      break;
    }
    break;
  case MotionNotify:
    EVInputPoint(v_p, event, &curr);
    ERptTrackPointer(&curr);
    if (move_started == YES) {
			ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
      EGXORDrawGraphics(v_p, echo);
      EGTranslateGraphic(echo, minus(&end));
      end.x = curr.x; end.y = curr.y; end.z = curr.z;
      EGTranslateGraphic(echo, &end);
      EGXORDrawGraphics(v_p, echo); 
      break;
    } else {
      ERptPrompt(MOVE_PROMPT_CLASS, 2, MESSAGE_TR2);
    }
    break;
  case LeaveNotify:
  case EnterNotify:
    if (move_started == YES) {
      EGXORDrawGraphics(v_p, echo);
    }
    break;
  case ClientMessage:
    ERptPrompt(MOVE_PROMPT_CLASS, 2, MESSAGE_TR2);
    break;  
  }
}

#define LARGE_NUMBER 1.e30
 /* ======================================================================= */

static GraphicObj *create_echo(EView *v_p)
{
  GraphicObj *g_p, *echo_return;
  LIST echo_list = NULL;

  create_glist(v_p);

  if (echo != NULL) 
    EGDeepDestroyGraphics(echo);
  echo = NULL;
  echo_list = make_list();
  g_p = (GraphicObj *)get_list_next(glist, NULL);
  while (g_p != NULL) {
    add_to_tail(echo_list, EGDeepCopyGraphics(g_p));
    g_p = (GraphicObj *)get_list_next(glist, g_p);
  }
  echo_return = CreateGgroup(echo_list);
  free_list(echo_list, NOT_NODES);
  return echo_return;
}
 /* ======================================================================= */

static void create_glist(EView *v_p)
{
  GraphicObj *g_p;

  if (glist != NULL)
    flush_list(glist, NOT_NODES);
  else
    glist = make_list();

  g_p = EVGetFromSelectionList();
  while (g_p != NULL) {
    add_to_tail(glist, g_p);
    g_p = EVGetFromSelectionList();
  }
}

 /* ======================================================================= */
  
void translate_resume(EView *v_p, caddr_t data)
{
  if (echo != NULL) {
    if (move_started == YES) {
      echo = EGTranslateGraphic(echo, minus(&end));
      ERptPrompt(MOVE_PROMPT_CLASS, 1, MESSAGE_TR1);
    }
  }
}

void 
scale_resume(EView *v_p, caddr_t data)
{
  ERptPrompt(MOVE_PROMPT_CLASS, 1, MESSAGE_SC1);
}

 /* ======================================================================= */
  
void mirror_resume(EView *v_p, caddr_t data)
{
}

 /* ======================================================================= */
  
void translate_start(EView *v_p, caddr_t data)
{
  EMInstallHandler(ESIModel(), TranslateHandler,
		      NULL, NULL,
		      NULL, NULL,
		      translate_resume, NULL,
		      NULL, NULL);
  ERptPrompt(MOVE_PROMPT_CLASS, 2, MESSAGE_TR2);
}
 
void 
scale_start(EView *v_p, caddr_t data)
{
  EMInstallHandler(ESIModel(), ScaleHandler,
		      NULL, NULL,
		      NULL, NULL,
		      scale_resume, NULL,
		      NULL, NULL);
  ERptPrompt(MOVE_PROMPT_CLASS, 2, MESSAGE_SC1);
}

 /* ======================================================================= */
  
void 
mirror_start_center(EView *v_p, caddr_t data)
{
  create_glist(v_p);
  EMInstallHandler(ESIModel(), MirrorCenterHandler,
		      NULL, NULL,
		      NULL, NULL,
		      mirror_resume, NULL,
		      NULL, NULL);
  ERptPrompt(MOVE_PROMPT_CLASS, 13, MESSAGE_MC1);  
}

/* ======================================================================== */
  
void 
translate_orig_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_ORIGINAL;
  init_translate();
}

/* ======================================================================== */
  
void 
translate_copy_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_COPY;
  init_translate();
}
 
  
void 
scale_orig_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_ORIGINAL;
  init_scale();
}

/* ======================================================================== */
  
void 
scale_copy_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_COPY;
  init_scale();
}
 
/* ======================================================================== */
  
void rotate_by_angle_orig_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_ORIGINAL;
  rot_ang = EASValsGetRotationAngle();
  init_rotate_by_angle();
}

/* ======================================================================== */
  
void rotate_by_angle_copy_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_COPY;
  rot_ang = EASValsGetRotationAngle();
  init_rotate_by_angle();
}
  
/* ======================================================================== */
  
void rotate_by_point_orig_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_ORIGINAL;
  init_rotate_by_point();
}

/* ======================================================================== */
  
void rotate_by_point_copy_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  mode = MOVE_COPY;
  init_rotate_by_point();
}
  
/* ======================================================================== */
  
void RotateByActiveAngleHandler(Widget w, EView *v_p, XEvent *event)
{
  WCRec curr;
  
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      if (move_started == YES) {
				EVInputPoint(v_p, event, &end);
				EVSetLastInputPoint(&end);
				axial.x = end.x - start.x;
				axial.y = end.y - start.y;
				axial.z = end.z - start.z;
				if (NormalizeVect3(&axial)) {
					axial.x *= rot_ang; axial.y *= rot_ang; axial.z *= rot_ang;
					CmdFlipState(AGECreateRotateCommand(glist, mode, &start, &axial));
					if (mode == MOVE_COPY)
						rot_ang += EASValsGetRotationAngle();
				}
				move_started = YES;
      } else {
				EVInputPoint(v_p, event, &start);
				EVSetLastInputPoint(&start);
				end.x = start.x; end.y = start.y; end.z = start.z + 1;
				move_started = YES;
      }
      ERptPrompt(MOVE_PROMPT_CLASS, 5, MESSAGE_RO1);
      break;
    case Button3:
      if (move_started == YES) {
				echo = NULL;
				init_rotate_by_angle();
      } else {
				ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
//				ERptStatusMessage(MOVE_STATUS_CLASS, 8, MESSAGE_RO4);
				EVClearSelection();
				EMDispatchToDependentViews(ESIModel(), reset_view, NULL);  /* by dr */
				EMUninstallHandler(EVGetModel(v_p));
      }
      break;
    }
    break;
  case MotionNotify:
    EVInputPoint(v_p, event, &curr);
    ERptTrackPointer(&curr);
    if (move_started){
			ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
      ERptPrompt(MOVE_PROMPT_CLASS, 5, MESSAGE_RO1);
		}
    else
      ERptPrompt(MOVE_PROMPT_CLASS, 6, MESSAGE_RO2);
    break;
  case LeaveNotify:
  case EnterNotify:
    break;
  case ClientMessage:
    ERptPrompt(MOVE_PROMPT_CLASS, 6, MESSAGE_RO2); 
    break;   
  }
}

 
static WCRec axis1, axis2;
static int rotate_point = 0;

/* ======================================================================== */
  
void RotateInteractiveHandler(Widget w, EView *v_p, XEvent *event)
{
  WCRec curr;
  
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      if (rotate_point == 3) {
				EVInputPoint(v_p, event, &end);
				EVSetLastInputPoint(&end);
				rot_ang = compute_rot_ang(&start, &end, &axis1, &axis2);
				axial.x = axis2.x - axis1.x;
				axial.y = axis2.y - axis1.y;
				axial.z = axis2.z - axis1.z;
				if (NormalizeVect3(&axial)) {
					axial.x *= rot_ang; axial.y *= rot_ang; axial.z *= rot_ang;
					CmdFlipState(AGECreateRotateCommand(glist, mode, &axis1, &axial));
					if (mode == MOVE_ORIGINAL) {
						start.x = end.x, start.y = end.y, start.z = end.z;
						EGRotateGraphic(echo, &axis1, &axial);
					}
				}
				rot_ang = 0;
      } else if (rotate_point == 2) {
				EVInputPoint(v_p, event, &start);
				EVSetLastInputPoint(&start);
				end.x = start.x, end.y = start.y, end.z = start.z;
				if (echo == NULL)
					EGXORDrawGraphics(v_p, (echo = create_echo(v_p)));
				rotate_point = 3;
        ERptPrompt(MOVE_PROMPT_CLASS, 12, MESSAGE_RO8);
      } else if (rotate_point == 1) {
				EVInputPoint(v_p, event, &axis2);
				EVSetLastInputPoint(&axis2);
				axial.x = axis2.x - axis1.x;
				axial.y = axis2.y - axis1.y;
				axial.z = axis2.z - axis1.z;
				NormalizeVect3(&axial);
				rotate_point = 2;
        ERptPrompt(MOVE_PROMPT_CLASS, 11, MESSAGE_RO7);
      } else if (rotate_point == 0) {
				EVInputPoint(v_p, event, &axis1);
				EVSetLastInputPoint(&axis1);
				axis2.x = axis1.x; axis2.y = axis1.y; axis2.z = axis1.z + 1;
				rotate_point = 1;
        ERptPrompt(MOVE_PROMPT_CLASS, 10, MESSAGE_RO6);
      }
      break;
    case Button3:
      if (rotate_point > 0) {
				if (echo != NULL) {
					axial.x=axis2.x-axis1.x;
					axial.y=axis2.y-axis1.y;
					axial.z=axis2.z-axis1.z;
					if (NormalizeVect3(&axial)) {
						axial.x *= rot_ang; axial.y *= rot_ang; axial.z *= rot_ang; 
						EGRotateGraphic(echo, &axis1, &axial);
						EGXORDrawGraphics(v_p, echo);
						EGRotateGraphic(echo, &axis1, minus(&axial));
					}
				}
				rotate_point--;
				if (rotate_point == 2)
          ERptPrompt(MOVE_PROMPT_CLASS, 11, MESSAGE_RO7);
				else if (rotate_point == 1)
          ERptPrompt(MOVE_PROMPT_CLASS, 10, MESSAGE_RO6);
				else if (rotate_point == 0){
					if (echo != NULL) 
						EGDeepDestroyGraphics(echo);
					echo = NULL;
					init_rotate_by_point();
				}
      } else {
				if (echo != NULL) 
					EGDeepDestroyGraphics(echo);
				echo = NULL;
				ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
//				ERptStatusMessage(MOVE_STATUS_CLASS, 8, MESSAGE_RO4);
				EVClearSelection();
				EMUninstallHandler(EVGetModel(v_p));
      }
      break;
    }
    break;
  case MotionNotify:
    EVInputPoint(v_p, event, &curr);
    ERptTrackPointer(&curr);
    if (rotate_point > 2) {
      FPNum pra = rot_ang;
      
      if (rot_ang != 0) {
				axial.x=axis2.x-axis1.x;
				axial.y=axis2.y-axis1.y;
				axial.z=axis2.z-axis1.z;
				if (NormalizeVect3(&axial)) {
					axial.x *= rot_ang; axial.y *= rot_ang; axial.z *= rot_ang; 
					EGRotateGraphic(echo, &axis1, &axial);
					EGXORDrawGraphics(v_p, echo);
					EGRotateGraphic(echo, &axis1, minus(&axial));
				}
      }
      end.x = curr.x; end.y = curr.y; end.z = curr.z;
      rot_ang = compute_rot_ang(&start, &end, &axis1, &axis2);
      if (rot_ang != pra) {
				axial.x=axis2.x-axis1.x;
				axial.y=axis2.y-axis1.y;
				axial.z=axis2.z-axis1.z;
				if (NormalizeVect3(&axial)) {
					axial.x *= rot_ang; axial.y *= rot_ang; axial.z *= rot_ang; 
					EGRotateGraphic(echo, &axis1, &axial);
					EGXORDrawGraphics(v_p, echo);
					EGRotateGraphic(echo, &axis1, minus(&axial));
				}
      }
    }
    if (rotate_point == 3)
      ERptPrompt(MOVE_STATUS_CLASS, 12, MESSAGE_RO8);
    else if (rotate_point == 2)
      ERptPrompt(MOVE_STATUS_CLASS, 11, MESSAGE_RO7);
    else if (rotate_point == 1)
      ERptPrompt(MOVE_STATUS_CLASS, 10, MESSAGE_RO6);
    else if (rotate_point == 0)
      ERptPrompt(MOVE_STATUS_CLASS,  9, MESSAGE_RO5);
    break;
  case LeaveNotify:
  case EnterNotify:
    if (rotate_point && echo != NULL) 
      EGXORDrawGraphics(v_p, echo);
    break;
  case ClientMessage:
    ERptPrompt(MOVE_PROMPT_CLASS, 6, MESSAGE_RO5); 
    break;  
  }
}


 /* ======================================================================= */
  
void rotate_resume(EView *v_p, caddr_t data)
{
  if (echo != NULL) {
    if (move_started == YES) {
      echo = EGRotateGraphic(echo, &start, minus(&end));
      ERptPrompt(MOVE_PROMPT_CLASS, 5, MESSAGE_RO1);
    }
  }
}

 /* ======================================================================= */
  
void rotate_start_by_point(EView *v_p, caddr_t data)
{
  rotate_point = 0;
  EMInstallHandler(ESIModel(), RotateInteractiveHandler,
		      NULL, NULL,
		      NULL, NULL,
		      rotate_resume, NULL,
		      NULL, NULL);
  ERptPrompt(MOVE_PROMPT_CLASS, 6, MESSAGE_RO2);
}

 /* ======================================================================= */
  
void rotate_start_by_angle(EView *v_p, caddr_t data)
{
  rotate_point = 0;
  move_started = NO;
  create_glist(v_p);
  EMInstallHandler(ESIModel(), RotateByActiveAngleHandler,
		      NULL, NULL,
		      NULL, NULL,
		      rotate_resume, NULL,
		      NULL, NULL);
  ERptPrompt(MOVE_PROMPT_CLASS, 6, MESSAGE_RO2);
}

 /* ======================================================================= */
  
#define VERY_SMALL_NUMBER 1.e-7

float compute_rot_ang(WCRec *start, WCRec *end, WCRec *axis1, WCRec *axis2)
{
  WCRec v1, v2, axial, cp;
  FPNum dp, tmp, angle;

  v1.x = start->x - axis1->x;
  v1.y = start->y - axis1->y;
  v1.z = start->z - axis1->z;
  v2.x = end->x - axis1->x;
  v2.y = end->y - axis1->y;
  v2.z = end->z - axis1->z;
  axial.x = axis2->x - axis1->x;
  axial.y = axis2->y - axis1->y;
  axial.z = axis2->z - axis1->z;
  if (!NormalizeVect3(&axial))
    return 0;
  dp = DotProd3(&v1, &axial);
  v1.x -= dp * axial.x;
  v1.y -= dp * axial.y;
  v1.z -= dp * axial.z;
  if (!NormalizeVect3(&v1))
    return 0;
  dp = DotProd3(&v2, &axial);
  v2.x -= dp * axial.x;
  v2.y -= dp * axial.y;
  v2.z -= dp * axial.z;
  if (!NormalizeVect3(&v2))
    return 0;
  CrossProd3(&cp, &v1, &v2);
  dp    = DotProd3(&axial, &cp);
  angle = asin(dp);
  tmp   = DotProd3(&v1, &v2);
  if (fabs(tmp) < VERY_SMALL_NUMBER)
    if (dp < 0)
      angle = PI*1.5;
    else
      angle = PI*0.5;
  else if (tmp < 0)
    angle = PI - angle;
  return angle;
}
  
/* ======================================================================== */
  
void 
MirrorCenterHandler(Widget w, EView *v_p, XEvent *event)
{
  WCRec curr;
  
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      EVInputPoint(v_p, event, &end);
      EVSetLastInputPoint(&end);
      CmdFlipState(AGECreateMirrorCenterCommand(glist, mode, &end));
      ERptPrompt(MOVE_PROMPT_CLASS, 13, MESSAGE_MC1);
      move_started = YES;
      break;
    case Button3:
      if (move_started) {
				init_mirror_center();
      } else {
				ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
//				ERptStatusMessage(MOVE_STATUS_CLASS, 15, MESSAGE_MC4);
				EVClearSelection();
				EMDispatchToDependentViews(ESIModel(), reset_view, NULL);  /* by dr */
				EMUninstallHandler(EVGetModel(v_p));
      }
      break;
    }
    break;
  case MotionNotify:
    EVInputPoint(v_p, event, &curr);
    ERptTrackPointer(&curr);
    if (move_started == YES)
			ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
		else
      ERptPrompt(MOVE_PROMPT_CLASS, 13, MESSAGE_MC1);
    break;
  case LeaveNotify:
  case EnterNotify:
    break;
  case ClientMessage:
    ERptPrompt(MOVE_PROMPT_CLASS, 6, MESSAGE_MC1); 
    break;  
  }
}

/* ======================================================================== */
  
void 
mirror_center_orig_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  mode = MOVE_ORIGINAL;
  init_mirror_center();
}

/* ======================================================================== */
  
void 
mirror_center_copy_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  mode = MOVE_COPY;
  init_mirror_center();
}
 

  
static int num_of_points_on_plane = 0;
static WCRec points_on_plane[3];

void 
MirrorPlaneHandler(Widget w, EView *v_p, XEvent *event)
{
  WCRec curr;
  
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      EVInputPoint(v_p, event, &end);
      EVSetLastInputPoint(&end);
      points_on_plane[num_of_points_on_plane].x = end.x;
      points_on_plane[num_of_points_on_plane].y = end.y;
      points_on_plane[num_of_points_on_plane].z = end.z;
      num_of_points_on_plane++;
      if (num_of_points_on_plane == 3) {
        CmdFlipState(AGECreateMirrorPlaneCommand(glist, mode,
                                                 points_on_plane));
        ERptPrompt(MOVE_PROMPT_CLASS, 13, MESSAGE_MP1);
        num_of_points_on_plane = 0;
      }
      move_started = YES;
      break;
    case Button3:
      if (move_started) {
				init_mirror_plane();
      } else {
				ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
//				ERptStatusMessage(MOVE_STATUS_CLASS, 15, MESSAGE_MC4);
				EVClearSelection();
				EMDispatchToDependentViews(ESIModel(), reset_view, NULL);  /* by dr */
				EMUninstallHandler(EVGetModel(v_p));
      }
      break;
    }
    break;
  case MotionNotify:
    EVInputPoint(v_p, event, &curr);
    ERptTrackPointer(&curr);
    if (num_of_points_on_plane == 2)
      ERptPrompt(MOVE_STATUS_CLASS, 18, MESSAGE_MP2);
    else if (num_of_points_on_plane == 1)
      ERptPrompt(MOVE_STATUS_CLASS, 17, MESSAGE_MP1);
    else if (num_of_points_on_plane == 0)
      ERptPrompt(MOVE_STATUS_CLASS, 16, MESSAGE_MP0);
		/*
    if (move_started)
      ERptPrompt(MOVE_PROMPT_CLASS, 13, MESSAGE_MP1);
		*/
    break;
  case LeaveNotify:
  case EnterNotify:
    break;
  case ClientMessage:
    ERptPrompt(MOVE_PROMPT_CLASS, 6, MESSAGE_MP1); 
    break;  
  }
}


 
void 
init_mirror_plane(void)
{
	if(move_started == NO)
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  move_started = NO;
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
                   NULL, NULL,
                   NULL, NULL,
                   mirror_resume, NULL,
                   init_reset, NULL);                         /* added by dr to prevent blinking */
  EVSetSelectActionFunction(mirror_start_plane, NULL, NULL);
  ERptStatusMessage(MOVE_STATUS_CLASS, 14,  MESSAGE_MP3);
}


 
void 
mirror_start_plane(EView *v_p, caddr_t data)
{
  int i;
  
  create_glist(v_p);
  
  num_of_points_on_plane = 0;
  for (i = 0; i < 3; i++) 
    points_on_plane[i].x = points_on_plane[i].y = points_on_plane[i].z = 0;
  
  EMInstallHandler(ESIModel(), MirrorPlaneHandler,
                   NULL, NULL,
                   NULL, NULL,
                   mirror_resume, NULL,
                   NULL, NULL);
  ERptPrompt(MOVE_PROMPT_CLASS, 13, MESSAGE_MP1);  
}


/* ======================================================================== */
  
void 
mirror_plane_orig_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  mode = MOVE_ORIGINAL;
  init_mirror_plane();
}

/* ======================================================================== */
  
void 
mirror_plane_copy_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  mode = MOVE_COPY;
  init_mirror_plane();
}
 



void 
ScaleHandler(Widget w, EView *v_p, XEvent *event)
{
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      EVInputPoint(v_p, event, &end);
      EVSetLastInputPoint(&end);
      CmdFlipState(AGECreateScaleCommand(glist, mode, &end));
      if (mode == MOVE_ORIGINAL) {
        start.x = end.x; start.y = end.y; start.z = end.z;
      }
      move_started = YES;
      ERptPrompt(MOVE_PROMPT_CLASS, 1, MESSAGE_SC2);
      break;
    case Button3:
      if (move_started) {
				init_scale();
      } else {
				ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "");           /* by dr */
//				ERptStatusMessage (MOVE_STATUS_CLASS, 4, MESSAGE_SC3);
				EVClearSelection();
				EMDispatchToDependentViews(ESIModel(), reset_view, NULL);  /* by dr */
				EMUninstallHandler(EVGetModel(v_p));
      }
      break;
    }
    break;
  case ClientMessage:
    ERptPrompt(MOVE_PROMPT_CLASS, 2, MESSAGE_SC2);
    break;  
  }
}
