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

#ifndef ECREATECB_H
#define ECREATECB_H

#ifdef __cplusplus
extern "C" {
#endif


#define CREATE_PROMPT_CLASS 2
#define CREATE_STATUS_CLASS 2002
#define CREATE_ERROR_CLASS  2

#define MESSAGE_CREATE_1ST_DONE  "B1=first point|B3=done"              /* 0 */
#define MESSAGE_CREATE_NEXT_BKP  "B1=next point|B3=back up one point"  /* 1 */
#define MESSAGE_CREATE_FINAL_BKP "B1=final point|B3=back up one point" /* 2 */
#define MESSAGE_CREATE_DONE      "Create done"                         /* 3 */

#define MESSAGE_CREATE_LOCATION  "B1=location|B3=done"                 /* 0 */

#define CREATE_ERROR_1 "No objects selected."           /* 1 */
#define CREATE_ERROR_2 "Create from curves failed."     /* 2 */
#define CREATE_ERROR_3 "Not enough curves selected."    /* 3 */
#define CREATE_ERROR_4 "Not enough objects selected."   /* 4 */
#define CREATE_ERROR_5 "No curve selected."             /* 5 */
#define CREATE_ERROR_6 "No patch selected."
#define CREATE_ERROR_7 "Extrusion failed."

#define MESSAGE_CREATE_DELTA "Select patches to extrude by delta"
#define MESSAGE_CREATE_CURVE "Select patch and curve to extrude along"
#define MESSAGE_CREATE_CURVS "Select curves (first two must give outer normal)"
typedef void (*ModelCreateNProcP)(WCRec *points);

typedef GraphicObj *(*CreateNEchoProcP)(WCRec *points);

typedef GraphicObj *(*ModifyNEchoProcP)(GraphicObj *, WCRec *points);

void SetByNPointsHandlerData(ModelCreateNProcP m_p_p,
			     CreateNEchoProcP  c_e_p_p,
			     ModifyNEchoProcP  m_e_p_p,
			     int N, int final_for_echo,
			     char *prompt1, char *prompt2,
			     char *prompt3, char *prompt4);
void CreateByNPointsHandler(Widget w, EView *v_p, XEvent *event);

#ifdef __cplusplus
}
#endif

#endif
