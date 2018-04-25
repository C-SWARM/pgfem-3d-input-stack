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

/* This file holds the redefinition of graphic methods for the graphic */
/* group entity. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"

#define GGROUP_PRIVATE_HEADER
#include "Eggroup.h"
#include "Ectlblock.h"


  
/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateGgroup(LIST g_objects)
{
  EGgroupRec *r_p;
  GraphicObj *g_p;

  r_p = (EGgroupRec *)make_node(sizeof(EGgroupRec));
  g_p = EGCreateGraphics(&ggroup_graphic_methods, (caddr_t)r_p,
			 sizeof(EGgroupRec));
  if (g_objects == (LIST)NULL)
    r_p->g_objects = make_list();
  else
    r_p->g_objects = copy_list(g_objects, NOT_NODES);
  r_p->attributes.ggroup.have_any = NO;
  return g_p;
}

/* ========================================================================= */

LIST 
GgroupGObjects(GraphicObj *g_p)
{
  if (EGGraphicType(g_p) != EG_GGROUP)
    return (LIST)NULL;
  else
    return (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
}

/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

static void 
Draw(EView *v_p, GraphicObj *g_p)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGDrawGraphics(v_p, ag_p);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }  
}

static void 
BoundingBoxVC(EView *v_p, GraphicObj *g_p, VCRec *ll, VCRec *ur)
{
  GraphicObj *ag_p;
  LIST list;
  VCRec p1, p2;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);

  if (!list_empty(list)) {
    ag_p = (GraphicObj *)get_list_next(list, NULL);
    EGGraphicsBoundingBoxVC(v_p, ag_p, ll, ur);
    while (ag_p != (GraphicObj *)NULL) {
      EGGraphicsBoundingBoxVC(v_p, ag_p, &p1, &p2);
      ll->u = min(ll->u, p1.u);
      ll->v = min(ll->v, p1.v);
      ll->n = min(ll->n, p1.n);
      ur->u = max(ur->u, p2.u);
      ur->v = max(ur->v, p2.v);
      ur->n = max(ur->n, p2.n);
      ag_p = (GraphicObj *)get_list_next(list, ag_p);
    }
  }
}

static void 
BoundingBoxWC(GraphicObj *g_p, WCRec *ll, WCRec *ur)
{
  GraphicObj *ag_p;
  LIST list;
  WCRec p1, p2;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);

  if (!list_empty(list)) {
    ag_p = (GraphicObj *)get_list_next(list, NULL);
    EGGraphicsBoundingBoxWC(ag_p, ll, ur);
    while (ag_p != (GraphicObj *)NULL) {
      EGGraphicsBoundingBoxWC(ag_p, &p1, &p2);
      ll->x = min(ll->x, p1.x);
      ll->y = min(ll->y, p1.y);
      ll->z = min(ll->z, p1.z);
      ur->x = max(ur->x, p2.x);
      ur->y = max(ur->y, p2.y);
      ur->z = max(ur->z, p2.z);
      ag_p = (GraphicObj *)get_list_next(list, ag_p);
    }
  }
}

static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGXORDrawGraphics(v_p, ag_p);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }  
}
 
static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGHiliteGraphics(v_p, ag_p);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }  
}

static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGUnhiliteGraphics(v_p, ag_p);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }  
}

static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGEraseGraphics(v_p, ag_p);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  } 
}

static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGTranslateGraphic(ag_p, by);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }
  return g_p;
}

static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGRotateGraphic(ag_p, center, axial);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }
  return g_p;
}

static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
			       WCRec *point_on_plane, WCRec *plane_normal)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGMirrorPlaneGraphic(ag_p, point_on_plane, plane_normal);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }
  return g_p;
}

static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGMirrorCenterGraphic(ag_p, center);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }
  return g_p;
}

static GraphicObj *
Scale(GraphicObj *g_p, WCRec *c,
		 FPNum sx, FPNum sy, FPNum sz)
{
  return g_p;
}

static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
		       ESelectCriteria sel_crit, GraphicObj *g_p)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    if (EGGraphicHitByBox(v_p, box, sel_crit, ag_p))
      return YES;
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  }
  return NO;
}

static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_GGROUP;
}

static char *
AsString(GraphicObj *g_p)
{
  EGgroupRec *l;

  l = (EGgroupRec *)g_p->spec.data;
  sprintf(string_rep_buffer, "GGROUP  GID %lu,%lu",
	  EGGraphicTimeID(g_p), EGGraphicSerialID(g_p));
  
  return string_rep_buffer; /* Don't free this memory!!! */
}

static LIST 
KeyPoints(GraphicObj *p)
{
  return (LIST)NULL;
}

static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *hp)
{
  return p;
}

static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *hp)
{
  return (EHandleNum)1;
}

static void 
ShowHandles(EView *v_p, GraphicObj *p)
{
}
  
static void 
EraseHandles(EView *v_p, GraphicObj *p)
{
}

static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *hp)
{
}

static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EGgroupRec *gg_p = NULL;
  GraphicObj *ag_p; 
  
  if ((gg_p = (EGgroupRec *)make_node(sizeof(EGgroupRec))) != NULL) {
    gg_p->g_objects = make_list();
  retrieve_next:
    ag_p = EGGraphicRetrieveFrom(file_p);
    if (ag_p != (GraphicObj *)NULL) {
      if (EGGraphicType(ag_p) == EG_CTLBLOCK)
	if (EGGraphicType(ag_p) == END_OF_GROUP_CTLBLOCK)
	  goto exit;
      add_to_tail(gg_p->g_objects, ag_p);
      goto retrieve_next;
    }
  }
 exit:
  gsp->data        = (caddr_t)gg_p;
  gsp->data_length = sizeof(EGgroupRec);
}
 
static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EGgroupRec *gg_p;
  LIST g_objects;
  GraphicObj *ag_p;

  gg_p = (EGgroupRec *)g_p->spec.data;
  g_objects = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(g_objects, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGGraphicStoreOn(file_p, ag_p);
    ag_p = (GraphicObj *)get_list_next(g_objects, ag_p);
  }
  EGGraphicStoreOn(file_p, CreateCtlBlock(END_OF_GROUP_CTLBLOCK));
}

static GraphicObj *
DeepCopy(GraphicObj *g_p)
{
  GraphicObj *ag_p, *gg_p;
  LIST old_list;
  LIST new_list; 

  gg_p = EGCopyGraphics(g_p);
  old_list = (((EGgroupRec *)(g_p->spec.data))->g_objects);
  new_list = ((EGgroupRec *)(gg_p->spec.data))->g_objects = make_list();
  ag_p = (GraphicObj *)get_list_next(old_list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    add_to_tail(new_list, EGDeepCopyGraphics(ag_p));
    ag_p = (GraphicObj *)get_list_next(old_list, ag_p);
  }
  return gg_p;
}

static void         
DeepDestroy(GraphicObj *g_p)
{
  GraphicObj *ag_p;
  LIST list;

  list = (LIST)(((EGgroupRec *)(g_p->spec.data))->g_objects);
  ag_p = (GraphicObj *)get_list_next(list, NULL);
  while (ag_p != (GraphicObj *)NULL) {
    EGDeepDestroyGraphics(ag_p);
    ag_p = (GraphicObj *)get_list_next(list, ag_p);
  } 
}
