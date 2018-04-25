/*
 *	Module Name:		hashPack
 *
 *	Purpose:		Toolbox module for hash tables
 *
 *	Contents:
 *			1)	Typedefs for hashPack structures
 *			2)	inclusion of listPack functions
 *
 *	History:
 *	22Apr86	raz	Created
 *	26Jun86 raz	Made some of the macros agree with the manual
 *			Added a little efficiency
 */

#ifndef HASHP_H
#define HASHP_H

#include "ckitcfg.h"		

#include "listP.h"		/* use listPack for internal structures */

typedef	struct {
    int		size,		/* number of sub-lists */
		number;		/* current count of elements */
    PFI		hashfunc,	/* convert name to hash-index */
		matchfunc;	/* match name to hashed node */
    LIST	lists[1];	/* dummy array */
    } HASH_struct, *HASH;

#define	NULLhash	((HASH) 0)

#define	HASH_NOTUNIQUE	0
#define	HASH_UNIQUE	1

/*
 *	Macros and externals
 */

#define	make_hashtable( size, hashfunc, matchfunc )	\
	_make_hashtable( (int) (size), (PFI) (hashfunc), (PFI) (matchfunc) )

#define free_hashtable( table, flag )				\
	_free_hashtable( (HASH) (table), (int) (flag) )

#define add_to_hashtable( table, node, ident )	\
	_add_to_hashtable( (HASH) (table), (NODE) (node), (NODE) (ident), HASH_NOTUNIQUE )

#define add_unique_to_hashtable( table, node, ident )	\
	_add_to_hashtable( (HASH) (table), (NODE) (node), (NODE) (ident), HASH_UNIQUE )

#define get_from_hashtable( table, ident )		\
	_get_from_hashtable( (HASH) (table),  (NODE) (ident) )

#define delete_from_hashtable( table, ident )		\
	_delete_from_hashtable( (HASH) (table),  (NODE) (ident) )

#define for_all_hashtable( table, node, func )		\
	_for_all_hashtable( (HASH) (table), (NODE) (node), (PFI) (func) )

#define for_all_2hashtable( table, node, node2, func )	\
	_for_all_2hashtable( (HASH) (table), (NODE) (node), (NODE) (node2), (PFI) (func) )

#define for_all_3hashtable( table, node, node2, node3, func )	\
	_for_all_3hashtable( (HASH) (table), (NODE) (node), (NODE) (node2), (NODE) (node3), (PFI) (func) )

#define hashtable_empty( table )	(size_of_hashtable( table ) == 0)

#define size_of_hashtable( table )	(((HASH) (table)) -> number )

/* STATIC	int check_hash_integrity(HASH table, char *routine); */
HASH	_make_hashtable(int size, PFI hashfunc, PFI matchfunc);
void	_free_hashtable(HASH table, int flag);
NODE	_add_to_hashtable(HASH table, NODE node, NODE ident, int flag);
NODE	_get_from_hashtable(HASH table, NODE ident);
NODE	_delete_from_hashtable(HASH table, NODE ident);
int	_for_all_hashtable(HASH table, NODE node, PFI func);
int	_for_all_2hashtable(HASH table, NODE node, NODE node2, PFI func);
int	_for_all_3hashtable(HASH table,
			    NODE node, NODE node2, NODE node3, PFI func);

#endif /* HASHP_H */
