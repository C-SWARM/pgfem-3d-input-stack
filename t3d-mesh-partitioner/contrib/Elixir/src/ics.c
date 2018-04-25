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

#include "Econfig.h"
#define CAN_USE_MACROS_INSTEAD_OF_FUNCS
#include "Eview.h"
#include "Ezbuffer.h"
#include "Ereprts.h"
#include "Edither.h"
#include "Elixir.h"
#include "Eutils.h"
#include "Esimple.h"
#include "Ecolors.h"
#include <X11/Xproto.h>


#define SHIFT_END_LABELS

#define FORMAT_LT                0
#define FORMAT_EQ                1
#define FORMAT_GT                2

#define FORMAT_NA                3


#define BIG_NUMBER               1.0e6

int isc_color_scale_num_labels = ELIXIR_COLOR_SCALE_NUM_LABELS;


void
DrawIntegratedColorScale (EView *v_p)
{

  Display *d;
  Window win;
  GC agc;
  static EPixel black;
	static int first = 1;
	int isc_mode = 2;
	int dir_ret, asc, desc;

	/* margin l/t/r/l */
	int margin = 2;
	/* origin - top right */
  int ox = 0, oy = 80;
	/* cell size */
	int cdx = 0, cdy = 0;
	/* vertical space between cells */
	int dy = 2;
	int xpos, ypos;

  FPNum v, delta, minv, maxv, value;
  EFringeTable ft;
  int i;
  EPixel color;
  char lval[64], *form = NULL;
  XCharStruct xst;
	XFontStruct *xfs;
	BOOLEAN SmoothScale = NO, shift = NO, mark_below = NO, mark_above = NO;

	static char format[4][8] = {"<%10.3e", " %10.3e", ">%10.3e", "%10.3e"};

	if (v_p->render_mode == WIRE_RENDERING)return;
	if (v_p->render_mode != NORMAL_RENDERING){
		SmoothScale = IsFringeTableSmoothScale();
	}
	
	mark_below = IsSameColorBelow();
	mark_above = IsSameColorAbove();

#ifdef SHIFT_END_LABELS
/* centers of ranges are used as labels for smooth color scale;
	 otherwise first and last fringe is used instead of centers of the first and last range */

	if(SmoothScale == NO)shift = YES;
#endif

  d   = XtDisplay(v_p->view_widget);
  win = v_p->draw_into;
  agc = v_p->writableGC;

  if (first) {
		BOOLEAN suc;
    black = ColorGetPixelFromString("black", &suc);
    if (!suc)
      black = 0;
    first = 0;
  }

	/* set font */
	XSetFont(d, agc, FontDefaultFont());
	xfs = XQueryFont (d, FontDefaultFont());

	/* determine the size of color cells */
  ft = EMGetAssocFringeTable(ESIModel());
  ColorFringesMinMax(ft, &minv, &maxv);
  delta = (maxv - minv) / isc_color_scale_num_labels;
  v = minv + delta / 2.0;
  for (i = 0; i < isc_color_scale_num_labels; i++) {
		value = v;
		if (i == 0) {
			if(mark_below == YES){
				if(shift == YES)value += delta / 2.0;
				form = format[FORMAT_LT];
			}
			else{
				form = format[FORMAT_NA];
			}
		} else if (i == isc_color_scale_num_labels - 1) {
			if(mark_above == YES){
				if(shift == YES)value -= delta / 2.0;
				form = format[FORMAT_GT];
			}
			else{
				form = format[FORMAT_NA];
			}
		} else {
			if(mark_below == YES || mark_above == YES)
				form = format[FORMAT_EQ];
			else
				form = format[FORMAT_NA];
		}

		if(fabs(value) < delta / BIG_NUMBER)value = 0.0;
		sprintf(lval, form, value);

		XTextExtents (xfs,lval,strlen(lval),&dir_ret, &asc, &desc, &xst);
		cdx = max (cdx, (xst.rbearing - xst.lbearing));
		cdy = max (cdy,(xst.ascent + xst.descent));
    v += delta;
	}

	cdx += 2*margin; cdy += 2*margin;

	if (isc_mode == 2) {
		ox =  v_p->view_dims_DC.x - cdx - margin;
		oy =  margin;
	} else {
		ox = margin;
		oy = 80;
	}

	XSetForeground(d, agc, v_p->background);
	XFillRectangle(d, win, agc, ox-margin, oy-margin, 
								 (cdx+2*margin), (isc_color_scale_num_labels)*(cdy+dy)-dy+2*margin);
	
	xpos = ox;
	ypos = oy;
  v = minv + delta / 2.0;
  for (i = 0; i < isc_color_scale_num_labels; i++) {
		if(SmoothScale == YES)
			color = ColorFringeValueToColorHSV(ft, v);
		else
			color = ColorFringeRangeToColor(ColorFringeValueToRange(ft, v));

		XSetForeground(d, agc, color);
		XFillRectangle(d, win, agc, xpos, ypos, (unsigned)(cdx), (unsigned)(cdy));
		XSetForeground(d, agc, black);
		
		value = v;
		if (i == 0) {
			if(mark_below == YES){
				if(shift == YES)value += delta / 2.0;
				form = format[FORMAT_LT];
			}
			else{
				form = format[FORMAT_NA];
			}
		} else if (i == isc_color_scale_num_labels - 1) {
			if(mark_above == YES){
				if(shift == YES)value -= delta / 2.0;
				form = format[FORMAT_GT];
			}
			else{
				form = format[FORMAT_NA];
			}
		} else {
			if(mark_below == YES || mark_above == YES)
				form = format[FORMAT_EQ];
			else
				form = format[FORMAT_NA];
		}

		if(fabs(value) < delta / BIG_NUMBER)value = 0.0;
		sprintf(lval, form, value);

		XDrawString(d, win, agc, xpos+2, ypos+cdy-1, lval, strlen(lval));

		ypos += cdy+dy;
		/*
    sprintf(lname, "color_scale_%d", i);
    sprintf(lval, "form", value);
		
    ac = 0;
    XtSetArg(al[ac], XtNbackground,                           color); ac++;
    XtSetArg(al[ac], XtNvisual,           ElixirGetVisual()->visual); ac++;
    XtSetArg(al[ac], XtNdepth,             ElixirGetVisual()->depth); ac++;
    XtSetArg(al[ac], XtNcolormap, ColorStandardColormap()->colormap); ac++;
    label = ESIAddButton(lname, lval,
                         labelWidgetClass, color_scale_palette,
                         al, ac, NULL, NULL);
		*/
    v += delta;
  }
  /*
  XSetForeground(d, agc, green);
  XFillRectangle(d, win, agc, 0, 40, (unsigned)(2*atx), (unsigned)(2*aty));
  XSetForeground(d, agc, black);

	XDrawString(d, win, agc, XS, YS, "Hello", 1);
	*/
}

