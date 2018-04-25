#ifndef DID_TOKENSP_H
#   define DID_TOKENSP_H

#include <stdio.h>
#include <ctype.h>

#ifndef FALSE
#define	FALSE	0
#define	TRUE	1
#endif


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif


		/*  Instructions to the token reader  */
#define	SKIP_SPACES	        1
#define	DONT_SKIP_SPACES	2

				/*  Types of tokens  */
#define	ENDOFLINE_TOKEN	0
#define	SPACES_TOKEN	1
#define	DELIMITER_TOKEN	2
#define	STRING_TOKEN	3
#define	ALPHA_TOKEN	4
#define	NUMBER_TOKEN	5
#define NO_TOKEN_AT_ALL -1


/* Tokenizer: set *char_p=0 on the first call */
int
tokens_get_token(char *line, /* IN: the input line */
                 int *char_p, /* INOUT: indexing the current token; */
                              /* must be set *char_p=0 before first call */
                 char token[], /* OUT: token as string */
                 int treat_spaces /* IN: how to handle spaces */
                 );
/* Might be used like this:
  while ((tok_type = tokens_get_token(to_be_parsed, &ptr, tok, SKIP_SPACES))
          != ENDOFLINE_TOKEN) {
     ...
  }
         */
/* Tokenize a line (represented as a string).  Build a structure of the */
/* parsed line, which could be then accessed token by token.  */
int /* returns non-zero if there are any tokens in the input line */
tokens_parse_line(char *to_be_parsed);


typedef enum {TOKENS_NO_TOKEN=0, TOKENS_STRING=1, TOKENS_NUMBER=2} tokens_token_type;

/* Which token type?  Might be any of the above  */
tokens_token_type 
tokens_type_of_token(int token_number);

/* Get token as string (for inspection only -- you may modify your */
/* COPY ONLY). */
char *
tokens_token_as_string(int token_number);

/* Get token as double.  It makes sense only when */
/* tokens_token_type(token_number) == TOKENS_NUMBER. */
double 
tokens_token_as_double(int token_number);

/* Get token as int (long int).  It makes sense only when */
/* tokens_token_type(token_number) == TOKENS_NUMBER. */
long
tokens_token_as_int(int token_number);

/* Is the capitalized token identical to capitalized keyword? */
/* For instance, "Key" and "kEy" are the identical strings for this purpose. */
int 
tokens_token_and_keyword_equiv(char *tok, char *keyword);

/* Does the parsed line contain a keyword? */
int 
tokens_have_keyword(int start_at, char *keyword);

/* How many tokens in the parsed line? */
int 
tokens_total_of_tokens(void);

/* Opens the file named.  If there was another file open at this time */
/* it is pushed onto the stack, and the new file is opened for reading. */
/* The file pushed onto the stack is reactivated when the current file */
/* is closed.  The function returns 0 when the open failed; otherwise */
/* non--zero value is returned. */
int
tokens_open_file(char *name);

/* The current file is closed.  If there was an file in the input stack */
/* it is reactivated and the function returns non-zero; otherwise zero */
/* is returned. */
int
tokens_close_curr_file(void);

/* Next line is read from the current input file and it is parsed. */
/* If there was no next line to be read, or in case of any error (e.g. */
/* when there was no current input file) the function returns zero; */
/* otherwise non-zero is returned. */
int
tokens_next_line(void);

/* Returns the number of the currently parsed line. */
int
tokens_curr_line_num(void);

#endif /* DID_TOKENSP_H */
