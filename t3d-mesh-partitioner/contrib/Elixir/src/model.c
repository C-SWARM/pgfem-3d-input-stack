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
 
/* This file holds methods on a model. A model is any object which maintains */
/* a collection of graphic entities. A model is associated to any number */
/* of views. */

#include "Econfig.h"
#include "Ecomptblt.h"

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>

#include "Eview.h"
#include "Emodel.h"
#include "Eactset.h"
#include "Eutils.h"
#include "Ecmds.h"
#include "Egecmds.h"
#include "Eondisk.h"

#include "Esimple.h"
#include "Ectlblock.h"


/* PRIVATE */

#define IMMEDIATE_DRAW 1

static int display_mode = IMMEDIATE_DRAW;

static int old_layer = 0, new_layer = 0;

int model_install_handler(NODE handler, NODE resume_proc, NODE v_p);
int model_uninstall_handler(NODE ptr, NODE v_p);
int model_suspend_handler(NODE ptr, NODE v_p);
int model_resume_suspended_handler(NODE ptr, NODE v_p);
int model_display_grph_iter_by_v(NODE g_p, NODE v_p);
int model_display_grph_iter_by_g(NODE v_p, NODE g_p);
int model_erase_graphics(NODE g_p, NODE v_p);
int model_draw_graphics(NODE g_p, NODE v_p);
int model_xordraw_graphics(NODE g_p, NODE v_p);
int model_hilite_graphics(NODE g_p, NODE v_p);
int model_unhilite_graphics(NODE g_p, NODE v_p);
int model_unlink_graphics(NODE g_p, NODE v_p);
int model_chlayer(NODE g_p, NODE v_p);
int model_fast_chlayer(NODE dummy, NODE v_p);
int model_regenerate(NODE ptr, NODE v_p);

static void EMAddGraphicsToList(EModel *m_p, GraphicObj *g_p);

static void 
EMAddGraphicsToList(EModel *m_p, GraphicObj *g_p)
{
  if (add_to_tail(m_p->graphics_list, g_p) != (NODE)g_p) {
    fprintf(stderr, "Failed to add in EMAddGraphicsToList\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
}



int 
model_chlayer(NODE g_p, NODE v_p)
{
  EVUpdateDisplayLists((EView *)v_p, (GraphicObj *)g_p);
  return 1;
}
 


int 
model_fast_chlayer(NODE dummy, NODE v_p)
{
  EVFastUpdateDisplayLists((EView *)v_p);
  return 1;
}
 


int 
model_unlink_graphics(NODE g_p, NODE v_p)
{
  EVUnlinkGraphics((EView *)v_p, (GraphicObj *)g_p);
  return 1;
}


int 
model_unlink_marked_graphics(NODE dummy, NODE v_p)
{
  EVUnlinkMarkedGraphics((EView *)v_p);
  return 1;
}



int 
model_draw_graphics(NODE g_p, NODE v_p)
{
  EVDrawGraphics((EView *)v_p, (GraphicObj *)g_p);
  return 1;
}



int 
model_xordraw_graphics(NODE g_p, NODE v_p)
{
  EVXORDrawGraphics((EView *)v_p, (GraphicObj *)g_p);
  return 1;
}




int 
model_erase_graphics(NODE g_p, NODE v_p)
{
  EVEraseGraphics((EView *)v_p, (GraphicObj *)g_p);
  return 1;
}



int 
model_display_grph_iter_by_v(NODE g_p, NODE v_p)
{
  EVDisplayGraphics((EView *)v_p, (GraphicObj *)g_p, display_mode);
  return 1;
}
      



int 
model_display_grph_iter_by_g(NODE v_p, NODE g_p)
{
  EVDisplayGraphics((EView *)v_p, (GraphicObj *)g_p, display_mode);
  return 1;
}



int 
model_unhilite_graphics(NODE g_p, NODE v_p)
{
  EVUnhiliteGraphics((EView *)v_p, (GraphicObj *)g_p);
  return 1;
}



int 
model_hilite_graphics(NODE g_p, NODE v_p)
{
  EVHiliteGraphics((EView *)v_p, (GraphicObj *)g_p);
  return 1;
}



int 
model_resume_suspended_handler(NODE ptr, NODE v_p)
{
  EVResumeSuspendedHandler((EView *)v_p);
  return 1;
}



int 
model_suspend_handler(NODE ptr, NODE v_p)
{
  EVSuspendActiveHandler((EView *)v_p);
  return 1;
}



int 
model_uninstall_handler(NODE ptr, NODE v_p)
{
  EVUninstallHandler((EView *)v_p);
  return 1;
}


int 
model_regenerate(NODE model, NODE v_p)
{
  EMRegenerateGraphics((EModel *)model, (EView *)v_p);
  return 1;
}
      


/* PUBLIC METHODS */


void 
EMAttachView(EModel *m_p, EView *v_p)
{
    add_to_tail(m_p->dependent_views, (NODE)v_p);
    EVSetModel(v_p, m_p);
    if (m_p->fringe_table != NULL) 
      EVSetAssocFringeTable(v_p, m_p->fringe_table);
}



void 
EMNotifyDestroyView(EModel *m_p, EView *v_p)
{
  EView *v;

  v = (EView *)get_list_next(m_p->dependent_views, NULL);
  while (v != NULL)
    if (v == v_p) {
      get_from_current_prev(m_p->dependent_views);
      break;
    } else
      v = (EView *)get_list_next(m_p->dependent_views, v);
}



EModel *
EMCreateModel(void)
{
  EModel *m_p;

/* Model storage  */
  m_p = (EModel *)make_node(sizeof(EModel));
  if (m_p == NULL) {
    EUFailedMakeNode("EMCreateModel");
  }

/* Model lists */
  m_p->dependent_views   = make_list();
  m_p->graphics_list     = make_list();

	m_p -> apply_to_all_views = NO;
	m_p -> view_to_apply = NULL;

/* To use GRAPHICS */
  EGInitGraphics();

  return m_p;
}


void
EMSetApplyToAllViews(EModel *m_p, BOOLEAN on_off)
{
	m_p -> apply_to_all_views = on_off;
}

BOOLEAN
EMGetApplyToAllViews(EModel *m_p)
{
	return(m_p -> apply_to_all_views);
}


EView *
EMSetViewToApply(EModel *m_p, EView *new_view)
{
	EView *old_view = m_p -> view_to_apply;

	m_p -> view_to_apply = new_view;
	return(old_view);
}


EView *
EMGetViewToApply(EModel *m_p)
{
	return(m_p -> view_to_apply);
}


EView *
EMAnyViewP(EModel *m_p)
{
  return (EView *)get_list_head(m_p->dependent_views);
}


int
EMCountDepViews (EModel *m_p)
{
  return list_length (m_p->dependent_views);
}


void 
EMDispatchToDependentViews(EModel *m_p, MessageP message,
				   caddr_t data)
{
  for_all_list(m_p->dependent_views, (NODE)data, message);
}



void 
EMDispatch2ToDependentViews(EModel *m_p, Message2P message,
				    caddr_t data1, caddr_t data2)
{
  for_all_2list(m_p->dependent_views, (NODE)data1, (NODE)data2, message);
}



void 
EMDispatch3ToDependentViews(EModel *m_p, Message3P message,
		             caddr_t data1, caddr_t data2, caddr_t data3)
{
  for_all_3list(m_p->dependent_views,
		(NODE)data1, (NODE)data2, (NODE)data3, message);
}



void 
EMInstallHandler(EModel *m_p,
		 EventHandlerP handler,
		 StartProcP   start_proc,
		 caddr_t        start_proc_data,
		 SuspendProcP suspend_proc,
		 caddr_t        suspend_proc_data,
		 ResumeProcP  resume_proc,
		 caddr_t        resume_proc_data,
		 RemoveProcP  remove_proc,
		 caddr_t        remove_proc_data)
{
  EView *v_p;
  
  v_p = (EView *)get_list_next(m_p->dependent_views, NULL);
  while (v_p != NULL) {
    EVInstallHandler(v_p, handler,
			 start_proc, start_proc_data,
			 suspend_proc, suspend_proc_data,
			 resume_proc, resume_proc_data,
			 remove_proc, remove_proc_data);
    v_p = (EView *)get_list_next(m_p->dependent_views, v_p);
  }
}



void 
EMUninstallHandler(EModel *m_p)
{
  EMDispatchToDependentViews(m_p, model_uninstall_handler, NULL);
}



void 
EMSuspendHandler(EModel *m_p)
{
  EMDispatchToDependentViews(m_p, model_suspend_handler, NULL);
}



void 
EMPushHandler(EModel *m_p, EventHandlerP handler, ResumeProcP resume_proc)
{
  EMSuspendHandler(m_p);
  EMInstallHandler(m_p, handler,
		      NULL, NULL,
		      NULL, NULL,
		      resume_proc, NULL,
		      NULL, NULL);
}



void 
EMResumeSuspendedHandler(EModel *m_p)
{
  EMDispatchToDependentViews(m_p, model_resume_suspended_handler, NULL);
}



void 
EMAddGraphicsToModel(EModel *m_p, GraphicObj *g_p)
{  
  EMAddGraphicsToList(m_p, g_p); /* add to list of graphics in model */
  display_mode = IMMEDIATE_DRAW; /* draw in immediate mode */
  EMDisplayGraphics(m_p, g_p); /* add to display lists of dependent views */
}
 

void
EMNoDrawAddGraphicsToModel(EModel *m_p, GraphicObj *g_p)
{  
  EMAddGraphicsToList(m_p, g_p); /* add to list of graphics in model */
  display_mode = !(IMMEDIATE_DRAW); /* don't draw in immediate mode */
  EMDisplayGraphics(m_p, g_p); /* add to display lists of dependent views */
}
 



void 
EMChangeGraphicAttributes(EModel *m_p, unsigned int mask, GraphicObj *g_p)
{
  EMEraseGraphics(m_p, g_p);
  
  if ((mask & LAYER_MASK) || (mask & ALL_ATTRIB_MASK)) {
    old_layer = EGGetLayer(g_p);
    new_layer = EASValsGetLayer();
    EGWithMaskChangeAttributes(mask, g_p);
    if (old_layer != new_layer) 
      EMDispatchToDependentViews(m_p, model_chlayer, (caddr_t)g_p);
  } else
    EGWithMaskChangeAttributes(mask, g_p);
  
  EMDrawGraphics(m_p, g_p);
}



void 
EMFastChangeGraphicAttributes(EModel *m_p, unsigned int mask)
{
  GraphicObj *p;

	if ((mask & LAYER_MASK) || (mask & ALL_ATTRIB_MASK))
		EMDispatchToDependentViews(m_p, model_fast_chlayer, NULL);

  p = (GraphicObj *)get_list_next(m_p->graphics_list, NULL);

  while (p != NULL) {
		if(EGIsMarked(p) == YES){
			EGSetMarked(p, NO);
			EMEraseGraphics(m_p, p);
			EGWithMaskChangeAttributes(mask, p);
			EMDrawGraphics(m_p, p);
		}
    p = (GraphicObj *)get_list_next(m_p->graphics_list, p);
	}
}



void 
EMRegenerateGraphics(EModel *m_p, EView *v_p)
{
  EVFlushAllDisplayLists(v_p);
  for_all_list(m_p->graphics_list, v_p, model_display_grph_iter_by_g);
}



void 
EMUnhiliteGraphics(EModel *m_p, GraphicObj *g_p)
{
	if(EGIsSelected(g_p) == YES){
		EGSetSelected(g_p, NO);
		EMDispatchToDependentViews(m_p, model_unhilite_graphics, (caddr_t)g_p);
		EMDispatchToDependentViews(m_p, model_draw_graphics, (caddr_t)g_p);
	}
}



void 
EMHiliteGraphics(EModel *m_p, GraphicObj *g_p)
{
	if(EGIsSelected(g_p) == NO){
		EGSetSelected(g_p, YES);
		EMDispatchToDependentViews(m_p, model_hilite_graphics, (caddr_t)g_p);
	}
}



void
EMDisplayGraphics(EModel *m_p, GraphicObj *g_p)
{
  EMDispatchToDependentViews(m_p, model_display_grph_iter_by_v, (caddr_t)g_p);
}



void 
EMDrawGraphics(EModel *m_p, GraphicObj *g_p)
{
  EMDispatchToDependentViews(m_p, model_draw_graphics, (caddr_t)g_p);
}



void 
EMXORDrawGraphics(EModel *m_p, GraphicObj *g_p)
{
  EMDispatchToDependentViews(m_p, model_xordraw_graphics, (caddr_t)g_p);
}



void 
EMEraseGraphics(EModel *m_p, GraphicObj *g_p)
{
  EMDispatchToDependentViews(m_p, model_erase_graphics, (caddr_t)g_p);
}




void 
EMUnlinkGraphics(EModel *m_p, GraphicObj *g_p)
{
  EMDispatchToDependentViews(m_p, model_unlink_graphics, (caddr_t)g_p);
}



void 
EMUnlinkMarkedGraphics(EModel *m_p)
{
  EMDispatchToDependentViews(m_p, model_unlink_marked_graphics, (caddr_t)NULL);
}



void 
EMDestroyAllGraphics(EModel *m_p)
{
  GraphicObj *p;

  p = (GraphicObj *)get_from_head(m_p->graphics_list);
  while (p != NULL) {
    EMUnlinkGraphics(m_p, p);
    EGDeepDestroyGraphics(p);
    p = (GraphicObj *)get_from_head(m_p->graphics_list);
  }
}



GraphicObj *
EMDeleteGraphics(EModel *m_p, GraphicObj *g_p)
{
  GraphicObj *p;

  p = (GraphicObj *)get_list_next(m_p->graphics_list, NULL);

  while (p != NULL) {
    if (p == g_p) {
      EMUnlinkGraphics(m_p, g_p); /* undraw */
      p = (GraphicObj *)get_from_current_prev(m_p->graphics_list);
      return p;
    }
    p = (GraphicObj *)get_list_next(m_p->graphics_list, p);
  }
  return NULL;
}


void
EMDeleteMarkedGraphics(EModel *m_p)
{
  GraphicObj *p, *prev_p;

	prev_p = NULL;
  p = (GraphicObj *)get_list_next(m_p->graphics_list, prev_p);

  while (p != NULL) {
		if(EGIsMarked(p) == YES){
			EGSetMarked(p, NO);
			get_from_current_prev(m_p->graphics_list);
		}
		else
			prev_p = p;
    p = (GraphicObj *)get_list_next(m_p->graphics_list, prev_p);
  }
}





#define ELIXIR_FILE_HEADER  "# ELIXIR file (C) 1994 Petr Krysl; version "
#define ELIXIR_FILE_VERSION 1


BOOLEAN 
EMSaveAllGraphics(EModel *m_p)
{
  GraphicObj *p;
  FILE *fp = NULL;

  if (!OnDiskEmptyAGEFile()) 
    return NO;
  
  fp = OnDiskOpenAGEFile("w");
  if (fp == NULL)
    return NO;

  fprintf(fp, ELIXIR_FILE_HEADER "%d\n", ELIXIR_FILE_VERSION);
  
  p = (GraphicObj *)get_list_next(m_p->graphics_list, NULL);

  while (p != NULL) {
    EGGraphicStoreOn(fp, p);
    p = (GraphicObj *)get_list_next(m_p->graphics_list, p);
  }

  EGGraphicStoreOn(fp, EASValsGetActiveSet()); /* save active set */

  OnDiskCloseAGEFile();
  
  return YES;
}


static int
elixir_file_version(FILE *fp);
void
EGSetFileVersion(int version);

BOOLEAN 
EMRetrieveAllGraphics(EModel *m_p)
{
  GraphicObj *p;
  FILE *fp;

  fp = OnDiskOpenAGEFile("r");
  
  if (fp == NULL)
    return NO;

  EGSetFileVersion(elixir_file_version(fp));

  do {
    p = EGGraphicRetrieveFrom(fp);
    if (p != NULL) {
      if (EGGraphicType(p) == EG_CTLBLOCK) {
        if (CtlBlockType(p) == ACTIVE_SET_CTLBLOCK)
          EASValsReplaceActiveSet(p);
      } else {
        EMNoDrawAddGraphicsToModel(m_p, p);
      }
    }
    if (feof(fp))
      break;
  } while (p != NULL);
  
  OnDiskCloseAGEFile();
  
  return YES;
}


static int
elixir_file_version(FILE *fp)
{
#define LINE_LEN  132
  char line[LINE_LEN];
  int version;
  
  fgets(line, LINE_LEN-1, fp);
  if (sscanf(line, ELIXIR_FILE_HEADER "%d", &version) != 1) {
    /* put back what you have read */
    rewind(fp);
    return 0;
  } else
    return version;
}




EFringeTable 
EMGetAssocFringeTable(EModel *m_p)
{
  return m_p->fringe_table;
}



EFringeTable 
EMSetAssocFringeTable(EModel *m_p, EFringeTable ft)
{
  EFringeTable old_ft;
  
  old_ft = m_p->fringe_table;
  m_p->fringe_table = ft;
  return old_ft;
}



void 
EMDispatchToGraphicObjects(EModel *m_p, MessageP message, caddr_t data)
{
  for_all_list(m_p->graphics_list, (NODE)data, message);
}
