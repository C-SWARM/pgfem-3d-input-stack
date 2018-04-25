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

#ifndef ETYPEIN_H
#define ETYPEIN_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "Etypes.h" /* viewer types */


/* ===================================================================== */
/* Some convenience macros and global constants */

#ifdef MAX_LINE_LENGTH
#  undef MAX_LINE_LENGTH
#endif
#define MAX_LINE_LENGTH 256        /* maximum length of line in input file */

#ifdef SEPARATORS
#  undef SEPARATORS
#endif
#define SEPARATORS " "   /* separators of tokens */

#ifdef MAX_TOKENS
#  undef MAX_TOKENS
#endif
#define MAX_TOKENS 1000            /* maximum of tokens per line */

#ifdef COMMENT_CHAR
#  undef COMMENT_CHAR
#endif
#define COMMENT_CHAR '!'          /* comment char to be used in input file */

#define QUOTE '"'
#define BLANK ' '

/* ===================================================================== */

typedef enum { NO_TOKEN, STRING, NUMBER } token_type;

typedef struct token_rec {
  int    token_type;
  char   *as_string;
  double  as_double;
} token_rec;

#define NO_TOKENS -1

typedef void (*exec_proc_p)(void);

typedef struct  _line_contents {
  int               tokens_total; /* >= 1         for non-empty command line */
				  /* == NO_TOKENS for empty line */
  int                line_number;
  token_rec tokens[MAX_TOKENS+1]; /* values of the tokens */
} line_contents;


typedef struct top_command {
  char              *keyword;
  int                keyword_length;
  exec_proc_p        handle_it;
  char              *syntax;
} top_command;
  
/* ===================================================================== */

int          TypeInBadToken( int ith, char *should_be_type );
void         TypeInErrorInInput(void);
void         TypeInPrintLineContents(void);

int          TypeInIsKeywordInLine( int start_at, char *keyword );
BOOLEAN      TypeInCompareTokenWithKeyword( char *tok, char *keyword );
void         TypeInBadSyntax( char *to_print );
double       TypeInGetTokenAsDouble( int token_number );
int          TypeInGetTokenAsInt(int token_number);
char         *TypeInGetTokenAsString( int token_number );
token_type   TypeInGetTokenType( int token_number );
int          TypeInGetTokensTotal(void);
int          TypeInParseLine( char *buffer );
void         TypeInOpenInFile( char *in_file );
int          TypeInReadNextLine(void);
void         TypeInInitCommandLine(void);
int          TypeInCurrentCommand(void);
void         TypeInInputData( char *in_file );
void         TypeInInterpretCurrentCommand(void);
top_command *TypeInAugmentCommandTable(top_command *table, int table_size);
top_command *
TypeInReplaceCommand(top_command *cmd);
/* The table MUST start by { NULL, 0, TypeInErrorInInput }. */
void         TypeInSetCommandTable(top_command *table, int table_size);
top_command *
TypeInGetCommandData(char *top_keyword);
int 
TypeInCommandTableSize(void);

#ifdef __cplusplus
}
#endif

#endif
