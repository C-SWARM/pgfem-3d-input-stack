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

#ifndef ERESHAPE_H
#define ERESHAPE_H

#ifdef __cplusplus
extern "C" {
#endif

void ReshapeHandler(Widget w, EView *v_p, XEvent *event);

#define RESHAPE_PROMPT_CLASS 3
#define RESHAPE_STATUS_CLASS 3003

#define MESSAGE_RSHP1 "B1=new handle position|B3=done" /* 1 */
#define MESSAGE_RSHP2 "B1=select handle|B3=done"       /* 2 */
#define MESSAGE_RSHP3 "Select graphics to reshape"     /* 3 */
#define MESSAGE_RSHP4 "Reshape done"                   /* 4 */

#ifdef __cplusplus
}
#endif

#endif
