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


/* This file holds the redefinition of graphic methods for  */
/* the control block graphic. */

#include "Econfig.h"
#include <stdlib.h>

#include "Eview.h"
#include "Egraphic.h"
#include "Egeomops.h"
#include "Ecolors.h"

#define CTLBLOCK_PRIVATE_HEADER
#include "Ectlblock.h"


  
/* ========================================================================= */
/* PRIVATE DATA */
/* ========================================================================= */

#define STRING_BUFF_SIZE 132

static char string_rep_buffer[STRING_BUFF_SIZE];

static GraphicObj *end_of_gg_ctlblock_p = NULL;

/* ========================================================================= */

GraphicObj *
CreateCtlBlock(ECtlblockType block_type)
{
  ECtlBlockRec *p;
  GraphicObj *g_p;

  if (block_type == END_OF_GROUP_CTLBLOCK) { /* use the static private copy */
    if (end_of_gg_ctlblock_p == NULL) {
      p = (ECtlBlockRec *)make_node(sizeof(ECtlBlockRec));
      end_of_gg_ctlblock_p =
	g_p = EGCreateGraphics(&ctlblock_graphic_methods, (caddr_t)p,
			       sizeof(ECtlBlockRec));
      p->block_type = block_type;
    } else
      g_p = end_of_gg_ctlblock_p;
  } else {
    p = (ECtlBlockRec *)make_node(sizeof(ECtlBlockRec));
    g_p = EGCreateGraphics(&ctlblock_graphic_methods, (caddr_t)p,
			   sizeof(ECtlBlockRec));
    p->block_type = block_type;
  }
  return g_p;
}

/* ========================================================================= */

static char *
AsString(GraphicObj *g_p)
{
  ECtlBlockRec *p;

  p = (ECtlBlockRec *)g_p->spec.data;
  sprintf(string_rep_buffer, "CTLBLOCK type %d GID %lu,%lu",
	  p->block_type, 
	  EGGraphicTimeID(g_p), EGGraphicSerialID(g_p));
  
  return string_rep_buffer; /* Don't free this memory!!! */
}

/* ========================================================================= */

#define READ_INT(name)                          \
   {                                            \
     fscanf(file_p, "%d", &int_val);            \
     p->block_data.active_set.name = int_val;   \
   }

#define READ_DBL(name)                          \
   {                                            \
     fscanf(file_p, "%lf", &dbl_val);           \
     p->block_data.active_set.name = dbl_val;   \
   }

static void 
RetrieveFrom(FILE *file_p, GraphicSpecObj *gsp)
{
  ECtlBlockRec *p;
  int int_val, i, ch;
  double dbl_val;
  char str_val[512];
  EPixel pixel;
  BOOLEAN success;
  Font font;
  
  if ((p = (ECtlBlockRec *)make_node(sizeof(ECtlBlockRec))) != NULL) {
    fscanf(file_p, "%d", &int_val);
    p->block_type = int_val;
    switch (p->block_type) {
    case END_OF_GROUP_CTLBLOCK:
      break;
    case ACTIVE_SET_CTLBLOCK:
      READ_INT(change_mask);
      READ_INT(layer);
      fscanf(file_p, "%s", str_val);
      pixel = ColorGetPixelFromString(str_val, &success);
      if (success)
	p->block_data.active_set.curve_color = pixel;
      else
	p->block_data.active_set.curve_color = 0x0;
      READ_INT(curve_style);
      READ_INT(curve_width);
      fscanf(file_p, "%s", str_val);
      font = FontGetFontFromString(str_val, &success);
      if (success)
	p->block_data.active_set.font_id = font;
      else
	p->block_data.active_set.font_id = FontDefaultFont();
      READ_INT(fill_style);
      ch = fgetc(file_p);
      i = 0;
      while ((ch = fgetc(file_p)) != EOF && ch != '\n')
        str_val[i++] = ch;
      str_val[i] = '\0';
      p->block_data.active_set.text = strdup(str_val);
      READ_INT(marker_size);
      READ_INT(marker_type);
      READ_INT(key_point_intervals);
      READ_DBL(shrink);
      READ_DBL(rotation_angle);
      READ_INT(show_polygon);
      READ_INT(show_entity);
      READ_INT(tessel_intervals);
      READ_INT(select_criteria);
      READ_INT(edge_flag);
      fscanf(file_p, "%s", str_val);
      pixel = ColorGetPixelFromString(str_val, &success);
      if (success)
	p->block_data.active_set.edge_color = pixel;
      else
	p->block_data.active_set.edge_color = 0x0;
      READ_INT(not_clippable_flag);
      READ_INT(invisible_when_clipped_flag);
      break;
    }
  }
  gsp->data        = (caddr_t)p;
  gsp->data_length = sizeof(ECtlBlockRec);
}
 
/* ========================================================================= */

#define SAVE_INT(name)                          \
   {                                            \
     int_val = p->block_data.active_set.name;   \
     fprintf(file_p, "%d\n", int_val);          \
   }

#define SAVE_DBL(name)                          \
   {                                            \
     dbl_val = p->block_data.active_set.name;   \
     fprintf(file_p, "%f\n", dbl_val);          \
   }

static void 
StoreOn(FILE *file_p, GraphicObj *g_p)
{
  ECtlBlockRec *p;
  int int_val;
  double dbl_val;

  p = (ECtlBlockRec *)g_p->spec.data;

  int_val = p->block_type;
  fprintf(file_p, "%d\n", int_val);
  
  switch (p->block_type) {
  case END_OF_GROUP_CTLBLOCK:
    break;
  case ACTIVE_SET_CTLBLOCK:
    SAVE_INT(change_mask)
    SAVE_INT(layer);
    fprintf(file_p, "%s\n",
	    ColorGetStringFromPixel(p->block_data.active_set.curve_color));
    SAVE_INT(curve_style);
    SAVE_INT(curve_width);
    fprintf(file_p, "%s\n",
	    FontGetStringFromFont(p->block_data.active_set.font_id));
    SAVE_INT(fill_style);
    fprintf(file_p, "%s\n", p->block_data.active_set.text);
    SAVE_INT(marker_size);
    SAVE_INT(marker_type);
    SAVE_INT(key_point_intervals);
    SAVE_DBL(shrink);
    SAVE_DBL(rotation_angle);
    SAVE_INT(show_polygon);
    SAVE_INT(show_entity);
    SAVE_INT(tessel_intervals);
    SAVE_INT(select_criteria);
    SAVE_INT(edge_flag);
    fprintf(file_p, "%s\n",
	    ColorGetStringFromPixel(p->block_data.active_set.edge_color));
    SAVE_INT(not_clippable_flag);
    SAVE_INT(invisible_when_clipped_flag);
    break;
  }
}

/* ========================================================================= */

static EGraphicType 
Type(GraphicObj *g_p)
{
  return EG_CTLBLOCK;
}

/* ========================================================================= */

ECtlblockType 
CtlBlockType(GraphicObj *g_p)
{
  return ((ECtlBlockRec *)g_p->spec.data)->block_type;
}
