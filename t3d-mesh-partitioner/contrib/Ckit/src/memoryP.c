/*************************************************************************

	C Programmers' Toolbox

	Copyright (c) 1989 by Carnegie-Mellon University, Pittsburgh, PA.

	Module Name:	memoryPack

	Purpose:	Memory management routines for non-unix o/s

	Author:		Robert A Zimmermann

	Notes:		Compile with the DO_CHECK option set to n or greater:
			0 - Do no run time checking or error reporting
			1 - Report errors
			2 - Generate statistics

	History:	

	25Jun87	raz	Rewrote to mirror mallocPack

 *************************************************************************/

/************************************************************************
 *		Set up defaults, error handlers, etc.			*
 ************************************************************************/

#define PRIVATE_MEMORY_NODES
#include "membase.h"

#include <stdio.h>

int memory_stats(FILE *fp, char *s);
void test_free_struct(void);
NODE make_node(unsigned nbytes);
NODE malloc1(unsigned nbytes);
int dup_node(register NODE ap);
int node_refcount(register NODE ap);
int free_node(register NODE ap);
unsigned node_size(register NODE ap);
NODE change_node_size(register NODE ap, register unsigned nbytes);
NODE copy_node(register NODE ap);

#ifdef	DEBUG
#define DO_CHECK	3
#else
#define DO_CHECK 0
#endif

#ifndef DO_CHECK		/* Set default error checking	*/
#define DO_CHECK	2
#endif


#if	DO_CHECK > 0
#include <stdio.h>
STATIC char *botch    = "memoryPack:  assertion failed: %s\n";
STATIC char *free_err = "%s:  attempt to access a (probably) freed node 0x%x\n";

#define ASSERT(p)   if (!(p)) {					\
			fprintf( stderr, botch, " p " );	\
			abort();				\
			}

#if DO_CHECK > 1
#define	MAGIC			0x1c	/* random value for tagging our nodes */
#define ASSERT_MAGIC( mp )	ASSERT(mp->head.our_block == MAGIC)

#define	BAD_POINTER(func,addr)				\
	if (mp->head.our_block != MAGIC) {		\
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


#ifndef STANDARD_MALLOC

#if DO_CHECK > 1
/************************************************************************
 * 		memory_stats - print out statistics about memoryPack	*
 ************************************************************************/

#define	another_node( ptr )	ptr -> num_in_use++;	ptr -> num_free++;

int memory_stats(FILE *fp, char *s)
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
  
  fprintf( fp, "Sizes up to:\tin use:\tfree:\tmake'd:\tAvg Bytes wasted:" );
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
}
#else
int memory_stats(FILE *fp, char *s) {}
#define	another_node( ptr )
#endif


void test_free_struct()
{
  int i;
  struct ckit_node_list *ptr;
  memory p;
  long_memory lp;
  
  for (ptr = nodes, i = 0; i < SIZES - 1; ptr++, i++)
    {p = ptr -> next_node;
     while (p)
       p = p -> next_node;
   }
  ptr = &nodes [8];
  if (ptr)
    {lp = (long_memory) ptr -> next_node;
     while (lp)
       lp = lp -> next_node;
   }
  
  return;
}


NODE make_node(unsigned nbytes)    /* get a block */
{
#ifdef DEBUG
  int		nb, size, index;
#else
  register int		nb, size, index;
#endif
  
  if (nbytes == 0)	{
#ifdef DEBUG
    test_free_struct ();
#endif
    return NULL; }
  
#ifdef DEBUG
  test_free_struct ();
#endif
  
  /* If the size needed exceeds the biggest standard sized block... */
  if ((nb  = nbytes + sizeof( header )) > BIGGEST_BLOCK) {
#ifdef DEBUG
    long_memory	lp, *lp2;
#else
    register long_memory	lp, *lp2;
#endif
    
    nb   = nbytes;
    size = (nb & BLOCK_BITS);			/* align to 1k */
    if (size > 0) nb += 1024 - size;		/* for efficiency */
    index = nb >> 10;				/* trash the low bits */
    
    for (lp = long_nodes; lp != NULL; lp = lp -> next_node)
      if (lp -> head.refcount == 0 && lp -> head.which_size >= index)
	break;
    
    if (lp == NULL) {
      
      another_node((&nodes[ BIG_LIST ]));		/* count the nodes */
      lp = (long_memory) make_node((unsigned)
				   sizeof( struct long_memory_struct ) );
      lp -> head.which_size = index;
      if (( lp -> node = (NODE) malloc1( nb )) == NULL) {
#ifdef DEBUG
	test_free_struct ();
#endif
	return NULL;}
      
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
#ifdef DEBUG
    test_free_struct ();
#endif
    return lp -> node;
  }
  
  else {
#ifdef DEBUG
    memory			p;
    struct ckit_node_list	*ptr;
#else
    register memory			p;
    register struct ckit_node_list	*ptr;
#endif
    
    for (index = 0, ptr = nodes; (index < SIZES) && (ptr -> size < nb);
	 ptr++, index++)	/* null loop statement */ ;	/* find index */
    
    if (ptr -> next_node == NULL) {		/* get more memory */
      /* minimum request is BLOCK_SIZE */
      size = (ptr -> size > BLOCK_SIZE) ? ptr -> size : BLOCK_SIZE;
      
      if ((ptr -> next_node = p = (memory) malloc1( size )) == NULL) {
#ifdef DEBUG
	test_free_struct ();
#endif
	return NULL;}
      
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
    
#ifdef DEBUG
    test_free_struct ();
#endif
    return (NODE) (((int) p) + sizeof( header ));
  }
}

/************************************************************************
 *	dup_node - Duplicate a node.  Actually this just increments	*
 *			the node's reference count			*
 ************************************************************************/

int dup_node(register NODE ap)
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

int node_refcount(register NODE ap)
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

int free_node(register NODE ap)
{
#ifdef DEBUG
  memory		mp;
  long_memory	lp;
  struct ckit_node_list	*ptr;
#else
  register long_memory	lp;
  register memory		mp;
  register struct ckit_node_list	*ptr;
#endif
  
  if (ap == NULL) return;		/* ignore this call */
  
#ifdef DEBUG
  test_free_struct ();
#endif
  
  if (is_small( ap )) {		/* small sized node */
    
    mp = (memory) (ap - sizeof(header));	/* point back to header */
    
    BAD_POINTER( "free_node", ap );		/* do additional checking */
    
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
    fprintf( stderr, free_err, "free_node", ap );
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

unsigned node_size(register NODE ap)
{
  register unsigned temp;
  
  if (ap == NULL) return 0;
  
  size_of_node( temp, ap );
  
  return temp;
}

NODE change_node_size(register NODE ap, register unsigned nbytes)
{
  register NODE		res;
  register struct ckit_node_list	*ptr;
  register unsigned int	size;
  
  if (ap == NULL)			return make_node(nbytes);
  
  if (nbytes == 0)			return (NODE) free_node(ap);
  
  size_of_node( size, ap );		/* how big is it? */
  
  if ((res = make_node(nbytes)) != NULL)
    bcopy( ap, res, (nbytes > size) ? size : nbytes );
  
  free_node( ap );
  
  return res;
}

NODE copy_node(register NODE ap)
{
  register NODE		res;
  register unsigned int	onb;
  
  if (ap == NULL) return NULL;
  
  size_of_node( onb, ap );
  
  if ((res = make_node(onb)) != NULL)
    bcopy( ap, res, (int) onb );
  
  return res;
}

#ifdef DEBUG_MAIN

#define	BLOCKS	1500

main() {
  NODE my_nodes[ BLOCKS ];
  int  i;
  
  for (i=0; i<BLOCKS; i++)
    my_nodes[i] = (NODE) make_node( i );
  memory_stats( stdout, "After initial allocation" );
  
  for (i=0; i<BLOCKS; i+=2)
    free_node( my_nodes[i] );
  memory_stats( stdout, "After partial freeing" );
  
  for (i=0; i<BLOCKS; i+=2)
    my_nodes[i] = (NODE) make_node( i );
  memory_stats( stdout, "After second allocation (should be same as first)" );
  
  for (i=0; i<BLOCKS; i++)
    free_node( my_nodes[i] );
  memory_stats( stdout, "After final free (everything free except 16's)" );
}
#endif

#endif /* STANDARD_MALLOC not defined */

#ifdef STANDARD_MALLOC

#include "memoryP.h"

NODE copy_alloc_std(register NODE ap, register unsigned int size)
{
  register NODE		res;

  if (ap == NULL) return NULL;
  
  if ((res = make_node(size)) != NULL)
    bcopy(ap, res, (int)size);
  
  return res;
}

#endif

#include <stdlib.h>

#ifdef STANDARD_MALLOC

/* The reason for having a special memory management is following: */
/* When compiled under MS-DOS (with Borland C++, huge memory model, Borland */
/* and ANSI source) the program crashed badly when using both the memoryPack */
/* and mallocPack routines and actually it crashed also with plain malloc -- */
/* free routines. The reason for the latter was due to accessing of freed */
/* memory by listPack routines. Therefore the solution is to use a special */
/* "free" routine which frees the blocks after letting them grow older */
/* in a short queue. */

static void *old_block = NULL;

#define LARGE_BLOCK_SIZE 512

void 
free_with_aging(void *block)
{
  if (old_block != NULL) {
    free(old_block);
  }
  old_block = block;
}


#include <assert.h>

typedef struct {
  char file_name[132];
  int  line;
  long num_of_allocs;
  long tot_size;
}   memoryP_alloc_rec;

static void
update_alloc_table(size_t size, char *file, int line);

static memoryP_alloc_rec *
find_in_alloc_table(char *file, int line);

NODE
dbg_malloc(size_t size, char *file, int line)
{
  NODE allocd = NULL;

  allocd = malloc(size);
  assert(allocd != NULL);
  update_alloc_table(size, file, line);
  return allocd;
}


static memoryP_alloc_rec *alloc_table = NULL;
static int alloc_table_size = 0;
static int alloc_table_curr = 0;
#define ALLOC_TABLE_CHUNK 1024

static void
update_alloc_table(size_t size, char *file, int line)
{
  memoryP_alloc_rec *arp;
  
  if (alloc_table == NULL) {
    alloc_table = (memoryP_alloc_rec *)calloc(ALLOC_TABLE_CHUNK,
                                              sizeof(memoryP_alloc_rec));
    alloc_table_size = ALLOC_TABLE_CHUNK;
    alloc_table_curr = -1;
  } else if (alloc_table_curr+1 == alloc_table_size) {
    alloc_table
      = (memoryP_alloc_rec *)realloc(alloc_table,
                                     alloc_table_size+ALLOC_TABLE_CHUNK
                                     * sizeof(memoryP_alloc_rec));
    alloc_table_size += ALLOC_TABLE_CHUNK;
  }
  arp = find_in_alloc_table(file, line);
  if (arp == NULL) {
    alloc_table_curr++;
    strcpy(alloc_table[alloc_table_curr].file_name, file);
    alloc_table[alloc_table_curr].line          = line;
    alloc_table[alloc_table_curr].num_of_allocs = 1;
    alloc_table[alloc_table_curr].tot_size      = size;
  } else {
    arp->num_of_allocs++;
    arp->tot_size += size;
  }
}


static memoryP_alloc_rec *
find_in_alloc_table(char *file, int line)
{
  int i;

  for (i = 0; i <= alloc_table_curr; i++) {
    if (   (strcmp(alloc_table[i].file_name, file) == 0)
        && (alloc_table[i].line == line))
      return &alloc_table[i];
  }
  return NULL;
}


int 
memory_stats(FILE *fp, char *s)
{
  int i;
  memoryP_alloc_rec *arp;

  fprintf(fp, "%s\n", s);
  for (i = 0; i <= alloc_table_curr; i++) {
    arp = &alloc_table[i];
    fprintf(fp, "File %s, line %d, num_of_allocs %ld, tot_size %ld\n",
            arp->file_name, arp->line, arp->num_of_allocs, arp->tot_size);
  }
}

#endif
