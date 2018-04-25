#ifndef DID_STRP_H
#define DID_STRP_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
      strP.h - include file for programs using strfn.c
*/
 
/*
   Hear ye, hear ye:
 
   This is the rule for using these routines:
 
   The char * returning routines return a pointer to a temporary
   string that does not have a stable value.  It will be correct
   for the first usage, but not guarenteed for consequetive use.
   If you want the value, copy it into your own string.  These
   routines provide their own garbage collection, so you don't have
   to worry about the strings returned by these routines being
     dropped into lost memory.

    Goodies-
	If you use #define STRFN_LINT, then this include
    file will declare all the ANSI function prototypes.
	If you use #define STRFN_SHORTCUT, then this
    include file will create a series of #defines for you that will
    make it so you can use the str_ functions without the leading
    str_ characters.
*/
 
/*
   Notice:
 
   isalpha, isdigit, and isalnum are in io.h
   toupper and tolower are in ctype.h
*/

#ifndef FALSE
#   define TRUE 1
#   define FALSE 0
#endif

#if STDC_HEADERS

#include <stdlib.h>

int	str_instr (char *pattern, char *string);
int	str_chrpos (int search_char, char *string);
char	*str_midstr (char *thestring, int start, int length);
char	*str_concat (char *string1, char *string2);
char	*str_right (char *linetext, int n);
char	*str_left (char *linetext, int n);
char	*str_UpperCase (char *q);
char	*str_LowerCase (char *q);
char	*str_trim (char *theline);
char	*str_trimwhite (char *theline);
int	str_copies (char *pattern, char *linetext);
char	*str_replace (char *oldpattern, char *newpattern, char *linetext);
int	str_index (char *pattern, char **data, int no_indices);
char	*str_reverse (char *s);
int	str_NoLeadSpaces (char *s);
int	str_NoLeadWhite (char *s);
char	*str_LeftTrim (char *s, int amount_to_shave);
char	*str_RightTrim (char *s, int amount_to_shave);

#else

int	str_instr ();
int	str_chrpos ();
char	*str_midstr ();
char	*str_concat ();
char	*str_right ();
char	*str_left ();
char	*str_UpperCase ();
char	*str_LowerCase ();
char	*str_trim ();
char	*str_trimwhite ();
int	str_copies ();
char	*str_replace ();
int	str_index ();
char	*str_reverse ();
int	str_NoLeadSpaces ();
int	str_NoLeadWhite ();
char	*str_LeftTrim ();
char	*str_RightTrim ();

#endif


#ifdef STRFN_SHORTCUT

#define instr(pattern,string) str_instr(pattern,string)
#define chrpos(search_char,string) str_chrpos(search_char string);
#define midstr (thestring,start,length) str_midst (thestring,start,length);
#define concat(string1,string2) str_concat(string1,string2)
#define right(linetext,n) str_right(linetext,n)
#define left(linetext,n) str_left(linetext,n)
#define UpperCase(q) str_UpperCase(q)
#define LowerCase(q) str_LowerCase(q)
#define trim(theline) str_trim(theline)
#define copies(pattern,linetext) str_copies(pattern linetext);
#define replace(oldpattern,newpattern,linetext) str_replace(oldpattern,newpattern,linetext)
#define index(pattern,data,no_indices) str_index(pattern data,no_indices);
#define reverse(s) str_reverse(s)
#define NoLeadSpaces(s) str_NoLeadSpaces(s)
#define NoLeadWhite(s) str_NoLeadWhite(s)
#define LeftTrim(s,amount_to_shave) str_LeftTrim(s amount_to_shave);
#define RightTrim(s,amount_to_shave) str_RightTrim(s amount_to_shave);

#endif

#endif /* DID_STRP_H */
