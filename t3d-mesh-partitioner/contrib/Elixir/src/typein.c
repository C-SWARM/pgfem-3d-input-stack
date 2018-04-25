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
#include <ctype.h>
#include "Etypein.h"
#include "tokensP.h" /* of Ckit library */

/* ========================================================================= */
/* PRIVATE */
/* ========================================================================= */

static top_command *top_command_table = NULL;

static int commands_total = 0;	/* number of fields in the commands[] array */

 /* FILE pointer for input */
static FILE *in_FILE_p = NULL;


static line_contents command_line; /* command-line data structure */


/* ========================================================================= */
/* PUBLIC */
/* ========================================================================= */

/* ========================================================================= */

void 
TypeInInputData(char *in_file) /* TypeInSetCommandTable must be */
				      /* called as the first thing before */
				      /* entering TypeInInputData */
{
  TypeInInitCommandLine();
  TypeInOpenInFile(in_file);
  while(TypeInReadNextLine() != EOF) {
    top_command_table[TypeInCurrentCommand()].handle_it();
  }
}

/* ========================================================================= */

void 
TypeInInterpretCurrentCommand()
{
  /* TypeInPrintLineContents(); */ /* uncomment for debugging purposes */
  top_command_table[TypeInCurrentCommand()].handle_it();
}
 
/* ========================================================================= */
/* The table MUST start by { NULL, 0, TypeInErrorInInput }. */

void 
TypeInSetCommandTable(top_command *table, int table_size)
{
  top_command_table = table;
  commands_total    = table_size;
  TypeInInitCommandLine();
}

/* ========================================================================= */
/* Works similarly to TypeInSetCommandTable when there was no command */
/* table active; otherwise appends the new command table at the end. */
/* The table must not start by { NULL, 0, TypeInErrorInInput }. */

top_command *
TypeInAugmentCommandTable(top_command *table, int table_size)
{
  top_command *old_table, *new_table;
  int old_commands_total, new_commands_total, i, j;

  if (table == NULL)
    return top_command_table;   /* request to get the current command table */
  
  old_table          = top_command_table;
  old_commands_total = commands_total;
  if (old_table == NULL) {
    old_commands_total = 1; /* take account of the nil command */
  }
  new_commands_total = old_commands_total + table_size;

  new_table = (top_command *)make_node(sizeof(top_command)*new_commands_total);
  if (new_table == NULL) {
    fprintf(stderr, "Failed allocate memory\n");
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
  if (old_table == NULL) {
    new_table[0].keyword        = NULL;
    new_table[0].keyword_length = 0;
    new_table[0].handle_it      = TypeInErrorInInput;
    new_table[0].syntax         = NULL;
  } else {
    for (i = 0; i < old_commands_total; i++) {
      if (old_table[i].keyword != NULL)
        new_table[i].keyword        = strdup(old_table[i].keyword);
      else
        new_table[i].keyword        = NULL;
      new_table[i].keyword_length = old_table[i].keyword_length;
      new_table[i].handle_it      = old_table[i].handle_it;
      if (old_table[i].syntax != NULL)
        new_table[i].syntax         = strdup(old_table[i].syntax);
      else
        new_table[i].syntax         = NULL;
    }
  }
  for (j = 0; j < table_size; j++) {
    i = j + old_commands_total;
    new_table[i].keyword        = strdup(table[j].keyword);
    new_table[i].keyword_length = table[j].keyword_length;
    new_table[i].handle_it      = table[j].handle_it;
    if (table[j].syntax != NULL)
      new_table[i].syntax         = strdup(table[j].syntax);
    else
      new_table[i].syntax         = NULL;
  }
  top_command_table = new_table;
  commands_total    = new_commands_total;
  TypeInInitCommandLine();
  return old_table;
}

/* ========================================================================= */

int 
TypeInCurrentCommand()
{
  int     i;
  char *tok;

  tok = TypeInGetTokenAsString(1);

  /* Note that we start at i = 1 (the error_handler is at the zero index) */
  for(i = 1; i < commands_total; i++) {
    if (TypeInCompareTokenWithKeyword(tok, top_command_table[i].keyword))
      return i;
  }
  return 0;
}

/* ========================================================================= */

void 
TypeInInitCommandLine()
{
  int i;

  if (top_command_table == NULL) {
    fprintf(stderr, "No command table!?\n");
    exit (ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
  for (i = 0; i <= MAX_TOKENS; i++) {
    command_line.line_number          = 0;
    command_line.tokens[i].token_type = NO_TOKEN;
    command_line.tokens[i].as_string  = NULL;
    command_line.tokens[i].as_double  = 0.0;
  }
}

/* ========================================================================= */

int 
TypeInReadNextLine() /* returns YES for valid input obtained or EOF */
{
  char buffer[MAX_LINE_LENGTH];
  int anything_to_return = NO;

  while(anything_to_return == NO) {
    command_line.line_number++;
    if (fgets(buffer, MAX_LINE_LENGTH-1, in_FILE_p) == NULL)	{
      if (feof(in_FILE_p))
	return EOF;
    }
    anything_to_return = TypeInParseLine(buffer);
  }
  return YES;
}

/* ========================================================================= */

void 
TypeInOpenInFile(char *in_file) 
{
  if ((in_FILE_p = fopen(in_file, "r")) == NULL) {
    fprintf(stderr, "\nFailed to open input file \"%s\".", in_file);
    exit(ESISetErrorExitCode(ELIXIR_ERROR_EXIT));
  }
}

/* ========================================================================= */

int 
TypeInParseLine(char *to_be_parsed)
{
  char           tok[MAX_LINE_LENGTH];
  int have_input = NO;
  int             ptr = 0;
  char           *chp;
  double      num_val;
  int          tok_ix, tok_type;

	command_line.tokens_total = 0;	
  if (to_be_parsed == NULL) return NO;
  
  tok_ix      = NO_TOKENS;
  while ((tok_type = tokens_get_token(to_be_parsed, &ptr, tok, SKIP_SPACES))
         != ENDOFLINE_TOKEN) {
    if (tok_type == NO_TOKEN_AT_ALL)
      break;
    
    tok_ix++;
    if ((command_line.tokens[tok_ix].as_string) != NULL)	{
      free(command_line.tokens[tok_ix].as_string);
      command_line.tokens[tok_ix].as_string = NULL;
    }
    if (tok[0] == COMMENT_CHAR)
      break;  /* this token is comment char -- return  */
    /* Assume string */
    command_line.tokens[tok_ix].as_string  = strdup(tok);
    command_line.tokens[tok_ix].token_type = STRING;
    if (tok_type == NUMBER_TOKEN) {
      errno = 0;
      num_val = strtod(tok, &chp);
      if (chp != tok && errno == 0) { /* successful conversion */
        command_line.tokens[tok_ix].as_double  = num_val;
        command_line.tokens[tok_ix].token_type = NUMBER;
      }
    }
    command_line.tokens_total = tok_ix + 1;
    have_input = YES;
  }
  return have_input;
}

/* ========================================================================= */

int 
TypeInGetTokensTotal()
{
  return command_line.tokens_total;
}

/* ========================================================================= */

token_type 
TypeInGetTokenType(int token_number)
{
  if (token_number <= command_line.tokens_total)
    return command_line.tokens[token_number-1].token_type;
  else
    return NO_TOKEN;
}

/* ========================================================================= */

char *
TypeInGetTokenAsString(int token_number)
{
  if (token_number <= command_line.tokens_total)
    return command_line.tokens[token_number-1].as_string;
  else
    return NULL;
}

/* ========================================================================= */

double 
TypeInGetTokenAsDouble(int token_number)
{
  if (token_number <= command_line.tokens_total)  {
    if (command_line.tokens[token_number-1].token_type == NUMBER)
      return command_line.tokens[token_number-1].as_double;
  }
  return 0.0;
}     

/* ========================================================================= */

int 
TypeInGetTokenAsInt(int token_number)
{
  if (token_number <= command_line.tokens_total){
    if (command_line.tokens[token_number-1].token_type == NUMBER)
      return (int)(command_line.tokens[token_number-1].as_double);
  }
  return 0;
}     

/* ========================================================================= */

int 
TypeInBadToken(int ith, char *should_be_type)
{
  fprintf(stderr, "\nBad token type (should be a %s) in line #%d:\n "
	  "   %dth token = \"%s\".\n", should_be_type,
	  command_line.line_number,
	  ith, TypeInGetTokenAsString(ith));
  return 0;
}
	
void 
TypeInPrintLineContents()
{
  int tot, i;
  token_type type;
  
  printf("\nLine # %d --> number of tokens: %d",
	 command_line.line_number, tot = TypeInGetTokensTotal());
  for (i=1; i <= tot; i++){
    type = TypeInGetTokenType(i);
    switch((int)type)	{
    case (int)NUMBER:
      printf("\n  %d type = NUMBER, val = %f",
	     i,TypeInGetTokenAsDouble(i));
      break;
    case (int)STRING:
      printf("\n  %d type = STRING, val = %s",
	     i,TypeInGetTokenAsString(i));
      break;
    default:
      printf("\n  %d type=UNKNOWN",i);
    }
  }
  printf("\n==========================================\n");
}

/* ========================================================================= */

/* this is quite dangerous;
	 for example calling TypeInIsKeywordInLine(2, "ZOOM") for line "view rotnor z 0.1" returns 3 */

int 
TypeInIsKeywordInLine(int start_at, char *keyword)
{
  int i, tot;
  char  *tok;

  for(i = (start_at > 0)?start_at:1,
      tot = TypeInGetTokensTotal(); i <= tot; i++)    {
    tok = TypeInGetTokenAsString(i);
    if (TypeInCompareTokenWithKeyword(tok, keyword) == YES)
      return i;
  }
  return 0;
}

/* ========================================================================= */

void 
TypeInErrorInInput()
{
  fprintf(stderr, "\nUnknown top keyword \"%s\" in line#%d. Ignoring.\n",
	  TypeInGetTokenAsString(1), command_line.line_number);
}

/* ========================================================================= */

BOOLEAN 
TypeInCompareTokenWithKeyword(char *tok, char *keyword)
{
  int j, len;
  
  len = min (strlen (tok), strlen (keyword));
  for(j = 0; j < len; j++) {
    if (toupper(*(tok+j)) != *(keyword+j))	{
      return NO;
    }
  }
  return YES;
}

/* ========================================================================= */

void 
TypeInBadSyntax(char *to_print)
{
  fprintf(stderr, "\nBad syntax in line#%d. %s\n",
	  command_line.line_number, to_print);
}


top_command *
TypeInGetCommandData(char *top_keyword)
{
  int i;

  for (i = 1; i < commands_total; i++)
    if (TypeInCompareTokenWithKeyword(top_keyword,
                                      top_command_table[i].keyword))
      return &top_command_table[i];
  
  return NULL; /* No command table or no such keyword */
}


top_command *
TypeInReplaceCommand(top_command *cmd)
{
  int i;
  
  if (top_command_table != NULL) {
    for (i = 1; i < commands_total; i++)
      if (TypeInCompareTokenWithKeyword(cmd->keyword,
                                        top_command_table[i].keyword)) {
        /* The command was already in the table */
        top_command_table[i].handle_it = cmd->handle_it;
        if (top_command_table[i].syntax != NULL)
          free(top_command_table[i].syntax);
        if (cmd->syntax != NULL)
          top_command_table[i].syntax = strdup(cmd->syntax);
        return NULL;
      }
  } 
  /* The command is new */
  return TypeInAugmentCommandTable(cmd, 1);
}



int 
TypeInCommandTableSize(void)
{
  return commands_total;
}
