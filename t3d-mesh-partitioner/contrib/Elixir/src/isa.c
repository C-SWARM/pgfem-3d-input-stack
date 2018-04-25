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
//#include "Ereprts.h"
//#include "Edither.h"
#include "Elixir.h"
#include "Eutils.h"
#include "Esimple.h"
#include "Ecolors.h"
#include "Eisa.h"
#include <X11/Xproto.h>


#define ISA_LINES      5
#define ISA_LENGTH   128

static char isa_message[ISA_LINES][ISA_LENGTH + 1] = {"", "", "", "", ""};


static char delimiter = '|';

static BOOLEAN use_bg_color = YES;

static EPixel fg_color;

static int first = 1;


void
DrawIntegratedStatusArea (EView *v_p)
{

  Display *d;
  Window win;
  GC agc;
	int isa_mode = 1;
	int dir_ret, asc, desc;

	/* margin l/t/r/l */
	int margin = 2;
	/* origin - top right */
  int ox = 0, oy = 80;
	/* area size */
	int cdx = 0, cdy = 0;
  int i, isa_lines;
	int first_isa_line, last_isa_line;
  XCharStruct xst;
	XFontStruct *xfs;
	char *ptr;
	int dx;
	char buff[ISA_LENGTH];

  d   = XtDisplay(v_p->view_widget);
  win = v_p->draw_into;
  agc = v_p->writableGC;

  if (first) {
		EPixel black;
		BOOLEAN suc;
    black = ColorGetPixelFromString("black", &suc);
    if (!suc)
      black = 0;
    first = 0;
		fg_color = black;
  }

	/* set font */
	XSetFont(d, agc, FontDefaultFont());
	xfs = XQueryFont (d, FontDefaultFont());

	first_isa_line = ISA_LINES;
	last_isa_line = -1;
	for (i=0; i<ISA_LINES; i++) {
		if (strlen(isa_message[i]) == 0) continue;
		
		if(i < first_isa_line)first_isa_line = i;
		if(i > last_isa_line)last_isa_line = i;
		
		/* determine the size of status cell */
		XTextExtents (xfs, isa_message[i], strlen(isa_message[i]), &dir_ret, &asc, &desc, &xst);
		
		/* search for delimiter */
		if ((ptr = strchr (isa_message[i], (int)delimiter)) == NULL) {
			cdx = max(cdx, (xst.rbearing - xst.lbearing));
			cdy = max(cdy, (xst.ascent + xst.descent));
		} else {
			if(ptr == strrchr (isa_message[i], (int)delimiter)){
				cdx = max (cdx, v_p->view_dims_DC.x-2*margin);
				cdy = max (cdy, (xst.ascent + xst.descent));
			}
			else{
				/* ambiguous delimiter ==> align left */
				cdx = max(cdx, (xst.rbearing - xst.lbearing));
				cdy = max(cdy, (xst.ascent + xst.descent));
			}
		}
	}

	if(last_isa_line < first_isa_line)return;
	isa_lines = last_isa_line - first_isa_line + 1;
	
		if (isa_mode == 1) {
		ox = margin;
		oy = v_p->view_dims_DC.y - (isa_lines-1)*(cdy+margin)-margin;
	} else {
		ox = margin;
		oy = margin;
	}

	if(use_bg_color == YES){
		XSetForeground(d, agc, v_p->background);
		XFillRectangle(d, win, agc, ox-margin, oy-cdy-margin, (cdx)+2*margin, isa_lines*(cdy+margin)+margin);
	}
	
	XSetForeground(d, agc, fg_color);
	for (i=first_isa_line; i<last_isa_line+1; i++) {
		if (strlen(isa_message[i]) != 0)
			strcpy (buff, isa_message[i])	;
		else
			sprintf(buff, " ");
		ptr = strchr(buff, (int)delimiter);
		if (ptr == NULL) {
			XDrawString(d, win, agc, ox, oy+i*(cdy+margin), buff, strlen(buff));
		} else {
			if(ptr == strrchr(buff, (int)delimiter)){
				*ptr = '\0';
				XDrawString(d, win, agc, ox, oy+i*(cdy+margin), buff, strlen(buff));
				XTextExtents (xfs,ptr+1, strlen(ptr+1), &dir_ret, &asc, &desc, &xst);
				dx = xst.rbearing-xst.lbearing;
				XDrawString(d, win, agc, ox+cdx-dx, oy+i*(cdy+margin), ptr+1, strlen(ptr+1));
			}
			else{
				XDrawString(d, win, agc, ox, oy+i*(cdy+margin), buff, strlen(buff));
			}
		}
	}
}




int
GetISASize(void)
{
	return(ISA_LINES);
}


int
SetISAContent(int line, char *message)
{
	if(line <= 0 || line > ISA_LINES)return(0);

	strncpy(isa_message[line - 1], message, ISA_LENGTH);
	isa_message[line - 1][ISA_LENGTH] = '\0';

	return(line);
}


void
SetISABgColorUse(BOOLEAN use_bg)
{
	use_bg_color = use_bg;
}


BOOLEAN
SetISAFgColor(char *color_name)
{
	BOOLEAN suc;
	EPixel color;

	color = ColorGetPixelFromString(color_name, &suc);
	if(suc == NO)return(NO);

	first = 0;
	fg_color = color;
	return(YES);
}


char
GetISADelimiter(void)
{
	return(delimiter);
}


void
SetISADelimiter(char delim)
{
	delimiter = delim;
}
