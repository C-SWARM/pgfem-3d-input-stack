
/* This source has been modified for ELIXIR by Petr Krysl from 
   the PEXlib Programming Manual by Tom Gaskins (1993 O'Reilly and 
   Associates). See copyright notice below. 
   
   Copyright 1992, 1993 O'Reilly and Associates, Inc.  Permission to
   use, copy, and modify this program is hereby granted, as long as
   this copyright notice appears in each copy of the program source
   code.
   */

#include "Econfig.h"
#include "Ecomptblt.h"
#include "Eexit.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

/* NOTE: The plain Xt Intrinsics' core widget leads to singularly poor */
/* performance of XOR-drawing -- it was most pronounced during creation */
/* of a bicubic Bezier patch. Thefore, as an alternative, the code below */
/* creates the drawing area widget using the Motif version. */

#include "Etypes.h"
#include "Eutils.h"
#include "Ecolors.h"
#include "Elixir.h"

#ifdef MOTIF_DRAWING_AREA
#   include <Xm/Xm.h>
#   include <Xm/DrawingA.h>
#else
#   ifdef ATHENA_DRAWING_AREA
#      include  <X11/Xaw/Box.h>
#      include  <X11/Xaw/Label.h>
#   else
#      include <X11/Core.h>
#   endif
#endif

static Display *display;
static Screen *screen;
static XVisualInfo *vis_info = NULL;
static BOOLEAN elixir_initialized = NO;


/* View defaults */

#define VUP_DATA     {0, 0, 1}
#define NORMAL_DATA  {3, 2, 1}
#define VRP_DATA     {0, 0, 0}
#define VORIGIN_DATA {0, 0, 0}
#define VC_XDIM       2.
#define VC_YDIM       2.

static Widget drawing_area_shell;

#define MAXARGS 12

/* ========================================================================= */
/* Initializes the ELIXIR system */

BOOLEAN
ElixirInit(Display *dpy,  /* IN: */
           Screen *scrn,  /* IN: */
           EMask options  /* IN: */
    )
{
    int nvi;
    XVisualInfo vis_info_tplt;
    
    display = dpy;
    screen  = scrn;
    if (options & ELIXIR_DEFAULT_VISUAL_OPTION) {
        vis_info_tplt.visualid =
            XVisualIDFromVisual(DefaultVisual(display,
                                              XScreenNumberOfScreen(screen)));
        vis_info = XGetVisualInfo(display, VisualIDMask, &vis_info_tplt,
                                  &nvi);
    } else {
        vis_info = (XVisualInfo *)make_node(sizeof(XVisualInfo));
        ElixirFindBestVisual(display, XScreenNumberOfScreen(screen), vis_info);
    }
    if (!ColorLoadStandardColormap(display, vis_info)) {
        fprintf(stderr, "Could not load standard colormap\n");
        return NO;
    }
    elixir_initialized = YES;
    return YES;
}

/* ========================================================================= */
/* Finds the "best" visual. The best one is the */
/* one with the most colors and highest capabilities. */

void 
ElixirFindBestVisual(Display *dpy,            /* IN: */
                     int  screen_num,         /* IN: */
		     XVisualInfo *chosen_vis  /* OUT: */
                     ) 
{
  int			i, size, num_vis, cmap_size;
  XVisualInfo		vis_templ, *vis_list;
  
  /* Get all the visuals for the screen. */
  vis_templ.screen = screen_num;
  vis_list = XGetVisualInfo(dpy, VisualScreenMask, &vis_templ, &num_vis);
  
  /* Determine the best visual available.  The best one is the */
  /* one with the most colors and highest capabilities. */
  cmap_size = 0;
  chosen_vis->class = -1;
  for (i = 0; i < num_vis; i++) {
    /* Determine the number of colors available. */
    switch (vis_list[i].class) {
    case TrueColor:
    case DirectColor:
      size = (vis_list[i].red_mask |
	      vis_list[i].green_mask |
	      vis_list[i].blue_mask) + 1;
      break;
    default:
      size = vis_list[i].colormap_size;
    }
    
    /* Choose this one if it supports more colors or is a higher */
    /* class, but favor TrueColor over DirectColor. */
    if (size >= cmap_size) {
      switch (vis_list[i].class) {
      case TrueColor:
	/* Choose TrueColor over anything else. */
	*chosen_vis = vis_list[i];
	cmap_size = size;
	break;
      case DirectColor:
	/* Don't choose DirectColor over TrueColor. */
	if (chosen_vis->class != TrueColor) {
	  *chosen_vis = vis_list[i];
	  cmap_size = size;
	}
	break;
      default:
	/* Choose the highest class. */
	if (vis_list[i].class >= chosen_vis->class) {
	  *chosen_vis = vis_list[i];
	  cmap_size = size;
	}
	break;
      }
    }
  }
}

/* ========================================================================= */
/* Returns the display pointer passed in during initialization. */

Display  *
ElixirGetDisplay(void)
{
  if (elixir_initialized)
    return display;
  else {
    fprintf(stderr, "ELIXIR not initialized\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
    return NULL;
  }
}


/* ========================================================================= */
/* Returns the screen pointer passed in during initialization. */

Screen  *
ElixirGetScreen(void)
{
  if (elixir_initialized)
    return screen;
  else {
    fprintf(stderr, "ELIXIR not initialized\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
    return NULL;
  }
}

/* ========================================================================= */
/* Returns the pointer at the visual info obtained during initialization. */

XVisualInfo  *
ElixirGetVisual(void)
{
  if (elixir_initialized)
    return vis_info;
  else {
    fprintf(stderr, "ELIXIR not initialized\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
    return NULL;
  }
}

/* ========================================================================= */
/* Was ELIXIR initialized? */

BOOLEAN 
ElixirInitialized(void)
{
  return elixir_initialized;
}

/* ======================================================================= */
/* Create new ELIXIR view. */

EView *
ElixirNewView(char *app_name, /* IN: */
              char *app_class, /* IN: */
	      char *bg_color, /* IN: */
              char *fg_color, /* IN: */
              int dimx, int dimy /* IN: */
              )
{
  Widget    drawing_area;
  Arg        al[MAXARGS];
  Cardinal            ac;
  WCRec vup = VUP_DATA;
  WCRec normal = NORMAL_DATA;
  WCRec vrp = VRP_DATA;
  WCRec vorigin = VORIGIN_DATA;
  EView        *v_p;
  EPixel pxl, fg_pixel, bg_pixel;
  BOOLEAN success;
  XSetWindowAttributes xswattrs;

  if (!ElixirInitialized()) {
    fprintf(stderr, "Elixir not initialized --> cannot open new frame\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  } else {
    fg_pixel = WhitePixel(ElixirGetDisplay(),
                          XScreenNumberOfScreen(ElixirGetScreen()));
    bg_pixel = BlackPixel(ElixirGetDisplay(),
                          XScreenNumberOfScreen(ElixirGetScreen()));
    if (fg_color != NULL) {
      pxl = ColorGetPixelFromString(fg_color, &success);
      if (success) fg_pixel = pxl;
    }
    if (bg_color != NULL) {
      pxl = ColorGetPixelFromString(bg_color, &success);
      if (success) bg_pixel = pxl;
    }

    ac = 0;
    XtSetArg(al[ac], XtNallowShellResize,                      True); ac++;
    XtSetArg(al[ac], XtNvisual,           ElixirGetVisual()->visual); ac++;
    XtSetArg(al[ac], XtNdepth,             ElixirGetVisual()->depth); ac++;
    XtSetArg(al[ac], XtNcolormap, ColorStandardColormap()->colormap); ac++;
    XtSetArg(al[ac], XtNforeground, fg_pixel);  ac++;
    XtSetArg(al[ac], XtNbackground, bg_pixel);  ac++;
    drawing_area_shell = 
      XtAppCreateShell(app_name, app_class,
		       topLevelShellWidgetClass,
		       ElixirGetDisplay(), al, ac);

    if (dimx <= 0) dimx = 500; 
    if (dimy <= 0) dimy = 400; 
    ac = 0;
    XtSetArg(al[ac], XtNheight,         dimy);  ac++;
    XtSetArg(al[ac], XtNwidth,          dimx);  ac++;
 /* Note: core does not support foreground
    XtSetArg(al[ac], XtNforeground, fg_pixel);  ac++;
    */
    XtSetArg(al[ac], XtNbackground, bg_pixel);  ac++;
#ifdef MOTIF_DRAWING_AREA
    drawing_area = XmCreateDrawingArea(drawing_area_shell,
				       "drawing_area", al, ac);
#else
#  ifdef ATHENA_DRAWING_AREA
    drawing_area = XtCreateWidget("drawing_area", boxWidgetClass,
				  drawing_area_shell, al, ac);
#  else
    drawing_area = XtCreateWidget("drawing_area", coreWidgetClass,
				  drawing_area_shell, al, ac);
#  endif
#endif
    
    XtManageChild(drawing_area);
    XtRealizeWidget(drawing_area_shell);

    xswattrs.backing_store = NotUseful;	/* make sure we're not using it */
    XChangeWindowAttributes(ElixirGetDisplay(), XtWindow(drawing_area),
			    CWBackingStore, &xswattrs);
    
    v_p = EVInitView(drawing_area,
		     drawing_area_shell,
		     bg_pixel, fg_pixel,
		     &vrp, &normal, &vup, &vorigin,
                     INITIAL_U, INITIAL_U*((double)dimy/dimx));
  }
  return v_p;
}
