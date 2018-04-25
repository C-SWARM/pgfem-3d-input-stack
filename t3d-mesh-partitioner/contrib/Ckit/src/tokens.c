
#include "tokensP.h"
#include "listP.h"
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


#define DUPLICATE_STRING(dest, src)                                \
   if (((dest) = (char *)malloc((size_t)(strlen(src)+1))) == NULL) \
      fprintf(stderr, "\nDUPLICATE_STRING\n");                     \
   strcpy((dest), (src))


#define MAXLINE	512 

#define SPACE	 ' '
#define TAB	 '\t'
#define DOT      '.'
#define PLUS     '+'
#define MINUS    '-'
#define MINUS    '-'
#define EOS      '\0'
#define UNDERBAR '_'
#define NEWLINE  '\n'
#define DQUOTE   '"'

/*
 * Get a token from an input line.  Tokens are delimited by runs of
 * spaces.  Tokens can be enclosed in "" signs.  See tokensP.h for
 * the types of tokens that are recognized. */

int
tokens_get_token(char *line, int *char_p, char token[], int treat_spaces)
{
  int	tok_pos;
  
  token[0] = EOS;
  tok_pos = 0;
  
  if (line[*char_p] == EOS || line[*char_p] == NEWLINE)
    return(ENDOFLINE_TOKEN);
  /*  Either remove runs of whitespace  */
  /*  or return the stuff  */
  if (treat_spaces == SKIP_SPACES) {
    while (line[*char_p] == SPACE || line[*char_p] == TAB)
      (*char_p)++;
  } else {
    if (isspace(line[*char_p])) {
      while (line[*char_p] == SPACE || line[*char_p] == TAB)
        token[tok_pos++] = line[(*char_p)++];
      token[tok_pos] = EOS;
      return(SPACES_TOKEN);
    }
  }
  /*  Treat delimiters here  */
  if (ispunct(line[*char_p])
      && !(line[*char_p] == MINUS && isdigit(line[*char_p+1]))
      && !(line[*char_p] == PLUS  && isdigit(line[*char_p+1]))
      && !(line[*char_p] == DOT   && isdigit(line[*char_p+1]))) {
    if (line[*char_p] != DQUOTE) {
      token[tok_pos++] = line[(*char_p)++];
      token[tok_pos] = EOS;
      return(DELIMITER_TOKEN);
    }
  }
  
  /*  Treat quoted strings here  */
  if (line[*char_p] == DQUOTE) {
    (*char_p)++;
    while (line[*char_p] != EOS && line[*char_p] != NEWLINE) {
      token[tok_pos] = line[(*char_p)++];
      /*  Check for doubled up quotes  */
      if (token[tok_pos] != DQUOTE) {
        tok_pos++;
      } else {
        if (line[*char_p] == DQUOTE) {
          tok_pos++;
          (*char_p)++;
        } else {
          token[tok_pos] = EOS;
          return(STRING_TOKEN);
        }
      }
    }
    token[tok_pos] = EOS;
    return(STRING_TOKEN);
  }
  /*  Treat Alphanumerics here  */
  if (isalpha(line[*char_p])) {
    while (line[*char_p] != EOS && line[*char_p] != NEWLINE) {
      token[tok_pos] = line[*char_p];
      if (!(isalnum(line[*char_p]) || line[*char_p] == UNDERBAR)) {
        token[tok_pos] = EOS;
        return(ALPHA_TOKEN);
      }
      tok_pos++;
      (*char_p)++;
    }
    token[tok_pos] = EOS;
    return(ALPHA_TOKEN);
  }
  /*  Treat numerics here  */
  if (isdigit(line[*char_p])
      || line[*char_p] == PLUS
      || line[*char_p] == MINUS
      || line[*char_p] == DOT) {
    while (line[*char_p] != EOS && line[*char_p] != NEWLINE) {
      token[tok_pos] = line[*char_p];
      if (!(isdigit(line[*char_p])
            || line[*char_p] == DOT
            || line[*char_p] == 'e'
            || line[*char_p] == 'E'
            || line[*char_p] == PLUS
            || line[*char_p] == MINUS
            )) {
        token[tok_pos] = EOS;
        return(NUMBER_TOKEN);
      }
      tok_pos++;
      (*char_p)++;
    }
    token[tok_pos] = EOS;
    return(NUMBER_TOKEN);
  }
  return (NO_TOKEN_AT_ALL);
}


#ifdef NEED_MAIN

static char *token_types[] = {
  "NO_TOKEN_AT_ALL",
  "SPACES_TOKEN",
  "DELIMITER_TOKEN",
  "STRING_TOKEN",
  "ALPHA_TOKEN",	
  "NUMBER_TOKEN",
  "ENDOFLINE_TOKEN"
};


void 
tokens_print_line_contents(void)
{
  int tot, i;
  tokens_token_type type;
  
  printf("\nLine --> number of tokens: %d", tot = tokens_total_of_tokens());
  for (i=1; i <= tot; i++){
    type = tokens_type_of_token(i);
    switch((int)type)	{
    case (int)TOKENS_NUMBER:
      printf("\n  %d type = NUMBER, val = %f", i, tokens_token_as_double(i));
      break;
    case (int)TOKENS_STRING:
      printf("\n  %d type = STRING, val = %s", i, tokens_token_as_string(i));
      break;
    default:
      printf("\n  %d type=UNKNOWN", i);
    }
  }
  printf("\n==========================================\n");
}

int
main(void)
{
  char line[256];

  while(1) {
    printf("Line > ");
    gets(line);
    tokens_parse_line(line);
    tokens_print_line_contents();
  }
}

#endif


#define TOKENS_MAX_TOKENS 1000  
#define TOKENS_MAX_LINE_LENGTH 512
#define TOKENS_COMMENT_CHAR '!'
#define TOKENS_NO_TOKENS -1


typedef struct tokens_token_rec {
  int    token_type;
  char   *as_string;
  double  as_double;
} tokens_token_rec;

typedef struct  tokens_line_contents {
  int                tokens_total; /* >= 1 for non-empty command line */
				   /* == NO_TOKENS for empty line */
  int                line_number;
  tokens_token_rec   tokens[TOKENS_MAX_TOKENS+1]; /* values of the tokens */
} tokens_line_contents;


static tokens_line_contents tokens_curr_line; /* parsed line */

 
int 
tokens_parse_line(char *to_be_parsed)
{
  char            tok[TOKENS_MAX_LINE_LENGTH];
  int             have_input = 0;
  int             ptr = 0;
  char           *chp;
  double          num_val;
  int             tok_ix, tok_type;

  if (to_be_parsed == NULL) return 0; /* No tokens in hat one */
  
  tok_ix      = TOKENS_NO_TOKENS;
  while ((tok_type = tokens_get_token(to_be_parsed, &ptr, tok, SKIP_SPACES))
         != ENDOFLINE_TOKEN) {
    if (tok_type == NO_TOKEN_AT_ALL)
      break;
    
    tok_ix++;
    if ((tokens_curr_line.tokens[tok_ix].as_string) != NULL)	{
      free(tokens_curr_line.tokens[tok_ix].as_string);
      tokens_curr_line.tokens[tok_ix].as_string = NULL;
    }
    if (tok[0] == TOKENS_COMMENT_CHAR)
      break;  /* this token is comment char -- return  */
    /* Assume string */
    DUPLICATE_STRING(tokens_curr_line.tokens[tok_ix].as_string, tok);
    tokens_curr_line.tokens[tok_ix].token_type = TOKENS_STRING;
    if (tok_type == NUMBER_TOKEN) {
      errno = 0;
      num_val = strtod(tok, &chp);
      if (chp != tok && errno == 0) { /* successful conversion */
        tokens_curr_line.tokens[tok_ix].as_double  = num_val;
        tokens_curr_line.tokens[tok_ix].token_type = TOKENS_NUMBER;
      }
    }
    tokens_curr_line.tokens_total = tok_ix + 1;
    have_input = 1;             /* Yes, have input */
  }
  return have_input;
}



tokens_token_type 
tokens_type_of_token(int token_number)
{
  if (token_number <= tokens_curr_line.tokens_total)
    return tokens_curr_line.tokens[token_number-1].token_type;
  else
    return TOKENS_NO_TOKEN;
}



char *
tokens_token_as_string(int token_number)
{
  if (token_number <= tokens_curr_line.tokens_total)
    return tokens_curr_line.tokens[token_number-1].as_string;
  else
    return NULL;
}



double 
tokens_token_as_double(int token_number)
{
  if (token_number <= tokens_curr_line.tokens_total)  {
    if (tokens_curr_line.tokens[token_number-1].token_type == TOKENS_NUMBER)
      return tokens_curr_line.tokens[token_number-1].as_double;
  }
  return 0.0;
}     


long
tokens_token_as_int(int token_number)
{
  if (token_number <= tokens_curr_line.tokens_total){
    if (tokens_curr_line.tokens[token_number-1].token_type == TOKENS_NUMBER)
      return (long)(tokens_curr_line.tokens[token_number-1].as_double);
  }
  return (long)0;
}     






int 
tokens_have_keyword(int start_at, char *keyword)
{
  int i, tot;

  for (i = (start_at > 0)?start_at:1, tot = tokens_total_of_tokens();
       i <= tot; i++)    {
    if (tokens_token_and_keyword_equiv(tokens_token_as_string(i), keyword)) {
      return i;
    }
  }
  return 0;
}


int 
tokens_token_and_keyword_equiv(char *tok, char *keyword)
{
  int j, lentok, lenkeyword;

  lentok     = strlen(tok);
  lenkeyword = strlen(keyword);
  if (lentok != lenkeyword)
    return FALSE;
  for(j = 0; j < lentok; j++) {
    if (toupper(*(tok+j)) != toupper(*(keyword+j)))	{
      return FALSE;
    }
  }
  return TRUE;
}



int 
tokens_total_of_tokens(void)
{
  return tokens_curr_line.tokens_total;
}




/* Package for handling of files of commands */

static FILE *tokens_in_file = NULL;
static STACK tokens_input_stack = NULL;
static int tokens_current_line_number = 0;

/* Opens the file named.  If there was another file open at this time */
/* it is pushed onto the stack, and the new file is opened for reading. */
/* The file pushed onto the stack is reactivated when the current file */
/* is closed.  The function returns 0 when the open failed; otherwise */
/* non--zero value is returned. */
int
tokens_open_file(char *name)
{
  if (tokens_input_stack == NULL) tokens_input_stack = make_stack();
  
  if (tokens_in_file != NULL) {
    push_stack(tokens_input_stack, tokens_in_file);
  }
  if (tokens_token_and_keyword_equiv("-", name)) {
    tokens_in_file = stdin;
  } else {
    tokens_in_file = fopen(name, "r");
  }
  if (tokens_in_file == NULL) {
    if (!stack_empty(tokens_input_stack)) {
      tokens_in_file = (FILE *)pop_stack(tokens_input_stack);
    }
    return 0; /* Failure */
  }
  tokens_current_line_number = 0;
  return 1;
}


/* The current file is closed.  If there was an file in the input stack */
/* it is reactivated and the function returns non-zero; otherwise zero */
/* is returned. */
int
tokens_close_curr_file(void)
{
  if (tokens_in_file != stdin)
    fclose(tokens_in_file);
  if (!stack_empty(tokens_input_stack)) {
    tokens_in_file = (FILE *)pop_stack(tokens_input_stack);
    return 1;
  } else {
    tokens_in_file = NULL;
    return 0;
  }
}


#define MAX_LINE_LENGTH 512
static char buffer[MAX_LINE_LENGTH];

/* Next line is read from the current input file and it is parsed. */
/* If there was no next line to be read, or in case of any error (e.g. */
/* when there was no current input file) the function returns zero; */
/* otherwise non-zero is returned. */
int 
tokens_next_line(void)
{
  int anything_to_return = 0;

  if (tokens_in_file == NULL) {
    return 0;
  } else {
    while(!anything_to_return) {
      tokens_current_line_number++;
      if (fgets(buffer, MAX_LINE_LENGTH-1, tokens_in_file) == NULL)	{
        if (feof(tokens_in_file))
          return 0;
      }
      anything_to_return = tokens_parse_line(buffer);
    }
    return 1;
  }
}


int
tokens_curr_line_num(void)
{
  return tokens_current_line_number;
}


