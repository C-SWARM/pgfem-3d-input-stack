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

#ifndef ETRANSFM_H
#define ETRANSFM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Etypes.h"
#include "Eview.h"
#include "Egraphic.h"

void AdjustVectorToShrink(FPNum *v, int nnodes, FPNum shrink);

BOOLEAN ComputeRotMatVects(WCRec *vectorUp, /* IN: vector up */
			   WCRec *normal,   /* INOUT: normal */
			   WCRec *vectorU,  /* OUT: vector U */
			   WCRec *vectorV   /* OUT: vector V */
			   );
void TransfPntWCToVC(WCRec *vectorU,
		     WCRec *vectorV,
		     WCRec *normal,
		     WCRec *shift,
		     WCRec *w,
		     VCRec *v);
void TransfPntVCToWC(WCRec *vectorU,
		     WCRec *vectorV,
		     WCRec *normal,
		     WCRec *shift,
		     VCRec *v, 
		     WCRec *w);
void CrossProd3(WCRec *reslt_vec, WCRec *Vec1, WCRec *Vec2);
FPNum DotProd3(WCRec *Vec1, WCRec *Vec2);
BOOLEAN NormalizeVect3(WCRec *vec);
void RotVectAboutVect(WCRec *aboutv, /* IN: vector to rotate about */
		      WCRec *avec    /* INOUT: original and rotated vector */
		      );
BOOLEAN VectorsColinear(FPNum epsilon, WCRec *v1, WCRec *v2);
void
MirrorPointInPlane(WCRec *point_on_plane, WCRec *unit_normal,
                   WCRec *point_to_mirror);

#ifdef __cplusplus
}
#endif

#endif
