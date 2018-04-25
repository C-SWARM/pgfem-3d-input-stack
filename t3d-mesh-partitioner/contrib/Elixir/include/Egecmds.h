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

#ifndef EGECMDS_H
#define EGECMDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Egraphic.h"
#include "Ecmds.h"

typedef struct ReshapeData {
  GraphicObj        *g_p;
  EHandleNum        handle;
  WCRec             old_coords;
  WCRec             new_coords;
} ReshapeData;

typedef enum { MOVE_COPY = 0, MOVE_ORIGINAL = 1 } MoveMode;

typedef struct TranslateData {
  LIST                 g_list;
  MoveMode             mode;
  WCRec                delta;
  LIST                 new_g_list;
} TranslateData;

typedef struct ScaleData {
  LIST                 g_list;
  MoveMode             mode;
  WCRec                center;
  double               sx, sy, sz;
  LIST                 new_g_list;
} ScaleData;

typedef struct RotateData {
  LIST                 g_list;
  MoveMode             mode;
  WCRec                center;
  WCRec                axial;
  LIST                 new_g_list;
} RotateData;

typedef struct MirrorData {
  LIST                 g_list;
  MoveMode             mode;
  WCRec                center;
  WCRec                normal;
  LIST                 new_g_list;
} MirrorData;

ECommand *
AGECreateDeleteCommand(LIST to_delete);
ECommand *
AGECreateFastDeleteCommand(LIST to_delete);
ECommand *
AGECreateCreateCommand(GraphicObj *g_p);
ECommand *
AGECreateReshapeCommand(GraphicObj *g_p, EHandleNum handle, WCRec *newp);
ECommand *
AGECreateTranslateCommand(LIST g_list, MoveMode mode, WCRec *by);
ECommand *
AGECreateScaleCommand(LIST g_list, MoveMode mode, WCRec *center);
ECommand *
AGECreateRotateCommand(LIST g_lst, MoveMode mode, WCRec *center, WCRec *axial);
ECommand *
AGECreateMirrorCenterCommand(LIST g_list, MoveMode mode, WCRec *center);
ECommand *
AGECreateMirrorPlaneCommand(LIST g_list, MoveMode mode,
                            WCRec points_on_plane[3]);

BOOLEAN 
AGEExecDeleteCmd(ECommand *c_p);
BOOLEAN 
AGEUndoDeleteCmd(ECommand *c_p);
 
BOOLEAN 
AGEExecFastDeleteCmd(ECommand *c_p);
BOOLEAN 
AGEUndoFastDeleteCmd(ECommand *c_p);

BOOLEAN 
AGEExecCreateCmd(ECommand *c_p);
BOOLEAN 
AGEUndoCreateCmd(ECommand *c_p);
 
BOOLEAN 
AGEExecReshapeCmd(ECommand *c_p);
BOOLEAN 
AGEUndoReshapeCmd(ECommand *c_p);

BOOLEAN 
AGEExecTranslateCmd(ECommand *c_p);
BOOLEAN 
AGEUndoTranslateCmd(ECommand *c_p);

BOOLEAN 
AGEExecScaleCmd(ECommand *c_p);
BOOLEAN 
AGEUndoScaleCmd(ECommand *c_p);

BOOLEAN 
AGEExecRotateCmd(ECommand *c_p);
BOOLEAN 
AGEUndoRotateCmd(ECommand *c_p);

BOOLEAN 
AGEExecMirrorCenterCmd(ECommand *c_p);
BOOLEAN 
AGEUndoMirrorCenterCmd(ECommand *c_p);

BOOLEAN 
AGEExecMirrorPlaneCmd(ECommand *c_p);
BOOLEAN 
AGEUndoMirrorPlaneCmd(ECommand *c_p);

/* Graphic creation routines */

void 
AGECreateLine3D(WCRec *points);
void 
AGECreateMarker3D(WCRec *points);
void 
AGECreateAnnText3D(WCRec *points);
void 
AGECreateTriangle3D(WCRec *points);
void 
AGECreateCBezC3D(WCRec *points);
void 
AGECreateBCBezS3D(WCRec *points);
void 
AGECreateQuad3D(WCRec *points);
void 
AGECreateTetra(WCRec *points);
void 
AGECreateHexahedron(WCRec *points);
void 
AGECreateBCBHex(WCRec *points);
void 
AGECreateRAPrism(WCRec *points);
void 
AGECreatePyramid(WCRec *points);


#ifdef __cplusplus
}
#endif
#endif

