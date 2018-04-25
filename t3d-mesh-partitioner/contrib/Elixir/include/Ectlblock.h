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


#ifndef ECTLBLOCK_H
#define ECTLBLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Egraphic.h"
#include "Eactset.h"
#include "Eggroup.h"


/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

typedef enum { END_OF_GROUP_CTLBLOCK, ACTIVE_SET_CTLBLOCK } ECtlblockType;

#ifdef CTLBLOCK_PRIVATE_HEADER
#   define CTLBLOCK_TYPEDEFS
#endif

#ifdef CTLBLOCK_TYPEDEFS

/* Data structures; never to be accessed from application programs */

typedef struct ECtlBlockRec {
  int                     block_type;
  union {
    EActiveSetRec      active_set;
    EEndOfGgRec        end_of_gg;
  }                       block_data;
}                                            ECtlBlockRec;

#endif

/* methods for a control block */ 
/* they are NEVER CALLED DIRECTLY -- always */
/* through the methods table (invoked by the generic methods listed */
/* in Egraphic.h). */

#ifdef CTLBLOCK_PRIVATE_HEADER

static char         *
AsString(GraphicObj *p);
static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp);
static void         
StoreOn(FILE *file_p, GraphicObj *g_p);
static EGraphicType 
Type(GraphicObj *g_p);

struct EGMethodsRec ctlblock_graphic_methods = {
/*    TYPE_FUNC_P           */   Type,
/*    STRING_REP_FUNC_P     */   AsString,
/*    STORE_ON_FUNC_P       */   StoreOn,
/*    RETRIEVE_FROM_FUNC_P  */   RetrieveFrom,
/*    COPY_FUNC_P           */   NULL,
/*    DEEP_COPY_FUNC_P      */   NULL,
/*    DESTROY_FUNC_P        */   NULL,
/*    DEEP_DESTROY_FUNC_P   */   NULL,
/*    INTERPOLATE_FUNC_P    */   NULL,
/*    BBOX_FUNC_P           */   NULL,
/*    BBOX_FUNC_P           */   NULL,
/*    DRAW_FUNC_P           */   NULL,
/*    XORDRAW_FUNC_P        */   NULL,
/*    HILITE_FUNC_P         */   NULL,
/*    UNHILITE_FUNC_P       */   NULL,
/*    ERASE_FUNC_P          */   NULL,
/*    HIT_FUNC_P            */   NULL,
/*    KEY_POINT_LIST_FUNC_P */   NULL,
/*    RESHAPE_HANDLE_FUNC_P */   NULL,
/*    XY_OF_HANDLE_FUNC_P   */   NULL,
/*    RESHAPE_FUNC_P        */   NULL,
/*    MODIFY_BY_2_FUNC_P    */   NULL,
                                 NULL,
                                 NULL,
                                 NULL,
/*    TRANSLATE_FUNC_P      */   NULL,
/*    ROTATE_FUNC_P         */   NULL,
/*    SCALE_FUNC_P          */   NULL,
/*    SHOW_HANDLES_FUNC_P   */   NULL,
/*    ERASE_HANDLES_FUNC_P  */   NULL,
				 NULL,
				 NULL,
				 NULL
};

#undef CTLBLOCK_PRIVATE_HEADER

#endif /* CTLBLOCK_PRIVATE_HEADER */

/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

GraphicObj *
CreateCtlBlock(ECtlblockType block_type);
ECtlblockType 
CtlBlockType(GraphicObj *g_p);


#ifdef __cplusplus
}
#endif

#endif
