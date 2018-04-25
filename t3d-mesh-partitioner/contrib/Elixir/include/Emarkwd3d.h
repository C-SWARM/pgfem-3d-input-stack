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

#ifndef EMARKWD3D_H
#define EMARKWD3D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Egraphic.h"
#define MARKER3D_TYPEDEFS
#include "Emark3d.h"

/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

#ifdef MARKERWD3D_PRIVATE_HEADER
#   define MARKERWD3D_TYPEDEFS
#endif


#ifdef MARKERWD3D_TYPEDEFS

/* Data structures; never to be accessed from application programs */

typedef struct EMarkerWD3DWCRec {
  WCRec                        center;
  union {
    AttributesRec        any;
    struct {
      BOOLEAN         have_any;
      Dimension       size;
      EMarkerType     type;
      EPixel          color;
      FPNum           val;
    }                    spec;
  }                            attributes;
}                                            EMarkerWD3DWCRec;

#endif /* MARKER3D_TYPEDEFS */

/* methods for a Marker3D; they are NEVER CALLED DIRECTLY -- always */
/* through the methods table (invoked by the generic methods listed */
/* in Egraphic.h). */

#ifdef MARKERWD3D_PRIVATE_HEADER

static BOOLEAN
GetAssocData(GraphicObj *g_p, FPNum *val, int *nvals);
static GraphicObj *
ModifyAssocData(GraphicObj *g_p, FPNum *val);

static void         
Draw(EView *v_p, GraphicObj *p);
static EGraphicType 
Type(GraphicObj *p);
static char         *
AsString(GraphicObj *p);
static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp);
static void         
StoreOn(FILE *file_p, GraphicObj *g_p);

static EGMethodsRec *marker_wd_3d_graphic_methods = NULL;

#undef MARKERWD3D_PRIVATE_HEADER

#endif /* Marker3D_PRIVATE_HEADER */

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateMarkerWD3D(WCRec *p, double val);

#ifdef __cplusplus
}
#endif

#endif
