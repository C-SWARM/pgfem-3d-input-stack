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

#ifndef ECMDS_H
#define ECMDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Eview.h"
#include "Egraphic.h"

struct ECommand;

typedef BOOLEAN (*ExecuteProcP)(struct ECommand *c_p);
typedef BOOLEAN (*UndoProcP)(struct ECommand *c_p);

typedef struct ECommand {
  BOOLEAN        executed;
  BOOLEAN        new_command;
  ExecuteProcP   execute_func;
  UndoProcP      undo_func;
  caddr_t        data;
}                                   ECommand;


ECommand *CmdCreateCommand(ExecuteProcP exec, UndoProcP undo, caddr_t data);
BOOLEAN CmdFlipState(ECommand *c_p);
ECommand *CmdAddCommandToHistory(ECommand *c_p);
ECommand *CmdPrevCommandInHistory(void);
ECommand *CmdNextCommandInHistory(void);
ECommand *CmdCurrentCommandInHistory(void);
BOOLEAN CmdUndoCmd(ECommand *c_p);
BOOLEAN CmdExecuteCmd(ECommand *c_p);
void CmdFlushCommandHistory(void);

#ifdef __cplusplus
}
#endif

#endif
