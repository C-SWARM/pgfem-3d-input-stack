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

/* Postscript image generator */

#include "Esimple.h"
#include <time.h>

#define PTS_PER_INCH          72
#define PTS_PER_CM            28.346457
#define PAGE_WIDTH_IN_PTS    602
#define PAGE_HEIGHT_IN_PTS   782


static BOOLEAN eps = NO;
static BOOLEAN portrait_orient = NO;
static char *ps_file_name = NULL;

static int image_width  = 0;
static int image_height = 0;
static int image_bits   = 8;    /* assuming 8-bit image */
static int image_x_trans = 0;
static int image_y_trans = 0;
static int image_top_left_x = 72;
static int image_top_left_y = 72;

static void
postscript_prolog(FILE *fp);
static void
postscript_epilog(FILE *fp);
static void
postscript_image_size(XImage *image, int width_in_pts, int height_in_pts);
static void
postscript_image(FILE *fp, XImage *image);
int
ps_dump(EView *v_p, char *fname,
        double width_in_cm, double height_in_cm,
        BOOLEAN portrait, BOOLEAN encapsulated_ps
        );

int 
ps_dump_error_handler(Display *display, XErrorEvent *e)
{
#define BUFL 132  
  char buf[BUFL];
  XGetErrorText(display, e->error_code, buf, BUFL);
  fprintf(stderr, "ps_dump_error_handler, %s", buf);
  return 0;
}
 

int
ps_dump(EView *v_p, char *fname,
        double width_in_cm, double height_in_cm,
        BOOLEAN portrait, BOOLEAN encapsulated_ps
        )
{
  FILE *fp; 
  XImage *image = NULL;       /* assuming 8 planes! */
  unsigned long plm = (1 | 2 | 4 | 8 | 16 | 32 | 64 | 128);
  double width_in_pts = width_in_cm * PTS_PER_CM;
  double height_in_pts = height_in_cm * PTS_PER_CM;
  XWindowAttributes xwats;
  Status status;
  int x, y;
  unsigned int dcx, dcy, w, h, bdw, d;
  Display *display = XtDisplay(v_p->view_widget);
  Window win = v_p->draw_into;
  Pixmap px;

  /* Set options */
  eps             = encapsulated_ps;
  portrait_orient = portrait;
  ps_file_name    = fname;
  
  if ((fp = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "ps_dump: couldn't open %s\n", fname);
    return NO;
  }

  ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1,
                    "\n   Postscript dump: starting ...");

  /* create pixmap, copy the window into it and extract X image */
  EVGetDimsDC(v_p, &dcx, &dcy);
  status = XGetWindowAttributes(display, win, &xwats);
  d = xwats.depth;
  XSetErrorHandler(ps_dump_error_handler); /*avoid abort when something fails*/
  if (EVUsingBackBuffer()) {
    px = EVBackBuffer();
  } else {
    px = XCreatePixmap(display, win, dcx, dcy, d);
    if (!XGetGeometry(display, px, &win, &x, &y, &w, &h, &bdw, &d)) { 
      ERptErrMessage(ELIXIR_ERROR_CLASS, 1,
                     "Pixmap could not be created in ps_dump", ERROR_GRADE);
      return NO;
    }
    XFillRectangle(display, px, EVEraseGC(v_p), 0, 0, dcx, dcy);
    XCopyArea(display, win, px, EVWritableGC(v_p), 0, 0, dcx, dcy, 0, 0);
  }
  image = XGetImage(display, px, 0, 0, dcx, dcy, plm, ZPixmap);
  if (image == NULL) {
    ERptErrMessage(ELIXIR_ERROR_CLASS, 1,
                   "X image could not be created in ps_dump", ERROR_GRADE);
    return NO;
  }
  XSetErrorHandler(NULL); /* we've caught all errors; reset */
  
  /* Image dimensions */
  postscript_image_size(image, width_in_pts, height_in_pts);
  
  /* first do the header */
  postscript_prolog(fp);

  /* store the image */
  postscript_image(fp, image);
  
  postscript_epilog(fp);

  fclose(fp);

  XDestroyImage(image);
  if (!EVUsingBackBuffer())
    XFreePixmap(display, px);

  ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "\n   Postscript dump: done.");
  
  return YES;
}




static void
postscript_image_size(XImage *image, int width_in_pts, int height_in_pts)
{
  image_width   = image->width;
  image_height  = image->height;
  if (image_height%2) image_height--;     /* must write even number */

  if (eps) {
    image_top_left_x = 0;
    image_top_left_y = 0;
  } else {
    image_top_left_x = 72;
    image_top_left_y = 72;
  }
  
  image_x_trans = image_top_left_x;
  image_y_trans = PAGE_WIDTH_IN_PTS - image_top_left_y - image_height;
}



static void
postscript_image(FILE *fp, XImage *image)
{
  int row, col;
  EPixel pixel;
  unsigned int ui;
  
  for (row = 0; row < image_height; row++) {
    for (col = 0; col < image_width; col++) {
      pixel = XGetPixel(image, col, row);
      ui    = 255 * ColorGreyLevel(pixel);
      fprintf(fp, "%2.2x", ui);
    }
    fprintf(fp, "\n");
  }
}



static void
postscript_prolog(FILE *fp)
{
  time_t tm;

  if (eps) {
    fprintf(fp, "%%!PS-Adobe-2.0 EPSF-2.0\n");
    fprintf(fp,"%%%%Title: %s\n", ps_file_name);
    if (!portrait_orient) {
      fprintf(fp,"%%%%BoundingBox: %d %d %d %d\n", 
	    (int) (image_top_left_y + 0.5),
	    (int) (image_top_left_x + 0.5),
	    (int) (image_top_left_y + 0.5) + image_height,
	    (int) (image_top_left_x + 0.5) + image_width);
    } else {
      fprintf(fp,"%%%%BoundingBox: %d %d %d %d\n",
              image_x_trans, image_y_trans,
              image_x_trans+image_width, image_y_trans+image_height);
    }
  } else {
    fprintf(fp, "%%!PS-ADOBE-2.0\n");
    fprintf(fp,"%%%%Title: %s\n", ps_file_name);
  }
  fprintf(fp, "%%%%Creator: Elixir (C) 1994,1995 Petr Krysl\n");
  fprintf(fp, "%%Pages: 1\n");
  time(&tm);
  fprintf(fp, "%%%%Created on: %s%% :-) %%\n", ctime(&tm));
  fprintf(fp, "%%%%EndComments\n");
  fprintf(fp, "%%EndProlog\n\n");
  if (!eps) 
    fprintf(fp, "%%Page: 1 1\n");
  
  fprintf(fp,"%% remember original state\n");
  fprintf(fp,"/origstate save def\n\n");

  fprintf(fp,"%% build a temporary dictionary\n");
  fprintf(fp,"20 dict begin\n\n");

  fprintf(fp,"%% define string to hold a scanline's worth of data\n");
  fprintf(fp,"/pix %d string def\n\n", image_width);

  if (!portrait_orient) {
    fprintf(fp,"%% print in landscape mode\n");
    fprintf(fp,"90 rotate 0 %d translate\n\n",
            (int)(-PAGE_WIDTH_IN_PTS));
  }

  fprintf(fp,"%% lower left corner\n");
  fprintf(fp,"%d %d translate\n\n", image_x_trans, image_y_trans);

  fprintf(fp,"%% size of image (on paper, in 1/72inch coords)\n");
  fprintf(fp,"%d %d scale\n\n", image_width, image_height);

  fprintf(fp,"%d %d %d\t\t\t%% dimensions of data\n",
          image_width, image_height, image_bits);
  fprintf(fp,"[%d 0 0 %d 0 %d]\t\t%% mapping matrix\n",
          image_width, -image_height, image_height);

  fprintf(fp,"{currentfile pix readhexstring pop}\n");
  fprintf(fp,"image\n");
}


static void
postscript_epilog(FILE *fp)
{
  fprintf(fp,"\n\nshowpage\n\n");

  fprintf(fp,"%% stop using temporary dictionary\n");
  fprintf(fp,"end\n\n");

  fprintf(fp,"%% restore original state\n");
  fprintf(fp,"origstate restore\n\n");
  fprintf(fp,"%%%%Trailer\n");
}

