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


#ifndef EMODEL_H
#define EMODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "EPmodel.h"
#include "Eview.h"

typedef int (*MessageP)(NODE ptr, NODE elem);
typedef int (*Message2P)(NODE ptr1, NODE ptr2, NODE elem);
typedef int (*Message3P)(NODE ptr1, NODE ptr2, NODE ptr3, NODE elem);
  
/* ========================================================================= */
/* PUBLIC FUNCTIONS  */
/* ========================================================================= */


EModel *EMCreateModel(void);

/* Views */

void EMAttachView(EModel *m_p, EView *v_p);
void EMNotifyDestroyView(EModel *m_p, EView *v_p);
void EMDispatchToDependentViews(EModel *m_p, MessageP message,
				   caddr_t data);
void EMDispatch2ToDependentViews(EModel *m_p, Message2P message,
				    caddr_t data1, caddr_t data2);
void EMDispatch3ToDependentViews(EModel *m_p, Message3P message,
		             caddr_t data1, caddr_t data2, caddr_t data3);
EView *EMAnyViewP(EModel *m_p);
int EMCountDepViews (EModel *m_p);
void EMSetApplyToAllViews(EModel *m_p, BOOLEAN on_off);
BOOLEAN EMGetApplyToAllViews(EModel *m_p);
EView *EMSetViewToApply(EModel *m_p, EView *new_view);
EView *EMGetViewToApply(EModel *m_p);

/* Handlers */

void EMInstallHandler(EModel *m_p,
			 EventHandlerP handler,
			 StartProcP   start_proc,
			 caddr_t        start_proc_data,
			 SuspendProcP suspend_proc,
			 caddr_t        suspend_proc_data,
			 ResumeProcP  resume_proc,
			 caddr_t        resume_proc_data,
			 RemoveProcP  remove_proc,
			 caddr_t        remove_proc_data);
void EMUninstallHandler(EModel *m_p);
void EMSuspendHandler(EModel *m_p);
void EMResumeSuspendedHandler(EModel *m_p);
void EMPushHandler(EModel *m_p, EventHandlerP handler,
		      ResumeProcP resume_proc);

/* Add, delete graphic to/from model */

void EMAddGraphicsToModel(EModel *m_p, GraphicObj *g_p);
void EMNoDrawAddGraphicsToModel(EModel *m_p, GraphicObj *g_p);
GraphicObj *EMDeleteGraphics(EModel *m_p, GraphicObj *g_p);
void EMDeleteMarkedGraphics(EModel *m_p);
void EMDestroyAllGraphics(EModel *m_p);
void EMRegenerateGraphics(EModel *m_p, EView *v_p);
void EMChangeGraphicAttributes(EModel *m_p, unsigned int mask, GraphicObj *g_p);
void EMFastChangeGraphicAttributes(EModel *m_p, unsigned int mask);
void EMDispatchToGraphicObjects(EModel *m_p, MessageP message, caddr_t data);

/* Drawing */

void EMUnhiliteGraphics(EModel *m_p, GraphicObj *g_p);
void EMHiliteGraphics(EModel *m_p, GraphicObj *g_p);
void EMDrawGraphics(EModel *m_p, GraphicObj *g_p);
void EMXORDrawGraphics(EModel *m_p, GraphicObj *g_p);
void EMDisplayGraphics(EModel *m_p, GraphicObj *g_p);
void EMEraseGraphics(EModel *m_p, GraphicObj *g_p);
void EMUnlinkGraphics(EModel *m_p, GraphicObj *g_p);
void EMUnlinkMarkedGraphics(EModel *m_p);


/* Immediate mode graphic */

void EMImmDrawLine3D(EModel *m_p, WCRec *points);
void EMImmDrawMarker3D(EModel *m_p, WCRec *p);
void EMImmDrawAnnText3D(EModel *m_p, WCRec *at, char *text);

/* Retrieval, save */

BOOLEAN 
EMSaveAllGraphics(EModel *m_p);
BOOLEAN 
EMRetrieveAllGraphics(EModel *m_p);

/* Fringe tables */

EFringeTable EMGetAssocFringeTable(EModel *m_p);
EFringeTable EMSetAssocFringeTable(EModel *m_p, EFringeTable ft);


#ifdef __cplusplus
}
#endif

#endif
