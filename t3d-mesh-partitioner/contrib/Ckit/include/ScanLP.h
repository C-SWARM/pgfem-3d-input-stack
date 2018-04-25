#ifndef SCANLP_H
#   define SCANLP_H

#include "ckitcfg.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "listP.h"


#define SCANLP_MAX_LINE_LENGTH 256

typedef struct ScanLineBuf {
  FILE       *stream;
  int         scanbuf_dim;
  char       *scanbuf;
  int         curr_line_num;
  int         line_valid;
  int         line_unread;
  int         comment_char;
}              ScanLineBuf;


/* To create new scan-line buffer.  The buffer is then used to retrieve */
/* the next line from the /stream/.  Note, that the /stream/ must have been */
/* opened before for reading.  You can specify /scanbuf_dim/ as zero, in */
/* which case 256 is used.  The lines in the /stream/ may start with */
/* a comment character.  In that case they are ignored. */
ScanLineBuf *
SL_new_buffer(FILE *stream, int scanbuf_dim, int comment_char);


/* This function scans next line from the stream associated to /slb/ */
/* and returns pointer to a string holding the contents.  Note, that you */
/* can access the number of the line read by SL_line_number(). */
char *
SL_scan_line(ScanLineBuf *slb, int eof_possible);

/* You may access the current line w/o actually reading.  This function */
/* returns either the previously read string, or if there is no such */
/* string, it performs the actual reading by calling SL_scan_line(). */
char *
SL_current_line(ScanLineBuf *slb, int eof_possible);

/* You may put back a line (to pretend that it has not been read). */
void
SL_put_back_line(ScanLineBuf *slb);


/* To access the current line number.  This is useful, if you want to */
/* report e.g. bad data in a stream. */
int
SL_line_number(ScanLineBuf *slb);


/* To destroy the buffer and to free its resources */
ScanLineBuf *
SL_delete_buffer(ScanLineBuf *slb);


#endif /* ifndef SCANLP_H */
