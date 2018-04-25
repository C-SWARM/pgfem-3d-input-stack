
#include "Esimple.h"
#include "Egecmds.h"
#include "Ege.h"
#include "Emove.h"



 
BOOLEAN 
AGEExecDeleteCmd(ECommand *c_p)
{
  GraphicObj *g_p;

  if ((LIST)c_p->data == (LIST)0)
    return YES;
  
  g_p = (GraphicObj *)get_list_next((LIST)c_p->data, NULL);
  while (g_p != NULL) {
    EMDeleteGraphics(ESIModel(), g_p);
    g_p = (GraphicObj *)get_list_next((LIST)c_p->data, g_p);
  }
  return YES;
}


 
BOOLEAN 
AGEUndoDeleteCmd(ECommand *c_p)
{
  GraphicObj *g_p;

  if ((LIST)c_p->data == (LIST)0)
    return YES;
  
  g_p = (GraphicObj *)get_list_next((LIST)c_p->data, NULL);
  while (g_p != NULL) {
    EMAddGraphicsToModel(ESIModel(), g_p);
    g_p = (GraphicObj *)get_list_next((LIST)c_p->data, g_p);
  }
  return YES;
}



BOOLEAN 
AGEExecFastDeleteCmd(ECommand *c_p)
{
  GraphicObj *g_p;

  if ((LIST)c_p->data == (LIST)0)
    return YES;
  
  g_p = (GraphicObj *)get_list_next((LIST)c_p->data, NULL);
  while (g_p != NULL) {
		EGSetMarked(g_p, YES);
    g_p = (GraphicObj *)get_list_next((LIST)c_p->data, g_p);
  }
	EMUnlinkMarkedGraphics(ESIModel());
	EMDeleteMarkedGraphics(ESIModel());
  return YES;
}



BOOLEAN 
AGEUndoFastDeleteCmd(ECommand *c_p)
{
  GraphicObj *g_p;

  if ((LIST)c_p->data == (LIST)0)
    return YES;
  
  g_p = (GraphicObj *)get_list_next((LIST)c_p->data, NULL);
  while (g_p != NULL) {
    EMAddGraphicsToModel(ESIModel(), g_p);
    g_p = (GraphicObj *)get_list_next((LIST)c_p->data, g_p);
  }
  return YES;
}


 
 
BOOLEAN 
AGEExecCreateCmd(ECommand *c_p)
{
  EMAddGraphicsToModel(ESIModel(), (GraphicObj *)c_p->data);
  return YES;
}


 
BOOLEAN 
AGEUndoCreateCmd(ECommand *c_p)
{
  EMDeleteGraphics(ESIModel(), (GraphicObj *)c_p->data);
  return YES;
}
  


ECommand *
AGECreateDeleteCommand(LIST to_delete)
{
  return CmdCreateCommand(AGEExecDeleteCmd,
			  AGEUndoDeleteCmd,
			  (caddr_t)copy_list(to_delete, NOT_NODES));
}
  


ECommand *
AGECreateFastDeleteCommand(LIST to_delete)
{
  return CmdCreateCommand(AGEExecFastDeleteCmd,
			  AGEUndoFastDeleteCmd,
			  (caddr_t)copy_list(to_delete, NOT_NODES));
}
  


ECommand *
AGECreateCreateCommand(GraphicObj *g_p)
{
  return CmdCreateCommand(AGEExecCreateCmd,
			  AGEUndoCreateCmd,
			  (caddr_t)g_p);
}
      


ECommand *
AGECreateReshapeCommand(GraphicObj *g_p, EHandleNum handle, WCRec *newp)
{
  ReshapeData *rd_p;

  if ((rd_p = (ReshapeData *)make_node(sizeof(ReshapeData))) == NULL) {
    EUFailedMakeNode("AGECreateReshapeCommand");
  }
  rd_p->g_p          = g_p;
  rd_p->handle       = handle;
  EGXYofGraphicsHandle(g_p, handle, &(rd_p->old_coords));
  rd_p->new_coords.x = newp->x;
  rd_p->new_coords.y = newp->y;
  rd_p->new_coords.z = newp->z;
  return CmdCreateCommand(AGEExecReshapeCmd,
			  AGEUndoReshapeCmd,
			  (caddr_t)rd_p);
}


 
BOOLEAN 
AGEExecReshapeCmd(ECommand *c_p)
{
  ReshapeData *rd_p;

  rd_p = (ReshapeData *)c_p->data;
  EMEraseGraphics(ESIModel(), (GraphicObj *)rd_p->g_p);
  EGModifyGraphicsByHandle((GraphicObj *)rd_p->g_p, rd_p->handle,
			  &(rd_p->new_coords));
  EMDrawGraphics(ESIModel(), (GraphicObj *)rd_p->g_p);
  return YES;
}


 
BOOLEAN 
AGEUndoReshapeCmd(ECommand *c_p)
{
  ReshapeData *rd_p;

  rd_p = (ReshapeData *)c_p->data;
  EMEraseGraphics(ESIModel(), (GraphicObj *)rd_p->g_p);
  EGModifyGraphicsByHandle((GraphicObj *)rd_p->g_p, rd_p->handle,
			  &(rd_p->old_coords));
  EMDrawGraphics(ESIModel(), (GraphicObj *)rd_p->g_p);
  return YES;
}
    


ECommand *
AGECreateTranslateCommand(LIST g_list, MoveMode mode, WCRec *by)
{
  TranslateData *d_p;

  if ((d_p = (TranslateData *)make_node(sizeof(TranslateData))) == NULL) {
    EUFailedMakeNode("AGECreateTranslateCommand"); 
  }
  d_p->g_list     = copy_list(g_list, NOT_NODES);
  d_p->mode       = mode;
  d_p->delta.x    = by->x;
  d_p->delta.y    = by->y;
  d_p->delta.z    = by->z;
  d_p->new_g_list = NULL;
  return CmdCreateCommand(AGEExecTranslateCmd,
			  AGEUndoTranslateCmd,
			  (caddr_t)d_p);
}
    


ECommand *
AGECreateRotateCommand(LIST g_list, MoveMode mode, WCRec *center, WCRec *axial)
{
  RotateData *d_p;

  if ((d_p = (RotateData *)make_node(sizeof(RotateData))) == NULL) {
    EUFailedMakeNode("AGECreateRotateCommand");
  }
  d_p->g_list     = copy_list(g_list, NOT_NODES);
  d_p->mode       = mode;
  d_p->center.x   = center->x;
  d_p->center.y   = center->y;
  d_p->center.z   = center->z;
  d_p->axial.x    = axial->x;
  d_p->axial.y    = axial->y;
  d_p->axial.z    = axial->z;
  d_p->new_g_list = NULL;
  return CmdCreateCommand(AGEExecRotateCmd,
			  AGEUndoRotateCmd,
			  (caddr_t)d_p);
}
 
    


ECommand *
AGECreateMirrorCenterCommand(LIST g_list, MoveMode mode, WCRec *center)
{
  MirrorData *d_p;

  if ((d_p = (MirrorData *)make_node(sizeof(MirrorData))) == NULL) {
    EUFailedMakeNode("AGECreateMirrorCenterCommand");
  }
  d_p->g_list     = copy_list(g_list, NOT_NODES);
  d_p->mode       = mode;
  d_p->center.x   = center->x;
  d_p->center.y   = center->y;
  d_p->center.z   = center->z;
  d_p->normal.x   = 0;
  d_p->normal.y   = 0;
  d_p->normal.z   = 0;
  d_p->new_g_list = NULL;
  return CmdCreateCommand(AGEExecMirrorCenterCmd,
			  AGEUndoMirrorCenterCmd,
			  (caddr_t)d_p);
}
 

 
BOOLEAN 
AGEExecTranslateCmd(ECommand *c_p)
{
  TranslateData      *d_p;
  GraphicObj        *g_p;
  GraphicObj    *new_g_p;

  d_p = (TranslateData *)c_p->data;

  if (d_p->mode == MOVE_COPY) {
    if (d_p->new_g_list == NULL)
      d_p->new_g_list = make_list();
    else
      d_p->new_g_list = flush_list(d_p->new_g_list, NOT_NODES);
  }
  g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
  while (g_p != NULL) {
    if (d_p->mode == MOVE_ORIGINAL) 
      EMEraseGraphics(ESIModel(), g_p);
    if (d_p->mode == MOVE_COPY) 
      add_to_tail(d_p->new_g_list, (new_g_p = EGDeepCopyGraphics(g_p)));
    else
      new_g_p = g_p;
    EGTranslateGraphic(new_g_p, &(d_p->delta));
    if (d_p->mode == MOVE_COPY) 
      EMAddGraphicsToModel(ESIModel(), new_g_p);
    else
      EMDrawGraphics(ESIModel(), new_g_p);
    g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
  }
  return YES;
}
 

 
BOOLEAN 
AGEUndoTranslateCmd(ECommand *c_p)
{
  TranslateData *d_p;
  GraphicObj *g_p;
  WCRec p;

  d_p = (TranslateData *)c_p->data;

  p.x = -d_p->delta.x, p.y = -d_p->delta.x, p.z = -d_p->delta.x;
  if (d_p->mode == MOVE_ORIGINAL) {
    g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
    while (g_p != NULL) {
      EMEraseGraphics(ESIModel(), g_p);
      EGTranslateGraphic(g_p, &p);
      EMDrawGraphics(ESIModel(), g_p);
      g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
    }
  } else {
    g_p = (GraphicObj *)get_list_next(d_p->new_g_list, NULL);
    while (g_p != NULL) {
      EMDeleteGraphics(ESIModel(), g_p);
      EGDeepDestroyGraphics(g_p);
      g_p = (GraphicObj *)get_list_next(d_p->new_g_list, g_p);
    }
    flush_list(d_p->new_g_list, NOT_NODES);
  } 
  return YES;
}


 
BOOLEAN 
AGEExecRotateCmd(ECommand *c_p)
{
  RotateData      *d_p;
  GraphicObj        *g_p;
  GraphicObj    *new_g_p;

  d_p = (RotateData *)c_p->data;

  if (d_p->mode == MOVE_COPY) {
    if (d_p->new_g_list == NULL)
      d_p->new_g_list = make_list();
    else
      d_p->new_g_list = flush_list(d_p->new_g_list, NOT_NODES);
  }
  g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
  while (g_p != NULL) {
    if (d_p->mode == MOVE_ORIGINAL) 
      EMEraseGraphics(ESIModel(), g_p);
    if (d_p->mode == MOVE_COPY) 
      add_to_tail(d_p->new_g_list, (new_g_p = EGDeepCopyGraphics(g_p)));
    else
      new_g_p = g_p;
    EGRotateGraphic(new_g_p, &(d_p->center), &(d_p->axial));
    if (d_p->mode == MOVE_COPY) 
      EMAddGraphicsToModel(ESIModel(), new_g_p);
    else
      EMDrawGraphics(ESIModel(), new_g_p);
    g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
  }
  return YES;
}
 

 
BOOLEAN 
AGEUndoRotateCmd(ECommand *c_p)
{
  RotateData *d_p;
  GraphicObj *g_p;
  WCRec negat_axial;

  d_p = (RotateData *)c_p->data;

  negat_axial.x = -d_p->axial.x;
  negat_axial.y = -d_p->axial.y;
  negat_axial.z = -d_p->axial.z;
  if (d_p->mode == MOVE_ORIGINAL) {
    g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
    while (g_p != NULL) {
      EMEraseGraphics(ESIModel(), g_p);
      EGRotateGraphic(g_p, &(d_p->center), &negat_axial);
      EMDrawGraphics(ESIModel(), g_p);
      g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
    }
  } else {
    g_p = (GraphicObj *)get_list_next(d_p->new_g_list, NULL);
    while (g_p != NULL) {
      EMDeleteGraphics(ESIModel(), g_p);
      EGDeepDestroyGraphics(g_p);
      g_p = (GraphicObj *)get_list_next(d_p->new_g_list, g_p);
    }
    flush_list(d_p->new_g_list, NOT_NODES);
  } 
  return YES;
}


  
void 
AGECreateLine3D(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateLine3D(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}


  
void 
AGECreateTriangle3D(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateTriangle3D(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}
  

void 
AGECreateTetra(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateTetra(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}


void 
AGECreateHexahedron(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateHexahedron(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}

  
void 
AGECreateQuad3D(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateQuad3D(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}



void 
AGECreateMarker3D(WCRec *p)
{
  GraphicObj *g_p;

  g_p = CreateMarker3D(p);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p); 
  CmdFlipState(AGECreateCreateCommand(g_p));
}


  
void 
AGECreateAnnText3D(WCRec *at)
{
  GraphicObj *g_p;

  g_p = CreateAnnText3D(at, EASValsGetText());
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p); 
  CmdFlipState(AGECreateCreateCommand(g_p));
}


  
void 
AGECreateCBezC3D(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateCBezC3D(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p); 
  CmdFlipState(AGECreateCreateCommand(g_p));
}


  
void 
AGECreateBCBezS3D(WCRec *points)
{
  GraphicObj *g_p;
  WCRec p[4][4];
  int i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++) {
      p[i][j].x = points[i+j*4].x; 
      p[i][j].y = points[i+j*4].y;
      p[i][j].z = points[i+j*4].z;
    }
  g_p = CreateBCBezS3D((WCRec *)p);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}
 

void 
AGECreateBCBHex(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateBCBHex((WCRec *)points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}


 
BOOLEAN 
AGEExecMirrorCenterCmd(ECommand *c_p)
{
  MirrorData      *d_p;
  GraphicObj        *g_p;
  GraphicObj    *new_g_p;

  d_p = (MirrorData *)c_p->data;

  if (d_p->mode == MOVE_COPY) {
    if (d_p->new_g_list == NULL)
      d_p->new_g_list = make_list();
    else
      d_p->new_g_list = flush_list(d_p->new_g_list, NOT_NODES);
  }
  g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
  while (g_p != NULL) {
    if (d_p->mode == MOVE_ORIGINAL) 
      EMEraseGraphics(ESIModel(), g_p);
    if (d_p->mode == MOVE_COPY) 
      add_to_tail(d_p->new_g_list, (new_g_p = EGDeepCopyGraphics(g_p)));
    else
      new_g_p = g_p;
    EGMirrorCenterGraphic(new_g_p, &(d_p->center));
    if (d_p->mode == MOVE_COPY) 
      EMAddGraphicsToModel(ESIModel(), new_g_p);
    else
      EMDrawGraphics(ESIModel(), new_g_p);
    g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
  }
  return YES;
}
 

 
BOOLEAN 
AGEUndoMirrorCenterCmd(ECommand *c_p)
{
  MirrorData *d_p;
  GraphicObj *g_p;

  d_p = (MirrorData *)c_p->data;

  if (d_p->mode == MOVE_ORIGINAL) {
    g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
    while (g_p != NULL) {
      EMEraseGraphics(ESIModel(), g_p);
      EGMirrorCenterGraphic(g_p, &(d_p->center));
      EMDrawGraphics(ESIModel(), g_p);
      g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
    }
  } else {
    g_p = (GraphicObj *)get_list_next(d_p->new_g_list, NULL);
    while (g_p != NULL) {
      EMDeleteGraphics(ESIModel(), g_p);
      EGDeepDestroyGraphics(g_p);
      g_p = (GraphicObj *)get_list_next(d_p->new_g_list, g_p);
    }
    flush_list(d_p->new_g_list, NOT_NODES);
  } 
  return YES;
}



ECommand *
AGECreateMirrorPlaneCommand(LIST g_list, MoveMode mode, WCRec pnts_on_plane[3])
{
  MirrorData *d_p;
  WCRec v1, v2;

  if ((d_p = (MirrorData *)make_node(sizeof(MirrorData))) == NULL) {
    EUFailedMakeNode("AGECreateMirrorPlaneCommand");
  }
  d_p->g_list     = copy_list(g_list, NOT_NODES);
  d_p->mode       = mode;
  d_p->center.x   = pnts_on_plane[0].x;
  d_p->center.y   = pnts_on_plane[0].y;
  d_p->center.z   = pnts_on_plane[0].z;
  v1.x = pnts_on_plane[1].x - pnts_on_plane[0].x;
  v1.y = pnts_on_plane[1].y - pnts_on_plane[0].y;
  v1.z = pnts_on_plane[1].z - pnts_on_plane[0].z;
  v2.x = pnts_on_plane[2].x - pnts_on_plane[0].x;
  v2.y = pnts_on_plane[2].y - pnts_on_plane[0].y;
  v2.z = pnts_on_plane[2].z - pnts_on_plane[0].z;
  CrossProd3(&d_p->normal, &v1, &v2);
  if (!NormalizeVect3(&d_p->normal)) {
    ERptErrMessage(1, 1, "Normal is invalid", ERROR_GRADE);
    d_p->normal.x   = 0;
    d_p->normal.y   = 0;
    d_p->normal.z   = 1;
  }
  d_p->new_g_list = NULL;
  return CmdCreateCommand(AGEExecMirrorPlaneCmd,
			  AGEUndoMirrorPlaneCmd,
			  (caddr_t)d_p);
}
 


 
BOOLEAN 
AGEExecMirrorPlaneCmd(ECommand *c_p)
{
  MirrorData      *d_p; 
  GraphicObj        *g_p;
  GraphicObj    *new_g_p;

  d_p = (MirrorData *)c_p->data;

  if (d_p->mode == MOVE_COPY) {
    if (d_p->new_g_list == NULL)
      d_p->new_g_list = make_list();
    else
      d_p->new_g_list = flush_list(d_p->new_g_list, NOT_NODES);
  }
  g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
  while (g_p != NULL) {
    if (d_p->mode == MOVE_ORIGINAL) 
      EMEraseGraphics(ESIModel(), g_p);
    if (d_p->mode == MOVE_COPY) 
      add_to_tail(d_p->new_g_list, (new_g_p = EGDeepCopyGraphics(g_p)));
    else
      new_g_p = g_p;
    EGMirrorPlaneGraphic(new_g_p, &(d_p->center), &(d_p->normal));
    if (d_p->mode == MOVE_COPY) 
      EMAddGraphicsToModel(ESIModel(), new_g_p);
    else
      EMDrawGraphics(ESIModel(), new_g_p);
    g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
  }
  return YES;
}
 

 
BOOLEAN 
AGEUndoMirrorPlaneCmd(ECommand *c_p)
{
  MirrorData *d_p;
  GraphicObj *g_p;

  d_p = (MirrorData *)c_p->data;

  if (d_p->mode == MOVE_ORIGINAL) {
    g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
    while (g_p != NULL) {
      EMEraseGraphics(ESIModel(), g_p);
      EGMirrorPlaneGraphic(g_p, &(d_p->center), &(d_p->normal));
      EMDrawGraphics(ESIModel(), g_p);
      g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
    }
  } else {
    g_p = (GraphicObj *)get_list_next(d_p->new_g_list, NULL);
    while (g_p != NULL) {
      EMDeleteGraphics(ESIModel(), g_p);
      EGDeepDestroyGraphics(g_p);
      g_p = (GraphicObj *)get_list_next(d_p->new_g_list, g_p);
    }
    flush_list(d_p->new_g_list, NOT_NODES);
  } 
  return YES;
}



  
void 
AGECreateRAPrism(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreateRAPrism(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}

  
void 
AGECreatePyramid(WCRec *points)
{
  GraphicObj *g_p;

  g_p = CreatePyramid(points);
  EGWithMaskChangeAttributes(ALL_ATTRIB_MASK, g_p);
  CmdFlipState(AGECreateCreateCommand(g_p));
}




ECommand *
AGECreateScaleCommand(LIST g_list, MoveMode mode, WCRec *center)
{
  ScaleData *d_p;

  if ((d_p = (ScaleData *)make_node(sizeof(ScaleData))) == NULL) {
    EUFailedMakeNode("AGECreateScaleCommand"); 
  }
  d_p->g_list     = copy_list(g_list, NOT_NODES);
  d_p->mode       = mode;
  d_p->center.x   = center->x;
  d_p->center.y   = center->y;
  d_p->center.z   = center->z;
  d_p->sx         = EASValsGetScaleX();
  d_p->sy         = EASValsGetScaleY();
  d_p->sz         = EASValsGetScaleZ();
  d_p->new_g_list = NULL;
  return CmdCreateCommand(AGEExecScaleCmd,
			  AGEUndoScaleCmd,
			  (caddr_t)d_p);
}
    
BOOLEAN 
AGEExecScaleCmd(ECommand *c_p)
{
  ScaleData      *d_p;
  GraphicObj        *g_p;
  GraphicObj    *new_g_p;

  d_p = (ScaleData *)c_p->data;

  if (d_p->mode == MOVE_COPY) {
    if (d_p->new_g_list == NULL)
      d_p->new_g_list = make_list();
    else
      d_p->new_g_list = flush_list(d_p->new_g_list, NOT_NODES);
  }
  g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
  while (g_p != NULL) {
    if (d_p->mode == MOVE_ORIGINAL) 
      EMEraseGraphics(ESIModel(), g_p);
    if (d_p->mode == MOVE_COPY) 
      add_to_tail(d_p->new_g_list, (new_g_p = EGDeepCopyGraphics(g_p)));
    else
      new_g_p = g_p;
    EGScaleGraphic(new_g_p, &(d_p->center), d_p->sx, d_p->sy, d_p->sz);
    if (d_p->mode == MOVE_COPY) 
      EMAddGraphicsToModel(ESIModel(), new_g_p);
    else
      EMDrawGraphics(ESIModel(), new_g_p);
    g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
  }
  return YES;
}
 

 
BOOLEAN 
AGEUndoScaleCmd(ECommand *c_p)
{
  ScaleData *d_p;
  GraphicObj *g_p;

  d_p = (ScaleData *)c_p->data;

  if (d_p->mode == MOVE_ORIGINAL) {
    g_p = (GraphicObj *)get_list_next(d_p->g_list, NULL);
    while (g_p != NULL) {
      EMEraseGraphics(ESIModel(), g_p);
      EGScaleGraphic(g_p, &(d_p->center), 1/d_p->sx, 1/d_p->sy, 1/d_p->sz);
      EMDrawGraphics(ESIModel(), g_p);
      g_p = (GraphicObj *)get_list_next(d_p->g_list, g_p);
    }
  } else {
    g_p = (GraphicObj *)get_list_next(d_p->new_g_list, NULL);
    while (g_p != NULL) {
      EMDeleteGraphics(ESIModel(), g_p);
      EGDeepDestroyGraphics(g_p);
      g_p = (GraphicObj *)get_list_next(d_p->new_g_list, g_p);
    }
    flush_list(d_p->new_g_list, NOT_NODES);
  } 
  return YES;
}
