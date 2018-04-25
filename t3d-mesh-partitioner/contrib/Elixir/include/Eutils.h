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


#ifndef EUTILS_H
#define EUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eexit.h"

#include "listP.h"
#include "alistP.h"

#ifndef min
#   define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#   define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define X_ 0 /* May be used to index a 3D vector */
#define Y_ 1
#define Z_ 2


#define msec_timer(start_timing) ckit_msec_timer(start_timing)

void
EUFailedMakeNode(const char *in_proc);

#ifdef __cplusplus
}
#endif

#endif
