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

#ifndef EMOVE_H
#define EMOVE_H

#ifdef __cplusplus
extern "C" {
#endif

void 
TranslateHandler(Widget w, EView *v_p, XEvent *event);
void 
ScaleHandler(Widget w, EView *v_p, XEvent *event);
void 
RotateByActiveAngleHandler(Widget w, EView *v_p, XEvent *event);
void 
RotateInteractiveHandler(Widget w, EView *v_p, XEvent *event);
void 
MirrorCenterHandler(Widget w, EView *v_p, XEvent *event);
void 
MirrorPlaneHandler(Widget w, EView *v_p, XEvent *event);


#define MOVE_PROMPT_CLASS 1
#define MOVE_STATUS_CLASS 1001

#define MESSAGE_TR1  "B1=new position|B3=done" /* 1 */
#define MESSAGE_TR2  "B1=start position|B3=done" /* 2 */
#define MESSAGE_TR3  "Select graphics to translate" /* 3 */
#define MESSAGE_TR4  "Translate done" /* 4 */

#define MESSAGE_SC1  "B1=center|B3=done" /* 1 */
#define MESSAGE_SC2  "Select graphics to scale" /* 2 */
#define MESSAGE_SC3  "Scale done" /* 3 */

#define MESSAGE_RO1  "B1=vector end (second point of axial vector)|B3=done" /* 5 */
#define MESSAGE_RO2  "B1=vector start (rotation center)|B3=done" /* 6 */
#define MESSAGE_RO3  "Select graphics to rotate" /* 7 */
#define MESSAGE_RO4  "Rotate done" /* 8 */

#define MESSAGE_RO5  "B1=first point of axial vector|B3=done" /* 9 */
#define MESSAGE_RO6  "B1=second point of axial vector|B3=new first point" /* 10 */
#define MESSAGE_RO7  "B1=start point|B3=new second axis point" /* 11 */
#define MESSAGE_RO8  "B1=end point|B3=new start point" /* 12 */

#define MESSAGE_MC1  "B1=center|B3=done" /* 13 */
#define MESSAGE_MC3  "Select graphics to mirror about center" /* 14 */
#define MESSAGE_MC4  "Mirror about center done" /* 15 */

#define MESSAGE_MP0  "B1=first point on mirror plane|B3=done" /* 16 */
#define MESSAGE_MP1  "B1=second point on mirror plane|B3=new first point" /* 17 */
#define MESSAGE_MP2  "B1=last point on mirror plane|B3=new second point" /* 18 */
#define MESSAGE_MP3  "Select graphics to mirror in plane" /* 19 */
#define MESSAGE_MP4  "Mirror in plane done" /* 20 */

#ifdef __cplusplus
}
#endif

#endif
