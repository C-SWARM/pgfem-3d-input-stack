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
#define EREPRTS_PRIVATE_HEADER
#include "Ereprts.h"
#include "Etypes.h"

#include <stdio.h>

static EPromptRoutine prompt_routine = NULL;
static EStatusRoutine status_msg_routine = NULL;
static EPtrTrackRoutine track_pointer_routine = NULL;
static EErrMessagehandler error_message_handler = NULL;


/* Output prompt.  The default prompt is printed on the standard error. */
/* If another behaviour is needed, alternate prompt-printer must be */
/* set by ERptSetPromptRoutine.   */

void
ERptPrompt (int class, int serial, const char *default_prompt)
{
  static const char *prev_prompt = NULL;

  if (prompt_routine == NULL) {
    if (default_prompt != prev_prompt) {
      fprintf (stderr, "---> %s \n", default_prompt);
    }
    prev_prompt = default_prompt;
  } else {
    (*prompt_routine)(class, serial, default_prompt);
  }
}
 


/* Report warning (error, fatal error).  The default warning printer */
/* prints to stderr.  If another behaviour is needed, alternate */
/* warning-printer must be set by ERptSetErrorHandler. */

void
ERptErrMessage(int class, int serial,
               const char *deflt_msg, EErrGrade grade)
{
  if (error_message_handler == NULL) {
    switch (grade) {
    case FATAL_ERROR_GRADE:
      fprintf (stderr, "Elixir Fatal Error: %s \n", deflt_msg);
      exit(ELIXIR_ERROR_EXIT);
      break;
    case ERROR_GRADE:
      fprintf (stderr, "Elixir Error: %s \n", deflt_msg);
      break;
    case WARNING_GRADE:
      fprintf (stderr, "Elixir Warning: %s \n", deflt_msg);
      break;
    default:
      fprintf (stderr, "Elixir Unknown Message Grade: %s \n", deflt_msg);
      break;
    }
  } else {
    (*error_message_handler)(class, serial, deflt_msg, grade);
  }
}



/* Set warning (error, fatal error) printer.   */

EErrMessagehandler 
ERptSetErrorHandler(EErrMessagehandler handler)
{
  EErrMessagehandler tmp_handler = error_message_handler;
  
  error_message_handler = handler;
  return tmp_handler;
}





/* Set prompt printer.   */

EPromptRoutine 
ERptSetPromptRoutine(EPromptRoutine routine)
{
  EPromptRoutine tmp_routine = prompt_routine;
  
  prompt_routine = routine;
  return tmp_routine;
}



/* Report pointer location in world coordinates */

void
ERptTrackPointer(WCRec *loc)
{
  if (track_pointer_routine != NULL) {
    (*track_pointer_routine)(loc);
  }
}

  


/* Set prompt printer.   */

EPtrTrackRoutine 
ERptSetPtrTrackRoutine(EPtrTrackRoutine routine)
{
  EPtrTrackRoutine tmp_routine = track_pointer_routine;
  
  track_pointer_routine = routine;
  return tmp_routine;
}


/* Output status.  The default status report is to print */
/* on the standard error. */
/* If another behaviour is needed, alternate status-printer must be */
/* set by ERptSetStatusRoutine.   */

void
ERptStatusMessage (int class, int serial, const char *default_msg)
{
  static const char *prev_status_msg = NULL;

  if (status_msg_routine == NULL) {
    if (default_msg != prev_status_msg) {
      fprintf (stderr, "---> %s \n", default_msg);
    }
    prev_status_msg = default_msg;
  } else {
    (*status_msg_routine)(class, serial, default_msg);
  }
}






/* Set status message printer.   */

EStatusRoutine 
ERptSetStatusRoutine(EStatusRoutine routine)
{
  EStatusRoutine tmp_routine = status_msg_routine;
  
  status_msg_routine = routine;
  return tmp_routine;
}
