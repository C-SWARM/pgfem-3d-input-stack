/*
 * Copyright 1989 Software Research Associates, Inc., Tokyo, Japan
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Software Research Associates
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SOFTWARE RESEARCH ASSOCIATES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL SOFTWARE RESEARCH ASSOCIATES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Erik M. van der Poel
 *         Software Research Associates, Inc., Tokyo, Japan
 *         erik@sra.co.jp
 */

#ifndef ESELFILE_H
#define ESELFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/Composite.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>

#include "Econfig.h"

#ifdef STDC_HEADERS
#  include <string.h>
#endif


#define SEL_FILE_CANCEL		-1
#define SEL_FILE_OK		0
#define SEL_FILE_NULL		1
#define SEL_FILE_TEXT		2

#define SF_DO_SCROLL		1
#define SF_DO_NOT_SCROLL	0

typedef struct SFEntry {
	int	statDone;
	char	*real;
	char	*shown;
} SFEntry;

typedef struct SFDir {
	char	*dir;
	char	*path;
	SFEntry	*entries;
	int	nEntries;
	int	vOrigin;
	int	nChars;
	int	hOrigin;
	int	changed;
	int	beginSelection;
	int	endSelection;
	time_t	mtime;
} SFDir;

extern int SFstatus;

extern char SFcurrentPath[], SFstartDir[], SFcurrentDir[];

extern Widget
		selFile,
		selFileCancel,
		selFileField,
		selFileForm,
		selFileHScroll,
		selFileHScrolls[],
		selFileLists[],
		selFileOK,
		selFilePrompt,
		selFileVScrolls[];

extern Display *SFdisplay;

extern int SFcharWidth, SFcharHeight, SFcharAscent;

extern SFDir *SFdirs;

extern int SFdirEnd, SFdirPtr;

extern Pixel SFfore, SFback;

extern Atom SFwmDeleteWindow;

extern XSegment SFsegs[], SFcompletionSegs[];

extern XawTextPosition SFtextPos;

extern void
	SFenterList(),
	SFleaveList(),
	SFmotionList(),
	SFbuttonPressList(),
	SFbuttonReleaseList();

extern void
	SFvSliderMovedCallback(),
	SFvFloatSliderMovedCallback(),
	SFhSliderMovedCallback(),
	SFpathSliderMovedCallback(),
	SFvAreaSelectedCallback(),
	SFhAreaSelectedCallback(),
	SFpathAreaSelectedCallback();

extern int SFupperX, SFlowerY, SFupperY;

extern int SFtextX, SFtextYoffset;

extern int SFentryWidth, SFentryHeight;

extern int SFlineToTextH, SFlineToTextV;

extern int SFbesideText, SFaboveAndBelowText;

extern int SFcharsPerEntry;

extern int SFlistSize;

extern int SFcurrentInvert[];

extern int SFworkProcAdded;

extern Boolean SFworkProc();

extern XtAppContext SFapp;

extern int SFpathScrollWidth, SFvScrollHeight, SFhScrollWidth;

extern char SFtextBuffer[];

extern int SFbuttonPressed;

extern int SFcompareEntries();

extern void SFdirModTimer();

extern char SFstatChar();

extern XtIntervalId SFdirModTimerId;

extern int (*SFfunc)();

#include <sys/stat.h>
#if !defined(S_ISDIR) && defined(S_IFDIR)
#define	S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG) && defined(S_IFREG)
#define	S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISSOCK) && defined(S_IFSOCK)
#define	S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif

#ifndef S_IXUSR
#define S_IXUSR 0100
#endif
#ifndef S_IXGRP
#define S_IXGRP 0010
#endif
#ifndef S_IXOTH
#define S_IXOTH 0001
#endif

#define S_ISXXX(m) ((m) & (S_IXUSR | S_IXGRP | S_IXOTH))


int
SFchdir(char	*path);
int
SFdrawList(int n, int doScroll);
int
SFinitFont(void);
int
SFcreateGC(void);
int
SFupdatePath(void);
int
SFsetText(char *path);
int
SFgetDir(SFDir	*dir);
int
SFdrawLists(int doScroll);
int
SFclearList(int n, int doScroll);

FILE *
XsraSelFile(Widget toplevel,
            char *prompt, char *ok, char *cancel, char *failed,
	    char *init_path, char *mode, int (*show_entry)(),
            char **name_return);

#ifdef __cplusplus
}
#endif

#endif
