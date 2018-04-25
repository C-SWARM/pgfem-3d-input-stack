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

#ifndef ESIMPLE_H
#define ESIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>


#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>


#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <X11/Shell.h>

#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>

#include "Etypes.h"
#include "Eview.h"
#include "Emodel.h"        
#include "Ereprts.h"       
#include "Eactset.h"
#include "Efonts.h"
#include "Ecolors.h"
#include "Egeomops.h"
#include "Etransfm.h"
#include "Etypein.h"
#include "Eutils.h"
#include "Elixir.h"
#include "Elixir.h"        
#include "Egecmds.h"       
#include "Egecb.h"
#include "Eondisk.h"
#include "Eobject.h"
#include "Egraphic.h"
#include "EXI.h"


#include "Eanntxt3d.h"
#include "Ebcbezs3d.h"
#include "Ebezier.h"
#include "Ecbezc3d.h"
#include "Eggroup.h"
#include "Eline3d.h"
#include "Emark3d.h"
#include "Equad3d.h"
#include "Equadwd3d.h"
#include "Etria3d.h"
#include "Etriawd3d.h"
#include "Etetra.h"
#include "Etetrawd.h"
#include "Ebcbhex.h"
#include "Eraprism.h"
#include "Epyramid.h"
#include "Ehexah.h"
#include "Ehexahwd.h"
#include "Evecmark3d.h"
#include "Evector3d.h"
#include "Erbezc3d.h"
#include "Erbezs3d.h"
#include "Eisa.h"
#include "Emarkwd3d.h"


#include "Eexit.h"




#ifdef SIMPLE_PRIVATE_HEADER
EModel *age_model; 
#else
extern EModel *age_model; 
#endif


FILE *
XsraSelFile(Widget toplevel,
            char *prompt, char *ok, char *cancel, char *failed,
	    char *init_path, char *mode, int (*show_entry)(),
            char **name_return);


void /* THIS MUST BE SUPPLIED BY THE PROGRAMMER */
ESICustomize(Widget parent_pane);

Widget
ESIAddButton(char *name, char *label, WidgetClass wclass, Widget parent,
             Arg *args, int argn,
             XtCallbackProc callback, XtPointer client_data);

/* Mask, passed into the routine ESIBuildInterface: select those palettes */
/* of the general functionality of a graphic editor that you need. */
/* ESI_GRAPHIC_EDITOR_MASK ... all palettes and areas */
#define ESI_GRAPHIC_EDITOR_MASK      (1L<<1)
/* ESI_FILE_PALETTE_MASK ... file palettes */
#define ESI_FILE_PALETTE_MASK        (1L<<2)
/* ESI_VIEW_PALETTE_MASK ... view palettes */
#define ESI_VIEW_PALETTE_MASK        (1L<<3)
/* ESI_EDIT_PALETTE_MASK ... edit palettes */
#define ESI_EDIT_PALETTE_MASK        (1L<<4)
/* ESI_DRAW_PALETTE_MASK ... edit palettes */
#define ESI_DRAW_PALETTE_MASK        (1L<<5)
/* ESI_ACTSET_PALETTE_MASK ... active set palettes */
#define ESI_ACTSET_PALETTE_MASK      (1L<<6)
/* ESI_INPUT_HELPS_PALETTE_MASK ... active set palettes */
#define ESI_INPUT_HELPS_PALETTE_MASK (1L<<7)
/* ESI_CUSTOM_PALETTE_MASK .. custom set palettes */
#define ESI_CUSTOM_PALETTE_MASK      (1L<<8)
/* ESI_TRACK_AREA_MASK ... track area */
#define ESI_TRACK_AREA_MASK          (1L<<9)
/* ESI_PROMPT_AREA_MASK ... prompt area */
#define ESI_PROMPT_AREA_MASK         (1L<<10)
/* ESI_STATUS_AREA_MASK ... status area */
#define ESI_STATUS_AREA_MASK         (1L<<11)
/* ESI_COMMAND_AREA_MASK ... command + history area */
#define ESI_COMMAND_AREA_MASK        (1L<<12)
/* ESI_CUSTOM_PALETTE_BOX_MASK ... custom palettes box */
#define ESI_CUSTOM_PALETTE_BOX_MASK  (1L<<13)


int
ESIBuildInterface(unsigned long type_mask, int argc, char **argv);
void
ESIPopupAndRun(void);
void
ESIPopup(void);
int 
ESIEventLoop(BOOLEAN stop_cmd, char *msg);
EModel *
ESIModel(void);
Widget
ESITopLevelWidget(void);
void
ESIEventLoopProceed(void);
void
ESIEventLoopStop(void);
void
ESIHandleCmd(char *cmd_string);
Widget
ESIAddPalette(char *pane_name, char *menu_label, Widget parent_pane,
              Arg *args, int argn,
              Widget *pane_return);

typedef enum { ESIDialogValueAny    = 0,
               ESIDialogValueString = 1,
               ESIDialogValueNumber = 2
             }                            ESIDialogValueType;

typedef BOOLEAN (*ESIVerifyValueProc)(void);

Widget
ESIAddPopupDialog(char *dialog_button_name, char *dialog_button_label,
                  char *dialog_prompt, char *initial_value,
                  Widget parent, Arg *args, int argn,
                  XtCallbackProc callback, XtPointer client_data,
                  ESIDialogValueType value_type,
                  ESIVerifyValueProc verify_proc);
Widget
ESIAddExtensiblePopupDialog(char *dialog_button_name, char *dialog_button_label,
														char *dialog_name, char *dialog_prompt, char *initial_value,
														Widget *ext_form,
														Widget parent, Arg *args, int argn,
														XtCallbackProc callback, XtPointer client_data,
														ESIDialogValueType value_type,
														ESIVerifyValueProc verify_proc);

typedef void (*ESIConfirmActionProc)(XtPointer client_data);

void 
ESIPopupConfirmDialog(Widget for_widget, char *with_label,
                      ESIConfirmActionProc ok_action,
                      XtPointer ok_client_data,
                      ESIConfirmActionProc cancel_action,
                      XtPointer cancel_client_data);
void
ESIPopupInfo(char *string);
void
ESIPopupInfoFromFile(char *file_name);

void
ESIPopupSingleInfo(char *string);
void
ESIPopupSingleInfoFromFile(char *file_name);

FILE *
ESISelFile(char *prompt, char *ok, char *cancel, char *failed,
           char *init_path, char *mode, char **name_return);
Widget
ESIAddMenuItem(char *widget_name, char *label,
               Widget menu_shell, Arg *args, int argn,
               XtCallbackProc callback, XtPointer client_data);
Widget
ESIAddPopdownMenu(char *button_widget, char *label,
                  Widget parent, Arg *args, int argn,
                  Widget *menu_shell);

#ifdef __cplusplus
}
#endif

#endif
