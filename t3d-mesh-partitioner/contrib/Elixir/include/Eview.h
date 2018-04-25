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


#ifndef EVIEWER_H
#define EVIEWER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>


#include "Ecomptblt.h"
#include "Etypes.h"
#include "EPview.h"
#include "Egraphic.h"
#include "Etransfm.h"
#include "Egeomops.h"
#include "Erectvc.h"
#include "Ecolors.h"
#include "Efonts.h"

/* ======================================================================== */
/* PUBLIC FUNCTIONS  */
/* ======================================================================== */

EView  *EVInitView(Widget drawing_area,
		   Widget top_widget,
		   EPixel bg_pixel,
		   EPixel fg_pixel,
		   WCRec *vrp,
		   WCRec *normal,
		   WCRec *vup,
		   WCRec *vorigin,
		   FPNum view_x_dim, FPNum view_y_dim);
Widget EVFreeView(EView *v_p);

/* Widget */

EView *EVRetrieveViewP(Widget w);
Widget EVViewPToWidget(EView *v_p);

/* Layers */

void EVToggleLayerOnOff(EView *v_p, int layer);
void EVSetLayerOnOff(EView *v_p, int layer, BOOLEAN on);
void 
EVSetMultLayersOnOff(EView *v_p,
                     int layers[],  /* IN layer numbers */
                     BOOLEAN on[], /* IN flags whether layer[i] is to be */
                                   /* set on or off */
                     int dim    /* IN dimension of /layers/ and /on/ */
                     );
void 
EVFastSetMultLayersOnOff(EView *v_p,
												 int layers[],  /* IN layer numbers */
												 BOOLEAN on[], /* IN flags whether layer[i] is to be */
												               /* set on or off */
												 int dim    /* IN dimension of /layers/ and /on/ */
												 );
BOOLEAN EVGetLayerOnOff(EView *v_p, int layer);

/* Transforms */

void EVWCtoDC(EView *v_p, WCRec *p, int *DCx, int *DCy);
void EVDCtoWC(EView *v_p, int DCx, int DCy, WCRec *p);
void EVSetOriginVC(EView *v_p, WCRec *p);
void EVGetOriginVC(EView *v_p, WCRec *p);
void EVSetVectUp(EView *v_p, WCRec *v);
void EVGetVectUp(EView *v_p, WCRec *v);
void EVPreserveVectUp (EView *v_p, BOOLEAN preserve);
void EVSetNormal(EView *v_p, WCRec *v);
void EVGetNormal(EView *v_p, WCRec *v);
void EVSetVRP(EView *v_p, WCRec *p);
void EVGetVRP(EView *v_p, WCRec *p);
void EVDCtoVC(EView *v_p, int DCx, int DCy, VCRec *vcp);
void EVVCtoDC(EView *v_p, VCRec *vcp, int *DCx, int *DCy);
void EVVCtoWC(EView *v_p, VCRec *vcp, WCRec *wcp);
void EVWCtoVC(EView *v_p, WCRec *wcp, VCRec *vcp);
void EVSetDimsVC(EView *v_p, FPNum x, FPNum y);
void EVGetDimsVC(EView *v_p, FPNum *x, FPNum *y);
void EVGetDimsDC(EView *v_p, unsigned int *x_dim, unsigned int *y_dim);
void EVGetViewBox(EView *v_p, VCRec *ll, VCRec *ur);
void EVSetRotAngle(FPNum a);
void EVRotNormalUp(EView *v_p);
void EVRotNormalDown(EView *v_p);
void EVRotNormalLeft(EView *v_p);
void EVRotNormalRight(EView *v_p);
void EVSetupViewOrientation(EView *v_p);
void EVSetViewOrientation(EView *v_p, EViewOrientation vo);
void EVInhibitViewOrientationSetup(void);

/* GCs */

GC   EVDefaultXORGC(EView *v_p);
GC   EVDefaultCopyGC(EView *v_p);
GC   EVDefaultEraseGC(EView *v_p);
GC   EVHiliteGC(EView *v_p);
GC   EVEraseGC(EView *v_p);
GC   EVErasehiliteGC(EView *v_p);
GC   EVWritableGC(EView *v_p);
GC   EVConstrCopyGC(EView *v_p);
GC   EVConstrEraseGC(EView *v_p);
GC   EVConstrXORGC(EView *v_p);
   

/* Display lists */

void EVFlushAllDisplayLists(EView *v_p);
void EVFlushLayerDisplayList(EView *v_p, int layer);
void EVForceRedisplay(EView *v_p);
void EVUpdateDisplayLists(EView *v_p, GraphicObj *g_p);
void EVFastUpdateDisplayLists(EView *v_p);
BOOLEAN EVUsingBackBuffer(void);
Pixmap EVBackBuffer(void);

/* Input points */

void EVInputPoint(EView *v_p, XEvent *event, WCRec *p);
void EVConvertInputPoint(EView *v_p, int in_x, int in_y, WCRec *p);
void EVGetLastInputPoint(WCRec *p);
void EVSetLastInputPoint(WCRec *p);
EPixel EVGetForeground(EView *v_p);
EPixel EVGetBackground(EView *v_p);
void EVSetBackground(EView *v_p, EPixel bkg);
void EVSetForeground(EView *v_p, EPixel frg);
void EVLastPointerWCPos(WCRec *wcp);

/* Utilities */

void EVPromptMessage(char *);

/* Clipping */

void EVSetFGBGViewPlaneDepth(EView *v_p, FPNum fg_depth, FPNum bg_depth);
void EVSetFGViewPlaneDepth(EView *v_p, FPNum fg_depth);
void EVSetBGViewPlaneDepth(EView *v_p, FPNum bg_depth);
void EVGetFGBGViewPlaneDepth(EView *v_p, FPNum *fg_depth, FPNum *bg_depth);
void EVSetFGBGViewPlaneClipOnOff(EView *v_p, BOOLEAN flag);
BOOLEAN EVGetFGBGViewPlaneClipOnOff(EView *v_p);

void
EVSetModelClipPlane(EView *v_p, int which_plane,
                    WCRec *center, WCRec *normal, BOOLEAN on_flag);
void
EVGetModelClipPlane(EView *v_p, int which_plane,
                    WCRec *center, WCRec *normal, BOOLEAN *on);

void EVSetModelPlaneClipOnOff(EView *v_p, BOOLEAN flag);
BOOLEAN EVGetModelPlaneClipOnOff(EView *v_p);



/* handlers */

#define STARTING_INPUT_HANDLER 1
#define RESUMING_INPUT_HANDLER 2

void EVInstallHandler(EView *v_p, EventHandlerP handler,
			  StartProcP   start_proc,
			  caddr_t        start_proc_data,
			  SuspendProcP suspend_proc,
			  caddr_t        suspend_proc_data,
			  ResumeProcP  resume_proc,
			  caddr_t        resume_proc_data,
			  RemoveProcP  remove_proc,
			  caddr_t        remove_proc_data);
void EVUninstallHandler(EView *v_p);
void EVSuspendActiveHandler(EView *v_p);
void EVResumeSuspendedHandler(EView *v_p);
void EVFastViewHandler(Widget w, EView *v_p, XEvent *event);
void EVSaveSentPoint(WCRec *p);
void EVRetrieveSentPoint(WCRec *p);
void EVSetApplyToViewFunction(ApplyFuncP func_p, caddr_t data_p, ResetFuncP reset_func_p);
void EVSetApplyToViewPreventRedisplay(BOOLEAN flag);
void EVSetApplyToViewPrompt(int class_number, int serial_number, char *p);
void EVApplyToViewHandler(Widget w, EView *v_p, XEvent *event);
void EVSelectGraphicsHandler(Widget w, EView *v_p, XEvent *event);
void EVUnhiliteGraphics(EView *v_p, GraphicObj *g_p);
void EVHiliteGraphics(EView *v_p, GraphicObj *g_p);
GraphicObj *EVGetFromSelectionList(void);
GraphicObj *EVFirstInSelectionList(void);
GraphicObj *EVNextInSelectionList(GraphicObj *g_p);
int EVItemsInSelectionList(void);
BOOLEAN EVIsPointerIn(void);
void EVSetAttribMask(unsigned long mask);
unsigned long EVGetAttribMask(void);
void EVSetPickableMask(unsigned long mask);
unsigned long EVGetPickableMask(void);
void EVSetTakeAsNOT(BOOLEAN flag);
void EVSetUseFastRedrawRedisplay(BOOLEAN flag);

/* display of graphics */

void EVDisplayGraphics(EView *v_p, GraphicObj *g_p, BOOLEAN immed_draw);
void EVUnlinkGraphics(EView *v_p, GraphicObj *g_p);
void EVUnlinkMarkedGraphics(EView *v_p);

void EVDrawGraphics(EView *v_p, GraphicObj *g_p);
void EVXORDrawGraphics(EView *v_p, GraphicObj *g_p);
void EVEraseGraphics(EView *v_p, GraphicObj *g_p);
void EVHiliteGraphics(EView *v_p, GraphicObj *g_p);
void EVUnhiliteGraphics(EView *v_p, GraphicObj *g_p);

void EVFitAllIntoView(EView *v_p);
void EVRedrawContents(EView *v_p);
void EVFastRedraw(EView *view);

/* grid */

void EVShowGrid(EView *v_p, BOOLEAN on_off);
void EVSetGridLock(EView *v_p, BOOLEAN on_off);
void EVSetGridOrigin(EView *v_p, FPNum x, FPNum y);
void EVSetGridDeltas(EView *v_p, FPNum delta_x, FPNum delta_y);

/* Coordinate axes */

void
EVShowAxes(EView *v_p, BOOLEAN on_off);
void
EVToggleAxes(EView *v_p);

/* Color scale */

void
EVShowScale(EView *v_p, BOOLEAN on_off);
void
EVToggleScale(EView *v_p);

/* Status */

void
EVShowStatus(EView *v_p, BOOLEAN on_off);
void
EVToggleStatus(EView *v_p);

/* construction plane */

void 
EVSetConstrPlaneLock(EView *v_p, BOOLEAN on);
void
EVSetConstrPlaneCenter(EView *v_p, WCRec *center);
void
EVGetConstrPlaneCenter(EView *v_p, WCRec *center);
void
EVSetConstrPlaneNormal(EView *v_p, WCRec *normal);
void
EVSetConstrPlaneOrientation(EView *v_p, WCRec *normal, WCRec *vup);
void
EVGetConstrPlaneOrientation(EView *v_p, WCRec *u, WCRec *v, WCRec *normal);

void EVSetSelectActionFunction(SelectActionFuncP func_p,
				   caddr_t data_p,
				   ResetFuncP reset_func_p);
void EVClearSelection(void);


/* Model */

void EVSetModel(EView *v_p, struct EModel *model);
struct EModel *EVGetModel(EView *v_p);

/* Fringe table */

EFringeTable EVGetAssocFringeTable(EView *v_p);
EFringeTable EVSetAssocFringeTable(EView *v_p, EFringeTable ft);

/* Drawing functions */

void 
EVBCBezS3DPrimitive(EView        *v_p,
                         GraphicObj   *g_p,
			 WCRec         wcpoints[4][4],  
			 EPixel        pixel,
			 BOOLEAN       fill,
                         int           edge_flag,
                         EPixel        edge_pixel,
			 int           tessel_intervals,   
			 BOOLEAN       show_poly,   
			 BOOLEAN       boundary_only,
			 EDrawMode     draw_mode);
void
EVCBezC3DPrimitive(EView         *v_p, 
                        GraphicObj    *g_p,        
			WCRec          wcpoints[4],
			int            style,
			EPixel         pixel,
			unsigned       width,
			int            tessel_intervals,   
			BOOLEAN        show_poly,
			EDrawMode      draw_mode);
void 
EVLine3DPrimitive(EView         *v_p,	
                  GraphicObj    *g_p,        
                  WCRec         *f,
                  WCRec         *s,
                  int           style,
                  EPixel        pixel,
                  unsigned      width,
									float         shrink,
                  EDrawMode     draw_mode);
void 
EVTriangle3DPrimitive(EView        *v_p,
                      GraphicObj   *g_p,        
		      WCRec        *f,
		      WCRec        *s,
		      WCRec        *t,
		      EPixel        pixel,
		      int           fill,
                      int           edge_flag,
                      EPixel        edge_pixel,
		      float         shrink,
		      EDrawMode     draw_mode);
void 
EVTriangleWD3DPrimitive(EView        *v_p,
                        GraphicObj   *g_p,        
                        WCRec        *f,
                        WCRec        *s,
                        WCRec        *t,
                        double        val1,
                        double        val2,
                        double        val3,
                        EPixel        pixel,
                        int           fill,
                        int           edge_flag,
                        EPixel        edge_pixel,
                        float         shrink,
                        EDrawMode     draw_mode);
void 
EVAnnText3DPrimitive(EView         *v_p,
                     GraphicObj    *g_p,        
		     WCRec         *at,
		     char          *text,
		     Font          font_id,
		     EPixel        pixel,
		     EDrawMode     draw_mode);
void 
EVMarker3DPrimitive(EView         *v_p,
                    GraphicObj    *g_p,        
		    WCRec         *center,
		    unsigned      size,
		    EMarkerType   type,
		    EPixel        pixel,
		    EDrawMode     draw_mode);
void 
EVTetraPrimitive(EView        *v_p,
                 GraphicObj    *g_p,        
                 WCRec        *first,
                 WCRec        *second,
                 WCRec        *third,
                 WCRec        *fourth,
                 EPixel        pixel,
                 int           fill,
                 int           edge_flag,
                 EPixel        edge_pixel,
                 float         shrink,
                 EDrawMode     draw_mode);
void 
EVQuadWD3DPrimitive(EView        *v_p,
                    GraphicObj    *g_p,        
                    WCRec        *first,
                    WCRec        *second,
                    WCRec        *third,
                    WCRec        *fourth,
                    double        val1,
                    double        val2,
                    double        val3,
                    double        val4,
                    EPixel        pixel,
                    int           fill,
                    int           edge_flag,
                    EPixel        edge_pixel,
                    float         shrink,
                    EDrawMode     draw_mode);
void 
EVQuad3DPrimitive(EView        *v_p,
                  GraphicObj    *g_p,        
                  WCRec        *first,
                  WCRec        *second,
                  WCRec        *third,
                  WCRec        *fourth,
                  EPixel        pixel,
                  int           fill,
                  int           edge_flag,
                  EPixel        edge_pixel,
                  float         shrink,
                  EDrawMode     draw_mode);
void 
EVRAPrismPrimitive(EView        *v_p,
                   GraphicObj   *g_p,
                   WCRec        *lower_left_corner,
                   WCRec        *upper_right_corner,
                   EPixel        pixel,
                   int           fill,
                   int           edge_flag,
                   EPixel        edge_pixel,
                   float         shrink,
                   EDrawMode     draw_mode);
void 
EVHexahPrimitive(EView        *v_p,
                 GraphicObj   *g_p,
                 WCRec         points[8],
                 EPixel        pixel,
                 int           fill,
                 int           edge_flag,
                 EPixel        edge_color,
                 float         shrink,
                 EDrawMode     draw_mode);

void 
EVHexahWDPrimitive(EView        *v_p,
                   GraphicObj   *g_p,
                   WCRec         points[8],
                   FPNum         values[8],
                   EPixel        pixel,
                   int           fill,
                   int           edge_flag,
                   EPixel        edge_color,
                   float         shrink,
                   EDrawMode     draw_mode);

void 
EVVecMarker3DPrimitive(EView      *v_p,
                    GraphicObj   *g_p, 
		    WCRec      *center,
		    WCRec      *direction,
		    unsigned    size,
		    int         shift,
				FPNum       rate,
		    EVecMarkerType type,
		    EPixel      pixel,
		    EDrawMode   draw_mode);

void 
EVVector3DPrimitive(EView      *v_p,
                    GraphicObj   *g_p, 
		    WCRec      *origin,
		    WCRec      *comp,
		    EVecMarkerType type,
		    FPNum       scale,
		    FPNum       rate,
		    EPixel      pixel,
        BOOLEAN     use_ftable,
        BOOLEAN     shift_flag,
		    EDrawMode   draw_mode);

void 
EVRBezC3DPrimitive(EView        *v_p,
                   GraphicObj   *g_p,
                   int           order,
		   WCRec         *wcpoints,
                   float         *weights,
		   int           style,
		   EPixel        pixel,
		   unsigned      width,
		   int           tessel_intervals,   
		   BOOLEAN       show_poly,
		   EDrawMode     draw_mode);

void 
EVRBezS3DPrimitive(EView        *v_p,
                   GraphicObj   *g_p, 
                   int           order_u,
                   int           order_v,
                   WCRec         *wcpoints,
                   float         *weights,
                   int           style,
                   EPixel        pixel,
                   unsigned      line_width,
                   int           fill,
                   int           edge_flag,
                   EPixel        edge_pixel,
                   int           tessel_u,   
                   int           tessel_v,   
                   BOOLEAN       show_poly,   
                   BOOLEAN       boundary_only,
                   EDrawMode     draw_mode);
void 
EVTetraWDPrimitive(EView        *v_p,
                   GraphicObj   *g_p,
                   WCRec        *first,
                   WCRec        *second,
                   WCRec        *third,
                   WCRec        *fourth,
                   FPNum         values[4],
                   EPixel        pixel,
                   int           fill,
                   int           edge_flag,
                   EPixel        edge_color,
                   float         shrink,
                   EDrawMode     draw_mode);



ERenderingType EVGetRenderMode(EView *v_p);
void EVSetRenderMode(EView *v_p, ERenderingType mode);
void EVSetShadeMode(EView *v_p, EShadingType mode);
EShadingType EVGetShadeMode(EView *v_p);
void EVStoreRenderMode(EView *v_p);
void EVRestoreRenderMode(EView *v_p);
void EVSetDirTowardsLight(EView *v_p, VCRec *vect);
FPNum EVGetLightDirVsNormalCos(EView *v_p, VCRec *normal);

/* ======================================================================== */
/* DEFINES  */
/* ======================================================================== */

#define TRACK_POINT /* tracks the pointer while fast-viewer active */

#ifdef __cplusplus
}
#endif

#endif
