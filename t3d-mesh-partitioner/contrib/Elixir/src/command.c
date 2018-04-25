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
#include "Etypein.h"
#include "Eview.h"
#include "Emodel.h"
#include "Eactset.h"
#include "Ege.h"
#include "Egecb.h"
#include "Ecolors.h"
#include "Efonts.h"
#include "Eondisk.h"
#include "Eggroup.h"
#include "Esimple.h"

static Widget widget = NULL;    /* handle_command called for this widget */

static void init_command(void);
void uninstall_apply_to_view(EView *v_p, caddr_t data);
static void apply_show_grid(EView *v_p, caddr_t data, WCRec *);
static void apply_lock_grid(EView *v_p, caddr_t data, WCRec *);
static void apply_lock_cplane(EView *v_p, caddr_t data, WCRec *);
static void redraw(EView *v_p, caddr_t data, WCRec *p);
static long unsigned mask_from_cl(void);
static void load_view_settings(EView *v_p, caddr_t f, WCRec *s);
static void save_view_settings(EView *v_p, caddr_t f, WCRec *s);

FILE *
ESISelFile(char *prompt, char *ok, char *cancel, char *failed,
           char *init_path, char *mode, char **name_return);

 /* The commands to be handled  */
static void
help_me_please(void);
static void send_point(void);
static void send_point_delta(void);
static void show_in_view(void);
static void show_grid(void);
static void lock_in_view(void);
static void lock_to_grid(void);
static void lock_to_cplane(void);
static void save_model(void);
static void load_model(void);
static void fit_into_a_view(void);
static void set_active_style(void);
static void set_active_width(void);
static void set_active_rangle(void);
static void set_active_mtype(void);
static void set_active_msize(void);
static void set_active_color(void);
static void set_vclip(void);
static void set_mclip(void);
static void 
set_active_edgeflag(void);
static void 
set_active_edgecolor(void);
static void set_active_shrink(void);
static void set_active_scale(void);
static void set_active_fill(void);
static void set_active_font(void);
static void set_active_text(void);
static void set_active_pshow(void);
static void set_active_eshow(void);
static void set_fringe_table(void);

static void set_active_notclippable(void);
static void set_active_inviswhenclipped(void);

static void set_active_tesseli(void);
static void set_active_selcrit(void);
static void set_grid_d_or_o(void);
static void set_constrplane(void);
static void layers_setup(void);
static void ggroups(void);
static void view_setup(void);
static void set_keypintervals(void);
static void set_change_mask(void);
static void change_attribs(void);
static void set_render_shade(void);
static void new_frame(void);
static void graphic_info(void);
static void postscript_dump(void);
static void set_active_u_tesseli(void);
static void set_active_v_tesseli(void);
static void set_vec_type(void);
static void set_vec_scale(void);
static void set_vec_rate(void);
static void set_vec_shift(void);
static void isolines_setup(void);


static top_command   commands[] = {
 /* command string      its length    command handler */ 
  { "HELP",               4,           help_me_please     ,
      "HELP keyword" },
  { "XY",                 2,           send_point         ,
      "# # # (set point absolute coordinates)" },
  { "DX",                 2,           send_point_delta   ,
      "# # # (set point by delta from the last input point)" },
  { "SHOW",               4,           show_in_view       ,
      "{GRID} (show ... in view)" },
  { "LOCK",               4,           lock_in_view       ,
      "{GRID|CPLANE} (lock in view to ...)" },
  { "SAVE",               4,           save_model         ,
      "AS file_name (save graphics to a file)" },
  { "LOAD",               4,           load_model         ,
      "file_name (load graphics from a file)" },
  { "FIT",                3,           fit_into_a_view    ,
      "(fit all graphics into a view)" },
  { "STYLE",              5,           set_active_style   ,
      "{SOLID|DASH} (set line style)" },
  { "WIDTH",              5,           set_active_width   ,
      "# (set line width)" },
  { "RANGLE",             6,           set_active_rangle  ,
      "# [DEG] (set rotation angle)" },
  { "MTYPE",              5,           set_active_mtype   ,
      "# (set marker type - 0,1,2,3)" },
  { "MSIZE",              5,           set_active_msize   ,
      "# (set marker size in pixels)" },
  { "COLOR",              5,           set_active_color   ,
      "color_name (set color)" },
  { "FRINGE",              5,           set_fringe_table   ,
      "low high (set fringe table by minmax values)" },
  { "FILL",               4,           set_active_fill    ,
      "{OFF|{ON|SOLID}} (set fill style)" },
  { "EDGECOLOR",          8,           set_active_edgecolor,
      "color_name (set edge color)" },
  { "EDGEFLAG",          8,            set_active_edgeflag,
      "{ON|OFF} (set edge flag)" },
  { "SHRINK",             6,           set_active_shrink  ,
      "# (set shrink 0 < s && s <= 1)" },
  { "SCALE",              5,           set_active_scale  ,
      "# (set scale > 0)" },
  { "FONT",               4,           set_active_font    ,
      "font_name (set font)" },
  { "TEXT",               4,           set_active_text    ,
      "the_text (set text)" },
  { "PSHOW",              5,           set_active_pshow   ,
      "{ON|OFF} (set show-control-polygon flag)" },
  { "NOTCLIP",            7,           set_active_notclippable,
      "{ON|OFF} (set \"not clippable\" flag)" },
  { "INVISWH",            7,           set_active_inviswhenclipped,
      "{ON|OFF} (set \"invisible when clipped\" flag)" },
  { "ESHOW",              5,           set_active_eshow   ,
      "{ON|OFF} (set show-entity flag)" },
  { "TESSEL",             6,           set_active_tesseli ,
      "# (set tesselation intervals >0)" },
  { "SELCRIT",            7,           set_active_selcrit ,
      "{OVERLAP|INTERSECT|INSIDE|\n\tATTRIB {CLEAR|NOT|attrib_mask}} (set selection criteria)" },
  { "LAYER",              5,           layers_setup       ,
      "{ACTIVE #|ON #|OFF #|TOGGLE} (set layer)" },
  { "GGROUP",             6,           ggroups            ,
      "{CREATE|BREAK} (create or break a graphic group)" },
  { "GDELTA",             6,           set_grid_d_or_o    ,
      "# # (set grid deltas in VC or in construction plane coordinates)" },
  { "GORIGIN",            7,           set_grid_d_or_o    ,
      "# # # (set grid origin in VC or in construction plane coordinates)" },
  { "CPLANE",             6,           set_constrplane    ,
      "{CENTER|ORIENTATION} # # # # # # (set construction plane \n"
        "                 orientation -- normal and the vector \"up\")" },
  { "VIEW",               4,           view_setup         ,
      "{{NOR|VUP|VRP|ORIGIN} # # #|\n\tZOOM #|{PANVIEW|ROTVIEW|ROTNOR} {X|Y|Z} # [DEG]|\n\tTOP|BOTTOM|LEFT|RIGHT|FRONT|BACK|ISO|\n\t{AXES|SCALE|STATUS|PRESERVE_VUP} {ON|OFF}|\n\t{SAVE|LOAD} name|BGROUND color} (set viewing parameters)" },
  { "KEYP",               4,           set_keypintervals  ,
      "# (set number of keypoints per graphic object)" },
  { "CHMASK",             6,           set_change_mask    ,
      "{LAYER|STYLE|COLOR|FONT|WIDTH|MTYPE|MSIZE|FILL|SHRINK|TEXT\n\tEDGECOLOR|EDGEFLAG|VECTYPE|VECSCALE|VECRATE|VECSHIFT\n\tTESSEL|UTESSEL|VTESSEL|PSHOW|ESHOW} (set change mask)" },
  { "CHANGE",             6,           change_attribs     ,
      "change_mask (see CHMASK command) (change named attribute)" },
  { "RENDER",             6,           set_render_shade   ,
      "{NORMAL|LDIR # # #|AMBIENT #|SHADE [DITHER]|FILL|WIRE}"
      "(set rendering parameters in a view)" },
  { "NEWFRAME",           4,           new_frame          ,
      "[{SHADE|WIRE|HIDDEN}] (opens new view window)" },
  { "GINFO",              5,           graphic_info,
      "(prints info about graphic object)" },
  { "PSDUMP",             6,           postscript_dump,
      "ps_file [# #] [PORTRAIT][EPS] (creates postscript dump of a view window)" },
  { "VCLIP",              5,           set_vclip,
      "{BGDEPTH #|FGDEPTH #|ON|OFF} (sets view clip params)" },
  { "MCLIP",              5,           set_mclip,
      "{CENTER # # #|NORMAL # # #|ON|OFF} (sets modelling clip params)" },
  { "UTESSEL",            7,           set_active_u_tesseli ,
      "# (set u tesselation intervals >0)" },
  { "VTESSEL",            7,           set_active_v_tesseli ,
      "# (set v tesselation intervals >0)" },
  { "VECTYPE",            7,           set_vec_type,
      "# (set vector type - 0,1,2,3)" },
  { "VECSCALE",           8,           set_vec_scale,
      "# (set vector scale)" },
  { "VECRATE",            7,           set_vec_rate,
      "# (set vector decoration rate)" },
  { "VECSHIFT",           8,           set_vec_shift,
      "# (set vector shift - 0,1)" },
	{ "ISOLINES",           8,           isolines_setup,
		"{{COLOR|SMOOTH|CONTOUR|TRANSPARENT} {ON|OFF}\n\tRANGE [# #]|{WIDTH|COUNT|SCALE} #|REVERT\n\tBGCOLOR color} (set isolines params)" },
};

XEvent command_event;
BOOLEAN on_off_flag;

static BOOLEAN first_time = YES;


static BOOLEAN can_use_target_view = TRUE;
static BOOLEAN force_redisplay_target_view = TRUE;

static void
apply_command_to_view (ApplyFuncP func_p, caddr_t data_p, ResetFuncP reset_func_p)
{
	EView *v_p_target = NULL;
  EModel *m_p = ESIModel ();

  if (EMCountDepViews (m_p) == 1)
    v_p_target = EMAnyViewP (m_p);
  else{
		if(EMGetApplyToAllViews(ESIModel()) == YES)
			v_p_target = NULL;
		else
			v_p_target = EMGetViewToApply(ESIModel());
	}

  if (v_p_target != NULL && can_use_target_view) {

/* the consequence of using v_p_target is that a command cannot be repeated by 
	 picking repeatedly still the same window;
	 however, the command issued from the command form can be repeated by pressing
	 repeatedly Enter or by clicking repeatedly Execute button - now fixed by dr */

    WCRec p = {0,0,0};
    (*func_p) (v_p_target, data_p, &p);
		if(force_redisplay_target_view)EVForceRedisplay(v_p_target);
		force_redisplay_target_view = TRUE;
  } else {
		if(EMGetApplyToAllViews(ESIModel()) == YES){
			WCRec p = {0,0,0};
			v_p_target = (EView *)get_list_next(m_p->dependent_views, NULL);
			while(v_p_target != NULL){
				(*func_p) (v_p_target, data_p, &p);
				v_p_target = (EView *)get_list_next(m_p->dependent_views, v_p_target);
			}
		}
		else{
			EVSetApplyToViewFunction (func_p, data_p, reset_func_p);
			EMPushHandler (m_p, EVApplyToViewHandler, NULL);
		}
  }
  can_use_target_view = TRUE;
}


static void
unset_target_view (void)
{
  can_use_target_view = FALSE;
}


static void
prevent_force_redisplay_target_view (void)
{
  force_redisplay_target_view = FALSE;
}


static void 
send_point(void)
{
  WCRec p;
  Window win_id;
  
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  if (TypeInGetTokenType(3) != NUMBER)
    TypeInBadToken(3, "NUMBER");
  if (TypeInGetTokenType(4) != NUMBER)
    TypeInBadToken(4, "NUMBER");

  p.x = (FPNum)TypeInGetTokenAsDouble(2);
  p.y = (FPNum)TypeInGetTokenAsDouble(3);
  p.z = (FPNum)TypeInGetTokenAsDouble(4);

  EVSaveSentPoint(&p);
  win_id = XtWindow(EVViewPToWidget(EMAnyViewP(ESIModel())));
  command_event.xbutton.type       = ButtonPress;
  command_event.xbutton.send_event = True;
  command_event.xbutton.display    = XtDisplay(widget);
  command_event.xbutton.window     = win_id;
  command_event.xbutton.x          = 0;
  command_event.xbutton.y          = 0;
  command_event.xbutton.state      = Button1Mask;
  command_event.xbutton.button     = Button1;

  if (XSendEvent(XtDisplay(widget), win_id, True, (unsigned long)0,
		  &command_event) == 0)
    fprintf(stderr, "XSendEvent failed\n");
}
  


static void 
send_point_delta(void)
{
  WCRec last, p;
  Window win_id;

  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  if (TypeInGetTokenType(3) != NUMBER)
    TypeInBadToken(3, "NUMBER");
  if (TypeInGetTokenType(4) != NUMBER)
    TypeInBadToken(4, "NUMBER");

  p.x = (FPNum)TypeInGetTokenAsDouble(2);
  p.y = (FPNum)TypeInGetTokenAsDouble(3);
  p.z = (FPNum)TypeInGetTokenAsDouble(4);
  
  EVGetLastInputPoint(&last);
  p.x += last.x;
  p.y += last.y;
  p.z += last.z;
  EVSaveSentPoint(&p);
  win_id = XtWindow(EVViewPToWidget(EMAnyViewP(ESIModel())));
  command_event.xbutton.type       = ButtonPress;
  command_event.xbutton.send_event = True;
  command_event.xbutton.display    = XtDisplay(widget);
  command_event.xbutton.window     = win_id;
  command_event.xbutton.x          = 0;
  command_event.xbutton.y          = 0;
  command_event.xbutton.state      = Button1Mask;
  command_event.xbutton.button     = Button1;

  if (XSendEvent(XtDisplay(widget), win_id, True, (unsigned long)0,
		  &command_event) == 0)
    fprintf(stderr, "XSendEvent failed\n");
}
  
 

 
static void 
init_command(void)
{
  TypeInAugmentCommandTable(commands, XtNumber(commands));
}





void
handle_command(Widget w, char *s)
{
  if (first_time == YES){
    init_command();
    first_time = NO;
  }

  widget = w; /* called for this widget */
  
  if (TypeInParseLine(s) != 0) {
    /*      TypeInPrintLineContents(void); */
    TypeInInterpretCurrentCommand();
  }
}



void 
uninstall_apply_to_view(EView *v_p, caddr_t data)
{
  EMUninstallHandler(EVGetModel(v_p));
}



static void 
apply_show_grid(EView *v_p, caddr_t data, WCRec *p)
{
  EVShowGrid(v_p, *(BOOLEAN*)data);
}



static void 
show_in_view(void)
{
  char *s;
  
  if (TypeInGetTokenType(2) != STRING)
    TypeInBadToken(2, "STRING");
  s = TypeInGetTokenAsString(2);
  if (TypeInCompareTokenWithKeyword(s, "GRID") == YES)
    show_grid();
}



static void 
show_grid(void)
{
  on_off_flag = YES;

  if (TypeInIsKeywordInLine(2, "ON") > 0)
    on_off_flag = YES;
  else
    on_off_flag = NO;

  apply_command_to_view (apply_show_grid, (caddr_t)&on_off_flag,
                         uninstall_apply_to_view);
}



static void 
lock_in_view(void)
{
  if (TypeInCompareTokenWithKeyword(TypeInGetTokenAsString(2), "GRID"))
    lock_to_grid();
  else if (TypeInCompareTokenWithKeyword(TypeInGetTokenAsString(2), "CPLANE"))
    lock_to_cplane();
}



static void 
apply_lock_grid(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetGridLock(v_p, *(BOOLEAN*)data);
  EVShowGrid(v_p, YES);
}



static void 
lock_to_grid(void)
{
  on_off_flag = YES;

  if (TypeInIsKeywordInLine(2, "ON") > 0)
    on_off_flag = YES;
  else
    on_off_flag = NO;

  apply_command_to_view (apply_lock_grid, (caddr_t)&on_off_flag,
                         uninstall_apply_to_view);
}


static void 
apply_lock_cplane(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetConstrPlaneLock(v_p, *(BOOLEAN*)data);
}



static void 
lock_to_cplane(void)
{
  on_off_flag = YES;

  if (TypeInIsKeywordInLine(2, "ON") > 0)
    on_off_flag = YES;
  else
    on_off_flag = NO;

  apply_command_to_view (apply_lock_cplane, (caddr_t)&on_off_flag,
                         uninstall_apply_to_view);
}



static void 
save_model(void)
{
  int   i;
  char *name = NULL;
  FILE *fp;

  i = TypeInIsKeywordInLine(2, "AS");
  if (TypeInGetTokensTotal() == 1) { /* save under the original name */
    name = OnDiskGetFileName();
  } else if (i > 0 && TypeInGetTokensTotal() >= 3) {
    if (TypeInGetTokenType(i+1) != STRING) {
      TypeInBadToken(i+1, "STRING");
      return;
    }
    name = TypeInGetTokenAsString(i+1);
  }
  if (name == NULL) {
    fp = ESISelFile("Select file name",
                    "   OK   ", "CANCEL", NULL, NULL, "w", &name);
    if (fp) {
      fclose(fp);
    } else {
      return;                   /* Cancelled */
    }
  }
  OnDiskSetFileName(name);
  EMSaveAllGraphics(ESIModel());
}



static void 
load_model(void)
{
  FILE *fp;
  char *name = NULL;
  
  /* expecting syntax: LOAD file_name */
  
  if (TypeInGetTokensTotal() != 2) {
    fp = ESISelFile("Select ELIXIR file",
                    "   OK   ", "CANCEL", NULL, NULL, "r", &name);
    if (fp) {
      OnDiskSetFileName(name);
      fclose(fp);
    } else {
      return;                   /* Cancelled */
    }
  } else {
    OnDiskSetFileName(TypeInGetTokenAsString(2));
  }
  EMRetrieveAllGraphics(ESIModel());
}



static void 
apply_fit(EView *v_p, caddr_t data, WCRec *p)
{
  EVFitAllIntoView(v_p);
}



static void 
fit_into_a_view(void)
{
  apply_command_to_view (apply_fit, NULL, uninstall_apply_to_view);
}



static void 
set_active_style(void)
{
  if (TypeInIsKeywordInLine(2, "SOLID") > 0)
    EASValsSetLineStyle(SOLID_STYLE);
  if (TypeInIsKeywordInLine(2, "DASH") > 0)
    EASValsSetLineStyle(DASHED_STYLE);
}



static void 
set_active_width(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetLineWidth((unsigned)TypeInGetTokenAsInt(2));
  }
}



static void 
set_active_rangle(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    FPNum angle;

    angle = TypeInGetTokenAsDouble(2);
    if (TypeInIsKeywordInLine(2, "DEG") >0 ||
	TypeInIsKeywordInLine(2, "DEGREE") >0) angle *= PI/180.0;
    
    EASValsSetRotationAngle(angle);
  }
}



static void 
set_active_mtype(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetMType(TypeInGetTokenAsInt(2));
  }
}



static void 
set_active_msize(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetMSize((unsigned)TypeInGetTokenAsInt(2));
  }
}



static void 
set_active_pshow(void)
{
  EASValsSetShowPoly(!(TypeInIsKeywordInLine(2, "OFF") > 0));
}



static void 
set_active_eshow(void)
{
  EASValsSetShowEntity(!(TypeInIsKeywordInLine(2, "OFF") > 0));
}



static void 
set_active_notclippable(void)
{
  EASValsSetNotClippable(!(TypeInIsKeywordInLine(2, "OFF") > 0));
}


static void 
set_active_inviswhenclipped(void)
{
  EASValsSetInvisibleWhenClipped(!(TypeInIsKeywordInLine(2, "OFF") > 0));
}


static void 
set_active_tesseli(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetTesselIntervals(TypeInGetTokenAsInt(2));
  }
}



static void 
set_active_selcrit(void)
{
  if (TypeInIsKeywordInLine(2, "OVERLAP") > 0)
    EASValsSetSelectCriterion(OVERLAP);
  if (TypeInIsKeywordInLine(2, "INTERSECT") > 0)
    EASValsSetSelectCriterion(INTERSECT);
  if (TypeInIsKeywordInLine(2, "INSIDE") > 0)
    EASValsSetSelectCriterion(INSIDE);
  if (TypeInIsKeywordInLine(2, "ATTRIB") > 0) {
    if (TypeInIsKeywordInLine(2, "CLEAR") > 0) {
      EVSetAttribMask((unsigned long)0);
      EVSetTakeAsNOT(NO);
    } else 
      EVSetAttribMask(EVGetAttribMask() | mask_from_cl());
    if (TypeInIsKeywordInLine(2, "NOT") > 0)
      EVSetTakeAsNOT(YES);
  }
    
}



static void 
set_keypintervals(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetKeyPIntervals((unsigned)TypeInGetTokenAsInt(2));
  }
}



static void 
set_active_color(void)
{
  EPixel clr;
  BOOLEAN     success;
  
  if (TypeInGetTokenType(2) != STRING)
    TypeInBadToken(2, "STRING");
  else {
    clr = ColorGetPixelFromString(TypeInGetTokenAsString(2), &success);
    if (success)
      EASValsSetColor(clr);
  }
}


static void 
set_active_edgecolor(void)
{
  EPixel clr;
  BOOLEAN     success;
  
  if (TypeInGetTokenType(2) != STRING)
    TypeInBadToken(2, "STRING");
  else {
    clr = ColorGetPixelFromString(TypeInGetTokenAsString(2), &success);
    if (success)
      EASValsSetEdgeColor(clr);
  }
}




static void 
set_active_font(void)
{
  Font fid;
  BOOLEAN     success;
  
  if (TypeInGetTokenType(2) != STRING)
    TypeInBadToken(2, "STRING");
  else {
    fid = FontGetFontFromString(TypeInGetTokenAsString(2), &success);
    if (success)
      EASValsSetFontId(fid);
  }
}



static void 
set_active_text(void)
{
  if (TypeInGetTokenType(2) != STRING)
    TypeInBadToken(2, "STRING");
  else {
    EASValsSetText(TypeInGetTokenAsString(2));
  }
}

#define DELTA  0
#define ORIGIN 1 

static int gflag = DELTA;
static FPNum val1 = 0., val2 = 0.;



static void apply_gdeltaorigin(EView *v_p, caddr_t data, WCRec *p)
{
  if (gflag == DELTA)
    EVSetGridDeltas(v_p, val1, val2);
  else
    EVSetGridOrigin(v_p, val1, val2);
}



static void 
set_grid_d_or_o(void)
{
  if (TypeInGetTokenType(2) != NUMBER) {
    TypeInBadToken(2, "NUMBER");
    return;
  } else if (TypeInGetTokenType(3) != NUMBER) {
    TypeInBadToken(3, "NUMBER");
    return;
  }
  if (TypeInCompareTokenWithKeyword(TypeInGetTokenAsString(1), "GDELTA"))
    gflag = DELTA;
  else
    gflag = ORIGIN;
  val1 = TypeInGetTokenAsDouble(2);   val2 = TypeInGetTokenAsDouble(3);

  apply_command_to_view (apply_gdeltaorigin, NULL,
                         uninstall_apply_to_view);
}



static int layer = 0, layer_on = YES;

static void 
toggle_layer_onoff(EView *v_p, caddr_t data, WCRec *p)
{
  EVToggleLayerOnOff(v_p, layer);
}

static void 
set_layer_onoff(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetLayerOnOff(v_p, layer, layer_on);
}

static void 
layers_setup(void)
{
  int pos;
  
  if ((pos = TypeInIsKeywordInLine(2, "ACTIVE")) > 0) {
    if ((layer = TypeInGetTokenAsInt(pos+1)) >= 0) 
      EASValsSetLayer(layer);
  } else if (TypeInGetTokenType(2) == NUMBER) {
    if ((layer = TypeInGetTokenAsInt(2)) >= 0) 
      EASValsSetLayer(layer);
  } else if ((pos = TypeInIsKeywordInLine(2, "ON")) > 0) {
    layer_on = YES;
    if ((layer = TypeInGetTokenAsInt(pos+1)) >= 0) {
      apply_command_to_view (set_layer_onoff, NULL,
                             uninstall_apply_to_view);
    }
  } else if ((pos = TypeInIsKeywordInLine(2, "OFF")) > 0) {
    layer_on = NO;
    if ((layer = TypeInGetTokenAsInt(pos+1)) >= 0) {
      apply_command_to_view (set_layer_onoff, NULL,
                             uninstall_apply_to_view);
    }
  } else if ((pos = TypeInIsKeywordInLine(2, "TOGGLE")) > 0) {
    if ((layer = TypeInGetTokenAsInt(pos+1)) >= 0) {
      apply_command_to_view (toggle_layer_onoff, NULL,
                             uninstall_apply_to_view);
    }
  }
}




static int 
set_view(NODE data, NODE v_p)            /* added by dr to prevent blinking */
{
	EVStoreRenderMode((EView *)v_p);
	EVSetRenderMode((EView *)v_p, WIRE_RENDERING);
	return(1);
}

	
static int 
reset_view(NODE data, NODE v_p)          /* added by dr to prevent blinking */
{
	EVRestoreRenderMode((EView *)v_p);
	EVForceRedisplay((EView *)v_p);
	return(1);
}



static void
ggroup_reset(EView *v_p, caddr_t data_p);
static void
ggroup_reset(EView *v_p, caddr_t data_p)
{
  EMUninstallHandler(EVGetModel(v_p));
	EMDispatchToDependentViews(ESIModel(), reset_view, NULL);   /* added by dr to prevent blinking */
}




void 
ggroup_create(EView *v_p, caddr_t data_p);
void 
ggroup_break(EView *v_p, caddr_t data_p);



static void 
ggroups(void)
{
  if (TypeInIsKeywordInLine(2, "CREATE") > 0) {
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);          /* added by dr to prevent blinking */
    EVSetSelectActionFunction(ggroup_create, NULL, NULL);
		ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "Select graphics to group");          /* added by dr */
	}
  else if (TypeInIsKeywordInLine(2, "BREAK") > 0){
		EMDispatchToDependentViews(ESIModel(), set_view, NULL);          /* added by dr to prevent blinking */
    EVSetSelectActionFunction(ggroup_break, NULL, NULL);
		ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "Select group(s) to break");          /* added by dr */
	}
  else
    goto exit;
	/* using ggroup_reset as the last argument of EVSetSelectActionFunction does not work here !!! */

	/*  EMPushHandler(ESIModel(), EVSelectGraphicsHandler, NULL); */
  EMInstallHandler(ESIModel(), EVSelectGraphicsHandler,              /* added by dr to prevent blinking */
		   NULL, NULL,
		   NULL, NULL,
		   NULL, NULL,
			 ggroup_reset, NULL);
 exit:
  ;
}






static WCRec a_vect = {0, 0, 0};

static void 
view_setup_vo(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetOriginVC(v_p, p);
}


/* added by dr */

/* panview and rotview shift and rotate the view;
	 thus the object is moving in opposite direction !!! */

static double panview_shift = 0.0;
static int panview_dir = 0;

static void 
view_setup_panview (EView *v_p, caddr_t data, WCRec *p)           
{
  WCRec refpnt;
  
	EVGetVRP(v_p, &refpnt);

  switch (panview_dir) {
  case 0:
		refpnt.x += panview_shift;
		break;
  case 1:
		refpnt.y += panview_shift;
		break;
  case 2:
		refpnt.z += panview_shift;
		break;
	default:
		return;
	}
	
	EVSetVRP(v_p, &refpnt);
}


static double rotview_angle = 0.0;
static int rotview_dir = 2;

static void 
view_setup_rotview (EView *v_p, caddr_t data, WCRec *p)           
{
  WCRec normal_old, normal_new, vect_up, e={0.0,0.0,0.0}, ee={0.0,0.0,0.0};
	WCRec refpnt;
  VCRec rp;

	EVGetVRP(v_p, &refpnt);
  EVWCtoVC(v_p, &refpnt, &rp);
  
	EVGetVectUp (v_p, &vect_up);
  EVGetNormal (v_p, &normal_old);

/* steps: 1. rotate axis after rotview_dir about rotview_dir by angle
          2. rotate axis before rotview_dir about rotview_dir by angle
             this is calculated is cross product rotview_dir x result of step 1
          3. setup transformation matrix 
          4. multiply transformation matrix by normal
					5. rotate vectUp about rotview_dir by -angle */

  switch (rotview_dir) {
  case 0:
    e.x = -rotview_angle;
		ee.y = 1.0;
		RotVectAboutVect (&e, &ee);
		normal_new.x = normal_old.x;
		normal_new.y = normal_old.y * ee.y + normal_old.z * ee.z;
		normal_new.z = -normal_old.y * ee.z + normal_old.z * ee.y;
		e.x *= -1.0;
    break;
  case 1:
    e.y = -rotview_angle;
		ee.z = 1.0;
		RotVectAboutVect (&e, &ee);
		normal_new.x = normal_old.x * ee.z - normal_old.z * ee.x;
		normal_new.y = normal_old.y;
		normal_new.z = normal_old.x * ee.x + normal_old.z * ee.z;
		e.y *= -1.0;
    break;
  case 2:
    e.z = -rotview_angle;
		ee.x = 1.0;
		RotVectAboutVect (&e, &ee);
		normal_new.x = normal_old.x * ee.x + normal_old.y * ee.y;
		normal_new.y = -normal_old.x * ee.y + normal_old.y * ee.x;
		normal_new.z = normal_old.z;
		e.z *= -1.0;
    break;
  default:
    return;
  }

	RotVectAboutVect (&e, &vect_up);
	EVInhibitViewOrientationSetup();
  EVSetVectUp (v_p, &vect_up);
  EVSetNormal (v_p, &normal_new);

  EVVCtoWC(v_p, &rp, &refpnt);
	EVSetVRP(v_p, &refpnt);
}


static double rotnor_angle = 0.0;
static int rotnor_dir = 2;

static void 
view_setup_rotnor (EView *v_p, caddr_t data, WCRec *p)
{
  WCRec normal, e={0.0,0.0,0.0};
  
  switch (rotnor_dir) {
  case 0:
    e.x = rotnor_angle;
    break;
  case 1:
    e.y = rotnor_angle;
    break;
  case 2:
    e.z = rotnor_angle;
    break;
  default:
    return;
  }
  
  EVGetNormal (v_p, &normal);
  RotVectAboutVect (&e, &normal);
  EVSetNormal (v_p, &normal);
}


static double zoom_factor = 1.0;

static void 
view_zoom (EView *v_p, caddr_t data, WCRec *p)
{
  FPNum xdim, ydim;
  
  EVGetDimsVC(v_p, &xdim, &ydim);
  EVSetDimsVC(v_p, xdim*zoom_factor, ydim*zoom_factor);
}


static void 
set_view_bkg(EView *v_p, EPixel bkg, caddr_t data);

static void 
set_view_bkg(EView *v_p, EPixel bkg, caddr_t data)
{
  EVSetBackground(v_p, bkg);
}


/* less flexible command syntax is forced by checking the precise position
	 of the keyword and not only its presence on command line - by dr */

static void 
view_setup(void)
{
  if (TypeInGetTokenType(3) == NUMBER &&
      TypeInGetTokenType(4) == NUMBER &&
      TypeInGetTokenType(5) == NUMBER)   {
    a_vect.x = TypeInGetTokenAsDouble(3);
    a_vect.y = TypeInGetTokenAsDouble(4);
    a_vect.z = TypeInGetTokenAsDouble(5);
    if        (TypeInIsKeywordInLine(2, "NOR") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetNormal,
														 (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "VUP") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetVectUp,
														 (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "VRP") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetVRP,
														 (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "ORIGIN") == 2) {
			EVSetApplyToViewPreventRedisplay(YES);
			prevent_force_redisplay_target_view();
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetOriginVC,
														 (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    }
  } else {
    if        (TypeInIsKeywordInLine(2, "TOP") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetViewOrientation,
														 (caddr_t)VIEW_ORIENT_TOP,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "BOTTOM") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetViewOrientation,
														 (caddr_t)VIEW_ORIENT_BOTTOM,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "LEFT") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetViewOrientation,
														 (caddr_t)VIEW_ORIENT_LEFT,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "RIGHT") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetViewOrientation,
														 (caddr_t)VIEW_ORIENT_RIGHT,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "FRONT") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetViewOrientation,
														 (caddr_t)VIEW_ORIENT_FRONT,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "BACK") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetViewOrientation,
														 (caddr_t)VIEW_ORIENT_BACK,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "ISO") == 2) {
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVSetViewOrientation,
														 (caddr_t)VIEW_ORIENT_ISO,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "ORIGIN") == 2) {
      unset_target_view ();
			EVSetApplyToViewPreventRedisplay(YES);
			prevent_force_redisplay_target_view();
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))view_setup_vo,
														 (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "PRESERVE_VUP") == 2) {
      BOOLEAN yesorno = YES;
      if (TypeInIsKeywordInLine(3, "OFF") == 3) yesorno = NO;
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVPreserveVectUp,
                             (caddr_t)yesorno,
                             uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "ZOOM") == 2) {
      zoom_factor = 1.0;
      if (TypeInGetTokenType (3) == NUMBER)
        zoom_factor = TypeInGetTokenAsDouble (3);
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))view_zoom,
                             NULL,
                             uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "PANVIEW") == 2) {
      if (TypeInIsKeywordInLine (3, "X") == 3) 
        panview_dir = 0;
      else if (TypeInIsKeywordInLine (3, "Y") == 3) 
        panview_dir = 1;
      else if (TypeInIsKeywordInLine (3, "Z") == 3) 
        panview_dir = 2;
			else {
				ESIHandleCmd("help view");
				fprintf(stderr, "Invalid command syntax\n");
				return;
			}
      if (TypeInGetTokenType (4) == NUMBER)
        panview_shift = TypeInGetTokenAsDouble (4);
			else {
				ESIHandleCmd("help view");
				fprintf(stderr, "Invalid command syntax\n");
				return;
			}
			apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))view_setup_panview,
														 NULL,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "ROTVIEW") == 2) {    /* it is better to precede ROTNOR */
      if (TypeInIsKeywordInLine (3, "X") == 3) 
        rotview_dir = 0;
      else if (TypeInIsKeywordInLine (3, "Y") == 3) 
        rotview_dir = 1;
      else if (TypeInIsKeywordInLine (3, "Z") == 3) 
        rotview_dir = 2;
			else {
				ESIHandleCmd("help view");
				fprintf(stderr, "Invalid command syntax\n");
				return;
			}
      if (TypeInGetTokenType (4) == NUMBER)
        rotview_angle = TypeInGetTokenAsDouble (4);
			else {
				ESIHandleCmd("help view");
				fprintf(stderr, "Invalid command syntax\n");
				return;
			}
			if (TypeInIsKeywordInLine(5, "DEG") == 5)
				rotview_angle *= PI / 180.0;
			apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))view_setup_rotview,
														 NULL,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "ROTNOR") == 2) {
      if (TypeInIsKeywordInLine (3, "X") == 3) 
        rotnor_dir = 0;
      else if (TypeInIsKeywordInLine (3, "Y") == 3) 
        rotnor_dir = 1;
      else if (TypeInIsKeywordInLine (3, "Z") == 3) 
        rotnor_dir = 2;
			else {
				ESIHandleCmd("help view");
				fprintf(stderr, "Invalid command syntax\n");
				return;
			}
      if (TypeInGetTokenType (4) == NUMBER)
        rotnor_angle = TypeInGetTokenAsDouble (4);
			else {
				ESIHandleCmd("help view");
				fprintf(stderr, "Invalid command syntax\n");
				return;
			}
			if (TypeInIsKeywordInLine(5, "DEG") == 5)
				rotnor_angle *= PI / 180.0;
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))view_setup_rotnor,
                             NULL,
                             uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "SAVE") == 2) {
      OnDiskSetFileName(TypeInGetTokenAsString(3));
			apply_command_to_view (save_view_settings,
                             (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "LOAD") == 2) {
      OnDiskSetFileName(TypeInGetTokenAsString(3));
      apply_command_to_view (load_view_settings,
														 (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "REDRAW") > 0) {
      apply_command_to_view (redraw,
														 (caddr_t)&a_vect,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "BGROUND") == 2) {
      EPixel bkg;
      BOOLEAN suc;
      if (TypeInGetTokenType(3) == STRING) {
        bkg = ColorGetPixelFromString(TypeInGetTokenAsString(3), &suc);
        if (suc) {
          apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))set_view_bkg,
																 (caddr_t)bkg,
																 uninstall_apply_to_view);
        }
      }
    } else if (TypeInIsKeywordInLine(2, "AXES") == 2) {
      BOOLEAN on_off;
      on_off = TypeInIsKeywordInLine(3, "ON") == 3;
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVShowAxes,
														 (caddr_t)on_off,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "SCALE") == 2) {
      BOOLEAN on_off;
      on_off = TypeInIsKeywordInLine(3, "ON") == 3;
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVShowScale,
														 (caddr_t)on_off,
														 uninstall_apply_to_view);
    } else if (TypeInIsKeywordInLine(2, "STATUS") == 2) {
      BOOLEAN on_off;
      on_off = TypeInIsKeywordInLine(3, "ON") == 3;
      apply_command_to_view ((void (*)(EView *, caddr_t, WCRec *))EVShowStatus,
														 (caddr_t)on_off,
														 uninstall_apply_to_view);
    }
  }
}


static void change_att(EView *v_p, caddr_t data_p);
static void fast_change_att(EView *v_p, caddr_t data_p);

static long unsigned change_mask = 0x0;

static void set_render_mode(EView *v_p, caddr_t data, WCRec *p);

static ERenderingType the_render_mode = NORMAL_RENDERING;
static EShadingType the_shade_mode = NO_SHADING;


static void 
change_att(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p;
  
  if (EVItemsInSelectionList() <= 0)
    return;

  while ((g_p = (GraphicObj *)EVGetFromSelectionList()) != NULL) 
    EMChangeGraphicAttributes(EVGetModel(v_p), change_mask, g_p);
}


static void 
fast_change_att(EView *v_p, caddr_t data_p)
{
  GraphicObj *g_p;
  
  if (EVItemsInSelectionList() <= 0)
    return;

  while ((g_p = (GraphicObj *)EVGetFromSelectionList()) != NULL) 
		EGSetMarked(g_p, YES);

	EMFastChangeGraphicAttributes(EVGetModel(v_p), change_mask);
}


static void
change_reset(EView *v_p, caddr_t data_p);
static void
change_reset(EView *v_p, caddr_t data_p)
{
  EMUninstallHandler(EVGetModel(v_p));
	EMDispatchToDependentViews(ESIModel(), reset_view, NULL);   /* added by dr to prevent blinking */
}


static void 
change_attribs(void)
{

  if (TypeInGetTokensTotal() > 1) 
    change_mask = mask_from_cl();
  else
    change_mask = EASValsGetChangeMask();

	EMDispatchToDependentViews(ESIModel(), set_view, NULL);   /* added by dr to prevent blinking */

/*  EVSetSelectActionFunction(change_att, NULL, NULL); */
/*	EVSetSelectActionFunction(fast_change_att, NULL, NULL); */
  EVSetSelectActionFunction(fast_change_att, NULL, change_reset);   /* modified by dr to prevent blinking */
	EMPushHandler(ESIModel(), EVSelectGraphicsHandler, NULL);
	ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, "Select graphics");    /* by dr */
}



static long unsigned 
mask_from_cl(void)
{
  unsigned long mask = 0;

  if (TypeInIsKeywordInLine(2, "LAYER") > 0)
    mask |= LAYER_MASK; 
  if (TypeInIsKeywordInLine(2, "STYLE") > 0)
    mask |= STYLE_MASK;
  if (TypeInIsKeywordInLine(2, "COLOR") > 0)
    mask |= COLOR_MASK;
  if (TypeInIsKeywordInLine(2, "EDGECOLOR") > 0)
    mask |= EDGE_COLOR_MASK;
  if (TypeInIsKeywordInLine(2, "EDGEFLAG") > 0)
    mask |= EDGE_FLAG_MASK;
  if (TypeInIsKeywordInLine(2, "FONT") > 0)
    mask |= FONT_MASK;
  if (TypeInIsKeywordInLine(2, "WIDTH") > 0)
    mask |= WIDTH_MASK;
  if (TypeInIsKeywordInLine(2, "MTYPE") > 0)
    mask |= MTYPE_MASK;
  if (TypeInIsKeywordInLine(2, "MSIZE") > 0)
    mask |= MSIZE_MASK;
  if (TypeInIsKeywordInLine(2, "FILL") > 0)
    mask |= FILL_MASK;
  if (TypeInIsKeywordInLine(2, "SHRINK") > 0)
    mask |= SHRINK_MASK;
  if (TypeInIsKeywordInLine(2, "TESSEL") > 0 ||
      TypeInIsKeywordInLine(2, "TESSELATION") > 0)
    mask |= TESSEL_INTERVALS_MASK;
  if (TypeInIsKeywordInLine(2, "PSHOW") > 0)
    mask |= SHOW_POLY_MASK; 
  if (TypeInIsKeywordInLine(2, "ESHOW") > 0)
    mask |= SHOW_ENTITY_MASK;
  if (TypeInIsKeywordInLine(2, "NOTCLIPPABLE") > 0)
    mask |= NOT_CLIPPABLE_MASK;
  if (TypeInIsKeywordInLine(2, "INVISWHENCLIPPED") > 0)
    mask |= INVISIBLE_WHEN_CLIPPED_MASK;
  if (TypeInIsKeywordInLine(2, "UTESSEL") > 0)
    mask |= TESSEL_U_MASK;
  if (TypeInIsKeywordInLine(2, "VTESSEL") > 0)
    mask |= TESSEL_V_MASK;
  if (TypeInIsKeywordInLine(2, "VECTYPE") > 0)
    mask |= VEC_TYPE_MASK;
  if (TypeInIsKeywordInLine(2, "VECSCALE") > 0)
    mask |= VEC_SCALE_MASK;
  if (TypeInIsKeywordInLine(2, "VECRATE") > 0)
    mask |= VEC_RATE_MASK;
  if (TypeInIsKeywordInLine(2, "VECSHIFT") > 0)
    mask |= VEC_SHIFT_MASK;
  if (TypeInIsKeywordInLine(2, "TEXT") > 0)
    mask |= TEXT_STRING_MASK;
    
  if (TypeInIsKeywordInLine(2, "CLEAR") > 0)
    mask = 0;
  
  return mask;
}



static void 
set_change_mask(void)
{
  EASValsSetChangeMask(mask_from_cl());
}



static void set_active_fill(void)
{
  int fill = FILL_SOLID;
  
  if      (   TypeInIsKeywordInLine(2, "SOLID") > 0
           || TypeInIsKeywordInLine(2, "ON") > 0)  fill = FILL_SOLID;
  else if (   TypeInIsKeywordInLine(2, "OFF") > 0 
	   || TypeInIsKeywordInLine(2, "HOLLOW") > 0) fill = FILL_HOLLOW;
  else
    return;
  EASValsSetFillStyle(fill);
}



static void 
set_active_edgeflag(void)
{
  int ef = NO;
  
  if      (TypeInIsKeywordInLine(2, "ON") > 0)  ef = YES;

  EASValsSetEdgeFlag(ef);
}



static void 
set_active_shrink(void)
{
  double s;

  if (TypeInGetTokenType(2) != NUMBER) {
    TypeInBadToken(2, "NUMBER");
    return;
  }
  s = TypeInGetTokenAsDouble(2);
  if (s < 0 || s > 1) s = 1;
  EASValsSetShrink(s);
}



static void 
set_active_scale(void)
{
  int pos;

  pos = TypeInIsKeywordInLine(2, "ISO");
  if (pos > 0) {
    if (TypeInGetTokenType(pos+1) != NUMBER){
      TypeInBadToken(pos+1, "NUMBER"); return;
    }
    EASValsSetIsotropicScale(TypeInGetTokenAsDouble(pos+1));
  } else {
    if (TypeInGetTokenType(2) != NUMBER) {TypeInBadToken(2, "NUMBER"); return;}
    if (TypeInGetTokenType(3) != NUMBER) {TypeInBadToken(3, "NUMBER"); return;}
    if (TypeInGetTokenType(4) != NUMBER) {TypeInBadToken(4, "NUMBER"); return;}
    EASValsSetScaleX(TypeInGetTokenAsDouble(2));
    EASValsSetScaleY(TypeInGetTokenAsDouble(3));
    EASValsSetScaleZ(TypeInGetTokenAsDouble(4));
  }
}



static VCRec dir_to_light = {1, 1, 1};

static void set_dir_to_light(EView *v_p, caddr_t data, WCRec *p);

static void 
set_dir_to_light(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetDirTowardsLight(v_p, &dir_to_light);
}

/*
static void set_render_mode(EView *v_p, caddr_t data, WCRec *p);

static ERenderingType the_render_mode = NORMAL_RENDERING;
static EShadingType the_shade_mode = NO_SHADING;
*/

static void 
set_render_mode(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetRenderMode(v_p, the_render_mode);
  EVSetShadeMode(v_p, the_shade_mode);
}


static void 
set_render_shade(void)
{
  int pos;

  the_shade_mode = NO_SHADING;
  if (TypeInIsKeywordInLine(2, "NORMAL") > 0) {
    the_render_mode = NORMAL_RENDERING;
		apply_command_to_view (set_render_mode, NULL, uninstall_apply_to_view);
  } else if ((pos = TypeInIsKeywordInLine(2, "LDIR")) > 0) {
    dir_to_light.u = TypeInGetTokenAsDouble(pos+1);
    dir_to_light.v = TypeInGetTokenAsDouble(pos+2);
    dir_to_light.n = TypeInGetTokenAsDouble(pos+3);
		apply_command_to_view (set_dir_to_light, NULL, uninstall_apply_to_view);
  } else if ((pos = TypeInIsKeywordInLine(2, "AMBIENT")) > 0) {
    ColorSetAmbientGrey(TypeInGetTokenAsDouble(3));
    return; /* NO HANDLER TO USE */
  } else {
    if (TypeInIsKeywordInLine(2, "SHADE") > 0)  {
      the_render_mode = CONST_SHADING_RENDERING;
      if (TypeInIsKeywordInLine(2, "DITHER") > 0)
				the_shade_mode = DITHER_SHADING;
      else
				the_shade_mode = COLOR_SHADING;
    } else if (TypeInIsKeywordInLine(2, "FILL") > 0)  {
      the_render_mode = FILLED_HIDDEN_RENDERING;
    } else if (TypeInIsKeywordInLine(2, "WIRE") > 0)  {
      the_render_mode = WIRE_RENDERING;
    }
		apply_command_to_view (set_render_mode, NULL, uninstall_apply_to_view);
  }
}


static void 
set_fringe_table(void)
{
  double low = -1, high = 1;

  if (TypeInGetTokenType(2) == NUMBER && TypeInGetTokenType(3) == NUMBER) {
    low  = TypeInGetTokenAsDouble(2);
    high = TypeInGetTokenAsDouble(3);
    ColorSetupFringeTableByMinMax(EMGetAssocFringeTable(ESIModel()),
                                  low, high);
  } else {
    char buf[132];

    ColorFringesMinMax(EMGetAssocFringeTable(ESIModel()), &low, &high);
    sprintf(buf, "Fringe table low = %g, high = %g", low, high);
    ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, buf);
  }
}

static void 
new_frame(void)
{
  EView *v_p = NULL;
  
  new_frame_CB(widget, (XtPointer)0, (XtPointer)&v_p);
  the_render_mode = NORMAL_RENDERING;
  the_shade_mode  = NO_SHADING;
  if (TypeInIsKeywordInLine(2, "SHADE") > 0)  {
    the_render_mode = CONST_SHADING_RENDERING;
    if (TypeInIsKeywordInLine(2, "DITHER") > 0)
      the_shade_mode = DITHER_SHADING;
    else
      the_shade_mode = COLOR_SHADING;
  } else if (TypeInIsKeywordInLine(2, "WIRE") > 0)  {
    the_render_mode = WIRE_RENDERING;
  }
  EVSetRenderMode(v_p, the_render_mode);
  EVSetShadeMode(v_p, the_shade_mode);
}



static void 
save_view_settings(EView *v_p, caddr_t f, WCRec *s)
{
  FILE *fp;
  WCRec p;
  float x, y, z;
  
  OnDiskEmptyAGEFile();
  if ((fp = OnDiskOpenAGEFile("w")) == NULL)
    return;

  EVGetOriginVC(v_p, &p);
  x = p.x, y = p.y, z = p.z;
  fprintf(fp, "%f %f %f\n", x, y, z);
  EVGetVectUp(v_p, &p);
  x = p.x, y = p.y, z = p.z;
  fprintf(fp, "%f %f %f\n", x, y, z);
  EVGetNormal(v_p, &p);
  x = p.x, y = p.y, z = p.z;
  fprintf(fp, "%f %f %f\n", x, y, z);
  EVGetVRP(v_p, &p);
  x = p.x, y = p.y, z = p.z;
  fprintf(fp, "%f %f %f\n", x, y, z);
  EVGetDimsVC(v_p, &p.x, &p.y);
  x = p.x, y = p.y, z = p.z;
  fprintf(fp, "%f %f\n", x, y);
  
  OnDiskCloseAGEFile();
  EMUninstallHandler(EVGetModel(v_p)); /* do it for one view only. */
}



static void 
load_view_settings(EView *v_p, caddr_t f, WCRec *s)
{
  FILE *fp;
  float x, y, z;
  WCRec p;
  
  if ((fp = OnDiskOpenAGEFile("r")) == NULL)
    return;

  fscanf(fp, "%f %f %f", &x, &y, &z);
  p.x = x, p.y = y, p.z = z;
  EVInhibitViewOrientationSetup(); EVSetOriginVC(v_p, &p);
  fscanf(fp, "%f %f %f", &x, &y, &z);
  p.x = x, p.y = y, p.z = z;
  EVInhibitViewOrientationSetup(); EVSetVectUp(v_p, &p);
  fscanf(fp, "%f %f %f", &x, &y, &z);
  p.x = x, p.y = y, p.z = z;
  EVInhibitViewOrientationSetup(); EVSetNormal(v_p, &p);
  fscanf(fp, "%f %f %f", &x, &y, &z);
  p.x = x, p.y = y, p.z = z;
  EVInhibitViewOrientationSetup(); EVSetVRP(v_p, &p);
  fscanf(fp, "%f %f", &x, &y);
  EVSetDimsVC(v_p, x, y);
  
  OnDiskCloseAGEFile();
  EVSetupViewOrientation(v_p);	/* This call resets the inhibit-flag  */
  EVSetupViewOrientation(v_p);
}

  
static void 
apply_set_constrpl_center(EView *v_p, caddr_t data, WCRec *p);
static void 
apply_set_constrpl_orientation(EView *v_p, caddr_t data, WCRec *p);

static WCRec constrpl_orientation_normal;
static WCRec constrpl_orientation_vup;
static WCRec constrpl_orientation_center;

static void 
apply_set_constrpl_center(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetConstrPlaneCenter(v_p, &constrpl_orientation_center);
}

static void 
apply_set_constrpl_orientation(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetConstrPlaneOrientation(v_p,
                              &constrpl_orientation_normal,
                              &constrpl_orientation_vup);
}

static void 
set_constrplane(void)
{
  if (TypeInIsKeywordInLine(2, "CENTER") > 0) {
    if (TypeInGetTokenType(3) == NUMBER &&
				TypeInGetTokenType(4) == NUMBER &&
				TypeInGetTokenType(5) == NUMBER) {
      constrpl_orientation_center.x = TypeInGetTokenAsDouble(3);
      constrpl_orientation_center.y = TypeInGetTokenAsDouble(4);
      constrpl_orientation_center.z = TypeInGetTokenAsDouble(5);
    }
		apply_command_to_view (apply_set_constrpl_center, NULL, uninstall_apply_to_view);
  } else if (TypeInIsKeywordInLine(2, "ORIENTATION") > 0) {
    if (TypeInGetTokenType(3) == NUMBER &&
				TypeInGetTokenType(4) == NUMBER &&
				TypeInGetTokenType(5) == NUMBER) {
      constrpl_orientation_normal.x = TypeInGetTokenAsDouble(3);
      constrpl_orientation_normal.y = TypeInGetTokenAsDouble(4);
      constrpl_orientation_normal.z = TypeInGetTokenAsDouble(5);
    }
    if (TypeInGetTokenType(6) == NUMBER &&
				TypeInGetTokenType(7) == NUMBER &&
				TypeInGetTokenType(8) == NUMBER) {
      constrpl_orientation_vup.x = TypeInGetTokenAsDouble(6);
      constrpl_orientation_vup.y = TypeInGetTokenAsDouble(7);
      constrpl_orientation_vup.z = TypeInGetTokenAsDouble(8);
    }
		apply_command_to_view (apply_set_constrpl_orientation, NULL, uninstall_apply_to_view);
  } else
    return;
}


static void
help_me_please(void)
{
  int i, n = XtNumber(commands);
  char *s;
  top_command *cmd;

  if (TypeInGetTokensTotal() > 1) {
    s = TypeInGetTokenAsString(2);
    cmd = TypeInGetCommandData(s);
    if (cmd != NULL) {
      fprintf(stderr, "\nSyntax of command %s\n", cmd->keyword);
      fprintf(stderr, 
							"\nNotation: \t[] ...optional item,\n "
              "          \t{s1|s2|..|sn} ...one of s1, s2,...,sn,\n"
              "          \t# ... number\n");
      fprintf(stderr, "%s %s\n", cmd->keyword, cmd->syntax);
    } else {
      fprintf(stderr, "\n    No such command %s\n", cmd->keyword);
    }
    fprintf(stderr, "\n ");
  } else {
    fprintf(stderr,
            "=============>    Available commands:   <=============\n");
    for (i = 0; i < n; i++) {
      fprintf(stderr, "\t%s", commands[i].keyword);
      if (i/6*6 == i) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n  Use command \"HELP keyword\" to get syntax. \n");
  }
}


static void
graphic_info(void)
{
  ginfo_CB(NULL, NULL, NULL);
}



int
ps_dump(EView *v_p, char *fname,
        double width_in_cm, double height_in_cm,
        BOOLEAN portrait, BOOLEAN encapsulated_ps
        );

static char psdump[256];
static double psdump_x_cm = 0, psdump_y_cm = 0;
static BOOLEAN encapsulated_ps = NO;
static BOOLEAN portrait = NO;

static void 
apply_postscript_dump(EView *v_p, char *file, WCRec *p)
{
  ps_dump(v_p, psdump, psdump_x_cm, psdump_y_cm, portrait, encapsulated_ps);
}


static void
postscript_dump(void)
{
  FILE *fp;
  char *name = NULL;

  strcpy(psdump, "!");
  
  if (TypeInGetTokensTotal() < 2) {
    fp = ESISelFile("Select file name for the Postscript dump",
                    "   OK   ", "CANCEL", NULL, NULL, "w", &name);
    if (fp) {
      strcpy(psdump, name);
      fclose(fp);
    }
    psdump_x_cm = psdump_y_cm = 15;
  } else {
    if (TypeInGetTokenType(2) == STRING) {
      strcpy(psdump, TypeInGetTokenAsString(2));
      if (   TypeInGetTokenType(3) == NUMBER
          && TypeInGetTokenType(4) == NUMBER) {
      psdump_x_cm = TypeInGetTokenAsDouble(3);
      psdump_y_cm = TypeInGetTokenAsDouble(4);
      } else {
        psdump_x_cm = psdump_y_cm = 10;
      }
    }
  }
  
  portrait = (TypeInIsKeywordInLine(2, "PORTRAIT") > 0);
  encapsulated_ps = (   TypeInIsKeywordInLine(2, "EPS") > 0
                     || TypeInIsKeywordInLine(2, "ENCAPSULATED") > 0);
  
  if (!(psdump[0] == '!')) {
    EVSetApplyToViewFunction(apply_postscript_dump, (caddr_t)psdump,
                             uninstall_apply_to_view);
    EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
  }
}



static WCRec  mclip_plane_center = {0, 0, 0};
static WCRec  mclip_plane_normal = {0, 0, 1};
static BOOLEAN mclip_plane_on = NO;

static void set_mclip_params(EView *v_p, caddr_t data, WCRec *p);

static void 
set_mclip_params(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetModelClipPlane(v_p, 0, &mclip_plane_center, &mclip_plane_normal,
                      mclip_plane_on);
  EVSetModelPlaneClipOnOff(v_p, mclip_plane_on);
}


static void
set_mclip(void)
{
  int pos;
  if (TypeInIsKeywordInLine(2, "ON") > 0)
    mclip_plane_on = YES;
  if (TypeInIsKeywordInLine(2, "OFF") > 0)
    mclip_plane_on = NO;
  if ((pos = TypeInIsKeywordInLine(2, "CENTER")) > 0) {
		if (TypeInGetTokenType(pos + 1) == NUMBER &&
				TypeInGetTokenType(pos + 1) == NUMBER &&
				TypeInGetTokenType(pos + 3) == NUMBER){
			mclip_plane_center.x = TypeInGetTokenAsDouble(pos+1);
			mclip_plane_center.y = TypeInGetTokenAsDouble(pos+2);
			mclip_plane_center.z = TypeInGetTokenAsDouble(pos+3);
		}
		else
			unset_target_view ();

		if(mclip_plane_on == NO){
			EVSetApplyToViewPreventRedisplay(YES);
			prevent_force_redisplay_target_view();
		}
  }
  if ((pos = TypeInIsKeywordInLine(2, "NORMAL")) > 0) {
    mclip_plane_normal.x = TypeInGetTokenAsDouble(pos+1);
    mclip_plane_normal.y = TypeInGetTokenAsDouble(pos+2);
    mclip_plane_normal.z = TypeInGetTokenAsDouble(pos+3);

		if(mclip_plane_on == NO){
			EVSetApplyToViewPreventRedisplay(YES);
			prevent_force_redisplay_target_view();
		}
  }

  apply_command_to_view (set_mclip_params, NULL, uninstall_apply_to_view);
}


static FPNum vclip_bg_depth = -FLT_MAX;
static FPNum vclip_fg_depth = FLT_MAX;
static BOOLEAN vclip_on = NO;
static BOOLEAN vclip_bg = NO, vclip_fg = NO;

static void set_vclip_params(EView *v_p, caddr_t data, WCRec *p);

static void 
set_vclip_params(EView *v_p, caddr_t data, WCRec *p)
{
	if(vclip_bg == YES && vclip_fg == YES)
		EVSetFGBGViewPlaneDepth(v_p, vclip_fg_depth, vclip_bg_depth);
	else{
		if(vclip_bg == YES)
			EVSetBGViewPlaneDepth(v_p, vclip_bg_depth);
		if(vclip_fg == YES)
			EVSetFGViewPlaneDepth(v_p, vclip_fg_depth);
	}

	EVSetFGBGViewPlaneClipOnOff(v_p, vclip_on);
}



static void
set_vclip(void)
{
  int pos;
  if (TypeInIsKeywordInLine(2, "ON") > 0)
    vclip_on = YES;
  if (TypeInIsKeywordInLine(2, "OFF") > 0)
    vclip_on = NO;

	vclip_bg = vclip_fg = NO;

  if ((pos = TypeInIsKeywordInLine(2, "BGDEPTH")) > 0){
		vclip_bg_depth = (FPNum)TypeInGetTokenAsDouble(pos+1);
		vclip_bg = YES;
	}
  if ((pos = TypeInIsKeywordInLine(2, "FGDEPTH")) > 0){
		vclip_fg_depth = (FPNum)TypeInGetTokenAsDouble(pos+1);
		vclip_fg = YES;
	}
	
	if(vclip_bg == YES || vclip_fg == YES){
		if(vclip_on == NO){
			EVSetApplyToViewPreventRedisplay(YES);
			prevent_force_redisplay_target_view();
		}
	}

  apply_command_to_view (set_vclip_params, NULL, uninstall_apply_to_view);
}


static void 
set_active_u_tesseli(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetUTesselIntervals(TypeInGetTokenAsInt(2));
  }
}

static void 
set_active_v_tesseli(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetVTesselIntervals(TypeInGetTokenAsInt(2));
  }
}

static void 
redraw (EView *v_p, caddr_t data, WCRec *p)
{
  EVFastRedraw (v_p);
}



static void 
set_vec_type(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetVecMType(TypeInGetTokenAsInt(2));
  }
}


static void 
set_vec_scale(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetVecScale(TypeInGetTokenAsDouble(2));
  }
}


static void 
set_vec_rate(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetVecRate(TypeInGetTokenAsDouble(2));
  }
}


static void 
set_vec_shift(void)
{
  if (TypeInGetTokenType(2) != NUMBER)
    TypeInBadToken(2, "NUMBER");
  else {
    EASValsSetVecShiftFlag((BOOLEAN)TypeInGetTokenAsInt(2));
  }
}


static int
redraw_view(NODE data, NODE v_p)
{
	EVFastRedraw((EView *)v_p);
	return(1);
}


static void
isolines_setup(void)
{
	BOOLEAN on_off;
	double min_val, max_val;
	int width, count, scale;

	if(TypeInGetTokenType(3) == STRING){
		if(TypeInIsKeywordInLine(3, "ON") == 3){
			on_off = YES;
		}else if(TypeInIsKeywordInLine(3, "OFF") == 3){
			on_off = NO;
		}else{
			if(TypeInIsKeywordInLine(2, "BGCOLOR") == 2){
				SetContourBgColor(TypeInGetTokenAsString(3));
				if(IsFringeTableContourScale == NO)return;
			}
			else
				return;
		}
		
		if(TypeInIsKeywordInLine(2, "COLOR") == 2){
			if(on_off == YES)
				SwitchFringeTableToColor();
			else
				SwitchFringeTableToGrey();
		} else if (TypeInIsKeywordInLine(2, "SMOOTH") == 2){
			if(on_off == YES)
				SetFringeTableSmoothScaleOn();
			else
				SetFringeTableSmoothScaleOff();
		} else if (TypeInIsKeywordInLine(2, "CONTOUR") == 2){
			if(on_off == YES)
				SetFringeTableContourScaleOn();
			else
				SetFringeTableContourScaleOff();
		} else if (TypeInIsKeywordInLine(2, "TRANSPARENT") == 2){
			SetContourBgTransparent(on_off);
			if(IsFringeTableContourScale == NO)return;
		}
	}
	else{
		if(TypeInGetTokenType(3) == NUMBER){
			if (TypeInIsKeywordInLine(2, "RANGE") == 2){
				if(TypeInGetTokenType(4) == NUMBER){
					min_val = TypeInGetTokenAsDouble(3);
					max_val = TypeInGetTokenAsDouble(4);
					ColorSetupFringeTableByMinMax(EMGetAssocFringeTable(ESIModel()), min_val, max_val);
				}
			} else if (TypeInIsKeywordInLine(2, "WIDTH") == 2){
				width = TypeInGetTokenAsInt(3);
				SetContourWidth(width);
				if(IsFringeTableContourScale == NO)return;
			} else if (TypeInIsKeywordInLine(2, "COUNT") == 2){
				count = TypeInGetTokenAsInt(3);
				SetContourCount(count);
				if(IsFringeTableContourScale == NO)return;
			} else if (TypeInIsKeywordInLine(2, "SCALE") == 2){
				scale = TypeInGetTokenAsInt(3);
				SetColorScale(scale);
			}
		}
		else{
			if(TypeInIsKeywordInLine(2, "REVERT") == 2){
				RevertFringeTableColorScale();
			} else if (TypeInIsKeywordInLine(2, "RANGE") == 2){
				char buffer[32];
				ColorFringesMinMax(EMGetAssocFringeTable(ESIModel()), &min_val, &max_val);
				sprintf(buffer, "Min: %e | Max: %e\n", min_val, max_val);
				ERptStatusMessage(ELIXIR_MESSAGE_CLASS, 1, buffer);
			}
		}
	}

	EMDispatchToDependentViews(ESIModel(), redraw_view, NULL);
}

