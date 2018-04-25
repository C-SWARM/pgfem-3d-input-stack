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


#ifndef ECOLORS_H
#define ECOLORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

#include "Etypes.h"
#include "Eview.h"
#include "Egraphic.h"

/* ======================================================================== */
/* PUBLIC FUNCTIONS  */
/* ======================================================================== */

#define ELIXIR_COLOR_SCALE_NUM_LABELS 11


enum{
	ELIXIR_DEFAULT_COLOR_SCALE = 0,
	ELIXIR_STANDARD_COLOR_SCALE = 1,
	ELIXIR_EXTENDED_COLOR_SCALE = 2,
	ELIXIR_MODIFIED_COLOR_SCALE = 3,
	ELIXIR_NUM_COLOR_SCALES
};



EPixel 
ColorGetPixelFromString(char *color_string, BOOLEAN *success);

char *
ColorGetStringFromPixel(EPixel pixel);

EFringeTable 
ColorCreateFringeTable(void);

void
SwitchFringeTableToGrey(void);

void
SwitchFringeTableToColor(void);

FPNum        
ColorFringesDelta(EFringeTable fringes);

FPNum        
ColorFringeClosestLowerValue(EFringeTable fringes, FPNum val);

void         
ColorSetupFringeTableByMinMax(EFringeTable fringes,
															FPNum minval, FPNum maxval);

void         
ColorSetupFringeTableByRange(EFringeTable fringes,
                             EPixel color,
                             FPNum lower, FPNum upper);

FPNum
ColorFringeToValue(EFringeTable fringes, int fringen);

int
ColorFringeValueToRange(EFringeTable fringes, FPNum val);

EPixel
ColorFringeValueToColorHSV(EFringeTable fringes, FPNum val);

EPixel
ColorFringeRangeToColor(int range);

void
ColorFringesMinMax(EFringeTable fringes, FPNum *minv, FPNum *maxv);

void 
ColorHueMinMaxSetup(double min, double max);

void 
GreyValueMinMaxSetup(double min, double max);

BOOLEAN 
IsFringeTableColorScale(void);

BOOLEAN 
IsFringeTableSmoothScale(void);

BOOLEAN 
IsFringeTableContourScale(void);

int 
GetNumColorScales(void);

BOOLEAN 
SetColorScale(int scale);

int
GetColorScale(void);

BOOLEAN
IsSameColorBelow(void);

BOOLEAN
IsSameColorAbove(void);

int 
GetContourWidth(void);

BOOLEAN 
SetContourWidth(int width);

int 
GetContourCount(void);

BOOLEAN 
SetContourCount(int count);

EPixel 
GetContourBgColor(void);

BOOLEAN 
SetContourBgColor(char *color);

BOOLEAN 
IsContourBgTransparent(void);

void 
SetContourBgTransparent(BOOLEAN transparent);

void 
ToggleFringeTableSmoothScale(void);

void 
SetFringeTableSmoothScaleOn(void);
		
void 
SetFringeTableSmoothScaleOff(void);

void 
ToggleFringeTableContourScale(void);

void 
SetFringeTableContourScaleOn(void);
		
void 
SetFringeTableContourScaleOff(void);

void 
RevertFringeTableColorScale(void);


BOOLEAN
ColorLoadColormap(Display *dpy,               /* IN */
                  XVisualInfo *vis_info,        /* IN */
                  BOOLEAN use_default_cmap
                  );

BOOLEAN
ColorLoadStandardColormap(Display *dpy,               /* IN */
			  XVisualInfo *vis_info       /* IN */
			  );
void
ColorSetStdcmapApprox(XVisualInfo *vis_info,        /* IN */
		      XStandardColormap *cmap_info, /* IN */
		      EColorApproxRec *capx_info   /* OUT */
		      );

double
ColorGreyLevel(EPixel rgb_color);

void 
ColorPixelToRGB(EColorApproxRec *color_approx,
                EPixel pixel, double *red, double *green, double *blue); 

EPixel 
ColorRGBtoPixel(EColorApproxRec *color_approx,
                double red, double green, double blue);

void 
ColorPixelToRGBStandard(EPixel pixel,
                        double *red, double *green, double *blue);

EPixel 
ColorRGBtoPixelStandard(double red, double green, double blue);

EPixel 
ColorShadeColor(EPixel pixel, double cos);

void 
ColorShadeInit(double s_cos);

EColorApproxRec *
ColorGetStandardCmapApprox(void);

XStandardColormap *
ColorStandardColormap(void);

void 
HSVtoRGB(double h, double s, double v, double *r, double *g, double *b);

void 
RGBtoHSV(double r, double g, double b, double *h, double *s, double *v);

void
ColorSetAmbientGrey(float grey);

#define ColorPixelFromName(COLOR_NAME, PIXEL_VALUE)                          \
{                                                                            \
  BOOLEAN status_of_ColorGetPixelFromString;                                 \
  PIXEL_VALUE = ColorGetPixelFromString(#COLOR_NAME,                         \
                                        &status_of_ColorGetPixelFromString); \
  if (!status_of_ColorGetPixelFromString)                                    \
    PIXEL_VALUE = 0;                                                         \
}
  
/* ======================================================================== */

#ifdef __cplusplus
}
#endif

#endif
