/*************************************************************************

	C Programmers' Toolbox
	Copyright (c) 1989 by Carnegie Mellon University, Pittsburgh, PA.

	Module Name:	memoryPack
	Purpose:	Memory management routines

	Author:		Robert A Zimmermann, Carnegie -Mellon University
			Department of Electrical and Computer Engineering

	History:	Notes:

	23 Feb 84	Created module out of routines from the layout
			system lUtils.

	 7 May 84	Added definition for copy_node

	25 Jun 84	Added definition for node_size

	 9 Jul 84	Changed the definition of free_node

	 7 Jan 84	Added definition of change_node_size
	
	15 Aug 85	Changed code module to perform its own allocation.
			This file then becomes function definitions.
	
	26 Jun 86	Changed to match the manual

 ************************************************************************* */

/* $Header: /u/04c/mnt/integ/source/c_toolkit/include/RCS/memoryPack.h,v 1.4 90/01/30 14:34:55 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Serpent Change History
|
$Log:	memoryPack.h,v $
 * Revision 1.4  90/01/30  14:34:55  ejh
 * removed reference to MEMORYPACK
 * 
 * Revision 1.3  89/12/23  21:10:40  ejh
 * no change
 * 
 * Revision 1.2  89/07/13  14:57:48  bmc
 * Added external definition for u22_process_name (defined in mallocPack.c).
 * 
\*--------------------------------------------------------------------------*/

/*
 * Data structures
 */

#ifndef	DID_MEMORY_PACK			/* test for previous inclusion	*/

#define	DID_MEMORY_PACK

#include <stdlib.h>
#include <stdio.h>
#include "membase.h"

#ifdef STANDARD_MALLOC

#include <stdlib.h>
/* #include <string.h> */
#ifdef UNIX
#include <unistd.h>
#endif

NODE copy_alloc_std(register NODE ap, register unsigned int size);

void free_with_aging(void *);

#ifdef DEBUG_MALLOC
NODE
dbg_malloc(size_t size, char *file, int line);

#   define make_node(n)	            dbg_malloc(n, __FILE__, __LINE__)
#   define free_node(p)	            free_with_aging((void *)(p))
#   define change_node_size(p,x)    ((p == NULL) ? malloc(x) : realloc(p, x))
#   ifndef bcopy
#      define bcopy(src, dest, len) memcpy(dest, src, len)
#   endif
#   define copy_node(p, len)        copy_alloc_std((NODE)p, len)

#   define dup_node(p)		    dup_alloc(p)
#   define node_refcount(p)	    alloc_refcount(p)
#else  /* !ifdef DEBUG */
#   define	make_node(n)        malloc(n)
#   define	free_node(p)        free_with_aging((void *)(p))
#   define change_node_size(p,x)    ((p == NULL) ? malloc(x) : realloc(p, x))
#   ifndef bcopy
#      define bcopy(src, dest, len) memcpy(dest, src, len)
#   endif
#   define	copy_node(p, len)   copy_alloc_std((NODE)p, len)

#   define dup_node(p)              dup_alloc(p)
#   define node_refcount(p)         alloc_refcount(p)
#endif /* !ifdef DEBUG */


#else  /* STANDARD_MALLOC not defined -- use memoryPack or mallocPack */


#ifndef	memoryPack
#ifndef	make_node
#define	make_node(n)		malloc1(n)
#define	copy_node(p, l)	copy_alloc(p)
#define	free_node(p)		free1((char *) (p))
#define dup_node(p)		dup_alloc(p)
#define node_refcount(p)	alloc_refcount(p)
#define change_node_size(p,x)	((p == NULL) ? malloc1(x) : realloc1(p, x))
#define	memory_stats(fp, str)	mstats(fp, str)
#endif
#else

NODE make_node(register unsigned nbytes);
NODE copy_node(register NODE ap);
NODE change_node_size(register NODE ap, register unsigned nbytes);
int free_node(register NODE ap);
int node_refcount(register NODE ap);
unsigned node_size(register NODE ap);

#endif /* memoryPack */

int mstats(FILE *fp, char *s);
NODE malloc1(unsigned nbytes) ;
int dup_alloc(register NODE ap);
int alloc_refcount(register NODE ap);
int free1(register NODE ap);
unsigned alloc_size(register NODE ap);
NODE realloc1(register NODE ap, register unsigned nbytes);
NODE copy_alloc(register NODE	ap);
#ifdef AEGIS
NODE calloc(register unsigned number, register unsigned nbytes);
#endif
int memory_stats(FILE *fp, char *s);
void test_free_struct();
int dup_node(register NODE ap);

extern	char	*u22_process_name; /* identifies current process */

#endif /* STANDARD_MALLOC */

#endif
