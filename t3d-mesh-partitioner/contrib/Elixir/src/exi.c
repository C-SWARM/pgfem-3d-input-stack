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

#include "Esimple.h"

static int
boot_exi(void);


typedef int (*EXI_write_OOGL_object_func)(GraphicObj *gp);

typedef int (*EXI_read_OOGL_object_func)(LIST_OF(GraphicObj *) gps);

typedef struct EXI_OOGL_Methods {
  EXI_write_OOGL_object_func  write;
  EXI_read_OOGL_object_func   read;
}              EXI_OOGL_Methods;


/* Private data */
static int first = YES;
static EXI_OOGL_Methods *methods;
static FILE *in_stream;
static FILE *out_stream;
static EModel *curr_model;
static double x, y, z;
#define DIM 56
static  WCRec pts[DIM];
static FPNum vals[DIM];
static  int np;
static char geomid[64];
static EGraphicId gtimeid = 0, gserid = 0, substid = 0;

static double r, g, b, a = 1;


#define GET_COLOR(GP, R, G, B)                          \
  {                                                     \
    EPixel aColor;                                      \
    EGCopyAttributesToActiveSet(GP);                    \
    aColor = EASValsGetColor();                         \
    ColorPixelToRGBStandard(aColor, &R, &G, &B);        \
  }

#define COORDS(ith) { x = pts[ith].x; y = pts[ith].y; z = pts[ith].z; }

#define OS out_stream
#define IS in_stream

#define SET_UP_OS(stream) { if (first) boot_exi(); out_stream = stream; }
#define SET_UP_IS(stream) { if (first) boot_exi(); in_stream = stream; }
#define SET_UP_MODEL(model) { if (first) boot_exi(); curr_model = model; }


#define NL() fprintf(OS, "\n")

#define START_BRACE() fprintf(OS, "{\n")
#define CLOSE_BRACE() fprintf(OS, "}\n")

#define START_GEOM(GP)                                      \
   { GEN_GEOM_ID(GP); fprintf(OS, "(geometry %s ", geomid); }
#define GEN_GEOM_ID(GP)                                                 \
  {                                                                     \
    if (GP == NULL) {                                                   \
      sprintf(geomid, "obj-%lu", ++substid);                            \
    } else {                                                            \
      gtimeid = EGGraphicTimeID(GP), gserid = EGGraphicSerialID(GP);    \
      sprintf(geomid, "GID-%lu-%lu", gtimeid, gserid);                  \
    }                                                                   \
  }
#define END_GEOM()     fprintf(OS, ")\n")
#define NORMALIZATION_NONE() fprintf(OS, "(normalization %s none)\n", geomid)

#define START_LIST()  { START_GEOM(NULL);  fprintf(OS, " { LIST \n"); }
#define END_LIST()    { fprintf(OS, "} # end list\n"); END_GEOM(); }

#define START_PROGN()  fprintf(OS, "\n(progn\n")
#define CLOSE_PROGN()  fprintf(OS, ") # end progn\n")

          

static int 
write_quad(GraphicObj *gp);
static int 
write_quad_wd(GraphicObj *gp);
static int 
write_hex(GraphicObj *gp);
static int 
write_hex_wd(GraphicObj *gp);
static int 
write_tri_wd(GraphicObj *gp);
static int 
write_tri(GraphicObj *gp);


int
EXIWriteOOGLStartPROGN(FILE *stream)
{
  SET_UP_OS(stream);
  START_PROGN();
	return(1);
}


int
EXIWriteOOGLClosePROGN(FILE *stream)
{
  SET_UP_OS(stream);
  CLOSE_PROGN();
	return(1);
}


int
EXIWriteOOGLStartList(FILE *stream)
{
  SET_UP_OS(stream);
  START_LIST();
	return(1);
}

int
EXIWriteOOGLEndList(FILE *stream)
{
  SET_UP_OS(stream);
  END_LIST();
	return(1);
}


int
EXIWriteOOGLGeom(FILE *stream, EModel *model, GraphicObj *gp)
{
  SET_UP_OS(stream);
  SET_UP_MODEL(model);
  START_GEOM(gp);
     EXIWriteOOGLObject(stream, model, gp);
  END_GEOM();
  NORMALIZATION_NONE();
	return(1);
}


int
EXIWriteOOGLObject(FILE *stream, EModel *model, GraphicObj *gp)
{
  EGraphicType type;

  SET_UP_OS(stream);
  SET_UP_MODEL(model);
  
  if (!gp)
    return YES;
  
  type       = EGGraphicType(gp);
  if (methods[type].write != NULL)
    return (*methods[type].write)(gp);
  else
    return YES;
}


static int
boot_exi(void)
{
#define SET_ENTRY(which, READ, WRITE)           \
  { methods[which].write = WRITE;               \
    methods[which].read  = READ;                \
  }

  methods = make_node(sizeof(EXI_OOGL_Methods) * EG_MAX_METHOD_TABLE);
  if (!methods)
    EUFailedMakeNode("boot_exi");

  SET_ENTRY(EG_UNKNOWN_TYPE, NULL, NULL);
  SET_ENTRY(EG_LINE3D, NULL, NULL);
  SET_ENTRY(EG_RECTANGLE_VC, NULL, NULL);
  SET_ENTRY(EG_MARKER3D, NULL, NULL);
  SET_ENTRY(EG_ANNTEXT3D, NULL, NULL);
  SET_ENTRY(EG_GGROUP, NULL, NULL);
  SET_ENTRY(EG_CTLBLOCK, NULL, NULL);
  SET_ENTRY(EG_TRIANGLE3D, NULL, write_tri);
  SET_ENTRY(EG_CBEZCURVE3D, NULL, NULL);
  SET_ENTRY(EG_BCBEZSURFACE3D, NULL, NULL);
  SET_ENTRY(EG_TRIANGLEWD3D, NULL, write_tri_wd);
  SET_ENTRY(EG_QUADWD3D, NULL, write_quad_wd);
  SET_ENTRY(EG_TETRAHEDRON, NULL, NULL);
  SET_ENTRY(EG_RIGHT_ANGLE_PRISM, NULL, NULL);
  SET_ENTRY(EG_BCB_HEXAHEDRON, NULL, NULL);
  SET_ENTRY(EG_QUAD3D, NULL, write_quad);
  SET_ENTRY(EG_PYRAMID, NULL, NULL);
  SET_ENTRY(EG_HEXAHEDRON, NULL, write_hex);
  SET_ENTRY(EG_VECMARKER3D, NULL, NULL);
  SET_ENTRY(EG_VECTOR3D, NULL, NULL);
  SET_ENTRY(EG_RBEZCURVE3D, NULL, NULL);
  SET_ENTRY(EG_RBEZSURFACE3D, NULL, NULL);
  SET_ENTRY(EG_HEXAHEDRONWD, NULL, write_hex_wd);
  first = NO;
	return(1);
}




static int 
write_quad(GraphicObj *gp)
{
  int i;
  
  if (!EGGetGraphicsGeometry(gp, pts, &np))
    return NO;
  
  START_BRACE();
  GET_COLOR(gp, r, g, b);
  fprintf(OS, "{ appearance { material { diffuse %f %f %f } } }\n", r, g, b);
  fprintf(OS, "QUAD\n");
  for (i = 0; i < 4; i++) {
    COORDS(i);
    fprintf(OS, "%f %f %f   ", x, y, z);
  }
  NL();
  CLOSE_BRACE();
  return YES;
}



static int 
write_quad_wd(GraphicObj *gp)
{
int i;
  EPixel color;
  
  if (!EGGetGraphicsGeometry(gp, pts, &np))
    return NO;
  if (!EGGetAssocData(gp, vals, &np))
    return NO;
  
  START_BRACE();
  fprintf(OS, "CQUAD\n");
  for (i = 0; i < 4; i++) {
    COORDS(i);
    color = ColorFringeValueToColorHSV(EMGetAssocFringeTable(curr_model),
                                       vals[i]);
    ColorPixelToRGBStandard(color, &r, &g, &b);
    fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  }
  NL();
  CLOSE_BRACE();
  return YES;
}


static int 
write_tri(GraphicObj *gp)
{
  int i;
  
  if (!EGGetGraphicsGeometry(gp, pts, &np))
    return NO;
  
  START_BRACE();
  GET_COLOR(gp, r, g, b);
  fprintf(OS, "{ appearance { material { diffuse %f %f %f } } }\n", r, g, b);
  fprintf(OS, "OFF\n");
  fprintf(OS, "3 1 3\n");
  for (i = 0; i < 3; i++) {
    COORDS(i);
    fprintf(OS, "%f %f %f\n", x, y, z);
  }
  fprintf(OS, "3 0 1 2\n");
  CLOSE_BRACE();
  return YES;
}


static int 
write_tri_wd(GraphicObj *gp)
{
  int i;
  EPixel color;
  
  if (!EGGetGraphicsGeometry(gp, pts, &np))
    return NO;
  if (!EGGetAssocData(gp, vals, &np))
    return NO;
  
  START_BRACE();
  fprintf(OS, "COFF\n");
  fprintf(OS, "3 1 3\n");
  for (i = 0; i < 3; i++) {
    COORDS(i);
    color = ColorFringeValueToColorHSV(EMGetAssocFringeTable(curr_model),
                                       vals[i]);
    ColorPixelToRGBStandard(color, &r, &g, &b);
    fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  }
  fprintf(OS, "3 0 1 2\n");
  CLOSE_BRACE();
  return YES;
}



static int 
write_hex(GraphicObj *gp)
{
  if (!EGGetGraphicsGeometry(gp, pts, &np))
    return NO;
  
  START_BRACE();
  GET_COLOR(gp, r, g, b);
  fprintf(OS, "{ appearance { material { diffuse %f %f %f } } }\n", r, g, b);
  fprintf(OS, "QUAD\n");

  COORDS(0); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(3); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(2); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(1); fprintf(OS, "%f %f %f\n", x, y, z);

  COORDS(0); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(1); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(5); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(4); fprintf(OS, "%f %f %f\n", x, y, z);

  COORDS(1); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(2); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(6); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(5); fprintf(OS, "%f %f %f\n", x, y, z);

  COORDS(2); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(3); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(7); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(6); fprintf(OS, "%f %f %f\n", x, y, z);

  COORDS(3); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(0); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(4); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(7); fprintf(OS, "%f %f %f\n", x, y, z);

  COORDS(4); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(5); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(6); fprintf(OS, "%f %f %f\n", x, y, z);
  COORDS(7); fprintf(OS, "%f %f %f\n", x, y, z);

  NL();
  CLOSE_BRACE();
  return YES;
}



static int 
write_hex_wd(GraphicObj *gp)
{
  EPixel color;
#define GCOLOR(I)                                                         \
  {                                                                       \
    color = ColorFringeValueToColorHSV(EMGetAssocFringeTable(curr_model), \
                                       vals[I]);                          \
    ColorPixelToRGBStandard(color, &r, &g, &b);                           \
  }
#define SETUP(I) { COORDS(I); GCOLOR(I);  }
  
  if (!EGGetGraphicsGeometry(gp, pts, &np))
    return NO;
  if (!EGGetAssocData(gp, vals, &np))
    return NO;
  
  START_BRACE();
  fprintf(OS, "CQUAD\n");
  
  SETUP(0); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(3); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(2); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(1); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);

  SETUP(0); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(1); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(5); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(4); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);

  SETUP(1); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(2); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(6); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(5); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);

  SETUP(2); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(3); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(7); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(6); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);

  SETUP(3); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(0); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(4); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(7); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);

  SETUP(4); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(5); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(6); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);
  SETUP(7); fprintf(OS, "%f %f %f  %f %f %f %f\n", x, y, z, r, g, b, a);

  NL();
  CLOSE_BRACE();
  return YES;
}
