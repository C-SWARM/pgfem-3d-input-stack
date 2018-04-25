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
#include "Ereprts.h"
#include "Egecmds.h"
#include "Ege.h"
#include "Egecb.h"
#include "Ereshape.h"
#include "Esimple.h"


/* Local variables of the graphics-creation routines */

static GraphicObj *echo = NULL;
static BOOLEAN reshape_started = NO; 
static WCRec curr;
static EHandleNum handle = -1;
static GraphicObj *curr_g_p = NULL;

static void reshape_resume(EView *v_p, caddr_t);
static void reshape_start(EView *v_p, caddr_t data);
static void init_reshape(void);
void reshape_CB(Widget w, XtPointer client_data,
		    XtPointer call_data);

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
reshape_reset(EView *v_p, caddr_t data_p);
static void
reshape_reset(EView *v_p, caddr_t data_p)
{
  EMUninstallHandler(EVGetModel(v_p));
	EMDispatchToDependentViews(ESIModel(), reset_view, NULL);   /* added by dr to prevent blinking */
}

/* ======================================================================== */

static void 
init_reshape(void)
{
	EMDispatchToDependentViews(ESIModel(), set_view, NULL);          /* added by dr to prevent blinking */
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,
		      NULL, NULL,
		      NULL, NULL,
		      reshape_resume, NULL,
		      reshape_reset, NULL);
  EVSetSelectActionFunction(reshape_start, NULL, NULL);
  ERptStatusMessage(RESHAPE_STATUS_CLASS, 4, MESSAGE_RSHP3);
}

/* ======================================================================== */
  
void 
ReshapeHandler(Widget w, EView *v_p, XEvent *event)
{
  switch(event->type) {
  case ButtonPress:
    switch(event->xbutton.button) {
    case Button1:
      if (reshape_started == YES) {
	EVInputPoint(v_p, event, &curr);
	EVSetLastInputPoint(&curr);
	if (EVIsPointerIn() == YES)
	  EGXORDrawGraphics(v_p, echo);
	CmdFlipState(AGECreateReshapeCommand(curr_g_p, handle, &curr));
        EGDeepDestroyGraphics(echo); echo = NULL;
	if (EVItemsInSelectionList() == 0) {
          EGGraphicShowHandles(v_p, curr_g_p);
	} else {
	  if ((curr_g_p =  EVFirstInSelectionList()) != NULL) {
	    EGGraphicShowHandles(v_p,curr_g_p); curr_g_p = NULL;
          }
	}
	reshape_started = NO;
      } else {
        if (curr_g_p == NULL)
          curr_g_p = EVGetFromSelectionList();
	if (curr_g_p != NULL) {
	  EVInputPoint(v_p, event, &curr);
	  EGXORDrawGraphics(v_p, (echo = EGDeepCopyGraphics(curr_g_p)));
	  handle = EGReshapeHandle(v_p, curr_g_p, &curr);
	  EGGraphicEraseHandles(v_p, curr_g_p);
	  ERptPrompt(RESHAPE_PROMPT_CLASS, 1, MESSAGE_RSHP1);
	  reshape_started = YES;
	} else {
	  init_reshape();
	}
      }
      break;
    case Button3:
      if (reshape_started == YES) {
	if (echo != NULL) 
	  EGXORDrawGraphics(v_p, echo);
	reshape_started = NO;
	init_reshape();
      } else {
        if (EVItemsInSelectionList() == 0) {
          if (curr_g_p != NULL)
            EGGraphicEraseHandles(v_p, curr_g_p);
          init_reshape();
        } else {
          if (echo != NULL) 
            EGDeepDestroyGraphics(echo);
          echo = NULL;
          if (curr_g_p != NULL)
            EGGraphicEraseHandles(v_p, curr_g_p);
          ERptPrompt(RESHAPE_PROMPT_CLASS, 4, MESSAGE_RSHP4);
          EVClearSelection();
          EMUninstallHandler(EVGetModel(v_p));
        }
      }
      break;
    }
    break;
  case MotionNotify:
    EVInputPoint(v_p, event, &curr);
    ERptTrackPointer(&curr);
    if (reshape_started == YES) {
      EGXORDrawGraphics(v_p, echo);
      EGXORDrawGraphics(v_p, EGModifyGraphicsByHandle(echo, handle, &curr));
      break;
    } else {
      ERptPrompt(RESHAPE_PROMPT_CLASS, 2, MESSAGE_RSHP2);
    }
    break;
  case LeaveNotify:
  case EnterNotify:
    if (reshape_started == YES) {
      EGXORDrawGraphics(v_p, echo);
    }
    break;
  case ClientMessage:
    ERptPrompt(RESHAPE_PROMPT_CLASS, 2, MESSAGE_RSHP2);
    break;
  }
}


 /* ======================================================================= */
  
static void 
reshape_resume(EView *v_p, caddr_t data)
{
  if (echo != NULL) {
    if (reshape_started == YES) {
      echo = EGModifyGraphicsByHandle(echo, handle, &curr);
      ERptPrompt(RESHAPE_PROMPT_CLASS, 2, MESSAGE_RSHP2);
    }
  }
}

 /* ======================================================================= */
  
static void 
reshape_start(EView *v_p, caddr_t data)
{
  EMInstallHandler(ESIModel(), ReshapeHandler,
		      NULL, NULL,
		      NULL, NULL,
		      reshape_resume, NULL,
		      NULL, NULL);
  ERptPrompt(RESHAPE_PROMPT_CLASS, 2, MESSAGE_RSHP2);
  curr_g_p = EVFirstInSelectionList();
  if (curr_g_p != NULL)
    EGGraphicShowHandles(v_p, curr_g_p);
  EVClearSelection(); /* don't admit more than one object  */
}

/* ======================================================================== */
  
void 
reshape_CB(Widget w, XtPointer client_data, XtPointer call_data)
{
  init_reshape();
}
