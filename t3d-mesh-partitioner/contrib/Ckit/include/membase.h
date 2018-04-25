#ifndef DID_MEMBASE
#define DID_MEMBASE

#include "ckitcfg.h"

#ifndef NULL			/* pointer to nowhere */
#define	NULL (void *)0
#endif

/************************************************************************
 *		Define the parameters for memory allocation		*
 *----------------------------------------------------------------------*
 *	SIZES:	The number of valid memory sizes to be allocated.	*
 *		Each size will require an entry in the 'nodes' table,	*
 *		and they must be put in the table in ascending order.	*
 *----------------------------------------------------------------------*
 *	BLOCK_SIZE:	The size of the pages which we will request	*
 *		from malloc1().  Each page, when received, will be	*
 *		subdivided into equal sized chunks of the required size.*
 *		The first will be returned to the user, the rest saved	*
 *		for future use.						*
 *		This must be a multiple of 1k (1024, 2048, etc.)	*
 *----------------------------------------------------------------------*
 *	BIGGEST_BLOCK:	The largest node that make_node() may get by	*
 *		subdividing a BLOCK_SIZE byte page.  Any request for	*
 *		nodes larger than this will be dealt with in multiples	*
 *		of BLOCK_SIZE chunks, so it should generally be the	*
 *		same as BLOCK_SIZE.					*
 ************************************************************************/

#define	SIZES		9			/* number of block lists */
#define	LAST_LIST	(SIZES-1)		/* this is the last small one */
#define	BIG_LIST	LAST_LIST		/* this is the over-sized one */

#define	BLOCK_SIZE	0x400			/* minimum for system request */
#define BIGGEST_BLOCK	BLOCK_SIZE		/* maximum 'small' block */
#define	BLOCK_BITS	(BLOCK_SIZE - 1)	/* low bits below 1k */


#define	is_small( ptr )	((((int) ptr) & BLOCK_BITS) != 0)


#define	MAX_REFCOUNT	255	/* beyond this, the block is un-freeable */


/************************************************************************
 *		Define the data structures and types			*
 ************************************************************************/

typedef char *NODE;		/* rather than importing the header */


typedef	struct {
  unsigned short	which_size;	/* which free-list is it from? */
  unsigned char	         our_block,	/* to tag blocks we created */
                          refcount;	/* advanced memory support */
} header;


typedef union memory_struct {
  header			head; /* our information bytes */
  union	memory_struct	  *next_node; /* the free-list pointer */
} *memory;


typedef struct long_memory_struct {
  header			    head; /* our information bytes */
  NODE			            node; /* where the block is */
  struct long_memory_struct   *next_node; /* next long block */
} *long_memory;

/************************************************************************
 *	This is the array which will hold the list of free nodes	*
 ************************************************************************/

typedef struct ckit_node_list {
  memory		next_node;	/* free-list pointer */
  unsigned int	num_in_use,	/* total nuber available */
  num_free,	/* number currently available */
  bytes_wasted,	/* total diff btw asked and given */
  nmalloc,	/* times malloc was used on this size */
  size;		/* sizeof(header) is included */
}              ckit_node_list;

#ifdef PRIVATE_MEMORY_NODES


STATIC ckit_node_list nodes[ SIZES ] = {
  { NULL, 0, 0, 0, 0, 16 },	/* these are big node links */
  { NULL, 0, 0, 0, 0, 20 },	/* LISTnode structures */
  { NULL, 0, 0, 0, 0, 32 },
  { NULL, 0, 0, 0, 0, 64 },
  { NULL, 0, 0, 0, 0, 128 },
  { NULL, 0, 0, 0, 0, 256 },
  { NULL, 0, 0, 0, 0, 512 },
  { NULL, 0, 0, 0, 0, BIGGEST_BLOCK },
  { NULL, 0, 0, 0, 0, 0 },	/* for big nodes */
};

#else

extern struct ckit_node_list nodes[SIZES];

#endif

#define long_nodes	((long_memory) nodes[ BIG_LIST ].next_node )

#define NULLnode	((NODE) 0)	/* Typecast NULL pointer */


#ifdef BELL
#define	bcopy(from, to, nbytes)	 memcpy(to, from, nbytes)
#endif

#ifndef UNIX
#define	bcopy(from, to, nbytes)	 memcpy(to, from, nbytes)
#endif

#ifdef STANDARD_MALLOC
#define	bcopy(from, to, nbytes)	 memcpy(to, from, nbytes)
#endif

#endif
