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


#ifndef EFONTS_H
#define EFONTS_H

#ifdef __cplusplus
extern "C" {
#endif


#include "Etypes.h"
#include "Egraphic.h"

/* ======================================================================== */
/* PUBLIC FUNCTIONS  */
/* ======================================================================== */

Font FontGetFontFromString(char *font_name, BOOLEAN *success);
char *FontGetStringFromFont(Font fid);
Font FontDefaultFont(void);
XFontStruct *FontGetFontInfo(Font fid);

/* ======================================================================== */

#ifdef __cplusplus
}
#endif

#endif
