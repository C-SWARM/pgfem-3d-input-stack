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

#define SIMPLE_PRIVATE_HEADER 
#include "Esimple.h"          

static void 
uninstall_apply_to_view(EView *v_p, caddr_t data);

static void 
uninstall_apply_to_view(EView *v_p, caddr_t data)
{
  EMUninstallHandler(EVGetModel(v_p));
}


static void
apply_viewclip_CB(Widget w, XtPointer ptr, XtPointer call_data);

static Widget do_viewclip, viewclip_form;

void
setup_the_viewclip_palette(Widget pal)
{
  Widget label1, bg, fg, bg_depth, fg_depth, apply;
  int ac;
  Arg al[7];
  
  do_viewclip = ESIAddButton("do_viewclip", "Do view clip", 
                             toggleWidgetClass, pal, NULL, 0, NULL, NULL);
  
  viewclip_form = XtCreateManagedWidget("viewclip_form",
                                        formWidgetClass, pal, NULL, 0);
  ac = 0;
  XtSetArg(al[ac], XtNlabel, "Depth of view clip planes"); ac++;
  label1 = XtCreateManagedWidget("label1", labelWidgetClass,
                                 viewclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNlabel, "Background"); ac++;
  bg = XtCreateManagedWidget("bg", labelWidgetClass, viewclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNfromHoriz, bg); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  XtSetArg(al[ac], XtNtranslations,
           XtParseTranslationTable("#override <Key>Return: ")); ac++;
  bg_depth = ESIAddButton("bg_depth", "", 
                         asciiTextWidgetClass, viewclip_form,
                          al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNfromHoriz, bg_depth); ac++;
  XtSetArg(al[ac], XtNlabel, "Foreground"); ac++;
  fg = XtCreateManagedWidget("fg", labelWidgetClass, viewclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNfromHoriz, fg); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  XtSetArg(al[ac], XtNtranslations,
           XtParseTranslationTable("#override <Key>Return: ")); ac++;
  fg_depth = ESIAddButton("fg_depth", "", 
                          asciiTextWidgetClass, viewclip_form,
                          al, ac, NULL, NULL); 

  ac = 0;
  XtSetArg(al[ac], XtNfromVert, fg_depth); ac++;
  apply = ESIAddButton("apply", "           Apply             ", 
                       commandWidgetClass, viewclip_form,
                       al, ac, apply_viewclip_CB, NULL);
}

static void
apply_viewclip(EView *v_p, caddr_t data, WCRec *p);

static void
apply_viewclip_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  EVSetApplyToViewFunction(apply_viewclip, 0,
                           uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}


static void
apply_viewclip(EView *v_p, caddr_t data, WCRec *p)
{
  int ac;
  Arg al[4];
  Boolean state;
  Widget aw;
  double fgd = 0, bgd = 0;
  char *s;
  
  aw = do_viewclip;
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstate, &state); ac++;
    XtGetValues(aw, al, ac);
    EVSetFGBGViewPlaneClipOnOff(v_p, (BOOLEAN)state);
  }
  aw = XtNameToWidget(viewclip_form, "bg_depth");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    bgd = TypeInGetTokenAsDouble(1);
  } else
    fprintf(stderr, "Widget not found: bg_depth\n");
  aw = XtNameToWidget(viewclip_form, "fg_depth");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    fgd = TypeInGetTokenAsDouble(1);
  } else
    fprintf(stderr, "Widget not found: fg_depth\n");

  EVSetFGBGViewPlaneDepth(v_p, fgd, bgd);
  return;
  
 err_exit:
    ERptErrMessage(1, 1, "Bad input; must get numbers for depth", ERROR_GRADE);
  return;
}















static void
pass_mclip_command(Widget w, XtPointer ptr, XtPointer call_data);
static void
apply_modelclip_CB(Widget w, XtPointer ptr, XtPointer call_data);
static void
apply_inc_CB(Widget w, XtPointer ptr, XtPointer call_data);
static void
take_from_view_CB(Widget w, XtPointer ptr, XtPointer call_data);

static void 
ReturnHitMClipXYZ(Widget w, XEvent *event, String *params,
                  Cardinal *num_params);

static XtTranslations tt3;
static XtActionsRec remap_return_mclip_xyz[] = {
  {"retActMClipOK", ReturnHitMClipXYZ },
};


static Widget do_modelclip, modelclip_form, modelclip_pal;


void
setup_the_modelclip_palette(Widget pal)
{
  Widget lbl, lx, ly, lz, cx, cy, cz, nx, ny, nz, apply, cok, nok;
  Widget inclbl, inc, move;
  Widget take_from_view;
  int ac;
  Arg al[7];
  
  do_modelclip = ESIAddButton("do_modelclip", "Do model clip", 
                             toggleWidgetClass, pal, NULL, 0, NULL, NULL);
  
  modelclip_form = XtCreateManagedWidget("modelclip_form",
                                         formWidgetClass, pal, NULL, 0);
  ac = 0;
  XtSetArg(al[ac], XtNlabel, "Clip plane center"); ac++;
  lbl = XtCreateManagedWidget("lbl", labelWidgetClass,
                                 modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNlabel, "CX"); ac++;
  lx = XtCreateManagedWidget("lx", labelWidgetClass, modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, lx); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  cx = ESIAddButton("cx", "", 
                    asciiTextWidgetClass, modelclip_form, al, ac, NULL, NULL); 
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, cx); ac++;
  XtSetArg(al[ac], XtNlabel, "CY"); ac++;
  ly = XtCreateManagedWidget("ly", labelWidgetClass, modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ly); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  cy = ESIAddButton("cy", "", 
                    asciiTextWidgetClass, modelclip_form, al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, cy); ac++;
  XtSetArg(al[ac], XtNlabel, "CZ"); ac++;
  lz = XtCreateManagedWidget("lz", labelWidgetClass, modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, lz); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  cz = ESIAddButton("cz", "", 
                    asciiTextWidgetClass, modelclip_form, al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, cz); ac++;
  cok = ESIAddButton("cok", " OK ", commandWidgetClass, modelclip_form,
                     al, ac, pass_mclip_command, (XtPointer)0);
  XtAppAddActions(XtWidgetToApplicationContext(ESITopLevelWidget()),
                  remap_return_mclip_xyz, XtNumber(remap_return_mclip_xyz));
  tt3 = XtParseTranslationTable("#override <KeyPress>Return:"
                                " retActMClipOK(0)");
  XtOverrideTranslations(cx, tt3);
  XtOverrideTranslations(cy, tt3);
  XtOverrideTranslations(cz, tt3);

  ac = 0;
  XtSetArg(al[ac], XtNfromVert, cok); ac++;
  XtSetArg(al[ac], XtNlabel, "Clip plane normal"); ac++;
  lbl = XtCreateManagedWidget("lbl", labelWidgetClass,
                                 modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNlabel, "NX"); ac++;
  lx = XtCreateManagedWidget("lx", labelWidgetClass, modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, lx); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  nx = ESIAddButton("nx", "", 
                    asciiTextWidgetClass, modelclip_form, al, ac, NULL, NULL); 
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, nx); ac++;
  XtSetArg(al[ac], XtNlabel, "NY"); ac++;
  ly = XtCreateManagedWidget("ly", labelWidgetClass, modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ly); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  ny = ESIAddButton("ny", "", 
                    asciiTextWidgetClass, modelclip_form, al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ny); ac++;
  XtSetArg(al[ac], XtNlabel, "NZ"); ac++;
  lz = XtCreateManagedWidget("lz", labelWidgetClass, modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, lz); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  nz = ESIAddButton("nz", "", 
                    asciiTextWidgetClass, modelclip_form, al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, lbl); ac++;
  XtSetArg(al[ac], XtNfromHoriz, nz); ac++;
  nok = ESIAddButton("nok", " OK ", commandWidgetClass, modelclip_form,
                     al, ac, pass_mclip_command, (XtPointer)1);
  tt3 = XtParseTranslationTable("#override <KeyPress>Return: "
                                " retActMClipOK(1)");
  XtOverrideTranslations(nx, tt3);
  XtOverrideTranslations(ny, tt3);
  XtOverrideTranslations(nz, tt3);

  ac = 0;
  XtSetArg(al[ac], XtNfromVert, nok); ac++;
  XtSetArg(al[ac], XtNlabel, "Increment along plane normal"); ac++;
  inclbl = XtCreateManagedWidget("inclbl", labelWidgetClass,
                                 modelclip_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, nok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, inclbl); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++; 
  inc = ESIAddButton("inc", "1", 
                    asciiTextWidgetClass, modelclip_form, al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, nok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, inc); ac++;
  move = ESIAddButton("move", "Move center along normal", 
                       commandWidgetClass, modelclip_form,
                       al, ac, apply_inc_CB, NULL);
  
  
  ac = 0;
  take_from_view = ESIAddButton("take_from_view", "Take normal from view", 
                       commandWidgetClass, pal,
                       al, ac, take_from_view_CB, NULL);

  ac = 0;
  apply = ESIAddButton("apply", "           Apply             ", 
                       commandWidgetClass, pal,
                       al, ac, apply_modelclip_CB, NULL);
  modelclip_pal = pal;
}

static void
take_normal_from_view(EView *v_p, caddr_t data, WCRec *p);

static void
take_normal_from_view(EView *v_p, caddr_t data, WCRec *p)
{
  WCRec vn;
  char s[132];
  Widget aw;
  int ac;
  Arg al[4];
  
  EVGetNormal(v_p, &vn);
  
  aw = XtNameToWidget(modelclip_form, "nx");
  if (aw != NULL) {
    sprintf(s, "%g", vn.x);
    ac = 0;
    XtSetArg(al[ac], XtNstring, s); ac++;
    XtSetValues(aw, al, ac);
  } else
    fprintf(stderr, "Widget not found: nx\n");
  
  aw = XtNameToWidget(modelclip_form, "ny");
  if (aw != NULL) {
    sprintf(s, "%g", vn.y);
    ac = 0;
    XtSetArg(al[ac], XtNstring, s); ac++;
    XtSetValues(aw, al, ac);
  } else
    fprintf(stderr, "Widget not found: ny\n");

  aw = XtNameToWidget(modelclip_form, "nz");
  if (aw != NULL) {
    sprintf(s, "%g", vn.z);
    ac = 0;
    XtSetArg(al[ac], XtNstring, s); ac++;
    XtSetValues(aw, al, ac);
  } else
    fprintf(stderr, "Widget not found: nz\n");

}

static void
take_from_view_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  EVSetApplyToViewFunction(take_normal_from_view, 0,
                           uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}

static void
apply_modelclip(EView *v_p, caddr_t data, WCRec *p);

static void
apply_modelclip_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  EVSetApplyToViewFunction(apply_modelclip, 0,
                           uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}


static BOOLEAN
get_c_and_n(WCRec *center, WCRec *normal);


static EView *prev_v_p = NULL;

static void
apply_inc_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  Widget aw;
  int ac;
  Arg al[4];
  char *s, buf[32];
  Boolean state;
  double along, nl;
  WCRec center = {0, 0, 0}, normal = {0, 0, 0};

  aw = do_modelclip;
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstate, &state); ac++;
    XtGetValues(aw, al, ac);
    if (!state)
      return; /* job is done */
  }

  if (prev_v_p == NULL)
    return;                     /* no previous view */
  
  if (!get_c_and_n(&center, &normal)) 
    goto err_exit;
  
  aw = XtNameToWidget(modelclip_form, "inc");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    along = TypeInGetTokenAsDouble(1);
    nl = sqrt(DotProd3(&normal, &normal));
    if (nl == 0) nl = 1;
    center.x += normal.x * along / nl;
    center.y += normal.y * along / nl;
    center.z += normal.z * along / nl;
    sprintf(buf, "%f", center.x);
    aw = XtNameToWidget(modelclip_form, "cx");
    if (aw != NULL) {
      ac = 0;
      XtSetArg(al[ac], XtNstring, buf); ac++;
      XtSetValues(aw, al, ac);
    }
    sprintf(buf, "%f", center.y);
    aw = XtNameToWidget(modelclip_form, "cy");
    if (aw != NULL) {
      ac = 0;
      XtSetArg(al[ac], XtNstring, buf); ac++;
      XtSetValues(aw, al, ac);
    }
    sprintf(buf, "%f", center.z);
    aw = XtNameToWidget(modelclip_form, "cz");
    if (aw != NULL) {
      ac = 0;
      XtSetArg(al[ac], XtNstring, buf); ac++;
      XtSetValues(aw, al, ac);
    }
    EVSetModelClipPlane(prev_v_p, 0, &center, &normal, YES);
    EVForceRedisplay(prev_v_p);
  } else
    goto err_exit;

  return;
  
  err_exit:
    ERptErrMessage(1, 1,
                   "Bad input; must get number for increment along normal",
                   ERROR_GRADE);
}


static BOOLEAN
get_c_and_n(WCRec *center, WCRec *normal)
{
  int ac;
  Arg al[4];
  Widget aw;
  char *s;
  
  aw = XtNameToWidget(modelclip_form, "cx");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    center->x = TypeInGetTokenAsDouble(1);
  } else
    goto err_exit;
  
  aw = XtNameToWidget(modelclip_form, "cy");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    center->y = TypeInGetTokenAsDouble(1);
  } else
    goto err_exit;

  aw = XtNameToWidget(modelclip_form, "cz");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    center->z = TypeInGetTokenAsDouble(1);
  } else
    goto err_exit;


  aw = XtNameToWidget(modelclip_form, "nx");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    normal->x = TypeInGetTokenAsDouble(1);
  } else
    goto err_exit;
  
  aw = XtNameToWidget(modelclip_form, "ny");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    normal->y = TypeInGetTokenAsDouble(1);
  } else
    goto err_exit;

  aw = XtNameToWidget(modelclip_form, "nz");
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, &s); ac++;
    XtGetValues(aw, al, ac);
    TypeInParseLine(s);
    if (TypeInGetTokenType(1) != NUMBER)
      goto err_exit;
    normal->z = TypeInGetTokenAsDouble(1);
  } else
    goto err_exit;
  
  return YES;
  
 err_exit:
  center->x = center->y = center->z = 0;
  normal->x = normal->y = normal->z = 1;
  return NO;
}

static void
apply_modelclip(EView *v_p, caddr_t data, WCRec *p)
{
  int ac;
  Arg al[4];
  Boolean state;
  Widget aw;
  WCRec center = {0, 0, 0}, normal = {0, 0, 0}; 
  
  aw = do_modelclip;
  if (aw != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstate, &state); ac++;
    XtGetValues(aw, al, ac);
    EVSetModelPlaneClipOnOff(v_p, (BOOLEAN)state);
    if (!state)
      return; /* job is done */
  }
  if (!get_c_and_n(&center, &normal)) 
    goto err_exit;

  prev_v_p = v_p;
  EVSetModelClipPlane(v_p, 0, &center, &normal, YES);

  return;
  
 err_exit:
    ERptErrMessage(1, 1,
                   "Bad input; must get numbers for center and normal",
                   ERROR_GRADE);
  return;
}




static void
pass_mclip_command(Widget wid, XtPointer ptr, XtPointer call_data)
{
  int ac;
  Arg al[2];
  char *s;
  char buf[132];
  int flag = (int)ptr;
  Widget w;

  if (flag == 0)
    strcpy(buf, "MCLIP ON CENTER ");         /* absolute */
  else
    strcpy(buf, "MCLIP ON NORMAL ");         /* relative */
  
  ac = 0;
  XtSetArg(al[ac], XtNstring, &s); ac++;
  if (flag == 0) {
    w = XtNameToWidget(modelclip_form, "cx");
    XtGetValues(w, al, ac);
  } else {
    w = XtNameToWidget(modelclip_form, "nx");
    XtGetValues(w, al, ac);
  }
  TypeInParseLine(s);
  if (TypeInGetTokenType(1) != NUMBER)
    goto err_exit;
  strcat(buf, s); strcat(buf, " ");
 
  ac = 0;
  XtSetArg(al[ac], XtNstring, &s); ac++;
  if (flag == 0) {
    w = XtNameToWidget(modelclip_form, "cy");
    XtGetValues(w, al, ac);
  } else {
    w = XtNameToWidget(modelclip_form, "ny");
    XtGetValues(w, al, ac);
  }
  TypeInParseLine(s);
  if (TypeInGetTokenType(1) != NUMBER)
    goto err_exit;
  strcat(buf, s); strcat(buf, " ");
  
  ac = 0;
  XtSetArg(al[ac], XtNstring, &s); ac++;
  if (flag == 0) {
    w = XtNameToWidget(modelclip_form, "cz");
    XtGetValues(w, al, ac);
  } else {
    w = XtNameToWidget(modelclip_form, "nz");
    XtGetValues(w, al, ac);
  }
  TypeInParseLine(s);
  if (TypeInGetTokenType(1) != NUMBER)
    goto err_exit;
  strcat(buf, s); strcat(buf, " ");
  
  ESIHandleCmd(buf);

  return;
  
 err_exit:
    ERptErrMessage(1, 1, "Bad input; must get 3 numbers", ERROR_GRADE);
  return;
}



static void 
ReturnHitMClipXYZ(Widget wid, XEvent *event, String *params,
                  Cardinal *num_params)
{
  Widget w;
  
  if (*num_params == 1) {
    if (strcmp(params[0], "0") == 0) {
      w = XtNameToWidget(modelclip_form, "cok");
      if (w != NULL)
        XtCallCallbacks(w, XtNcallback, NULL);
    } else {
      w = XtNameToWidget(modelclip_form, "nok");
      if (w != NULL)
        XtCallCallbacks(w, XtNcallback, NULL);
    }
  }
}


extern int color_scale_num_labels;

void
edit_color_scale(Widget color_scale_palette, EView *v_p)
{
  FPNum delta;
  FPNum minv, maxv, v;
  EPixel color;
  int i;
  Widget label;
  char lname[64];
  Arg al[2];
  int ac;
  EFringeTable ft;

  ft = EVGetAssocFringeTable(v_p);
  ColorFringesMinMax(ft, &minv, &maxv);
  delta = (maxv - minv) / color_scale_num_labels;
  v = minv + delta / 2.0;
  for (i = 0; i < color_scale_num_labels; i++) {
    if (v_p->render_mode == FILLED_HIDDEN_RENDERING ||
        v_p->render_mode == CONST_SHADING_RENDERING) {
      color = ColorFringeValueToColorHSV(ft, v);
    } else {
      color = ColorFringeRangeToColor(ColorFringeValueToRange(ft, v));
    }
    sprintf(lname, "color_scale_%d", i);
    label = XtNameToWidget(color_scale_palette, lname);
    sprintf(lname, "%g", v);
    ac = 0; 
    XtSetArg(al[ac], XtNbackground, color); ac++;
    XtSetArg(al[ac], XtNlabel, lname); ac++;
    XtSetValues(label, al, ac);
    v += delta;
  }
}
