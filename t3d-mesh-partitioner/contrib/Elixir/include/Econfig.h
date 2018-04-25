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

#ifndef ECONFIG_H
#define ECONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

/* This is the "configure"-generated file.  It defines symbols to use */
/* in source code. */
#include <Ecfg.h>

#ifndef UNIX
#   define UNIX
#endif


/* Include to be consistent with Ckit (basically due to some problems */
/* with memory allocation). */
#include <ckitcfg.h>

#define NO_MOTIF
#undef  NO_ATHENA  /* use the athena interface */

#ifndef NO_MOTIF
#   define MOTIF_DRAWING_AREA   /* to use DrawingArea widget */
#endif

#ifndef NO_ATHENA
#   define ATHENA_DRAWING_AREA  /* to use Box widget */
#endif

#ifdef __cplusplus
}
#endif

#endif
