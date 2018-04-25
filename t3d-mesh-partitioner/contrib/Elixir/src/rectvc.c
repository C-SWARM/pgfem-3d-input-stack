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

/* This file holds the redefinition of graphic methods for */
/* the rectangle in view coordinates (VC). */

#include "Econfig.h"
#include "Eview.h"
#include "Egraphic.h"

#include "Erectvc.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Eactset.h"

  
/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

void draw_rectvc(EView *v_p, ERectVCRec *p, GC agc);

/* ========================================================================= */

ERectVCRec *CreateRectVC(VC2DRec *fp, VC2DRec *sp)
{
  ERectVCRec *r_p;

  r_p = (ERectVCRec *)make_node(sizeof(ERectVCRec));
  r_p->first_corner.u    = fp->u;
  r_p->first_corner.v    = fp->v;
  r_p->second_corner.u   = sp->u;
  r_p->second_corner.v   = sp->v;
  r_p->attributes.spec.style    = LineSolid;
  r_p->attributes.spec.width    = 0;
  r_p->attributes.spec.color    = 0x0;
  r_p->attributes.spec.have_any = NO;
  return r_p;
}


/* ========================================================================= */

void DestroyRectVC(ERectVCRec *p)
{
  free_node(p);
}


/* ========================================================================= */

void DrawRectVC(EView *v_p, ERectVCRec *p) 
{
  GC agc;

  if (p->attributes.spec.have_any) {
    agc = EVWritableGC(v_p);
    EGModifyCurveDrawGC(XtDisplay(EVViewPToWidget(v_p)),
			agc,
			p->attributes.spec.style,
			p->attributes.spec.width,
			p->attributes.spec.color,
			CapButt, JoinMiter);
  } else {
    agc = EVDefaultCopyGC(v_p);
  }
  draw_rectvc(v_p, p, agc);
}

/* ========================================================================= */

void draw_rectvc(EView *v_p, ERectVCRec *p, GC agc)
{
  int x1, x2, y1, y2;
  VCRec ll, ur, f = {0, 0, 0}, l = {0, 0, 0};
  Widget w;

  EVGetViewBox(v_p, &ll, &ur);
  f.u = min(p->first_corner.u,  p->second_corner.u);
  f.v = min(p->first_corner.v,  p->second_corner.v);
  l.u = max(p->first_corner.u,  p->second_corner.u);
  l.v = max(p->first_corner.v,  p->second_corner.v);

  if (f.u <= ur.u && f.v <= ur.v && l.u >= ll.u && l.v >= ll.v) {
    FPNum margin;
    
    margin = (ur.u-ll.u) + (ur.v-ll.v);
    margin /= 200.;
    f.u = max(f.u, ll.u-margin);
    f.v = max(f.v, ll.v-margin);
    l.u = min(l.u, ur.u+margin);
    l.v = min(l.v, ur.v+margin);
    EVVCtoDC(v_p, &f, &x1, &y1);
    EVVCtoDC(v_p, &l, &x2, &y2);
    w = EVViewPToWidget(v_p);
    XDrawRectangle(XtDisplay(w), XtWindow(w), agc,
		   min(x1,x2), min(y1,y2),
		   (unsigned int)abs(x2-x1), (unsigned int)abs(y2-y1));
  } 
}


/* ========================================================================= */

void RectVCBoundingBox(EView *v_p, ERectVCRec *p, VC2DRec *ll, VC2DRec *ur)
{
  ll->u = min(p->first_corner.u, p->second_corner.u);
  ll->v = min(p->first_corner.v, p->second_corner.v);
  ur->u = max(p->first_corner.u, p->second_corner.u);
  ur->v = max(p->first_corner.v, p->second_corner.v);
}

/* ========================================================================= */

void XorDrawRectVC(EView *v_p, ERectVCRec *g_p)
{
  draw_rectvc(v_p, g_p, EVDefaultXORGC(v_p));
}


/* ========================================================================= */

ERectVCRec *ModifyRectVC(ERectVCRec *g_p, VC2DRec *fp, VC2DRec *sp)
{
  g_p->first_corner.u    = fp->u;
  g_p->first_corner.v    = fp->v;
  g_p->second_corner.u   = sp->u;
  g_p->second_corner.v   = sp->v;

  return g_p;
}

/* ========================================================================= */

void HiliteRectVC(EView *v_p, ERectVCRec *g_p)
{
  draw_rectvc(v_p, g_p, EVHiliteGC(v_p));
}

/* ========================================================================= */

void UnhiliteRectVC(EView *v_p, ERectVCRec *g_p)
{
  draw_rectvc(v_p, g_p, EVErasehiliteGC(v_p));
}

/* ========================================================================= */

void EraseRectVC(EView *v_p, ERectVCRec *p)
{
  GC agc;

  if (p->attributes.spec.have_any) {
    agc = EVWritableGC(v_p);
    EGModifyCurveDrawGC(XtDisplay(EVViewPToWidget(v_p)),
			agc,
			p->attributes.spec.style,
			p->attributes.spec.width,
			EVGetBackground(v_p),
			CapButt, JoinMiter);
  } else {
    agc = EVEraseGC(v_p);
  }
  draw_rectvc(v_p, p, agc);
}

