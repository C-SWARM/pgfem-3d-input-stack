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

/* This file holds the redefinition of graphic methods for the annotation
   text. */

#include "Econfig.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Eview.h"
#include "Egraphic.h"

#define ANNTEXT3D_PRIVATE_HEADER
#include "Eanntxt3d.h"
#include "Egeomops.h"
#include "Ecolors.h"
#include "Efonts.h"
#include "Eactset.h"
#include "Elixir.h"


  
/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 1024

static char string_rep_buffer[STRING_BUFF_SIZE];
#define BUF string_rep_buffer

#define DUPL_STRING(dest, src)                      \
   if (((dest) = (char *)malloc((size_t)(strlen(src)+1))) == NULL) \
      fprintf(stderr, "DUPL_STRING: not enough memory"), \
      exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT)); \
   strcpy((dest), (src))

#define IS_NEWLINE_ESCAPE(at_p) ((*at_p == '\\') && ( *(at_p+1) == 'n'))
#define NULL_CHAR '\0'

/* ========================================================================= */
/* PUBLIC METHODS */
/* ========================================================================= */

GraphicObj *
CreateAnnText3D(WCRec *pos, const char *text)
{
  EAnnText3DWCRec *p;
  GraphicObj *g_p;
  Display *d;

  p = (EAnnText3DWCRec *)make_node(sizeof(EAnnText3DWCRec));
  g_p = EGCreateGraphics(&anntext3d_graphic_methods,
			 (caddr_t)p, sizeof(EAnnText3DWCRec));
  p->at.x = pos->x;   p->at.y = pos->y;   p->at.z = pos->z;
	if(text == NULL){
		char notext[] = "No text specified";
		DUPL_STRING(p->text, notext);
	}
	else{
		DUPL_STRING(p->text, text);
	}
  p->attributes.spec.have_any = YES;
  p->attributes.spec.font  = FontDefaultFont();
  d = ElixirGetDisplay();
  p->attributes.spec.color = WhitePixel(d, DefaultScreen(d));
  return g_p;
}

 
static GraphicObj *
ModifyGeometry(GraphicObj *g_p, WCRec *at)
{
  EAnnText3DWCRec *r_p;

  r_p = (EAnnText3DWCRec *)(g_p->spec.data);
  r_p->at.x = at->x; r_p->at.y = at->y; r_p->at.z = at->z;
  return g_p;
}


static BOOLEAN 
GetGeometry(GraphicObj *g_p, WCRec *points, int *npoints)
{
  EAnnText3DWCRec *m_p = (EAnnText3DWCRec *)(g_p->spec.data);

  points[0].x = m_p->at.x;
  points[0].y = m_p->at.y;
  points[0].z = m_p->at.z;
  *npoints    = 1;
  return YES;
}


static void 
Draw(EView *v_p, GraphicObj *g_p)
{
  EAnnText3DWCRec *thedp;
  
  thedp = (EAnnText3DWCRec *)(g_p->spec.data);
  EVAnnText3DPrimitive(v_p,  g_p,&(thedp->at), thedp->text,
		       thedp->attributes.spec.font,
		       thedp->attributes.spec.color,
		       DRAW_MODE_DRAW);
}



static void 
BoundingBoxVC(EView *v_p, GraphicObj *p,
			  VCRec *ll_return, VCRec *ur_return)
{
  EAnnText3DWCRec *thedp;
  VCRec vcp;

  thedp = (EAnnText3DWCRec *)(p->spec.data);
  EVWCtoVC(v_p, &(thedp->at), &vcp);
  ll_return->u = vcp.u;
  ll_return->v = vcp.v;
  ll_return->n = vcp.n;
  ur_return->u = vcp.u;
  ur_return->v = vcp.v;
  ur_return->n = vcp.n;
}



static void 
BoundingBoxWC(GraphicObj *p, WCRec *ll_return, WCRec *ur_return)
{
  EAnnText3DWCRec *thedp;

  thedp = (EAnnText3DWCRec *)(p->spec.data);
  ll_return->x = thedp->at.x;
  ll_return->y = thedp->at.y;
  ll_return->z = thedp->at.z;
  ur_return->x = thedp->at.x;
  ur_return->y = thedp->at.y;
  ur_return->z = thedp->at.z;
}



static void 
XorDraw(EView *v_p, GraphicObj *g_p)
{
  EAnnText3DWCRec *thedp;
  
  thedp = (EAnnText3DWCRec *)(g_p->spec.data);
  EVAnnText3DPrimitive(v_p, g_p, &(thedp->at), thedp->text,
		       thedp->attributes.spec.font,
		       thedp->attributes.spec.color,
		       DRAW_MODE_XORDRAW);
}



static void 
Hilite(EView *v_p, GraphicObj *g_p)
{
  EAnnText3DWCRec *thedp;
  
  thedp = (EAnnText3DWCRec *)(g_p->spec.data);
  EVAnnText3DPrimitive(v_p, g_p, &(thedp->at), thedp->text,
		       thedp->attributes.spec.font,
		       thedp->attributes.spec.color,
		       DRAW_MODE_HILITE);
}



static void 
Unhilite(EView *v_p, GraphicObj *g_p)
{
  EAnnText3DWCRec *thedp;
  
  thedp = (EAnnText3DWCRec *)(g_p->spec.data);
  EVAnnText3DPrimitive(v_p,  g_p,&(thedp->at), thedp->text,
		       thedp->attributes.spec.font,
		       thedp->attributes.spec.color,
		       DRAW_MODE_UNHILITE);
}



static void 
Erase(EView *v_p, GraphicObj *g_p)
{
  EAnnText3DWCRec *thedp;
  
  thedp = (EAnnText3DWCRec *)(g_p->spec.data);
  EVAnnText3DPrimitive(v_p,  g_p,&(thedp->at), thedp->text,
		       thedp->attributes.spec.font,
		       thedp->attributes.spec.color,
		       DRAW_MODE_ERASE);
}



static GraphicObj *
Translate(GraphicObj *g_p, WCRec *by)
{
  EAnnText3DWCRec *c;

  c = (EAnnText3DWCRec *)g_p->spec.data;
  c->at.x += by->x, c->at.y += by->y, c->at.z += by->z;
  return g_p;
}



static GraphicObj *
Rotate(GraphicObj *g_p,
			    WCRec *center, WCRec *axial)
{
  EAnnText3DWCRec *c;
  WCRec x_c;

  c = (EAnnText3DWCRec *)g_p->spec.data;
  x_c.x = c->at.x - center->x;
  x_c.y = c->at.y - center->y;
  x_c.z = c->at.z - center->z;
  RotVectAboutVect(axial, &x_c);
  c->at.x = center->x + x_c.x;
  c->at.y = center->y + x_c.y;
  c->at.z = center->z + x_c.z;
  return g_p;
}



static GraphicObj  *
MirrorPlane(GraphicObj *g_p,
                     WCRec *point_on_plane, WCRec *unit_plane_normal)
{
  EAnnText3DWCRec *dp;

  dp = (EAnnText3DWCRec *)g_p->spec.data;
  MirrorPointInPlane(point_on_plane, unit_plane_normal, &dp->at);
  
  return g_p;
}



static GraphicObj  *
MirrorCenter(GraphicObj *g_p, WCRec *center)
{
  EAnnText3DWCRec *dp;

  dp = (EAnnText3DWCRec *)g_p->spec.data;
  dp->at.x = 2.*center->x - dp->at.x;
  dp->at.y = 2.*center->y - dp->at.y;
  dp->at.z = 2.*center->z - dp->at.z;
  return g_p;
}



static GraphicObj *
Scale(GraphicObj *g_p, WCRec *center,
			   FPNum sx, FPNum sy, FPNum sz)
{
  return g_p;
}



static BOOLEAN 
HitByBox(EView *v_p, EBoxVC2DRec *box,
			ESelectCriteria sel_crit, GraphicObj *p)
{
  EAnnText3DWCRec *c;
  VCRec vcp;
  
  c = (EAnnText3DWCRec *)p->spec.data;
  
  EVWCtoVC(v_p, &(c->at), &vcp);
  if (vcp.u >= box->left   && vcp.u <= box->right &&
      vcp.v >= box->bottom && vcp.v <= box->top)
    return YES;
  else
    return NO;
}



static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_ANNTEXT3D;
}



static char *
AsString(GraphicObj *g_p)
{
  EAnnText3DWCRec *c;

  c = (EAnnText3DWCRec *)g_p->spec.data;
  sprintf(string_rep_buffer,
	  "ANNTEXT3D at [%f,%f,%f], text %s",
	  c->at.x, c->at.y, c->at.z, c->text);
    
  return string_rep_buffer; /* Don't free this memory!!! */
}



static LIST 
KeyPoints(GraphicObj *p)
{
  LIST temp_list;
  KeyPointRec *key_p;
  EAnnText3DWCRec *m_p;

  m_p       = (EAnnText3DWCRec *)(p->spec.data);
  temp_list = make_list();
  key_p     =
    (KeyPointRec*)add_to_tail(temp_list, make_node(sizeof(KeyPointRec)));
  key_p->coords.x      = m_p->at.x;
  key_p->coords.y      = m_p->at.y;
  key_p->coords.z      = m_p->at.z;
  key_p->dist_from_hit = 0.0;

  return temp_list;
}



static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  EAnnText3DWCRec *p;
  Font fid;
  char font[128];
  char color[64];
  EPixel pixel;
  BOOLEAN success;
  double atx, aty, atz;
  
  if ((p = (EAnnText3DWCRec *)make_node(sizeof(EAnnText3DWCRec))) != NULL) {
    fscanf(file_p,
	   "%lf %lf %lf\n",
	   &atx, &aty, &atz);
    fgets(BUF, STRING_BUFF_SIZE-1, file_p);
    BUF[strlen(BUF)-1] = NULL_CHAR;
    DUPL_STRING(p->text, BUF);
    fscanf(file_p, "%d %s",
	   &(p->attributes.spec.have_any),
	   font);
    fscanf(file_p, "%s",
	   color);
    p->at.x = atx; p->at.y = aty; p->at.z = atz;
    pixel = ColorGetPixelFromString(color, &success);
    if (success)
      p->attributes.spec.color = pixel;
    fid = FontGetFontFromString(font, &success);
    if (success)
      p->attributes.spec.font = fid; 
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(EAnnText3DWCRec);
}



static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  EAnnText3DWCRec *p;
  char *color;
  char *font;
  
  p = (EAnnText3DWCRec *)g_p->spec.data;
  if (p->attributes.spec.have_any == YES) {
    font  = FontGetStringFromFont(p->attributes.spec.font);
    if (font == NULL)
      font = "fixed";
    color = ColorGetStringFromPixel(p->attributes.spec.color);
    if (color == NULL)
      color = "black";
  } else {
    font  = "fixed";
    color = "black";
  }
  fprintf(file_p,
	  "%f %f %f\n%s\n%d %s\n%s\n",
	  p->at.x, p->at.y, p->at.z, p->text,
	  p->attributes.spec.have_any,
	  font, color);
}





static void 
ChangeAttributes(GraphicObj *p)
{
  EAnnText3DWCRec *dp;
  unsigned long mask;
  char *text;

  mask = EASValsGetChangeMask();
  dp = (EAnnText3DWCRec *)p->spec.data;
  if ((mask & FONT_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.font = EASValsGetFontId();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) {
    dp->attributes.spec.color = EASValsGetColor();
    dp->attributes.spec.have_any = YES;
  }
  if ((mask & TEXT_STRING_MASK) || (mask & ALL_ATTRIB_MASK)) {
    text = EASValsGetText();
		if(text != NULL){
			if (dp->text != NULL) {
				if (strcmp(dp->text, text) != 0) 
					free(dp->text);
			}
			DUPL_STRING(dp->text, text);
		}
  }
}
			       
static void 
CopyAttributesToActiveSet(GraphicObj *p)
{
  EAnnText3DWCRec *dp;

  dp = (EAnnText3DWCRec *)p->spec.data;
  if (dp->attributes.spec.have_any) {
    EASValsSetFontId(dp->attributes.spec.font);
    EASValsSetColor(dp->attributes.spec.color);
  }
}

static BOOLEAN
AttribValAsInAS(GraphicObj *p, unsigned long mask) 
{
  EAnnText3DWCRec *dp;

  dp = (EAnnText3DWCRec *)p->spec.data;
  
  if ((mask & FONT_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.font != EASValsGetFontId())
      return NO;
  if ((mask & COLOR_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (dp->attributes.spec.color != EASValsGetColor())
      return NO;
  if ((mask & TEXT_STRING_MASK) || (mask & ALL_ATTRIB_MASK)) 
    if (strcmp(dp->text, EASValsGetText()) != 0) 
      return NO;
  return YES;
}
