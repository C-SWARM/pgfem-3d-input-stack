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
#include "Eview.h"
#include "Egraphic.h"
#include "Emodel.h"
#include "Ecmds.h"
#include "Ereprts.h"
#include "Eutils.h"
#include "Egecmds.h"
#include "Egecb.h"

/* This file holds code connected to the commands. */

static LIST command_list = NULL;
static ECommand *current_command = NULL;

/* ======================================================================== */

#define CmdCheckCommandList() if (command_list == NULL) \
                                if ((command_list = make_list()) == NULL) \
                                   fprintf(stderr, "Failed make_list \n"), \
                                   exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT))

/* ======================================================================== */

ECommand *
CmdCreateCommand(ExecuteProcP exec, UndoProcP undo, caddr_t data)
{
  ECommand *c_p;
  
  CmdCheckCommandList();
  if ((c_p = (ECommand *)make_node(sizeof(ECommand))) == NULL) {
    EUFailedMakeNode("CmdCreateCommand");
  }
  c_p->new_command  = YES;
  c_p->executed     = NO;
  c_p->execute_func = exec;
  c_p->undo_func    = undo;
  c_p->data         = data;
  return c_p;
}
 
/* ======================================================================== */
 
BOOLEAN 
CmdFlipState(ECommand *c_p)
{
  if (c_p == NULL) {
    ERptErrMessage(ELIXIR_WARNING_CLASS, 1, ELIXIR_WARNING_1, WARNING_GRADE);
    return NO;
  } else {
    if (c_p->executed == YES) {
      c_p->executed = NO;
      return CmdUndoCmd(c_p);
    } else {
      c_p->executed = YES;
      return CmdExecuteCmd(c_p);
    }
  }
}

/* ======================================================================== */

BOOLEAN 
CmdExecuteCmd(ECommand *c_p)
{
  if (c_p->execute_func != NULL) {
    if (((*c_p->execute_func)(c_p)) == YES) {
      if (c_p->new_command == YES) {
	current_command = (ECommand *)add_to_tail(command_list, c_p);
	c_p->new_command = NO;
      } else {
	add_to_tail(command_list, c_p);
      }
      return YES;
    } else
      return NO;
  } else {
    return NO;
  }
}
   
/* ======================================================================== */
 
BOOLEAN 
CmdUndoCmd(ECommand *c_p)
{
  if (c_p->undo_func != NULL) {
    if (((*c_p->undo_func)(c_p)) == YES)
      add_to_tail(command_list, c_p);
    return YES;
  } else 
    return NO;
}
 
/* ======================================================================== */

ECommand *
CmdAddCommandToHistory(ECommand *c_p)
{
  if (command_list == NULL)
    command_list = make_list();
  return (current_command = (ECommand *)add_to_tail(command_list, c_p));
}

/* ======================================================================== */
  
void 
CmdFlushCommandHistory()
{
  if (command_list != NULL)
    flush_list(command_list, AND_NODES);
}

/* ======================================================================== */
  
ECommand *
CmdPrevCommandInHistory()
{
  if (command_list != NULL)
    return (current_command =
	    (ECommand *)get_list_prev(command_list, current_command));
  else
    return NULL;
}

/* ======================================================================== */
  
ECommand *
CmdNextCommandInHistory()
{
  if (command_list != NULL)
    return (current_command =
	    (ECommand *)get_list_next(command_list, current_command));
  else
    return NULL;
} 

/* ======================================================================== */
  
ECommand *
CmdCurrentCommandInHistory()
{
  if (command_list != NULL)
    return current_command;
  else
    return NULL;
}
