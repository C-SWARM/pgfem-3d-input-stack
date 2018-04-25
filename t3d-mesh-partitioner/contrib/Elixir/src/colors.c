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
#include "Ecolors.h"
#include "Eutils.h"
#include "Ereprts.h"
#include "Elixir.h"

/* PRIVATE */

#define MAX_RGB 65534.0



#define EXTENDED_SCALE    /* enables to draw colors also below and above fringe table */

#define REDUCED_VALUE        0.50
#define REDUCED_SATURATION   0.75


Display  *
ElixirGetDisplay(void);

Status XmuLookupStandardColormap(
    Display*		/* dpy */,
    int			/* screen */,
    VisualID		/* visualid */,
    unsigned int	/* depth */,
    Atom		/* property */,
    Bool		/* replace */,
    Bool		/* retain */
);

static BOOLEAN
ColorGetStdColormap(Display *dpy, XVisualInfo *vis_info,
		    XStandardColormap *std_cmap);

static Display *display = (Display *)NULL;

static XStandardColormap *StdCMap = NULL;
static EColorApproxRec StdCMapApprox;


typedef struct _ColorPairRec {
  char           *color_name;
  EPixel          pixel;
} ColorPairRec;

static LIST colors = NULL;
static EPixel previous_pixel = 0x0;
static char *previous_color_name = NULL;

static double smallest_cos = 0;

static int search_by_name(char *to_be_searched_for, ColorPairRec *p);
static int search_by_pixel(EPixel to_be_searched_for, ColorPairRec *p);
BOOLEAN ColorNamesEquivalent(char *s1, char *s2);

static double
grey_level_from_rgb(EColorApproxRec *color_approx,
                    double r, double g, double b);

#define SATUR_TANGENT 0.75
#define AMBIENT_GREY  0.25
static float ambient_grey = AMBIENT_GREY;


static char *UNSPECIFIED_COLOR = "white";

/*#define COLOR_APPROX_DEBUG*/

/* PUBLIC */

static char hex_color_name_buf[7];

EPixel 
ColorGetPixelFromString(char *color_string, BOOLEAN *success)
{
  ColorPairRec *p;
  XColor clr;
  EPixel ret_pixel;

  if (strcmp(color_string, UNSPECIFIED_COLOR) == 0) {
    color_string = "white";
  }

  if (previous_color_name != NULL) {
    if (ColorNamesEquivalent(previous_color_name, color_string)) {
      *success = YES;
      return previous_pixel;
    }
  }
  
  if (colors == (LIST)NULL)
    colors = make_list(); 

  if (display == (Display *)NULL)
      display = ElixirGetDisplay();

  if (StdCMap == NULL) {
    fprintf(stderr, "No standard colormap!?\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
  
  if ((p = (ColorPairRec *)search_list(colors, color_string,
                                       search_by_name)) != NULL) {
    ret_pixel = p->pixel;
    *success  = YES;
  } else {
    ret_pixel = BlackPixel(display, DefaultScreen(display));
    if (XParseColor(display, StdCMap->colormap, color_string, &clr) != 0) {
      if (StdCMapApprox.type == COLOR_APPROX_RANGE) {
        /* Adjust the rgb values to gray */
        clr.red = clr.green = clr.blue =
          grey_level_from_rgb(&StdCMapApprox,
                              (double)clr.red/MAX_RGB,
                              (double)clr.green/MAX_RGB,
                              (double)clr.blue/MAX_RGB);
      }
      ret_pixel = ColorRGBtoPixel(&StdCMapApprox,
                                  (double)clr.red/MAX_RGB,
                                  (double)clr.green/MAX_RGB,
                                  (double)clr.blue/MAX_RGB);
      if ((p = (ColorPairRec *)make_node(sizeof(ColorPairRec))) == NULL) {
        EUFailedMakeNode("ColorGetPixelFromString");
      }
      add_to_tail(colors, p);
      p->pixel      = ret_pixel;
      p->color_name = strdup(color_string);
      *success      = YES;
    } else {
      ERptErrMessage(ELIXIR_WARNING_CLASS, 4, ELIXIR_WARNING_4,
                     WARNING_GRADE);
      fprintf(stderr, "Failed activate specified color: %s\n",
	      color_string);
      *success  = NO;
    }
  }
	if(*success == YES){
		previous_pixel = ret_pixel;
		previous_color_name = p->color_name;
	}
	return ret_pixel;
}


char *
ColorGetStringFromPixel(EPixel pixel)
{
  ColorPairRec *p;

  if (previous_pixel == pixel) {
    return previous_color_name;
  }
  
  if (colors == (LIST)NULL) {
    goto convert_to_hex_string;
  }

  if ((p = (ColorPairRec *)search_list(colors, pixel, search_by_pixel))
      != NULL) {
    previous_pixel = pixel;
    previous_color_name = p->color_name; 
    return p->color_name;
  }
  
convert_to_hex_string:
  {
    double red, green, blue;
    ColorPixelToRGBStandard(pixel, &red, &green, &blue);
    sprintf(hex_color_name_buf,
            "#%02X%02X%02X",
            (int)(red*255), (int)(green*255), (int)(blue*255));
    return hex_color_name_buf;
  }
}


static int search_by_name(char *to_be_searched_for, ColorPairRec *p)
{
  if (ColorNamesEquivalent(to_be_searched_for, p->color_name))
    return TRUE;
  else
    return FALSE;
}


static int search_by_pixel(EPixel to_be_searched_for, ColorPairRec *p)
{
  if (to_be_searched_for == p->pixel)
    return TRUE;
  else
    return FALSE;
}


BOOLEAN ColorNamesEquivalent(char *s1, char *s2)
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

/* ======================================================================== */
/* COLOR-TABLE DATA */
/* ======================================================================== */

#define MAXFRG    (ELIXIR_COLOR_SCALE_NUM_LABELS - 1)

#define V_MIN     0.2
#define V_MAX     0.9

static double min_h[ELIXIR_NUM_COLOR_SCALES] = {250.0, 250.0, 305.0, 280.0};
static double max_h[ELIXIR_NUM_COLOR_SCALES] = {  0.0,   0.0,   0.0,   0.0};

/* specify, whether the first/last color is used below/above fringe table */

static BOOLEAN same_below[ELIXIR_NUM_COLOR_SCALES] = {NO, YES, YES, YES};
static BOOLEAN same_above[ELIXIR_NUM_COLOR_SCALES] = {NO, YES, YES, YES};

static BOOLEAN same_below_grey = YES;
static BOOLEAN same_above_grey = YES;

static int color_scale = 0;
static unsigned int contour_width = 0, contour_count = 0;

static EPixel contour_bg_pixel = 0;
static BOOLEAN contour_bg_transparent = YES;

static BOOLEAN GreyScale = NO, SmoothScale = NO, ContourScale = NO, RevertColor = NO, RevertGrey = NO;

static double v_min = V_MIN;
static double v_max = V_MAX;

static double h_min = 250.0;     /* min_h[color_scale] */
static double h_max = 0.0;       /* max_h[color_scale] */


/* first (color[0]) and last (color[MAXFRG+2]) colors are only used 
	 for outside of the fringe table */
/* note that ranges in fringe table are numbered from 1, just color[0]
	 must be defined anyway even if normally being unused */


/*
static char *color_names_table[MAXFRG+3] = {
  "black",
  "slateblue",
  "blue",
  "lightblue",
  "cyan",
  "green",
  "greenyellow",
  "yellow",
  "orange",
  "red",
  "hotpink",
  "blueviolet"
  "white"
}; 
*/

/*
static ERGBColor rgb_Color[MAXFRG+3] = {
  {0.4140625,  0.3515625,  0.80078125},
  {0.4140625,  0.3515625,  0.80078125},
  {0,          0,          1},
  {0.67578125, 0.84375,    0.8984375},
  {0,          1,          1},
  {0,          1,          0},
  {0.67578125, 1,          0.18359375},
  {1,          1,          0},
  {1,          0.64453125, 0},
  {1,          0,          0},
  {1,          0.41015625, 0.703125},
	{0.5390625,  0.16796875, 0.8828125},
	{0.5390625,  0.16796875, 0.8828125}
};
*/

/* note: the first/last color is used to draw values below/above fringe table */

static ERGBColor rgb_Grey[MAXFRG+3] = {
  {0.090909091, 0.090909091, 0.090909091},
  {0.090909091, 0.090909091, 0.090909091},
  {0.18181818,  0.18181818,  0.18181818},
  {0.27272727,  0.27272727,  0.27272727},
  {0.36363636,  0.36363636,  0.36363636},
  {0.45454545,  0.45454545,  0.45454545},
  {0.54545455,  0.54545455,  0.54545455},
  {0.63636364,  0.63636364,  0.63636364},
  {0.72727273,  0.72727273,  0.72727273},
  {0.81818182,  0.81818182,  0.81818182},
  {0.90909091,  0.90909091,  0.90909091},
  {1.0,         1.0,         1.0 },
  {1.0,         1.0,         1.0 }
};


static ERGBColor rgb_Color[ELIXIR_NUM_COLOR_SCALES][MAXFRG+3] = {
	{
		{0.125,      0.125,      0.5},      /* REDUCED_VALUE, REDUCED_SATURATION */
		{0,          0,          1},
		{0,          0.59607843, 1},
		{0,          0.79607843, 1},
		{0,          1,          1},
		{0,          1,          0.62745098},
		{0,          1,          0},
		{0.61568627, 1,          0},
		{1,          1,          0},
		{1,          0.78431373, 0},
		{1,          0.47450980, 0},
		{1,          0,          0},
		{0.5,        0.125,      0.125}      /* REDUCED_VALUE, REDUCED_SATURATION */
	},
	{
		{0,          0,          1},
		{0,          0,          1},
		{0,          0.59607843, 1},
		{0,          0.79607843, 1},
		{0,          1,          1},
		{0,          1,          0.62745098},
		{0,          1,          0},
		{0.61568627, 1,          0},
		{1,          1,          0},
		{1,          0.78431373, 0},
		{1,          0.47450980, 0},
		{1,          0,          0},
		{1,          0,          0}
	},
	{
		{1,          0.41015625, 0.703125},
		{1,          0.41015625, 0.703125},
		{0.7372549,  0.28627451, 0.68627451},
		{0,          0,          1},
		{0.43921569, 0.43921569, 0.87843137},
		{0.52156863, 0.63137255, 0.86666667},
		{0,          1,          1},
		{0,          1,          0},
		{0.61568627, 1,          0.18359375},
		{1,          1,          0},
		{1,          0.64453125, 0},
		{1,          0,          0},
		{1,          0,          0}
	},
	{
		{0.70588235, 0,          1},
		{0.70588235, 0,          1},
		{0,          0,          1},
		{0.43921569, 0.43921569, 0.87843137},
		{0.52156863, 0.63137255, 0.86666667},
		{0.67578125, 0.84375,    0.8984375},
		{0,          1,          1},
		{0,          1,          0},
		{0.67578125, 1,          0.18359375},
		{1,          1,          0},
		{1,          0.64453125, 0},
		{1,          0,          0},
		{1,          0,          0}
	}
};


static EPixel Colors[MAXFRG+3] = {
  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0
};

void DefaultRangeColors(void);
void DefaultRangeGreys(void);

/* ======================================================================== */

void DefaultRangeColors(void)
{
  int k;
   
	if(RevertColor == NO){
		for (k = 0; k <= MAXFRG + 2; k++) {
			Colors[k] = ColorRGBtoPixelStandard(rgb_Color[color_scale][k].R, 
																					rgb_Color[color_scale][k].G, 
																					rgb_Color[color_scale][k].B);
		}
	}
	else{
		for (k = 0; k <= MAXFRG + 2; k++) {
			Colors[MAXFRG + 2 - k] = ColorRGBtoPixelStandard(rgb_Color[color_scale][k].R, 
																											 rgb_Color[color_scale][k].G, 
																											 rgb_Color[color_scale][k].B);
		}
	}
		
	GreyScale = NO;
}

void DefaultRangeGreys(void)
{
  int k;
	double delta, value;
   
	delta = (v_max - v_min) / (double)MAXFRG;

	if(RevertGrey == NO){
		for (k = 1; k <= MAXFRG + 1; k++) {
/*			Colors[k] = ColorRGBtoPixelStandard(rgb_Grey[k].R, rgb_Grey[k].G, rgb_Grey[k].B); */
			value = v_min + delta * (k - 1);
			Colors[k] = ColorRGBtoPixelStandard(value, value, value);
		}
	}
	else{
		for (k = 1; k <= MAXFRG + 1; k++) {
/*			Colors[MAXFRG + 2 - k] = ColorRGBtoPixelStandard(rgb_Grey[k].R, rgb_Grey[k].G, rgb_Grey[k].B); */
			value = v_min + delta * (k - 1);
			Colors[k] = ColorRGBtoPixelStandard(value, value, value);
		}
	}

	Colors[0] = Colors[1];
	Colors[MAXFRG + 2] = Colors[MAXFRG + 1];

	GreyScale = YES;
}

/* ======================================================================== */

void ColorHueMinMaxSetup(double min, double max)
{
	if(min == max){
		fprintf(stderr,"Invalid min/max in ColorHueMinMaxSetup\n");
		return;
  }

	if(min < max){
		if(min < -360.0 || max > 360.0){
			fprintf(stderr,"Invalid min/max in ColorHueMinMaxSetup\n");
			return;
		}
	}
	else{
		if(max < -360.0 || min > 360.0){
			fprintf(stderr,"Invalid min/max in ColorHueMinMaxSetup\n");
			return;
		}
	}

	h_min = min;
	h_max = max;

	DefaultRangeColors();
}

void GreyValueMinMaxSetup(double min, double max)
{
	if(min == max){
		fprintf(stderr,"Invalid min/max in GreyValueMinMaxSetup\n");
		return;
  }

	if(min < max){
		if(min < 0.0 || max > 1.0){
			fprintf(stderr,"Invalid min/max in GreyValueMinMaxSetup\n");
			return;
		}
	}
	else{
		if(max < 0.0 || min > 1.0){
			fprintf(stderr,"Invalid min/max in GreyValueMinMaxSetup\n");
			return;
		}
	}

	v_min = min;
	v_max = max;

	DefaultRangeGreys();
}

/* ======================================================================== */

BOOLEAN IsFringeTableColorScale(void)
{
	return(GreyScale);
}


BOOLEAN IsFringeTableSmoothScale(void)
{
	return(SmoothScale);
}


BOOLEAN IsFringeTableContourScale(void)
{
	return(ContourScale);
}

/* ======================================================================== */

int GetNumColorScales(void)
{
	return(ELIXIR_NUM_COLOR_SCALES);
}

BOOLEAN SetColorScale(int scale)
{
	if(scale >= 0 && scale < ELIXIR_NUM_COLOR_SCALES){
		color_scale = scale;

		if(RevertColor == NO){
			h_min = min_h[scale];
			h_max = max_h[scale];
		}
		else{
			h_max = min_h[scale];
			h_min = max_h[scale];
		}

		DefaultRangeColors();

		return(YES);
	}

	return(NO);
}

int GetColorScaleNum(void)
{
	return(color_scale);
}

/* ======================================================================== */

BOOLEAN IsSameColorBelow(void)
{
	if(GreyScale == NO)return(same_below[color_scale]);
	return(same_below_grey);
}

BOOLEAN IsSameColorAbove(void)
{
	if(GreyScale == NO)return(same_above[color_scale]);
	return(same_above_grey);
}

/* ======================================================================== */

int GetContourWidth(void)
{
	return((int)contour_width);
}

BOOLEAN SetContourWidth(int width)
{
	if(width < 0)return(NO);

	contour_width = (unsigned)width;
	return(YES);
}

int GetContourCount(void)
{
	return((int)contour_count);
}

BOOLEAN SetContourCount(int count)
{
	if(count < 0)return(NO);

	contour_count = (unsigned)count;
	return(YES);
}

EPixel GetContourBgColor(void)
{
	return(contour_bg_pixel);
}

BOOLEAN SetContourBgColor(char *color)
{
	BOOLEAN success;
	EPixel pixel;

	pixel = ColorGetPixelFromString(color, &success);
	if(success == YES)contour_bg_pixel = pixel;
	return(success);
}

BOOLEAN IsContourBgTransparent(void)
{
	return(contour_bg_transparent);
}

void SetContourBgTransparent(BOOLEAN transparent)
{
	contour_bg_transparent = transparent;
}

/* ======================================================================== */

void ToggleFringeTableSmoothScale(void)
{
	if(SmoothScale == YES)
		SetFringeTableSmoothScaleOff();
	else
		SetFringeTableSmoothScaleOn();
}


void SetFringeTableSmoothScaleOn(void)
{
	SmoothScale = YES;
}
		
void SetFringeTableSmoothScaleOff(void)
{
	SmoothScale = NO;
}

/* ======================================================================== */

void ToggleFringeTableContourScale(void)
{
	if(ContourScale == YES)
		SetFringeTableContourScaleOff();
	else
		SetFringeTableContourScaleOn();
}


void SetFringeTableContourScaleOn(void)
{
	ContourScale = YES;
}
		
void SetFringeTableContourScaleOff(void)
{
	ContourScale = NO;
}

/* ======================================================================== */

void RevertFringeTableColorScale(void)
{
	double val;

	if(GreyScale == NO){
		val = h_min;
		h_min = h_max;
		h_max = val;
		
		RevertColor = !RevertColor;

		DefaultRangeColors();
	}
	else{
		val = v_min;
		v_min = v_max;
		v_max = val;
		
		RevertGrey = !RevertGrey;
		
		DefaultRangeGreys();
	}
}

/* ======================================================================== */

FPNum ColorFringesDelta(EFringeTable fringes)
{
  return fringes[2] - fringes[1];
}

/* ======================================================================== */

void ColorFringesMinMax(EFringeTable fringes, FPNum *minv, FPNum *maxv)
{
  double delta = fringes[2] - fringes[1];
  
/* modified to return real min and max */

  *minv = fringes[1] - delta;
  *maxv = fringes[MAXFRG] + delta;
}

/* ======================================================================== */

EFringeTable ColorCreateFringeTable(void)
{
  EFringeTable f;

  f = (EFringeTable)make_node(sizeof(FPNum)*(MAXFRG+1));
  if (f == NULL) {
    EUFailedMakeNode("ColorCreateFringeTable");
  }
  return f;
}


/* ======================================================================== */

FPNum ColorFringeClosestLowerValue(EFringeTable fringes, FPNum val)
{
  FPNum delta, v;
  
  delta = fringes[2] - fringes[1];
  if     (val >= fringes[1]) {
    v = fringes[1]; 
  label_1:
    if (val <= v) 
      return (v - delta);
    v = v + delta;
    goto label_1;
  } else {
    v = fringes[1] - delta;
  label_2:
      if (val >= v) 
	return v;
    v = v - delta;
    goto label_2;
  }
}

/* ======================================================================== */

void
SwitchFringeTableToGrey(void)
{
	DefaultRangeGreys();
}

void
SwitchFringeTableToColor(void)
{
	DefaultRangeColors();
}

/* ======================================================================== */

void 
ColorSetupFringeTableByMinMax(EFringeTable fringes, 
															FPNum minval, FPNum maxval)
{
/* Setup the fringe of the scalar field contours:
      color    1 |  2  |     |    ... |        | MAXFRG+1
      range    1 |  2  |     |    ... | MAXFRG | MAXFRG+1 
      fringe     1     2     3     MAXFRG-1  MAXFRG
              min                                 max
            |                                          |
         minval                                      maxval
 */
  FPNum deltav;
  int ir;
  
/* originally, the effective size of the first and the last range was half because
   minval corresponded to center of the first range and maxval to the center
	 of the last range ==> correction of minval and maxval to make the 
	 first and last range of full effective size */

	deltav = (maxval - minval) / (MAXFRG + 1);
  if (deltav <= 0.0) {
    fprintf(stderr,"Invalid delta in ColorSetupFringeTableByMinMax\n");
    deltav = 1.0;
  }

  fringes[1] = minval + deltav;
  for (ir = 2; ir <= MAXFRG; ir++)
    fringes[ir] = fringes[ir - 1] + deltav;

	if(GreyScale == NO)
		DefaultRangeColors();
	else
		DefaultRangeGreys();
}

/* ======================================================================== */

void 
ColorSetupFringeTableByRange(EFringeTable fringes,
                             EPixel color,
                             FPNum lower, FPNum upper)
{
  int foundpos, ir;
  FPNum deltav = fringes[2] - fringes[1];
  FPNum minval, maxval;
  
	if(GreyScale == NO)
		DefaultRangeColors();
	else
		DefaultRangeGreys();

  if (upper < lower) {
    FPNum tmp;

    fprintf(stderr,"Invalid range bound in ColorSetupFringeTableByRange\n");
    tmp = upper; upper = lower; lower = tmp;
  } else if (upper == lower) {
    fprintf(stderr,"Invalid range bound in ColorSetupFringeTableByRange\n");
    upper = lower + 1.;
  }
    
  for (ir = 1; ir <= MAXFRG+1; ir++) {
    if (Colors[ir] == color) {
      foundpos = ir;
      goto found_color;
    }
  }
  fprintf(stderr, "No such color %lu\n", color);
  foundpos = MAXFRG;

/* seems to make no sense;
	 new min and max values are calculated from given color bounds
	 using original deltav ???
	 should not be deltav calculated as upper - lower ??? */

 found_color:
  minval = lower - (foundpos - 1) * deltav /* + deltav * 0.5 */;
  maxval = upper + (MAXFRG + 1 - foundpos) * deltav /* - deltav * 0.5 */;

  ColorSetupFringeTableByMinMax(fringes, minval, maxval);
}


FPNum 
ColorFringeToValue(EFringeTable fringes, int fringen)
{
  if (fringen > 0 && fringen <= MAXFRG) 
    return fringes[fringen];
  else {
    fprintf(stderr, "Invalid fringe number in ColorFringeToValue\n");
    return 0.0;
  }
}



int 
ColorFringeValueToRange(EFringeTable fringes, FPNum val)
{
	double delta = fringes[2] - fringes[1];
  int i;

  for (i = MAXFRG; i >= 2; i--) {
    if (val <= fringes[i]) {
      if (val > fringes[i-1]) 
				return i;
    } else {
#ifdef EXTENDED_SCALE
			if(val > fringes[MAXFRG] + delta)return MAXFRG+2;
#endif
      return MAXFRG+1;
    }
  }
#ifdef EXTENDED_SCALE
	if(val < fringes[1] - delta)
		return 0;
#endif
  return 1;
}



EPixel
ColorFringeValueToColorHSV(EFringeTable fringes, FPNum val)
{
	double delta = fringes[2] - fringes[1];
  double tmin = fringes[1] - delta;
  double tmax = fringes[MAXFRG] + delta;
  double h, s = 1.0, v = 1.0, r, g, b;
	int range;
 
	if(SmoothScale == NO){
		range = ColorFringeValueToRange(fringes, val);
		return(ColorFringeRangeToColor(range));
	}
		
/* use reduced value to make the colors below and above the table
	 darker than the colors of the first and last fringe */

  if (val < tmin){
		val = tmin;
		if(same_below[color_scale] == NO){
			v = REDUCED_VALUE;
			s = REDUCED_SATURATION;
			
		}
	}
  if (val > tmax){
		val = tmax;
		if(same_above[color_scale] == NO){
			v = REDUCED_VALUE;
			s = REDUCED_SATURATION;
		}
	}

	if(GreyScale == NO){
		//    s = 1.0;
		//		v = 1.0;
		h = h_min + (h_max - h_min) / (tmax - tmin) * (val - tmin);
		if(h < 0.0)
			h += 359.9999999;
		else{
			if(h > 359.9999999)h -= 359.9999999;
		}
		HSVtoRGB(h, s, v, &r, &g, &b);
	}
	else{
		v = v_min + (v_max - v_min) / (tmax - tmin) * (val - tmin);
		if(v < 0.0)v = 0.0;
		if(v > 1.0)v = 1.0;
		r = g = b = v;
	}

  return ColorRGBtoPixelStandard(r, g, b);
}




EPixel 
ColorFringeRangeToColor(int range)
{
#ifdef EXTENDED_SCALE
  if (range < 1)
    return Colors[0];
  if (range > MAXFRG + 1)
    return Colors[MAXFRG+2];
#else
  if (range < 1)
    return Colors[1];
  if (range > MAXFRG + 1)
    return Colors[MAXFRG+1];
#endif
  return Colors[range];
}




void
ColorSetStdcmapApprox(XVisualInfo *vis_info,        /* IN */
		      XStandardColormap *cmap_info, /* IN */
		      EColorApproxRec *capx_info   /* OUT */
		      )
{
  switch (vis_info->class) {
  case DirectColor:
  case TrueColor:
  case PseudoColor:
  case StaticColor:
  default:
    capx_info->type = COLOR_APPROX_SPACE;
    capx_info->base_pixel = cmap_info->base_pixel;
    capx_info->max1 = cmap_info->red_max;
    capx_info->max2 = cmap_info->green_max;
    capx_info->max3 = cmap_info->blue_max;
    /* Give the weights the NTSC intensity coefficients. */
    capx_info->weight1 = 0.299; 
    capx_info->weight2 = 0.587;
    capx_info->weight3 = 0.114;
    capx_info->mult1 = cmap_info->red_mult;
    capx_info->mult2 = cmap_info->green_mult;
    capx_info->mult3 = cmap_info->blue_mult;
    break;
  case GrayScale:
  case StaticGray:
    capx_info->type = COLOR_APPROX_RANGE;
    capx_info->base_pixel = cmap_info->base_pixel;
    capx_info->max1 = cmap_info->red_max;
    capx_info->max2 = 0; /* not used by COLOR_APPROX_RANGE */
    capx_info->max3 = 0; /* not used by COLOR_APPROX_RANGE */
    /* Give the weights the NTSC intensity coefficients. */
    capx_info->weight1 = 0.299; 
    capx_info->weight2 = 0.587;
    capx_info->weight3 = 0.114;
    capx_info->mult1 = cmap_info->red_mult;
    capx_info->mult2 = 0;
    capx_info->mult3 = 0;
    break;
  }
}


BOOLEAN
ColorLoadStandardColormap(Display *dpy,               /* IN */
			 XVisualInfo *vis_info        /* IN */
			 )
{
  StdCMap = (XStandardColormap *)malloc(sizeof(XStandardColormap));
  if (ColorGetStdColormap(dpy, vis_info, StdCMap)) {
    ColorSetStdcmapApprox(vis_info, StdCMap, &StdCMapApprox);
    return YES;
  } else
    return NO;
}


static BOOLEAN
ColorGetStdColormap(Display *dpy, XVisualInfo *vis_info,
		    XStandardColormap *std_cmap)
{
  XStandardColormap	*std_cmaps;
  Atom		        property;
  int			i, num_cmaps, num_of_retries;
  
  switch (vis_info->class) {
  case TrueColor:
  case StaticColor:
    property = XA_RGB_BEST_MAP;
    break;
  case PseudoColor:
  case DirectColor:
  default:
    property = XA_RGB_DEFAULT_MAP;
    break;
  case StaticGray:
  case GrayScale:
    property = XA_RGB_GRAY_MAP;
    break;
  }

  num_of_retries = 0;
 retry:
  /* Ensure that the property is defined. */
  if (XmuLookupStandardColormap(dpy, vis_info->screen,
				vis_info->visualid,
				(unsigned)vis_info->depth, property,
				False, True)) {
    
    /* Get the standard colormap properties. */
    if (XGetRGBColormaps(dpy,
			 RootWindow(dpy, DefaultScreen(dpy)),
			 &std_cmaps, &num_cmaps, property)) {
      
      /* Find the properties for the specified visual. */
      for (i = 0; i < num_cmaps; i++) {
	if (vis_info->visualid == std_cmaps[i].visualid) {
	  *std_cmap = std_cmaps[i];
	  return YES;
	}
      }
      XFree((char *)std_cmaps);
    }
  }
  if (property == XA_RGB_DEFAULT_MAP && num_of_retries < 3) {
    property = XA_RGB_BEST_MAP; /* If we used the default map, we might */
                                /* try the best;  and the other way round */
    num_of_retries++;
    goto retry;
  } else if (property == XA_RGB_BEST_MAP && num_of_retries < 3) {
    property = XA_RGB_DEFAULT_MAP;
    num_of_retries++;
    goto retry;
  }
  /* Now we are really at a loss what to do  */
  return NO;
}


void ColorPixelToRGB(EColorApproxRec *color_approx,
		     EPixel pixel,
		     double *red, 
		     double *green, 
		     double *blue
		     )
{
  EPixel p;
  unsigned redp, greenp, bluep;
  
  if (color_approx->type == COLOR_APPROX_SPACE) {
    p = pixel - color_approx->base_pixel;
    redp   = p / color_approx->mult1;
    greenp = (p - redp*color_approx->mult1) / color_approx->mult2;
    bluep  = (p - redp*color_approx->mult1 - greenp*color_approx->mult2) /
      color_approx->mult3;
    *red   = (double)(redp)/(double)color_approx->max1;
    *green = (double)(greenp)/(double)color_approx->max2;
    *blue  = (double)(bluep)/(double)color_approx->max3;
  } else if (color_approx->type == COLOR_APPROX_RANGE) {
    /* RAW:  Needs to be computed somehow */
  } else {
    *red = *green = *blue = 0;
  }
}


EPixel ColorRGBtoPixel(EColorApproxRec *color_approx,
			    double red, 
			    double green, 
			    double blue 
			    )
{
  float s;
  unsigned i;
  
  if (color_approx->type == COLOR_APPROX_SPACE) {
    red    *= color_approx->max1;
    green  *= color_approx->max2;
    blue   *= color_approx->max3;
    return (EPixel)(color_approx->mult1 * (unsigned)(red) +
			 color_approx->mult2 * (unsigned)(green) +
			 color_approx->mult3 * (unsigned)(blue) +
			 color_approx->base_pixel);
  } else if (color_approx->type == COLOR_APPROX_RANGE) {
    s = MAX_RGB * grey_level_from_rgb(color_approx, red, green, blue);
    i = (unsigned)(s);
    return (EPixel)(i * color_approx->mult1 + color_approx->base_pixel);
  } else {
    return (EPixel)0;
  }
}


void 
ColorPixelToRGBStandard(EPixel pixel,
                        double *red, 
                        double *green, 
                        double *blue
                        )
{
  ColorPixelToRGB(&StdCMapApprox, pixel, red, green, blue);
}


EPixel 
ColorRGBtoPixelStandard(double red, double green, double blue)
{
  return ColorRGBtoPixel(&StdCMapApprox, red, green, blue);
}


EPixel 
ColorShadeColor(EPixel pixel, double cosine)
{
  double r, g, b;
  double interp_cos;

  ColorPixelToRGB(&StdCMapApprox, pixel, &r, &g, &b);
  interp_cos = max(0, cosine);
#if 0
/* This shading was done in the HSV color description. */
  {
     double h, s, v, V, S;
     RGBtoHSV(r, g, b, &h, &s, &v);
     V = smallest_cos + (1 - smallest_cos) * interp_cos;
     V = 1 - (1 - V) * v;
     S = s /* - SATUR_TANGENT * (interp_cos - smallest_cos) */; 
     HSVtoRGB(h, S, V, &r, &g, &b);
   }
#endif
/* This is done in RGB directly.  Seems to be better, especially */
/* for ambient set to zero (gives the best colors). */
  r *= interp_cos; g *= interp_cos; b *= interp_cos;
  r += ambient_grey; g += ambient_grey; b += ambient_grey;
  return ColorRGBtoPixel(&StdCMapApprox, min(r, 1), min(g, 1), min(b, 1));
}


void 
ColorShadeInit(double s_cos)
{
  smallest_cos = s_cos;
}


EColorApproxRec *ColorGetStandardCmapApprox(void)
{
  return &StdCMapApprox;
}


void 
RGBtoHSV(double r, double g, double b,
		     double *h, double *s, double *v)
{
  double amax, amin, delta;

  amax = max(r, g); amax = max(amax, b);
  amin = min(r, g); amin = min(amin, b);
  *v = amax;
  if (amax == 0)
    *s = 0;
  else
    *s = (amax - amin)/amax;
  if (*s == 0)
    *h = -1;			/* not defined */
  else {
    delta = amax - amin;
    if (r == amax)
      *h = (g-b)/delta;
    else
      if (g == amax)
				*h = 2 + (b-r)/delta;
      else
				*h = 4 + (r-g)/delta;
    *h = *h * 60;
    if (*h < 0)
      *h += 360;
  }
}

void 
HSVtoRGB(double h, double s, double v,
				 double *r, double *g, double *b)
{
  int i;
  double f, p, q, t;

  if (h == -1) {
    *r = *g = *b = v;
  } else {
    if (s == 0)			/* error */
      fprintf(stderr, "Bad input in HSVtoRGB\n");
    else {
      h /= 60;
      i = h;
      f = h - i;
      p = v * (1 - s);
      q = v * (1 - (s * f));
      t = v * (1 - (s * (1 - f)));
      switch (i) {
      case 0:
				*r = v;	*g = t;	*b = p;
				break;
      case 1:
				*r = q;	*g = v;	*b = p;
				break;
      case 2:
				*r = p;	*g = v;	*b = t;
				break;
      case 3:
				*r = p;	*g = q;	*b = v;
				break;
      case 4:
				*r = t;	*g = p;	*b = v;
				break;
      case 5:
				*r = v;	*g = p;	*b = q;
				break;
      }
    }
  }
}
	


XStandardColormap *
ColorStandardColormap(void)
{
  return StdCMap;
}


void
ColorSetAmbientGrey(float grey)
{
  ambient_grey = max((min(grey, 1)), 0);
}



double
ColorGreyLevel(EPixel rgb_color)
{
  double r, g, b;
  
  ColorPixelToRGBStandard(rgb_color, &r, &g, &b);
  return grey_level_from_rgb(&StdCMapApprox, r, g, b);
}


static double
grey_level_from_rgb(EColorApproxRec *color_approx,
                    double r, double g, double b)
{
  return (  r * color_approx->weight1 /* NTSC weights probably */
          + g * color_approx->weight2
          + b * color_approx->weight3);
}

