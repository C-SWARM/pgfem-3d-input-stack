
#include "Esimple.h"
#include "Eutils.h"


static int elixir_exit_code = ELIXIR_OK_EXIT;

int
ESISetExitCode(int exit_code)
{
  int old_exit_code = elixir_exit_code;
  
  elixir_exit_code = exit_code;
  return old_exit_code;
}


static int elixir_error_exit_code = ELIXIR_ERROR_EXIT;

int
ESISetErrorExitCode(int exit_code)
{
  int old_exit_code = elixir_error_exit_code;
  
  elixir_error_exit_code = exit_code;
  return old_exit_code;
}



static Widget
MakeNewSingleInfoViewer(void);


int
ErrorMessage(int class, int serial,
             const char *deflt_msg, EErrGrade grade);

static void 
ReturnHitInPopupDialog(Widget w, XEvent *event, String *params,
                       Cardinal *num_params);

static XtTranslations tt2;
static XtActionsRec remap_return_popup_dialog[] = {
  {"retActOK", ReturnHitInPopupDialog },
};

typedef struct PopupDialogData { /* Used by popup dialogs */
  Widget               popup_shell;
  Widget               dialog_button;
  Widget               text_field;
  ESIDialogValueType   value_type;
  ESIVerifyValueProc   verify_proc;
  XtCallbackProc       callback;
  XtPointer            client_data;
} PopupDialogData;


static void 
PopupDialog(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget ps = (Widget) client_data;
  Position x, y;
  Dimension width, height;

  XtVaGetValues(w,
                XtNwidth, &width, XtNheight, &height,
                XtNx, &x, XtNy, &y,
                NULL);
  XtTranslateCoords(w, (Position) width/4, (Position) height/4, &x, &y); 
  XtVaSetValues(ps, XtNx, x, XtNy, y, NULL);
}



static void 
PopupDialogCancel(Widget w, XtPointer client_data, XtPointer call_data)
{
  PopupDialogData *pdd = (PopupDialogData *)client_data;
  Widget ps = pdd->popup_shell;
  Widget db = pdd->dialog_button;
  
  XtPopdown(ps);
  XtSetSensitive(db, TRUE);
}




static void 
PopupDialogOK(Widget w, XtPointer client_data, XtPointer call_data)
{
  PopupDialogData *pdd = (PopupDialogData *)client_data;
  Widget ps = pdd->popup_shell;
  Widget db = pdd->dialog_button;
  Widget tf = pdd->text_field;
  String string;
  BOOLEAN verified = NO;

  XtVaGetValues(tf, XtNstring, &string, NULL);

  TypeInParseLine(string);
  
  if (pdd->verify_proc != NULL) { /* Have verify procedure */
    verified = pdd->verify_proc();
  } else {                        /* Haven't.  Test in place */
    if        (pdd->value_type == ESIDialogValueAny) {
      verified = YES;
    } else if (pdd->value_type == ESIDialogValueNumber) {
      if (TypeInGetTokenType(1) == NUMBER) verified = YES;
      else ErrorMessage(ELIXIR_ERROR_CLASS, 1,
                        "You should input a number", ERROR_GRADE);
    } else if (pdd->value_type == ESIDialogValueString) {
      if (TypeInGetTokenType(1) == STRING) verified = YES;
      else ErrorMessage(ELIXIR_ERROR_CLASS, 1,
                        "You should input a string", ERROR_GRADE);
    }
  }

  if (verified) {
    XtPopdown(ps);
    XtSetSensitive(db, TRUE);
    if (pdd->callback != NULL) (*pdd->callback)(w, pdd->client_data, string);
  }
}


static void 
ReturnHitInPopupDialog(Widget w, XEvent *event, String *params,
                       Cardinal *num_params)
{
  Widget form, ok = NULL;

  form = XtNameToWidget(XtParent(w), "form");
  if (form != NULL)
    ok = XtNameToWidget(form, "ok");
  if (ok != NULL) XtCallCallbacks(ok, XtNcallback, NULL);
}



void
handle_command(Widget w, char *s);

void
ESIHandleCmd(char *cmd_string)
{
  char buf[1024];
  strcpy(buf, cmd_string);
  handle_command(ESITopLevelWidget(), buf);
}

Widget
ESIAddMenuItem(char *widget_name, char *label,
               Widget menu_shell, Arg *args, int argn,
               XtCallbackProc callback, XtPointer client_data)
{
  Widget b;
  Arg al[3];
  int ac;
  
  b = XtCreateManagedWidget(widget_name, smeBSBObjectClass,
                            menu_shell, args, argn);
  if (label != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNlabel, label); ac++;
    XtSetValues(b, al, ac);
  }
  XtAddCallback(b, XtNcallback, callback, (XtPointer)client_data);

  return b;
}


FILE *
ESISelFile(char *prompt, char *ok, char *cancel, char *failed,
           char *init_path, char *mode, char **name_return)
{
  FILE *fp;
  
  fp = XsraSelFile(ESITopLevelWidget(),
                   prompt, ok, cancel, failed,
                   init_path, mode, NULL,
                   name_return);
  return fp;
}


Widget
ESIAddPopdownMenu(char *button_widget, char *label,
                  Widget parent, Arg *args, int argn,
                  Widget *menu_shell)
{
  Widget b;
  Arg al[3];
  int ac;
  
  b = XtCreateManagedWidget(button_widget, menuButtonWidgetClass,
                            parent, args, argn);
  if (label != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNlabel, label); ac++;
    XtSetValues(b, al, ac);
  }
  *menu_shell = XtCreatePopupShell("menu", simpleMenuWidgetClass,
                                   b, NULL, 0);
  return b;
}


Widget
GetFromFreeInfoViewers(void);

void
ESIPopupInfo(char *string)
{
  Widget info, txt;
  Arg al[3];
  int ac;
  Position x, y;
  Dimension width, height;
  
  info = GetFromFreeInfoViewers();
  txt  = XtNameToWidget(info, "*infotext");
  if (txt != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, ""); ac++;
    XtSetArg(al[ac], XtNtype, XawAsciiString); ac++;
    XtSetValues(txt, al, ac);

    XtVaGetValues(XtParent(info), XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(XtParent(info),                /* Widget */
                      (Position) width/4,        /* x */
                      (Position) height/4,       /* y */
                      &x, &y);          /* coords on root window */
    XtVaSetValues(info, XtNx, x, XtNy, y, NULL);

    ac = 0;
    XtSetArg(al[ac], XtNstring, string); ac++;
    XtSetValues(txt, al, ac);

    XtPopup(info, XtGrabNone);
  } else {
    fprintf(stderr, "No info?");
  }
}



void
ESIPopupSingleInfo(char *string)
{
  Widget info, txt;
  Arg al[3];
  int ac;
  Position x, y;
  Dimension width, height;
  
  info = MakeNewSingleInfoViewer();
  txt  = XtNameToWidget(info, "*infotext");
  if (txt != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, ""); ac++;
    XtSetArg(al[ac], XtNtype, XawAsciiString); ac++;
    XtSetValues(txt, al, ac);

    XtVaGetValues(XtParent(info), XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(XtParent(info),                /* Widget */
                      (Position) width/4,        /* x */
                      (Position) height/4,       /* y */
                      &x, &y);          /* coords on root window */
    XtVaSetValues(info, XtNx, x, XtNy, y, NULL);

    ac = 0;
    XtSetArg(al[ac], XtNstring, string); ac++;
    XtSetValues(txt, al, ac);

    XtPopup(info, XtGrabExclusive);
  } else {
    fprintf(stderr, "No info?");
  }
}



void
ESIPopupInfoFromFile(char *file_name)
{
  Widget info, txt;
  Arg al[3];
  int ac;
  Position x, y;
  Dimension width, height;
  
  info = GetFromFreeInfoViewers();
  txt  = XtNameToWidget(info, "*infotext");
  if (txt != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, file_name); ac++;
    XtSetArg(al[ac], XtNtype, XawAsciiFile); ac++;
    XtSetValues(txt, al, ac);

    XtVaGetValues(XtParent(info), XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(XtParent(info),                /* Widget */
                      (Position) width/4,        /* x */
                      (Position) height/4,       /* y */
                      &x, &y);          /* coords on root window */
    XtVaSetValues(info, XtNx, x, XtNy, y, NULL);

    XtPopup(info, XtGrabNone);
  } else {
    fprintf(stderr, "No info?");
  }
}



void
ESIPopupSingleInfoFromFile(char *file_name)
{
  Widget info, txt;
  Arg al[3];
  int ac;
  Position x, y;
  Dimension width, height;
  
  info = MakeNewSingleInfoViewer();
  txt  = XtNameToWidget(info, "*infotext");
  if (txt != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNstring, file_name); ac++;
    XtSetArg(al[ac], XtNtype, XawAsciiFile); ac++;
    XtSetValues(txt, al, ac);

    XtVaGetValues(XtParent(info), XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(XtParent(info),                /* Widget */
                      (Position) width/4,        /* x */
                      (Position) height/4,       /* y */
                      &x, &y);          /* coords on root window */
    XtVaSetValues(info, XtNx, x, XtNy, y, NULL);

    XtPopup(info, XtGrabExclusive);
  } else {
    fprintf(stderr, "No info?");
  }
}



static Widget confirm_popup_shell,
                   confirm_popup_label,
                   confirm_popup_paned,
                      confirm_popup_form,
                         confirm_popup_OK,
                         confirm_popup_cancel;


static void (*confirm_action_ok)(XtPointer client_data) = NULL;
static void (*confirm_action_cancel)(XtPointer client_data) = NULL;
static XtPointer confirm_action_ok_client_data = NULL;
static XtPointer confirm_action_cancel_client_data = NULL;

static void
run_confirm_action_ok(Widget w, XtPointer client_data, XtPointer call_data)
{
  XtPopdown(confirm_popup_shell);
  if (confirm_action_ok != NULL)
    (*confirm_action_ok)(confirm_action_ok_client_data);
}

static void
run_confirm_action_cancel(Widget w, XtPointer client_data, XtPointer call_data)
{
  XtPopdown(confirm_popup_shell);
  if (confirm_action_cancel != NULL)
    (*confirm_action_cancel)(confirm_action_cancel_client_data);
}


void
SetupConfirmDialog(void)
{
  Arg al[10];
  int ac;

  if (confirm_popup_shell != NULL) return;
  
  confirm_popup_shell = XtCreatePopupShell("confirm_dialog",
                                           transientShellWidgetClass,
                                           ESITopLevelWidget(), NULL, 0);
  
  confirm_popup_paned = XtCreateManagedWidget("confirm_popup_paned",
                                              panedWidgetClass,
                                              confirm_popup_shell, NULL, 0);
  ac = 0;
  confirm_popup_label = ESIAddButton("confirm_popup_label",
                                     "", labelWidgetClass,
                                     confirm_popup_paned, al, ac, NULL, NULL);
  confirm_popup_form = XtCreateManagedWidget("confirm_popup_form",
                                             formWidgetClass,
                                             confirm_popup_paned, NULL, 0);
  ac = 0;
  confirm_popup_OK     = ESIAddButton("confirm_popup_OK",      "     OK      ",
                                      commandWidgetClass,
                                      confirm_popup_form, al, ac,
                                      run_confirm_action_ok, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromHoriz, confirm_popup_OK); ac++;
  confirm_popup_cancel = ESIAddButton("confirm_popup_cancel",  "    CANCEL   ",
                                      commandWidgetClass,
                                      confirm_popup_form, al, ac,
                                      run_confirm_action_cancel, NULL);
}


void 
ESIPopupConfirmDialog(Widget for_widget, char *with_label,
                      ESIConfirmActionProc ok_action,
                      XtPointer ok_client_data,
                      ESIConfirmActionProc cancel_action,
                      XtPointer cancel_client_data)
{
  Position x, y;
  Dimension width, height;

  SetupConfirmDialog();

  XtVaSetValues(confirm_popup_label, XtNlabel, with_label, NULL);

  if (for_widget != NULL) {
    XtVaGetValues(for_widget, XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(for_widget,                /* Widget */
                      (Position) width/4,        /* x */
                      (Position) height/4,       /* y */
                      &x, &y);          /* coords on root window */
    XtVaSetValues(confirm_popup_shell, XtNx, x, XtNy, y, NULL);
  }
  
  confirm_action_ok                 = ok_action;
  confirm_action_ok_client_data     = ok_client_data;
  confirm_action_cancel             = cancel_action;
  confirm_action_cancel_client_data = cancel_client_data;
  
  XtPopup(confirm_popup_shell, XtGrabExclusive);
}


static void 
PopupPalette(Widget w, XtPointer client_data, XtPointer call_data);
static void
CreatePalette(char *palette, char *paned_name, 
                Arg *args, int argn, Widget *shell, Widget *paned);
static void 
ClosePalette(Widget w, XtPointer client_data, XtPointer call_data);

Widget
ESIAddPalette(char *paned_name, char *menu_label, Widget parent_paned,
                Arg *args, int argn,
                Widget *paned_return)
{
  char buff[132];
  Widget shell;

  strcpy(buff, paned_name);
  strcat(buff, "_paned");
  CreatePalette(menu_label, buff, args, argn, &shell, paned_return);

  return  ESIAddButton(paned_name, menu_label, 
                       commandWidgetClass, parent_paned,
                       NULL, 0, PopupPalette, (XtPointer)shell); 
}

static void 
PopupDialog(Widget w, XtPointer client_data, XtPointer call_data);

Widget
ESIAddPopupDialog(char *dialog_button_name, char *dialog_button_label,
                  char *dialog_prompt, char *initial_value,
                  Widget parent, Arg *args, int argn,
                  XtCallbackProc callback, XtPointer client_data,
                  ESIDialogValueType value_type,
                  ESIVerifyValueProc verify_proc)
{
	return(ESIAddExtensiblePopupDialog(dialog_button_name, dialog_button_label,
																		 NULL, dialog_prompt, initial_value, NULL,
																		 parent, args, argn,
																		 callback, client_data,
																		 value_type,
																		 verify_proc));
}


Widget
ESIAddExtensiblePopupDialog(char *dialog_button_name, char *dialog_button_label,
														char *dialog_name, char *dialog_prompt, char *initial_value,
														Widget *ext_form,
														Widget parent, Arg *args, int argn,
														XtCallbackProc callback, XtPointer client_data,
														ESIDialogValueType value_type,
														ESIVerifyValueProc verify_proc)
{
  Widget popup_shell, dialog_button, text_field;
  Widget label, form, paned, ok, cancel;
  PopupDialogData *pdd;
  Arg al[10];
  int ac;

  pdd = (PopupDialogData *)make_node(sizeof(PopupDialogData));

  ac = 0;
  XtSetArg(al[ac], XtNtitle, "DIALOG"); ac++;
	if(dialog_name != NULL)
		popup_shell = XtCreatePopupShell(dialog_name,
																		 transientShellWidgetClass,
																		 ESITopLevelWidget(), al, ac);
	else
		popup_shell = XtCreatePopupShell("popup_shell",
																		 transientShellWidgetClass,
																		 ESITopLevelWidget(), al, ac);

	if(dialog_button_name != NULL)
		dialog_button = ESIAddButton(dialog_button_name,
																 dialog_button_label, commandWidgetClass,
																 parent, args, argn, PopupDialog, (XtPointer)popup_shell);
	else
		dialog_button = ESIAddButton("dialog_button",
																 dialog_button_label, commandWidgetClass,
																 parent, args, argn, PopupDialog, (XtPointer)popup_shell);

/* make the extensible popup-shell not exclusive
	 to enable eventual interaction with the drawing window via commands located
	 in ext_form */

	if(ext_form != NULL)
		XtAddCallback(dialog_button, XtNcallback, XtCallbackNone, popup_shell);
	else
		XtAddCallback(dialog_button, XtNcallback, XtCallbackExclusive, popup_shell);

  ac = 0;
  XtSetArg(al[ac], XtNwidth, 300); ac++;
  paned = XtCreateManagedWidget("paned", panedWidgetClass,
                                popup_shell, al, ac);
  label = ESIAddButton("label", dialog_prompt, labelWidgetClass,
                       paned, NULL, 0, NULL, NULL);
  ac = 0;
  if (initial_value != NULL)
    XtSetArg(al[ac], XtNstring, initial_value); ac++;
  XtSetArg(al[ac], XtNeditType, XawtextEdit); ac++;
  XtSetArg(al[ac], XtNresizable, True); ac++;
  XtSetArg(al[ac], XtNresize, XawtextResizeBoth); ac++;
  XtSetArg(al[ac], XtNfromVert, label); ac++;
  XtSetArg(al[ac], XtNright, XawChainRight); ac++;
  text_field = ESIAddButton("text_field", " ", asciiTextWidgetClass,
                            paned, al, ac, NULL, NULL);
  XtAppAddActions(XtWidgetToApplicationContext(parent),
                  remap_return_popup_dialog,
                  XtNumber(remap_return_popup_dialog));
  tt2 = XtParseTranslationTable("#override <KeyPress>Return: retActOK()");
  XtOverrideTranslations(text_field, tt2);

  pdd->popup_shell   = popup_shell;
  pdd->dialog_button = dialog_button;
  pdd->text_field    = text_field;
  pdd->value_type    = value_type;
  pdd->callback      = callback;
  pdd->client_data   = client_data;
  pdd->verify_proc   = verify_proc;

	if(ext_form != NULL){
		ac = 0;
		*ext_form = XtCreateManagedWidget("extensible", formWidgetClass, paned, al, ac);
	}

	ac = 0;
  form = XtCreateManagedWidget("form", formWidgetClass, paned, al, ac);
  ac = 0;
  ok  = ESIAddButton("ok", "  OK  ", commandWidgetClass,
                     form, al, ac, PopupDialogOK, (XtPointer)pdd);
  ac = 0;
	XtSetArg(al[ac], XtNfromHoriz, ok); ac++;
  cancel = ESIAddButton("cancel",  "CANCEL", commandWidgetClass,
                        form, al, ac, PopupDialogCancel, (XtPointer)pdd);

  return dialog_button;
}






static void
CreatePalette(char *palette, char *paned_name, 
                Arg *args, int argn, Widget *shell, Widget *paned)
{
  Widget close_pal;
  Arg al[2];
  int ac;
  
  *shell = XtCreatePopupShell("palette", transientShellWidgetClass,
                              ESITopLevelWidget(), args, argn);
  ac = 0;
  XtSetArg(al[ac], XtNtitle, palette); ac++;
  XtSetValues(*shell, al, ac);
  *paned = XtCreateManagedWidget(paned_name, panedWidgetClass,
                                *shell, args, argn);
  ac = 0;
  XtSetArg(al[ac], XtNshowGrip, False); ac++;
  XtSetValues(*paned, al, ac);

  close_pal = ESIAddButton("close", "Close", commandWidgetClass, *paned,
                           args, argn, ClosePalette, (XtPointer)*shell);
}



static void 
PopupPalette(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget pal_shell = (Widget)client_data;
  Position x = 0, y = 0;
  Dimension width, height;

  if (XtIsRealized(pal_shell))
    XtVaGetValues(pal_shell, XtNx, &x, XtNy, &y, NULL);
  if (x == 0 && y == 0) {
    XtVaGetValues(w, XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(w, (Position) width/2, (Position) height, &x, &y);
    XtVaSetValues(pal_shell, XtNx, x, XtNy, y, NULL);
  }
  XtRealizeWidget(pal_shell);
  XtPopup(pal_shell, XtGrabNone);
}


static void 
ClosePalette(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget pal_shell = (Widget) client_data;

  XtPopdown(pal_shell);
}



static void
SetupErrorDialog(void);
static void 
PopupErrorDialog(const char *with_label, EErrGrade grade);
static void 
PopDownErrorDialog(Widget w, XtPointer client_data, XtPointer call_data);


static Widget error_dialog_label = NULL;
static Widget error_dialog_popup_shell = NULL;

static void
SetupErrorDialog(void)
{
  Widget form, ok;
  Arg al[2];
  int ac;

  if (error_dialog_popup_shell != NULL) return;

  ac = 0;
  XtSetArg(al[ac], XtNtitle, "ERROR"); ac++;
  error_dialog_popup_shell = XtCreatePopupShell("error_popup_shell",
                                                transientShellWidgetClass,
                                                ESITopLevelWidget(), al, ac);

  ac = 0;
  XtSetArg(al[ac], XtNwidth, 300); ac++;
  form = XtCreateManagedWidget("form", formWidgetClass,
                               error_dialog_popup_shell, al, ac);

  ac = 0;
  XtSetArg(al[ac], XtNtop, XawChainTop); ac++;
  XtSetArg(al[ac], XtNbottom, XawChainBottom); ac++;
  error_dialog_label = ESIAddButton("error_dialog_label",
                                    " ", labelWidgetClass,
                                    form, al, ac, NULL, NULL);


  ac = 0;
  XtSetArg(al[ac], XtNfromVert, error_dialog_label); ac++;
  ok  = ESIAddButton("ok", "  OK  ", commandWidgetClass,
                     form, al, ac, PopDownErrorDialog, NULL);
}



static void 
PopupErrorDialog(const char *with_label, EErrGrade grade)
{
  Position x, y;
  Dimension width, height;
  char buf[1024];

  SetupErrorDialog();
  
  XtVaGetValues(ESITopLevelWidget(),
                XtNwidth, &width,
                XtNheight, &height,
                NULL);
    
  XtTranslateCoords(ESITopLevelWidget(), /* Widget */
                    (Position) width/4,        /* x */
                    (Position) height/4,       /* y */
                    &x, &y);          /* coords on root window */

  XtVaSetValues(error_dialog_popup_shell, XtNx, x, XtNy, y, NULL);
  if      (grade == FATAL_ERROR_GRADE)
    sprintf(buf, "FATAL ERROR: %s", with_label);
  else if (grade == ERROR_GRADE) 
    sprintf(buf, "ERROR: %s", with_label);
  else if (grade == WARNING_GRADE) 
    sprintf(buf, "WARNING: %s", with_label);
  else if (grade == MESSAGE_GRADE) 
    sprintf(buf, "%s", with_label);
  
  XtVaSetValues(error_dialog_label, XtNlabel, buf, NULL);
  XtPopup(error_dialog_popup_shell, XtGrabExclusive);
}


static void 
PopDownErrorDialog(Widget w, XtPointer client_data, XtPointer call_data)
{
  XtPopdown(error_dialog_popup_shell);
}



int
ErrorMessage(int class, int serial,
             const char *deflt_msg, EErrGrade grade)
{
  PopupErrorDialog(deflt_msg, grade);
  return 1;
}



LIST free_info_viewers = NULL;
LIST used_info_viewers = NULL;

static void
DismissInfoViewer(Widget w, XtPointer ptr, XtPointer call_data)
{
  Widget info = (Widget)ptr;
  XtPopdown(info);
  delete_list_node(used_info_viewers, info);
  add_to_tail(free_info_viewers, info);
}


static void
SaveInfoViewer(Widget w, XtPointer ptr, XtPointer call_data);

static void
SaveInfoViewer(Widget w, XtPointer ptr, XtPointer call_data)
{
  char *name = NULL;
  FILE *fp;
  Widget infotext = (Widget)ptr;
  int ac;
  Arg al[10];
  int type;
  String str;
  char buf[256]; 

  fp = ESISelFile("Select file in which to save",
                  "   OK   ", "CANCEL", NULL, NULL, "w", &name);
  if (fp) {
    ac = 0;
    XtSetArg(al[ac], XtNtype, &type); ac++;
    XtGetValues(infotext, al, ac);
    ac = 0;
    XtSetArg(al[ac], XtNstring, &str); ac++;
    XtGetValues(infotext, al, ac);
    if (type == XawAsciiFile) {
      sprintf(buf, "cp %s %s", str, name);
      system(buf);
    } else {
      fprintf(fp, "%s\n", str);
    }
    fclose(fp);
  } else {
    return; /* Cancelled */
  }
}

static Widget
MakeNewInfoViewer(void);

static Widget
MakeNewInfoViewer(void)
{
  Widget info, form, infotext, dismiss, saveas;
  int ac;
  Arg al[10];
  static int info_num = 0;
  char buf[32];
  
  sprintf(buf, "INFO %d", ++info_num);
  ac = 0;
  XtSetArg(al[ac], XtNtitle, buf); ac++;
  info = XtCreatePopupShell("info", transientShellWidgetClass,
                            ESITopLevelWidget(), al, ac);
  form = XtCreateManagedWidget("form", formWidgetClass, info, NULL, 0);
  
  ac = 0;
  XtSetArg(al[ac], XtNscrollVertical, XawtextScrollWhenNeeded); ac++;
  XtSetArg(al[ac], XtNscrollHorizontal, XawtextScrollWhenNeeded); ac++;
  XtSetArg(al[ac], XtNautoFill, True); ac++;
  XtSetArg(al[ac], XtNresizable, True); ac++;
  XtSetArg(al[ac], XtNheight, 50); ac++;
  XtSetArg(al[ac], XtNwidth, 200); ac++;
  XtSetArg(al[ac], XtNtop, XawChainTop); ac++;
  XtSetArg(al[ac], XtNbottom, XawChainBottom); ac++;
  infotext =
    ESIAddButton("infotext", "", asciiTextWidgetClass,
                 form, al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, infotext); ac++;
  XtSetArg(al[ac], XtNbottom, XawChainBottom); ac++;
  XtSetArg(al[ac], XtNtop, XawChainBottom); ac++;
  dismiss =
    ESIAddButton("dismiss", "DISMISS", commandWidgetClass,
                 form, al, ac, DismissInfoViewer, (XtPointer)info);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, infotext); ac++;
  XtSetArg(al[ac], XtNfromHoriz, dismiss); ac++;
  XtSetArg(al[ac], XtNbottom, XawChainBottom); ac++;
  XtSetArg(al[ac], XtNtop, XawChainBottom); ac++;
  saveas =
    ESIAddButton("saveas", "SAVE AS", commandWidgetClass,
                 form, al, ac, SaveInfoViewer, (XtPointer)infotext);
  return info;
}


static void
DismissSingleInfoViewer(Widget w, XtPointer ptr, XtPointer call_data)
{
  Widget info = (Widget)ptr;
  XtPopdown(info);
}



static Widget
MakeNewSingleInfoViewer(void)
{
  static Widget info = NULL;
	Widget form, infotext, acknowledge;
  int ac;
  Arg al[10];
  char buf[32];

	if(info != NULL)return(info);
  
  sprintf(buf, "INFO");
  ac = 0;
  XtSetArg(al[ac], XtNtitle, buf); ac++;
  info = XtCreatePopupShell("info", transientShellWidgetClass,
                            ESITopLevelWidget(), al, ac);
  form = XtCreateManagedWidget("form", formWidgetClass, info, NULL, 0);
  
  ac = 0;
  XtSetArg(al[ac], XtNscrollVertical, XawtextScrollWhenNeeded); ac++;
  XtSetArg(al[ac], XtNscrollHorizontal, XawtextScrollWhenNeeded); ac++;
  XtSetArg(al[ac], XtNautoFill, True); ac++;
  XtSetArg(al[ac], XtNresizable, True); ac++;
  XtSetArg(al[ac], XtNheight, 100); ac++;
  XtSetArg(al[ac], XtNwidth, 400); ac++;
  XtSetArg(al[ac], XtNtop, XawChainTop); ac++;
  XtSetArg(al[ac], XtNbottom, XawChainBottom); ac++;
  infotext =
    ESIAddButton("infotext", "", asciiTextWidgetClass,
                 form, al, ac, NULL, NULL);
  ac = 0;
  XtSetArg(al[ac], XtNfromVert, infotext); ac++;
  XtSetArg(al[ac], XtNbottom, XawChainBottom); ac++;
  XtSetArg(al[ac], XtNtop, XawChainBottom); ac++;
  acknowledge =
    ESIAddButton("acknowledge", "ACKNOWLEDGE", commandWidgetClass,
                 form, al, ac, DismissSingleInfoViewer, (XtPointer)info);
  return info;
}



Widget
GetFromFreeInfoViewers(void)
{
  Widget info;
  
  if (free_info_viewers == NULL) {
    free_info_viewers = make_list();
    used_info_viewers = make_list();
  } 
  if (list_empty(free_info_viewers)) {
    info =  MakeNewInfoViewer();
    add_to_tail(free_info_viewers, info);
  }
  info = (Widget)get_from_head(free_info_viewers);
  add_to_tail(used_info_viewers, info);
  return info;
}



void
ESIPopupAndRun(void)
{
  ESIPopup();
  XtAppMainLoop(XtWidgetToApplicationContext(ESITopLevelWidget()));
}


void
ESIPopup(void)
{
  Arg args[2];
  int argn;

  argn = 0;
  XtSetArg(args[argn], XtNiconic, FALSE); argn++;
  XtSetArg(args[argn], XtNinitialState, NormalState); argn++;
  XtSetValues(ESITopLevelWidget(), args, argn);
  XtRealizeWidget(ESITopLevelWidget());
}



Widget
ESIAddButton(char *name, char *label, WidgetClass wclass, Widget parent,
          Arg *args, int argn,
          XtCallbackProc callback, XtPointer client_data)
{
  Widget w;
  Arg al[2];
  int ac;

  
  w = XtCreateManagedWidget(name, wclass, parent, args, argn);
  if (label != NULL) {
    ac = 0;
    XtSetArg(al[ac], XtNlabel, label); ac++;
    XtSetValues(w, al, ac);
  }
  
  ac = 0;
  XtSetArg(al[ac], XtNshowGrip, False); ac++;
  XtSetValues(w, al, ac);
  
  if (callback != NULL)
    XtAddCallback(w, XtNcallback, callback, client_data);

  return w;
}



/* These are private data of the ESI package */
static   XtAppContext app_con;
static   Widget toplevel, topPaned;
/* Application name -- saved by BuildTop */
static char *app_name = NULL;


/* Returns widget of class panedWidgetClass; the programmer can add to this */
/* widget commands, labels, text as needed using ESIAddButton. */

#undef XtAppInitialize_NEEDS_Cardinal
#if XtSpecificationRelease <= 4
#   define XtAppInitialize_NEEDS_Cardinal
#endif


int elixir_default_visual = 0;
int elixir_best_visual = 0;
int elixir_default_cmap   = 0;

static void
check_visual_and_cmap_options(int *argc, char **argv);


#define COMPACT_ARGV(argc, argv, i)             \
  {                                             \
    int _i_, _n_ = (argc) - 1;                  \
    for (_i_ = i; _i_ < _n_; _i_++) {           \
      argv[_i_] = argv[_i_+1];                  \
    }                                           \
    (argc)--;                                   \
  }


static void
check_visual_and_cmap_options(int *argc, char **argv)
{
  int i = *argc - 1;
  
  while (i > 0) {
    if (argv[i] != NULL) {
      if (strcmp("-defcmap", argv[i]) == 0) {
        elixir_default_cmap = 1;
        COMPACT_ARGV(*argc, argv, i);
      }
      if (strcmp("-defvisual", argv[i]) == 0) {
        elixir_default_visual = 1;
        COMPACT_ARGV(*argc, argv, i);
      }
      if (strcmp("-bestvisual", argv[i]) == 0) {
        elixir_best_visual = 1;
        COMPACT_ARGV(*argc, argv, i);
      }
    }
    i--;
  }
}



Widget
BuildTop(int *argc, char **argv, char *Class, char **fallback_resources)
{
#ifdef XtAppInitialize_NEEDS_Cardinal
  Cardinal argc_card = *argc;
#endif
  
  app_name = strdup(argv[0]);

  
  toplevel = XtAppInitialize(&app_con, Class,
                             NULL, 0,
#ifdef XtAppInitialize_NEEDS_Cardinal
                             &argc_card,
#else
                             argc,
#endif
                             argv,
                             fallback_resources, NULL, 0);

  check_visual_and_cmap_options(argc, argv);
  
  topPaned =
    XtCreateManagedWidget("topPaned", panedWidgetClass, toplevel, NULL, 0);

#ifdef XtAppInitialize_NEEDS_Cardinal
  *argc = argc_card;
#endif
  
  return topPaned;
}


Widget
ESITopLevelWidget(void)
{
  return toplevel;
}



static int force_proceed = 0;


#include <ctype.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>

#define ESCAPE '\033'

static int 
get_key_pressed(XEvent *ev, KeySym keysym, Mask modifier);

#define PROCEED XK_p
#define STOP    XK_s
#define EXIT    XK_x
#define FIT_ALL XK_a

#define PROCEED_MESSAGE " press ^p to proceed."
#define STOP_MESSAGE    " press ^s to stop."
#define EXIT_MESSAGE    " press ^x to exit."
#define MESSAGE_SUFFIX  " ^p=proceed;^s=stop;^x=exit"

#if 0
static int 
escape_loop(XEvent *ev)
{
  char buffer[12];
  KeySym ksym;
  int lngth;
  static XComposeStatus	c_stat;
  
  if (ev->type == KeyPress) {
    lngth = XLookupString(&(ev->xkey), buffer, 12, &ksym, &c_stat);
    buffer[lngth] = '\0';
    if (buffer[0] == ESCAPE) {
      return TRUE;
    }
  }
  return FALSE;
}
#endif



static int 
get_key_pressed(XEvent *ev, KeySym keysym, Mask modifier)
{
  char buffer[12];
  KeySym ksym;
  int lngth;
  static XComposeStatus	c_stat;
  
  if (ev->type == KeyPress) {
    lngth = XLookupString(&(ev->xkey), buffer, 12, &ksym, &c_stat);
    buffer[lngth] = '\0';
    if ((ksym == keysym) && (modifier != 0? ev->xkey.state: True))  {
      return TRUE;
    }
  }
  return FALSE;
}


static int
fit_into_all_views(NODE ptr, NODE view);


static int
fit_into_all_views(NODE ptr, NODE view)
{
  EVFitAllIntoView((EView *)view);
  return 1;
}

void
ESIEventLoopProceed(void)
{
  force_proceed = 1;
}


void
ESIEventLoopStop(void)
{
  force_proceed = 0;
}


static void 
do_quit(XtPointer ptr)
{
  XtCloseDisplay(XtDisplay(ESITopLevelWidget()));
  exit(ESISetExitCode(ELIXIR_OK_EXIT));
}




int 
ESIEventLoop(BOOLEAN stop_cmd, char *msg)
{
  XEvent event;
  char buffer[1024];
  BOOLEAN stop_here = stop_cmd;

  if ((!XPending(XtDisplay(ESITopLevelWidget()))) && (!stop_here)) {
    return 0;
  }

  if (stop_here) {
    force_proceed = 0;
    if (msg != NULL) {
      strcpy(buffer, msg); strcat(buffer, MESSAGE_SUFFIX); 
      ERptStatusMessage(1, 2, buffer);
    }
  }

  while (!force_proceed) {
    XtAppNextEvent(app_con, &event);
    if (get_key_pressed(&event, STOP, ControlMask)) {
      stop_here = YES;
    } else if (get_key_pressed(&event, PROCEED, ControlMask)) {
			force_proceed = 0;
			return 2;
      break;
    } else if (get_key_pressed(&event, FIT_ALL, ControlMask)) {
      EMDispatchToDependentViews(ESIModel(), fit_into_all_views, NULL);
    } else if (get_key_pressed(&event, EXIT, ControlMask)) {
      ESIPopupConfirmDialog(ESITopLevelWidget(),
                            "Really quit?", do_quit, NULL, NULL, NULL);
    } else {
      if ((!stop_here) && (!XPending(XtDisplay(ESITopLevelWidget())))) {
				XtDispatchEvent(&event);
				force_proceed = 0;
				return 1;
        break;
      }
    }
    XtDispatchEvent(&event); 
  }
	return 3;
}
