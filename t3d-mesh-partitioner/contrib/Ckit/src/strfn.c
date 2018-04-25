/*
  file: strfn.c

    Written by Anthony V. Edwards, TAVVE Software Co.
       Used in Northware with permission from author.
 
  This file contains various string functions that make string manipulation
  easier.
  routines
    =right (linetext: sline; n: integer): sline;
    =left (linetext: sline; n: integer): sline;
    =LoCase (c: char): char;
    =UpperCase (q: sline): sline;
    =LowerCase (q: sline): sline;
    =trim (theline: sline): sline;
    =trimwhite (theline: sline): sline;
    =copies (pattern, linetext: sline): integer;
    =replace (oldpattern, newpattern, linetext: sline): sline;
    =index (pattern: sline; sarray: ArrOfStrPtr; n: integer): integer;
    =reverse (s: sline): sline;
    =IsAlpha (c: char): boolean;
    =IsDigit (c: char): boolean;
    =IsAlphanumeric (c: char): boolean;
    =NoLeadSpaces (s: sline): integer;
    =NoLeadWhite (s: sline): integer;
    =LeftTrim (s: sline; amount_to_shave: integer): sline;
    =RightTrim (s: sline; amount_to_shave: integer): sline;
*/

/* xon: string.0 */
/* xtoc: #Strings */
/* xpage */
/*
	STRINGS

	In these routines, the char * that is returned is a temporary
pointer that these routines memory manage.  It will contain the correct
contents for the first usage, but is not guaranteed for consecutive use.
If you want the value, copy it into your own string.  These routines 
provide their own garbage collection, so you don't have to worry about 
free()ing the strings returned by these routines.
	Note:  The input strings are not modified by any of these routines.

	To use these routines, you may want to include the function 
prototypes in
	#include <strfn.h>
(strfn.h is in /usr/northware/include).

*/
/* xoff */

#include "strP.h"
 
/* xon: string.010 */
/* xtoc: 	str_instr()	- locate pattern in string */
int str_instr (pattern, string)
char *pattern;	/* INPUT:  pattern you are searching for */
char *string;	/* INPUT:  string being searched */
/*
	This routine returns the character position of /pattern/ in
/string/.  The first match is the one used.  The number returned is
the first character position of the match.  Valid output is 0 and up.
A -1 returned means the pattern was not found.
*/
/* xoff */
{
  int ofs, i, len;
  len= strlen (string);
  for (i= 0; i<len; i++) {
    /* find matching first character */
    while (i<len && *(string+i) != *(pattern))
      i++;
    /* find all matching characters */
    for (ofs= 0;
	  i+ofs<len &&
	    *(pattern+ofs)>0 &&
	    *(pattern+ofs) == *(string+i+ofs);
	 ofs++) ;
    /* if we completed whole pattern, we're done */
    if (*(pattern+ofs) == '\0')
      return (i);  /* return 1st char position */
    /* otherwise, next char... */
    }
  /* pattern not found */
  return (-1);
}
 

/* xon: string.020 */
/* xtoc: 	str_chrpos()	- locate character in string */
int str_chrpos (search_char, string)
int search_char;   /* INPUT:  character you are searching for */
char *string;	    /* INPUT:  string being searched through */
/*
	This routine returns the character position of the first
match of the search character in /string/.  If /search_char/ is not
in /string/, -1 is returned.
*/
/* xoff */
{
  int len, pos;
  len= strlen (string);
  for (pos= 0; pos<len; pos++)
    if (search_char==string[pos])
      return (pos);
  return (-1);
}
 
#define ROTATION 50
char *strfn_strings[ROTATION];
int strfn_strlens[ROTATION], strfn_first= TRUE, strfn_next= 0;
char strfn_nada0= '\0';
char *strfn_nada= &strfn_nada0;
 
#define getstr(param) strfn_getstr(param)
 
char *getstr(len)
int len;    /* INPUT:  length of requested variable */
/*
   This routine allows for temporary strings to be used and not bother
   with garbage collection.  I don't expect an expression to be so
   complicated that more than ROTATION strings are temporary.
*/
{
  if (len<=0)
    return (strfn_nada);
  if (strfn_first) {
    int i;
    for (i= 0; i<ROTATION; i++)
      strfn_strlens[i]= 0,
      strfn_strings[i]= NULL;
    strfn_first= !strfn_first;
    }
  strfn_next = (strfn_next+1) % ROTATION;
  if (strfn_strlens[strfn_next]<len) {
    if (strfn_strings[strfn_next] != NULL)
      free (strfn_strings[strfn_next]);
    strfn_strings[strfn_next]= (char *) malloc (len+1);
    strfn_strlens[strfn_next]= len;
    }
  return (strfn_strings[strfn_next]);
}
 
/* xon: string.030 */
/* xtoc: 	str_midstr()	- extract a substring from string */
char *str_midstr (thestring, start, length)
char *thestring;    /* INPUT: the main string */
int start;	    /* INPUT: starting character */
int length;	    /* INPUT: length of substring */
/*
	Extract a substring from /thestring/.  The substring starts
at the /start/th character in /thestring/ and has a length of /length/.
Error checking is performed, so null strings will be returned if the
substring is bogus or unavailable.
*/
/* xoff */
{
  char *target;
  int i, reall;
  reall= strlen(thestring);
  /* this actually prevents A LOT of errors... */
  if (length<=0 || start>=reall)
    return  (strfn_nada);
  if (length>(int)strlen(thestring))
    length= strlen(thestring)-start+1;
  target= getstr(length);
  for (i= 0; i<length; i++)
    *(target+i)= *(thestring+start+i);
  *(target+length)= '\0';
  return (target);
}
 

/* xon: string.040 */
/* xtoc: 	str_concat()	- concatenate two strings */
char *str_concat (string1, string2)
char *string1;	/* INPUT:  first string */
char *string2;	/* INPUT:  second string */
/*
	Concatenate two strings together.
*/
/* xoff */
{
  char *target;
  target= getstr (strlen(string1)+strlen(string2)+1);
  strcpy (target, string1);
  strcat (target, string2);
  return (target);
}
 

/* xon: string.050 */
/* xtoc: 	str_right()	- extract rightmost characters of string */
char *str_right (linetext, n)
char *linetext;	    /* INPUT:  the string in question */
int n;		    /* INPUT:  the amount to copy */
/*
	Return the /n/ rightmost characters of the string.  If the
string is shorter than n, the whole string is returned.
*/
/* xoff */
{
  int len;
  if (n<(len=strlen(linetext)))
    return (str_midstr(linetext, len-n, n));
  else
    return (linetext);
}
 

/* xon: string.060 */
/* xtoc: 	str_left()	- extract leftmost characters of string */
char *str_left (linetext, n)
char *linetext;	    /* INPUT:  the string in question */
int n;		    /* INPUT:  the amount to copy */
/*
	Return the /n/ leftmost characters of the string.  If the
string is shorter than n, the whole string is returned.
*/
/* xoff */
{
  return (str_midstr (linetext, 0, n));
}
 

/* xon: string.070 */
/* xtoc: 	str_UpperCase()	- convert string to upper case */
char *str_UpperCase (q)
char *q;    /* INPUT:  string to be converted */
/*
	Return the input string completely in upper case.  (The input
string is NOT altered.
*/
/* xoff */
{
  char *target, *hitman;
  target= getstr (strlen(q));
  strcpy (target, q);
  for (hitman= target; *hitman; hitman++)
    if (islower(*hitman))
      *hitman= toupper(*hitman);
  /* strupr (target); */
  return (target);
}
 

/* xon: string.080 */
/* xtoc: 	str_LowerCase()	- convert string to lower case */
char *str_LowerCase (q)
char *q;    /* INPUT:  string to be converted */
/*
	Return the input string completely in lower case.  (The input
string is NOT altered.)
*/
/* xoff */
{
  char *target, *hitman;
  target= getstr (strlen(q));
  strcpy (target, q);
  for (hitman= target; *hitman; hitman++)
    if (isupper(*hitman))
      *hitman= tolower(*hitman);
  /* strlwr (target); */
  return (target);
}
 

/* xon: string.090 */
/* xtoc: 	str_trim()	- trim spaces from begining and end */
char *str_trim (theline)
char *theline;	/* INPUT: the string to be trimed */
/*
	Remove spaces from the beginning and end of the line.  If
there are no leading or trailing spaces, then the text remains the
same.  (The input string is NOT altered.)
*/
/* xoff */
{
  int stop;
  char *target;
 
  target= getstr (strlen(theline));
  strcpy (target, theline);
  while (target[0]==' ')
    target++;
  stop= strlen(target)-1;
  while (stop>0 && *(target+stop)==' ')
    stop--;
  *(target+(stop>=0? stop+1: 0))= '\0';  /* set EOS */
  return (target);
}
 
/* xon: string.095 */
/* xtoc: 	str_trimwhite()	- trim whitespace from beginning and end */
char *str_trimwhite (theline)
char *theline;	/* INPUT: the string to be trimed */
/*
	Remove spaces, newlines, tabs, returns,  from the beginning and
end of the line.  If there are no leading or trailing spaces, then the
text remains the same.  (The input string is NOT altered.)
*/
/* xoff */
{
  int stop;
  char *target;
 
  target= getstr (strlen(theline));
  strcpy (target, theline);
  while ((target[0]==' ') || (target[0]=='\t') ||
	(target[0]=='\n') || (target[0]=='\r'))
    target++;
  stop= strlen(target)-1;
  while (stop>0 && ((*(target+stop)==' ') || (*(target+stop)=='\t') ||
	(*(target+stop)=='\n') || (*(target+stop)=='\r')))
    stop--;
  *(target+(stop>=0? stop+1: 0))= '\0';  /* set EOS */
  return (target);
}
 

/* xon: string.100 */
/* xtoc: 	str_copies()	- count number of copies of pattern in string */
int str_copies (pattern, linetext)
char *pattern;	    /* INPUT:  the pattern being searched for */
char *linetext;	    /* INPUT:  the string being searched */
/*
	Return the number of unique copies of /pattern/ in /linetext/.
The value returned will be 0 or greater.
*/
/* xoff */
{
  int p0, p= 0, n= 0, patlen;
 
  if ((patlen= strlen(pattern))==0)
    return (0);
  while ((p0= str_instr (pattern, linetext+p))>=0) {
    n++;
    p += p0+patlen;
    }
  return (n);
}
 

/* xon: string.110 */
/* xtoc: 	str_replace()	- replace pattern with another pattern */
char *str_replace (oldpattern, newpattern, linetext)
char *oldpattern;   /* INPUT:  present pattern (to be replaced) */
char *newpattern;   /* INPUT:  new, replacement pattern */
char *linetext;	    /* INPUT:  the line to be affected */
/*
	Return a string similar to /linetext/, but with all unique 
occurances of the pattern /oldpattern/ replaced with /newpattern/.
The new pattern may contain the old pattern without problems.
*/
/* xoff */
{
  int p, opl, npl, ltl, pos= 0;
  char *target, *newtarget;
  int repcount = ROTATION*5; /* dav: countdown of replacements */
 
  opl= strlen(oldpattern);
  npl= strlen(newpattern);
  ltl= strlen(linetext);
  if (npl <= opl) /* new size won't be larger */
    target= (char *) malloc (ltl+1);
  else /* guess that the new string won't have more than
	5*ROTATIONs occurances */
    target= (char *) malloc (ltl+(npl-opl)*ROTATION*5+2);
  strcpy (target, "");
  while ((p= str_instr (oldpattern, linetext+pos))>=0) {
    strcat (target, str_midstr(linetext, pos, p));  /* chars upto pattern */
    strcat (target, newpattern);  /* make replacement */
    pos += p+opl;
    if ((--repcount) == 0) { /* dav: too many replaces */
	repcount = ROTATION*5; /* dav: reset */
	if (npl <= opl) { /* dav: new size won't be larger */
	    continue; /* dav: */
	} /* dav: */
	newtarget = target; /* dav: save string so far */
	ltl += (npl-opl)*ROTATION*5; /* dav: update ltl */
	/* dav: ok, we can make 5*ROTATION space again */
	target = (char *) malloc (ltl+(npl-opl)*ROTATION*5+2); /* dav: */
	strcpy(target, newtarget); /* dav: copy over string so far */
	free(newtarget); /* dav: get rid of old target space */
    } /* dav: */
  }
  strcat (target, linetext+pos);
  newtarget= getstr (strlen(target));
  strcpy (newtarget, target);
  free (target);
  return (newtarget);
}
 

/* xon: string.120 */
/* xtoc: 	str_index()	- linear search an array of strings */
int str_index (pattern, data, no_indices)
char *pattern;	    /* INPUT:  pattern being sought */
char **data;	    /* INPUT:  array of strings being searched */
int no_indices;	    /* INPUT:  no of strings in /data/ array */
/*
	Given an array of pointers, /data/, and a pattern to search for,
/pattern/, and the number of strings in /data/, this routine will return
the index into /data/ that pattern matches exactly.
	This routine is useful when you have a list of keywords and you
want to find out which keyword has been selected using one routine call.
*/
/* xoff */
{
  int i;
  for (i= 0; i<no_indices; i++)
    if (pattern[0]== (*(data+i))[0])
    if (strcmp (pattern, *(data+i))==0)
      /* found pattern, return index */
      return (i);
  /* didn't find pattern, oh well */
  return -1;
}
 

/* xon: string.130 */
/* xtoc: 	str_reverse()	- reverse the order of characters */
char *str_reverse (s)
char *s;	/* INPUT: string to be reversed */
/*
	Return the input string with all its characters in the reverse
order (without altering the input string).  So, an input string of "bark"
will be reversed to "krab".
*/
/* xoff */
{
  int l, i;
  char *target;
 
  target= getstr (l= strlen(s));
  target[l]= '\0';
  for (i= 0; i<l; i++)
    target[i]= s[l-i-1];
  return (target);
}
 

/* xon: string.140 */
/* xtoc: 	str_NoLeadSpaces()	- count the number of leading spaces */
int str_NoLeadSpaces (s)
char *s;    /* INPUT:  string in question */
/*
	Return the count of the number of leading spaces in the input
string.  This routine returns 0 or larger.
*/
/* xoff */
{
  int i=0, l;
  l= strlen(s);
  while (i<=l && s[i]==' ')
    i++;
  return (i);
}
 

/* xon: string.145 */
/* xtoc: 	str_NoLeadWhite()	- count the number of whitespace chars */
int str_NoLeadWhite (s)
char *s;    /* INPUT:  string in question */
/*
	Return the count of the number of leading spaces and tabs in the 
input string.  This routine returns 0 or larger.
*/
/* xoff */
{
  int i=0, l;
  l= strlen(s);
  while (i<=l && ((s[i]==' ') || (s[i]=='\t')))
    i++;
  return (i);
}
 

/* xon: string.150 */
/* xtoc: 	str_LeftTrim()	- remove leftmost characters */
char *str_LeftTrim (s, amount_to_shave)
char *s;	    /* INPUT:  string in question */
int amount_to_shave; /* INPUT: amount to remove from left */
/*
	Return the input string with the leftmost characters removed.
/amount_to_shave/ characters are removed from the left of the string.
If that quantity is larger than the string, then return the null string.
*/
/* xoff */
{
  char *target;
  int l;
  if (amount_to_shave>(l=strlen(s)) || amount_to_shave<1)
    return (strfn_nada);
  strcpy (target= getstr(l-amount_to_shave), s+amount_to_shave);
  return (target);
}
 

/* xon: string.160 */
/* xtoc: 	str_RightTrim()	- remove rightmost characters */
char *str_RightTrim (s, amount_to_shave)
char *s;	    /* INPUT:  string in question */
int amount_to_shave; /* INPUT: amount to remove from right */
/*
	Return the input string with the rightmost characters removed.
/amount_to_shave/ characters are removed from the right of the string.
If that quantity is larger than the string, then return the null string.
*/
/* xoff */
{
  int l;
  if (amount_to_shave>=(l=strlen(s)))
    return (strfn_nada);
  else
    return (str_midstr (s, 0, l-amount_to_shave));
}
