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
#include <fcntl.h>
#include "Eondisk.h"
#include "Ereprts.h"


/* ======================================================================= */
/* PRIVATE DATA */
/* ======================================================================= */


#ifdef HAVE_STRERROR
#  if (!HAVE_STRERROR)
#    undef HAVE_STRERROR
#  endif
#endif
#ifndef HAVE_STRERROR
#     define strerror(E_NUM) strerror(E_NUM)
      static char *
      strerror(int e_num);
      static char buf[132];
      static char *
      strerror(int e_num)
      {
        sprintf(buf, "Error #%d", e_num);
        return buf;
      }
#endif

char current_file[MAXFILENAMECHAR];
static BOOLEAN file_name_set = NO;
static FILE *file_p = NULL;

#define PMODE 0755

/* ======================================================================= */
/* PUBLIC */
/* ======================================================================= */

/* ======================================================================= */

void OnDiskSetFileName(char *file)
{
  int ix;

  ix = 0;
  while (*(file+ix) != '\0' && ix < MAXFILENAMECHAR) {
    current_file[ix] = *(file+ix);
    ix++;
  }
  current_file[ix] = '\0';
  file_name_set    = YES;
}

/* ======================================================================= */

char *OnDiskGetFileName()
{
  if (file_name_set == YES)
    return current_file;
  else
    return NULL;
}

/* ======================================================================= */

FILE *OnDiskOpenAGEFile(char *mode)
{
  errno = 0;
  if ((file_p = fopen(current_file, mode)) == NULL) {
    ERptErrMessage(ELIXIR_ERROR_CLASS, 1, ELIXIR_ERROR_1, ERROR_GRADE);
    fprintf(stderr, "Open for file '%s' in mode '%s' failed: \n   %s\n",
	    current_file, mode, strerror(errno));
  }
  return file_p;
}

/* ======================================================================= */

BOOLEAN OnDiskCloseAGEFile()
{
  int s = NO;

  errno = 0;
  if ((s = fclose(file_p)) == EOF) {
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, ELIXIR_ERROR_2, ERROR_GRADE);
    fprintf(stderr, "Close for file '%s' failed: \n   %s\n",
	    current_file, strerror(errno));
    return NO;
  }
  return YES;
}

/* ======================================================================= */

BOOLEAN OnDiskEmptyAGEFile()
{
  int fd;
  
  if ((fd = creat(current_file, PMODE)) == -1) {
    ERptErrMessage(ELIXIR_ERROR_CLASS, 3, ELIXIR_ERROR_3, ERROR_GRADE);
    fprintf(stderr, "File '%s' could not be emptied.\n", current_file);
    return YES;
  } else
    close(fd);
  return YES;
}
 
