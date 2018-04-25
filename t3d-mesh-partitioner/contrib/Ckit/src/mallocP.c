/*************************************************************************

	C Programmers' Toolbox
	Copyright (c) 1989 by Carnegie-Mellon University, Pittsburgh, PA.

	Module Name:	memoryPack module One.

	Purpose:	Memory management routines.  The standard Unix
			free1(), malloc1(), calloc(), and realloc1() are
			provided, as well copy_alloc() alloc_size(),
			and alloc_refcount()

	Author:		Thaddeus Julius Kowalski (1983,1984,1985)
			Robert A Zimmermann (1985 ...)

	Notes:		Compile with the DO_CHECK option set to n or greater:
			0 - Do no run time checking or error reporting
			1 - Report errors
			2 - Generate statistics

	History:	
	05 Dec 85	Complete re-write:
			    to allow more flexibility of allocation sizes
			    to allow refcounts
			    to better generate stats
	10 Dec 85	Expanded alloc_size in-line for speed
			Changed statistics, slightly
			Sorted the big-list in malloc()
	10 Jan 86	Added alloc_refcount

	28 Jul 86	Fixed realloc bug

	31 Jul 86	Added berkeley o/s testing

	15 Sep 86	Changed which_size to a short.
			malloc will now break if 32 gigabyte blocks are req'd

	04 Dec 86	Removed copymem in favor of bcopy

	25 Feb 87	Added bcopy macro on BELL machines

	17 Mar 87	Added proper defs for non-debug
			Added proper include stuff

	21 Apr 87	Added non-berkley error messages

	13 May 87	Folded in Roy Jewell apollo code

	01 Sep 87	Set calloc() to be compiled on AEGIS

 *************************************************************************/

/*--------------------------------------------------------------------------*\
|
|  Serpent Change History
|
$Log: mallocPack.c,v $
 * Revision 1.5  1991/04/11  21:11:56  little
 * no changes
 *
 * Revision 1.4  91/02/19  14:45:47  rcs
 * added call to perror for reasons i can't remember
 * 
 * Revision 1.3  89/12/23  21:14:10  ejh
 * no change
 * 
 * Revision 1.2  89/07/13  14:53:31  bmc
 * Added u22_process_name and identify_process() macro and modified error
 * messages to identify the process incurring the error.
 * 
\*--------------------------------------------------------------------------*/

/*
This is a very fast storage allocator.  It allocates blocks of a small 
number of different sizes, and keeps free lists of each size.  Blocks that
don't exactly fit are passed up to the next larger size.  Beyond the largest
size, it will allocate blocks of the requested size (rounded up to the
nearest 1k boundary) and do a first-fit reallocation from freed blocks.

In this implementation, the available sizes stored in the 'nodes' table.
This table contains the information needed to do allocation, and will
maintain statistics on their use.  Additional sizes may be added by changing
the 'SIZES' constant, and adding the desired block size.  Warning:  be
sure to include the 4 byte overhead.

This is designed for use in a program that uses vast quantities of memory,
but bombs when it runs out.  To make it a little better, it warns the
user when he starts to get near the end.


allocation routines:
NODE	malloc1( size )		Returns a pointer to a block of memory
				with SIZE bytes.

NODE	copy_alloc( node )	Returns a pointer to a block of memory
				with with a copy of 'node'.

NODE	realloc1( node, new_size )
				Allocates a new block of memory, and
				copies as much data as possible into
				the new node.  The old node is freed.
	
NODE	dup_alloc( node )	Increments the reference count of
				the node.

deallocation routine:
int	free1( node )		Decrements the refcount, and returns the
				node to the free-list if it was zero.

other routines:
int	alloc_size( node )	Returns number of bytes in node

int	alloc_refcount( node )	Returns the reference count
*/

/************************************************************************
 *		Set up defaults, error handlers, etc.			*
 ************************************************************************/

#define PRIVATE_MEMORY_NODES
#include "membase.h"

NODE malloc1(unsigned nbytes);
int dup_alloc(register NODE ap);
int alloc_refcount(register NODE ap);
int free1(register NODE ap);
unsigned alloc_size(register NODE ap);
NODE realloc1(register NODE ap, register unsigned nbytes);
NODE copy_alloc(register NODE	ap);

#ifndef MSDOS
STATIC	 NODE	morecore(register int size);
#endif

#ifdef STANDARD_MALLOC
#else
char *u22_process_name= "UNKNOWN"; /* identifies current process */

#ifdef SYSV
#define identify_process() \
	fprintf( stderr, "%s (PID %d): ", u22_process_name, getpid() );
#else
#define identify_process() \
	fprintf( stderr, "%s ", u22_process_name );
#endif

#ifdef	DEBUG
#define DO_CHECK	3
#else
#define DO_CHECK 0
#endif

#ifndef DO_CHECK		/* Set default error checking	*/
#define DO_CHECK	2
#endif


#ifdef BERK
#include <sys/time.h>		/* needed by the storage requester */
#include <sys/resource.h>
#endif

#ifdef BELL
#define	bcopy( from, to, nbytes )	memcpy( to, from, nbytes )
#endif

#ifdef apollo
#define SBRKSIZE 8000000   /* Apollo's max dynamic memory was chosen so that it     */
                           /* would be compatible with a dn300,as well as a 3000    */
                           /* A dn3000 can handle handle quite a bit more.          */ 
#endif 

#include <stdio.h>

#if	DO_CHECK > 0

STATIC char *botch    = "mallocPack:  assertion failed: %s\n";
STATIC char *free_err = "%s:  attempt to access a (probably) freed node 0x%x\n";

#define ASSERT(p)   if (!(p)) {					\
			identify_process();			\
			fprintf( stderr, botch, " p " );	\
			abort();				\
			}
#if DO_CHECK > 1
#define	MAGIC			0x1b	/* random value for tagging our nodes */
#define ASSERT_MAGIC( mp )	ASSERT(mp->head.our_block == MAGIC)

#define	BAD_POINTER(func,addr)				\
	if (mp->head.our_block != MAGIC) {		\
	    identify_process();				\
	    fprintf( stderr, free_err, func, addr );	\
	    return 0;					\
	    }

#else
#define ASSERT_MAGIC( mp )
#define	BAD_POINTER(func,addr)	
#endif

#else
#define ASSERT(p)
#define ASSERT_MAGIC( mp )
#define	BAD_POINTER(func,addr)	
#endif

int mstats(FILE *fp, char *s);


#if DO_CHECK > 1
/************************************************************************
 * 		mstats - print out statistics about malloc		*
 ************************************************************************/

#define	another_node( ptr )	ptr -> num_in_use++;	ptr -> num_free++;

STATIC unsigned int	allocated = 0,
			adjust_count = 0,
			adjust_size  = 0;

int mstats(FILE *fp, char *s)
{
  register long_memory p;
  register int i;
  
  fprintf( fp, "Memory allocation statistics for %s\n", s);
  
  for (i=0; i< LAST_LIST ; i++) {
    if (i > 0 && nodes[i].size <= nodes[i-1].size)
      fprintf( fp, "warning: list %d (size %d) smaller that list %d (size %d)\n",
	      i, nodes[i].size, i-1, nodes[i-1].size );
    if ((nodes[i].size % sizeof(NODE)) != 0)
      fprintf( fp, "warning: list %d (size %d) not pointer aligned\n",
	      i, nodes[i].size );
  }
  
  fprintf( fp, "Sizes up to:\tin use:\tfree:\tmalloc'd:\tAvg Bytes wasted:" );
  for (i=0; i < LAST_LIST; i++) {
    fprintf( fp, "\n\t%d\t%d \t%d \t%d",
	    nodes[i].size, nodes[i].num_in_use - nodes[i].num_free,
	    nodes[i].num_free, nodes[i].nmalloc );
    if ( nodes[i].nmalloc )
      fprintf( fp, " \t\t%d", nodes[i].bytes_wasted / nodes[i].nmalloc );
  }
  
  fprintf( fp, "\n<big nodes>\t%d \t%d \t%d\n",
	  nodes[BIG_LIST].num_in_use - nodes[BIG_LIST].num_free,
	  nodes[BIG_LIST].num_free, nodes[BIG_LIST].nmalloc );
  
  for (p = long_nodes; p != NULL; p = p->next_node)
    fprintf( fp, "\t0x%x, refcount(%d), size(%dK)\n",
	    p -> node, p -> head.refcount, p -> head.which_size );
  
  fprintf( fp, "\nTotal Allocated = %dKbytes", allocated / 1024 );
  fprintf( fp, "\nRe-aligned sbrk point %d times for %d bytes\n",
	  adjust_count, adjust_size );
}
#else
int mstats(FILE *fp, char *s) {}
#define	another_node( ptr )
#endif
     
STATIC	 NODE	morecore(register int size) /* number of bytes to get */
{   
  extern	char	*sbrk();
  register	NODE	cp;
#ifdef BERK
  struct	rlimit	rl;
#endif
  
#ifdef apollo
  STATIC short initialized=0;  /* flag to tell us if we've been here before */
  /*          
   *  On apollo you must either call set_sbrk_size or set an environment
   *  variable SBRKSIZE if you desire to dynamically allocate more than
   *  256k bytes of memory.  No matter what Apollo says, if the environment
   *  variable is set, it will override any calls to set_sbrk_size. 
   */
  if(!initialized)
    {
      set_sbrk_size((int)SBRKSIZE);
      initialized=1;
    }
#endif
  
  /* ensure that we start on BLOCK_SIZE boundaries */
  if ((((int) (cp = (NODE) sbrk(0))) & BLOCK_BITS) != 0) {
#if DO_CHECK > 1
    adjust_count++;
    adjust_size += BLOCK_SIZE - (((int) cp ) & BLOCK_BITS);
#endif
    sbrk( BLOCK_SIZE - (((int) cp ) & BLOCK_BITS) );
  }
  
  if ((int) (cp = (NODE) sbrk( size )) == -1) {	/* no more room!  */
#ifdef BERK
#ifndef apollo
    getrlimit( RLIMIT_DATA, &rl );
    if (rl.rlim_cur >= rl.rlim_max) {
      write( 2, "mallocPack:  Ran out of memory\7\n", 34 );
      abort();
    }
    rl.rlim_cur = rl.rlim_max;
    setrlimit( RLIMIT_DATA, &rl );
#if DO_CHECK
    write( 2, "Warning:  Setting data resource limit to max\7\n", 48 );
#endif
#endif
    if ((int) (cp = (NODE) sbrk( size )) == -1) {
#if DO_CHECK
      write( 2, "mallocPack:  attempt at sbrk() failed\7\n", 41 );
      perror("mallocPack");
      abort();
#else
      return NULL;
#endif
    }
#else
#if DO_CHECK
    write( 2, "mallocPack:  attempt at sbrk() failed\7\n", 41 );
    abort();
#else
    return NULL;
#endif
#endif
  }
  
#if DO_CHECK > 1
  allocated += size;	/* save the amount of memory we have allocated */
#endif
  return cp;
}

NODE malloc1(unsigned nbytes)    /* get a block */
{
  register int		nb, size, index;
  register memory		p;
  register long_memory	lp, *lp2;
  
  if (nbytes == 0)	return NULL;
  
  /* If the size needed exceeds the biggest standard sized block... */
  if ((nb  = nbytes + sizeof( header )) > BIGGEST_BLOCK) {
    
    nb   = nbytes;
    size = (nb & BLOCK_BITS);			/* align to 1k */
    if (size > 0) nb += 1024 - size;		/* for efficiency */
    index = nb >> 10;				/* trash the low bits */
    
    for (lp = long_nodes; lp != NULL; lp = lp -> next_node)
      if (lp -> head.refcount == 0 && lp -> head.which_size >= index)
	break;
    
    if (lp == NULL) {
      
      another_node((&nodes[ BIG_LIST ]));		/* count the nodes */
      lp = (long_memory) malloc1( sizeof( struct long_memory_struct ) );
      lp -> head.which_size = index;
      if (( lp -> node = morecore( nb )) == NULL) return NULL;
      
      /* insert pointer into list of long nodes */
      for ( lp2 = (long_memory *) &(nodes[BIG_LIST].next_node); ;
	   lp2 = &((*lp2)->next_node) )
	if ( *lp2 == NULL || (*lp2)->head.which_size >= index ) {
	  lp -> next_node	= *lp2;
	  (*lp2)		= lp;
	  break;
	}
    }
    
#if DO_CHECK > 1
    nodes[ BIG_LIST ].num_free--;
    nodes[ BIG_LIST ].nmalloc++;
#endif
    lp  -> head.refcount		= 1;
    return lp -> node;
  }
  
  else {
    register struct ckit_node_list	*ptr;
    
    for (index = 0, ptr = nodes; (index < SIZES) && (ptr -> size < nb);
	 ptr++, index++)	/* null loop statement */ ;	/* find index */
    
    if (ptr -> next_node == NULL) {		/* get more memory */
      /* minimum request is BLOCK_SIZE */
      size = (ptr -> size > BLOCK_SIZE) ? ptr -> size : BLOCK_SIZE;
      
      if ((ptr -> next_node = p = (memory) morecore( size )) == NULL) return NULL;
      
      for ( ;; ) {    		/* set up linked list of the nodes */
	another_node( ptr );	/* keep track of number of nodes */
	if ((size -= ptr->size) < ptr->size) {
	  p -> next_node = NULL;
	  break;
	}
	p = p -> next_node = (memory) (((int) p) + ptr->size );
      }
    }
    
    p = ptr -> next_node;
    ptr -> next_node	= p->next_node;	/* update free-list */
    p   -> head.which_size	= index;	/* save the list index */
    p   -> head.refcount	= 1;		/* block is in use */
#if DO_CHECK > 1
    p   -> head.our_block  = MAGIC;		/* tag it for later */
    ptr -> num_free--;
    ptr -> nmalloc++;
    ptr -> bytes_wasted += ptr -> size - nb;
#endif
    return (NODE) (((int) p) + sizeof( header ));
  }
}

/************************************************************************
 *	dup_alloc - Duplicate a node.  Actually this just increments	*
 *			the node's reference count			*
 ************************************************************************/

int dup_alloc(register NODE ap)
{
  register memory		mp;
  register long_memory	lp;
  
  if (ap == NULL) return;		/* ignore this call */
  
  if (is_small( ap )) {		/* small sized node */
    
    mp = (memory) (ap - sizeof(header));	/* point back to header */
    
    ASSERT_MAGIC( mp );			/* make sure it's one of ours */
    
    if (mp->head.refcount < MAX_REFCOUNT)	/* so we don't overflow char */
      ++mp->head.refcount;		/* bump the reference count */
  }
  else {
    for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)
      if (lp -> node == ap) break;
    
    ASSERT( lp != NULL );
    if (lp->head.refcount < MAX_REFCOUNT)	/* so we don't overflow char */
      ++lp->head.refcount;		/* bump the reference count */
  }
}

int alloc_refcount(register NODE ap)
{
  register memory		mp;
  register long_memory	lp;
  
  if (ap == NULL) return 0;		/* ignore this call */
  
  if (is_small( ap )) {		/* small sized node */
    
    mp = (memory) (ap - sizeof(header));	/* point back to header */
    
    BAD_POINTER( "alloc_refcount", ap );	/* do additional checking */
    
    return mp->head.refcount;
  }
  
  for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)
    if (lp -> node == ap) break;
  
  ASSERT( lp != NULL );
  
  return lp->head.refcount;
}

int free1(register NODE ap)
{
  register memory		mp;
  register long_memory	lp;
  register struct ckit_node_list	*ptr;
  
  if (ap == NULL) return;		/* ignore this call */
  
  if (is_small( ap )) {		/* small sized node */
    
    mp = (memory) (ap - sizeof(header));	/* point back to header */
    
    BAD_POINTER( "free", ap );		/* do additional checking */
    
    if (mp->head.refcount == MAX_REFCOUNT) return;	/* in use */
    
    if (--mp->head.refcount > 0) return;		/* still in use */
    
    ASSERT(mp->head.which_size < BIG_LIST);		/* a valid list index */
    
    ptr = &( nodes[ mp->head.which_size ] );	/* get list structure */
    mp  -> next_node = ptr -> next_node;
    ptr -> next_node = mp;
#if DO_CHECK > 1
    ptr -> num_free++;
#endif
    return 0;
  }
  
  for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)
    if (lp -> node == ap) break;
  
  ASSERT( lp != NULL );
  
  if (lp->head.refcount == MAX_REFCOUNT) return;	/* in use */
  
#if DO_CHECK > 1
  if (lp->head.refcount == 0) {
    identify_process();
    fprintf( stderr, free_err, "free", ap );
    return 0;
  }
#endif
  
  if (--lp->head.refcount != 0) return;		/* is it free? */
  
#if DO_CHECK > 1
  nodes[ BIG_LIST ].num_free++;
#endif
  return;
}

#define	size_of_node( val, ap )						\
    if (is_small( ap )) {			/* small sized node */	\
	register memory mp;			/* temp. register */	\
	mp = (memory) (ap - sizeof(header));	/* point to header */	\
	ASSERT_MAGIC( mp );			/* it's one of ours? */	\
	ASSERT(mp->head.which_size < BIG_LIST);	/* valid list index? */	\
	val = nodes[ mp->head.which_size ].size - sizeof( header );	\
	}								\
    else {					/* big-list node */	\
	register long_memory lp;		/* temp. register */	\
	for (lp = long_nodes; lp != NULL;  lp = lp -> next_node)	\
	    if (lp -> node == ap) break;	/* found the record? */	\
	ASSERT( lp != NULL );			/* check to be sure */	\
	val = ((lp -> head.which_size) << 10);	/* load the value */	\
								      }

unsigned alloc_size(register NODE ap)
{
  register unsigned temp;
  
  if (ap == NULL) return 0;
  
  size_of_node( temp, ap );
  
    return temp;
    }

NODE realloc1(register NODE ap, register unsigned nbytes)
{
  register NODE		res;
  register struct ckit_node_list	*ptr;
  register unsigned int	size;
  
  if (ap == NULL)			return malloc1(nbytes);
  
  if (nbytes == 0)			return (NODE) free1(ap);
  
  size_of_node( size, ap );		/* how big is it? */
  
  if ((res = malloc1(nbytes)) != NULL)
    bcopy( ap, res, (nbytes > size) ? size : nbytes );
  
  free1( ap );
  
  return res;
}

NODE copy_alloc(register NODE	ap)
{
  register NODE		res;
  register unsigned int	onb;
  
  if (ap == NULL) return NULL;
  
  size_of_node( onb, ap );
  
  if ((res = malloc1(onb)) == NULL)	return(NULL);
  
  bcopy( ap, res, (int) onb );
  
  return res;
}

#ifdef AEGIS

NODE calloc(number, nbytes)
{
  register NODE	  p;
  register int  	  *save;
  register unsigned int onb;
  
  onb = number * nbytes;	/* number of bytes to be allocated */
  if ((p = malloc1( onb )) == NULL)	return NULL;
  
  onb = ((onb - 1)/sizeof(int)) + 1;	/* number of words to be cleared */
  for (save = (int *) p; onb-- != 0; *save++ = 0);
  
  return p;
}

#endif

#ifdef DEBUG
#ifdef NEED_MAIN

#define	BLOCKS	1500

main() {
  NODE my_nodes[ BLOCKS ], from_malloc, from_calloc;
  int  i;
  
  /* make sure we're the only library running */
  from_malloc = malloc1( 1 );
  free1( from_malloc );
  from_calloc = calloc( 1, 1 );
  free1( from_calloc );
  if ( from_malloc != from_calloc ) {
    identify_process();
    fprintf( stdout, "Warning: malloc1() and calloc() not using same heap\n" );
  }
  
  for (i=0; i<BLOCKS; i++)
    my_nodes[i] = malloc1( i );
  mstats( stdout, "After initial allocation" );
  
  for (i=0; i<BLOCKS; i+=2)
    free1( my_nodes[i] );
  mstats( stdout, "After partial freeing" );
  
  for (i=0; i<BLOCKS; i+=2)
    my_nodes[i] = malloc1( i );
  mstats( stdout, "After second allocation (should be same as first)" );
  
  for (i=0; i<BLOCKS; i++)
    free1( my_nodes[i] );
  mstats( stdout, "After final free (everything free except 16's)" );
}
#endif /* NEED_MAIN */
#endif
#endif /* STANDARD_MALLOC */


