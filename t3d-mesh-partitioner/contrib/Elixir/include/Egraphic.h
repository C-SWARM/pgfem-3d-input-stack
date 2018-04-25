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


#ifndef EGRAPHIC_H
#define EGRAPHIC_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include "Etypes.h"
#include "listP.h"
#include "Eobject.h"

struct GraphicObj;
struct EBoxVC2DRec;


typedef unsigned long EMask;
typedef EMask EMaskArray[2];

typedef unsigned long EGraphicId;

/* NOTE: The numbers must follow each other in ascending */
/* order by increment 1 starting at zero, the last item number must */
/* give the (EG_MAX_METHOD_TABLE = last-item-number + 1) */

typedef enum {   EG_UNKNOWN_TYPE       =  0,
		 EG_LINE3D             =  1,
		 EG_RECTANGLE_VC       =  2,
		 EG_MARKER3D           =  3,
		 EG_ANNTEXT3D          =  4,
		 EG_GGROUP             =  5,
		 EG_CTLBLOCK           =  6,
                 EG_TRIANGLE3D         =  7,
                 EG_CBEZCURVE3D        =  8,
                 EG_BCBEZSURFACE3D     =  9,
                 EG_TRIANGLEWD3D       = 10,
                 EG_QUADWD3D           = 11,
                 EG_TETRAHEDRON        = 12,
                 EG_RIGHT_ANGLE_PRISM  = 13,
                 EG_BCB_HEXAHEDRON     = 14,
                 EG_QUAD3D             = 15,
                 EG_PYRAMID            = 16,
                 EG_HEXAHEDRON         = 17,
                 EG_VECMARKER3D        = 18,
                 EG_VECTOR3D           = 19,
                 EG_RBEZCURVE3D        = 20,
                 EG_RBEZSURFACE3D      = 21,
                 EG_HEXAHEDRONWD       = 22,
                 EG_TETRAHEDRONWD      = 23,
                 EG_MARKERWD3D         = 24,
                 EG_MAX_METHOD_TABLE   
	     }                EGraphicType;

typedef int EHandleNum;

typedef struct GraphicSpecObj {
  caddr_t                  data;
  int                      data_length;
} GraphicSpecObj;

typedef void (*StoreOnFuncP)(FILE *file_p, struct GraphicObj *g_p);
typedef void (*RetrieveFromFuncP)(FILE *file_p, GraphicSpecObj *gsp);
typedef void (*InterpolateFuncP)(FPNum eta, struct GraphicObj *g_p,
                                 FPNum *x_return, FPNum *y_return);
typedef void (*DrawFuncP)(EView *, struct GraphicObj *);
typedef void (*XordrawFuncP)(EView *, struct GraphicObj *);
typedef void (*EraseFuncP)(EView *, struct GraphicObj *);
typedef void (*HiliteFuncP)(EView *, struct GraphicObj *);
typedef void (*UnhiliteFuncP)(EView *, struct GraphicObj *);
typedef void (*BBoxFuncVCP)(EView *, struct GraphicObj *, VCRec *, VCRec*);
typedef void (*BBoxFuncWCP)(struct GraphicObj *, WCRec *, WCRec*);
typedef BOOLEAN (*HitFuncP)(EView *, struct EBoxVC2DRec *, ESelectCriteria,
			    struct GraphicObj *);
typedef struct GraphicObj *(*TranslateFuncP)(struct GraphicObj *, WCRec *);
typedef struct GraphicObj *(*ReshapeFuncP)(struct GraphicObj *,
					    EHandleNum, WCRec *);
typedef struct GraphicObj *(*ModifyGeomFuncP)(struct GraphicObj *, WCRec *);
typedef BOOLEAN (*GetGeomFuncP)(struct GraphicObj *, WCRec *, int *);
typedef struct GraphicObj *(*ModifyAssocDataFuncP)(struct GraphicObj *,
                                                    FPNum *);
typedef BOOLEAN (*GetAssocDataFuncP)(struct GraphicObj *, FPNum *, int *);
typedef struct GraphicObj *(*RotateFuncP)(struct GraphicObj *,
					   WCRec *, WCRec *);
typedef struct GraphicObj *(*MirrorPlaneFuncP)(struct GraphicObj *,
						WCRec *, WCRec *);
typedef struct GraphicObj *(*MirrorCenterFuncP)(struct GraphicObj *,WCRec *);
typedef struct GraphicObj *(*ScaleFuncP)(struct GraphicObj *,
					  WCRec *, FPNum, FPNum, FPNum);
typedef struct GraphicObj *(*CopyFuncP)(struct GraphicObj *);
typedef struct GraphicObj *(*DeepCopyFuncP)(struct GraphicObj *);
typedef void (*DestroyFuncP)(struct GraphicObj *);
typedef void (*DeepDestroyFuncP)(struct GraphicObj *);
typedef EGraphicType (*TypeFuncP)(struct GraphicObj *);
typedef char *(*StringRepFuncP)(struct GraphicObj *);
typedef LIST (*KeyPointListFuncP)(struct GraphicObj *);
typedef EHandleNum (*ReshapeHandleFuncP)(EView *v_p, struct GraphicObj *,
						       WCRec *);
typedef void (*XYOfHandleFuncP)(struct GraphicObj *,
				EHandleNum handle, WCRec *);
typedef void (*ShowHandlesFuncP)(EView *, struct GraphicObj *);
typedef void (*EraseHandlesFuncP)(EView *, struct GraphicObj *);
typedef void (*ChangeAttribFuncP)(struct GraphicObj *);
typedef void (*CopyAttribFuncP)(struct GraphicObj *);
typedef BOOLEAN (*AttribValAsInASFuncP)(struct GraphicObj *,
					unsigned long mask);

typedef struct KeyPointRec {
  WCRec                coords;
  FPNum                dist_from_hit;
}    KeyPointRec;

typedef struct EGMethodsRec {
  TypeFuncP              type_func;
  StringRepFuncP         string_rep_func;
  StoreOnFuncP           store_on_func;
  RetrieveFromFuncP      retrieve_from_func;
  CopyFuncP              copy_func;
  DeepCopyFuncP          deep_copy_func;
  DestroyFuncP           destroy_func;
  DeepDestroyFuncP       deep_destroy_func;
  InterpolateFuncP       interpolate_func;
  BBoxFuncVCP            bbox_func_VC;
  BBoxFuncWCP            bbox_func_WC;
  DrawFuncP              draw_func;
  XordrawFuncP           xordraw_func;
  HiliteFuncP            hilite_func;
  UnhiliteFuncP          unhilite_func;
  EraseFuncP             erase_func;
  HitFuncP               hit_func;
  KeyPointListFuncP      key_point_list_func;
  ReshapeHandleFuncP     reshape_handle_func;
  XYOfHandleFuncP        xy_of_handle_func;
  ReshapeFuncP           reshape_func;
  ModifyGeomFuncP        modify_geom_func;
  GetGeomFuncP           get_geom_func;
  ModifyAssocDataFuncP   modify_assoc_data_func;
  GetAssocDataFuncP      get_assoc_data_func;
  TranslateFuncP         translate_func;
  RotateFuncP            rotate_func;
  MirrorPlaneFuncP       mirror_plane_func;
  MirrorCenterFuncP      mirror_center_func;
  ScaleFuncP             scale_func;
  ShowHandlesFuncP       show_handles_func;
  EraseHandlesFuncP      erase_handles_func;
  ChangeAttribFuncP      change_attrib_func;
  CopyAttribFuncP        copy_attrib_func;
  AttribValAsInASFuncP   attrib_val_as_in_AS_func;
}                                               EGMethodsRec;


/* ===================================================================== */
#define partition_1_mask                (1L<<1)
/* ===================================================================== */
#define type_func_mask1                 (1L<<1)
#define type_func_mask2                 partition_1_mask
#define string_rep_func_mask1           (1L<<2)
#define string_rep_func_mask2           partition_1_mask
#define store_on_func_mask1             (1L<<3)
#define store_on_func_mask2             partition_1_mask
#define retrieve_from_func_mask1        (1L<<4)
#define retrieve_from_func_mask2        partition_1_mask
#define copy_func_mask1                 (1L<<5)
#define copy_func_mask2                 partition_1_mask
#define deep_copy_func_mask1            (1L<<6)
#define deep_copy_func_mask2            partition_1_mask
#define destroy_func_mask1              (1L<<7)
#define destroy_func_mask2              partition_1_mask
#define deep_destroy_func_mask1         (1L<<8)
#define deep_destroy_func_mask2         partition_1_mask
#define interpolate_func_mask1          (1L<<9)
#define interpolate_func_mask2          partition_1_mask
#define bbox_func_VC_mask1              (1L<<10)
#define bbox_func_VC_mask2              partition_1_mask
#define bbox_func_WC_mask1              (1L<<11)
#define bbox_func_WC_mask2              partition_1_mask
#define draw_func_mask1                 (1L<<12)
#define draw_func_mask2                 partition_1_mask
#define xordraw_func_mask1              (1L<<13)
#define xordraw_func_mask2              partition_1_mask
#define hilite_func_mask1               (1L<<14)
#define hilite_func_mask2               partition_1_mask
#define unhilite_func_mask1             (1L<<15)
#define unhilite_func_mask2             partition_1_mask
#define erase_func_mask1                (1L<<16)
#define erase_func_mask2                partition_1_mask
#define hit_func_mask1                  (1L<<17)
#define hit_func_mask2                  partition_1_mask
#define key_point_list_func_mask1       (1L<<18)
#define key_point_list_func_mask2       partition_1_mask
#define reshape_handle_func_mask1       (1L<<19)
#define reshape_handle_func_mask2       partition_1_mask
#define xy_of_handle_func_mask1         (1L<<20)
#define xy_of_handle_func_mask2         partition_1_mask
#define reshape_func_mask1              (1L<<21)
#define reshape_func_mask2              partition_1_mask
#define modify_geom_func_mask1          (1L<<22)
#define modify_geom_func_mask2          partition_1_mask
#define get_geom_func_mask1             (1L<<23)
#define get_geom_func_mask2             partition_1_mask
#define modify_assoc_data_func_mask1    (1L<<24)
#define modify_assoc_data_func_mask2    partition_1_mask
#define get_assoc_data_func_mask1       (1L<<25)
#define get_assoc_data_func_mask2       partition_1_mask
#define translate_func_mask1            (1L<<26)
#define translate_func_mask2            partition_1_mask
#define rotate_func_mask1               (1L<<27)
#define rotate_func_mask2               partition_1_mask
#define mirror_plane_func_mask1         (1L<<28)
#define mirror_plane_func_mask2         partition_1_mask
#define mirror_center_func_mask1        (1L<<29)
#define mirror_center_func_mask2        partition_1_mask
#define scale_func_mask1                (1L<<30)
#define scale_func_mask2                partition_1_mask
/* ===================================================================== */
#define partition_2_mask                (1L<<2)
/* ===================================================================== */
#define show_handles_func_mask1         (1L<<1)
#define show_handles_func_mask2         partition_2_mask
#define erase_handles_func_mask1        (1L<<2)
#define erase_handles_func_mask2        partition_2_mask
#define change_attrib_func_mask1        (1L<<3)
#define change_attrib_func_mask2        partition_2_mask
#define copy_attrib_func_mask1          (1L<<4)
#define copy_attrib_func_mask2          partition_2_mask
#define attrib_val_as_in_AS_func_mask1  (1L<<5)
#define attrib_val_as_in_AS_func_mask2  partition_2_mask



typedef struct GraphicObj {
  EGraphicId               serial_id;
  EGraphicId               time_id;
  EGMethodsRec            *methods;
  GraphicSpecObj           spec;
  short int                layer;
  unsigned long            pickable_mask;
  EObjectP                 app_obj;
	EBool                    marked;
  EBool                    selected;
  EBool                    invisible_when_clipped;
  EBool                    not_clippable;
}                                              GraphicObj;

typedef struct AttributesRec {
  BOOLEAN           have_any;
}                                            AttributesRec;


typedef struct EBoxVC2DRec {
  FPNum left, bottom, right, top;
}                                            EBoxVC2DRec;
   

typedef enum {
	    CIRCLE_MARKER,
		  FILLED_CIRCLE_MARKER,
		  SQUARE_MARKER,
		  X_LETTER_MARKER
		}                     EMarkerType;


typedef enum { 
      ARROW_VECMARKER,
      DOUBLE_ARROW_VECMARKER,
      TRIPLE_ARROW_VECMARKER,
      TRIANGLE_VECMARKER
    }                     EVecMarkerType ;



/* ========================================================================= */
/* PUBLIC FUNCTIONS */
/* ========================================================================= */

void         EGInitGraphics(void);
GraphicObj  *EGCreateGraphics(EGMethodsRec *EGMethodsRec_p,
			      caddr_t data_p, int data_length);
EGraphicType EGGraphicType(GraphicObj *g_p);
void         EGGraphicStoreOn(FILE *file_p, GraphicObj *g_p);
GraphicObj  *EGGraphicRetrieveFrom(FILE *file_p);
void         EGDrawGraphics(EView *v_p, GraphicObj *g_p);
void         EGEraseGraphics(EView *v_p, GraphicObj *g_p);
void         EGXORDrawGraphics(EView *v_p, GraphicObj *g_p);
void         EGHiliteGraphics(EView *v_p, GraphicObj *g_p);
void         EGUnhiliteGraphics(EView *v_p, GraphicObj *g_p);
void         EGGraphicsBoundingBoxVC(EView *v_p, GraphicObj *g_p,
				     VCRec *ll, VCRec *ur);
void         EGGraphicsBoundingBoxWC(GraphicObj *g_p,
				     WCRec *ll, WCRec *ur);
BOOLEAN      EGGraphicHitByBox(EView *v_p, EBoxVC2DRec *box,
			      ESelectCriteria crit, GraphicObj *g_p);
void         EGDestroyGraphics(GraphicObj *g_p);
BOOLEAN      EGIsSelected(GraphicObj *g_p);
void         EGSetSelected(GraphicObj *g_p, BOOLEAN selected);
BOOLEAN      EGIsMarked(GraphicObj *g_p);
void         EGSetMarked(GraphicObj *g_p, BOOLEAN marked);
void 
EGSetNotClippable(GraphicObj *g_p, BOOLEAN flag);
BOOLEAN 
EGIsClippable(GraphicObj *g_p);
void 
EGSetInvisibleWhenClipped(GraphicObj *g_p, BOOLEAN flag);
BOOLEAN 
EGInvisibleWhenClipped(GraphicObj *g_p);
void EGSetLayer(GraphicObj *g_p, int layer);
int EGGetLayer(GraphicObj *g_p);
EGraphicId   EGGraphicSerialID(GraphicObj *g_p);
EGraphicId   EGGraphicTimeID(GraphicObj *g_p);
void         EGDestroyGraphics(GraphicObj *g_p);
void         EGDeepDestroyGraphics(GraphicObj *g_p);
GraphicObj  *EGCopyGraphics(GraphicObj *g_p);
GraphicObj  *EGDeepCopyGraphics(GraphicObj *g_p);
GraphicObj  *EGModifyGraphicsGeometry(GraphicObj *g_p, WCRec *points);
BOOLEAN      EGGetGraphicsGeometry(GraphicObj *g_p, WCRec *points,
                                   int *npoints);
GraphicObj  *EGModifyAssocData(GraphicObj *g_p, FPNum *vals);
BOOLEAN      EGGetAssocData(GraphicObj *g_p, FPNum *points, int *npoints);
GraphicObj  *EGModifyGraphicsByHandle(GraphicObj *g_p,
				      EHandleNum hn, WCRec *new_pos);
EHandleNum   EGReshapeHandle(EView *v_p, GraphicObj *p, WCRec *near_point);
LIST         EGGraphicKeyPointList(GraphicObj *g_p);
void         EGDrawHandle(EView *v_p, WCRec *h_pos);
void         EGEraseHandle(EView *v_p, WCRec *h_pos);
void         EGGraphicShowHandles(EView *v_p, GraphicObj *g_p);
void         EGGraphicEraseHandles(EView *v_p, GraphicObj *g_p);
void         EGXYofGraphicsHandle(GraphicObj *g_p,
				  EHandleNum hn, WCRec *h_pos);
char        *EGStringGraphicRepresentation(GraphicObj *g_p);
GraphicObj  *EGTranslateGraphic(GraphicObj *p, WCRec *by);
GraphicObj * EGScaleGraphic(GraphicObj *g_p, WCRec *center, 
                     FPNum sx, FPNum sy, FPNum sz);
GraphicObj  *EGRotateGraphic(GraphicObj *g_p, WCRec *center, WCRec *axial);
GraphicObj  *EGMirrorPlaneGraphic(GraphicObj *g_p,
				  WCRec *point_on_plane, WCRec *plane_normal);
GraphicObj  *EGMirrorCenterGraphic(GraphicObj *g_p, WCRec *center);
int          EGModifyCurveDrawGC(Display *dsp, GC agc,
				int style, int width, EPixel color,
				int cap, int join);
void         EGChangeAttributes(GraphicObj *g_p);
void EGWithMaskChangeAttributes(unsigned long mask, GraphicObj *g_p);
void         EGCopyAttributesToActiveSet(GraphicObj *g_p);
BOOLEAN      EGAttribValAsInAS(GraphicObj *g_p, unsigned long mask);
void          EGSetPickableMask(GraphicObj *g_p, unsigned long mask);
unsigned long EGGetPickableMask(GraphicObj *g_p);


/* Object */

void         
EGAttachObject(GraphicObj *g_p, EObjectP appobj);
EGMethodsRec *
EGCopyMethodTable(GraphicObj *g_p,
                  EMaskArray mask_array, EGMethodsRec *arg);
EGMethodsRec *
EGOverrideMethodTable(GraphicObj *g_p, EGMethodsRec *new_mt);
EObjectP
EGGetAppObj(GraphicObj *g_p);


#define EGSetOverrideEntry(theMaskArray, theMethodTableP,               \
                           theMethodName, theNewEntry)                  \
{                                                                       \
  theMaskArray[0] |= theMethodName##_mask1;                             \
  theMaskArray[1] |= theMethodName##_mask2;                             \
  theMethodTableP->theMethodName = theNewEntry;                         \
}

/* Registration of methods (persistence) */

BOOLEAN      
EGRegisterMethods(GraphicObj *g_p);


#define EGMethodIsDefined(G_P, METHOD_NAME)                   \
         ((G_P)->methods->METHOD_NAME != NULL)

#define EGInvokeMethod(G_P, METHOD_NAME, METHOD_ARGS)         \
    (*((G_P)->methods->METHOD_NAME)) METHOD_ARGS            

#define EGMethodIsDefinedInMethodTable(METHOD_TABLE, METHOD_NAME)        \
         ((METHOD_TABLE)->METHOD_NAME != NULL)

#define EGInvokeMethodUsingMethodTable(METHOD_TABLE,                     \
                                       METHOD_NAME, METHOD_ARGS)         \
    (*((METHOD_TABLE)->METHOD_NAME)) METHOD_ARGS            


#ifdef CAN_USE_MACROS_INSTEAD_OF_FUNCS

#   define EGSetSelected(G_P, SELECTED) ((G_P)->selected = (SELECTED))

#   define EGIsSelected(G_P) ((G_P)->selected)

#   define EGSetMarked(G_P, MARKED) ((G_P)->marked = (MARKED))

#   define EGIsMarked(G_P) ((G_P)->marked)

#   define EGSetLayer(G_P, LAYER) ((G_P)->layer = (LAYER))

#   define EGGetLayer(G_P) ((G_P)->layer)

#   define EGDrawGraphics(V_P, G_P) \
     if ((G_P)->methods->draw_func != NULL) \
       (*((G_P)->methods->draw_func))((V_P), (G_P))

#   define EGXORDrawGraphics(V_P, G_P) \
     if ((G_P)->methods->xordraw_func != NULL) \
       (*((G_P)->methods->xordraw_func))((V_P), (G_P))

#   define EGGraphicsBoundingBoxVC(V_P, G_P, LL, UR)                   \
  {                                                                    \
    if ((G_P)->methods->bbox_func_VC != NULL)                          \
      (*((G_P)->methods->bbox_func_VC))((V_P), (G_P), (LL), (UR));     \
    else                                                               \
      (LL)->u = (LL)->v = (LL)->n = (UR)->u = (UR)->v = (UR)->n = 0.0; \
  } 

#   define EGIsClippable(G_P) (!(G_P)->not_clippable)

#   define EGInvisibleWhenClipped(G_P) ((G_P)->invisible_when_clipped)


#endif /* CAN_USE_MACROS_INSTEAD_OF_FUNCS */

#ifdef __cplusplus
}
#endif

#endif












