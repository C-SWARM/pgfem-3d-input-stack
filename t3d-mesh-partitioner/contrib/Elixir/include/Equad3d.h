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


#ifndef EQUAD3D_H
#define EQUAD3D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Egraphic.h"


/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

#ifdef QUAD3D_PRIVATE_HEADER
#   define QUAD3D_TYPEDEFS
#endif

#ifdef QUAD3D_TYPEDEFS

/* Data structures; never to be accessed from application programs */

typedef struct EQuad3DWCRec {
  WCRec                             point_1;
  WCRec                             point_2;
  WCRec                             point_3;
  WCRec                             point_4;
  union {
    AttributesRec            any;
    struct {
      BOOLEAN         have_any;
      short int       fill_style;
      short int       edge_flag;
      EPixel          color;
      EPixel          edge_color;
      float           shrink;
    }                         spec;
  }                                  attributes;
}                                            EQuad3DWCRec;

#endif /* QUAD3D_TYPEDEFS */

#ifdef QUAD3D_PRIVATE_HEADER

/* methods for a general quadrangle; they are NEVER CALLED DIRECTLY -- always*/
/* through the methods table (invoked by the generic methods listed */
/* in Egraphic.h). */

static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *points);
static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints);
static void 
Draw(EView *v_p, GraphicObj *p);
static EGraphicType 
Type(GraphicObj *g_p);
static char *
AsString(GraphicObj *g_p);
static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp);
static void 
StoreOn(FILE *file_p, GraphicObj *g_p);
static void 
BoundingBoxVC(EView *v_p, GraphicObj *g_p,
                    VCRec *ll_return, VCRec *ur_return);
static void 
BoundingBoxWC(GraphicObj *g_p,
                    WCRec *ll_return, WCRec *ur_return);
static void 
XorDraw(EView *v_p, GraphicObj *g_p);
static void 
Hilite(EView *v_p, GraphicObj *g_p);
static void 
Unhilite(EView *v_p, GraphicObj *g_p);
static void 
Erase(EView *v_p, GraphicObj *g_p);
static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by);
static GraphicObj *
Rotate(GraphicObj *g_p, WCRec *center, WCRec *axial);
static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
				  WCRec *point_on_plane, WCRec *plane_normal);
static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center);
static GraphicObj *
Scale(GraphicObj *g_p, WCRec *center, 
		   FPNum sx, FPNum sy, FPNum sz);
static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
			   ESelectCriteria sel_crit, GraphicObj *p);
static LIST 
KeyPoints(GraphicObj *p);
static GraphicObj *
Reshape(GraphicObj *p, EHandleNum hn, WCRec *pos);
static EHandleNum 
ReshapeHandle(EView *v_p, GraphicObj *p, WCRec *nearp);
static void 
ShowHandles(EView *v_p, GraphicObj *p);
static void 
EraseHandles(EView *v_p, GraphicObj *p);
static void 
XYofHandle(GraphicObj *p, EHandleNum hn, WCRec *pos);
static void 
ChangeAttributes(GraphicObj *p);
static void 
CopyAttributesToActiveSet(GraphicObj *p);
static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask);

EGMethodsRec quad_3d_graphic_methods = {
/*    TYPE_FUNC_P           */   Type,
/*    STRING_REP_FUNC_P     */   AsString,
/*    STORE_ON_FUNC_P       */   StoreOn,
/*    RETRIEVE_FROM_FUNC_P  */   RetrieveFrom,
/*    COPY_FUNC_P           */   NULL,
/*    DEEP_COPY_FUNC_P      */   NULL,
/*    DESTROY_FUNC_P        */   NULL,
/*    DEEP_DESTROY_FUNC_P   */   NULL,
/*    INTERPOLATE_FUNC_P    */   NULL,
/*    BBOX_FUNC_P           */   BoundingBoxVC,
/*    BBOX_FUNC_P           */   BoundingBoxWC,
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
/*    MODIFY_GEOM_FUNC_P    */   ModifyGeometry,
                                 GetGeometry,
                                 NULL,
                                 NULL,
/*    TRANSLATE_FUNC_P      */   Translate,
/*    ROTATE_FUNC_P         */   Rotate,
/*    MIRROR_PLANE_FUNC_P   */   MirrorPlane,
/*    MIRROR_CENTER_FUNC_P  */   MirrorCenter,
/*    SCALE_FUNC_P          */   Scale,
/*    SHOW_HANDLES_FUNC_P   */   ShowHandles,
/*    ERASE_HANDLES_FUNC_P  */   EraseHandles,
/*    CHANGE_ATTRIB_FUNC_P  */   ChangeAttributes,
/*    COPY_ATTRIB_FUNC_P    */   CopyAttributesToActiveSet,
				 AttribValAsInAS
};

#undef QUAD3D_PRIVATE_HEADER

#else

extern EGMethodsRec quad_3d_graphic_methods;

#endif /* QUAD3D_PRIVATE_HEADER */


/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateQuad3D(WCRec *points);

#ifdef __cplusplus
}
#endif

#endif
