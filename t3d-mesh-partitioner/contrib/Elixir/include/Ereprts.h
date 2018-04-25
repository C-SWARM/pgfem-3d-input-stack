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

#ifndef EREPRTS_H
#define EREPRTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Etypes.h"

/* Definitions of message classes and serial numbers */

#define ELIXIR_MESSAGE_CLASS     -131313
#define ELIXIR_WARNING_CLASS     -10000
#define ELIXIR_ERROR_CLASS       -9000
#define ELIXIR_FATAL_ERROR_CLASS -8000
#define ELIXIR_PROMPT_CLASS      -7000

/* Plain messages */

#define ELIXIR_ANY_MESSAGE        "any message"                     /* 1 */
#define ELIXIR_KEYPNT_LOC_MESSAGE /* construed in EVSetSnapPoint */ /* 2 */

/* Warning messages */

#define ELIXIR_WARNING_1 "Cannot flip state of a non-existent command" /* 1 */
#define ELIXIR_WARNING_2 "Font not activated"                          /* 2 */
#define ELIXIR_WARNING_3 "Color not activated--allocate failed"        /* 3 */
#define ELIXIR_WARNING_4 "Color not activated--parsing failed"         /* 4 */
#define ELIXIR_WARNING_5 " "         /* 5 */

/* Error messages */

#define ELIXIR_ERROR_1 "File could not be opened"       /* 1 */
#define ELIXIR_ERROR_2 "File could not be closed"       /* 2 */
#define ELIXIR_ERROR_3 "File could not be emptied"      /* 3 */
#define ELIXIR_ERROR_4 "Not enough memory for Z-buffer" /* 4 */
#define ELIXIR_ERROR_5 "Failed malloc."                 /* 5 */

/* Prompts for internal operations  */

#define MESSAGE_CLEAR_PROMPT   0
#define MESSAGE_FV1            1
#define MESSAGE_FV2            2
#define MESSAGE_FV3            3
#define MESSAGE_FV4            4
#define MESSAGE_FV5            5
#define MESSAGE_SEL1           6
#define MESSAGE_SEL2           7
#define MESSAGE_SEL3           8
#define MESSAGE_SEL4           9
#define MESSAGE_ATV1          10


#ifdef EREPRTS_PRIVATE_HEADER

char *elixir_default_prompts[] = {
/* MESSAGE_CLEAR_PROMPT */ 
                   "",
/* MESSAGE_FV1  */ "B1=window|^B1=pan|^B2=zoom|SB2=fit|^SB1=rotate|B3=done",
/* MESSAGE_FV2  */ "B1=2nd corner|B3=cancel",
/* MESSAGE_FV3  */ "B1up=stop rotating",
/* MESSAGE_FV4  */ "B1up=stop panning",
/* MESSAGE_FV5  */ "B2up=stop zooming",
/* MESSAGE_SEL1 */ "B1=pick|B2=accept|^B1=start fence|B3=cancel",
/* MESSAGE_SEL2 */ "B2=accept this one|B3=reject this one",
/* MESSAGE_SEL3 */ "Accept/reject first graphic: B2=accept|B3=reject",
/* MESSAGE_SEL4 */ "^B1=end fence|B3=reject",
/* MESSAGE_ATV1 */ "B1=select view|B3=done",
   };
#  undef EREPRTS_PRIVATE_HEADER
#else  /* !EREPRTS_PRIVATE_HEADER */
  
extern char *elixir_default_prompts[];

#endif

typedef enum { FATAL_ERROR_GRADE = 0,
               ERROR_GRADE,
               WARNING_GRADE,
               MESSAGE_GRADE
             }                   EErrGrade;

/* Customized error handler must be of this type */

typedef int (*EErrMessagehandler)(int msg_class, int serial,
                                  const char *deflt_msg, EErrGrade grade);

/* Customized prompt printer must be of this type */

typedef int (*EPromptRoutine)(int msg_class, int serial,
                              const char *default_prompt);

/* Customized status printer must be of this type */

typedef int (*EStatusRoutine)(int msg_class, int serial,
                              const char *default_status_msg);

/* Customized pointer tracker must be of this type */

typedef void (*EPtrTrackRoutine)(WCRec *loc);

void
ERptPrompt(int msg_class, int serial, const char *prompt);

void
ERptStatusMessage(int msg_class, int serial, const char *msg);

void
ERptErrMessage(int msg_class, int serial,
               const char *deflt_msg, EErrGrade grade);

EErrMessagehandler 
ERptSetErrorHandler(EErrMessagehandler handler);

EPromptRoutine 
ERptSetPromptRoutine(EPromptRoutine routine);

EStatusRoutine 
ERptSetStatusRoutine(EStatusRoutine routine);

void
ERptTrackPointer(WCRec *loc);

EPtrTrackRoutine 
ERptSetPtrTrackRoutine(EPtrTrackRoutine routine);

#ifdef __cplusplus
}
#endif

#endif /* EREPRTS_H */
