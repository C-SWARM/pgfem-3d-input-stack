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

#ifndef EEXIT_H
#define EEXIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* EXIT CODES */
#define ELIXIR_ERROR_EXIT -1
#define ELIXIR_OK_EXIT     0


/* Sets the exit code which will be returned by the exit() function when it */
/* is called from within Elixir in normal circumstances (i.e. when exit() */
/* is not called to provide error signalization);  the currently set */
/* exit code is returned.*/
int
ESISetExitCode(int exit_code);
/* Sets the exit code which is returned on error exit, i.e. when Elixir */
/* decides it is time to part with the user because of fatal error. */
int
ESISetErrorExitCode(int exit_code);



#ifdef __cplusplus
}
#endif

#endif
