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
#include <string.h>
#include <ctype.h>
#include "Eview.h"
#include "Egraphic.h"
#include "Efonts.h"
#include "Elixir.h"
#include "Ereprts.h"
#include "Eutils.h"

/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

typedef struct _FontPairRec {
  char           *font_name;
  XFontStruct    *font_struct;
} FontPairRec;

static LIST fonts = NULL;
static Font previous_font_id = (Font)-1;
static char *previous_font_name = NULL;
static XFontStruct *previous_font_struct = (XFontStruct *)NULL;
static Display *display = NULL;

static char  *DEFAULT_FONT = "fixed";

static Font default_font_id = (Font)0;

static int search_by_name(char *to_be_searched_for, FontPairRec *p);
static int search_by_font(Font to_be_searched_for, FontPairRec *p);
BOOLEAN FontNamesEquivalent(char *s1, char *s2);




static int 
search_by_name(char *to_be_searched_for, FontPairRec *p)
{
  if (FontNamesEquivalent(to_be_searched_for, p->font_name))
    return TRUE;
  else
    return FALSE;
}



static int 
search_by_font(Font to_be_searched_for, FontPairRec *p)
{
  if (to_be_searched_for == p->font_struct->fid)
    return TRUE;
  else
    return FALSE;
}



BOOLEAN 
FontNamesEquivalent(char *s1, char *s2)
{
  int j;

  j = -1;
  do {
    j++;
    if (toupper(*(s1+j)) != toupper(*(s2+j))) {
      return NO;
    }
  }  while (*(s1+j) != '\0' && *(s2+j) != '\0');
  return YES;
}


/* PUBLIC */


Font 
FontGetFontFromString(char *font_name, BOOLEAN *success)
{
  FontPairRec *p;
  Font ret_font;

  if (previous_font_name != NULL) {
    if (FontNamesEquivalent(previous_font_name, font_name)) {
      *success = YES;
      return previous_font_id;
    }
  }
  
  if (fonts == (LIST)NULL)
    fonts = make_list();

  if ((p = (FontPairRec *)search_list(fonts, font_name,
					search_by_name)) != NULL) {
    ret_font = p->font_struct->fid;
    *success  = YES;
  } else {
    ret_font = (Font)0;
    if ((p = (FontPairRec *)make_node(sizeof(FontPairRec))) == NULL) {
      EUFailedMakeNode("FontGetFontFromString");
    }
    if (display == (Display *)NULL)
      display = ElixirGetDisplay();
    if ((p->font_struct = XLoadQueryFont(display, font_name)) != NULL) {
      add_to_tail(fonts, p);
      p->font_name = strdup(font_name);
	ret_font     = p->font_struct->fid;
      *success      = YES;
    } else {
      fprintf(stderr, "Failed load font %s\n", font_name);
      ERptErrMessage(ELIXIR_WARNING_CLASS, 2, ELIXIR_WARNING_2,
                     WARNING_GRADE);
      *success  = NO;
    }
  }
  previous_font_id     = ret_font;
  previous_font_name   = p->font_name;
  previous_font_struct = p->font_struct;
  default_font_id      = ret_font;
  return ret_font;
}



char *
FontGetStringFromFont(Font fid)
{
  FontPairRec *p;

  if (previous_font_id == fid) {
    return previous_font_name;
  }
  
  if (fonts == (LIST)NULL) 
    return DEFAULT_FONT;

  if ((p = (FontPairRec *)search_list(fonts, fid,
                                      search_by_font)) != NULL) {
    previous_font_id     = fid;
    previous_font_name   = p->font_name;
    previous_font_struct = p->font_struct;
    return p->font_name;
  } else {
    return DEFAULT_FONT;
  }
}



XFontStruct *
FontGetFontInfo(Font fid)
{
  FontPairRec *p;

  if (previous_font_id == fid) {
    return previous_font_struct;
  }
  
  if (fonts == (LIST)NULL)
    return (XFontStruct *)NULL;

  if ((p = (FontPairRec *)search_list(fonts, fid,
					search_by_font)) != NULL) {
    previous_font_id     = fid;
    previous_font_name   = p->font_name;
    previous_font_struct = p->font_struct;
    return previous_font_struct;
  } else {
    return (XFontStruct *)NULL;
  }
}



Font 
FontDefaultFont(void)
{
  if (default_font_id == (Font)0) {
    if (display == (Display *)NULL)
      display = ElixirGetDisplay();
    default_font_id = XLoadFont(display, DEFAULT_FONT);
  }
  return default_font_id;
}
