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


#ifndef EXI_H
#define EXI_H

#ifdef __cplusplus
extern "C" {
#endif

int
EXIWriteOOGLStartPROGN(FILE *stream);

int
EXIWriteOOGLClosePROGN(FILE *stream);


int
EXIWriteOOGLStartList(FILE *stream);

int
EXIWriteOOGLEndList(FILE *stream);

int
EXIWriteOOGLGeom(FILE *stream, EModel *model, GraphicObj *gp);

int
EXIWriteOOGLObject(FILE *stream, EModel *model, GraphicObj *gp);




#ifdef __cplusplus
}
#endif

#endif
