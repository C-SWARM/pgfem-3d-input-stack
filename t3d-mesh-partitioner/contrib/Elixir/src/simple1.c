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

#include <string.h>

#include "Econfig.h"
#include "Eutils.h"

#include <stdio.h>
#include <errno.h>		/* This is because of errno, pk. */


#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>

#include <X11/Xaw/Cardinals.h>

#define SIMPLE_PRIVATE_HEADER 
#include "Esimple.h"          


/* The widgets used here */
static   Widget /* ESITopLevelWidget(), */
                   topPaned,
                      copyright, /* shows copyright notice */
                      cmd_form,   /* holds the command  */
                         cmd_buttons_paned,
                            executeCmd, /* execute current command */
                            clearCmd,   /* clear the command text widget */
                            help,       /* help on command */
                         cmdtext,
                         history,
                      palettes_box, /* holds the palette buttons */
                         file_button, /* opens the file menu */
                         view_button, /* opens the view palette */
                         edit_button, /* opens the edit palette */
                         draw_button, /* opens the draw palette */
                         actset_button, /* opens the actset palette */
                         locks_button, /* opens the locks palette */
                         precinput_button, /* opens the locks palette */
                      status,   /* text field: shows status */
                      track,    /* text field: tracks the pointer */
                      prompt,   /* text field: shows the prompt */
                   file_menu,
                      load, 
                      save, 
                      saveas, 
                      psdump, 
                      quit,
                   view_palette,
                      newframe,
                      redraw,
                      fit_all,
                      stdview_button, /* opens the stdview view menu */
                      viewclip_button, /* opens the viewclip palette */
                      modelclip_button, /* opens the modelclip palette */
                      layers_button, /* opens the layers palette */
                      render_button, /* opens the render palette */
                      color_scale_button,
                   stdview_menu,
                   viewclip_palette,
                   modelclip_palette,
                   layers_palette,
	                    layers_form,
                      layers_apply,
                   render_palette,
                   color_scale_palette,
                   edit_palette,
                      undo,
                      redo,
                      change_button, /* opens the change palette */
                      copy_or_orig, /* move type toggle */
                      reshape,
                      translate,
                      scale,
                      mirror_center,
                      mirror_plane,
                      rotate_by_point,
                      rotate_by_angle,
                      ggroup,
                      delete,
                      ginfo,
                      oogl_export,
                   change_palette,
                   ggroup_menu,
                   draw_palette,
                      line,
                      marker,
                      cbezc,
                      bcbezs,
                      triangle,
                      quadrangle,
                      tetrahedron,
                      hexahedron,
                      bcbhexahedron,
                      anntext,
                      raprism,
                      pyramid,
                   bcbezs_menu,
                      bcbezs_by_points,
                      bcbezs_from_curves,
                      bcbezs_extrude_curve_along_curve,
                      bcbezs_extrude_curve_by_delta,
                   hexahedron_menu,
                      hexa_from_curves,
                      hexa_extrude_patch_along_curve,
                      hexa_extrude_patch_by_delta,
                      hexa_set_extrusion_delta,
                   actset_palette,
                      layer, 
                      edgecolor, 
                      edgeflag, 
                      color, 
                      style, 
                      width, 
                      font, 
                      fill, 
                      text,
                      msize, 
                      mtype, 
                      keyp, 
                      shrink, 
                      rangle, 
	                    vectype,
	                    vecscale,
	                    vecrate,
	                    vecshift,
                      tessel, 
	                    utessel,
	                    vtessel,
                      selcrit,
                      pshow,
                      notclippable,
                      inviswhenclipped,
                   locks_palette,
                      lock_grid,
                      lock_cplane,
                      lock_setup_cplane_button,
                      locks_form,
                         locks_u,
                         locks_v,
                         locks_du,
                         locks_dv,
                      locks_apply,
                   lock_setup_cplane_palette,
                   precinput_palette,
                      precinput_form,
                         precinput_x,
                         precinput_y,
                         precinput_z,
                         precinput_xyz_ok,
                         precinput_dx,
                         precinput_dy,
                         precinput_dz,
                         precinput_dxyz_ok,
                   custom_palette /* the customization palette */
; /* end of widgets */


/* translations */
static void 
ReturnHitInCmd(Widget w, XEvent *event, String *params,
               Cardinal *num_params);

static void 
ReturnHitInXYZ(Widget w, XEvent *event, String *params,
                       Cardinal *num_params);

static XtTranslations tt1;
static XtActionsRec remap_return[] = {
  {"retActCmd", ReturnHitInCmd },
};

static XtTranslations tt3;
static XtActionsRec remap_return_XYZ[] = {
  {"retActXYZOK", ReturnHitInXYZ },
};



/* Fallback resources */
String fallback_resources[] = {
  "SimpleXF*Font:                        *-helvetica-*-r-*",
  "SimpleXF*Background:                  peachpuff",
  "SimpleXF*Foreground:                  black",
  "SimpleXF*Command.Background:          #cccccc",
  "SimpleXF*Toggle.Background:           lightblue",
  "SimpleXF*SimpleMenu.Background:       #cccccc",
  "SimpleXF*MenuButton.Background:       #cccccc",
  "SimpleXF*close.Background:            #cccc80",
  "SimpleXF*quit.Background:             LightSalmon",
  "SimpleXF*input:                       True",
  "SimpleXF*showGrip:                    False",
  "SimpleXF*cmdtext*preferredPaneSize:      40",
  "SimpleXF*cmdtext*autoFill:               False",
  "SimpleXF*cmdtext*editType:               edit",
/*  "SimpleXF*cmdtext*scrollVertical:         off", */
  "SimpleXF*cmdtext*scrollHorizontal:       whenNeeded",
  "SimpleXF*status*preferredPaneSize:    120",
  "SimpleXF*status*showGrip:             on",
/*  "SimpleXF*status*editType:             off", */
  "SimpleXF*status*scrollVertical:       whenNeeded",
  "SimpleXF*status*scrollHorizontal:     whenNeeded",
  "SimpleXF*track*preferredPaneSize:     30",
  "SimpleXF*prompt*preferredPaneSize:    30",
  "SimpleXF*history*preferredPaneSize:   40",
  "SimpleXF*history*scrollVertical:      whenNeeded",
  "SimpleXF*history*scrollHorizontal:    whenNeeded",
  NULL
};
 

#define START_UP_MESSAGE ""
/*"Type command here. Press \"EXECUTE COMMAND\" or hit return." */
#define TRACK_HEADER "Pointer at"
#define STATUS_HEADER "STATUS> "
#define PROMPT_HEADER "PROMPT> "

/* Maximum history buffer size */
#define MAX_HBL 1024



static void
ClearText(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);
static void
ExecuteCommand(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);
static void 
Quit(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);
static void 
HelpMe(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);
static void 
InitCommand(void);
static void
PrintPointerLoc(const char *message);
static void
PrintPrompt(const char *message);
static int
StatusMessage(int class, int serial, const char *message);
static void 
TrackPointer(WCRec *loc);
static int
PromptMessage(int class, int serial, const char *message);
int
ErrorMessage(int class, int serial,
             const char *deflt_msg, EErrGrade grade);
static void
PrintStatus(const char *message);
static void
PrintHelp(void);
static void
add_to_history(char *s);
static void
repeat_from_history(void);

static void
GraphicEditor(Widget parent_paned, unsigned long mask);

void
handle_command(Widget w, char *s);

void 
scale_orig_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
scale_copy_CB(Widget w, XtPointer client_data, XtPointer call_data);


 
/* ======================================================================== */
/* PRIVATE */
/* ======================================================================== */

static void
ClearText(Widget w, XtPointer cmdtext_ptr, XtPointer call_data)
{
    Widget cmdtext = *((Widget *)cmdtext_ptr);
    Arg args[1];

    XtSetArg(args[0], XtNstring, "");
    XtSetValues(cmdtext, args, ONE);
}

static void
exec_cmd(char *str);


static void 
ReturnHitInCmd(Widget w, XEvent *event, String *params,
               Cardinal *num_params)
{
  Arg args[1];
  String str;

  XtSetArg(args[0], XtNstring, &str);
  XtGetValues(w, args, ONE);

  exec_cmd(str);

  XtSetArg(args[0], XtNstring, "");
  XtSetValues(w, args, ONE);
}


static void
ExecuteCommand(Widget w, XtPointer cmdtext_ptr, XtPointer call_data)
{
  Widget cmdtext = *((Widget *)cmdtext_ptr);
  Arg args[1];
  String str;
  
  XtSetArg(args[0], XtNstring, &str);
  XtGetValues(cmdtext, args, ONE);

  exec_cmd(str);
  
  ClearText(w, cmdtext_ptr, call_data);
}


static int
non_blank(char *s); 

static int
non_blank(char *s)
{
  while (*(s) != '\0') {
    if (*s != ' ')
      return 1;
    s++;
  }
  return 0;
}


static void
exec_cmd(char *str)
{
  char *ptr;
  static int first_time = 1;
  
  /*  Eliminate newlines from the message  */
  ptr = (char *) str;
  while( *ptr != '\0' ) {
    if ( *ptr == '\n' )
      *ptr = ' ';
    ptr++;
  }
  
  if (first_time == 1){
    InitCommand();
    first_time = 0;
  }
  if (non_blank(str)) {
    add_to_history(str);
    handle_command(ESITopLevelWidget(), str);
  } else {
    repeat_from_history();
  }
}


static void
add_to_history(char *s)
{
  int ac, hbl = 0;
  Arg al[1];
  char *str = NULL;
  char buff[MAX_HBL];
	static BOOLEAN first = YES;

/* order in which commands appear in the history is reversed - by dr */

	sprintf(buff, "%s\n", s);
	hbl = strlen(buff);

	if(first == YES)
		first = NO;
	else{
		ac = 0;
		XtSetArg(al[ac], XtNstring, &str); ac++;
		XtGetValues(history, al, ac);
		if(str != NULL){
			strncpy(&buff[hbl], str, MAX_HBL - hbl - 1);
			buff[MAX_HBL - 1] = '\0';

/* cut off the last incomplete command */

			str = &buff[strlen(buff)];
			while (*--str != '\n');
			*++str = '\0';
		}
	}
  ac = 0;
  XtSetArg(al[ac], XtNstring, buff); ac++;
  XtSetValues(history, al, ac);
}

static void
repeat_from_history(void)
{
  int ac = 0, i;
  Arg al[1];
  char buff[MAX_HBL] = {' '};
  char *ptr;

  ac = 0;
  XtSetArg(al[ac], XtNstring, &ptr); ac++;
  XtGetValues(history, al, ac);

  for (i = 0; i < MAX_HBL; i++) buff[i] = ' ';
  i = 0;
  while (*ptr != '\0') {
    buff[i++] = *ptr;
    if (*ptr == '\n') {
      buff[--i] = '\0';
      handle_command(ESITopLevelWidget(), buff);
      break;
    }
    ptr++;
  }
}

static void 
do_quit(XtPointer ptr)
{
  XtCloseDisplay(XtDisplay(ESITopLevelWidget()));
  exit(ESISetExitCode(ELIXIR_OK_EXIT));
}

static void 
Quit(Widget w, XtPointer ptr, XtPointer call_data)
{
  ESIPopupConfirmDialog((Widget)ptr, "Really quit?", do_quit, ptr, NULL, NULL);
}




static void 
InitCommand(void)
{
  handle_command(ESITopLevelWidget(), " ");
}



static int
StatusMessage(int class, int serial, const char *message)
{
  PrintStatus(message);
  return 1;
}



static void
PrintStatus(const char *message)
{
  Arg args[1];
  int argn;
  if (message != NULL) {
    argn = 0;
    XtSetArg(args[argn], XtNstring, message); argn++;
    XtSetValues(status, args, argn);
  }
  ESIEventLoop(NO, NULL);
}


static void
PrintPointerLoc(const char *message)
{
  Arg args[1];
  int argn;

  argn = 0;
  XtSetArg(args[argn], XtNstring, message); argn++;
  XtSetValues(track, args, argn);
}


static void
PrintPrompt(const char *p)
{
  Arg args[1];
  int argn;

  if (p != NULL) {
    argn = 0;
    XtSetArg(args[argn], XtNstring, p); argn++;
    XtSetValues(prompt, args, argn);
  } 
}


static void 
TrackPointer(WCRec *loc)
{
  char message[132];

  sprintf (message, "Pointer at [%f;%f;%f]", loc->x, loc->y, loc->z);
  PrintPointerLoc(message);
}



static int
PromptMessage(int class, int serial, const char *message)
{
  PrintPrompt(message);
  return 1;
}




static void 
HelpMe(Widget w, XtPointer cmdtext_ptr, XtPointer call_data)
{
  static int first_time_help = 1;
  top_command help_command = {   "HELP", 4, PrintHelp,
                                   "[keyword] (type HELP when at a loss)"};

  if (first_time_help) {
    TypeInReplaceCommand(&help_command);
    first_time_help = 0;
  }
  handle_command(ESITopLevelWidget(), "HELP");
}




static char buffer[1024];

static void
PrintHelp(void)
{
  int i, n;
  char *s;
  top_command *cmd;
  char cmd_name[32];

  strcpy(buffer, "Help");
  if (TypeInGetTokensTotal() > 1) {
    s = TypeInGetTokenAsString(2);
    cmd = TypeInGetCommandData(s);
    if (cmd != NULL) {
      sprintf(buffer,
              "Syntax of command %s:\n%s %s\n%s\n", cmd->keyword,
              cmd->keyword, cmd->syntax,
              "\nNotation: \t[] ...optional item,\n"
              "          \t{s1|s2|..|sn} ...one of s1, s2,...,sn,\n"
              "          \t# ... number\n");
    } else {
      sprintf(buffer, "\n    No such command %s\n", s);
    }
  } else {
    n = TypeInCommandTableSize();
    cmd = TypeInAugmentCommandTable(NULL, 0);
    if (cmd != NULL) {
      sprintf(buffer, "%s\n%s\n",
              "=============>    Available commands:   <=============",
              "    Use command \"HELP keyword\" to get syntax. ");
      for (i = 0; i < n; i++) {
				if(cmd[i].keyword == NULL)continue;
        sprintf(cmd_name, "%12s", cmd[i].keyword);
        strcat(buffer, cmd_name); 
        if (i/4*4 == i) strcat(buffer, "\n");
      }
    }  else {
      sprintf(buffer, "%s\n",
              "=============>    No available commands!?");
    }
  }
  PrintStatus(buffer);
}


#if 0

void
ESICustomize(Widget parent_paned)
{
  /* VOID: It is up to the programmer who wants to customize */
  /* the simple interface to define any additional buttons under custom button. */
}

#endif





static void 
fit_into_a_view_CB(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);

static void 
fit_into_a_view_CB(Widget w, XtPointer cmdtext_ptr, XtPointer call_data)
{
  handle_command(ESITopLevelWidget(), "FIT");
}

 
static int move_copy_or_orig = MOVE_COPY;

static void 
set_move_copy_or_orig(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);

static void 
set_move_copy_or_orig(Widget w, XtPointer cmdtext_ptr, XtPointer call_data)
{
  if (move_copy_or_orig == MOVE_COPY)
    move_copy_or_orig = MOVE_ORIGINAL;
  else
    move_copy_or_orig = MOVE_COPY;
}

static void 
start_translate(Widget w, XtPointer ptr, XtPointer call_data);

static void 
start_translate(Widget w, XtPointer ptr, XtPointer call_data)
{
  if (move_copy_or_orig == MOVE_COPY)
    translate_copy_CB(w, ptr, call_data);
  else
    translate_orig_CB(w, ptr, call_data);
}

static void 
start_scale(Widget w, XtPointer ptr, XtPointer call_data);

static void 
start_scale(Widget w, XtPointer ptr, XtPointer call_data)
{
  if (move_copy_or_orig == MOVE_COPY)
    scale_copy_CB(w, ptr, call_data);
  else
    scale_orig_CB(w, ptr, call_data);
}

static void 
start_mirror_center(Widget w, XtPointer ptr, XtPointer call_data);

static void 
start_mirror_center(Widget w, XtPointer ptr, XtPointer call_data)
{
  if (move_copy_or_orig == MOVE_COPY)
    mirror_center_copy_CB(w, ptr, call_data);
  else
    mirror_center_orig_CB(w, ptr, call_data);
}

static void 
start_mirror_plane(Widget w, XtPointer ptr, XtPointer call_data);

static void 
start_mirror_plane(Widget w, XtPointer ptr, XtPointer call_data)
{
  if (move_copy_or_orig == MOVE_COPY)
    mirror_plane_copy_CB(w, ptr, call_data);
  else
    mirror_plane_orig_CB(w, ptr, call_data);
}

static void 
start_rotate_by_point(Widget w, XtPointer ptr, XtPointer call_data);

static void 
start_rotate_by_point(Widget w, XtPointer ptr, XtPointer call_data)
{
  if (move_copy_or_orig == MOVE_COPY)
    rotate_by_point_copy_CB(w, ptr, call_data);
  else
    rotate_by_point_orig_CB(w, ptr, call_data);
}

static void 
start_rotate_by_angle(Widget w, XtPointer ptr, XtPointer call_data);

static void 
start_rotate_by_angle(Widget w, XtPointer ptr, XtPointer call_data)
{
  if (move_copy_or_orig == MOVE_COPY)
    rotate_by_angle_copy_CB(w, ptr, call_data);
  else
    rotate_by_angle_orig_CB(w, ptr, call_data);
}

static void set_render_mode(EView *v_p, caddr_t data, WCRec *p);

static ERenderingType the_render_mode = NORMAL_RENDERING;
static EShadingType the_shade_mode = CONST_SHADING_RENDERING;

static void 
set_render_mode(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetRenderMode(v_p, the_render_mode);
  EVSetShadeMode(v_p, the_shade_mode);
}

static void 
uninstall_apply_to_view(EView *v_p, caddr_t data);

static void 
uninstall_apply_to_view(EView *v_p, caddr_t data)
{
  EMUninstallHandler(EVGetModel(v_p));
}

static void 
render_CB(Widget w, XtPointer ptr, XtPointer call_data);

static void 
render_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  the_render_mode = (ERenderingType) ptr;
  EVSetApplyToViewFunction(set_render_mode, NULL, uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}

  
static void 
simple_command(Widget w, XtPointer ptr, XtPointer call_data);
 
static void 
simple_command(Widget w, XtPointer cl, XtPointer cd)
{
  char buf[512];

  if (cd != NULL) {
    sprintf(buf, "%s %s", (char *)cl, (char *)cd);
  } else {
    sprintf(buf, "%s", (char *)cl);
  }
  ESIHandleCmd(buf);
}

static void 
set_layer_on_off(EView *v_p, caddr_t data, WCRec *p)
{
  char tname[32];
  Widget paned = (Widget)data;
  Widget t;
  int i;
  Arg al[1];
  int ac;
  Boolean s;

	int layers[MAX_LAYER], dim = 0;
	BOOLEAN on[MAX_LAYER];

  for (i = 0; i < MAX_LAYER; i++) {
    sprintf(tname, "layer_toggle_%d", i);
    t = XtNameToWidget(paned, tname);
    if (t != NULL) {
      ac = 0;
      XtSetArg(al[ac], XtNstate, &s); ac++;
      XtGetValues(t, al, ac);
/*      EVSetLayerOnOff(v_p, i, (BOOLEAN)s);  */
			if(v_p->view_layers[i].layer_on != (BOOLEAN)s){
				layers[dim] = i;
				on[dim] = (BOOLEAN)s;
				dim++;
			}
    }
  }
	if(dim > 0)
		EVFastSetMultLayersOnOff(v_p, layers, on, dim);
}

static void apply_layer_settings(Widget w, XtPointer ptr, XtPointer call_data);
 
static void apply_layer_settings(Widget w, XtPointer ptr, XtPointer call_data)
{
  EVSetApplyToViewFunction(set_layer_on_off, ptr, uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}

static void apply_change(Widget w, XtPointer ptr, XtPointer call_data);
 
static void apply_change(Widget wid, XtPointer ptr, XtPointer call_data)
{
  char buf[512];
  Widget paned = (Widget)ptr, w;
  Boolean s;
  int ac;
  Arg al[2];

  ESIHandleCmd("CHMASK CLEAR");
  sprintf(buf, "%s", "CHMASK ");
#define ADD_TO_MASK(name)                     \
  {                                           \
    w = XtNameToWidget(paned, #name);         \
    if (w != NULL) {                          \
      ac = 0;                                 \
      XtSetArg(al[ac], XtNstate, &s); ac++;   \
      XtGetValues(w, al, ac);                 \
      if (s) strcat(buf, #name);              \
      strcat(buf, " ");                       \
    }                                         \
  }
  ADD_TO_MASK(layer);
  ADD_TO_MASK(color);
  ADD_TO_MASK(edgecolor);
  ADD_TO_MASK(edgeflag);
  ADD_TO_MASK(style);
  ADD_TO_MASK(width);
  ADD_TO_MASK(font);
  ADD_TO_MASK(fill);
  ADD_TO_MASK(text);
  ADD_TO_MASK(msize);
  ADD_TO_MASK(mtype);
  ADD_TO_MASK(keyp);
  ADD_TO_MASK(shrink);
  ADD_TO_MASK(rangle);
  ADD_TO_MASK(vectype);
  ADD_TO_MASK(vecscale);
  ADD_TO_MASK(vecrate);
  ADD_TO_MASK(vecshift);
  ADD_TO_MASK(tessel);
  ADD_TO_MASK(utessel);
  ADD_TO_MASK(vtessel);
  ADD_TO_MASK(pshow);
  ADD_TO_MASK(notclippable);
  ADD_TO_MASK(inviswhenclipped);
  ESIHandleCmd(buf);
  ESIHandleCmd("CHANGE");
}

static void
setup_locks_palette(void);
static void
setup_precinput_palette(void);
void
setup_the_viewclip_palette(Widget pal);
void
setup_the_modelclip_palette(Widget pal);
static void
setup_color_scale(void);


static void
GraphicEditor(Widget parent_paned, unsigned long mask)
{
  char tname[32], ltname[32];
  int i, j, cols, layer_num, rows;
  Arg al[3];
  int ac;
	Widget last_button, end_button;

/* Palettes of the graphic editor */

/* FILE */
  if (   (mask & ESI_GRAPHIC_EDITOR_MASK)
      || (mask & ESI_FILE_PALETTE_MASK)) {
    file_button = ESIAddPopdownMenu("file_button", "File",
                                  palettes_box, NULL, 0, &file_menu);
    load      = ESIAddMenuItem("load", "Load ELIXIR file",
                               file_menu, NULL, 0, simple_command, "LOAD");
    save      = ESIAddMenuItem("save", "Save",   
                               file_menu, NULL, 0, simple_command, "SAVE");
    saveas    = ESIAddMenuItem("saveas", "Save as",   
                               file_menu, NULL, 0, simple_command, "SAVE AS");
    psdump    = ESIAddMenuItem("psdump", "Dump Postscript of view",   
                               file_menu, NULL, 0, simple_command, "PSDUMP");
    quit      = ESIAddMenuItem("quit", "Quit program",
                               file_menu, NULL, 0, Quit, (XtPointer)palettes_box);
  }
  /* VIEW */
  if (   (mask & ESI_GRAPHIC_EDITOR_MASK)
      || (mask & ESI_VIEW_PALETTE_MASK)) {
    ac = 0;
    view_button = ESIAddPalette("view_button", "View",
                                palettes_box, al, ac, &view_palette);
    
    newframe = ESIAddButton("newframe", "New view",
                            commandWidgetClass, view_palette,
                            NULL, 0, new_frame_CB, NULL);  
    redraw = ESIAddButton("redraw", "Redraw", 
                          commandWidgetClass, view_palette,
                          NULL, 0, redraw_view_CB, NULL);
    fit_all    = ESIAddButton("fit_all", "Fit all",
                              commandWidgetClass, view_palette,
                              NULL, 0, fit_into_a_view_CB, NULL);
    stdview_button = ESIAddPopdownMenu("stdview", "Standard views",
                                        view_palette, NULL, 0, &stdview_menu);
    ESIAddMenuItem("top_view", "Top", stdview_menu, NULL, 0,
                   simple_command, "View top");
    ESIAddMenuItem("bottom_view", "Bottom", stdview_menu, NULL, 0,
                   simple_command, "VIEW BOTTOM");
    ESIAddMenuItem("front_view", "Front", stdview_menu, NULL, 0,
                   simple_command, "VIEW FRONT");
    ESIAddMenuItem("back_view", "Back", stdview_menu, NULL, 0,
                   simple_command, "VIEW BACK");
    ESIAddMenuItem("left_view", "Left", stdview_menu, NULL, 0,
                   simple_command, "VIEW LEFT");
    ESIAddMenuItem("right_view", "Right", stdview_menu, NULL, 0,
                   simple_command, "VIEW RIGHT");
    ESIAddMenuItem("iso_view", "Iso", stdview_menu, NULL, 0,
                   simple_command, "VIEW ISO");
    /* VIEW/VIEWCLIP */
    viewclip_button = ESIAddPalette("viewclip_button", "View clip",
                                    view_palette, NULL, 0, &viewclip_palette);
    setup_the_viewclip_palette(viewclip_palette);
    /* VIEW/MODELCLIP */
    modelclip_button = ESIAddPalette("modelclip_button", "Model clip",
                                     view_palette, NULL, 0,
                                     &modelclip_palette);
    setup_the_modelclip_palette(modelclip_palette);
    /* VIEW/LAYERS */
    layers_button = ESIAddPalette("layers_button", "Layers", view_palette,
                                NULL, 0, &layers_palette);
		rows = 8;
		cols = (MAX_LAYER - 1) / rows + 1;
		if(cols == 1){ 
			for (i = 0; i < MAX_LAYER; i++) {
				sprintf(ltname, "%3d", i);
				sprintf(tname, "layer_toggle_%d", i);
				ESIAddButton(tname, ltname, toggleWidgetClass, layers_palette,
										 NULL, 0, NULL, NULL);
			}
			layers_apply = ESIAddButton("layers_apply", "Apply", 
																	commandWidgetClass, layers_palette,
																	NULL, 0, apply_layer_settings, (XtPointer)layers_palette);
		}
		else{
			layers_form = XtCreateManagedWidget("layers_form", formWidgetClass,
																					layers_palette, NULL, 0);
			for(i = 0; i < rows; i++){
				for(j = 0; j < cols; j++){
					layer_num = j * rows + i;
					if(layer_num < MAX_LAYER){
						sprintf(ltname, "%3d", layer_num);
						sprintf(tname, "layer_toggle_%d", layer_num);
					}
					else{
						sprintf(ltname, "%3s", " ");
						sprintf(tname, "layer_toggle_%d", MAX_LAYER);
					}
					ac = 0;
					if(i != 0){
						XtSetArg(al[ac], XtNfromVert, end_button); ac++;
					}
					if(j != 0){
						XtSetArg(al[ac], XtNfromHoriz, last_button); ac++;
					}
					last_button = ESIAddButton(tname, ltname, toggleWidgetClass, layers_form,
																		 al, ac, NULL, NULL);
					if(j == cols - 1)end_button = last_button;
				}
			}
			layers_apply = ESIAddButton("layers_apply", "Apply", 
																	commandWidgetClass, layers_palette,
																	NULL, 0, apply_layer_settings, (XtPointer)layers_form);
		}
    /* VIEW/RENDER */
    render_button = ESIAddPalette("render_button", "Render", view_palette,
                                NULL, 0, &render_palette);
    ESIAddButton("wire", "Wire",
                 commandWidgetClass, render_palette,
                 NULL, 0, render_CB, (XtPointer)WIRE_RENDERING);
    ESIAddButton("normal", "Normal",
                 commandWidgetClass, render_palette,
                 NULL, 0, render_CB, (XtPointer)NORMAL_RENDERING);
    ESIAddButton("fill", "Filled hidden",
                 commandWidgetClass, render_palette,
                 NULL, 0, render_CB, (XtPointer)FILLED_HIDDEN_RENDERING);
    ESIAddButton("shade", "Shaded",
                 commandWidgetClass, render_palette,
                 NULL, 0, render_CB, (XtPointer)CONST_SHADING_RENDERING);
    ESIAddPopupDialog("view_bg", "View bkg color",   
                      "Color name", "slateblue", render_palette,
                      NULL, 0, simple_command, "VIEW BG ",
                      ESIDialogValueString, NULL);
    /* VIEW/RENDER */
    ac = 0;
    XtSetArg(al[ac], XtNvisual,           ElixirGetVisual()->visual); ac++;
    XtSetArg(al[ac], XtNdepth,             ElixirGetVisual()->depth); ac++;
    XtSetArg(al[ac], XtNcolormap, ColorStandardColormap()->colormap); ac++;
    color_scale_button = ESIAddPalette("color_scale_button",
                                       "Color scale", view_palette,
                                       al, ac, &color_scale_palette);
  }
  
  /* EDIT */
  if (   (mask & ESI_GRAPHIC_EDITOR_MASK)
      || (mask & ESI_EDIT_PALETTE_MASK)) {
    edit_button = ESIAddPalette("edit_button", "Edit",
                                palettes_box, NULL, 0, &edit_palette);
    
    undo = ESIAddButton("undo", "Undo", 
                        commandWidgetClass, edit_palette,
                        NULL, 0, undo_CB, NULL);
    redo = ESIAddButton("redo", "Redo",
                        commandWidgetClass, edit_palette,
                        NULL, 0, redo_CB, NULL);
    change_button = ESIAddPalette("change_button", "Change",
                                  edit_palette, NULL, 0, &change_palette);
    copy_or_orig = ESIAddButton("copy_or_orig",
                                       "Move original",
                                       toggleWidgetClass, edit_palette,
                                       NULL, 0, set_move_copy_or_orig, NULL);
    reshape = ESIAddButton("reshape", "Reshape", 
                           commandWidgetClass, edit_palette,
                           NULL, 0, reshape_CB, NULL);
    translate = ESIAddButton("translate", "Translate", 
                             commandWidgetClass, edit_palette,
                             NULL, 0, start_translate, NULL);
    scale = ESIAddButton("scale", "Scale", 
                             commandWidgetClass, edit_palette,
                             NULL, 0, start_scale, NULL);
    mirror_center = ESIAddButton("mirror_center", "Mirror/Center",
                                 commandWidgetClass, edit_palette,
                                 NULL, 0, start_mirror_center, NULL);
    mirror_plane = ESIAddButton("mirror_plane", "Mirror/Plane",
                                 commandWidgetClass, edit_palette,
                                 NULL, 0, start_mirror_plane, NULL);
    rotate_by_point = ESIAddButton("rotate_by_point", "Rotate/Point",
                                   commandWidgetClass, edit_palette,
                                   NULL, 0, start_rotate_by_point, NULL);
    rotate_by_angle = ESIAddButton("rotate_by_angle", "Rotate/Angle",
                                   commandWidgetClass, edit_palette,
                                   NULL, 0, start_rotate_by_angle, NULL);
    ggroup = ESIAddPopdownMenu("ggroup", "Graphic group",
                               edit_palette, NULL, 0, &ggroup_menu);
    ESIAddMenuItem("create_ggroup", "Create graphic group", ggroup_menu,
                   NULL, 0, simple_command, "GGROUP CREATE");
    ESIAddMenuItem("break_ggroup", "Break graphic group", ggroup_menu,
                   NULL, 0, simple_command, "GGROUP BREAK");
    delete = ESIAddButton("delete", "Delete", 
                          commandWidgetClass, edit_palette,
                          NULL, 0, delete_graphics_CB, NULL);  
    ginfo = ESIAddButton("ginfo", "Graphic info",
                         commandWidgetClass, edit_palette,
                         NULL, 0, ginfo_CB, NULL);
    oogl_export = ESIAddButton("oogl_export", "Export to GEOMVIEW",
                         commandWidgetClass, edit_palette,
                         NULL, 0, oogl_export_CB, NULL);
    /* EDIT/CHANGE */
#define ADD_TOGGLE_TO_CHANGE_PANED(WNAME, BLABEL)                           \
    WNAME = ESIAddButton(#WNAME, BLABEL, toggleWidgetClass, change_palette, \
                         NULL, 0, NULL, NULL)
      
    ADD_TOGGLE_TO_CHANGE_PANED(layer, "Layer");
    ADD_TOGGLE_TO_CHANGE_PANED(color, "Color");
    ADD_TOGGLE_TO_CHANGE_PANED(edgecolor, "Edge color");
    ADD_TOGGLE_TO_CHANGE_PANED(edgeflag, "Edge flag");
    ADD_TOGGLE_TO_CHANGE_PANED(style, "Line style");
    ADD_TOGGLE_TO_CHANGE_PANED(width, "Width");
    ADD_TOGGLE_TO_CHANGE_PANED(font, "Font");
    ADD_TOGGLE_TO_CHANGE_PANED(fill, "Fill");
    ADD_TOGGLE_TO_CHANGE_PANED(text, "Text");
    ADD_TOGGLE_TO_CHANGE_PANED(msize, "Marker size");
    ADD_TOGGLE_TO_CHANGE_PANED(mtype, "Marker type");
    ADD_TOGGLE_TO_CHANGE_PANED(keyp, "Key points");
    ADD_TOGGLE_TO_CHANGE_PANED(shrink, "Shrink");
    ADD_TOGGLE_TO_CHANGE_PANED(rangle, "Rot angle");
    ADD_TOGGLE_TO_CHANGE_PANED(vectype, "Vector type");
    ADD_TOGGLE_TO_CHANGE_PANED(vecscale, "Vector scale");
    ADD_TOGGLE_TO_CHANGE_PANED(vecrate, "Vector rate");
    ADD_TOGGLE_TO_CHANGE_PANED(vecshift, "Vector shift");
    ADD_TOGGLE_TO_CHANGE_PANED(tessel, "Tesselation");
		ADD_TOGGLE_TO_CHANGE_PANED(utessel, "U Tesselation");
		ADD_TOGGLE_TO_CHANGE_PANED(vtessel, "V Tesselation");
    ADD_TOGGLE_TO_CHANGE_PANED(pshow, "Show Ctrl poly");
    ADD_TOGGLE_TO_CHANGE_PANED(notclippable, "Not clippable");
    ADD_TOGGLE_TO_CHANGE_PANED(inviswhenclipped, "Clipped invisible");
    ESIAddButton("apply_change", "Apply", 
                 commandWidgetClass, change_palette,
                 NULL, 0, apply_change, (XtPointer)change_palette);
  }
  /* DRAW */
  if (   (mask & ESI_GRAPHIC_EDITOR_MASK)
      || (mask & ESI_DRAW_PALETTE_MASK)) {
    draw_button = ESIAddPalette("draw_button", "Draw",
                              palettes_box, NULL, 0, &draw_palette);  
    
    line = ESIAddButton("line", "Line", 
                             commandWidgetClass, draw_palette,
                             NULL, 0, draw_line_CB, NULL);
    marker = ESIAddButton("marker", "Marker", 
                               commandWidgetClass, draw_palette,
                               NULL, 0, draw_marker_CB, NULL);
    cbezc = ESIAddButton("cbezc", "Bezier curve",
                              commandWidgetClass, draw_palette,
                              NULL, 0, draw_cbezc_CB, NULL);
    bcbezs = ESIAddPopdownMenu("bcbezs", "Bezier patch",
                               draw_palette, 
                               NULL, 0, &bcbezs_menu);
    bcbezs_by_points = ESIAddMenuItem("bcbezs_by_points", "By points",
                                      bcbezs_menu, NULL, 0,
                                      draw_bcbezs_CB, NULL);
    bcbezs_from_curves = ESIAddMenuItem("bcbezs_from_curves", "From curves",
                                      bcbezs_menu, NULL, 0,
                                      from_curves_CB, "Select curves");
    bcbezs_extrude_curve_along_curve
      = ESIAddMenuItem("bcbezs_extrude_curve_along_curve",
                       "Extrude curve along curve",
                       bcbezs_menu, NULL, 0,
                       simple_command, "Unimplemented");
    bcbezs_extrude_curve_by_delta
      = ESIAddMenuItem("bcbezs_extrude_curve_by_delta",
                       "Extrude curve by delta",
                       bcbezs_menu, NULL, 0,
                       simple_command, "Unimplemented");
    triangle = ESIAddButton("triangle", "Triangle",
                                 commandWidgetClass, draw_palette,
                                 NULL, 0, draw_triangle_CB, NULL);
    tetrahedron = ESIAddButton("tetrahedron", "Tetrahedron",
                               commandWidgetClass, draw_palette,
                               NULL, 0, draw_tetra_CB, NULL);
    hexahedron = ESIAddButton("hexahedron", "Hexahedron",
                               commandWidgetClass, draw_palette,
                               NULL, 0, draw_hexah_CB, NULL);
    quadrangle = ESIAddButton("quadrangle", "Quadrangle",
                             commandWidgetClass, draw_palette,
                             NULL, 0, draw_quad_CB, NULL);
    bcbhexahedron = ESIAddPopdownMenu("bcbhexahedron", "BCB Hexahedron",
                                      draw_palette, 
                                      NULL, 0, &hexahedron_menu);
    hexa_from_curves = ESIAddMenuItem("hexa_from_curves", "From curves",
                                      hexahedron_menu, NULL, 0,
                                      from_curves_CB, NULL);
    hexa_extrude_patch_along_curve
      = ESIAddMenuItem("hexa_extrude_patch_along_curve",
                       "Extrude patch along curve",
                       hexahedron_menu, NULL, 0,
                       extrude_along_curve_CB, NULL);
    hexa_extrude_patch_by_delta
      = ESIAddMenuItem("hexa_extrude_patch_by_delta",
                       "Extrude patch by delta",
                       hexahedron_menu, NULL, 0,
                       extrude_by_delta_CB, NULL);
    hexa_set_extrusion_delta
      = ESIAddMenuItem("hexa_set_extrusion_delta",
                       "Set extrusion delta",
                       hexahedron_menu, NULL, 0,
                       set_extrusion_delta_CB, (XtPointer)draw_palette);
    anntext = ESIAddButton("anntext", "Annot. text",
                                commandWidgetClass, draw_palette,
                                NULL, 0, draw_anntext3d_CB, NULL);
    raprism = ESIAddButton("raprism", "Right angle prism", 
                             commandWidgetClass, draw_palette,
                             NULL, 0, draw_raprism_CB, NULL);
    pyramid = ESIAddButton("pyramid", "Pyramid", 
                             commandWidgetClass, draw_palette,
                             NULL, 0, draw_pyramid_CB, NULL);
  }
  
  /* ACTIVE SET */
  if (   (mask & ESI_GRAPHIC_EDITOR_MASK)
      || (mask & ESI_ACTSET_PALETTE_MASK)) {
    actset_button = ESIAddPalette("actset_pal", "Set",
                              palettes_box, NULL, 0, &actset_palette);
    
#undef ADD_DIALOG
#define ADD_DIALOG(the_command, dialog_button_name,         \
                   dialog_button_label,                     \
                   dialog_prompt, initial_value,            \
                   value_type)                              \
    dialog_button_name =                                       \
    ESIAddPopupDialog(#dialog_button_name,                     \
                      dialog_button_label,                     \
                      dialog_prompt, initial_value,            \
                      actset_palette, NULL, 0,                 \
                      simple_command, the_command,             \
                      value_type,                              \
                      NULL)                             
      
    ADD_DIALOG("LAYER", layer, "Layer", 
               "Layer (0 <= layer < MAX_LAYER)", "0",
               ESIDialogValueNumber);
    ADD_DIALOG("COLOR", color, "Color", "Color name", "green",
               ESIDialogValueString);
    ADD_DIALOG("EDGECOLOR", edgecolor, "Edge color", "Edge color name",
               "white", ESIDialogValueString);
    ADD_DIALOG("EDGEFLAG", edgeflag, "Edge flag", "Edge flag {ON|OFF}",
               "off", ESIDialogValueString);
    ADD_DIALOG("STYLE", style, "Line style", "Style (\"S\"olid, \"D\"ash)",
               "solid", ESIDialogValueString);
    ADD_DIALOG("WIDTH", width, "Width", "Width (width >=0)", "0",
               ESIDialogValueNumber);
    ADD_DIALOG("FONT", font, "Font",
               "Font name", "fixed", ESIDialogValueString);
    ADD_DIALOG("FILL", fill, "Fill style", "Fill style (\"S\"olid, \"O\"ff)",
               "solid", ESIDialogValueString);
    ADD_DIALOG("TEXT", text, "Text", "Text", "any text",
               ESIDialogValueString);
    ADD_DIALOG("MSIZE", msize, "Marker size", "Size >= 0", "10",
               ESIDialogValueNumber);
    ADD_DIALOG("MTYPE", mtype, "Marker type",
               "Type (0=circle, 1=filled circle, 2=square, 3=X)", "0",
               ESIDialogValueNumber);
    ADD_DIALOG("KEYP", keyp, "Key points",
               "Number of intervals (>1)", "1",
               ESIDialogValueNumber);
    ADD_DIALOG("SHRINK", shrink, "Shrink", "Shrink (0 <= shrink <= 1)", "1",
               ESIDialogValueNumber);
    ADD_DIALOG("RANGLE", rangle, "Rotation angle",
               "Angle (say \"angle DEG\" for degrees)", "90 DEG",
               ESIDialogValueNumber);
    ADD_DIALOG("VECTYPE", vectype, "Vector type",
               "Type (0=arrow, 1=double arrow, 2=tripple arrow, 3=filled arrow)", "0",
               ESIDialogValueNumber);
    ADD_DIALOG("VECSCALE", vecscale, "Vector scale", "Scaling factor", "1",
               ESIDialogValueNumber);
    ADD_DIALOG("VECRATE", vecrate, "Vector rate", "Rate of decoration", "0.15",
               ESIDialogValueNumber);
    ADD_DIALOG("VECSHIFT", vecshift, "Vector shift", "Shifting (0=no shift, 1=shift)", "0",
               ESIDialogValueNumber);
    ADD_DIALOG("TESSEL", tessel, "Tesselation", "Number of intervals", "16",
               ESIDialogValueNumber);
    ADD_DIALOG("UTESSEL", utessel, "U Tesselation", "Number of intervals in u direction", "8",
               ESIDialogValueNumber);
    ADD_DIALOG("VTESSEL", vtessel, "V Tesselation", "Number of intervals in v direction", "8",
               ESIDialogValueNumber);
    ADD_DIALOG("SELCRIT", selcrit, "Selection crit",
               "Criterion (\"INS\"ide, \"O\"verlap, \"INT\"ersect)",
               "inside", ESIDialogValueAny);
    ADD_DIALOG("PSHOW", pshow, "Show CTRL poly",
               "(ON or OFF)", "on", ESIDialogValueString);
    ADD_DIALOG("NOTCLIPPABLE", notclippable, "Not clippable",
               "(ON or OFF)", "off", ESIDialogValueString);
    ADD_DIALOG("INVISWHENCLIPPED", inviswhenclipped, "Clipped invisible",
               "(ON or OFF)", "off", ESIDialogValueString);
  }
  /* INPUT HELPS */
  if (   (mask & ESI_GRAPHIC_EDITOR_MASK)
      || (mask & ESI_INPUT_HELPS_PALETTE_MASK)) {
    /* LOCKS */
    locks_button = ESIAddPalette("locks_button", "Locks",
                                 palettes_box, NULL, 0, &locks_palette);
    setup_locks_palette();
    /* PRECISION INPUT */
    precinput_button = ESIAddPalette("precinput", "Input",
                                      palettes_box, NULL, 0,
                                     &precinput_palette);
    setup_precinput_palette();
  }
  /* CUSTOM */
  if (   (mask & ESI_GRAPHIC_EDITOR_MASK)
      || (mask & ESI_CUSTOM_PALETTE_MASK)
      || (mask & ESI_CUSTOM_PALETTE_BOX_MASK)) {
		if((mask & ESI_CUSTOM_PALETTE_BOX_MASK) != 0)
			ESICustomize(palettes_box);
		else{
			ESIAddPalette("custom_button", "Custom",
										palettes_box, NULL, 0, &custom_palette);
			ESICustomize(custom_palette);
		}
	}
}


static void
apply_grid_locks_CB(Widget w, XtPointer ptr, XtPointer call_data);
static void
apply_grid_locks(EView *v_p, caddr_t data, WCRec *p);

static void
apply_grid_locks(EView *v_p, caddr_t data, WCRec *p)
{
  int ac;
  Arg al[4];
  Boolean s;
  
  ac = 0;
  XtSetArg(al[ac], XtNstate, &s); ac++;
  XtGetValues(lock_cplane, al, ac);
  EVSetConstrPlaneLock(v_p, (BOOLEAN)s);
  ac = 0;
  XtSetArg(al[ac], XtNstate, &s); ac++;
  XtGetValues(lock_grid, al, ac);
  EVSetGridLock(v_p, (BOOLEAN)s);
  EVShowGrid(v_p, (BOOLEAN)s);
}

static void
apply_grid_locks_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  EVSetApplyToViewFunction(apply_grid_locks, 0,
                           uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}


static void
setup_cplane_palette(void);

static void
setup_locks_palette(void)
{
  Widget label1, label2, lu, lv, ldu, ldv;
  int ac;
  Arg al[6];
  
  lock_grid = ESIAddButton("lock_grid", "Lock to grid", 
                           toggleWidgetClass, locks_palette,
                           NULL, 0, NULL, NULL);
  lock_cplane = ESIAddButton("lock_cplane", "Lock to construction plane", 
                             toggleWidgetClass, locks_palette,
                             NULL, 0, NULL, NULL);
  lock_setup_cplane_button = ESIAddPalette("lock_setup_cplane_button",
                                           "Set up construction plane",
                                           locks_palette, NULL, 0,
                                           &lock_setup_cplane_palette);
  
  locks_form = XtCreateManagedWidget("locks_form", formWidgetClass,
                                     locks_palette, NULL, 0);
  ac = 0;
  XtSetArg(al[ac], XtNlabel,
           "grid origin"); ac++;
  label1 = XtCreateManagedWidget("label1", labelWidgetClass,
                                 locks_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNlabel, " U"); ac++;
  lu = XtCreateManagedWidget("lu", labelWidgetClass, locks_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNfromHoriz, lu); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  locks_u = ESIAddButton("locks_u", "", 
                         asciiTextWidgetClass, locks_form,
                         al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNfromHoriz, locks_u); ac++;
  XtSetArg(al[ac], XtNlabel, " V"); ac++;
  lv = XtCreateManagedWidget("lv", labelWidgetClass, locks_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label1); ac++;
  XtSetArg(al[ac], XtNfromHoriz, lv); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  locks_v = ESIAddButton("locks_v", "", 
                         asciiTextWidgetClass, locks_form,
                         al, ac, NULL, NULL); 


  ac = 0;
  XtSetArg(al[ac], XtNlabel, "Grid deltas"); ac++;
  XtSetArg(al[ac], XtNfromVert, locks_v); ac++;
  label2 = XtCreateManagedWidget("label2", labelWidgetClass,
                                 locks_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label2); ac++;
  XtSetArg(al[ac], XtNlabel, "DU"); ac++;
  ldu = XtCreateManagedWidget("ldu", labelWidgetClass, locks_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label2); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ldu); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  locks_du = ESIAddButton("locks_du", "", 
                         asciiTextWidgetClass, locks_form,
                         al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label2); ac++;
  XtSetArg(al[ac], XtNfromHoriz, locks_du); ac++;
  XtSetArg(al[ac], XtNlabel, "DV"); ac++;
  ldv = XtCreateManagedWidget("ldv", labelWidgetClass, locks_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, label2); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ldv); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  locks_dv = ESIAddButton("locks_dv", "", 
                         asciiTextWidgetClass, locks_form,
                         al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, locks_dv); ac++;
  locks_apply = ESIAddButton("locks_apply",
                             "           Apply             ", 
                             commandWidgetClass, locks_form,
                             al, ac, apply_grid_locks_CB, NULL);

  setup_cplane_palette();
}


static void
setup_cplane_palette(void)
{
  int ac;
  Arg al[6];

  ac = 0;
  ESIAddButton("cplane_center",
               " Center ", 
               commandWidgetClass, lock_setup_cplane_palette,
               al, ac, NULL, NULL);
  ac = 0;
  ESIAddButton("cplane_orientation",
               " By three points ", 
               commandWidgetClass, lock_setup_cplane_palette,
               al, ac, cplane_by_three_points_CB, NULL);
  ac = 0;
  ESIAddPopupDialog("cplane_normal", "Normal",   
                    "Normal (Nx Ny Nz)", "1 0 0",
                    lock_setup_cplane_palette,
                    NULL, 0, NULL, NULL,
                    ESIDialogValueAny, NULL);
  ac = 0;
  ESIAddPopupDialog("cplane_vector_up", "Vector Up",   
                    "Vector up (Vx Vy Vz)", "0 0 1",
                    lock_setup_cplane_palette,
                    NULL, 0, NULL, NULL,
                    ESIDialogValueAny, NULL);
}

static void
pass_xy_command(Widget w, XtPointer ptr, XtPointer call_data);


static void 
ReturnHitInXYZ(Widget w, XEvent *event, String *params,
               Cardinal *num_params)
{
  if (*num_params == 1) {
    if (!strcmp(params[0], "0"))
      XtCallCallbacks(precinput_xyz_ok, XtNcallback, NULL);
    else
      XtCallCallbacks(precinput_dxyz_ok, XtNcallback, NULL);
  }
}



static void
setup_precinput_palette(void)
{
  Widget lx, ly, lz, ldx, ldy, ldz;
  int ac;
  Arg al[6];
  
  precinput_form = XtCreateManagedWidget("precinput_form", formWidgetClass,
                                         precinput_palette, NULL, 0);
  ac = 0;
  XtSetArg(al[ac], XtNlabel, " X"); ac++;
  lx = XtCreateManagedWidget("lx", labelWidgetClass, precinput_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromHoriz, lx); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  precinput_x = ESIAddButton("precinput_x", "", 
                             asciiTextWidgetClass, precinput_form,
                             al, ac, NULL, NULL); 
  ac = 0;
  XtSetArg(al[ac], XtNfromHoriz, precinput_x); ac++;
  XtSetArg(al[ac], XtNlabel, " Y"); ac++;
  ly = XtCreateManagedWidget("ly", labelWidgetClass, precinput_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromHoriz, ly); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  precinput_y = ESIAddButton("precinput_y", "", 
                             asciiTextWidgetClass, precinput_form,
                             al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromHoriz, precinput_y); ac++;
  XtSetArg(al[ac], XtNlabel, " Z"); ac++;
  lz = XtCreateManagedWidget("lz", labelWidgetClass, precinput_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromHoriz, lz); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  precinput_z = ESIAddButton("precinput_z", "", 
                             asciiTextWidgetClass, precinput_form,
                             al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromHoriz, precinput_z); ac++;
  precinput_xyz_ok = ESIAddButton("precinput_xyz_ok", " OK ", 
                                  commandWidgetClass, precinput_form,
                                  al, ac, pass_xy_command, (XtPointer)0);
  XtAppAddActions(XtWidgetToApplicationContext(ESITopLevelWidget()),
                  remap_return_XYZ, XtNumber(remap_return_XYZ));
  tt3 = XtParseTranslationTable("#override <KeyPress>Return: retActXYZOK(0)");
  XtOverrideTranslations(precinput_x, tt3);
  XtOverrideTranslations(precinput_y, tt3);
  XtOverrideTranslations(precinput_z, tt3);

  ac = 0;
  XtSetArg(al[ac], XtNfromVert, precinput_xyz_ok); ac++;
  XtSetArg(al[ac], XtNlabel, "DX"); ac++;
  ldx = XtCreateManagedWidget("ldx", labelWidgetClass, precinput_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, precinput_xyz_ok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ldx); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  precinput_dx = ESIAddButton("precinput_dx", "", 
                             asciiTextWidgetClass, precinput_form,
                             al, ac, NULL, NULL); 
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, precinput_xyz_ok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, precinput_dx); ac++;
  XtSetArg(al[ac], XtNlabel, "DY"); ac++;
  ldy = XtCreateManagedWidget("ldy", labelWidgetClass, precinput_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, precinput_xyz_ok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ldy); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  precinput_dy = ESIAddButton("precinput_dy", "", 
                             asciiTextWidgetClass, precinput_form,
                             al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, precinput_xyz_ok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, precinput_dy); ac++;
  XtSetArg(al[ac], XtNlabel, "DZ"); ac++;
  ldz = XtCreateManagedWidget("ldz", labelWidgetClass, precinput_form, al, ac);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, precinput_xyz_ok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, ldz); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  precinput_dz = ESIAddButton("precinput_dz", "", 
                             asciiTextWidgetClass, precinput_form,
                             al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, precinput_xyz_ok); ac++;
  XtSetArg(al[ac], XtNfromHoriz, precinput_dz); ac++;
  precinput_dxyz_ok = ESIAddButton("precinput_dxyz_ok", " OK ", 
                                  commandWidgetClass, precinput_form,
                                  al, ac, pass_xy_command, (XtPointer)1);
  tt3 = XtParseTranslationTable("#override <KeyPress>Return: retActXYZOK(1)");
  XtOverrideTranslations(precinput_dx, tt3);
  XtOverrideTranslations(precinput_dy, tt3);
  XtOverrideTranslations(precinput_dz, tt3);
}


static void
pass_xy_command(Widget w, XtPointer ptr, XtPointer call_data)
{
  int ac;
  Arg al[2];
  char *s;
  char buf[132];
  int flag = (int)ptr;

  if (flag == 0)
    strcpy(buf, "XY ");         /* absolute */
  else
    strcpy(buf, "DX ");         /* relative */
  
  ac = 0;
  XtSetArg(al[ac], XtNstring, &s); ac++;
  if (flag == 0)
    XtGetValues(precinput_x, al, ac);
  else
    XtGetValues(precinput_dx, al, ac);
  TypeInParseLine(s);
  if (TypeInGetTokenType(1) != NUMBER)
    goto err_exit;
  strcat(buf, s); strcat(buf, " ");
  ac = 0;
  XtSetArg(al[ac], XtNstring, &s); ac++;
  if (flag == 0)
    XtGetValues(precinput_y, al, ac);
  else
    XtGetValues(precinput_dy, al, ac);
  TypeInParseLine(s);
  if (TypeInGetTokenType(1) != NUMBER)
    goto err_exit;
  strcat(buf, s); strcat(buf, " ");
  ac = 0;
  XtSetArg(al[ac], XtNstring, &s); ac++;
  if (flag == 0)
    XtGetValues(precinput_z, al, ac);
  else
    XtGetValues(precinput_dz, al, ac);
  TypeInParseLine(s);
  if (TypeInGetTokenType(1) != NUMBER)
    goto err_exit;
  strcat(buf, s); 
  
  ESIHandleCmd(buf);

  return;
  
 err_exit:
    ERptErrMessage(1, 1, "Bad input; must get 3 numbers", ERROR_GRADE);
  return;
}




/* ======================================================================== */
/* PUBLIC */
/* ======================================================================== */

Widget
BuildTop(int *argc, char **argv, char *Class, char **fallback_resources);

extern int elixir_default_visual;
extern int elixir_best_visual;
extern int elixir_default_cmap;

int
ESIBuildInterface(unsigned long type_mask, int argc, char **argv)
{
  Arg args[10];
  int argn;
  Display *dsp;
  EFringeTable ft;
  int elixir_options;
  
  topPaned = BuildTop(&argc, argv, "SimpleXF", fallback_resources);

  /* Look at Elixir-specific options */
  
  
  dsp = XtDisplay(topPaned);

  if (  elixir_default_visual
      | elixir_best_visual
      | elixir_default_cmap)
    elixir_options = 0;
  else
    elixir_options = ELIXIR_INIT_OPTIONS;
  
  if (elixir_default_cmap)   elixir_options |= ELIXIR_DEFAULT_COLORMAP_OPTION;
  if (elixir_default_visual) elixir_options |= ELIXIR_DEFAULT_VISUAL_OPTION;
  if (elixir_best_visual)    elixir_options |= ELIXIR_BEST_VISUAL_OPTION;
  
  if (!ElixirInit(XtDisplay(ESITopLevelWidget()),
                  XtScreen(ESITopLevelWidget()),
                  elixir_options)) 
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT)); 
  ERptSetPromptRoutine((EPromptRoutine)PromptMessage);
  ERptSetPtrTrackRoutine((EPtrTrackRoutine)TrackPointer);
  ERptSetStatusRoutine((EStatusRoutine)StatusMessage);
  ERptSetErrorHandler((EErrMessagehandler)ErrorMessage);
  
  copyright =
    ESIAddButton("copyright",
                 "          ELIXIR (C) Copyright 1994, Petr Krysl           ",
                 labelWidgetClass, topPaned,
                 NULL, 0, NULL, NULL);

  if (   (type_mask & ESI_GRAPHIC_EDITOR_MASK)
      || (type_mask & ESI_TRACK_AREA_MASK)) {
		argn = 0;
		XtSetArg(args[argn], XtNstring, TRACK_HEADER); argn++;
		track = ESIAddButton("track", " ", asciiTextWidgetClass, topPaned,
												 args, argn, NULL, NULL);
	}

  if (   (type_mask & ESI_GRAPHIC_EDITOR_MASK)
      || (type_mask & ESI_PROMPT_AREA_MASK)) {
		argn = 0;
		XtSetArg(args[argn], XtNstring, PROMPT_HEADER); argn++;
		prompt = ESIAddButton("prompt", " ", asciiTextWidgetClass, topPaned,
													args, argn, NULL, NULL);
	}
  
  if (   (type_mask & ESI_GRAPHIC_EDITOR_MASK)
      || (type_mask & ESI_STATUS_AREA_MASK)) {
		argn = 0;
		XtSetArg(args[argn], XtNstring, STATUS_HEADER); argn++;
		status = ESIAddButton("status", " ", asciiTextWidgetClass, topPaned,
													args, argn, NULL, NULL);
	}

  if (   (type_mask & ESI_GRAPHIC_EDITOR_MASK)
      || (type_mask & ESI_COMMAND_AREA_MASK)) {
		argn = 0;
		XtSetArg(args[argn], XtNresizable, True); argn++;
		cmd_form = XtCreateManagedWidget("cmd_form", formWidgetClass,
																		 topPaned, args, argn);

		argn = 0;
		cmd_buttons_paned = XtCreateManagedWidget("cmd_buttons_paned",
																							panedWidgetClass,
																							cmd_form, args, argn);
  
		executeCmd = ESIAddButton("executeCmd", "Execute",
															commandWidgetClass, cmd_buttons_paned, NULL, 0,
															ExecuteCommand, (XtPointer)&cmdtext);
		
		clearCmd  = ESIAddButton("clearCmd", "Clear",
														 commandWidgetClass, cmd_buttons_paned, NULL, 0,
														 ClearText, (XtPointer)&cmdtext);
		
		help = ESIAddButton("help", "Help",
												commandWidgetClass, cmd_buttons_paned,
												NULL, 0, HelpMe, NULL);

		argn = 0;
		XtSetArg(args[argn], XtNwidth, 150); argn++;   /* 250 */
		XtSetArg(args[argn], XtNheight, 47); argn++;   /*  50 */
		XtSetArg(args[argn], XtNstring, START_UP_MESSAGE); argn++;
		XtSetArg(args[argn], XtNfromVert, NULL); argn++;
		XtSetArg(args[argn], XtNfromHoriz, cmd_buttons_paned); argn++;
		XtSetArg(args[argn], XtNtop, XawChainTop); argn++;
		XtSetArg(args[argn], XtNbottom, XawChainBottom); argn++;
		cmdtext = ESIAddButton("cmdtext", " ", asciiTextWidgetClass,
													 cmd_form, args, argn, NULL, NULL);
		XtAppAddActions(XtWidgetToApplicationContext(ESITopLevelWidget()),
										remap_return, XtNumber(remap_return));
		tt1 = XtParseTranslationTable("#override <KeyPress>Return: retActCmd()");
		XtOverrideTranslations(cmdtext, tt1);
		
		argn = 0;
		XtSetArg(args[argn], XtNstring, ""); argn++;
		XtSetArg(args[argn], XtNwidth, 150); argn++; /* 250 */
		XtSetArg(args[argn], XtNheight, 47); argn++; /*  50 */
		XtSetArg(args[argn], XtNfromVert, NULL); argn++;
		XtSetArg(args[argn], XtNfromHoriz, cmdtext); argn++;
		XtSetArg(args[argn], XtNtop, XawChainTop); argn++;
		XtSetArg(args[argn], XtNbottom, XawChainBottom); argn++;
		history = ESIAddButton("history", " ", asciiTextWidgetClass, 
													 cmd_form, args, argn, NULL, NULL);
	}

  if (   (type_mask & ESI_GRAPHIC_EDITOR_MASK)
      || (type_mask & ESI_FILE_PALETTE_MASK)
      || (type_mask & ESI_VIEW_PALETTE_MASK)
      || (type_mask & ESI_EDIT_PALETTE_MASK)
      || (type_mask & ESI_DRAW_PALETTE_MASK)
      || (type_mask & ESI_ACTSET_PALETTE_MASK)
      || (type_mask & ESI_INPUT_HELPS_PALETTE_MASK)
      || (type_mask & ESI_CUSTOM_PALETTE_MASK)
      || (type_mask & ESI_CUSTOM_PALETTE_BOX_MASK)) {
		argn = 0;
		XtSetArg(args[argn], XtNorientation, XtorientHorizontal); argn++;
		palettes_box = ESIAddButton("palettes_box", " ",
																panedWidgetClass, topPaned,
																args, argn, NULL, NULL);
		GraphicEditor(palettes_box, type_mask);
	}
  
  age_model = EMCreateModel( );
  ft = ColorCreateFringeTable(); 
  EMSetAssocFringeTable(ESIModel(), ft);
  ColorSetupFringeTableByMinMax(ft, -1, 1);
  if (   (type_mask & ESI_GRAPHIC_EDITOR_MASK)
      || (type_mask & ESI_VIEW_PALETTE_MASK)) 
		setup_color_scale();
  
  HelpMe(NULL, NULL, NULL);
  
  return 0;
}




EModel *
ESIModel(void)
{
  return age_model;
}


static void 
apply_adopt_color_scale(EView *v_p, caddr_t data, WCRec *p);
static void
adopt_color_scale_CB(Widget w, XtPointer ptr, XtPointer call_data);


int color_scale_num_labels = ELIXIR_COLOR_SCALE_NUM_LABELS;

static void
setup_color_scale(void)
{
  char lname[64];
  char lval[64];
  Arg al[5];
  int ac;
  Widget label;
  FPNum v, delta, minv, maxv;
  EFringeTable ft;
  int i;
  EPixel color;
  
  ft = EMGetAssocFringeTable(ESIModel());
  ColorFringesMinMax(ft, &minv, &maxv);
  delta = (maxv - minv) / color_scale_num_labels;
  v = minv + delta / 2.0;
  for (i = 0; i < color_scale_num_labels; i++) {
    sprintf(lname, "color_scale_%d", i);
    sprintf(lval, "%g", v);
		color = ColorFringeRangeToColor(ColorFringeValueToRange(ft, v));
    ac = 0;
    XtSetArg(al[ac], XtNbackground,                           color); ac++;
    XtSetArg(al[ac], XtNvisual,           ElixirGetVisual()->visual); ac++;
    XtSetArg(al[ac], XtNdepth,             ElixirGetVisual()->depth); ac++;
    XtSetArg(al[ac], XtNcolormap, ColorStandardColormap()->colormap); ac++;
    label = ESIAddButton(lname, lval,
                         labelWidgetClass, color_scale_palette,
                         al, ac, NULL, NULL);
    v += delta;
  }
  ac = 0;
  ESIAddButton("color_scale_adopt", "Pick view",
               commandWidgetClass, color_scale_palette,
               al, ac, adopt_color_scale_CB, NULL); 
}




static void
adopt_color_scale_CB(Widget w, XtPointer ptr, XtPointer call_data)
{
  EVSetApplyToViewFunction(apply_adopt_color_scale, 0,
                           uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}



void
edit_color_scale(Widget pal, EView *v_p);

static void 
apply_adopt_color_scale(EView *v_p, caddr_t data, WCRec *p)
{
  edit_color_scale(color_scale_palette, v_p);
}
