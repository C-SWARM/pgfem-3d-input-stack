/*
  file: tokens.c
  
    Written by Anthony V. Edwards, TAVVE Software Co.
 
  This file contains the routine TOKENIZE that may be useful for various
  applications
 
  =Tokenize (var token_string: tline; pattern: tline; var tokens: StrPtrArr): byte;
      The value returned from Tokenize is the number of tokens found.
  =TokenStr (raw: TokPtr): tline;
      The value returned from TokenStr is the constructed token string.
  =Parse (var token_string: tline; var tokens: StrPtrArr; delim: char): byte;
      The value returned from Parse is the number of tokens found.
  =GetToken (token_string: tline; delim: tline): tline;
      The value returned is the characters in token_string up to (not incl.)
      the delimeter specified.
  =GetAlphaToken (token_string: tline): tline;
      Returns the leading alphabetical characters of token_string.
  =GetDigitToken (token_string: tline): tline;
      Returns the leading digits of token_string allowed in a number.
  =GetAlphaNumToken (token_string: tline): tline;
      Returns the leading characters of token_string that are alphanumeric.
*/ 

/* xon: tok.000 */
/* xtoc: #Tokenizing routines */
/* xpage */
/*
		TOKENIZING ROUTINES

  How to use Tokenize:
     The reason why token_string is passed by reference (var) is to prevent
  it from being the result of a function.  Part of Tokenize's output, tokens,
  has pointers that point to parts of token_string.  So, DO NOT CHANGE
  TOKEN_STRING UNTIL YOU ARE DONE WITH TOKENS.
     The pattern may contain any characters, with two characters being hot or
  special characters.  By default, '*' is the wildcard variable.  It means
  0+ characters.  You can force it to wildcard a minimum number of characters
  by putting a '?' per character preceding *.  Example:  ???* means 3 or more
  characters.  The '?' symbol itself may be used to single out a token that
  has an expected length.  For example, if you expect a token to be 5 long,
  use ????? .  Note:  although you may want to tokenize two tokens next to
  each other that are both 5 long, using ?????????? will make a token 10
  characters long.  It is up to you to subdivide (use COPY).
     All other symbols are taken to be distinct patterns and will be clustered
  together.  Note that if the pattern was not found, Tokenize will terminate,
  putting all remaining text into the last token.
*/
/* xoff */
 

#include "tokP.h"
 
#define  onechar    '?'
#define  anychar    '*'
 
extern char *strfn_nada;  /* a little cheating... */
 
#define AddToken(pos,length) {     \
if (length>0) {                    \
  (tokens+no_tokens)->StrPtr= pos; \
  (tokens+no_tokens)->len= length; \
  no_tokens++;                     \
  }                                \
}
 
 
/* xon: tok.010 */
/* xtoc: 	tok_tokenize() */
int tok_tokenize (token_string, pattern, tokens)
char *token_string;	/* INPUT: string to be tokenized */
char *pattern;		/* INPUT: general pattern of tokens */
struct TokPtr *tokens;	/* OUTPUT: the tokens */
/*
	This routine returns the number of tokens that it created.
*/
/* xoff */
{
  char curr_patt[100];
  int i, j,
    no_tokens= 0,
    tspos= 0, tslen,
    papos= 0, plen,
    cpos= 0,
    done= FALSE;
 
  /* TOKENIZE */
  /* initialize everything */
  tslen= strlen(token_string);
  plen= strlen(pattern);
#define iswild(char) (char==onechar || char==anychar)
  /* grand loop to cycle through token_string */
  do {
    /* fetch *, n?, or delimeter */
    strcpy (curr_patt, "");
    cpos= 0;
    if (iswild(pattern[papos])) {
      /* wild-card pattern next */
      while (iswild(pattern[papos]) && papos<plen)
        curr_patt[cpos++]= pattern[papos++];
      curr_patt[cpos]= '\0';
      if (str_chrpos(anychar, curr_patt)>=0) {
        /* find terminating pattern to find next token */
        j= cpos-1;     /* the number of ?'s */
        strcpy (curr_patt, "");
	cpos= 0;
	/* figure out what the terminating text is */
        while (! (iswild(pattern[papos]) && papos<plen))
          curr_patt[cpos++]= pattern[papos++];
	curr_patt[cpos]= '\0';
	/* where is the terminating text? */
	i= str_instr(curr_patt, token_string+tspos+j-1);
	/* i contains length of string that matched the wild card *,
	 but does not include the n?'s (which is j) */
        if (cpos==0 || i<0)
          done= TRUE;
        else {
          /* the wildcard part */
          AddToken (token_string+tspos, i+j);
          /* the terminating string part */
          AddToken (token_string+tspos+i+j, cpos);
          /* update current character position */
          tspos += cpos+i+j;
          }
        }
      else {  /* pattern is next n characters */
        i= cpos;
        if (tspos+i-1>tslen)
          AddToken (token_string+tspos, tslen-tspos+1)
        else
          AddToken (token_string+tspos, strlen(curr_patt));
        tspos++;
        }
      }
    else { /* match expected delimeter */
      while (! (iswild(pattern[papos]) && papos<plen))
        curr_patt[cpos++]= pattern[papos++];
      curr_patt[cpos]= '\0';
      if (cpos==0 || strcmp (str_left(token_string+tspos,cpos), curr_patt))
          done= TRUE;
      else {
        AddToken (token_string+tspos, cpos);
        tspos += cpos;
        }
      }
    if (papos>=plen)
      papos= 0;
    } while (!(tspos>tslen || done));
  if (tspos<=tslen)
    AddToken (token_string+tspos, tslen-tspos+1);
  return (no_tokens);
}
 
/* xon: tok.020 */
/* xtoc: 	tok_TokenStr() */
char * tok_token_str (raw)
struct TokPtr *raw; /* INPUT:  token description */
/* xoff */
{
  char *a;
  a= str_left (raw->StrPtr, raw->len);
  return (a);
}
 
/* xon: tok.030 */
/* xtoc: 	tok_Parse() */
int tok_parse (token_string, tokens, delim)
char *token_string;	/* INPUT: the string to be tokenized */
struct TokPtr *tokens;	/* OUTPUT: the tokens */
int delim;		/* INPUT: the delimiter between tokens */
/*
	This routine is a simplification of the tok_Tokenize routine.
The input string is assumed to be a series of tokens separated by
one or more delimeters.
*/
/* xoff */
{
  int lpos= 0,
    p, nt= 0;
  do {
    p= str_chrpos (delim, token_string+lpos);
    if (p>0) {
      (tokens+nt)->StrPtr= token_string+lpos;
      (tokens+nt)->len= p;
      nt++;
      lpos += p+1;
      }
    else
      if (p==0)
        lpos++;
    } while (p>=0);
  if (lpos+1<(p=strlen(token_string))) { /* more text on line */
      (tokens+nt)->StrPtr= token_string+lpos;
      (tokens+nt)->len= p-lpos;
      nt++;
      }
  return (nt);
}
 
/* xon: tok.040 */
/* xtoc: 	tok_GetToken() */
char * tok_get_token (token_string, delim)
char *token_string; /* INPUT:  string in question */
char *delim;	    /* INPUT:  token delimeter */
/*
	Return the first token in /token_string/.  Return 
all characters up to the first occurrance of /delim/.
*/
/* xoff */
{
  int p;
  p= str_instr (delim, token_string);
  if (p>=0)
    return (str_midstr (token_string, 0, p));
  else
    return (strfn_nada);
}
 
/* xon: tok.050 */
/* xtoc: 	tok_GetAlphaToken() */
char * tok_get_alpha_token (token_string)
char *token_string; /* INPUT:  string in question */
/*
	Return the first string of alphabetical characters, 
including '_'.
*/
/* xoff */
{
  int l, i= 0;
 
  l= strlen (token_string);
  while (i<l && (isalpha(token_string[i]) || token_string[i]=='_'))
    i++;
  return (str_midstr (token_string, 0, i));
}
 
/* xon: tok.050 */
/* xtoc: 	tok_GetDigitToken() */
char * tok_get_digit_token (token_string)
char *token_string; /* INPUT:  string in question */
/*
	Return the first string of numeric characters.  A + or a
- may appear as the first character.
*/
/* xoff */
{
  int l,i= 0;
 
  l= strlen (token_string);
  if ((token_string[0]=='-' || token_string[0]=='+') && l>1)
    i++;
  while (i<l && isdigit(token_string[i]))
    i++;
  return (str_midstr (token_string, 0, i));
}
 
/* xon: tok.060 */
/* xtoc: 	tok_GetAlphaNumToken() */
char * tok_get_alphanum_token (token_string)
char *token_string; /* INPUT:  string in question */
/*
	Return the first string of alphanumeric characters.  Note
that a + or a - may not be the first character, however _ is still 
considered an alphabetical character.
*/
/* xoff */
{
  int l,i= 0;
 
  l= strlen (token_string);
  while (i<l && (isalnum(token_string[i]) || token_string[i]=='_'))
    i++;
  return (str_midstr (token_string, 0, i));
}
