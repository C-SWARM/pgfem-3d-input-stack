/*
 *	Module Name:		treePack.h
 *
 *	Purpose:		Headers for toolbox module for binary Trees
 *
 *	Contents:
 *
 *	History:
 *	22Apr86	raz	Created
 *	26Jun86 raz	Added copy_bintree, size_of_bintree
 *	10Jul87	raz	Reversed parameters for add_to_bintree to make it
 *			consistant with the documentation
 */

#include "ckitcfg.h"

#ifndef	NULLtree			/* test for previous inclusion */

#include "memoryP.h"

typedef struct _binTreeStruct {
	    struct _binTreeStruct	*left, *right;
	    NODE			node;
	    int				value;
	    } TREE_struct, *TREE;

#define	NULLtree	((TREE) 0)

#define	TREE_NOTUNIQUE	0
#define	TREE_UNIQUE	1

#ifndef	AND_NODES
#define	AND_NODES	1
#define	NOT_NODES	0
typedef int (*PFI)();			/* pointer to func returning int */
#endif

#define	TREE_NODE( tree )	(((TREE) (tree)) -> node)
#define	TREE_LEFT( tree )	(((TREE) (tree)) -> left)
#define	TREE_RIGHT( tree )	(((TREE) (tree)) -> right)
#define TREE_LENGTH( tree )	(((TREE) (tree)) -> value)
#define	TREE_ROOT( tree )	TREE_LEFT( tree )
#define	TREE_FUNC( tree )	((PFI) TREE_NODE( tree ))

#define size_of_bintree( tree )	TREE_LENGTH( tree )
#define bintree_empty( tree )	(TREE_LENGTH( tree ) == 0)

/*
 *	Macros
 */

#define	copy_bintree( tree, flag )			\
    _copy_bintree( (TREE) (tree), (int) (flag) )

#define	make_bintree( func )				\
    _make_bintree( (PFI) (func) )

#define free_bintree( tree, flag )			\
    _free_bintree( (TREE) (tree), (int) (flag) )

#define add_to_bintree( tree, ident, node )		\
    _add_to_bintree((TREE) (tree), (NODE) (node), (NODE) (ident), TREE_NOTUNIQUE)

#define add_unique_to_bintree( tree, ident, node )	\
    _add_to_bintree( (TREE) (tree), (NODE) (node), (NODE) (ident), TREE_UNIQUE )

#define	get_from_bintree( tree, ident )	\
	_get_from_bintree( (TREE) (tree), &(TREE_ROOT(tree)), (NODE) (ident))

#define	delete_from_bintree( tree, ident )	\
	_delete_from_bintree( (TREE) (tree), &(TREE_ROOT(tree)), (NODE) (ident))

#define for_all_bintree( tree, node, func )	\
	_for_all_bintree( TREE_ROOT(tree), (NODE) (node), (PFI) (func) )

#define for_all_2bintree( tree, node, node2, func )	\
	_for_all_2bintree( TREE_ROOT(tree), (NODE) (node), (NODE) (node2), (PFI) (func) )

#define for_all_3bintree( tree, node, node2, node3, func )	\
	_for_all_3bintree( TREE_ROOT(tree), (NODE) (node), (NODE) (node2), (NODE) (node3), (PFI) (func) )


/*
 *	Externs
 */

TREE	_make_bintree(NODE treefunc);
TREE	_copy_bintree(TREE tree, int flag);
STATIC	int copy_subtree(TREE *root, int flag);
STATIC	int _flush_bintree(TREE tree, int flag);
void	_free_bintree(TREE tree, int flag);
TREE	*_find_bintree_point(TREE tree, TREE *root, NODE ident, int flag);
NODE	_add_to_bintree(TREE tree, NODE node, NODE ident, int flag);
NODE	_get_from_bintree(TREE tree, TREE *root, NODE ident);
STATIC	int add_deleted_branch(TREE root, TREE branch);
NODE	_delete_from_bintree(TREE tree, TREE *root, NODE ident);
int	_for_all_bintree(TREE tree, NODE node, PFI func);
int	_for_all_2bintree(TREE tree, NODE node, NODE node2, PFI func);
int	_for_all_3bintree(TREE tree,
			  NODE node, NODE node2, NODE node3, PFI func);

#endif
