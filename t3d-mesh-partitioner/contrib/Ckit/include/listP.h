/* ***************************************************************************

	C Programmer's Toolbox

	Module Name:	listPack

	Purpose:	Provides the data structures necessary to
			support a DECK, and the functions needed
			to give full list access abilities.

	Author:		Robert A Zimmermann, Carnegie-Mellon University
			Department of Electrical and Computer Engineering
	
	History:
	27 Feb 84	Created module

	...

	09 Dec 85	Rewrittem
	13 Mar 86	(hauth) typecast the node into a NODE in
			add_to_head_val and add_to_tail_val macros to save
			on lint errors.
	26 Jun 86	(raz) Fixed multiple defintion problems
			Added functions for new manual
	08 Dec 86	(raz) Changed loop_through_list
			Added loop_through_list_func() macro
	15 Dec 86	(raz) Changed loop_through_list again to
			simplify expansion
	04 May 87	(raz) Fixed insert_list_node() macro

************************************************************************ */

#ifndef LISTP_H
#define LISTP_H

#include "ckitcfg.h"			

#ifndef	AND_NODES
#define AND_NODES		1	/* flags for free_list, */
#define NOT_NODES		0	/* copy_list, and flush_list */
typedef int (*PFI)();			/* pointer to func returning int */
#endif

/************************************************************************
 *		Type Definitions and exported constants			*
 ************************************************************************/

#include "memoryP.h"			/* Memory management routines */

#define	NULLlist	((LIST) 0)	/* Typecast NULL pointer */

#define LIST_DEFAULT_VALUE	0	/* default element tag value */

typedef struct listNode {
	struct listNode	*next, *prev;	/* or list head and tail respectively */
	NODE		node;		/* the pointer to the data */
	int		value;		/* for sorting and insertion */
	} LISTNODE, *LIST, *STACK, *QUEUE;

/* Convenience macro:  Use in declarations so as to be able to keep */
/* track of the type of elements stored in a list. */
#define LIST_OF(elem_type)  LIST



/************************************************************************
 *	Define LIST functions that can be done as macros		*
 *----------------------------------------------------------------------*
 * These macros will allow the compiler to in-line expand some of the	*
 * simpler listPack functions.  That will reduce code size and		*
 * execution time.  It will also prohibit run-time checking of list	*
 * parameters.								*
 ************************************************************************/

#define	LISThead( list )	(((LIST) (list)) -> next)
#define	LISTtail( list )	(((LIST) (list)) -> prev)
#define	LISTvalue( list )	(((LIST) (list)) -> value)
#define	LISTnode( list )	(((LIST) (list)) -> node)
#define	LISTcurrent( list )	(((LIST) ((list) -> node)) -> node)

#define	list_length( list )	(int) (LISTvalue( list ))

#define list_empty( list )	(list_length( list ) == 0)

#define get_list_head( list )		\
	    (list_empty( list ) ? NULLnode : LISTnode( LISThead( list ) ) )

#define	get_from_head( list )		\
	    _list_remove_node( LISThead( list ), list )

#define get_head_val( list )	\
    (list_empty( list ) ? LIST_DEFAULT_VALUE : LISTvalue(LISThead(list)))

#define first_in_list( list, node )	\
	    (get_list_head( list ) == ((NODE) node) )

#define get_list_tail( list )		\
	    (list_empty( list ) ? NULLnode : LISTnode( LISTtail( list ) ) )

#define	get_from_tail( list )		\
	    _list_remove_node( LISTtail( list ), list )

#define get_tail_val( list )	\
    (list_empty( list ) ? LIST_DEFAULT_VALUE : LISTvalue(LISTtail(list)))

#define last_in_list( list, node )	\
	    (get_list_tail( list ) == ((NODE) node) )

#define	add_to_head_val( list, node, val )			\
(list_empty( list ) ? 						\
    _list_add_node( list, &LISTtail(list), &LISThead(list),	\
    		    (NODE) node, val )	:			\
    _list_add_node( list, &LISThead(list)->prev,		\
    		    &LISThead(list), (NODE) node, val ))

#define	add_to_head( list, node )	\
	add_to_head_val( list, node, LIST_DEFAULT_VALUE )

#define	add_to_tail_val( list, node, val )			\
(list_empty( list ) ? 						\
    _list_add_node( list, &LISTtail(list), &LISThead(list),	\
    		    (NODE) node, val ) : 			\
    _list_add_node( list, &LISTtail(list),			\
    		    &LISTtail(list)->next, (NODE) node, val ))

#define	add_to_tail( list, node )	\
	add_to_tail_val( list, node, LIST_DEFAULT_VALUE )

/* Daniel Rypl, 1996 */
#define	link_to_head( list, List )			\
(list_empty( list ) ? 						\
    _list_link_list( list, &LISTtail(list), &LISThead(list),	\
    		    (LIST) List )	:			\
    _list_link_list( list, &LISThead(list)->prev,		\
    		    &LISThead(list), (LIST) List ))

/* Daniel Rypl, 1996 */
#define	link_to_tail( list, List )			\
(list_empty( list ) ? 						\
    _list_link_list( list, &LISTtail(list), &LISThead(list),	\
    		    (LIST) List ) : 			\
    _list_link_list( list, &LISTtail(list),			\
    		    &LISTtail(list)->next, (LIST) List ))


/****************************************************************
 *	These macros construct a for() loop for C programs	*
 ****************************************************************/


#define loop_through_list_func( list, ptr, type, recovery_func )	\
	for ( ptr = (type) (list), LISTnode(list) = (NODE) (list);	\
	     (ptr = ((LISTnode(list) != NULLnode &&			\
		      ptr == (type) LISTcurrent(list) &&		\
		      LISThead((list)->node) != NULLlist ) ?		\
     (type) LISTnode(LISTnode(list) = (NODE) LISThead(LISTnode(list)))	\
     : (type) recovery_func( list, ptr ))) != (type) 0; )


#define loop_through_list( list, ptr, type )				\
	for ( ptr = (type) (list), LISTnode(list) = (NODE) (list);	\
	     (ptr = ((LISTnode(list) != NULLnode &&			\
		      ptr == (type) LISTcurrent(list) &&		\
		      LISThead((list)->node) != NULLlist ) ?		\
     (type) LISTnode(LISTnode(list) = (NODE) LISThead(LISTnode(list)))	\
		     : (type) 0 )) != (type) 0; )

/************************************************************************
 *		External Routine Declarations.				*
 *	Macros will redefine the calling sequences of all routines	*
 ************************************************************************/

int check_list_integrity(LIST list, char *routine);
NODE	_list_add_node(LIST list, LIST *prev, LIST *next, NODE node, int val);
NODE	_list_remove_node(LIST ptr, LIST list);
NODE	_insert_list_node(LIST list, NODE node, NODE target);
NODE	_insert_list_pos(LIST list, NODE node, int pos);
NODE	_insert_list_val(LIST list, NODE node, int val);
NODE	_get_list_pos(LIST list, int pos);
NODE	_get_list_val(LIST list, int val);
NODE	_delete_list_val(LIST list, int val);
NODE	_delete_list_pos(LIST list, int pos);
NODE	_delete_list_node(LIST list, NODE node);
NODE	_get_from_current(LIST list);
LIST	_make_list(void);
LIST	_flush_list(LIST list, int free_nodes);
void	_free_list(LIST list, int free_nodes);
LIST	_append_list(LIST list1, LIST list2);
LIST	_insert_sublist_pos(LIST list1, LIST list2, int pos);
LIST	_insert_sublist_node(LIST list1, LIST list2, NODE node);
int	_for_all_list(LIST list, NODE node, PFI func);
int	_for_all_2list(LIST list, NODE node, NODE node2, PFI func);
int	_for_all_3list(LIST list, NODE node, NODE node2, NODE node3, PFI func);
NODE	_search_list(LIST list, NODE node, PFI func);
int	_list_equal(LIST list1, LIST list2, PFI func);
LIST	_eval_list(LIST list, PFI func);
int	_get_list_pos_val(LIST list, int pos);
int  	_change_list_val(LIST list, NODE node, int new_val);
LIST	_sort_list_func(LIST list, PFI func);
LIST	_sort_list(LIST list);
LIST	_reverse_list(LIST list);
LIST	_copy_list(LIST list, int flag);
int	_replace_list(LIST list, NODE old, NODE new);
int	_in_list(LIST list, NODE node);
int	_get_node_val(LIST list, NODE node);
int	_get_node_val_current(LIST list);
NODE	_get_list_next(LIST list, NODE current);
NODE	_get_list_current(LIST list);
NODE	_get_list_prev(LIST list, NODE current);
NODE	_get_list_next_func(LIST list, NODE current, NODE node, PFI func);
NODE	__get_list(LIST list, NODE ptr);

/* Daniel Rypl, 1996 */
NODE	_list_link_list(LIST list, LIST *prev, LIST *next, LIST List);
LIST	_list_unlink_list(LIST ptr, LIST list);
LIST	_unlink_current_prev(LIST list);
LIST	_unlink_current_next(LIST list);
NODE	_get_from_current_prev(LIST list);
NODE	_get_from_current_next(LIST list);



/************************************************************************
 *		Macros for automatic type recasting.			*
 ************************************************************************/

#define	make_list()			_make_list()
#define	reverse_list( list )		_reverse_list( (LIST) (list) )
#define	sort_list( list )		_sort_list( (LIST) (list) )

#define	free_list( list, flag )		\
	    _free_list( (LIST) (list), (int) (flag) )
#define	flush_list( list, flag )	\
	    _flush_list( (LIST) (list), (int) (flag) )
#define	get_node_val( list, node )	\
	    _get_node_val((LIST) (list), (NODE)(node))
#define	get_node_val_current( list )	\
	    _get_node_val_current((LIST) (list))
#define	sort_list_func( list, func )	\
	    _sort_list_func((LIST) (list), (PFI) (func))
#define	copy_list( list, flag )	\
	    _copy_list( (LIST) (list), (int) (flag) )
#define	append_list( list1, list2 )	\
	    _append_list((LIST) (list1), (LIST) (list2))
#define	in_list( list, node )	\
	    _in_list( (LIST) (list), (NODE) (node) )
#define	get_list_pos( list, pos )	\
	    _get_list_pos( (LIST) (list), (int) (pos) )
#define	get_list_val( list, val )	\
	    _get_list_val( (LIST) (list), (int) (val) )
#define	delete_list_pos( list, pos )	\
	    _delete_list_pos((LIST) (list), (int) (pos))
#define	delete_list_val( list, val )	\
	    _delete_list_val((LIST) (list), (int) (val))
#define get_list_prev( list, curr )	\
	    _get_list_prev((LIST) (list), (NODE) (curr))
#define get_list_next( list, curr )	\
	    _get_list_next((LIST) (list), (NODE) (curr))
#define get_list_next_func( list, curr, node, func ) \
	    _get_list_next_func((LIST)(list),(NODE)(curr),(NODE)(node),(PFI)(func))
#define get_list_current( list )	\
	    _get_list_current((LIST) (list))
#define	delete_list_node( list, node ) \
	    _delete_list_node((LIST) (list), (NODE) (node))
#define	get_from_current( list ) \
	    _get_from_current((LIST) (list))
#define	insert_list_pos( list, node, pos) \
	    _insert_list_pos( (LIST) (list), (NODE) (node), (int) (pos) )
#define	get_list_pos_val( list, pos ) \
	    _get_list_pos_val( (LIST) (list), (int) (pos) )
#define	change_list_val( list, node, val ) \
	    _change_list_val( (LIST) (list), (NODE) (node), (int) (val) )
#define	insert_list_val( list, node, val ) \
	    _insert_list_val( (LIST) (list), (NODE) (node), (int) (val) )
#define	insert_list_node( list, node, target ) \
	    _insert_list_node( (LIST) list, (NODE) (node), (NODE) (target) )
#define insert_sublist_node( list1, list2, node )	\
	    _insert_sublist_node((LIST) (list1), (LIST) (list2), (NODE) (node))
#define insert_sublist_pos( list1, list2, pos )	\
	    _insert_sublist_pos((LIST) (list1), (LIST) (list2), (int) (pos))
#define	replace_list( list, old, new ) \
	    _replace_list( (LIST) (list), (NODE) (old), (NODE) (new) )
#define	list_equal( list1, list2, func )	\
	    _list_equal( (LIST) (list1), (LIST) (list2), (PFI) (func) )
#define	eval_list( list, func )	\
	    _eval_list( (LIST) (list), (PFI) (func) )
#define	search_list( list, node, func )	\
	    _search_list( (LIST) (list), (NODE) (node), (PFI) (func) )
#define	for_all_list( list, node, func )	\
	    _for_all_list( (LIST) (list), (NODE) (node), (PFI) (func) )
#define	for_all_2list( list, node, node2, func  )	\
	    _for_all_2list((LIST)(list),(NODE)(node),(NODE)(node2),(PFI)(func))
#define	for_all_3list( list, node, node2, node3, func  )	\
	    _for_all_3list((LIST)(list),(NODE)(node),(NODE)(node2),(NODE)(node3),(PFI)(func))

/* Daniel Rypl, 1996 */
#define	unlink_current_prev( list ) \
	    _unlink_current_prev((LIST) (list))
#define	unlink_current_next( list ) \
	    _unlink_current_next((LIST) (list))
#define	get_from_current_prev( list ) \
	    _get_from_current_prev((LIST) (list))
#define	get_from_current_next( list ) \
	    _get_from_current_next((LIST) (list))


/*
 * STACK routines
 */

#define make_stack()			(STACK) make_list()

#define free_stack( stack, flag )	free_list( stack, flag )

#define push_stack( stack, node )	add_to_tail( stack, node )

#define pop_stack( stack )		get_from_tail( stack )

#define top_of_stack( stack )		get_list_tail( stack )

#define height_of_stack( stack )	list_length( stack )

#define stack_empty( stack )		(height_of_stack( stack ) == 0)

/*
 * QUEUE routines
 */

#define make_queue()			(QUEUE) make_list()

#define free_queue( queue, flag )	free_list( queue, flag )

#define enqueue( queue, node )		add_to_tail( queue, node )

#define dequeue( queue )		get_from_head( queue )

#define head_of_queue( queue )		get_list_head( queue )

#define length_of_queue( queue )	list_length( queue )

#define queue_empty( queue )		(length_of_queue( queue ) == 0)

#endif /* LISTP_H */
