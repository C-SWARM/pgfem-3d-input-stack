/*

                  #     #####  #######
                 # #   #     # #
                #   #  #       #
               #     # #  #### #####
               ####### #     # #
               #     # #     # #
               #     #  #####  #######

	          A Graphics Editor

	   (C) Copyright Petr Krysl, 1993, 1994.
 */

#include "Econfig.h"
#include "Ecomptblt.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <X11/Intrinsic.h>
#include <X11/Core.h>
#include <X11/Shell.h>

#include "Eview.h"
#include "Emodel.h"
#include "Ereprts.h"
#include "Ege.h"
#include "Egecb.h"
#include "Eondisk.h"
#include "Ecmds.h"
#include "Egecmds.h"
#include "Eline3d.h"
#include "Elixir.h"
#include "Ecolors.h"
#include "Esimple.h"

void uninstall_apply_to_view(EView *v_p, caddr_t data);
void delete_from_selection(EView *v_p, caddr_t data_p);
static void produce_ginfo(EView *v_p, caddr_t data_p);
static void produce_oogl_export(EView *v_p, caddr_t data_p);
void apply_redraw(EView *v_p, caddr_t data, WCRec *);

/* ======================================================================= */
  
void 
quit_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  XtCloseDisplay(XtDisplay(w));
  exit(ESISetExitCode(ELIXIR_OK_EXIT));
}


/* ======================================================================= */
  
void 
fast_viewing_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  EMSuspendHandler(ESIModel());
}

/* ======================================================================= */


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
delete_reset(EView *v_p, caddr_t data_p);
static void
delete_reset(EView *v_p, caddr_t data_p)
{
  EMUninstallHandler(EVGetModel(v_p));
	EMDispatchToDependentViews(ESIModel(), reset_view, NULL);   /* added by dr to prevent blinking */
}


static void
info_reset(EView *v_p, caddr_t data_p);
static void
info_reset(EView *v_p, caddr_t data_p)
{
  EMUninstallHandler(EVGetModel(v_p));
	EMDispatchToDependentViews(ESIModel(), reset_view, NULL);   /* added by dr to prevent blinking */
}

/* ======================================================================== */
 
void 
delete_from_selection(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p;
  LIST          to_delete;

  if (EVItemsInSelectionList() <= 0)
    return;

  to_delete = make_list();
  while ((g_p = (GraphicObj *)EVGetFromSelectionList()) != NULL) 
    add_to_tail(to_delete, g_p);
  CmdFlipState(AGECreateFastDeleteCommand(to_delete));
/*  CmdFlipState(AGECreateDeleteCommand(to_delete)); */
  free_list(to_delete, NOT_NODES);
  EMUninstallHandler(EVGetModel(v_p));
}

/* ======================================================================= */
  
void 
delete_graphics_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
	EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  EVSetSelectActionFunction(delete_from_selection, NULL, NULL);
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL,
		      delete_reset, NULL);                                     /* added by dr to prevent blinking */
	ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "Select graphics to delete");          /* added by dr */
}


/* ======================================================================= */
 
static void 
produce_ginfo(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p;
  char *buf = NULL;
  char s[80];
  char *sgr;

  while ((g_p = (GraphicObj *)EVGetFromSelectionList()) != NULL) {
    
    sprintf(s, "\nGID time stamp: %lu, serial: %lu, layer %d\n",
	    EGGraphicTimeID(g_p), EGGraphicSerialID(g_p), EGGetLayer(g_p));
    sgr = EGStringGraphicRepresentation(g_p);
    if (buf == NULL) {
      buf = (char *)malloc(strlen(s) + strlen(sgr) + 1);
      buf[0] = '\0';
    } else {
      buf = (char *)realloc(buf, strlen(buf) + strlen(s) + strlen(sgr) + 1);
    }
    if (buf == NULL) {
      ERptErrMessage(ELIXIR_ERROR_CLASS, 1, ELIXIR_ERROR_5, ERROR_GRADE);
      goto exit;
    } else {
      strcat(buf, s);
      strcat(buf, sgr);
    }
  }
  if (buf != NULL) {
    ESIPopupInfo(buf);
    free(buf);
  }
 exit:
  EMUninstallHandler(EVGetModel(v_p));
}

 /* ======================================================================= */
  
void 
ginfo_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
	EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

  EVSetSelectActionFunction(produce_ginfo, NULL, NULL);
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL,
		      info_reset, NULL);                                     /* added by dr to prevent blinking */
	ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "Select graphics for info");          /* added by dr */
}


 /* ======================================================================= */
  
void 
undo_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  CmdFlipState(CmdCurrentCommandInHistory());
  CmdPrevCommandInHistory();
}


 /* ======================================================================= */
  
void 
redo_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  CmdNextCommandInHistory();
  CmdFlipState(CmdCurrentCommandInHistory());
}


/* ========================================================================= */

void 
apply_redraw(EView *v_p, caddr_t data, WCRec *p)
{
/* Doing nothing: redraw is the default of apply_to_view input handler. */
}

 /* ======================================================================= */
  
void 
redraw_view_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  EVSetApplyToViewFunction(apply_redraw, NULL, uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}

 /* ======================================================================= */
    
void 
new_frame_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  EView        *v_p;
  EView        **ret_v_pp = (EView **)call_data;

  if (ESIModel() == NULL) {
    ERptErrMessage(AGE_ERROR_CLASS, 1, AGE_ERROR_1, ERROR_GRADE);
  } else {
    v_p = ElixirNewView("elixir", "SimpleXF", "black", "white", 500, 400);
    EMAttachView(ESIModel(), v_p);
    EMRegenerateGraphics(ESIModel(), v_p);
    if (ret_v_pp != NULL)
      *ret_v_pp = v_p;
  }
}


static void 
get_cplane_def_point(EView *v_p, caddr_t data, WCRec *);

void
cplane_by_three_points_CB(Widget w, XtPointer cli, XtPointer call)
{
  EVSetApplyToViewFunction(get_cplane_def_point, 0,
                           uninstall_apply_to_view);
  EVSetApplyToViewPreventRedisplay(YES);
  EVSetApplyToViewPrompt(1, 1, "B1=Set construction plane center;B3=cancel");
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}


static WCRec cplane_def_point[3];

static void 
def_cplane(EView *v_p, caddr_t data, WCRec *p);

static void 
get_cplane_def_point(EView *v_p, caddr_t data, WCRec *p)
{
  int ix = (int)data;
  
  cplane_def_point[ix].x = p->x;
  cplane_def_point[ix].y = p->y;
  cplane_def_point[ix].z = p->z;
  if (ix == 2) {
    EVSetApplyToViewFunction(def_cplane, 0, uninstall_apply_to_view);
  } else {
    ix++;
    EVSetApplyToViewFunction(get_cplane_def_point, (caddr_t)(ix),
                             uninstall_apply_to_view);
    if (ix == 1)
      EVSetApplyToViewPrompt(1, 1, "B1=End of U vector;B3=cancel");
    else
      EVSetApplyToViewPrompt(1, 1, "B1=Third point in plane;B3=cancel");
    EVSetApplyToViewPreventRedisplay(YES);
  }
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}


static void 
def_cplane(EView *v_p, caddr_t data, WCRec *p)
{
  WCRec n, v1, v2, vup;
  
  EVSetConstrPlaneCenter(v_p, &cplane_def_point[0]);
  v1.x = cplane_def_point[1].x - cplane_def_point[0].x;
  v1.y = cplane_def_point[1].y - cplane_def_point[0].y;
  v1.z = cplane_def_point[1].z - cplane_def_point[0].z;
  v2.x = cplane_def_point[2].x - cplane_def_point[0].x;
  v2.y = cplane_def_point[2].y - cplane_def_point[0].y;
  v2.z = cplane_def_point[2].z - cplane_def_point[0].z;
  CrossProd3(&n, &v1, &v2);
  CrossProd3(&vup, &n, &v1);
  EVSetConstrPlaneOrientation(v_p, &n, &vup);
}



static void 
produce_oogl_export(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p;
  FILE *fp;
  
#ifdef HAVE_MKSTEMP
	char buf[32] = "ooglXXXXXX";
	mkstemp(buf);
#else
  char buf[L_tmpnam];
  tmpnam(buf);
#endif
  fp = fopen(buf, "w");
  fprintf(fp, "#  Export of Elixir object(s) into the OOGL format\n");
  fprintf(fp, "#  \n");
  EXIWriteOOGLStartPROGN(fp);
  while ((g_p = (GraphicObj *)EVGetFromSelectionList()) != NULL) {
    EXIWriteOOGLGeom(fp, EVGetModel(v_p), g_p);
  }
  EXIWriteOOGLClosePROGN(fp);
  fclose(fp);
  ESIPopupInfoFromFile(buf);
 exit:
  EMUninstallHandler(EVGetModel(v_p));
}

 /* ======================================================================= */
  
void 
oogl_export_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  EVSetSelectActionFunction(produce_oogl_export, NULL, NULL);
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL,
		      NULL, NULL);
}
