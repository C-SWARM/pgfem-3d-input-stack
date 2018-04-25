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

#ifndef ERECTVC_H
#define ERECTVC_H

#ifdef __cplusplus
extern "C" {
#endif

 /* NOTE: THIS IS NOT AN ORDINARY GRAPHIC OBJECT */

typedef struct ERectVCRec {
  VC2DRec                           first_corner;
  VC2DRec                           second_corner;
  union {
    AttributesRec            any;
    struct {
      BOOLEAN            have_any;
      unsigned short     style;
      unsigned short     width;
      EPixel             color;
    }                        spec;
  }                                  attributes;
}                                            ERectVCRec;

ERectVCRec *CreateRectVC(VC2DRec *fp, VC2DRec *sp);
void DestroyRectVC(ERectVCRec *p);
void DrawRectVC(EView *v_p, ERectVCRec *p);
void RectVCBoundingBox(EView *v_p, ERectVCRec *p, VC2DRec *ll, VC2DRec *ur);
void XorDrawRectVC(EView *v_p, ERectVCRec *g_p);
ERectVCRec *ModifyRectVC(ERectVCRec *g_p, VC2DRec *fp, VC2DRec *sp);
void HiliteRectVC(EView *v_p, ERectVCRec *g_p);
void UnhiliteRectVC(EView *v_p, ERectVCRec *g_p);
void EraseRectVC(EView *v_p, ERectVCRec *p);
void RectVCSetAttributes(GraphicObj *p, unsigned long input_mask,
			 int style, int width, EPixel color);
void RectVCGetAttributes(GraphicObj *p, BOOLEAN *have_any,
			 int *style, int *width, EPixel *color);


#ifdef __cplusplus
}
#endif

#endif
