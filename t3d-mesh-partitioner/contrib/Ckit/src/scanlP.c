#include "ScanLP.h"


ScanLineBuf *
SL_new_buffer(FILE *stream, int scanbuf_dim, int comment_char)
{
  ScanLineBuf *slb;

  slb = (ScanLineBuf *)make_node(sizeof(ScanLineBuf));
  if (slb == NULL)
    return NULL;
  slb->stream = stream;
  slb->scanbuf_dim = scanbuf_dim;
  if (slb->scanbuf_dim <= 0) slb->scanbuf_dim = SCANLP_MAX_LINE_LENGTH;
  slb->scanbuf = (char *)make_node(slb->scanbuf_dim);
  if (slb->scanbuf == NULL)
    return NULL;
  slb->curr_line_num = 0;
  slb->line_valid    = 0;
  slb->line_unread   = 0;
  slb->comment_char  = comment_char;

  return slb;
}


char *
SL_scan_line(ScanLineBuf *slb, int eof_possible)
{
  int i;

  if (slb->line_unread) {
    slb->line_unread = 0; 
    return slb->scanbuf; /* return the "unread" line */
  }

  slb->line_unread = 0; 
  slb->line_valid = 0; /* scan buffer is not valid yet */
 try_next:
  (slb->curr_line_num)++;
  if (fgets(slb->scanbuf, SCANLP_MAX_LINE_LENGTH-1, slb->stream) == NULL) {
    if (feof(slb->stream)) {
      if (!eof_possible) {
        fprintf(stderr, "Bad data!? Line %d\n", slb->curr_line_num);
      }
      return NULL;
    } else {
      fprintf(stderr, "Bad data!? Line %d\n", slb->curr_line_num);
      return NULL;
    }
  } else {
    if (slb->scanbuf[0] == slb->comment_char)
      goto try_next;
  }
  i = 0;
  while (slb->scanbuf[i] != '\0') 
    if      (slb->scanbuf[i] == '\n')              slb->scanbuf[i] = '\0';
    else if (slb->scanbuf[i] == slb->comment_char) slb->scanbuf[i] = '\0';
    else
      i++;

  slb->line_valid = 1; /* valid scan buffer */
  return slb->scanbuf;
}



char *
SL_current_line(ScanLineBuf *slb, int eof_possible)
{
  if (slb->line_valid) return slb->scanbuf; /* line was read before */
  else {
    return SL_scan_line(slb, eof_possible);
  }
}


void
SL_put_back_line(ScanLineBuf *slb)
{
  if (slb->line_valid) slb->line_unread = 1;
}


int
SL_line_number(ScanLineBuf *slb)
{
  return slb->curr_line_num;
}


ScanLineBuf *
SL_delete_buffer(ScanLineBuf *slb)
{
  free_node(slb->scanbuf_dim);
  free_node(slb);
  return NULL;
}
