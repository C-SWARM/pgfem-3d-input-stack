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


#ifndef EGGROUP_H
#define EGGROUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Egraphic.h"

/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

typedef struct EEndOfGgRec {
  int                dummy;
}                                            EEndOfGgRec;

#ifdef GGROUP_PRIVATE_HEADER
#define GGROUP_TYPEDEFS
#endif

#ifdef GGROUP_TYPEDEFS

/* Data structures; never to be accessed from application programs */

typedef struct EGgroupRec {
  LIST                g_objects;
  union {
    AttributesRec            any;
    struct {
      BOOLEAN            have_any;
    }                      ggroup;
  }                       attributes;
}                                            EGgroupRec;

#endif /* GGROUP_TYPEDEFS */

/* methods for a graphic group; */
/* they are NEVER CALLED DIRECTLY -- always */
/* through the methods table (invoked by the generic methods listed */
/* in Egraphic.h). */

#ifdef GGROUP_PRIVATE_HEADER


static void         
Draw(EView *v_p, GraphicObj *p);
static void         
XorDraw(EView *v_p, GraphicObj *p);
static void         
Erase(EView *v_p, GraphicObj *p);
static void         
Hilite(EView *v_p, GraphicObj *p);
static void         
Unhilite(EView *v_p, GraphicObj *p);
static void         
BoundingBoxVC(EView *v_p, GraphicObj *p,
				 VCRec *ll, VCRec *ur);
static void         
BoundingBoxWC(GraphicObj *p, WCRec *ll, WCRec *ur);
static BOOLEAN      
HitByBox(EView *v_p, EBoxVC2DRec *box,
			    ESelectCriteria sel_crit, GraphicObj *p);
static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pnt);
static GraphicObj *
Translate(GraphicObj *p, WCRec *by);
static GraphicObj *
Rotate(GraphicObj *p,  WCRec *center, WCRec *axial);
static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
				  WCRec *point_on_plane, WCRec *plane_normal);
static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center);
static GraphicObj *
Scale(GraphicObj *p, WCRec *center,
		 FPNum sx, FPNum sy, FPNum sz);
static EGraphicType 
Type(GraphicObj *p);
static char        *
AsString(GraphicObj *p);
static EHandleNum   
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *hp);
static LIST         
KeyPoints(GraphicObj *p);
static void         
ShowHandles(EView *v_p, GraphicObj *p);
static void         
EraseHandles(EView *v_p, GraphicObj *p);
static void         
XYofHandle(GraphicObj *g_p, EHandleNum hn, WCRec *p);
static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp);
static void         
StoreOn(FILE *file_p, GraphicObj *g_p);
static GraphicObj *
DeepCopy(GraphicObj *g_p);
static void         
DeepDestroy(GraphicObj *g_p);       

static EGMethodsRec ggroup_graphic_methods = {
/*    TYPE_FUNC_P           */   Type,
/*    STRING_REP_FUNC_P     */   AsString,
/*    STORE_ON_FUNC_P       */   StoreOn,
/*    RETRIEVE_FROM_FUNC_P  */   RetrieveFrom,
/*    COPY_FUNC_P           */   NULL,
/*    DEEP_COPY_FUNC_P      */   DeepCopy,
/*    DESTROY_FUNC_P        */   NULL,
/*    DEEP_DESTROY_FUNC_P   */   DeepDestroy,
/*    INTERPOLATE_FUNC_P    */   NULL,
/*    BBOX_FUNC_VC_P        */   BoundingBoxVC,
/*    BBOX_FUNC_WC_P        */   BoundingBoxWC,
/*    DRAW_FUNC_P           */   Draw,
/*    XORDRAW_FUNC_P        */   XorDraw,
/*    HILITE_FUNC_P         */   Hilite,
/*    UNHILITE_FUNC_P       */   Unhilite,
/*    ERASE_FUNC_P          */   Erase,
/*    HIT_FUNC_P            */   HitByBox,
/*    KEY_POINT_LIST_FUNC_P */   KeyPoints,
/*    RESHAPE_HANDLE_FUNC_P */   ReshapeHandle,
/*    XY_OF_HANDLE_FUNC_P   */   XYofHandle,
/*    RESHAPE_FUNC_P        */   Reshape,
/*    MODIFY_GEOM_FUNC_P    */   NULL,
                                 NULL,
                                 NULL,
                                 NULL,
/*    TRANSLATE_FUNC_P      */   Translate,
/*    ROTATE_FUNC_P         */   Rotate,
/*    MIRROR_PLANE_FUNC_P   */   MirrorPlane,
/*    MIRROR_CENTER_FUNC_P  */   MirrorCenter,
/*    SCALE_FUNC_P          */   Scale,
/*    SHOW_HANDLES_FUNC_P   */   ShowHandles,
/*    ERASE_HANDLES_FUNC_P  */   EraseHandles,
				 NULL,
				 NULL,
				 NULL
};

#undef GGROUP_PRIVATE_HEADER

#endif /* GGROUP_PRIVATE_HEADER */

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateGgroup(LIST g_objects);
LIST         
GgroupGObjects(GraphicObj *g_p);

#ifdef __cplusplus
}
#endif

#endif
