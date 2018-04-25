/********************************************************************/
/*                                                                  */
/*                     NODE RENUMBERING UTILITY                     */
/*                                                                  */
/*                        Sloan's algorithm                         */
/*                                                                  */
/*                 Programmed by Daniel Rypl, 1998                  */
/*                                                                  */
/********************************************************************/


/*
This algorithm implementation requires that the connectivity array 
corresponds to an undirected simply connected graph !!!
(a node cannot be connected to another node by more than one edge,
if node1 is connected to node2 then node2 must be also connected 
to node1, node cannot be connected with itself).
Nodes are expected to be numbered continuously.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* #define MAIN */

/* #define DEBUG */

#ifndef MAIN
/* #define CHECK_CONNECTIVITY */
#endif


#define RENUMBERING_OK                     0
#define MEMORY_ALLOCATION_ERROR          -10
#define UNEXPECTED_SITUATION             -20
#define INVALID_CONNECTIVITY             -30


#define alloc(PNTR, TYPE)                PNTR = new_(TYPE)
#define alloc_zero(PNTR, TYPE)           PNTR = new_zero(TYPE)
#define alloc_array(PNTR, NEL, TYPE)     PNTR = new_array(NEL, TYPE)

#define new_(TYPE)               (TYPE *)malloc((size_t)sizeof(TYPE))
#define new_zero(TYPE)           (TYPE *)calloc(1, (size_t)sizeof(TYPE))
#define new_array(NEL, TYPE)     (TYPE *)calloc((NEL), (size_t)sizeof(TYPE))

#define swap(ARG1, ARG2, TMP)   {(TMP) = (ARG1); (ARG1) = (ARG2); (ARG2) = (TMP);}

#define LN2 0.693147180559945309417232121458176568


typedef struct node_rec node_rec;
typedef struct list list;

struct node_rec{
	long id;                      /* in: old id   out: new id */
	long pos;                     /*              out: position of new id equal to old id */
	long contact;
	long *connection;
};

/* eg:   old_id (in):  1  2  3  4  5  6
         new_id (out): 4  6  1  2  3  5
         pos (out):    2  3  4  0  5  1 */


/* old ids should start from 1;
	 they are used to calculate profile */


struct list{
	long item;
	list *next;
};


typedef enum logic{
	NO = 0,
	YES = 1
}logic;



static double
renumber_mesh_nodes(long root_nd);

static void
get_peripheral_nodes(long root_nd, long *start_nd, long *end_nd);
static logic
build_rooted_level_structure(long root_nd, long *depth, long *width);
static void
initiate_rooted_level_structure(long root_nd);
static void
renumber_nodes(long root_nd);


static long
get_initial_node(void);
static long
get_next_initial_node(void);


static void
init_node(long nd);
static void
check_node(long nd);


static void
sort_degree(long n, int *key, long *item);


static double
calculate_profile(void);

static void
update_profile_node(long nd);

static void
update_connection(node_rec *node, long nd);


static list *
add_item_to_head(list **head, long item);
static list *
add_item_to_tail(list **head, long item);
static list *
add_list_to_head(list **head, list *lst);
static list *
add_list_to_tail(list **head, list *lst);
static list *
get_head_list(list *lst);
static list *
get_tail_list(list *lst);
static long
get_list_size(list *lst);
static list *
destroy_list(list *lst);


#ifdef CHECK_CONNECTIVITY
static void
check_connectivity(long num_nodes, node_rec *node_array);
#endif


static list *prev_level_front = NULL, *next_level_front = NULL;
static long max_depth, min_width, last_level_width, nds, nds_pos;
static long graph_nodes, labeled_graph_nodes, isolated_nodes;
static int min_degree, max_degree, degree_weight, level_weight, error_status;
static long *prior_array = NULL;
static int *status_array = NULL;
static double profile;
static node_rec *node_recs = NULL;
static long init_nd, nodes, shift;
static logic simple_domain, first_run;


#define SETTING                 5

static int weight_setting[SETTING][2] = {
	{1, 0}, {1, 1}, {2, 1},  {3, 1}, {4, 1}                   /* {degree, level} */
};


#define MAX_BACK_RUN            50
#define MIN_BACK_RUN            10


#define LAST_TO_SELECTED        1       /* priority queue is not maintained in order of creation */
#define SELECTED_TO_LAST        2       /* priority queue is maintained in order of creation */

#define QUEUE_CONCEPT           LAST_TO_SELECTED

/* there is no significant difference in the number of cases providing the minimum profile
	 between the two above concepts;
	 the LAST_TO_SELECTED concept, however, seems to be better in terms of avoiding 
	 catastrophical scenarios */

/* #define SEARCH_SPEEDUP */

/* the speedup is based on skipping "half" of priority queue searches;
	 during the search of the priority queue the first and second largest priority 
	 are detected simultaneously (if possible);
	 during the processing of node corresponding to the first priority the second priority
	 is maintained updated to be still the second largest;
	 during the next loop the search of the priority queue is omitted and the node corresponding
	 to the second largest priority is used as selected node;
	 note that this approach does NOT respect QUEUE_CONCEPT (in the phase of updating 
	 the second priority during processing of selected node corresponding to the first priority);
	 the second largest priority cannot be detected if the first largest priority is the
	 first item in the queue

	 generally the worst the renumbering the more time it required (the time is directly
	 dependent on the length of priority queue (larger for shallow graphs));
	 using SEARCH_SPEEDUP does not necessarily speed up the renumbering as it may result
	 in worse renumbering */

/* #define CHANGE_TO_NEGATIVE */

/* it seems that CHANGE_TO_NEGATIVE directive does not lead any speedup, moreover its use
	 would cause failure if a node is connected with itself */


typedef enum{
	INACTIVE_STATUS = 0,
	PREACTIVE_STATUS = 1,
	ACTIVE_STATUS = 2,
	POSTACTIVE_STATUS = 3
}status;


int
renumber_mesh(long num_nodes, node_rec *node_array, long num_shift, double *old_profile, double *new_profile);

#ifdef MAIN

#include <stdio.h>

int
main(int argc, char **argv)
{
	FILE *file = NULL;
	long num_nodes, i, j, k, nd_id, contact, *connection = NULL, con, *connect = NULL, count;
	node_rec *node_array = NULL, *nd_array = NULL;
	double old_profile, new_profile;
	logic error = NO;

	if(argc != 2){
		fprintf(stderr, "\n");
		fprintf(stderr, "Usage: renumber ren_file\n\n");
		fprintf(stderr, "ren_file: nodes\n");
		fprintf(stderr, "          node_id contacts [node_ids]\n\n");
		exit(1);
	}
	if((file = fopen(argv[1], "r")) == NULL){
		fprintf(stderr, "File %s not found\n", argv[1]);
		exit(1);
	}
	
	if(fscanf(file, "%ld", &num_nodes) != 1){
		fprintf(stderr, "Invalid record [nodes]\n");
		exit(1);
	}
	if(num_nodes == 0)exit(1);

	node_array = (node_rec *)calloc(num_nodes, sizeof(node_rec));
	if(node_array == NULL)exit(1);

	for(i = 0; i < num_nodes; i++){
		if(fscanf(file, "%ld %ld", &nd_id, &contact) != 2){
			fprintf(stderr, "Invalid %ld-th node record [nd_id contact]\n", i + 1);
			exit(1);
		}
			
		if(nd_id <= 0 || nd_id > num_nodes)exit(1);

		if(nd_id != i + 1){
			fprintf(stderr, "Invalid node id (node %ld read, node %ld expected)\n", nd_id, i + 1);
			exit(1);
		}

		nd_array = &(node_array[nd_id - 1]);
		nd_array -> id = nd_id;
		nd_array -> pos = 0;
		nd_array -> contact = contact;
		if(contact != 0){
			nd_array -> connection = connection = (long *)calloc(contact, sizeof(long));
			if(connection == NULL)exit(1);

			for(j = 0; j < contact; j++){
				if(fscanf(file, "%ld", &(connection[j])) != 1){
					fprintf(stderr, "Invalid %ld-th item of %ld-th node record [node_ids]\n", j + 1, i + 1);
					exit(1);
				}
			}
		}
	}

	for(i = 0; i < num_nodes; i++){
		contact = node_array[i].contact;
		connection = node_array[i].connection;
		for(j = 0; j < contact; j++){
			nd_id = connection[j];
			if(nd_id == i + 1){
				fprintf(stderr, "Invalid connectivity (node %ld connected with itself)\n", nd_id);
				error = YES;
			}
			con = node_array[nd_id - 1].contact;
			connect = node_array[nd_id - 1].connection;
			count = 0;
			for(k = 0; k < con; k++){
				if(connect[k] == i + 1)count++;
			}
			if(count != 1){
				fprintf(stderr, "Invalid connectivity (node %ld connected to node %ld %ld times)\n",
								i + 1, nd_id, count);
				error = YES;
			}
		}
	}

	if(error == YES)exit(1);
	fprintf(stdout, "Renfile successfully checked\n\n");

	if(renumber_mesh(num_nodes, node_array, 0, &old_profile, &new_profile) != RENUMBERING_OK){
		fprintf(stderr, "Renumbering error\n");
		exit(1);
	}

#ifdef DEBUG
	fprintf(stdout, "\n    old           new          pos (of old in new)\n");
	fprintf(stdout, "---------------------------------------------------\n");
	nd_array = &(node_array[0]);
	for(i = 0; i < num_nodes; i++, nd_array++){
		fprintf(stdout, "%6ld    --> %6ld     : %6ld\n", i + 1, nd_array -> id, nd_array -> pos + 1);
	}
	fprintf(stdout, "\n");
#endif

	fprintf(stdout, "Old profile: %.0f\n", old_profile);
	fprintf(stdout, "New profile: %.0f\n\n", new_profile);

	return(0);
}
	
#endif


int
renumber_mesh(long num_nodes, node_rec *node_array, long num_shift, double *old_profile, double *new_profile)
{
	logic best_last = YES;
	double best_profile, last_profile;
	int setting, best_degree, best_level, max_deg, degree_incr, over_iter;
	long root_nd = 0L;

#ifdef CHECK_CONNECTIVITY
	check_connectivity(num_nodes, node_array);
#endif

	nds = shift = num_shift;
	nds_pos = 0;

	node_recs = node_array;
	nodes = num_nodes;

	prior_array = (long *)calloc(nodes, sizeof(long));
	if(prior_array == NULL){
		error_status = MEMORY_ALLOCATION_ERROR;
		return(error_status);
	}
	status_array = (int *)calloc(nodes, sizeof(int));
	if(status_array == NULL){
		free(prior_array);
		error_status = MEMORY_ALLOCATION_ERROR;
		return(error_status);
	}

	max_depth = 0L;
	min_width = 0L;
	graph_nodes = 0L;
	labeled_graph_nodes = 0L;
	isolated_nodes = 0L;
	error_status = RENUMBERING_OK;
	simple_domain = YES;
	first_run = YES;

	*old_profile = calculate_profile();

	root_nd = get_initial_node();
	max_deg = max_degree;

#ifdef DEBUG
	fprintf(stderr, "Root node: %ld\n", root_nd);
#endif

	if(root_nd == 0){

/* mesh consists of isolated vertices only */

		*new_profile = *old_profile;
	}
	else{
		best_profile = (double)nodes * (double)nodes;
		setting = 0;
		while(YES){

			degree_weight = weight_setting[setting][0];
			level_weight = weight_setting[setting][1];

#ifdef DEBUG
/*			fprintf(stderr, "Setting %d (%d %d)\n", setting + 1, degree_weight, level_weight); */
#endif

/* enlarge the max degree to prevent priority of any node to be zero */
			
			max_degree = (level_weight == 0) ? max_deg + 1 : max_deg;
			*new_profile = renumber_mesh_nodes(root_nd);
			if(error_status != 0){
				free(prior_array);
				free(status_array);
				prev_level_front = destroy_list(prev_level_front);
				next_level_front = destroy_list(next_level_front);
				return(error_status);
			}
			
			best_last = NO;
			if(best_profile > *new_profile){
				best_profile = *new_profile;
				best_degree = degree_weight;
				best_level = level_weight;
				best_last = YES;
			}
			
			if(++setting == SETTING){
				last_profile = *new_profile;
				break;
			}
		}

		over_iter = 0;
		degree_incr = 1;
		while(YES){
			over_iter++;
			degree_weight += degree_incr;

/* enlarge the max degree to prevent priority of any node to be zero */

			max_degree = (level_weight == 0) ? max_deg + 1 : max_deg;
			*new_profile = renumber_mesh_nodes(root_nd);
			if(error_status != 0){
				free(prior_array);
				free(status_array);
				prev_level_front = destroy_list(prev_level_front);
				next_level_front = destroy_list(next_level_front);
				return(error_status);
			}
			
			best_last = NO;
			if(best_profile > *new_profile){
				best_profile = *new_profile;
				best_degree = degree_weight;
				best_level = level_weight;
				best_last = YES;
			}

			if(*new_profile >= last_profile - 1.0)break;
			last_profile = *new_profile;

			if(over_iter / degree_incr == 2)degree_incr *= 2;
		}
		
		if(best_last == NO){
			degree_weight = best_degree;
			level_weight = best_level;

/* enlarge the max degree to prevent priority of any node to be zero */
			
			max_degree = (level_weight == 0) ? max_deg + 1 : max_deg;
			*new_profile = renumber_mesh_nodes(root_nd);
			if(error_status != 0){
				free(prior_array);
				free(status_array);
				prev_level_front = destroy_list(prev_level_front);
				next_level_front = destroy_list(next_level_front);
				return(error_status);
			}
		}
	}

	free(prior_array);
	free(status_array);

	prev_level_front = destroy_list(prev_level_front);
	next_level_front = destroy_list(next_level_front);

	return(error_status);
}



static double
renumber_mesh_nodes(long root_nd)
{
	static long start_nd, end_nd;

/* loop over disconnected domains;
	 graph_nodes is the exact number of nodes of the graph (isolated nodes 
	 are not considered) and it is calculated in get_initial_node */

	labeled_graph_nodes = 0L;
	nds = shift + isolated_nodes;
	nds_pos = 0 + isolated_nodes;

	while(YES){
		if(simple_domain == NO || first_run == YES){
			get_peripheral_nodes(root_nd, &start_nd, &end_nd);

#ifdef DEBUG
			fprintf(stderr, "Start node: %ld\n", start_nd);
			fprintf(stderr, "End node: %ld\n", end_nd);
#endif

			if(error_status != 0)return(0.0);
			first_run = NO;
		}

		initiate_rooted_level_structure(end_nd);
		if(error_status != 0)return(0.0);

		renumber_nodes(start_nd);
		if(error_status != 0)return(0.0);

		if(graph_nodes == labeled_graph_nodes)break;
		simple_domain = NO;

		if((root_nd = get_next_initial_node()) == 0){
			error_status = UNEXPECTED_SITUATION;
			return(0.0);
		}
	}

/* calculate the reduced profile and reset the level and status */

	return(calculate_profile());
}
	


static void
get_peripheral_nodes(long root_nd, long *start_nd, long *end_nd)
{
	list *front = NULL;
	logic repeat = YES, built;
	long depth, width, count, shrink_count, nd;
	long *items = NULL, *items_ptr = NULL;
	int *degree = NULL, *degree_ptr = NULL;

	min_width = 0;       /* to ensure that initial rooted level structure is built completely */

	build_rooted_level_structure(*start_nd = root_nd, &depth, &width);
	if(error_status != 0)return;

	while(repeat == YES){
		alloc_array(items, last_level_width, long);                 /* macro */
		if(items == NULL){
			error_status = MEMORY_ALLOCATION_ERROR;
			return;
		}
		alloc_array(degree, last_level_width, int);                 /* macro */
		if(degree == NULL){
			free(items);
			error_status = MEMORY_ALLOCATION_ERROR;
			return;
		}

		front = prev_level_front;
		count = last_level_width;
		shrink_count = (count + 2) / 2;
		if(shrink_count > MAX_BACK_RUN)shrink_count = MAX_BACK_RUN;
		if(shrink_count < MIN_BACK_RUN)shrink_count = (count < MIN_BACK_RUN) ? count : MIN_BACK_RUN;
		while(count--){
			nd = *items++ = front -> item;
			*degree++ = (node_recs + nd - 1) -> contact;
			front = front -> next;
		}
	
		degree_ptr = degree -= last_level_width;
		items_ptr = items -= last_level_width;
		sort_degree(last_level_width, degree, items);

		max_depth = depth;
		min_width = 10 * nodes;

		repeat = NO;
		while(shrink_count--){
			nd = *items++;

			built = build_rooted_level_structure(nd, &depth, &width);
			if(error_status != 0){
				free(items_ptr);
				free(degree_ptr);
				return;
			}
			if(built == NO)continue;
			
			if(depth > max_depth){
				*start_nd = nd;
				repeat = YES;
				break;
			}

			*end_nd = nd;
			min_width = width;
		}

		free(items_ptr);
		free(degree_ptr);
	}
}



static logic
build_rooted_level_structure(long root_nd, long *depth, long *width)
{
	node_rec *root_node = node_recs + root_nd - 1;
	node_rec *front_node = NULL, *node = NULL;
	list *prev_front = NULL, *next_front = NULL, *front = NULL;
	long max_width, prev_width, next_width, level, id;
	long front_nd = 0L, nd = 0L;
	int i, j;

	level = 1L;
	max_width = prev_width = 1L;
/*	prior_array[root_nd - 1] = level; */
	root_node -> id = -(root_node -> id);

	if(prev_level_front == NULL){
		if(add_item_to_head(&prev_level_front, root_nd) == NULL)return(NO);
	}
	else
		prev_level_front -> item = root_nd;

	while(YES){
		level++;
		next_width = 0L;

		prev_front = prev_level_front;
		next_front = next_level_front;

		last_level_width = prev_width;
		while(prev_width--){
			front_nd = prev_front -> item;
			front_node = node_recs + front_nd - 1;
			prev_front = prev_front -> next;

/* note: use of signed ids seems to be the most efficient method in building
	 the rooted level structure;
	 the other alternative is to used test on already assigned level;
	 however this cannot be used in two consecutives buildings without reinitiation
	 of the level back to 0 */

			for(i = 0; i < front_node -> contact; i++){
				if((nd = front_node -> connection[i]) < 0){
					front_node -> connection[i] = -nd;
					continue;
				}
				
#ifdef CHANGE_TO_NEGATIVE
				front_node -> connection[i] = -nd;
#endif

				node = node_recs + nd - 1;
				update_connection(node, front_nd);
				if((id = node -> id) < 0)continue;
				node -> id = -id;
/*				prior_array[nd - 1] = level; */
				
				if(next_front == NULL){
					if(add_item_to_head(&next_level_front, nd) == NULL)return(NO);
				}
				else{
					next_front -> item = nd;
					next_front = next_front -> next;
				}

/* stop criterion if no improvement in width */

				if(++next_width == min_width){

/* restore the positive ids (current node, remaining prev nodes and all next nodes) */

					front_node -> id = -(front_node -> id);
					for(j = 0; j < front_node -> contact; j++){
						nd = front_node -> connection[j];
						front_node -> connection[j] = abs(nd);
						
#ifdef CHANGE_TO_NEGATIVE
						if(nd < 0)update_connection(node_recs + abs(nd) - 1, -abs(front_nd));
#else
						node = node_recs + abs(nd) - 1;
						for(i = 0; i < node -> contact; i++){
							node -> connection[i] = abs(node -> connection[i]);
						}
#endif

					}
					while(prev_width--){
						front_nd = prev_front -> item;
						front_node = node_recs + front_nd - 1;
						prev_front = prev_front -> next;
						
						front_node -> id = -(front_node -> id);
						for(j = 0; j < front_node -> contact; j++){
							nd = front_node -> connection[j];
							front_node -> connection[j] = abs(nd);

#ifdef CHANGE_TO_NEGATIVE
							if(nd < 0)update_connection(node_recs + abs(nd) - 1, -abs(front_nd));
#else
							node = node_recs + abs(nd) - 1;
							for(i = 0; i < node -> contact; i++){
								node -> connection[i] = abs(node -> connection[i]);
							}
#endif
							
						}
					}
					next_front = next_level_front;
					while(next_width--){
						front_nd = next_front -> item;
						front_node = node_recs + front_nd - 1;
						next_front = next_front -> next;
						
						front_node -> id = -(front_node -> id);
						for(j = 0; j < front_node -> contact; j++){
							nd = front_node -> connection[j];
							front_node -> connection[j] = abs(nd);
							
#ifdef CHANGE_TO_NEGATIVE
							if(nd < 0)update_connection(node_recs + abs(nd) - 1, -abs(front_nd));
#else
							node = node_recs + abs(nd) - 1;
							for(i = 0; i < node -> contact; i++){
								node -> connection[i] = abs(node -> connection[i]);
							}
#endif
							
						}
					}
						
					*depth = level;
					*width = min_width;
					return(NO);
				}
			}
			front_node -> id = -(front_node -> id);

#ifdef CHANGE_TO_NEGATIVE
			for(i = 0; i < front_node -> contact; i++){
				front_node -> connection[i] = abs(front_node -> connection[i]);
			}
#endif

		}
	
		if(next_width == 0)break;
		if(next_width > max_width)max_width = next_width;

		prev_width = next_width;
		swap(prev_level_front, next_level_front, front);             /* macro */
	}

	*depth = level - 1;
	*width = max_width;

	return(YES);
}



static void
initiate_rooted_level_structure(long root_nd)
{
	node_rec *root_node = node_recs + root_nd - 1;
	node_rec *front_node = NULL, *node = NULL;
	list *prev_front = NULL, *next_front = NULL, *front = NULL;
	long prev_width, next_width, level, id;
	long front_nd = 0L, nd = 0L;
	int i;

/* current_degree = degree - number of active and postactive neigbours + 1 if node is preactive or inactive;
	 priority = (max_degree - current_degree) * degree_weight + level * level_weight */

	level = 1L;
	prev_width = 1L;
	prior_array[root_nd - 1] = (max_degree - root_node -> contact - 1) * degree_weight + level_weight;
	status_array[root_nd - 1] = INACTIVE_STATUS;
	root_node -> id = -(root_node -> id);

	if(prev_level_front == NULL){
		if(add_item_to_head(&prev_level_front, root_nd) == NULL)return;
	}
	else
		prev_level_front -> item = root_nd;

	while(YES){
		level++;
		next_width = 0L;

		prev_front = prev_level_front;
		next_front = next_level_front;

		while(prev_width--){
			front_nd = prev_front -> item;
			front_node = node_recs + front_nd - 1;
			prev_front = prev_front -> next;

			for(i = 0; i < front_node -> contact; i++){
				if((nd = front_node -> connection[i]) < 0){
					front_node -> connection[i] = -nd;
					continue;
				}
				node = node_recs + nd - 1;
				update_connection(node, front_nd);
				if((id = node -> id) < 0)continue;
				node -> id = -id;
				prior_array[nd - 1] = (max_degree - node -> contact - 1) * degree_weight + level * level_weight;
				status_array[nd - 1] = INACTIVE_STATUS;

				if(next_front == NULL){
					if(add_item_to_head(&next_level_front, nd) == NULL)return;
				}
				else{
					next_front -> item = nd;
					next_front = next_front -> next;
				}
				next_width++;
			}
			front_node -> id = -(front_node -> id);
		}
	
		if(next_width == 0)return;

		prev_width = next_width;
		swap(prev_level_front, next_level_front, front);             /* macro */
	}
}




static void
renumber_nodes(long root_nd)
{
	node_rec *node = NULL, *node_nd = NULL, *selected_node = NULL;
	list *selected_list = NULL, *last_list = NULL;
	list *queue = NULL, *active_queue = NULL, *prev_list = NULL, *next_list = NULL;
	logic search;
	long size = 1L, sz, priority;
	long nd = 0L, nd_nd = 0L, selected_nd = 0L;
	int i, j;

#ifdef SEARCH_SPEEDUP
	node_rec *next_node = NULL;
	long next_priority, next_nd = 0L;
#endif

#if QUEUE_CONCEPT == SELECTED_TO_LAST
	list *lst = NULL;
#endif

/* prev and next fronts are connected to serve as queue of eligible nodes */

	active_queue = add_list_to_head(&active_queue, prev_level_front);
	active_queue = add_list_to_head(&active_queue, next_level_front);

	if(active_queue == NULL)
		add_item_to_head(&active_queue, root_nd);
	else
		active_queue -> item = root_nd;

	status_array[root_nd - 1] = PREACTIVE_STATUS;

	while(size != 0){
		search = YES;

#ifdef SEARCH_SPEEDUP
		if(next_nd != 0){
/*		if(next_node != NULL){ */
			selected_nd = next_nd;
			selected_node = next_node;
			next_nd = 0L;
			next_node = NULL;
			search = NO;
		}
#endif

		if(search == YES){

/* find node with highest priority and remove it from the queue */

			priority = 0L;
			sz = size;
			last_list = NULL;
			selected_nd = 0L;
			selected_node = NULL;
			queue = active_queue;
			while(sz--){
				nd = queue -> item;
				node = node_recs + nd - 1;

#ifdef SEARCH_SPEEDUP

/* move the list with postactive node to the end of the queue */

				if(status_array[nd - 1] == POSTACTIVE_STATUS){
					if(next_list == NULL)next_list = get_tail_list(queue);
					if(last_list == NULL){
						active_queue = queue -> next;
						queue -> next = next_list -> next;
						next_list -> next = queue;
						queue = active_queue;
					}
					else{
						last_list -> next = queue -> next;
						queue -> next = next_list -> next;
						next_list -> next = queue;
						queue = last_list -> next;
					}
					if(--size == 0)break;         /* the postactive node is the only one in the queue */
					continue;
				}
#endif
					
				if(prior_array[nd - 1] > priority){

#ifdef SEARCH_SPEEDUP
					next_priority = priority;
					next_nd = selected_nd;
					next_node = selected_node;
#endif

					priority = prior_array[nd - 1];
					selected_nd = nd;
					selected_node = node;
					selected_list = queue;
					prev_list = last_list;
				}
				last_list = queue;
				queue = queue -> next;
			}
			size--;

#ifdef SEARCH_SPEEDUP
			if(selected_nd == 0)break;
/*			if(selected_node == NULL)break; */
#endif

#if QUEUE_CONCEPT == LAST_TO_SELECTED

/* item from the last list is moved to the selected list */

			selected_list -> item = last_list -> item;
			next_list = last_list;
#endif

#if QUEUE_CONCEPT == SELECTED_TO_LAST

/* selected list is detached and added after last list */

			if(last_list != selected_list){
				if(prev_list == NULL)
					active_queue = selected_list -> next;
				else
					prev_list -> next = selected_list -> next;
				selected_list -> next = last_list -> next;
				last_list -> next = selected_list;
			}
			next_list = selected_list;
#endif

		}

#ifdef DEBUG
/*		fprintf(stderr, "Selected node: %ld\n", selected_nd); */
#endif

		if(status_array[selected_nd - 1] == PREACTIVE_STATUS){
			for(i = 0; i < selected_node -> contact; i++){
				nd = selected_node -> connection[i];
				node = node_recs + nd - 1;

/* current_degree is increased by 1 because the neighbour (selected node) is changing status 
	 from preactive to postactive ==> priority increased by degree_weight */

				prior_array[nd - 1] += degree_weight;

#ifdef SEARCH_SPEEDUP
				if(next_nd != 0){
/*				if(next_node != NULL){ */
					if(next_priority < prior_array[nd - 1]){
						next_priority = prior_array[nd - 1];
						next_node = node;
						next_nd = nd;
					}
				}
#endif

				if(status_array[nd - 1] == INACTIVE_STATUS){
					status_array[nd - 1] = PREACTIVE_STATUS;

					if(next_list == NULL)
						add_item_to_head(&active_queue, nd);
					else{
						next_list -> item = nd;

#if QUEUE_CONCEPT == SELECTED_TO_LAST

/* this ensures that next_list is never equal to NULL (which might result in undesirable adding 
	 the next item to the head of the queue (see above)) */

						if((lst = next_list -> next) == NULL){
							alloc(lst, list);              /* macro */	
							if(lst == NULL){
								error_status = MEMORY_ALLOCATION_ERROR;
								return;
							}
							next_list -> next = list;
						}
#endif

						next_list = next_list -> next;
					}
					size++;
				}
			}
		}

		status_array[selected_nd - 1] = POSTACTIVE_STATUS;

/* note: do not use selected node id in the following statemenet
	 because during second and subsequent runs ids are already renumbered */

		(node_recs + nds_pos++) -> pos = (long)(selected_node - node_recs);
		selected_node -> id = ++nds;

		labeled_graph_nodes++;

		for(i = 0; i < selected_node -> contact; i++){
			nd = selected_node -> connection[i];
			node = node_recs + nd - 1;
			if(status_array[nd - 1] == POSTACTIVE_STATUS)continue;
			if(status_array[nd - 1] == PREACTIVE_STATUS){

/* current_degree is increased by 1 because the node is changing status 
	 from preactive to active ==> priority increased by degree_weight */

				prior_array[nd - 1] += degree_weight;
				status_array[nd - 1] = ACTIVE_STATUS;

#ifdef SEARCH_SPEEDUP
				if(next_nd != 0){
/*				if(next_node != NULL){ */
					if(next_priority < prior_array[nd - 1]){
						next_priority = prior_array[nd - 1];
						next_node = node;
						next_nd = nd;
					}
				}
#endif

				for(j = 0; j < node -> contact; j++){
					nd_nd = node -> connection[j];
					node_nd = node_recs + nd_nd - 1;
					if(status_array[nd_nd - 1] == POSTACTIVE_STATUS)continue;

/* current_degree is increased by 1 because the neigbour (node) changed  status 
	 from preactive to active ==> priority increased by degree_weight */

					prior_array[nd_nd - 1] += degree_weight;

#ifdef SEARCH_SPEEDUP
					if(next_nd != 0){
/*					if(next_node != NULL){ */
						if(next_priority < prior_array[nd_nd - 1]){
							next_priority = prior_array[nd_nd - 1];
							next_node = node_nd;
							next_nd = nd_nd;
						}
					}
#endif
					if(status_array[nd_nd - 1] == INACTIVE_STATUS){
						status_array[nd_nd - 1] = PREACTIVE_STATUS;

						if(next_list == NULL){
							if(add_item_to_head(&active_queue, nd_nd) == NULL)return;
						}
						else{
							next_list -> item = nd_nd;

#if QUEUE_CONCEPT == SELECTED_TO_LAST

/* this ensures that next_list is never equal to NULL (which might result in undesirable adding 
	 the next item to the head of the queue (see above)) */

							if((lst = next_list -> next) == NULL){
								alloc(lst, list);      /* macro */	
								if(lst == NULL){
									error_status = MEMORY_ALLOCATION_ERROR;
									return;
								}
								next_list -> next = list;
							}
#endif

							next_list = next_list -> next;
						}
						size++;
					}
				}
			}
		}
	}

	size = get_list_size(active_queue);
	size /= 2;

	prev_level_front = active_queue;
	while(--size)active_queue = active_queue -> next;
	next_level_front = active_queue -> next;
	active_queue -> next = NULL;
}



static long
get_initial_node(void)
{
	long i;

	init_nd = 0L;
	max_degree = 0;
	min_degree = nodes;

	for(i = 0; i < nodes; i++)init_node(i + 1);

/* enlarge the max degree to prevent priority of any node to be zero */

	max_degree++;
	return(init_nd);
}


static long
get_next_initial_node(void)
{
	long i;

	init_nd = 0L;
	min_degree = nodes;

	for(i = 0; i < nodes; i++)check_node(i + 1);

	return(init_nd);
}


static void
init_node(long nd)
{
	node_rec *node = node_recs + nd - 1;
	int degree;

	prior_array[nd - 1] = 0L;

	if((degree = node -> contact) < min_degree){

/* isolated nodes (vertices) are processed immediately (and only once) */

		if(degree == 0){
			(node_recs + nds_pos++) -> pos = nd - 1;
			node -> id = ++nds;
			isolated_nodes++;
			return;
		}

		init_nd = nd;
		min_degree = degree;
	}
	else{
		if(degree > max_degree)max_degree = degree;
	}

	graph_nodes++;
}


static void
check_node(long nd)
{
	node_rec *node = node_recs + nd - 1;
	int degree;

	if(prior_array[nd - 1] != 0)return;
	if((degree = node -> contact) == 0)return;         /* isolated vertex */
	
/* I do rely on fact that all not yet renumbered nodes have level
	 equal to zero (init_node) and that all already renumbered nodes do not have 
	 level (post-priority) equal to zero */

	if(degree < min_degree){
		init_nd = nd;
		min_degree = degree;
	}
}



static void
sort_degree(long n, int *key, long *item)
{
	long i, j, d = 1;
	int tmp_key;
	long tmp_item;

	if(n == 1)return;

	i = (long)(log((double)n + 0.5) / LN2);
	while(i--)d *= 2;
	d -= 1;

	while(d != 0){
		for(i = 0; i < n - d; i++){
			if(key[i] <= key[i + d])continue;
			tmp_key = key[i + d];
			key[i + d] = key[i];
			tmp_item = item[i + d];
			item[i + d] = item[i];
			if(i <= d){
				key[i] = tmp_key;
				item[i] = tmp_item;
				continue;
			}
			for(j = i - d; j >= 0; j -= d){
				if(tmp_key >= key[j])break;
				key[j + d] = key[j];
				item[j + d] = item[j];
			}
			key[j + d] = tmp_key;
			item[j + d] = tmp_item;
		}
		d /= 2;
	}
}
				
		

static double
calculate_profile(void)
{
	long i;

	profile = 0.0;
	for(i = 0; i < nodes; i++)update_profile_node(i + 1);
	return(profile);
}


static void
update_profile_node(long nd)
{
	node_rec *node = node_recs + nd - 1, *node_nd = NULL;
	long min_id, nd_nd = 0L;
	int i;

	min_id = node -> id;

	for(i = 0; i < node -> contact; i++){
		nd_nd = node -> connection[i];
		node_nd = node_recs + nd_nd - 1;
		if(node_nd -> id < min_id)min_id = node_nd -> id;
	}

	prior_array[nd - 1] = 0L;
	status_array[nd - 1] = INACTIVE_STATUS;

	profile += (double)(node -> id - min_id + 1);
}
	

static void
update_connection(node_rec *node, long nd)
{
	int i;
	
	for(i = 0; i < node -> contact; i++){
		if(node -> connection[i] == nd){
			node -> connection[i] = -nd;
			break;
		}
	}
}



/* add item actions return allocated list;
	 they should not be used directly with mesh entities because they do not use mem_alloc;
	 this is resolved by taking lists from listing of list and using add list actions */

static list *
add_item_to_head(list **head, long item)
{
	list *lst = NULL;

	alloc(lst, list);      /* macro */
	if(lst == NULL){
		error_status = MEMORY_ALLOCATION_ERROR;
		return(NULL);
	}
	lst -> next = *head;
	lst -> item = item;
	return(*head = lst);
}



static list *
add_item_to_tail(list **head, long item)
{
	list *lst = NULL;

	alloc(lst, list);      /* macro */
	if(lst == NULL){
		error_status = MEMORY_ALLOCATION_ERROR;
		return(NULL);
	}
	lst -> next = NULL;
	lst -> item = item;
	if(*head == NULL)return(*head = lst);
	return(get_tail_list(*head) -> next = lst);
}




/* add list actions return attached list;
	 (list may comprise more elements) */

static list *
add_list_to_head(list **head, list *lst)
{
	get_tail_list(lst) -> next = *head;
	return(*head = lst);
}



static list *
add_list_to_tail(list **head, list *lst)
{
	if(*head == NULL)return(*head = lst);
	return(get_tail_list(*head) -> next = lst);
}

/* get list actions return required list */


static list *
get_head_list(list *lst)
{
	return(lst);
}



static list *
get_tail_list(list *lst)
{
	if(lst == NULL)return(NULL);
	while(lst -> next != NULL)
		lst = lst -> next;
	return(lst);
}

/* returns list size */

static long
get_list_size(list *lst)
{
	long size = 0L;

	if(lst == NULL)return(0);
	while((lst = lst -> next) != NULL)
		size++;
	return(size + 1);
}


/* destroy action assumes that the list is the head, otherwise modification
	 of the previous list to the tail cannot be accomplished ;
	 destroy list should not be used directly with mesh entities because it does not update memory */


static list *
destroy_list(list *lst)
{
	list *prev = lst;

	if(lst != NULL){
		do{
			lst = (prev = lst) -> next;
			free(prev);                      /* macro */
		}while(lst != NULL);
	}
	return(NULL);
}



#ifdef CHECK_CONNECTIVITY
static void
check_connectivity(long num_nodes, node_rec *node_array)
{
	node_rec *node = node_array, *nd = NULL;
	long i, j, k, id;
	logic back_con;

	for(i = 0; i < num_nodes; i++, node++){
		for(j = 0; j < node -> contact; j++){
			id = node -> connection[j];
			if(id == i + 1){
				fprintf(stderr, "Node %ld is self connected\n", id);
				exit(INVALID_CONNECTIVITY);
			}
			for(k = j + 1; k < node -> contact; k++){
				if(node -> connection[k] == id){
					fprintf(stderr, "Node %ld is connected to node %ld nore than once\n", i + 1, id);
					exit(INVALID_CONNECTIVITY);
				}
			}
			nd = node_array + id - 1;
			back_con = NO;
			for(k = 0; k < nd -> contact; k++){
				if(nd -> connection[k] == i + 1){
					back_con = YES;
					break;
				}
			}
			if(back_con == NO){
				fprintf(stderr, "Node %ld is node back connected from node %ld\n", i + 1, id);
				exit(INVALID_CONNECTIVITY);
			}
		}
	}

	fprintf(stdout, "Connectivity OK\n");
}
#endif


