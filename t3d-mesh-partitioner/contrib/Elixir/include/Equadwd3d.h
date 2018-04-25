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


#ifndef EQUADWD3D_H
#define EQUADWD3D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Egraphic.h"
#define QUAD3D_TYPEDEFS         /* to gain access to the private types */
#include "Equad3d.h"

/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

#ifdef QUADWD3D_PRIVATE_HEADER
#   define QUADWD3D_TYPEDEFS
#endif

#ifdef QUADWD3D_TYPEDEFS

/* Data structures; never to be accessed from application programs */

typedef struct AssocQuadDataRec {
    FPNum  first_val;
    FPNum  second_val;
    FPNum  third_val;
    FPNum  fourth_val;
} AssocQuadDataRec;


typedef struct EQuadWD3DWCRec {
  EQuad3DWCRec       quad3d;
  AssocQuadDataRec   assoc_data;
}                                            EQuadWD3DWCRec;
 
#endif /* QUADWD3D_TYPEDEFS */


/* methods for a quadrangle with data;
   they are NEVER CALLED DIRECTLY -- always */
/* through the methods table (invoked by the generic methods listed */
/* in Egraphic.h). */

#ifdef QUADWD3D_PRIVATE_HEADER

static BOOLEAN
GetAssocData(GraphicObj *g_p, FPNum *vals, int *nvals);
static GraphicObj *
ModifyAssocData(GraphicObj *g_p, FPNum *vals);
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

/* Must be set up in the constructor -- the object inherits from */
/* a generic quadrangle */
static EGMethodsRec *quad_wd_3d_graphic_methods = NULL;

#undef QUADWD3D_PRIVATE_HEADER

#endif /* QUADWD3D_PRIVATE_HEADER */

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateQuadWD3D(WCRec *points, double v1, double v2, double v3, double v4);

#ifdef __cplusplus
}
#endif

#endif
