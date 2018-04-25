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
#include "Ecomptblt.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Emodel.h"
#include "Eactset.h"
#include "Ereprts.h"
#include "Ege.h"
#include "Egecmds.h"
#include "Ecreate.h"
#include "Esimple.h"

 
/* Local variables of the graphics-creation routines */

static GraphicObj *echo = NULL;
 
#define MAX_POINTS_IN_BY_N_HANDLER 17

static WCRec points[MAX_POINTS_IN_BY_N_HANDLER];
static int current_point;

/*

modelcreateproc     ... model procedure to be called to create the graphics
createechographics  ... procedure to call to create the echo graphics
modifyechographics  ... procedure to call to modify the echo graphics
prompts:            ... prompts to put up

*/

static struct {
  int                    N;
  int                    final_for_echo;
  ModelCreateNProcP      model_create_proc;
  CreateNEchoProcP       create_echo_graphics;
  ModifyNEchoProcP       modify_echo_graphics;
  char                  *prompts[4];
} CBNPHD;

  
void create_resume(EView *v_p, caddr_t);

void draw_line_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);
void draw_triangle_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);
void draw_quad_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);
void draw_marker_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);
void draw_cbezc_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);
void draw_bcbezs_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);
void draw_anntext3d_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);

/* Wrappers to include attributes in the echo such as font, marker type etc. */

static GraphicObj *Create_Ann_Text_3D_Wrapper(WCRec *p);
static GraphicObj *Modify_Ann_Text_3D_Wrapper(GraphicObj *gp, WCRec *p);

static GraphicObj *Create_Marker_3D_Wrapper(WCRec *p);
static GraphicObj *Modify_Marker_3D_Wrapper(GraphicObj *gp, WCRec *p);

	
 /* ======================================================================= */
  		      
void CreateByNPointsHandler(Widget w, EView *v_p, XEvent *event)
{
  int i;
  WCRec currp;
  
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      if (current_point == CBNPHD.N-1) {
	EVInputPoint(v_p, event, &(points[current_point]));
	EVSetLastInputPoint(&(points[current_point]));
	if (EVIsPointerIn() && echo != NULL)
	  EGXORDrawGraphics(v_p, echo);
	for (i = current_point+1; i < MAX_POINTS_IN_BY_N_HANDLER-1; i++)
	  points[i] = points[current_point];
	(*CBNPHD.model_create_proc)(points);
	points[0] = points[current_point];
	for (i = 1; i < MAX_POINTS_IN_BY_N_HANDLER-1; i++)
	  points[i] = points[0];
	if (EVIsPointerIn() && current_point == 0 && echo != NULL)
	  EGXORDrawGraphics(v_p, echo);
	current_point = min(1, CBNPHD.N-1);
	if (echo != NULL) 
	  (*CBNPHD.modify_echo_graphics)(echo, points);
      } else {
	EVInputPoint(v_p, event, &(points[current_point]));
	EVSetLastInputPoint(&(points[current_point]));
	for (i = current_point+1; i < MAX_POINTS_IN_BY_N_HANDLER-1; i++)
	  points[i] = points[current_point]; 
	current_point++;
      }
      break;
    case Button3:
      if (--current_point <= 0) {
	if (echo != NULL) {
	  EGXORDrawGraphics(v_p, echo);
	  EGDeepDestroyGraphics(echo), echo = NULL;
	}
	if (current_point < 0) {
	  ERptPrompt(CREATE_PROMPT_CLASS, 3, CBNPHD.prompts[3]);
	  EMUninstallHandler(EVGetModel(v_p));
	}
      }
      break;
    } /* end switch(event->xbutton.button) */
    if (current_point == 0)
      ERptPrompt(CREATE_PROMPT_CLASS, 0, CBNPHD.prompts[0]);
    else if (current_point == CBNPHD.N-1)
      ERptPrompt(CREATE_PROMPT_CLASS, 2, CBNPHD.prompts[2]);
    else
      ERptPrompt(CREATE_PROMPT_CLASS, 1, CBNPHD.prompts[1]);
    break;
  case MotionNotify:
    EVInputPoint(v_p, event, &currp);
    ERptTrackPointer(&currp);
    if (current_point >= CBNPHD.final_for_echo) {
      if (echo != NULL)
	EGXORDrawGraphics(v_p, echo);
      points[current_point].x = currp.x;
      points[current_point].y = currp.y;
      points[current_point].z = currp.z;
      for (i = current_point+1; i < MAX_POINTS_IN_BY_N_HANDLER-1; i++)
	  points[i] = points[current_point]; 
      if (echo != NULL && current_point > CBNPHD.final_for_echo-1)
	EGXORDrawGraphics(v_p, (*CBNPHD.modify_echo_graphics)(echo, points));
      else
	EGXORDrawGraphics(v_p, (echo=(*CBNPHD.create_echo_graphics)(points)));
    }
    if (current_point == 0)
      ERptPrompt(CREATE_PROMPT_CLASS, 0, CBNPHD.prompts[0]);
    else if (current_point == CBNPHD.N-1)
      ERptPrompt(CREATE_PROMPT_CLASS, 2, CBNPHD.prompts[2]);
    else
      ERptPrompt(CREATE_PROMPT_CLASS, 1, CBNPHD.prompts[1]);
    break;
  case LeaveNotify:
  case EnterNotify:
    if (current_point >= CBNPHD.final_for_echo && echo != NULL) 
      EGXORDrawGraphics(v_p, echo);
    break;
  case ClientMessage:
    ERptPrompt(CREATE_PROMPT_CLASS, 0, CBNPHD.prompts[0]);
    break;
  }
}

 /* ======================================================================= */
  
void SetByNPointsHandlerData(ModelCreateNProcP m_p_p,
			     CreateNEchoProcP  c_e_p_p,
			     ModifyNEchoProcP  m_e_p_p,
			     int N,
			     int final_for_echo,
			     char *prompt1, char *prompt2,
			     char *prompt3, char *prompt4)
{
  if (echo != NULL)
    EGDeepDestroyGraphics(echo), echo = NULL;
  
  CBNPHD.model_create_proc    = m_p_p;
  CBNPHD.create_echo_graphics = c_e_p_p;
  CBNPHD.modify_echo_graphics = m_e_p_p;
  CBNPHD.N                    = N;
  CBNPHD.final_for_echo       = final_for_echo;
  CBNPHD.prompts[0]           = prompt1;
  CBNPHD.prompts[1]           = prompt2;
  CBNPHD.prompts[2]           = prompt3;
  CBNPHD.prompts[3]           = prompt4;
  
  current_point = 0;
}

 /* ======================================================================= */
  
void create_resume(EView *v_p, caddr_t data)
{
  if (echo != NULL) {
    if (current_point > 0) 
      echo = (*CBNPHD.modify_echo_graphics)(echo, points);
  }
}

 /* ======================================================================= */
  
void draw_line_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateLine3D, CreateLine3D,
                          EGModifyGraphicsGeometry,
			  2, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
                          MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL, 
		      create_resume, NULL,
		      NULL, NULL);
}

 /* ======================================================================= */
 
void draw_cbezc_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateCBezC3D,
			  CreateCBezC3D, EGModifyGraphicsGeometry,
			  4, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
                          MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}

 /* ======================================================================= */
  
void draw_bcbezs_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateBCBezS3D,
			  CreateBCBezS3D, EGModifyGraphicsGeometry,
			  16, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}


 /* ======================================================================= */

static GraphicObj *Create_Ann_Text_3D_Wrapper(WCRec *p)
{
  return CreateAnnText3D(p, EASValsGetText());
}

 /* ======================================================================= */

static GraphicObj *Modify_Ann_Text_3D_Wrapper(GraphicObj *gp, WCRec *p)
{
  EGWithMaskChangeAttributes(TEXT_STRING_MASK | FONT_MASK | COLOR_MASK, gp);
  return EGModifyGraphicsGeometry(gp, p);
}

 /* ======================================================================= */

static GraphicObj *Create_Marker_3D_Wrapper(WCRec *p)
{
  GraphicObj *g_p;
  g_p = CreateMarker3D(p);
  EGWithMaskChangeAttributes(MTYPE_MASK | MSIZE_MASK | COLOR_MASK, g_p);
  return g_p;
}

 /* ======================================================================= */

static GraphicObj *Modify_Marker_3D_Wrapper(GraphicObj *gp, WCRec *p)
{
  EGWithMaskChangeAttributes(MTYPE_MASK | MSIZE_MASK | COLOR_MASK, gp);
  return EGModifyGraphicsGeometry(gp, p);
}

 /* ======================================================================= */
  

void draw_anntext3d_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateAnnText3D,
			  Create_Ann_Text_3D_Wrapper,
			  Modify_Ann_Text_3D_Wrapper, 1, 0,
			  MESSAGE_CREATE_LOCATION,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}

 /* ======================================================================= */
  
void draw_marker_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateMarker3D, Create_Marker_3D_Wrapper,
			  Modify_Marker_3D_Wrapper,
			  1, 0,
                          MESSAGE_CREATE_LOCATION,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}


 /* ======================================================================= */
  
void draw_triangle_CB(Widget w, XtPointer client_data,
		    XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateTriangle3D,
			  CreateTriangle3D, EGModifyGraphicsGeometry, 3, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}


  
void 
draw_tetra_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateTetra,
			  CreateTetra, EGModifyGraphicsGeometry, 4, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}


void 
draw_hexah_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateHexahedron,
			  CreateHexahedron, EGModifyGraphicsGeometry, 8, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}


 /* ======================================================================= */
  
void 
draw_quad_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateQuad3D,
			  CreateQuad3D, EGModifyGraphicsGeometry, 4, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_NEXT_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      create_resume, NULL,
		      NULL, NULL);
}


void 
ggroup_create(EView *v_p, caddr_t data_p);

void 
create_ggroup_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  EVSetSelectActionFunction(ggroup_create, NULL, NULL);
  EMPushHandler(ESIModel(), EVSelectGraphicsHandler, NULL);
}

void 
ggroup_break(EView *v_p, caddr_t data_p);

void 
break_ggroup_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  EVSetSelectActionFunction(ggroup_break, NULL, NULL);
  EMPushHandler(ESIModel(), EVSelectGraphicsHandler, NULL);
}


void 
ggroup_create(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p;
  LIST          l;
  GraphicObj *gg_p;

  if (EVItemsInSelectionList() <= 0)
    return;

  l = make_list();
  while ((g_p = (GraphicObj *)EVGetFromSelectionList()) != NULL) 
    add_to_tail(l, g_p);
  gg_p = CreateGgroup(l);
  g_p = (GraphicObj *)get_list_next(l, NULL);
  while (g_p != (GraphicObj *)NULL) {
    EMDeleteGraphics(EVGetModel(v_p), g_p);
    g_p = (GraphicObj *)get_list_next(l, g_p);
  }
  free_list(l, NOT_NODES);
  EMAddGraphicsToModel(EVGetModel(v_p), gg_p);
  EMUninstallHandler(EVGetModel(v_p));
}



void 
ggroup_break(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p, *gg_p;
  LIST l;

  if (EVItemsInSelectionList() <= 0)
    return;

  while ((gg_p = (GraphicObj *)EVGetFromSelectionList()) != NULL) 
    if (EGGraphicType(gg_p) == EG_GGROUP) {
      l = GgroupGObjects(gg_p);
      EMDeleteGraphics(EVGetModel(v_p), gg_p);
      g_p = (GraphicObj *)get_from_head(l);
      while (g_p != (GraphicObj *)NULL) {
	EMAddGraphicsToModel(EVGetModel(v_p), g_p);
	g_p = (GraphicObj *)get_from_head(l);
      }
      EGDeepDestroyGraphics(gg_p);
    }
  EMUninstallHandler(EVGetModel(v_p));
}

 
void 
draw_raprism_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreateRAPrism, CreateRAPrism,
                          EGModifyGraphicsGeometry,
			  2, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
                          MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL, 
		      create_resume, NULL,
		      NULL, NULL);
}

 
void 
draw_pyramid_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  SetByNPointsHandlerData(AGECreatePyramid, CreatePyramid,
                          EGModifyGraphicsGeometry,
			  5, 1,
			  MESSAGE_CREATE_1ST_DONE,
			  MESSAGE_CREATE_FINAL_BKP,
			  MESSAGE_CREATE_FINAL_BKP,
                          MESSAGE_CREATE_DONE);
  EMInstallHandler(ESIModel(), CreateByNPointsHandler,
		      NULL, NULL,
		      NULL, NULL, 
		      create_resume, NULL,
		      NULL, NULL);
}





static void 
do_extrude_along_curve(EView *v_p, caddr_t data_p);

static void 
do_extrude_along_curve(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p1, *g_p2, *curve = NULL, *patch = NULL, *hex = NULL;
  BOOLEAN to_destroy_curve = NO;
  WCRec points[56];
  int npts;
  
  if (EVItemsInSelectionList() < 2) {
    ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_4, ERROR_GRADE);
    goto exit;
  }
  g_p1 = EVGetFromSelectionList();
  g_p2 = EVGetFromSelectionList();

  if        (EGGraphicType(g_p1) == EG_LINE3D) {
    curve = LineToCBezC3D(g_p1);
    to_destroy_curve = YES;
  } else if (EGGraphicType(g_p2) == EG_LINE3D) {
    curve = LineToCBezC3D(g_p2);
    to_destroy_curve = YES;
  } else if (EGGraphicType(g_p1) == EG_CBEZCURVE3D) {
    curve = g_p1;
  } else if (EGGraphicType(g_p2) == EG_CBEZCURVE3D) {
    curve = g_p2;
  }
  if (curve == NULL) {
    ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_5, ERROR_GRADE);
    goto exit;
  }
  
  if        (EGGraphicType(g_p1) == EG_BCBEZSURFACE3D) {
    patch = g_p1;
  } else if (EGGraphicType(g_p2) == EG_BCBEZSURFACE3D) {
    patch = g_p2;
  }
  if (patch == NULL) {
    ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_6, ERROR_GRADE);
    goto exit;
  }
  hex = BCBezS3DExtrudeToBCBHex(patch, curve);
  if (hex != NULL) {
    EGGetGraphicsGeometry(hex, points, &npts);
    AGECreateBCBHex(points);
    EGDeepDestroyGraphics(hex);
  } else {
    ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_7, ERROR_GRADE);
    goto exit;
  }
 exit:
  if (to_destroy_curve)
    EGDeepDestroyGraphics(curve);
}



static WCRec extrusion_delta = {0, 0, 0};

static void 
do_extrude_by_delta(EView *v_p, caddr_t data_p);

static void 
do_extrude_by_delta(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p, *curve = NULL, *patch = NULL, *hex = NULL;
  WCRec pnts[4] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  WCRec ppts[4][4];
  int nppts;
  WCRec points[56];
  int npts;
  
  if (EVItemsInSelectionList() < 0) {
    ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_1, ERROR_GRADE);
    goto exit;
  }

  curve = CreateCBezC3D(pnts);
  while ((g_p = EVGetFromSelectionList()) != NULL) {
    if        (EGGraphicType(g_p) == EG_BCBEZSURFACE3D) {
      patch = g_p;
      EGGetGraphicsGeometry(patch, (WCRec *)ppts, &nppts);
      pnts[0].x = ppts[0][0].x;
      pnts[0].y = ppts[0][0].y;
      pnts[0].z = ppts[0][0].z;
      pnts[1].x = pnts[0].x + extrusion_delta.x / 3; 
      pnts[1].y = pnts[0].y + extrusion_delta.y / 3;
      pnts[1].z = pnts[0].z + extrusion_delta.z / 3;
      pnts[2].x = pnts[1].x + extrusion_delta.x / 3; 
      pnts[2].y = pnts[1].y + extrusion_delta.y / 3;
      pnts[2].z = pnts[1].z + extrusion_delta.z / 3;
      pnts[3].x = pnts[2].x + extrusion_delta.x / 3; 
      pnts[3].y = pnts[2].y + extrusion_delta.y / 3;
      pnts[3].z = pnts[2].z + extrusion_delta.z / 3; 
      hex = BCBezS3DExtrudeToBCBHex(patch,
                                    EGModifyGraphicsGeometry(curve, pnts));
      if (hex != NULL) {
        EGGetGraphicsGeometry(hex, points, &npts);
        AGECreateBCBHex(points);
        EGDeepDestroyGraphics(hex);
      } else {
        ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_7, ERROR_GRADE);
        goto exit;
      }
    }
  }
 exit:
  EGDeepDestroyGraphics(curve);
}



static void 
do_from_curves(EView *v_p, caddr_t data_p);

static void 
do_from_curves(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p, *curves[12], *result = NULL;
  int i, to_destroy[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  WCRec points[56];
  int npts;
  
  if (EVItemsInSelectionList() < 0) {
    ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_1, ERROR_GRADE);
    goto exit;
  }

  i = 0; 
  while ((g_p = EVGetFromSelectionList()) != NULL) {
    to_destroy[i] = NO;
    if        (EGGraphicType(g_p) == EG_LINE3D) {
      to_destroy[i] = YES;
      curves[i++] = LineToCBezC3D(g_p);
    } else if (EGGraphicType(g_p) == EG_CBEZCURVE3D) {
      curves[i++] = g_p;
    } 
  }
  if (i == 12) {
    result = BCBezCurvesToBCBHex(curves);
    if (result != NULL) {
      EGGetGraphicsGeometry(result, points, &npts);
      AGECreateBCBHex(points);
      EGDeepDestroyGraphics(result);
    } else {
      ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_2, ERROR_GRADE);
      goto exit;
    }
  } else if (i > 0 && i <= 4) {
    result = CurvesToBCBezS3D(curves, i);
    if (result != NULL) {
      EGGetGraphicsGeometry(result, points, &npts);
      AGECreateBCBezS3D(points);
      EGDeepDestroyGraphics(result);
    } else {
      ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_2, ERROR_GRADE);
      goto exit;
    }
  } else {
    ERptErrMessage(CREATE_ERROR_CLASS, 2, CREATE_ERROR_3, ERROR_GRADE);
  }
 exit:
  for (i = 0; i < 12; i++)
    if (to_destroy[i]) EGDeepDestroyGraphics(curves[i]);
}


static void
set_extrusion_delta(Widget w, XtPointer text_ptr, XtPointer call_data);

static void
set_extrusion_delta(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  if (TypeInGetTokenType(1) == NUMBER)
    extrusion_delta.x = TypeInGetTokenAsDouble(1);
  if (TypeInGetTokenType(2) == NUMBER)
    extrusion_delta.y = TypeInGetTokenAsDouble(2);
  if (TypeInGetTokenType(3) == NUMBER)
    extrusion_delta.z = TypeInGetTokenAsDouble(3);
}



void
set_extrusion_delta_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  static Widget b = NULL;
  Widget p = (Widget)ptr;

  if (b == NULL) {
    b    = ESIAddPopupDialog("delta_dialog", "DELTA",
                             "Extrusion delta (dx dy dz) >",
                             "0 0 0", p, NULL, 0, set_extrusion_delta, NULL,
                             ESIDialogValueAny, NULL);
  }
  XtCallCallbacks(b, XtNcallback, NULL);
}


   

void
extrude_by_delta_CB(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  EVSetSelectActionFunction(do_extrude_by_delta, NULL, NULL);
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL);
  ERptStatusMessage(CREATE_STATUS_CLASS, 1, MESSAGE_CREATE_DELTA);
}



void
extrude_along_curve_CB(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  EVSetSelectActionFunction(do_extrude_along_curve, NULL, NULL);
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL);
  ERptStatusMessage(CREATE_STATUS_CLASS, 1, MESSAGE_CREATE_CURVE);
}



void
from_curves_CB(Widget w, XtPointer text_ptr, XtPointer call_data)
{
  EVSetSelectActionFunction(do_from_curves, NULL, NULL);
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL);
  ERptStatusMessage(CREATE_STATUS_CLASS, 1, MESSAGE_CREATE_CURVS);
}

