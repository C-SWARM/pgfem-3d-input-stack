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

#ifndef ELIXIR_H
#define ELIXIR_H

#ifdef __cplusplus
extern "C" {
#endif

/* This source has been modified for ELIXIR by Petr Krysl from */
/* the PEXlib Programming Manual by Tom Gaskins (1993 O'Reilly and */
/* Associates). See copyright notice below. */
/*
    Copyright 1992, 1993 O'Reilly and Associates, Inc.  Permission to
    use, copy, and modify this program is hereby granted, as long as
    this copyright notice appears in each copy of the program source
    code.
*/

#include <stdio.h>
#include <math.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

#include "Econfig.h"

#define SET_COLOR( r, g, b, c ) \
   { (c).rgb.red = (r); (c).rgb.green = (g); (c).rgb.blue = (b); }

/* Option to pass to ElixirInit(): */
/* 1) Use the default visual instead of the best visual. */
#define ELIXIR_DEFAULT_VISUAL_OPTION    (1L<<1)
/* 2) Use the best visual. */
#define ELIXIR_BEST_VISUAL_OPTION       (1L<<2)
/* 3) Use the default colormap */
#define ELIXIR_DEFAULT_COLORMAP_OPTION  (1L<<3)

/* What options to pass to ElixirInit()? */
#undef ELIXIR_INIT_OPTIONS
#ifdef USE_DEFAULT_VISUAL
#  ifdef USE_DEFAULT_COLORMAP
#     define ELIXIR_INIT_OPTIONS                        \
                (  ELIXIR_DEFAULT_VISUAL_OPTION         \
                 | ELIXIR_DEFAULT_COLORMAP_OPTION)
#  else
#     define ELIXIR_INIT_OPTIONS                        \
                (  ELIXIR_DEFAULT_VISUAL_OPTION)
#  endif
#else
#  define ELIXIR_INIT_OPTIONS 0
#endif

BOOLEAN
ElixirInit(Display *dpy, Screen *screen, EMask options);
void 
ElixirFindBestVisual(Display *dpy,            /* IN */
                     int screen_num,         /* IN: */
		     XVisualInfo *chosen_vis) /* OUT */;
Display  *
ElixirGetDisplay(void);
Screen  *
ElixirGetScreen(void);
XVisualInfo  *
ElixirGetVisual(void);
BOOLEAN 
ElixirInitialized(void);

#define INITIAL_U 1.0        /* default initial view dimension along u axis */

EView *
ElixirNewView(char *app_name, char *app_class, 
	      char *bg_color, char *fg_color, int dimx, int dimy);

#ifdef __cplusplus
}
#endif

#endif
