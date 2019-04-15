/*******************************************************************************************\


   #######   #####   ######    #####   ######    #####    ###   #######  #######  #
      #     #     #  #     #  #     #  #     #  #     #    #    #        #        #
      #           #  #     #        #  #     #  #          #    #        #        #
      #      #####   #     #   #####   ######    #####     #    #####    #####    #
      #           #  #     #  #        #              #    #    #        #        #
      #     #     #  #     #  #        #        #     #    #    #        #        #
      #      #####   ######   #######  #         #####    ###   #        #######  #######



                             Covertor from T3d to PSIFEL


                         Copyright: Daniel Rypl 2001 - 2005



                        Czech Technical University in Prague,
          Faculty of Civil Engineering, Department of Structural Mechanics, 
                      Thakurova 7, 166 29 Prague, Czech Republic,
                               email: drypl@fsv.cvut.cz


\*******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <omp.h>

#include <sys/stat.h>  // for mkdir etc
#include <sys/errno.h> // for errno

#define CHUNKSIZE  1

#define METIS
#define ELIXIR

/* #define DEBUG */

/* #define CHECK_GRAPH */

#ifdef CHECK_GRAPH
#define MY_CHECK
#define METIS_CHECK
#endif

#ifdef METIS
#include "metis.h"
#undef ASSERT

//#define PAWBAL    /* partitioning and work balance */
#endif

#ifdef ELIXIR
#define nodes Ckit_nodes
#include "Esimple.h"
#undef nodes
#undef head
#undef list
#undef NO
#undef YES
#undef reverse_list

#define MORE_COLORS

#ifdef MORE_COLORS
#define NUM_COLORS        18
#else
#define NUM_COLORS         9
#endif
#endif


#define FILE_NAME_SIZE   128
#define BUFFER_SIZE    65536

#define KEY_SIZE          32


#define SEPARATOR      " \n"
#define REMARK           '#'


#define VERTEX_ENTITY      1
#define CURVE_ENTITY       2
#define SURFACE_ENTITY     3
#define REGION_ENTITY      4
#define PATCH_ENTITY       5
#define SHELL_ENTITY       6
#define INTERFACE_ENTITY   7

#define LINEAR             1
#define QUADRATIC          2

#define TRI_TETRA          3
#define QUAD_HEXA          4

#define EDGE_ELEM          1
#define FACE_ELEM          2
#define QUAD_ELEM          3
#define TETRA_ELEM         4
#define PYRAM_ELEM         5
#define WEDGE_ELEM         6
#define HEXA_ELEM          7

#define NONE_RENUM         0
#define NODE_RENUM         1
#define ELEM_RENUM         2


#define GENERAL_ERROR      1
#define LONG_NAME_ERROR    2
#define FILE_OPEN_ERROR    3
#define FILE_READ_ERROR    4
#define FILE_WRITE_ERROR   5
#define MEMORY_ERROR       6
#define RENUMBER_ERROR     7
#define PARTITIONING_ERROR 8

#ifdef METIS

/* weights for node-cut with respect to number of nodes;

	 the weights are setuped separately for 1D, 2D and 3D;
	 the weights are setuped with respect to how many elements
	 must be clustered to get number of nodes of the most complex
	 element (quad, hexa);
	 the actual weights for Metis should be taken as inverse value
	 multiplied by the largest weight (because Metis requires integer weights)

	 edge     1  ->  1

	 face     2  ->  1
	 quad     1  ->  2

	 tetra    6  ->  1
	 pyram    3  ->  2
   wedge    2  ->  3
	 hexa     1  ->  6

	 for quadratic meshes these weights (or weight rates) are further modified */

/* isolated nodes are explicitly handled only in node cut;
	 they are output to all partitions always numbered from 1;

	 if number of partitions is 1 and renumbering is not required 
	 their numbering is dependent on their numbering in the input;

	 in element cut isolated nodes are assigned to partitions by Metis */


#define EDGE_NODE_WEIGHT_LINEAR            1

#define FACE_NODE_WEIGHT_LINEAR           10
#define QUAD_NODE_WEIGHT_LINEAR           20

#define TETRA_NODE_WEIGHT_LINEAR          20
#define PYRAM_NODE_WEIGHT_LINEAR          40
#define WEDGE_NODE_WEIGHT_LINEAR          60
#define HEXA_NODE_WEIGHT_LINEAR          120


#define EDGE_NODE_WEIGHT_QUADRATIC         1

#define FACE_NODE_WEIGHT_QUADRATIC        10
#define QUAD_NODE_WEIGHT_QUADRATIC        15

#define TETRA_NODE_WEIGHT_QUADRATIC       20
#define PYRAM_NODE_WEIGHT_QUADRATIC       30        /* 25 - 30 */
#define WEDGE_NODE_WEIGHT_QUADRATIC       60        /* not setuped yet */
#define HEXA_NODE_WEIGHT_QUADRATIC        65        /* 65 - 68 */

#endif


typedef enum logic{
	NO = 0,
	YES = 1
}logic;


typedef struct list list;

struct list{
	void *item;
	list *next;
};


typedef struct entity entity;

struct entity{
	long id;
	int type;
	long property_id;
	long nodes, elems;
	long first_node;
	long first_edge;
	long first_face;
	long first_quad;
	long first_tetra;
	long first_pyram;
	long first_wedge;
	long first_hexa;
	double elem_weight;
	double edge_weight;
	double face_weight;
	double quad_weight;
	double tetra_weight;
	double pyram_weight;
	double wedge_weight;
	double hexa_weight;
	double width;
	logic boundary;
	logic output;
	entity *ngb_ent[2];
	logic selected;
};


typedef struct node_rec node_rec;

struct node_rec{
	long id;                      /* in: old id   out: new id */
	long pos;                     /*              out: position of new id equal to old id */
	long contact;
	long *connection;
};


typedef struct fe_node fe_node;
typedef struct fe_edge fe_edge;
typedef struct fe_face fe_face;
typedef struct fe_quad fe_quad;
typedef struct fe_tetra fe_tetra;
typedef struct fe_pyram fe_pyram;
typedef struct fe_wedge fe_wedge;
typedef struct fe_hexa fe_hexa;


struct fe_node{
	long id;
	entity *entity;
	double x, y, z;
	long glob_id;
};


struct fe_edge{
	entity *entity;
	fe_node *node[2];
	fe_node *midnd[1];
};

struct fe_face{
	entity *entity;
	fe_node *node[3];
	fe_node *midnd[3];
	short bflag;
	union{
		long ngb_elem_id;             /* not used */
		entity *bound_ent;
	}data[3];
};
	
struct fe_quad{
	entity *entity;
	fe_node *node[4];
	fe_node *midnd[4];
	short bflag;
	union{
		long ngb_elem_id;             /* not used */
		entity *bound_ent;
	}data[4];
};
	
struct fe_tetra{
	entity *entity;
	fe_node *node[4];
	fe_node *midnd[6];
	short bflag;
	union{
		long ngb_elem_id;             /* not used */
		entity *bound_ent;
	}data[4];
};
	
struct fe_pyram{
	entity *entity;
	fe_node *node[5];
	fe_node *midnd[8];
	short bflag;
	union{
		long ngb_elem_id;             /* not used */
		entity *bound_ent;
	}data[5];
};
	
struct fe_wedge{
	entity *entity;
	fe_node *node[6];
	fe_node *midnd[9];
	short bflag;
	union{
		long ngb_elem_id;             /* not used */
		entity *bound_ent;
	}data[5];
};
	
struct fe_hexa{
	entity *entity;
	fe_node *node[8];
	fe_node *midnd[12];
	short bflag;
	union{
		long ngb_elem_id;             /* not used */
		entity *bound_ent;
	}data[6];
};

/*
// *entityB = *entityA
#define COPY_entity(entity *entityA, entity *entityB) do {
  entityB = entityA;
} while(0)

// *node_arrayB = *node_arrayA
#define COPY_node_array(fe_node *node_arrayA, fe_node *node_arrayB) do {
  long i;
  for(i = 0, i < nodes; i++){
//    COPY_entity(entityA, entityB);
    B[i] = A[i];
  }
} while(0)
*/

	

#define EDGES          (edges)
#define FACES          (edges + faces)
#define QUADS          (edges + faces + quads)
#define TETRAS         (edges + faces + quads + tetras)
#define PYRAMS         (edges + faces + quads + tetras + pyrams)
#define WEDGES         (edges + faces + quads + tetras + pyrams + wedges)
#define HEXAS          (edges + faces + quads + tetras + pyrams + wedges + hexas)        


#define elem_type(ID)    (((ID) <= EDGES) ? EDGE_ELEM :    \
                          ((ID) <= FACES) ? FACE_ELEM :    \
                          ((ID) <= QUADS) ? QUAD_ELEM :    \
                          ((ID) <= TETRAS) ? TETRA_ELEM :  \
                          ((ID) <= PYRAMS) ? PYRAM_ELEM :  \
                          ((ID) <= WEDGES) ? WEDGE_ELEM :  \
                          ((ID) <= HEXAS) ? HEXA_ELEM : 0) 

#define is_edge(ID)       ((elem_type(ID) == EDGE_ELEM) ? YES : NO)
#define is_face(ID)       ((elem_type(ID) == FACE_ELEM) ? YES : NO)
#define is_quad(ID)       ((elem_type(ID) == QUAD_ELEM) ? YES : NO)
#define is_tetra(ID)      ((elem_type(ID) == TETRA_ELEM) ? YES : NO)
#define is_pyram(ID)      ((elem_type(ID) == PYRAM_ELEM) ? YES : NO)
#define is_wedge(ID)      ((elem_type(ID) == WEDGE_ELEM) ? YES : NO)
#define is_hexa(ID)       ((elem_type(ID) == HEXA_ELEM) ? YES : NO)

/*
#define is_edge(ID)    (((ID) <= EDGES && (ID) > 0) ? YES : NO)
#define is_face(ID)    (((ID) <= FACES && (ID) > EDGES) ? YES : NO)
#define is_quad(ID)    (((ID) <= QUADS && (ID) > FACES) ? YES : NO)
#define is_tetra(ID)   (((ID) <= TETRAS && (ID) > QUADS) ? YES : NO)
#define is_pyram(ID)   (((ID) <= PYRAMS && (ID) > TETRAS) ? YES : NO)
#define is_wedge(ID)   (((ID) <= WEDGES && (ID) > PYRAMS) ? YES : NO)
#define is_hexa(ID)    (((ID) <= HEXAS && (ID) > WEDGES) ? YES : NO)
*/

#define global_edge_id(ID)    ((ID))
#define global_face_id(ID)    ((ID) + EDGES)
#define global_quad_id(ID)    ((ID) + FACES)
#define global_tetra_id(ID)   ((ID) + QUADS)
#define global_pyram_id(ID)   ((ID) + TETRAS)
#define global_wedge_id(ID)   ((ID) + PYRAMS)
#define global_hexa_id(ID)    ((ID) + WEDGES)


#define local_edge_id(ID)     ((ID))
#define local_face_id(ID)     ((ID) - EDGES)
#define local_quad_id(ID)     ((ID) - FACES)
#define local_tetra_id(ID)    ((ID) - QUADS)
#define local_pyram_id(ID)    ((ID) - TETRAS)
#define local_wedge_id(ID)    ((ID) - PYRAMS)
#define local_hexa_id(ID)     ((ID) - WEDGES)



/* note: ordering of neighbouring elements and boundary entities 
	 is the same as the one of t3d on output */


static short face_ed_nd[3][2] = {{0, 1}, {1, 2}, {2, 0}};
static short quad_ed_nd[4][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};

static short tetra_fc_nd[4][3] = {{0, 1, 2}, {0, 3, 1}, {1, 3, 2}, {2, 3, 0}};
static short pyram_fc_nd[5][4] = {{0, 1, 2, 3}, {0, 4, 1, -1}, {1, 4, 2, -1}, {2, 4, 3, -1}, {3, 4, 0, -1}};
static short wedge_fc_nd[5][4] = {{0, 1, 2, -1}, {3, 5, 4, -1}, {0, 3, 4, 1}, {1, 4, 5, 2}, {2, 5, 3, 0}};
static short hexa_fc_nd[6][4] = {{0, 1, 2, 3}, {4, 7, 6, 5}, {0, 4, 5, 1}, {1, 5, 6, 2}, {2, 6, 7, 3}, {3, 7, 4, 0}};


static list *vertex_list = NULL;
static list *curve_list = NULL;
static list *surface_list = NULL;
static list *patch_list = NULL;
static list *shell_list = NULL;
static list *region_list = NULL;
static list *interface_list = NULL;

static logic vertex_select = NO;
static logic curve_select = NO;
static logic surface_select = NO;
static logic patch_select = NO;
static logic shell_select = NO;
static logic region_select = NO;
static logic interface_select = NO;


static enum keyword{
	VERTEX,
	CURVE,
	SURFACE,
	PATCH,
	SHELL,
	REGION,
	INTERFACE,
	ALL,
	EXCEPT,
	WEIGHT,
	KEY_NUM
}key, entity_key, last_key = KEY_NUM;

static char keyword[KEY_NUM][KEY_SIZE] = {
	"VERTEX",
	"CURVE",
	"SURFACE",
	"PATCH",
	"SHELL",
	"REGION",
	"INTERFACE",
	"ALL",
	"EXCEPT",
	"WEIGHT"
};


typedef enum key_case{
	MIXED_CASE,
	UPPER_CASE,
	LOWER_CASE
}key_case;


#define KEY_CASE UPPER_CASE


static char line_buffer[BUFFER_SIZE];
static char in_err_msg[256], out_err_msg[256];
static FILE *active_in_file = NULL, *active_out_file = NULL;

static node_rec *nd_array = NULL;

static fe_node *node_array = NULL;
static fe_edge *edge_array = NULL;
static fe_face *face_array = NULL;
static fe_quad *quad_array = NULL;
static fe_tetra *tetra_array = NULL;
static fe_pyram *pyram_array = NULL;
static fe_wedge *wedge_array = NULL;
static fe_hexa *hexa_array = NULL;

static long nodes = 0, elems = 0, edges = 0, faces = 0, quads = 0, tetras = 0, pyrams = 0, wedges = 0, hexas = 0;
static long num_nodes, num_elems, num_edges, num_faces, num_quads, num_tetras, num_pyrams, num_wedges, num_hexas;

static long midnodes = 0;

static long *node_num_elems = NULL, *node_con_elems = NULL;

static int *remote_node = NULL;
static int *remote_elem = NULL;

static logic node_cut = YES, elem_cut = NO, renum = NO, interface_last = NO;
static logic node_global = NO, elem_global = NO, elem_node_global = NO;
static logic total_numbers = NO, shared_partition = NO;
static logic node_weight = NO, elem_weight = NO, profile_opt = NO;

static double edge_weight, face_weight, quad_weight, tetra_weight, pyram_weight, wedge_weight, hexa_weight;

static int nparts = 1; 

static long *nd_num_nodes = NULL, *nd_con_nodes = NULL, *nd_con_nds = NULL;

#ifdef METIS

/* the following must be separate arrays because they are passed to metis */

static idxtype *node_num_nodes = NULL, *node_con_nodes = NULL;
static idxtype *elem_num_elems = NULL, *elem_con_elems = NULL;

static idxtype *node_part = NULL;
static idxtype *elem_part = NULL;

static idxtype *weight_elem = NULL;

#endif




static list *
add_item_to_list_head(list *any_list, void *item);
static list *
deep_destroy_list(list *any_list);
static list *
destroy_list(list *any_list);
static list *
reverse_list(list *any_list);
static long
get_list_size(list *any_list);


static char *
get_next_record(char *err_msg);
static char *
get_next_relevant_record(char *err_msg);

static char *
get_first_token(char *buffer);
static char *
get_next_first_token(char *err_msg);
static char *
get_next_token(char *err_msg);
static int
get_token_key(char *token, char keyword[1][KEY_SIZE], int keynum, key_case type);

long
get_next_int(char *err_msg);
double
get_next_fpn(char *err_msg);

long
get_int_from_token(char *token);
double
get_fpn_from_token(char *token);

static void
write_current_record(char *err_msg);
static void
read_write_next_record(char *in_err_msg, char *out_err_msg);
static void
read_write_next_relevant_record(char *in_err_msg, char *out_err_msg);
static void
read_write_unrelevant_records(char *in_err_msg, char *out_err_msg);
static void
read_write_until_next_relevant_record(char *in_err_msg, char *out_err_msg);


static void
process_entity_list(void);
static void
apply_weight(void);


static entity *
get_entity_id(list *entity_list, long id);
static entity *
create_entity_id(long id);
static entity *
create_missing_entity_id(long id, int entity_type);


static void
mark_remote_elems(fe_node *node, long node_id, int part_id, long property_id, double width);
static void
mark_remote_elem(long elem_id, int part_id);
static void
unmark_nodes(long node_id, int part_id, long property_id);

static double
calculate_distance_square(fe_node *node1, fe_node *node2);

static void
sort_array(long n, long *key, long *pos);


static void
exit_error_message(char *message, int exit_code, int line);


#define error_message(MSG, CODE)  exit_error_message(MSG, CODE, __LINE__)



#ifdef ELIXIR
void 
EVFastRedraw(EView *view);                       /* not declared in any Elixir header file */


#define FONT  "-adobe-courier-bold-r-normal--*-120-*-*-m-*-iso8859-1"

#define SHRINK    0.8
#define MSIZE       6

#define TRASH_LAYER       MAX_LAYER-1
#define ACTIVE_LAYER                0

static ERenderingType render_mode = NORMAL_RENDERING;
static EShadingType shade_mode = CONST_SHADING_RENDERING;

static double shrink = 1.0;      /* or SHRINK */
static int msize = 0;            /* or MSIZE */

static logic *layers = NULL, virtual_layers = NO;

static GraphicObj ***layer_obj = NULL;
static long *layer_obj_num = NULL;

static Widget set_button[NUM_COLORS], add_button[NUM_COLORS], del_button[NUM_COLORS];

static logic draw_nodes = YES;

int make_path(const char *path, mode_t mode);

static void
draw_node(fe_node *node, int layer, EPixel color, int type, int msize);
static void
draw_edge(fe_edge *edge, int layer, EPixel color);
static void
draw_face(fe_face *face, int layer, EPixel color, EPixel edge_color);
static void
draw_quad(fe_quad *quad, int layer, EPixel color, EPixel edge_color);
static void
draw_tetra(fe_tetra *tetra, int layer, EPixel color, EPixel edge_color);
static void
draw_pyram(fe_pyram *pyram, int layer, EPixel color, EPixel edge_color);
static void
draw_wedge(fe_wedge *wedge, int layer, EPixel color, EPixel edge_color);
static void
draw_hexa(fe_hexa *hexa, int layer, EPixel color, EPixel edge_color);
static void
draw_number(double x, double y, double z, int layer, EPixel color, Font font, long number, char *prefix);

static void
new_view(Widget w, XtPointer ptr, XtPointer call_data);
static void 
redraw_view(Widget w, XtPointer client_data, XtPointer call_data);
static void 
render_view(Widget w, XtPointer ptr, XtPointer call_data);

static void
all_domains(Widget w, XtPointer ptr, XtPointer call_data);
static void
set_domain(Widget w, XtPointer ptr, XtPointer call_data);
static void
add_domain(Widget w, XtPointer ptr, XtPointer call_data);
static void
del_domain(Widget w, XtPointer ptr, XtPointer call_data);

static void
all_domains_virtual(Widget w, XtPointer ptr, XtPointer call_data);
static void
set_domain_virtual(Widget w, XtPointer ptr, XtPointer call_data);
static void
add_domain_virtual(Widget w, XtPointer ptr, XtPointer call_data);
static void
del_domain_virtual(Widget w, XtPointer ptr, XtPointer call_data);

static void
set_nth_domain(Widget w, XtPointer ptr, XtPointer call_data);
static void
add_nth_domain(Widget w, XtPointer ptr, XtPointer call_data);
static void
del_nth_domain(Widget w, XtPointer ptr, XtPointer call_data);

static int
view_all_domains(NODE data, NODE v);
static int
view_set_domain(NODE data, NODE v);
static int
view_add_domain(NODE data, NODE v);
static int
view_del_domain(NODE data, NODE v);

static void
toggle_shrink(Widget w, XtPointer ptr, XtPointer call_data);
static int
shrink_graphics(NODE data, NODE gr_obj);
static void
toggle_msize(Widget w, XtPointer ptr, XtPointer call_data);
static int
msize_graphics(NODE data, NODE gr_obj);

static int
view_normalize_off(NODE data, NODE v);
static int
view_normalize_on(NODE data, NODE v);

static void 
fit_all(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);
static void 
view_origin(Widget w, XtPointer cmdtext_ptr, XtPointer call_data);

static int
reset_active_layer(NODE data, NODE v);
#endif


int
renumber_mesh(long num_nodes, node_rec *node_array, long num_shift, double *old_profile, double *new_profile);


#ifdef PAWBAL

/* since pawbal is using the same data types as metis it is defined under METIS */

typedef struct pawbal_SimpleMesh pawbal_SimpleMesh;
typedef struct pawbal_SimpleGraph pawbal_SimpleGraph;

void
pawbal_partMeshKwaySloan(pawbal_SimpleMesh *mesh_connectivity,
                         pawbal_SimpleGraph *dual_graph,
                         pawbal_SimpleGraph *nodal_graph,
                         int nparts, 
												 idxtype *nodepartnum, idxtype *elempartnum);

pawbal_SimpleGraph *
pawbal_simpleGraphCreateFromParams(int nvtxs, idxtype *xadj, idxtype *adjncy);

pawbal_SimpleMesh *
pawbal_simpleMeshCreateFromParams(int n_elems, int n_nodes, idxtype *xadj, idxtype *adjncy, idxtype *property);
#endif


void postpartition(long shared_nodes, long *part_nodes)
{

  int part, part_id, count;
  long i, j, pos, pnodes, elem, idx_shared_node;
  logic first, find;
  long *shared_node = NULL, *shared_node_part = NULL;
  long *part_pos = NULL;


  if((shared_node = (long *)calloc(shared_nodes, sizeof(long))) == NULL)
    error_message("Memory allocation error", MEMORY_ERROR);
  if((shared_node_part = (long *)calloc(shared_nodes, sizeof(long))) == NULL)
    error_message("Memory allocation error", MEMORY_ERROR);
  if((part_pos = (long *)calloc(nparts, sizeof(long))) == NULL)
    error_message("Memory allocation error", MEMORY_ERROR);

  for(part = 0; part < nparts; part++)part_pos[part] = part;

  pos = 0;
  for(i = 0; i < nodes + midnodes; i++){
    if(node_part[i] > 0)continue;
    shared_node_part[pos] = -node_part[i];
    shared_node[pos] = i;
    pos++;
  }

  /* sort shared nodes with respect to increasing number of domains sharing it */

  sort_array(shared_nodes, shared_node_part, shared_node);

  /* sort domains with respect to increasing number of nodes */

  sort_array(nparts, part_nodes, part_pos);


  double tt2d = omp_get_wtime();

  count = 0;
  first = YES;
  //======================= block1 starts - takes 50% time ====================
  while(YES){ 
    for(part = 0; part < nparts; part++){
      if(part_nodes[part] < 0)continue;

      if(first == YES)
      {
        pnodes = part_nodes[part];
      }
      else{
        if(part_nodes[part] > pnodes)continue;
      }

      part_id = part_pos[part] + 1;

      find = NO;
      //====== multithread- break not possible ======
      for(i = 0; i < shared_nodes; i++){
        if(shared_node_part[i] < 0)continue;
        idx_shared_node = shared_node[i];
        for(j = node_num_elems[idx_shared_node]; j < node_num_elems[idx_shared_node + 1]; j++){
          elem = node_con_elems[j] - 1;
          if(elem_part[elem] == part_id){
            part_nodes[part]++;
            shared_node_part[i] = -part_id;
            count++;
            find = YES;
            break;
          }
        }
        if(find == YES)break;
      }
      //============================================
      if(count == shared_nodes)break;

      part_nodes[part] = -part_nodes[part];
      if(find == YES)
      {
        part_nodes[part] = -part_nodes[part];
        first = NO;
      }

    }  //ends part loop
    if(count == shared_nodes)break;
    first = YES;
  }  // ends while 
  //======================= block1 ends ====================

  double t3 = omp_get_wtime();
  printf("Postpartition2 - not parallelizable = %f seconds \n", t3-tt2d);

  for(i = 0; i < shared_nodes; i++)node_part[shared_node[i]] = shared_node_part[i];

  free(shared_node);
  free(shared_node_part);
  free(part_pos);

} // subroutine ends here


//subroutine
void mthread(long isolated, 
             double old_profile, 
             double new_profile, 
             int mesh_type, 
             int elem_degree, 
             int renum_type, 
             int output_type, 
             logic elixir, 
             EPixel white, 
             EPixel black, 
             EPixel colors[], 
             char sifel_in_name[],
             const int print_map,
             const char *map_filename,
             int nthreads)
{
#pragma omp parallel num_threads(nthreads) firstprivate(elem_part,remote_elem,nd_con_nds,node_array,edge_array,face_array,quad_array,tetra_array,pyram_array,wedge_array,hexa_array,elem_node_global,msize,out_err_msg,active_out_file) private(num_elems,num_nodes,num_edges,num_faces,num_quads,num_tetras,num_pyrams,num_wedges,num_hexas,old_profile,new_profile,stdout) shared(elems,isolated,nodes,midnodes,edges,faces,quads,tetras,pyrams,wedges,hexas,nparts,node_part,node_cut,node_con_elems,node_num_elems,elem_cut,interface_last,renum,total_numbers,elixir,virtual_layers,node_global,draw_nodes,shared_partition,elem_global,nd_num_nodes,nd_con_nodes,output_type,elem_degree,renum_type,mesh_type,sifel_in_name,layer_obj,white,black,colors)
  { 

    int part, part_id, type, nd_part, last_type, bound_ent_tp[6];
    long i, j, k, elem, contact, status, ii, node;
    long property_id, node_id, elem_id, glob_elem_id, orig_id;
    long num_renum_nodes, con_node;
    long nd_id[8], midnd_id[12], curve_id[4], bound_ent_id[6];
    double width;
    logic shared0, remote;

    FILE *psifel_in_file = NULL;
    FILE *map = NULL;
    char psifel_in_name[FILE_NAME_SIZE + 5], suffix[5], err_msg[256];

    fe_node *fe_Node = NULL, *fe_nd = NULL;

    fe_edge *fe_edge = NULL;
    fe_face *fe_face = NULL;
    fe_quad *fe_quad = NULL;
    fe_tetra *fe_tetra = NULL;
    fe_pyram *fe_pyram = NULL;
    fe_wedge *fe_wedge = NULL;
    fe_hexa *fe_hexa = NULL;

    entity *entity = NULL;


    idxtype *elem_part0 = NULL;
    if((elem_part0 = (idxtype *)calloc(elems, sizeof(idxtype))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    int *remote_elem0 = NULL;
    if((remote_elem0 = (int *)calloc(elems, sizeof(int))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    int *remote_node0 = NULL;
    if((remote_node0 = (int *)calloc(nodes + midnodes, sizeof(int))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);


    node_rec *nd_array0 = NULL;
    if(renum == YES || interface_last == YES){
      if((nd_array0 = (node_rec *)calloc(nodes + midnodes, sizeof(node_rec))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);
    }

    fe_node *node_array0 = NULL;
    if((node_array0 = (struct fe_node *)calloc(nodes + midnodes, sizeof(struct fe_node))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);


    for(i = 0; i < elems; i++)
    {
      elem_part0[i] = elem_part[i];
      remote_elem0[i] = remote_elem[i];
    }
    for(i = 0; i < nodes + midnodes; i++)
      memcpy(node_array0 + i, node_array + i, sizeof(struct fe_node));


    #pragma omp for schedule(dynamic, CHUNKSIZE) 
    for(part = 0; part < nparts; part++){


      part_id = part + 1;

      num_elems = 0;
      num_nodes = 0;
      num_edges = 0;
      num_faces = 0;
      num_quads = 0;
      num_tetras = 0;
      num_pyrams = 0;
      num_wedges = 0;
      num_hexas = 0;

      //printf("%d thread++%d of %d\n", part, omp_get_thread_num(), omp_get_num_threads());

      if(node_cut == YES)num_nodes = isolated;        /* each partition will posses all isolated nodes */

      /* node cut - remote_node: zero for nodes inside partition
         negative current partition id for shared node
         positive partition id for nodes outside partition */

      /* note: node ids must be set to prevent them to be zero from previous partition processing
         because negative values of node ids are used */


      fe_Node = node_array0;  
      for(i = 0; i < nodes + midnodes; i++, fe_Node++){
        remote_node0[i] = 0;
        if(fe_Node -> id == 0)fe_Node -> id = i + 1;          /* do not overwrite isolated node id */
        if(node_part[i] < 0){
          for(j = node_num_elems[i]; j < node_num_elems[i+1]; j++){
            elem = node_con_elems[j] - 1;
            if(elem_part0[elem] == part_id){
              remote_node0[i] = -part_id;           /* shared */
              break;
            }
          }
        }
      }


      for(i = 0; i < elems; i++){
        if(elem_part0[i] == part_id){
          num_elems++;
          elem = i + 1;
          type = elem_type(elem);                          /* macro */
          switch(type){
            case EDGE_ELEM:
              num_edges++;
              break;
            case FACE_ELEM:
              num_faces++;
              break;
            case QUAD_ELEM:
              num_quads++;
              break;
            case TETRA_ELEM:
              num_tetras++;
              break;
            case PYRAM_ELEM:
              num_pyrams++;
              break;
            case WEDGE_ELEM:
              num_wedges++;
              break;
            case HEXA_ELEM:
              num_hexas++;
              break;
            default:
              break;
          }
        }
        else{
          if(elem_part0[i] < 0){

            /* midnodes are not handled in this part because 
               this part seems to be relevant only for elem-cut */

            elem = i + 1;
            type = elem_type(elem);                        /* macro */
            switch(type){
              case EDGE_ELEM:
                elem = local_edge_id(elem);                  /* macro */
                fe_edge = &(edge_array[elem - 1]);
                for(j = 0; j < 2; j++){
                  node = fe_edge -> node[j] -> id - 1;
                  if(node_part[node] == part_id){
                    num_elems++;
                    remote_elem0[i] = -part_id;       /* shared */
                    for(k = 0; k < 2; k++){
                      node = fe_edge -> node[k] -> id - 1;
                      if(node_part[node] != part_id)remote_node0[node] = node_part[node];
                    }
                    break;
                  }
                }
                break;
              case FACE_ELEM:
                elem = local_face_id(elem);                  /* macro */
                fe_face = &(face_array[elem - 1]);
                for(j = 0; j < 3; j++){
                  node = fe_face -> node[j] -> id - 1;
                  if(node_part[node] == part_id){
                    num_elems++;
                    remote_elem0[i] = -part_id;       /* shared */
                    for(k = 0; k < 3; k++){
                      node = fe_face -> node[k] -> id - 1;
                      if(node_part[node] != part_id)remote_node0[node] = node_part[node];
                    }
                    break;
                  }
                }
                break;
              case QUAD_ELEM:
                elem = local_quad_id(elem);                 /* macro */
                fe_quad = &(quad_array[elem - 1]);
                for(j = 0; j < 4; j++){
                  node = fe_quad -> node[j] -> id - 1;
                  if(node_part[node] == part_id){
                    num_elems++;
                    remote_elem0[i] = -part_id;       /* shared */
                    for(k = 0; k < 4; k++){
                      node = fe_quad -> node[k] -> id - 1;
                      if(node_part[node] != part_id)remote_node0[node] = node_part[node];
                    }
                    break;
                  }
                }
                break;
              case TETRA_ELEM:
                elem = local_tetra_id(elem);                /* macro */
                fe_tetra = &(tetra_array[elem - 1]);
                for(j = 0; j < 4; j++){
                  node = fe_tetra -> node[j] -> id - 1;
                  if(node_part[node] == part_id){
                    num_elems++;
                    remote_elem0[i] = -part_id;       /* shared */
                    for(k = 0; k < 4; k++){
                      node = fe_tetra -> node[k] -> id - 1;
                      if(node_part[node] != part_id)remote_node0[node] = node_part[node];
                    }
                    break;
                  }
                }
                break;
              case PYRAM_ELEM:
                elem = local_pyram_id(elem);                /* macro */
                fe_pyram = &(pyram_array[elem - 1]);
                for(j = 0; j < 5; j++){
                  node = fe_pyram -> node[j] -> id - 1;
                  if(node_part[node] == part_id){
                    num_elems++;
                    remote_elem0[i] = -part_id;       /* shared */
                    for(k = 0; k < 5; k++){
                      node = fe_pyram -> node[k] -> id - 1;
                      if(node_part[node] != part_id)remote_node0[node] = node_part[node];
                    }
                    break;
                  }
                }
                break;
              case WEDGE_ELEM:
                elem = local_wedge_id(elem);                /* macro */
                fe_wedge = &(wedge_array[elem - 1]);
                for(j = 0; j < 6; j++){
                  node = fe_wedge -> node[j] -> id - 1;
                  if(node_part[node] == part_id){
                    num_elems++;
                    remote_elem0[i] = -part_id;       /* shared */
                    for(k = 0; k < 6; k++){
                      node = fe_wedge -> node[k] -> id - 1;
                      if(node_part[node] != part_id)remote_node0[node] = node_part[node];
                    }
                    break;
                  }
                }
                break;
              case HEXA_ELEM:
                elem = local_hexa_id(elem);                 /* macro */
                fe_hexa = &(hexa_array[elem - 1]);
                for(j = 0; j < 8; j++){
                  node = fe_hexa -> node[j] -> id - 1;
                  if(node_part[node] == part_id){
                    num_elems++;
                    remote_elem0[i] = -part_id;       /* shared */
                    for(k = 0; k < 8; k++){
                      node = fe_hexa -> node[k] -> id - 1;
                      if(node_part[node] != part_id)remote_node0[node] = node_part[node];
                    }
                    break;
                  }
                }
                break;
            }
          }
        }
      }   //ends - i loop
		
      if(node_cut == YES){

        /* caution: in this section negative node ids are used;
           this includes functions: mark_remote_elems
           mark_remote_elem
           unmark_nodes */

        /* midnodes are not considered for nonlocal model */

        for(i = 0; i < nodes; i++){
          if(node_part[i] > 0)continue;
          if(remote_node0[i] >= 0)continue;
          for(j = node_num_elems[i]; j < node_num_elems[i + 1]; j++){
            elem = node_con_elems[j];
            if(elem_part0[elem - 1] != part_id)continue;
            elem_part0[elem - 1] = -part_id;
            type = elem_type(elem);                        /* macro */
            switch(type){
              case EDGE_ELEM:
                elem = local_edge_id(elem);                  /* macro */
                fe_edge = &(edge_array[elem - 1]);
                entity = fe_edge -> entity;
                property_id = entity -> property_id;
                width = entity -> width;
                if(width == 0.0)continue;
                for(k = 0; k < 2; k++){
                  node_id = abs(fe_edge -> node[k] -> id);
                  if(node_part[node_id - 1] < 0){
                    fe_Node = &(node_array0[node_id - 1]);
                    mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);  
                    unmark_nodes(node_id, part_id, property_id);                               
                  }
                }
                break;
              case FACE_ELEM:
                elem = local_face_id(elem);                 /* macro */
                fe_face = &(face_array[elem - 1]);
                entity = fe_face -> entity;
                property_id = entity -> property_id;
                width = entity -> width;
                if(width == 0.0)continue;
                for(k = 0; k < 3; k++){
                  node_id = abs(fe_face -> node[k] -> id);
                  if(node_part[node_id - 1] < 0){
                    fe_Node = &(node_array0[node_id - 1]);
                    mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                    unmark_nodes(node_id, part_id, property_id);
                  }
                }
                break;
              case QUAD_ELEM:
                elem = local_quad_id(elem);                /* macro */
                fe_quad = &(quad_array[elem - 1]);
                entity = fe_quad -> entity;
                property_id = entity -> property_id;
                width = entity -> width;
                if(width == 0.0)continue;
                for(k = 0; k < 4; k++){
                  node_id = abs(fe_quad -> node[k] -> id);
                  if(node_part[node_id - 1] < 0){
                    fe_Node = &(node_array0[node_id - 1]);
                    mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                    unmark_nodes(node_id, part_id, property_id);
                  }
                }
                break;
              case TETRA_ELEM:
                elem = local_tetra_id(elem);               /* macro */
                fe_tetra = &(tetra_array[elem - 1]);
                entity = fe_tetra -> entity;
                property_id = entity -> property_id;
                width = entity -> width;
                if(width == 0.0)continue;
                for(k = 0; k < 4; k++){
                  node_id = abs(fe_tetra -> node[k] -> id);
                  if(node_part[node_id - 1] < 0){
                    fe_Node = &(node_array0[node_id - 1]);
                    mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                    unmark_nodes(node_id, part_id, property_id);
                  }
                }
                break;
              case PYRAM_ELEM:
                elem = local_pyram_id(elem);               /* macro */
                fe_pyram = &(pyram_array[elem - 1]);
                entity = fe_pyram -> entity;
                property_id = entity -> property_id;
                width = entity -> width;
                if(width == 0.0)continue;
                for(k = 0; k < 5; k++){
                  node_id = abs(fe_pyram -> node[k] -> id);
                  if(node_part[node_id - 1] < 0){
                    fe_Node = &(node_array0[node_id - 1]);
                    mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                    unmark_nodes(node_id, part_id, property_id);
                  }
                }
                break;
              case WEDGE_ELEM:
                elem = local_wedge_id(elem);               /* macro */
                fe_wedge = &(wedge_array[elem - 1]);
                entity = fe_wedge -> entity;
                property_id = entity -> property_id;
                width = entity -> width;
                if(width == 0.0)continue;
                for(k = 0; k < 6; k++){
                  node_id = abs(fe_wedge -> node[k] -> id);
                  if(node_part[node_id - 1] < 0){
                    fe_Node = &(node_array0[node_id - 1]);
                    mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                    unmark_nodes(node_id, part_id, property_id);
                  }
                }
                break;
              case HEXA_ELEM:
                elem = local_hexa_id(elem);               /* macro */
                fe_hexa = &(hexa_array[elem - 1]);
                entity = fe_hexa -> entity;
                property_id = entity -> property_id;
                width = entity -> width;
                if(width == 0.0)continue;
                for(k = 0; k < 8; k++){
                  node_id = abs(fe_hexa -> node[k] -> id);
                  if(node_part[node_id - 1] < 0){
                    fe_Node = &(node_array0[node_id - 1]);
                    mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                    unmark_nodes(node_id, part_id, property_id);
                  }
                }
                break;
            }
          }  //ends j loop
          for(j = node_num_elems[i]; j < node_num_elems[i + 1]; j++){
            elem = node_con_elems[j];
            if(elem_part0[elem - 1] < 0)elem_part0[elem - 1] = part_id;
          }
        } //ends i loop
      } /* node_cut */


      if(elem_cut == YES){

        /* caution: in this section negative node ids are used;
           this includes functions: mark_remote_elems
           mark_remote_elem
           unmark_nodes */

        /* midnodes are not considered for nonlocal model */

        for(i = 0; i < elems; i++){
          if(elem_part0[i] > 0)continue;
          if(remote_elem0[i] >= 0)continue;
          elem = i + 1;
          type = elem_type(elem);                        /* macro */
          switch(type){
            case EDGE_ELEM:
              elem = local_edge_id(elem);                  /* macro */
              fe_edge = &(edge_array[elem - 1]);
              entity = fe_edge -> entity;
              property_id = entity -> property_id;
              width = entity -> width;
              if(width == 0.0)continue;
              for(k = 0; k < 2; k++){
                node_id = abs(fe_edge -> node[k] -> id);
                if(node_part[node_id - 1] != part_id){
                  fe_Node = &(node_array0[node_id - 1]);
                  mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                  unmark_nodes(node_id, part_id, property_id);
                }
              }
              break;
            case FACE_ELEM:
              elem = local_face_id(elem);                  /* macro */
              fe_face = &(face_array[elem - 1]);
              entity = fe_face -> entity;
              property_id = entity -> property_id;
              width = entity -> width;
              if(width == 0.0)continue;
              for(k = 0; k < 3; k++){
                node_id = abs(fe_face -> node[k] -> id);
                if(node_part[node_id - 1] != part_id){
                  fe_Node = &(node_array0[node_id - 1]);
                  mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                  unmark_nodes(node_id, part_id, property_id);
                }
              }
              break;
            case QUAD_ELEM:
              elem = local_quad_id(elem);                  /* macro */
              fe_quad = &(quad_array[elem - 1]);
              entity = fe_quad -> entity;
              property_id = entity -> property_id;
              width = entity -> width;
              if(width == 0.0)continue;
              for(k = 0; k < 4; k++){
                node_id = abs(fe_quad -> node[k] -> id);
                if(node_part[node_id - 1] != part_id){
                  fe_Node = &(node_array0[node_id - 1]);
                  mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                  unmark_nodes(node_id, part_id, property_id);
                }
              }
              break;
            case TETRA_ELEM:
              elem = local_tetra_id(elem);                 /* macro */
              fe_tetra = &(tetra_array[elem - 1]);
              entity = fe_tetra -> entity;
              property_id = entity -> property_id;
              width = entity -> width;
              if(width == 0.0)continue;
              for(k = 0; k < 4; k++){
                node_id = abs(fe_tetra -> node[k] -> id);
                if(node_part[node_id - 1] != part_id){
                  fe_Node = &(node_array0[node_id - 1]);
                  mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                  unmark_nodes(node_id, part_id, property_id);
                }
              }
              break;
            case PYRAM_ELEM:
              elem = local_pyram_id(elem);                 /* macro */
              fe_pyram = &(pyram_array[elem - 1]);
              entity = fe_pyram -> entity;
              property_id = entity -> property_id;
              width = entity -> width;
              if(width == 0.0)continue;
              for(k = 0; k < 5; k++){
                node_id = abs(fe_pyram -> node[k] -> id);
                if(node_part[node_id - 1] != part_id){
                  fe_Node = &(node_array0[node_id - 1]);
                  mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                  unmark_nodes(node_id, part_id, property_id);
                }
              }
              break;
            case WEDGE_ELEM:
              elem = local_wedge_id(elem);                 /* macro */
              fe_wedge = &(wedge_array[elem - 1]);
              entity = fe_wedge -> entity;
              property_id = entity -> property_id;
              width = entity -> width;
              if(width == 0.0)continue;
              for(k = 0; k < 6; k++){
                node_id = abs(fe_wedge -> node[k] -> id);
                if(node_part[node_id - 1] != part_id){
                  fe_Node = &(node_array0[node_id - 1]);
                  mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                  unmark_nodes(node_id, part_id, property_id);
                }
              }
              break;
            case HEXA_ELEM:
              elem = local_hexa_id(elem);                 /* macro */
              fe_hexa = &(hexa_array[elem - 1]);
              entity = fe_hexa -> entity;
              property_id = entity -> property_id;
              width = entity -> width;
              if(width == 0.0)continue;
              for(k = 0; k < 8; k++){
                node_id = abs(fe_hexa -> node[k] -> id);
                if(node_part[node_id - 1] != part_id){
                  fe_Node = &(node_array0[node_id - 1]);
                  mark_remote_elems(fe_Node, node_id, part_id, property_id, width * width);
                  unmark_nodes(node_id, part_id, property_id);
                }
              }
              break;
          }
        }
      } /* elem_cut */

      /* local partition numbering */

      if(interface_last == NO){
        fe_Node = node_array0;
        for(i = 0; i < nodes + midnodes; i++, fe_Node++){
          if(node_part[i] == nparts + 1)continue;               /* isolated nodes */
          fe_Node -> id = 0;
          if(node_part[i] == part_id)
            fe_Node -> id = ++num_nodes;
          else{
            if(remote_node0[i] != 0)
              fe_Node -> id = ++num_nodes;
          }
        }
        num_renum_nodes = num_nodes;
      }
      else{

        /* partition nodes */

        fe_Node = node_array0;
        for(i = 0; i < nodes + midnodes; i++, fe_Node++){
          if(node_part[i] == nparts + 1)continue;               /* isolated nodes */
          fe_Node -> id = 0;
          if(node_part[i] == part_id){
            fe_Node -> id = ++num_nodes;
            if(renum == NO)nd_array0[fe_Node -> id - 1].pos = i;   
          }
        }
        num_renum_nodes = num_nodes;


        /* nd_array needs to be setuped also for nodes not subjected to renumbering in order
           to be able to reorder the nodes in increasing id order on the output */

        /* interface nodes */

        fe_Node = node_array0;
        for(i = 0; i < nodes + midnodes; i++, fe_Node++){
          if(node_part[i] != part_id){
            if(remote_node0[i] < 0){
              fe_Node -> id = ++num_nodes;
              nd_array0[fe_Node -> id - 1].pos = i;
            }
          }
        }
      }

      if(renum == YES){

        /* note: only nodes relevant to processed partition are numbered
           therefore the connected nodes must be "renumbered" into nd_con_nds array */

        if(node_cut == YES){
          if(interface_last == NO){
            fe_Node = node_array0;
            for(i = 0; i < nodes + midnodes; i++, fe_Node++){

              /* isolated nodes are added to renumbering to ensure that they are started from 1;
                 (this is guaranteed by the renumbering);
                 alternatively, renumbering may be done without isolated nodes, but in that case
                 it must be ensured, that ids in nd_array are starting from 1 (this is required by renumbering)
                 and that the renumbering is used with the proper number of nodes and with the proper shift */



              if(node_part[i] == part_id || node_part[i] == nparts + 1){
                j = fe_Node -> id - 1;
                nd_array0[j].id = j + 1;
                nd_array0[j].pos = j;
                nd_array0[j].contact = nd_num_nodes[i + 1] - nd_num_nodes[i];
                nd_array0[j].connection = &(nd_con_nds[nd_num_nodes[i]]);

                for(k = nd_num_nodes[i]; k < nd_num_nodes[i + 1]; k++){
                  con_node = nd_con_nodes[k];
                  fe_nd = &(node_array0[con_node - 1]);
                  nd_con_nds[k] = fe_nd -> id;
                }
              }
              else{
                if(remote_node0[i] != 0){
                  j = fe_Node -> id - 1;
                  nd_array0[j].id = j + 1;
                  nd_array0[j].pos = j;
                  nd_array0[j].connection = &(nd_con_nds[nd_num_nodes[i]]);

                  /* note: not all connections are relevant !!!;
                     count the number of relevant connections and "renumber" them into nd_con_nds array */

                  contact = 0;
                  for(k = nd_num_nodes[i]; k < nd_num_nodes[i + 1]; k++){
                    con_node = nd_con_nodes[k];
                    fe_nd = &(node_array0[con_node - 1]);
                    if(fe_nd -> id != 0)
                      nd_array0[j].connection[contact++] = fe_nd -> id;    
                  }

                  nd_array0[j].contact = contact;
                }
              }
            }
          }
          else{
            fe_Node = node_array0;
            for(i = 0; i < nodes + midnodes; i++, fe_Node++){
              if(node_part[i] == part_id || node_part[i] == nparts + 1){
                j = fe_Node -> id - 1;
                nd_array0[j].id = j + 1;
                nd_array0[j].pos = j;
                nd_array0[j].connection = &(nd_con_nds[nd_num_nodes[i]]);

                /* note: not all connections are relevant !!!;
                   count the number of relevant connections and "renumber" them into nd_con_nds array */

                contact = 0;
                for(k = nd_num_nodes[i]; k < nd_num_nodes[i + 1]; k++){
                  con_node = nd_con_nodes[k];
                  fe_nd = &(node_array0[con_node - 1]);
                  if(fe_nd -> id <= num_renum_nodes)
                    nd_array0[j].connection[contact++] = fe_nd -> id;
                }

                nd_array0[j].contact = contact;
              }
            }
          }

#ifdef DEBUG
          fprintf(stderr, "Partition %d:\n", part_id);
          for(i = 0; i < num_renum_nodes; i++){
            fprintf(stderr, "node %ld: %ld:", nd_array0[i].id, nd_array0[i].contact);
            for(j = 0; j < nd_array0[i].contact; j++)fprintf(stderr, " %ld", nd_array0[i].connection[j]);
            fprintf(stderr, "\n");
          }
#endif

        } /* node_cut */


        status = renumber_mesh(num_renum_nodes, nd_array0, 0, &old_profile, &new_profile);
        if(status != 0)
          error_message("Renumbering error", RENUMBER_ERROR);

        //fprintf(stdout, "Partition %d - nominal profile: %.0f (old) --> %.0f (new)\n", part_id, old_profile, new_profile);

        fe_Node = node_array0;
        for(i = 0; i < nodes + midnodes; i++, fe_Node++){
          if((node_id = fe_Node -> id) != 0){
            if(node_id > num_renum_nodes && interface_last == YES)continue;
            fe_Node -> id = nd_array0[node_id - 1].id;
            nd_array0[node_id - 1].id = i;
          }
        }

        for(i = 0; i < num_renum_nodes; i++)nd_array0[i].pos = nd_array0[nd_array0[i].pos].id;
      }   //ends  if(renum == YES)




      if(node_cut == YES){

        /* global interface numbering */

        node_id = 0;
        fe_Node = node_array0;
        for(i = 0; i < nodes + midnodes; i++, fe_Node++){
          if(node_part[i] < 0)fe_Node -> glob_id = ++node_id;
        }
      }


      strcpy(psifel_in_name, sifel_in_name);
      if(strcmp(sifel_in_name, "/dev/null") != 0){
        sprintf(suffix, ".%ld", (long)part);
        strcat(psifel_in_name, suffix);
      }
      
      if(print_map){
        char map_filename_n[2048];
        sprintf(map_filename_n, "%s_%ld.map", map_filename, (long) part);
        map = fopen(map_filename_n, "w");
        if(map == NULL){
          sprintf(err_msg, "File %s opening error", map_filename_n);
          error_message(err_msg, FILE_OPEN_ERROR);          
        }
      }

      if((psifel_in_file = fopen(psifel_in_name, "w")) == NULL){
        sprintf(err_msg, "File %s opening error", psifel_in_name);
        error_message(err_msg, FILE_OPEN_ERROR);
      }
      active_out_file = psifel_in_file;
      sprintf(out_err_msg, "File %s writing error", psifel_in_name);

      if(fprintf(psifel_in_file, " %9d %9d %9d %9d %9d\n", mesh_type, elem_degree, renum_type, output_type, part_id) < 0)
        error_message(out_err_msg, FILE_WRITE_ERROR);

      if(total_numbers == YES){
        switch(mesh_type){
          case TRI_TETRA:
            if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld\n", nodes, edges, faces, tetras) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
            break;
          case QUAD_HEXA:
            if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld\n", nodes, edges, quads, hexas) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
            break;
          case TRI_TETRA + QUAD_HEXA:
            if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld\n", 
                  nodes, edges, faces, quads, tetras, pyrams, wedges, hexas) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
        }
      }

      if(print_map)
        fprintf(map, "%ld %ld\n\n", num_nodes, num_edges + num_faces + num_tetras + num_quads + num_hexas + num_pyrams + num_wedges);

      switch(mesh_type){
        case TRI_TETRA:
          if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld\n\n", num_nodes, num_edges, num_faces, num_tetras) < 0)
            error_message(out_err_msg, FILE_WRITE_ERROR);
          break;
        case QUAD_HEXA:
          if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld\n\n", num_nodes, num_edges, num_quads, num_hexas) < 0)
            error_message(out_err_msg, FILE_WRITE_ERROR);
          break;
        case TRI_TETRA + QUAD_HEXA:
          if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld\n\n", 
                num_nodes, num_edges, num_faces, num_quads, num_tetras, num_pyrams, num_wedges, num_hexas) < 0)
            error_message(out_err_msg, FILE_WRITE_ERROR);
      }


      /*
         fprintf(stdout, "Partition %d - nodes %ld elems %ld (%ld(e) %ld(f) %ld(q) %ld(t) %ld(p) %ld(w) %ld(h))\n", 
         part_id, num_nodes, 
         num_edges + num_faces + num_quads + num_tetras + num_pyrams + num_wedges + num_hexas,
         num_edges, num_faces, num_quads, num_tetras, num_pyrams, num_wedges, num_hexas);
         */

#ifdef ELIXIR
      if(elixir == YES){
        if(virtual_layers == YES && num_elems != 0){

          /* space is allocated only for elements without numbers !!! */

          if((layer_obj[part_id] = (GraphicObj **)calloc(num_elems, sizeof(GraphicObj *))) == NULL)
            error_message("Memory allocation error", MEMORY_ERROR);
        }
      }
#endif


      node_id = 0;
      fe_Node = node_array0;
      for(i = 0; i < nodes + midnodes; i++, fe_Node++){
        if(fe_Node -> id == 0)continue;
        ii = i;
        if(renum == YES || interface_last == YES)
          fe_nd = node_array0 + (ii = nd_array0[node_id++].pos);
        else
          fe_nd = fe_Node;



        shared0 = remote = NO;
        if(node_part[ii] != part_id && node_part[ii] != nparts + 1){    /* account for isolated nodes */
          if(node_part[ii] < 0){
            if(-remote_node0[ii] == part_id)shared0 = YES;
            if(remote_node0[ii] == part_id){
              remote = shared0 = YES;
            }
          }
          else{
            if(remote_node0[ii] != 0)remote = YES;
          }
          if(shared0 == NO && remote == NO)continue;
        }
        
        if(print_map)
          fprintf(map, "%ld ", ii + 1);

        if(node_global == YES){
          if(fe_nd -> glob_id == 0)
            fe_nd -> glob_id = ii + 1;
          else
            fe_nd -> glob_id = -(ii + 1);
        }

#ifdef ELIXIR
        if(elixir == YES){
          if(draw_nodes == YES){
            if(shared0 == NO && remote == NO){
              if(node_part[ii] == nparts + 1)
                draw_node(fe_nd, part_id, white, CIRCLE_MARKER, msize);       /* isolated node */
              else
                draw_node(fe_nd, part_id, colors[(part_id - 1) % NUM_COLORS], FILLED_CIRCLE_MARKER, msize);
            }
            else{
              if(shared0 == YES)
                draw_node(fe_nd, part_id, white, FILLED_CIRCLE_MARKER, msize);
              else
                draw_node(fe_nd, part_id, colors[(remote_node0[ii] - 1) % NUM_COLORS], FILLED_CIRCLE_MARKER, msize);
            }
          }
        }
#endif



        entity = fe_nd -> entity;


        if(shared_partition == YES){
          nd_part = node_part[ii];
          if(nd_part == nparts + 1)nd_part = 0;         /* zero domain is used for isolated nodes ??? */
          nd_part = abs(nd_part);
          if(fprintf(psifel_in_file, " %9ld %9ld %4ld %14.6e %14.6e %14.6e %4d %9ld %9ld\n", 
                fe_nd -> id, fe_nd -> glob_id, (long)nd_part, fe_nd -> x, fe_nd -> y, fe_nd -> z,
                entity -> type, entity -> id, entity -> property_id) < 0)
            error_message(out_err_msg, FILE_WRITE_ERROR);
        }
        else{
          if(fprintf(psifel_in_file, " %9ld %9ld %14.6e %14.6e %14.6e %4d %9ld %9ld\n", 
                fe_nd -> id, fe_nd -> glob_id, fe_nd -> x, fe_nd -> y, fe_nd -> z,
                entity -> type, entity -> id, entity -> property_id) < 0)
            error_message(out_err_msg, FILE_WRITE_ERROR);
        }

      } // ends - fe_Node & i


     if(print_map)  
       fprintf(map, "\n\n");

      last_type = -1;
      elem_id = 0;
      for(i = 0; i < elems; i++){
        shared0 = remote = NO;
        if(elem_part0[i] != part_id){
          if(elem_part0[i] < 0){
            if(-remote_elem0[i] == part_id)shared0 = YES;
            if(remote_elem0[i] == part_id){
              remote = shared0 = YES;
            }
          }
          else{
            if(remote_elem0[i] != 0)remote = YES;
          }
          if(shared0 == NO && remote == NO)continue;
        }

        elem_id++;
        glob_elem_id = i + 1;
        
        if(print_map)  
          fprintf(map, "%ld ", glob_elem_id);
                      
        type = elem_type(glob_elem_id);                /* macro */
        if(last_type != type){
          if(fprintf(psifel_in_file, "\n") < 0)error_message(out_err_msg, FILE_WRITE_ERROR);
        }
        switch(type){
          case EDGE_ELEM:
            elem = local_edge_id(glob_elem_id);                  /* macro */
            fe_edge = &(edge_array[elem - 1]);
            entity = fe_edge -> entity;

#ifdef ELIXIR
            if(elixir == YES){
              if(shared0 == NO && remote == NO)
                draw_edge(fe_edge, part_id, colors[(part_id - 1) % NUM_COLORS]);
              else{
                if(shared0 == YES)
                  draw_edge(fe_edge, part_id, white);
                else
                  draw_edge(fe_edge, part_id, colors[(remote_elem0[i] - 1) % NUM_COLORS]);
              }
            }
#endif

            if(elem_node_global == NO)
              //for(j = 0; j < 2; j++)nd_id[j] = fe_edge -> node[j] -> id;
              for(j = 0; j < 2; j++)
              { 
                orig_id = fe_edge -> node[j] -> id;
                nd_id[j] = node_array0[orig_id - 1].id;
              }
            else
              for(j = 0; j < 2; j++)nd_id[j] = abs(fe_edge -> node[j] -> glob_id);

            if(elem_global == NO)glob_elem_id = elem_id;
            if(elem_degree == LINEAR){
              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %4d %9ld %9ld\n", 
                    glob_elem_id, nd_id[0], nd_id[1],
                    entity -> type, entity -> id, entity -> property_id) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            if(elem_degree == QUADRATIC){
              if(fe_edge -> midnd[0] != NULL)
                midnd_id[0] = fe_edge -> midnd[0] -> id;
              else
                midnd_id[0] = 0;

              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %4d %9ld %9ld\n", 
                    glob_elem_id, nd_id[0], nd_id[1], midnd_id[0],
                    entity -> type, entity -> id, entity -> property_id) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            break;
          case FACE_ELEM:
            elem = local_face_id(glob_elem_id);                    /* macro */
            fe_face = &(face_array[elem - 1]);
            entity = fe_face -> entity;

#ifdef ELIXIR
            if(elixir == YES){
              if(shared0 == NO && remote == NO)
                draw_face(fe_face, part_id, colors[(part_id - 1) % NUM_COLORS], black);
              else{
                if(shared0 == YES)
                  draw_face(fe_face, part_id, white, black);
                else
                  draw_face(fe_face, part_id, colors[(remote_elem0[i] - 1) % NUM_COLORS], black);
              }
            }
#endif

            if(elem_node_global == NO)
              for(j = 0; j < 3; j++)
              { 
                orig_id = fe_face -> node[j] -> id;
                nd_id[j] = node_array0[orig_id - 1].id;
              }
            else
              for(j = 0; j < 3; j++)nd_id[j] = abs(fe_face -> node[j] -> glob_id);

            for(j = 0; j < 3; j++){
              curve_id[j] = 0;
              if((fe_face -> bflag & (1 << j)) != 0)curve_id[j] = fe_face -> data[j].bound_ent -> id;
            }

            if(elem_global == NO)glob_elem_id = elem_id;
            if(elem_degree == LINEAR){
              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2],
                    entity -> type, entity -> id, entity -> property_id,
                    curve_id[0], curve_id[1], curve_id[2]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            if(elem_degree == QUADRATIC){
              for(j = 0; j < 3; j++)midnd_id[j] = fe_face -> midnd[j] -> id;

              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], midnd_id[0], midnd_id[1], midnd_id[2],
                    entity -> type, entity -> id, entity -> property_id,
                    curve_id[0], curve_id[1], curve_id[2]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            break;
          case QUAD_ELEM:
            elem = local_quad_id(glob_elem_id);                       /* macro */
            fe_quad = &(quad_array[elem - 1]);
            entity = fe_quad -> entity;

#ifdef ELIXIR
            if(elixir == YES){
              if(shared0 == NO && remote == NO)
                draw_quad(fe_quad, part_id, colors[(part_id - 1) % NUM_COLORS], black);
              else{
                if(shared0 == YES)
                  draw_quad(fe_quad, part_id, white, black);
                else
                  draw_quad(fe_quad, part_id, colors[(remote_elem0[i] - 1) % NUM_COLORS], black);
              }
            }
#endif

            if(elem_node_global == NO)
              for(j = 0; j < 4; j++)
              { 
                orig_id = fe_quad -> node[j] -> id;
                nd_id[j] = node_array0[orig_id - 1].id;
              }
            else
              for(j = 0; j < 4; j++)nd_id[j] = abs(fe_quad -> node[j] -> glob_id);

            for(j = 0; j < 4; j++){
              curve_id[j] = 0;
              if((fe_quad -> bflag & (1 << j)) != 0)curve_id[j] = fe_quad -> data[j].bound_ent -> id;
            }

            if(elem_global == NO)glob_elem_id = elem_id;
            if(elem_degree == LINEAR){
              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3],
                    entity -> type, entity -> id, entity -> property_id,
                    curve_id[0], curve_id[1], curve_id[2], curve_id[3]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            if(elem_degree == QUADRATIC){
              for(j = 0; j < 4; j++){
                if(fe_quad -> midnd[j] != NULL)
                  midnd_id[j] = fe_quad -> midnd[j] -> id;
                else
                  midnd_id[j] = 0;
              }

              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], 
                    midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3],
                    entity -> type, entity -> id, entity -> property_id,
                    curve_id[0], curve_id[1], curve_id[2], curve_id[3]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            break;
          case TETRA_ELEM:
            elem = local_tetra_id(glob_elem_id);                          /* macro */
            fe_tetra = &(tetra_array[elem - 1]);
            entity = fe_tetra -> entity;

#ifdef ELIXIR
            if(elixir == YES){
              if(shared0 == NO && remote == NO)
                draw_tetra(fe_tetra, part_id, colors[(part_id - 1) % NUM_COLORS], black);
              else{
                if(shared0 == YES)	
                  draw_tetra(fe_tetra, part_id, white, black);
                else
                  draw_tetra(fe_tetra, part_id, colors[(remote_elem0[i] - 1) % NUM_COLORS], black);
              }
            }
#endif

            if(elem_node_global == NO)
              //for(j = 0; j < 4; j++)nd_id[j] = fe_tetra -> node[j] -> id; // ksaha: original id!
              for(j = 0; j < 4; j++)
              { 
                orig_id = fe_tetra -> node[j] -> id;
                nd_id[j] = node_array0[orig_id - 1].id;
              }
            else
              for(j = 0; j < 4; j++)nd_id[j] = abs(fe_tetra -> node[j] -> glob_id);

            for(j = 0; j < 4; j++){
              bound_ent_id[j] = 0;
              bound_ent_tp[j] = 0;
              if((fe_tetra -> bflag & (1 << j)) != 0){
                bound_ent_id[j] = fe_tetra -> data[j].bound_ent -> id;
                bound_ent_tp[j] = fe_tetra -> data[j].bound_ent -> type;
              }
            }

            if(elem_global == NO)glob_elem_id = elem_id;
            if(elem_degree == LINEAR){
              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            if(elem_degree == QUADRATIC){
              for(j = 0; j < 6; j++){
                orig_id = fe_tetra -> midnd[j] -> id;
                midnd_id[j] = node_array0[orig_id - 1].id;
              } 

              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3],
                    midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], midnd_id[5],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            break;
          case PYRAM_ELEM:
            elem = local_pyram_id(glob_elem_id);                          /* macro */
            fe_pyram = &(pyram_array[elem - 1]);
            entity = fe_pyram -> entity;

#ifdef ELIXIR
            if(elixir == YES){
              if(shared0 == NO && remote == NO)
                draw_pyram(fe_pyram, part_id, colors[(part_id - 1) % NUM_COLORS], black);
              else{
                if(shared0 == YES)	
                  draw_pyram(fe_pyram, part_id, white, black);
                else
                  draw_pyram(fe_pyram, part_id, colors[(remote_elem0[i] - 1) % NUM_COLORS], black);
              }
            }
#endif

            if(elem_node_global == NO)
              for(j = 0; j < 5; j++)
              { 
                orig_id = fe_pyram -> node[j] -> id;
                nd_id[j] = node_array0[orig_id - 1].id;
              }
            else
              for(j = 0; j < 5; j++)nd_id[j] = abs(fe_pyram -> node[j] -> glob_id);

            for(j = 0; j < 5; j++){
              bound_ent_id[j] = 0;
              bound_ent_tp[j] = 0;
              if((fe_pyram -> bflag & (1 << j)) != 0){
                bound_ent_id[j] = fe_pyram -> data[j].bound_ent -> id;
                bound_ent_tp[j] = fe_pyram -> data[j].bound_ent -> type;
              }
            }

            if(elem_global == NO)glob_elem_id = elem_id;
            if(elem_degree == LINEAR){
              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            if(elem_degree == QUADRATIC){
              for(j = 0; j < 8; j++){
                orig_id = fe_pyram -> midnd[j] -> id;
                midnd_id[j] = node_array0[orig_id - 1].id;
              }

              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4],
                    midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], midnd_id[5], midnd_id[6], midnd_id[7],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            break;
          case WEDGE_ELEM:
            elem = local_wedge_id(glob_elem_id);                          /* macro */
            fe_wedge = &(wedge_array[elem - 1]);
            entity = fe_wedge -> entity;

#ifdef ELIXIR
            if(elixir == YES){
              if(shared0 == NO && remote == NO)
                draw_wedge(fe_wedge, part_id, colors[(part_id - 1) % NUM_COLORS], black);
              else{
                if(shared0 == YES)	
                  draw_wedge(fe_wedge, part_id, white, black);
                else
                  draw_wedge(fe_wedge, part_id, colors[(remote_elem0[i] - 1) % NUM_COLORS], black);
              }
            }
#endif

            if(elem_node_global == NO)
              for(j = 0; j < 6; j++)
              { 
                orig_id = fe_wedge -> node[j] -> id;
                nd_id[j] = node_array0[orig_id - 1].id;
              }
            else
              for(j = 0; j < 6; j++)nd_id[j] = abs(fe_wedge -> node[j] -> glob_id);

            for(j = 0; j < 5; j++){
              bound_ent_id[j] = 0;
              bound_ent_tp[j] = 0;
              if((fe_wedge -> bflag & (1 << j)) != 0){
                bound_ent_id[j] = fe_wedge -> data[j].bound_ent -> id;
                bound_ent_tp[j] = fe_wedge -> data[j].bound_ent -> type;
              }
            }

            if(elem_global == NO)glob_elem_id = elem_id;
            if(elem_degree == LINEAR){
              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            if(elem_degree == QUADRATIC){
              for(j = 0; j < 9; j++){
                if(fe_wedge -> midnd[j] != NULL){
                  orig_id = fe_wedge -> midnd[j] -> id;
                  midnd_id[j] = node_array0[orig_id - 1].id;
                }
                else
                  midnd_id[j] = 0;
              }

              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5],
                    midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], 
                    midnd_id[5], midnd_id[6], midnd_id[7], midnd_id[8],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            break;
          case HEXA_ELEM:
            elem = local_hexa_id(glob_elem_id);                           /* macro */
            fe_hexa = &(hexa_array[elem - 1]);
            entity = fe_hexa -> entity;

#ifdef ELIXIR
            if(elixir == YES){
              if(shared0 == NO && remote == NO)
                draw_hexa(fe_hexa, part_id, colors[(part_id - 1) % NUM_COLORS], black);
              else{
                if(shared0 == YES)	
                  draw_hexa(fe_hexa, part_id, white, black);
                else
                  draw_hexa(fe_hexa, part_id, colors[(remote_elem0[i] - 1) % NUM_COLORS], black);
              }
            }
#endif

            if(elem_node_global == NO)
              for(j = 0; j < 8; j++)
              { 
                orig_id = fe_hexa -> node[j] -> id;
                nd_id[j] = node_array0[orig_id - 1].id;
              }
            else
              for(j = 0; j < 8; j++)nd_id[j] = abs(fe_hexa -> node[j] -> glob_id);

            for(j = 0; j < 6; j++){
              bound_ent_id[j] = 0;
              bound_ent_tp[j] = 0;
              if((fe_hexa -> bflag & (1 << j)) != 0){
                bound_ent_id[j] = fe_hexa -> data[j].bound_ent -> id;
                bound_ent_tp[j] = fe_hexa -> data[j].bound_ent -> type;
              }
            }

            if(elem_global == NO)glob_elem_id = elem_id;
            if(elem_degree == LINEAR){
              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5], nd_id[6], nd_id[7],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4], bound_ent_id[5],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4], bound_ent_tp[5]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            if(elem_degree == QUADRATIC){
              for(j = 0; j < 12; j++){
                if(fe_hexa -> midnd[j] != NULL){
                  orig_id = fe_hexa -> midnd[j] -> id;
                  midnd_id[j] = node_array0[orig_id - 1].id;
                }
                else
                  midnd_id[j] = 0;
              }

              if(fprintf(psifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d %4d\n", 
                    glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5], nd_id[6], nd_id[7],
                    midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], midnd_id[5], 
                    midnd_id[6], midnd_id[7], midnd_id[8], midnd_id[9], midnd_id[10], midnd_id[11],
                    entity -> type, entity -> id, entity -> property_id,
                    bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4], bound_ent_id[5],
                    bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4], bound_ent_tp[5]) < 0)
                error_message(out_err_msg, FILE_WRITE_ERROR);
            }
            break;
        }
        last_type = type;
        
        if(print_map)  
          fprintf(map, "%ld %ld\n", entity -> type, entity -> property_id);

        remote_elem0[i] = 0;
      }

      fclose(psifel_in_file);
      if(print_map)
        fclose(map);

    } // ends - part


    free(elem_part0);
    free(remote_elem0);
    free(remote_node0);
    free(nd_array0);
    if(node_array0!=NULL)
      free(node_array0);


  } // ends - pragma 

} // ends - subroutine 



// start of main code
int main(int argc, char **argv)
{

  double t1 = omp_get_wtime();

  long node_id, elem_id, glob_elem_id, entity_id, property_id, last_entity_id = 0;
  long node, elem, nd, node1, node2, node3, node4, elem1, elem2, nd_id[8], midnd_id[12];
  long i, j, k, m, n, p, j1, j2, j3, j4, size, pos, number;
  long curve_id[4], bound_ent_id[6], first_midnd_id = 0;
  int mesh_type, elem_degree, renum_type, output_type, entity_type, last_entity_type = 0, bound_ent_tp[6];
  int argcc, count, part, status, dimension, dim_coeff[3], weight_coeff[8];
  int options[5], edge_cut, graph_size, num_flag, weight_flag, type, last_type, nthreads;
  double x, y, z, weight, old_profile, new_profile;
  logic bflag, *part_flag = NULL, usage = NO, mixed = NO;
  FILE *t3d_out_file = NULL, L, *graph_file = NULL, *weight_file = NULL;
  FILE *sifel_in_file = NULL;
  char t3d_out_name[FILE_NAME_SIZE], sifel_in_name[FILE_NAME_SIZE], *token = NULL;

  int print_map = 0;  
  char map_filename[FILE_NAME_SIZE];
  
  char err_msg[256], weight_name[FILE_NAME_SIZE];
  list *entity_list = NULL;
  entity *entity = NULL, *ent = NULL, *iface_ent = NULL;
  fe_node *fe_Node = NULL, *fe_nd = NULL;
  fe_edge *fe_edge = NULL;
  fe_face *fe_face = NULL;
  fe_quad *fe_quad = NULL;
  fe_tetra *fe_tetra = NULL;
  fe_pyram *fe_pyram = NULL;
  fe_wedge *fe_wedge = NULL;
  fe_hexa *fe_hexa = NULL;
  long *part_nodes = NULL; 
  long shared_nodes = 0, isolated = 0;
  logic entity_select = NO, specification = NO;

#ifdef METIS
	logic sg = NO, ag = NO, el = NO, eg = NO, nw = NO, ew = NO;
	long num_renum_nodes;
#endif
#ifdef PAWBAL
	pawbal_SimpleGraph *nodal_graph = NULL, *dual_graph = NULL;
	pawbal_SimpleMesh *mesh_connectivity = NULL;

	idxtype *elem_num_con = NULL;
	idxtype *elem_connect = NULL;
	idxtype *node_prop = NULL;
#endif

#ifdef ELIXIR
  EView *view = NULL;
  EPixel white, black, colors[NUM_COLORS];
  EPixel green, red, yellow, magenta, cyan, blue, orange, purple, pink;
  Font font;
  BOOLEAN success;
  logic elixir = NO;
  char app_name[] = "T3d2psifel - Metis - DD", *app_nm = app_name;
  Arg args[2];
  int argn;
#endif

#ifdef METIS
  if(argc < 5)usage = YES;
#else
  if(argc < 3)usage = YES;
#endif

  if(usage == YES){
    fprintf(stderr, "\n");

#ifdef METIS
    fprintf(stderr, "Usage: t3d2psifel -np [nproc] t3d_output_file sifel_input_file -nt [nthreads]\n");
    fprintf(stderr, "                  [-rn] [-sg|ag] [-el|eg] [-nw] [-ew weight_file]\n");
    fprintf(stderr, "                  [-ng] [-tn] [-sp] [-il]");
#ifdef PAWBAL
    fprintf(stderr, " [-po]");
#endif
#ifdef ELIXIR
    fprintf(stderr, " [-X] [-dn]");
#endif
    fprintf(stderr, "\n\n");

    fprintf(stderr, "Options: -np    number of partitions\n");
    fprintf(stderr, "         -nt    number of OMP_NUM_THREADS\n"); 
    fprintf(stderr, "         -rn    node renumbering\n"); 
    fprintf(stderr, "         -sg    global node ids for shared nodes only (default)\n"); 
    fprintf(stderr, "         -ag    global node ids for all nodes (shared negative)\n"); 
    fprintf(stderr, "         -el    local elem ids (default)\n"); 
    fprintf(stderr, "         -eg    global elem ids\n"); 
    fprintf(stderr, "         -ng    global nodes of elements (only together with -ag)\n");
    fprintf(stderr, "         -tn    total number of nodes and elements\n"); 
    fprintf(stderr, "         -sp    partition assignment for shared nodes\n"); 
    fprintf(stderr, "         -il    number interface nodes last\n"); 
    fprintf(stderr, "         -nw    node weighting (mixed meshes only)\n"); 
    fprintf(stderr, "         -ew    element weighting\n"); 
    fprintf(stderr, "         -map   print mapping files (global node and element IDs) [arg1], [arg2]\n");
    fprintf(stderr, "                [arg1]: directory name\n");
    fprintf(stderr, "                [arg2]: filebase name\n");
#ifdef PAWBAL
    fprintf(stderr, "         -po    profile optimization (only together with -rn)\n"); 
#endif
#ifdef ELIXIR
    fprintf(stderr, "         -X     graphic interface\n");
    fprintf(stderr, "         -dn    do not draw nodes (only for -np < %d)\n", MAX_LAYER);
#endif
    fprintf(stderr, "\n");

#ifdef PAWBAL
    fprintf(stderr, "Options -sg -ag -el -eg -ng -tn -sp -po -il -nw -ew -X\n");
#else
    fprintf(stderr, "Options -sg -ag -el -eg -ng -tn -sp -il -nw -ew -X\n");
#endif
    fprintf(stderr, "        are relevant only for -np # > 1\n\n");

    /* with options -il renumbering is subjected to non-interface nodes only */

#else
    fprintf(stderr, "Usage: t3d2psifel t3d_output_file sifel_input_file [-rn]\n\n");
    fprintf(stderr, "Options: -rn    node renumbering\n");
#endif

		exit(1);
	}

  argcc = 1;


#ifdef METIS
  if(strcmp(argv[argcc++], "-np") == 0){
    nparts = atoi(argv[argcc++]);
    if(nparts <= 0)error_message("Invalid number of partitions", GENERAL_ERROR);
  }
  else
    error_message("Number of partitions missing\nUse t3d2psifel without arguments to see the usage", GENERAL_ERROR);
#endif


  if(strlen(argv[argcc]) >= FILE_NAME_SIZE)error_message("Too long t3d_output_file name", LONG_NAME_ERROR);
  strncpy(t3d_out_name, argv[argcc++], FILE_NAME_SIZE);

  if(strlen(argv[argcc]) >= FILE_NAME_SIZE)error_message("Too long sifel_input_file name", LONG_NAME_ERROR);
  strncpy(sifel_in_name, argv[argcc++], FILE_NAME_SIZE);

  /* set defalut for OMP_NUM_THREADS */ 
  nthreads = 1;

  while(argcc != argc){

    /* Read OMP_NUM_THREADS */
    if(strcmp(argv[argcc], "-nt") == 0){
      nthreads = atoi(argv[++argcc]);
      if(nthreads <= 0)error_message("Invalid number for -nt (OMP_NUM_THREADS)", GENERAL_ERROR);
      if(nthreads > omp_get_max_threads())printf("WARNING: You are asking for more threads than the system has (=omp_get_max_threads())");
      argcc++;
      continue;
    }

    if(strcmp(argv[argcc], "-rn") == 0){
      renum = YES;
      argcc++;
      continue;
    }

#ifdef METIS
    if(strcmp(argv[argcc], "-sg") == 0){
      sg = YES;
      node_global = NO;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-ag") == 0){
      ag = YES;
      node_global = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-el") == 0){
      el = YES;
      elem_global = NO;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-eg") == 0){
      eg = YES;
      elem_global = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-ng") == 0){
      elem_node_global = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-tn") == 0){
      total_numbers = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-sp") == 0){
      shared_partition = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-il") == 0){
      interface_last = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-nw") == 0){
      nw = YES;
      node_weight = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-ew") == 0){
      ew = YES;
      elem_weight = YES;
      argcc++;

      if(strlen(argv[argcc]) >= FILE_NAME_SIZE)error_message("Too long weight_file name", LONG_NAME_ERROR);
      strncpy(weight_name, argv[argcc++], FILE_NAME_SIZE);
      continue;
    }    
    if(strcmp(argv[argcc], "-map") == 0){      
      print_map = 1;
      ++argcc;
      
      char map_filebase[FILE_NAME_SIZE];
      char map_dir[FILE_NAME_SIZE];      

      if(strlen(argv[argcc]) >= FILE_NAME_SIZE)error_message("Too long map directory name", LONG_NAME_ERROR);
      strncpy(map_dir, argv[argcc], FILE_NAME_SIZE);
      
      ++argcc;

      if(strlen(argv[argcc]) >= FILE_NAME_SIZE)error_message("Too long map file base name", LONG_NAME_ERROR);
      strncpy(map_filebase, argv[argcc], FILE_NAME_SIZE);
      
      sprintf(map_filename, "%s/%s", map_dir, map_filebase);
      
      printf("Generate [%s]...\n", map_dir);
      make_path(map_dir,0777);
      
      ++argcc;
      continue;
    }
#ifdef PAWBAL
    if(strcmp(argv[argcc], "-po") == 0){
      profile_opt = YES;
      argcc++;
      continue;
    }
#endif
#ifdef ELIXIR
    if(strcmp(argv[argcc], "-X") == 0){
      elixir = YES;
      argcc++;
      continue;
    }
    if(strcmp(argv[argcc], "-dn") == 0){
      draw_nodes = NO;
      argcc++;
      continue;
    }
#endif
#endif

    fprintf(stderr, "Unknown option: %s\n", argv[argcc]);
    argcc++;
  }

#ifdef METIS
  if(sg == YES && ag == YES)
    error_message("Options -sg and -ag are mutually exclusive", GENERAL_ERROR);
  if(el == YES && eg == YES)
    error_message("Options -el and -eg are mutually exclusive", GENERAL_ERROR);
  if(nw == YES && ew == YES)
    error_message("Options -nw and -ew are mutually exclusive", GENERAL_ERROR);

  /* dodelat multi constrain partitioning for simultaneous -nw and -ew */

  if(elem_node_global == YES && ag == NO)
    error_message("Option -ng must be used together with option -ag", GENERAL_ERROR);

#ifdef PAWBAL
  if(profile_opt == YES){
    if(renum == NO)profile_opt = NO;
    if(nparts == 1)profile_opt = NO;

    if(profile_opt == YES){
      if(node_weight == YES){
        fprintf(stderr, "WARNING: Node weighting disabled when profile optimization requested.\n");
        node_weight = NO;
      }
    }
  }
#endif
#endif


  // if rank != 0 goto partition
  if((t3d_out_file = fopen(t3d_out_name, "r")) == NULL){
    sprintf(err_msg, "File %s opening error", t3d_out_name);
    error_message(err_msg, FILE_OPEN_ERROR);
  }
  active_in_file = t3d_out_file;
  sprintf(in_err_msg, "File %s reading error", t3d_out_name);

  get_next_relevant_record(in_err_msg);
  if(sscanf(line_buffer, " %9d %9d %9d %9d", &mesh_type, &elem_degree, &renum_type, &output_type) != 4)
    error_message(in_err_msg, FILE_READ_ERROR);

  if((output_type &= 8) == 0)
    error_message("Invalid t3d -p option (8-missing)", GENERAL_ERROR);

  get_next_relevant_record(in_err_msg);
  switch(mesh_type){
    case TRI_TETRA:
      if(sscanf(line_buffer, " %9ld %9ld %9ld %9ld", &nodes, &edges, &faces, &tetras) != 4)
        error_message(in_err_msg, FILE_READ_ERROR);
      elems = edges + faces + tetras;
      break;
    case QUAD_HEXA:
      if(sscanf(line_buffer, " %9ld %9ld %9ld %9ld", &nodes, &edges, &quads, &hexas) != 4)
        error_message(in_err_msg, FILE_READ_ERROR);
      elems = edges + quads + hexas;
      break;
    case TRI_TETRA + QUAD_HEXA:
      if(sscanf(line_buffer, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld", 
            &nodes, &edges, &faces, &quads, &tetras, &pyrams, &wedges, &hexas) != 8)
        error_message(in_err_msg, FILE_READ_ERROR);
      elems = edges + faces + quads + tetras + pyrams + wedges + hexas;
      break;
    default:
      error_message("Unknown element type", GENERAL_ERROR);
      break;
  }

  if(tetras != 0 || pyrams != 0 || wedges != 0 || hexas != 0)
    dimension = 3;
  else{
    if(faces != 0 || quads != 0)
      dimension = 2;
    else
      dimension = 1;
  }

  if(elems != edges && edges != 0)mixed = YES;
  if(elems != faces && faces != 0)mixed = YES;
  if(elems != quads && quads != 0)mixed = YES;
  if(elems != tetras && tetras != 0)mixed = YES;
  if(elems != pyrams && pyrams != 0)mixed = YES;
  if(elems != wedges && wedges != 0)mixed = YES;
  if(elems != hexas && hexas != 0)mixed = YES;

  switch(elem_degree){
    case LINEAR:
      break;
    case QUADRATIC:
      if(node_cut == NO)error_message("Quadratic elements not supported for elem-cut", GENERAL_ERROR);
      if(profile_opt == YES)error_message("Quadratic elements not supported for profile optimization", GENERAL_ERROR);
      break;
    default:
      error_message("Unsupported element degree", GENERAL_ERROR);
  }

  switch(renum_type){
    case NONE_RENUM:
      break;
    case NODE_RENUM:
      if(elem_degree == QUADRATIC){
        if((output_type &= 128) == 0)
          error_message("Invalid t3d -p option (128-missing)", GENERAL_ERROR);
      }
      if(renum == YES && nparts == 1){
        fprintf(stderr, "WARNING: Renumbered mesh ==> node renumbering ignored.\n");
        renum = NO;
      }
      break;
    case ELEM_RENUM:
      error_message("Renumbering of elements not supported\n", GENERAL_ERROR);
      break;
    default:
      error_message("Unknown renumbering type", GENERAL_ERROR);
  }

  if(nodes == 0)
    error_message("Not enough nodes found", GENERAL_ERROR);

  if((node_array = (struct fe_node *)calloc(nodes, sizeof(struct fe_node))) == NULL)
    error_message("Memory allocation error", MEMORY_ERROR);

  if(edges != 0){
    if((edge_array = (struct fe_edge *)calloc(edges, sizeof(struct fe_edge))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }
  if(faces != 0){
    if((face_array = (struct fe_face *)calloc(faces, sizeof(struct fe_face))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }
  if(quads != 0){
    if((quad_array = (struct fe_quad *)calloc(quads, sizeof(struct fe_quad))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }
  if(tetras != 0){
    if((tetra_array = (struct fe_tetra *)calloc(tetras, sizeof(struct fe_tetra))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }
  if(pyrams != 0){
    if((pyram_array = (struct fe_pyram *)calloc(pyrams, sizeof(struct fe_pyram))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }
  if(wedges != 0){
    if((wedge_array = (struct fe_wedge *)calloc(wedges, sizeof(struct fe_wedge))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }
  if(hexas != 0){
    if((hexa_array = (struct fe_hexa *)calloc(hexas, sizeof(struct fe_hexa))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }

  /* I am using fscanf because I am reading fixed format, any item of which is used as
     flow control variable;
     to enable use of variable number of parameters on the input line the rest of the record 
     (at least the new-line character at the end of the record) is read into string 
     from which the remaining variables are read */

  for(i = 0; i < nodes; i++){
    if(fscanf(active_in_file, "%ld %lf %lf %lf %d %ld %ld", &node_id, &x, &y, &z, 
          &entity_type, &entity_id, &property_id) != 7)
      error_message(in_err_msg, FILE_READ_ERROR);

    get_next_record(in_err_msg);

    switch(entity_type){
      case VERTEX_ENTITY:
        entity_list = vertex_list;
        break;
      case CURVE_ENTITY:
        entity_list = curve_list;
        /*
           if(sscanf(line_buffer, "%lf %lf %lf", &tx, &ty, &tz) != 3)
           error_message(in_err_msg, FILE_READ_ERROR);
           */
        break;
      case SURFACE_ENTITY:
        entity_list = surface_list;
        /*
           if(sscanf(line_buffer, "%lf %lf %lf", &nx, &ny, &nz) != 3)
           error_message(in_err_msg, FILE_READ_ERROR);
           */
        break;
      case PATCH_ENTITY:
        entity_list = patch_list;
        /*
           if(sscanf(line_buffer, "%lf %lf %lf", &nx, &ny, &nz) != 3)
           error_message(in_err_msg, FILE_READ_ERROR);
           */
        break;
      case SHELL_ENTITY:
        entity_list = shell_list;
        /*
           if(sscanf(line_buffer, "%lf %lf %lf", &nx, &ny, &nz) != 3)
           error_message(in_err_msg, FILE_READ_ERROR);
           */
        break;
      case REGION_ENTITY:
        entity_list = region_list;
        break;
      case INTERFACE_ENTITY:
        entity_list = interface_list;
        break;
      default:
        error_message("Unknown entity type", GENERAL_ERROR);
    }

    if(node_id <= 0 || node_id > nodes)
      error_message("Incosistent node numbering", GENERAL_ERROR);

    fe_Node = &(node_array[node_id - 1]);

    if(last_entity_type != entity_type || last_entity_id != entity_id){
      entity = get_entity_id(entity_list, entity_id);
      if(entity == NULL){
        entity = create_missing_entity_id(entity_id, entity_type);
        entity -> property_id = property_id;
      }
      last_entity_type = entity_type;
      last_entity_id = entity_id;
    }
    else{
      if(entity -> property_id != property_id)
        error_message("Inconsistent node property", GENERAL_ERROR);
    }

    if(entity -> first_node == 0)entity -> first_node = node_id;
    entity -> nodes++;

    fe_Node -> id = node_id;
    fe_Node -> entity = entity;

    fe_Node -> x = x;
    fe_Node -> y = y;
    fe_Node -> z = z;

    fe_Node -> glob_id = 0;
  }

  iface_ent = create_missing_entity_id(-1, INTERFACE_ENTITY);

  if(elem_degree == LINEAR){
    if(edges != 0){
      for(i = 0; i < edges; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %d %ld %ld", 
              &elem_id, &nd_id[0], &nd_id[1], &entity_type, &entity_id, &property_id) != 6)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_edge(elem_id) == NO)
          error_message("Inconsistent edge numbering", GENERAL_ERROR);

        elem_id = local_edge_id(elem_id);                        /* macro */
        fe_edge = &(edge_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == CURVE_ENTITY)entity_list = curve_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent edge property", GENERAL_ERROR);
        }

        if(entity -> first_edge == 0)entity -> first_edge = elem_id;
        entity -> elems++;

        fe_edge -> entity = entity;
        for(j = 0; j < 2; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid edge node", GENERAL_ERROR);

          fe_edge -> node[j] = &(node_array[nd - 1]);
        }
        fe_edge -> midnd[0] = NULL;
      }
    }

    if(faces != 0){
      for(i = 0; i < faces; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %d %ld %ld %ld %ld %ld", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &entity_type, &entity_id, &property_id,
              &curve_id[0], &curve_id[1], &curve_id[2]) != 10)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_face(elem_id) == NO)
          error_message("Inconsistent face numbering", GENERAL_ERROR);

        elem_id = local_face_id(elem_id);                        /* macro */
        fe_face = &(face_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == SURFACE_ENTITY)entity_list = surface_list;
          if(entity_type == PATCH_ENTITY)entity_list = patch_list;
          if(entity_type == SHELL_ENTITY)entity_list = shell_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent face property", GENERAL_ERROR);
        }

        if(entity -> first_face == 0)entity -> first_face = elem_id;
        entity -> elems++;

        fe_face -> entity = entity;

        for(j = 0; j < 3; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid face node", GENERAL_ERROR);

          fe_face -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 3; j++){
          fe_face -> midnd[j] = NULL;
        }

        for(j = 0; j < 3; j++){
          fe_face -> data[j].ngb_elem_id = 0;
          if(curve_id[j] != 0){
            if((ent = get_entity_id(curve_list, curve_id[j])) == NULL)
              ent = create_missing_entity_id(curve_id[j], CURVE_ENTITY);
            fe_face -> data[j].bound_ent = ent;
            fe_face -> bflag |= 1 << j;
            ent -> boundary = YES;
          }
        }

      }
    }

    if(quads != 0){
      for(i = 0; i < quads; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], &entity_type, &entity_id, &property_id,
              &curve_id[0], &curve_id[1], &curve_id[2], &curve_id[3]) != 12)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_quad(elem_id) == NO)
          error_message("Inconsistent quad numbering", GENERAL_ERROR);

        if(nd_id[0] == nd_id[1] || nd_id[1] == nd_id[2] || nd_id[2] == nd_id[3] || nd_id[3] == nd_id[0])
          error_message("Degenerated quads not supported", GENERAL_ERROR);

        elem_id = local_quad_id(elem_id);                        /* macro */
        fe_quad = &(quad_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == SURFACE_ENTITY)entity_list = surface_list;
          if(entity_type == PATCH_ENTITY)entity_list = patch_list;
          if(entity_type == SHELL_ENTITY)entity_list = shell_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent quad property", GENERAL_ERROR);
        }

        if(entity -> first_quad == 0)entity -> first_quad = elem_id;
        entity -> elems++;

        fe_quad -> entity = entity;

        for(j = 0; j < 4; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid quad node", GENERAL_ERROR);

          fe_quad -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 4; j++){
          fe_quad -> midnd[j] = NULL;
        }

        for(j = 0; j < 4; j++){
          fe_quad -> data[j].ngb_elem_id = 0;
          if(curve_id[j] != 0){
            if(curve_id[j] == -1)
              ent = iface_ent;
            else{
              if((ent = get_entity_id(curve_list, curve_id[j])) == NULL)
                ent = create_missing_entity_id(curve_id[j], CURVE_ENTITY);
            }
            fe_quad -> data[j].bound_ent = ent;
            fe_quad -> bflag |= 1 << j;
            ent -> boundary = YES;
          }
        }
      }
    }

    if(tetras != 0){
      for(i = 0; i < tetras; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], 
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3]) != 16)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_tetra(elem_id) == NO)
          error_message("Inconsistent tetra numbering", GENERAL_ERROR);

        elem_id = local_tetra_id(elem_id);                       /* macro */
        fe_tetra = &(tetra_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          entity = get_entity_id(region_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent tetra property", GENERAL_ERROR);
        }

        if(entity -> first_tetra == 0)entity -> first_tetra = elem_id;
        entity -> elems++;

        fe_tetra -> entity = entity;

        for(j = 0; j < 4; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid tetra node", GENERAL_ERROR);

          fe_tetra -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 6; j++){
          fe_tetra -> midnd[j] = NULL;
        }

        for(j = 0; j < 4; j++){
          fe_tetra -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
            if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
            if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
            if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
              ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            fe_tetra -> data[j].bound_ent = ent;
            fe_tetra -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }

    if(pyrams != 0){
      for(i = 0; i < pyrams; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %ld %d %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], &nd_id[4], 
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], &bound_ent_id[4], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3], &bound_ent_tp[4]) != 19)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_pyram(elem_id) == NO)
          error_message("Inconsistent pyram numbering", GENERAL_ERROR);

        if(nd_id[0] == nd_id[1] || nd_id[1] == nd_id[2] || nd_id[2] == nd_id[3] || nd_id[3] == nd_id[0])
          error_message("Degenerated pyrams not supported", GENERAL_ERROR);

        elem_id = local_pyram_id(elem_id);                        /* macro */
        fe_pyram = &(pyram_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          entity = get_entity_id(region_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent pyram property", GENERAL_ERROR);
        }

        if(entity -> first_pyram == 0)entity -> first_pyram = elem_id;
        entity -> elems++;

        fe_pyram -> entity = entity;

        for(j = 0; j < 5; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid pyram node", GENERAL_ERROR);

          fe_pyram -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 8; j++){
          fe_pyram -> midnd[j] = NULL;
        }

        for(j = 0; j < 5; j++){
          fe_pyram -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
            if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
            if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
            if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
              ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            fe_pyram -> data[j].bound_ent = ent;
            fe_pyram -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }

    if(wedges != 0){
      for(i = 0; i < wedges; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %ld %d %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], &nd_id[4], &nd_id[5], 
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], &bound_ent_id[4], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3], &bound_ent_tp[4]) != 20)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_wedge(elem_id) == NO)
          error_message("Inconsistent wedge numbering", GENERAL_ERROR);

        if(nd_id[0] == nd_id[1] || nd_id[1] == nd_id[4] || nd_id[4] == nd_id[3] || nd_id[3] == nd_id[0])
          error_message("Degenerated wedges not supported", GENERAL_ERROR);
        if(nd_id[1] == nd_id[2] || nd_id[2] == nd_id[5] || nd_id[5] == nd_id[4] || nd_id[4] == nd_id[1])
          error_message("Degenerated wedges not supported", GENERAL_ERROR);
        if(nd_id[2] == nd_id[0] || nd_id[0] == nd_id[3] || nd_id[3] == nd_id[5] || nd_id[5] == nd_id[2])
          error_message("Degenerated wedges not supported", GENERAL_ERROR);

        elem_id = local_wedge_id(elem_id);                        /* macro */
        fe_wedge = &(wedge_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == REGION_ENTITY)entity_list = region_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent wedge property", GENERAL_ERROR);
        }

        if(entity -> first_wedge == 0)entity -> first_wedge = elem_id;
        entity -> elems++;

        fe_wedge -> entity = entity;

        for(j = 0; j < 6; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid wedge node", GENERAL_ERROR);

          fe_wedge -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 9; j++){
          fe_wedge -> midnd[j] = NULL;
        }

        for(j = 0; j < 5; j++){
          fe_wedge -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_id[j] == -1)
              ent = iface_ent;
            else{
              if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
              if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
              if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
              if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
                ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            }
            fe_wedge -> data[j].bound_ent = ent;
            fe_wedge -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }

    if(hexas != 0){
      for(i = 0; i < hexas; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %ld %ld %d %d %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], &nd_id[4], &nd_id[5], &nd_id[6], &nd_id[7], 
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], &bound_ent_id[4], &bound_ent_id[5], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3], &bound_ent_tp[4], &bound_ent_tp[5]) != 24)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_hexa(elem_id) == NO)
          error_message("Inconsistent hexa numbering", GENERAL_ERROR);

        if(nd_id[0] == nd_id[1] || nd_id[1] == nd_id[2] || nd_id[2] == nd_id[3] || nd_id[3] == nd_id[0])
          error_message("Degenerated hexas not supported", GENERAL_ERROR);
        if(nd_id[4] == nd_id[5] || nd_id[5] == nd_id[6] || nd_id[6] == nd_id[7] || nd_id[7] == nd_id[4])
          error_message("Degenerated hexas not supported", GENERAL_ERROR);
        if(nd_id[0] == nd_id[1] || nd_id[1] == nd_id[5] || nd_id[5] == nd_id[4] || nd_id[4] == nd_id[0])
          error_message("Degenerated hexas not supported", GENERAL_ERROR);
        if(nd_id[1] == nd_id[2] || nd_id[2] == nd_id[6] || nd_id[6] == nd_id[5] || nd_id[5] == nd_id[1])
          error_message("Degenerated hexas not supported", GENERAL_ERROR);
        if(nd_id[2] == nd_id[3] || nd_id[3] == nd_id[7] || nd_id[7] == nd_id[6] || nd_id[6] == nd_id[2])
          error_message("Degenerated hexas not supported", GENERAL_ERROR);
        if(nd_id[3] == nd_id[0] || nd_id[0] == nd_id[4] || nd_id[4] == nd_id[7] || nd_id[7] == nd_id[3])
          error_message("Degenerated hexas not supported", GENERAL_ERROR);

        elem_id = local_hexa_id(elem_id);                        /* macro */
        fe_hexa = &(hexa_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == REGION_ENTITY)entity_list = region_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent hexa property", GENERAL_ERROR);
        }

        if(entity -> first_hexa == 0)entity -> first_hexa = elem_id;
        entity -> elems++;

        fe_hexa -> entity = entity;

        for(j = 0; j < 8; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid hexa node", GENERAL_ERROR);

          fe_hexa -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 12; j++){
          fe_hexa -> midnd[j] = NULL;
        }

        for(j = 0; j < 6; j++){
          fe_hexa -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_id[j] == -1)
              ent = iface_ent;
            else{
              if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
              if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
              if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
              if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
                ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            }
            fe_hexa -> data[j].bound_ent = ent;
            fe_hexa -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }
  }

  if(elem_degree == QUADRATIC){
    if(edges != 0){
      for(i = 0; i < edges; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %d %ld %ld", 
              &elem_id, &nd_id[0], &nd_id[1], &midnd_id[0], &entity_type, &entity_id, &property_id) != 7)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_edge(elem_id) == NO)
          error_message("Inconsistent edge numbering", GENERAL_ERROR);

        elem_id = local_edge_id(elem_id);                        /* macro */
        fe_edge = &(edge_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == CURVE_ENTITY)entity_list = curve_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent edge property", GENERAL_ERROR);
        }

        if(entity -> first_edge == 0)entity -> first_edge = elem_id;
        entity -> elems++;

        fe_edge -> entity = entity;
        for(j = 0; j < 2; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid edge node", GENERAL_ERROR);

          fe_edge -> node[j] = &(node_array[nd - 1]);
        }

        nd = midnd_id[0];
        if(entity_type == INTERFACE_ENTITY){
          if(nd == 0){
            fe_edge -> midnd[0] = NULL;
            continue;
          }
        }

        if(nd <= 0 || nd > nodes)
          error_message("Invalid edge midnode", GENERAL_ERROR);

        fe_edge -> midnd[0] = fe_nd = &(node_array[nd - 1]);
        if(fe_nd -> id > 0){
          if(first_midnd_id > fe_nd -> id || first_midnd_id == 0)first_midnd_id = fe_nd -> id;
          fe_nd -> id = -fe_nd -> id;
          midnodes++;
        }
      }
    }

    if(faces != 0){
      for(i = 0; i < faces; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &midnd_id[0], &midnd_id[1], &midnd_id[2], 
              &entity_type, &entity_id, &property_id,
              &curve_id[0], &curve_id[1], &curve_id[2]) != 13)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_face(elem_id) == NO)
          error_message("Inconsistent face numbering", GENERAL_ERROR);

        elem_id = local_face_id(elem_id);                        /* macro */
        fe_face = &(face_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == SURFACE_ENTITY)entity_list = surface_list;
          if(entity_type == PATCH_ENTITY)entity_list = patch_list;
          if(entity_type == SHELL_ENTITY)entity_list = shell_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent face property", GENERAL_ERROR);
        }

        if(entity -> first_face == 0)entity -> first_face = elem_id;
        entity -> elems++;

        fe_face -> entity = entity;

        for(j = 0; j < 3; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid face node", GENERAL_ERROR);

          fe_face -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 3; j++){
          nd = midnd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid face midnode", GENERAL_ERROR);

          fe_face -> midnd[j] = fe_nd = &(node_array[nd - 1]);
          if(fe_nd -> id > 0){
            if(first_midnd_id > fe_nd -> id || first_midnd_id == 0)first_midnd_id = fe_nd -> id;
            fe_nd -> id = -fe_nd -> id;
            midnodes++;
          }
        }

        for(j = 0; j < 3; j++){
          fe_face -> data[j].ngb_elem_id = 0;
          if(curve_id[j] != 0){
            if((ent = get_entity_id(curve_list, curve_id[j])) == NULL)
              ent = create_missing_entity_id(curve_id[j], CURVE_ENTITY);
            fe_face -> data[j].bound_ent = ent;
            fe_face -> bflag |= 1 << j;
            ent -> boundary = YES;
          }
        }
      }
    }

    if(quads != 0){
      for(i = 0; i < quads; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], 
              &midnd_id[0], &midnd_id[1], &midnd_id[2], &midnd_id[3],
              &entity_type, &entity_id, &property_id,
              &curve_id[0], &curve_id[1], &curve_id[2], &curve_id[3]) != 16)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_quad(elem_id) == NO)
          error_message("Inconsistent quad numbering", GENERAL_ERROR);

        elem_id = local_quad_id(elem_id);                        /* macro */
        fe_quad = &(quad_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == SURFACE_ENTITY)entity_list = surface_list;
          if(entity_type == PATCH_ENTITY)entity_list = patch_list;
          if(entity_type == SHELL_ENTITY)entity_list = shell_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent quad property", GENERAL_ERROR);
        }

        if(entity -> first_quad == 0)entity -> first_quad = elem_id;
        entity -> elems++;

        fe_quad -> entity = entity;

        for(j = 0; j < 4; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid quad node", GENERAL_ERROR);

          fe_quad -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 4; j++){
          nd = midnd_id[j];

          if(entity_type == INTERFACE_ENTITY){
            if(nd == 0){
              if(j == 1 || j == 3){
                fe_quad -> midnd[j] = NULL;
                continue;
              }
            }
          }

          if(nd <= 0 || nd > nodes)
            error_message("Invalid quad midnode", GENERAL_ERROR);

          fe_quad -> midnd[j] = fe_nd = &(node_array[nd - 1]);
          if(fe_nd -> id > 0){
            if(first_midnd_id > fe_nd -> id || first_midnd_id == 0)first_midnd_id = fe_nd -> id;
            fe_nd -> id = -fe_nd -> id;
            midnodes++;
          }
        }

        for(j = 0; j < 4; j++){
          fe_quad -> data[j].ngb_elem_id = 0;
          if(curve_id[j] != 0){
            if((ent = get_entity_id(curve_list, curve_id[j])) == NULL)
              ent = create_missing_entity_id(curve_id[j], CURVE_ENTITY);
            fe_quad -> data[j].bound_ent = ent;
            fe_quad -> bflag |= 1 << j;
            ent -> boundary = YES;
          }
        }
      }
    }

    if(tetras != 0){
      for(i = 0; i < tetras; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], 
              &midnd_id[0], &midnd_id[1], &midnd_id[2], &midnd_id[3], &midnd_id[4], &midnd_id[5],
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3]) != 22)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_tetra(elem_id) == NO)
          error_message("Inconsistent tetra numbering", GENERAL_ERROR);

        elem_id = local_tetra_id(elem_id);                       /* macro */
        fe_tetra = &(tetra_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          entity = get_entity_id(region_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent tetra property", GENERAL_ERROR);
        }

        if(entity -> first_tetra == 0)entity -> first_tetra = elem_id;
        entity -> elems++;

        fe_tetra -> entity = entity;

        for(j = 0; j < 4; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid tetra node", GENERAL_ERROR);

          fe_tetra -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 6; j++){
          nd = midnd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid tetra midnode", GENERAL_ERROR);

          fe_tetra -> midnd[j] = fe_nd = &(node_array[nd - 1]);
          if(fe_nd -> id > 0){
            if(first_midnd_id > fe_nd -> id || first_midnd_id == 0)first_midnd_id = fe_nd -> id;
            fe_nd -> id = -fe_nd -> id;
            midnodes++;
          }
        }

        for(j = 0; j < 4; j++){
          fe_tetra -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
            if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
            if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
            if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
              ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            fe_tetra -> data[j].bound_ent = ent;
            fe_tetra -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }

    if(pyrams != 0){
      for(i = 0; i < pyrams; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %ld %d %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], &nd_id[4], 
              &midnd_id[0], &midnd_id[1], &midnd_id[2], &midnd_id[3], &midnd_id[4], &midnd_id[5], &midnd_id[6], &midnd_id[7],
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], &bound_ent_id[4], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3], &bound_ent_tp[4]) != 27)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_pyram(elem_id) == NO)
          error_message("Inconsistent pyram numbering", GENERAL_ERROR);

        elem_id = local_pyram_id(elem_id);                        /* macro */
        fe_pyram = &(pyram_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          entity = get_entity_id(region_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent pyram property", GENERAL_ERROR);
        }

        if(entity -> first_pyram == 0)entity -> first_pyram = elem_id;
        entity -> elems++;

        fe_pyram -> entity = entity;

        for(j = 0; j < 5; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid pyram node", GENERAL_ERROR);

          fe_pyram -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 8; j++){
          nd = midnd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid pyram midnode", GENERAL_ERROR);

          fe_pyram -> midnd[j] = fe_nd = &(node_array[nd - 1]);
          if(fe_nd -> id > 0){
            if(first_midnd_id > fe_nd -> id || first_midnd_id == 0)first_midnd_id = fe_nd -> id;
            fe_nd -> id = -fe_nd -> id;
            midnodes++;
          }
        }

        for(j = 0; j < 5; j++){
          fe_pyram -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
            if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
            if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
            if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
              ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            fe_pyram -> data[j].bound_ent = ent;
            fe_pyram -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }

    if(wedges != 0){
      for(i = 0; i < wedges; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %ld %d %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], &nd_id[4], &nd_id[5], 
              &midnd_id[0], &midnd_id[1], &midnd_id[2], &midnd_id[3], &midnd_id[4], 
              &midnd_id[5], &midnd_id[6], &midnd_id[7], &midnd_id[8],
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], &bound_ent_id[4], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3], &bound_ent_tp[4]) != 29)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_wedge(elem_id) == NO)
          error_message("Inconsistent wedge numbering", GENERAL_ERROR);

        elem_id = local_wedge_id(elem_id);                        /* macro */
        fe_wedge = &(wedge_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == REGION_ENTITY)entity_list = region_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent wedge property", GENERAL_ERROR);
        }

        if(entity -> first_wedge == 0)entity -> first_wedge = elem_id;
        entity -> elems++;

        fe_wedge -> entity = entity;

        for(j = 0; j < 6; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid wedge node", GENERAL_ERROR);

          fe_wedge -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 9; j++){
          nd = midnd_id[j];

          if(entity_type == INTERFACE_ENTITY){
            if(nd == 0){
              if(j > 5){
                fe_wedge -> midnd[j] = NULL;
                continue;
              }
            }
          }

          if(nd <= 0 || nd > nodes)
            error_message("Invalid wedge midnode", GENERAL_ERROR);

          fe_wedge -> midnd[j] = fe_nd = &(node_array[nd - 1]);
          if(fe_nd -> id > 0){
            if(first_midnd_id > fe_nd -> id || first_midnd_id == 0)first_midnd_id = fe_nd -> id;
            fe_nd -> id = -fe_nd -> id;
            midnodes++;
          }
        }

        for(j = 0; j < 5; j++){
          fe_wedge -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
            if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
            if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
            if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
              ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            fe_wedge -> data[j].bound_ent = ent;
            fe_wedge -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }

    if(hexas != 0){
      for(i = 0; i < hexas; i++){
        get_next_relevant_record(in_err_msg);
        if(sscanf(line_buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld %ld %ld %d %d %d %d %d %d", 
              &elem_id, &nd_id[0], &nd_id[1], &nd_id[2], &nd_id[3], &nd_id[4], &nd_id[5], &nd_id[6], &nd_id[7], 
              &midnd_id[0], &midnd_id[1], &midnd_id[2], &midnd_id[3], &midnd_id[4], &midnd_id[5], 
              &midnd_id[6], &midnd_id[7], &midnd_id[8], &midnd_id[9], &midnd_id[10], &midnd_id[11],
              &entity_type, &entity_id, &property_id,
              &bound_ent_id[0], &bound_ent_id[1], &bound_ent_id[2], &bound_ent_id[3], &bound_ent_id[4], &bound_ent_id[5], 
              &bound_ent_tp[0], &bound_ent_tp[1], &bound_ent_tp[2], &bound_ent_tp[3], &bound_ent_tp[4], &bound_ent_tp[5]) != 36)
          error_message(in_err_msg, FILE_READ_ERROR);

        if(is_hexa(elem_id) == NO)
          error_message("Inconsistent hexa numbering", GENERAL_ERROR);

        elem_id = local_hexa_id(elem_id);                        /* macro */
        fe_hexa = &(hexa_array[elem_id - 1]);

        if(last_entity_type != entity_type || last_entity_id != entity_id){
          if(entity_type == REGION_ENTITY)entity_list = region_list;
          if(entity_type == INTERFACE_ENTITY)entity_list = interface_list;
          entity = get_entity_id(entity_list, entity_id);
          if(entity == NULL){
            entity = create_missing_entity_id(entity_id, entity_type);
            entity -> property_id = property_id;
          }
          last_entity_type = entity_type;
          last_entity_id = entity_id;
          entity -> output = YES;
        }
        else{
          if(entity -> property_id != property_id)
            error_message("Inconsistent hexa property", GENERAL_ERROR);
        }

        if(entity -> first_hexa == 0)entity -> first_hexa = elem_id;
        entity -> elems++;

        fe_hexa -> entity = entity;

        for(j = 0; j < 8; j++){
          nd = nd_id[j];
          if(nd <= 0 || nd > nodes)
            error_message("Invalid hexa node", GENERAL_ERROR);

          fe_hexa -> node[j] = &(node_array[nd - 1]);
        }
        for(j = 0; j < 12; j++){
          nd = midnd_id[j];

          if(entity_type == INTERFACE_ENTITY){
            if(nd == 0){
              if(j > 7){
                fe_hexa -> midnd[j] = NULL;
                continue;
              }
            }
          }

          if(nd <= 0 || nd > nodes)
            error_message("Invalid hexa midnode", GENERAL_ERROR);

          fe_hexa -> midnd[j] = fe_nd = &(node_array[nd - 1]);
          if(fe_nd -> id > 0){
            if(first_midnd_id > fe_nd -> id || first_midnd_id == 0)first_midnd_id = fe_nd -> id;
            fe_nd -> id = -fe_nd -> id;
            midnodes++;
          }
        }

        for(j = 0; j < 6; j++){
          fe_hexa -> data[j].ngb_elem_id = 0;
          if(bound_ent_id[j] != 0){
            if(bound_ent_tp[j] == SURFACE_ENTITY)entity_list = surface_list;
            if(bound_ent_tp[j] == PATCH_ENTITY)entity_list = patch_list;
            if(bound_ent_tp[j] == SHELL_ENTITY)entity_list = shell_list;
            if((ent = get_entity_id(entity_list, bound_ent_id[j])) == NULL)
              ent = create_missing_entity_id(bound_ent_id[j], bound_ent_tp[j]);
            fe_hexa -> data[j].bound_ent = ent;
            fe_hexa -> bflag |= 1 << j;
            ent -> boundary = YES;

            /* I do not need ngb 3D entities for 2D entity without elements;

               this is used to prevent "Unexpected situation" below 
               when a 2D entity is shared by more than just two 3D entities;
               this may happen if slave 2D entities bounding a 3D entity are used in t3d model;
               I also rely on the fact that elements are read in the order of ascending dimension
               and that the mesh is either linear or quadratic */

            if(ent -> first_face == 0 && ent -> first_quad == 0)continue;
            if(ent -> ngb_ent[0] == NULL)
              ent -> ngb_ent[0] = entity;
            else{
              if(ent -> ngb_ent[0] != entity){
                if(ent -> ngb_ent[1] == NULL)
                  ent -> ngb_ent[1] = entity;
                else{
                  if(ent -> ngb_ent[1] != entity)
                    error_message("Unexpected situation", GENERAL_ERROR);
                }
              }
            }
          }
        }
      }
    }

    /* IMPORTANT: check that midnodes are at the end of node list */

    if(first_midnd_id + midnodes != nodes + 1)
      error_message("Unsupported node ordering", GENERAL_ERROR);

    nodes -= midnodes;

    fe_Node = node_array + nodes;
    for(i = 0; i < midnodes; i++, fe_Node++)fe_Node -> id = -fe_Node -> id;
  }

  fclose(t3d_out_file);

  if(elem_weight == YES){
    if((weight_file = fopen(weight_name, "r")) == NULL){
      sprintf(err_msg, "File %s opening error", weight_name);
      error_message(err_msg, FILE_OPEN_ERROR);
    }
    active_in_file = weight_file;
    sprintf(in_err_msg, "File %s reading error", weight_name);

    while((token = get_next_first_token(NULL)) != NULL){
      key = get_token_key(token, keyword, KEY_NUM, KEY_CASE);

      switch(key){
        case VERTEX:
        case CURVE:
        case SURFACE:
        case PATCH:
        case SHELL:
        case REGION:
        case INTERFACE:
          if(entity_select == YES)
            error_message("Entity specification not expected", GENERAL_ERROR);
          entity_select = YES;
          specification = NO;
          break;
        case WEIGHT:
          if(entity_select == NO)
            error_message("Entity specification missing", GENERAL_ERROR);
          specification = YES;
          entity_select = NO;
          break;
        default:
          sprintf(err_msg, "Unexpected or invalid keyword \"%s\" encountered", token);
          error_message(err_msg, GENERAL_ERROR);
      }

      switch(key){
        case VERTEX:
        case CURVE:
        case SURFACE:
        case PATCH:
        case SHELL:
        case REGION:
        case INTERFACE:
          entity_key = key;            /* must preced call to process_entity_list */
          process_entity_list();
          break;
        case WEIGHT:
          switch(entity_key){
            case VERTEX:
              error_message("Keyword WEIGHT not expected for vertex specification", GENERAL_ERROR);
              break;
            case CURVE:
              edge_weight = get_next_fpn("Edge weight specification missing");
              if(edge_weight < 0.0)
                error_message("Negativ weight not allowed", GENERAL_ERROR);
              break;
            case SURFACE:
            case PATCH:
            case SHELL:
              face_weight = get_next_fpn("Face weight specification missing");
              quad_weight = get_next_fpn("Quad weight specification missing");
              if(face_weight < 0.0 || quad_weight < 0.0)
                error_message("Negativ weight not allowed", GENERAL_ERROR);
              break;
            case REGION:
              tetra_weight = get_next_fpn("Tetra weight specification missing");
              pyram_weight = get_next_fpn("Pyram weight specification missing");
              wedge_weight = get_next_fpn("Wedge weight specification missing");
              hexa_weight = get_next_fpn("Hexa weight specification missing");
              if(tetra_weight < 0.0 || pyram_weight < 0.0 || wedge_weight < 0.0 || hexa_weight < 0.0)
                error_message("Negativ weight not allowed", GENERAL_ERROR);
              break;
            case INTERFACE:
              edge_weight = get_next_fpn("Edge weight specification missing");
              quad_weight = get_next_fpn("Quad weight specification missing");
              wedge_weight = get_next_fpn("Wedge weight specification missing");
              hexa_weight = get_next_fpn("Hexa weight specification missing");
              if(edge_weight < 0.0 || quad_weight < 0.0 || wedge_weight < 0.0 || hexa_weight < 0.0)
                error_message("Negativ weight not allowed", GENERAL_ERROR);
              break;
            default:
              break;
          }
          apply_weight();
          break;
        default:
          sprintf(err_msg, "Unexpected or invalid keyword \"%s\" encountered", token);
          error_message(err_msg, GENERAL_ERROR);
      }
    }
  }

  if(renum == YES || nparts > 1){
    if((node_num_elems = (long *)calloc(nodes + midnodes + 1, sizeof(long))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    /* count number of elements incident to nodes (this can be done when reading the mesh) */

    fe_edge = edge_array;
    for(i = 0; i < edges; i++, fe_edge++){
      for(j = 0; j < 2; j++){
        nd = fe_edge -> node[j] -> id;
        node_num_elems[nd - 1]++;
      }
    }

    fe_face = face_array;
    for(i = 0; i < faces; i++, fe_face++){
      for(j = 0; j < 3; j++){
        nd = fe_face -> node[j] -> id;
        node_num_elems[nd - 1]++;
      }
    }

    fe_quad = quad_array;
    for(i = 0; i < quads; i++, fe_quad++){
      for(j = 0; j < 4; j++){
        nd = fe_quad -> node[j] -> id;
        node_num_elems[nd - 1]++;
      }
    }

    fe_tetra = tetra_array;
    for(i = 0; i < tetras; i++, fe_tetra++){
      for(j = 0; j < 4; j++){
        nd = fe_tetra -> node[j] -> id;
        node_num_elems[nd - 1]++;
      }
    }

    fe_pyram = pyram_array;
    for(i = 0; i < pyrams; i++, fe_pyram++){
      for(j = 0; j < 5; j++){
        nd = fe_pyram -> node[j] -> id;
        node_num_elems[nd - 1]++;
      }
    }

    fe_wedge = wedge_array;
    for(i = 0; i < wedges; i++, fe_wedge++){
      for(j = 0; j < 6; j++){
        nd = fe_wedge -> node[j] -> id;
        node_num_elems[nd - 1]++;
      }
    }

    fe_hexa = hexa_array;
    for(i = 0; i < hexas; i++, fe_hexa++){
      for(j = 0; j < 8; j++){
        nd = fe_hexa -> node[j] -> id;
        node_num_elems[nd - 1]++;
      }
    }

    if(midnodes != 0){

      /* count number of elements incident to midnodes (this can be done when reading the mesh) */

      fe_edge = edge_array;
      for(i = 0; i < edges; i++, fe_edge++){
        if(fe_edge -> midnd[0] == NULL)continue;               /* interface */
        nd = fe_edge -> midnd[0] -> id;
        node_num_elems[nd - 1]++;
      }

      fe_face = face_array;
      for(i = 0; i < faces; i++, fe_face++){
        for(j = 0; j < 3; j++){
          nd = fe_face -> midnd[j] -> id;
          node_num_elems[nd - 1]++;
        }
      }

      fe_quad = quad_array;
      for(i = 0; i < quads; i++, fe_quad++){
        for(j = 0; j < 4; j++){
          if(fe_quad -> midnd[j] == NULL)continue;               /* interface */
          nd = fe_quad -> midnd[j] -> id;
          node_num_elems[nd - 1]++;
        }
      }

      fe_tetra = tetra_array;
      for(i = 0; i < tetras; i++, fe_tetra++){
        for(j = 0; j < 6; j++){
          nd = fe_tetra -> midnd[j] -> id;
          node_num_elems[nd - 1]++;
        }
      }

      fe_pyram = pyram_array;
      for(i = 0; i < pyrams; i++, fe_pyram++){
        for(j = 0; j < 8; j++){
          nd = fe_pyram -> midnd[j] -> id;
          node_num_elems[nd - 1]++;
        }
      }

      fe_wedge = wedge_array;
      for(i = 0; i < wedges; i++, fe_wedge++){
        for(j = 0; j < 9; j++){
          if(fe_wedge -> midnd[j] == NULL)continue;               /* interface */
          nd = fe_wedge -> midnd[j] -> id;
          node_num_elems[nd - 1]++;
        }
      }

      fe_hexa = hexa_array;
      for(i = 0; i < hexas; i++, fe_hexa++){
        for(j = 0; j < 12; j++){
          if(fe_hexa -> midnd[j] == NULL)continue;               /* interface */
          nd = fe_hexa -> midnd[j] -> id;
          node_num_elems[nd - 1]++;
        }
      }
    }

    /* recalculate the number of elements to current addresses */

    pos = 0;
    for(i = 0; i < nodes + midnodes; i++){
      number = node_num_elems[i];
      node_num_elems[i] = pos;
      pos += number;
    }

    node_num_elems[nodes + midnodes] = size = pos;
    if((node_con_elems = (long *)calloc(size, sizeof(long))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    /* store element numbers incident to nodes */

    fe_edge = edge_array;
    for(i = 0; i < edges; i++, fe_edge++){
      for(j = 0; j < 2; j++){
        nd = fe_edge -> node[j] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1;
      }
    }

    fe_face = face_array;
    for(i = 0; i < faces; i++, fe_face++){
      for(j = 0; j < 3; j++){
        nd = fe_face -> node[j] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges;
      }
    }

    fe_quad = quad_array;
    for(i = 0; i < quads; i++, fe_quad++){
      for(j = 0; j < 4; j++){
        nd = fe_quad -> node[j] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces;
      }
    }

    fe_tetra = tetra_array;
    for(i = 0; i < tetras; i++, fe_tetra++){
      for(j = 0; j < 4; j++){
        nd = fe_tetra -> node[j] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads;
      }
    }

    fe_pyram = pyram_array;
    for(i = 0; i < pyrams; i++, fe_pyram++){
      for(j = 0; j < 5; j++){
        nd = fe_pyram -> node[j] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads + tetras;
      }
    }

    fe_wedge = wedge_array;
    for(i = 0; i < wedges; i++, fe_wedge++){
      for(j = 0; j < 6; j++){
        nd = fe_wedge -> node[j] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads + tetras + pyrams;
      }
    }

    fe_hexa = hexa_array;
    for(i = 0; i < hexas; i++, fe_hexa++){
      for(j = 0; j < 8; j++){
        nd = fe_hexa -> node[j] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads + tetras + pyrams + wedges;
      }
    }

    if(midnodes != 0){

      /* store element numbers incident to midnodes */

      fe_edge = edge_array;
      for(i = 0; i < edges; i++, fe_edge++){
        if(fe_edge -> midnd[0] == NULL)continue;               /* interface */
        nd = fe_edge -> midnd[0] -> id;
        node_con_elems[node_num_elems[nd - 1]++] = i + 1;
      }

      fe_face = face_array;
      for(i = 0; i < faces; i++, fe_face++){
        for(j = 0; j < 3; j++){
          nd = fe_face -> midnd[j] -> id;
          node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges;
        }
      }

      fe_quad = quad_array;
      for(i = 0; i < quads; i++, fe_quad++){
        for(j = 0; j < 4; j++){
          if(fe_quad -> midnd[j] == NULL)continue;               /* interface */
          nd = fe_quad -> midnd[j] -> id;
          node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces;
        }
      }

      fe_tetra = tetra_array;
      for(i = 0; i < tetras; i++, fe_tetra++){
        for(j = 0; j < 6; j++){
          nd = fe_tetra -> midnd[j] -> id;
          node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads;
        }
      }

      fe_pyram = pyram_array;
      for(i = 0; i < pyrams; i++, fe_pyram++){
        for(j = 0; j < 8; j++){
          nd = fe_pyram -> midnd[j] -> id;
          node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads + tetras;
        }
      }

      fe_wedge = wedge_array;
      for(i = 0; i < wedges; i++, fe_wedge++){
        for(j = 0; j < 9; j++){
          if(fe_wedge -> midnd[j] == NULL)continue;               /* interface */
          nd = fe_wedge -> midnd[j] -> id;
          node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads + tetras + pyrams;
        }
      }

      fe_hexa = hexa_array;
      for(i = 0; i < hexas; i++, fe_hexa++){
        for(j = 0; j < 12; j++){
          if(fe_hexa -> midnd[j] == NULL)continue;               /* interface */
          nd = fe_hexa -> midnd[j] -> id;
          node_con_elems[node_num_elems[nd - 1]++] = i + 1 + edges + faces + quads + tetras + pyrams + wedges;
        }
      }
    }

    /* recalculate the addresses to address of the first element */

    pos = 0;
    for(i = 0; i < nodes + midnodes; i++){
      number = node_num_elems[i] - pos;
      node_num_elems[i] = pos;
      pos += number;
    }
  }

  if(renum == YES){
    if((nd_num_nodes = (long *)calloc(nodes + midnodes + 1, sizeof(long))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    /* count number of nodes incident to nodes (including midside nodes);

       this actually duplicates node_num_nodes and node_con_nodes;
       this seems necessary because node_num_nodes and node_con_nodes
       are of type idxtype, but nd_con_nodes which are part of nd_array
       passed to renumber_mesh must be of type long;
       I cannot rely on the fact that these two types are same;
       moreover node_num_nodes and node_con_nodes are built for elem-cut only;
       another reason to do it separately is when quadratic elements are handled */

    for(i = 0; i < nodes + midnodes; i++){
      node = i + 1;
      for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
        elem = node_con_elems[j];
        type = elem_type(elem);                              /* macro */
        switch(type){
          case EDGE_ELEM:
            elem = local_edge_id(elem);                        /* macro */
            fe_edge = &(edge_array[elem - 1]);
            for(k = 0; k < 2; k++){
              nd = fe_edge -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_edge -> node[k] -> id = -nd;
                nd_num_nodes[i]++;
              }
            }
            if(midnodes != 0){
              if(fe_edge -> midnd[0] != NULL){               /* not interface */
                nd = fe_edge -> midnd[0] -> id;
                if(nd != node && nd > 0){
                  fe_edge -> midnd[0] -> id = -nd;
                  nd_num_nodes[i]++;
                }
              }
            }
            break;
          case FACE_ELEM:
            elem = local_face_id(elem);                        /* macro */
            fe_face = &(face_array[elem - 1]);
            for(k = 0; k < 3; k++){
              nd = fe_face -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_face -> node[k] -> id = -nd;
                nd_num_nodes[i]++;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 3; k++){
                nd = fe_face -> midnd[k] -> id;
                if(nd != node && nd > 0){
                  fe_face -> midnd[k] -> id = -nd;
                  nd_num_nodes[i]++;
                }
              }
            }
            break;
          case QUAD_ELEM:
            elem = local_quad_id(elem);                        /* macro */
            fe_quad = &(quad_array[elem - 1]);
            for(k = 0; k < 4; k++){
              nd = fe_quad -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_quad -> node[k] -> id = -nd;
                nd_num_nodes[i]++;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 4; k++){
                if(fe_quad -> midnd[k] != NULL){               /* not interface */
                  nd = fe_quad -> midnd[k] -> id;
                  if(nd != node && nd > 0){
                    fe_quad -> midnd[k] -> id = -nd;
                    nd_num_nodes[i]++;
                  }
                }
              }
            }
            break;
          case TETRA_ELEM:
            elem = local_tetra_id(elem);                       /* macro */
            fe_tetra = &(tetra_array[elem - 1]);
            for(k = 0; k < 4; k++){
              nd = fe_tetra -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_tetra -> node[k] -> id = -nd;
                nd_num_nodes[i]++;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 6; k++){
                nd = fe_tetra -> midnd[k] -> id;
                if(nd != node && nd > 0){
                  fe_tetra -> midnd[k] -> id = -nd;
                  nd_num_nodes[i]++;
                }
              }
            }
            break;
          case PYRAM_ELEM:
            elem = local_pyram_id(elem);                       /* macro */
            fe_pyram = &(pyram_array[elem - 1]);
            for(k = 0; k < 5; k++){
              nd = fe_pyram -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_pyram -> node[k] -> id = -nd;
                nd_num_nodes[i]++;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 8; k++){
                nd = fe_pyram -> midnd[k] -> id;
                if(nd != node && nd > 0){
                  fe_pyram -> midnd[k] -> id = -nd;
                  nd_num_nodes[i]++;
                }
              }
            }
            break;
          case WEDGE_ELEM:
            elem = local_wedge_id(elem);                       /* macro */
            fe_wedge = &(wedge_array[elem - 1]);
            for(k = 0; k < 6; k++){
              nd = fe_wedge -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_wedge -> node[k] -> id = -nd;
                nd_num_nodes[i]++;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 9; k++){
                if(fe_wedge -> midnd[k] != NULL){               /* not interface */
                  nd = fe_wedge -> midnd[k] -> id;
                  if(nd != node && nd > 0){
                    fe_wedge -> midnd[k] -> id = -nd;
                    nd_num_nodes[i]++;
                  }
                }
              }
            }
            break;
          case HEXA_ELEM:
            elem = local_hexa_id(elem);                        /* macro */
            fe_hexa = &(hexa_array[elem - 1]);
            for(k = 0; k < 8; k++){
              nd = fe_hexa -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_hexa -> node[k] -> id = -nd;
                nd_num_nodes[i]++;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 12; k++){
                if(fe_hexa -> midnd[k] != NULL){               /* not interface */
                  nd = fe_hexa -> midnd[k] -> id;
                  if(nd != node && nd > 0){
                    fe_hexa -> midnd[k] -> id = -nd;
                    nd_num_nodes[i]++;
                  }
                }
              }
            }
            break;
          default:
            error_message("Invalid element number", GENERAL_ERROR);
            break;
        }
      }

      /* restore connected node id to positive value */

      for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
        elem = node_con_elems[j];
        type = elem_type(elem);                              /* macro */
        switch(type){
          case EDGE_ELEM:
            elem = local_edge_id(elem);                        /* macro */
            fe_edge = &(edge_array[elem - 1]);
            for(k = 0; k < 2; k++)fe_edge -> node[k] -> id = abs(fe_edge -> node[k] -> id);
            if(midnodes != 0){
              if(fe_edge -> midnd[0] != NULL){               /* not interface */
                fe_edge -> midnd[0] -> id = abs(fe_edge -> midnd[0] -> id);
              }
            }
            break;
          case FACE_ELEM:
            elem = local_face_id(elem);                        /* macro */
            fe_face = &(face_array[elem - 1]);
            for(k = 0; k < 3; k++)fe_face -> node[k] -> id = abs(fe_face -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 3; k++)fe_face -> midnd[k] -> id = abs(fe_face -> midnd[k] -> id);
            }
            break;
          case QUAD_ELEM:						
            elem = local_quad_id(elem);                        /* macro */
            fe_quad = &(quad_array[elem - 1]);
            for(k = 0; k < 4; k++)fe_quad -> node[k] -> id = abs(fe_quad -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 4; k++){
                if(fe_quad -> midnd[k] != NULL){               /* not interface */
                  fe_quad -> midnd[k] -> id = abs(fe_quad -> midnd[k] -> id);
                }
              }
            }
            break;
          case TETRA_ELEM:
            elem = local_tetra_id(elem);                       /* macro */
            fe_tetra = &(tetra_array[elem - 1]);
            for(k = 0; k < 4; k++)fe_tetra -> node[k] -> id = abs(fe_tetra -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 6; k++)fe_tetra -> midnd[k] -> id = abs(fe_tetra -> midnd[k] -> id);
            }
            break;
          case PYRAM_ELEM:
            elem = local_pyram_id(elem);                       /* macro */
            fe_pyram = &(pyram_array[elem - 1]);
            for(k = 0; k < 5; k++)fe_pyram -> node[k] -> id = abs(fe_pyram -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 8; k++)fe_pyram -> midnd[k] -> id = abs(fe_pyram -> midnd[k] -> id);
            }
            break;
          case WEDGE_ELEM:
            elem = local_wedge_id(elem);                       /* macro */
            fe_wedge = &(wedge_array[elem - 1]);
            for(k = 0; k < 6; k++)fe_wedge -> node[k] -> id = abs(fe_wedge -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 9; k++){
                if(fe_wedge -> midnd[k] != NULL){               /* not interface */
                  fe_wedge -> midnd[k] -> id = abs(fe_wedge -> midnd[k] -> id);
                }
              }
            }
            break;
          case HEXA_ELEM:
            elem = local_hexa_id(elem);                        /* macro */
            fe_hexa = &(hexa_array[elem - 1]);
            for(k = 0; k < 8; k++)fe_hexa -> node[k] -> id = abs(fe_hexa -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 12; k++){
                if(fe_hexa -> midnd[k] != NULL){               /* not interface */
                  fe_hexa -> midnd[k] -> id = abs(fe_hexa -> midnd[k] -> id);
                }
              }
            }
            break;
          default:
            error_message("Invalid element number", GENERAL_ERROR);
            break;
        }
      }
    }

    /* recalculate the number of nodes to current addresses */

    pos = 0;
    for(i = 0; i < nodes + midnodes; i++){
      number = nd_num_nodes[i];
      nd_num_nodes[i] = pos;
      pos += number;
    }

    nd_num_nodes[nodes + midnodes] = size = pos;
    if((nd_con_nodes = (long *)calloc(size, sizeof(long))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
    if(nparts > 1){
      if((nd_con_nds = (long *)calloc(size, sizeof(long))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);
    }

    /* store nodes incident to nodes */

    for(i = 0; i < nodes + midnodes; i++){
      node = i + 1;
      for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
        elem = node_con_elems[j];
        type = elem_type(elem);                              /* macro */
        switch(type){
          case EDGE_ELEM:
            elem = local_edge_id(elem);                        /* macro */
            fe_edge = &(edge_array[elem - 1]);
            for(k = 0; k < 2; k++){
              nd = fe_edge -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_edge -> node[k] -> id = -nd;
                nd_con_nodes[nd_num_nodes[i]++] = nd;
              }
            }
            if(midnodes != 0){
              if(fe_edge -> midnd[0] != NULL){               /* not interface */
                nd = fe_edge -> midnd[0] -> id;
                if(nd != node && nd > 0){
                  fe_edge -> midnd[0] -> id = -nd;
                  nd_con_nodes[nd_num_nodes[i]++] = nd;
                }
              }
            }
            break;
          case FACE_ELEM:
            elem = local_face_id(elem);                        /* macro */
            fe_face = &(face_array[elem - 1]);
            for(k = 0; k < 3; k++){
              nd = fe_face -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_face -> node[k] -> id = -nd;
                nd_con_nodes[nd_num_nodes[i]++] = nd;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 3; k++){
                nd = fe_face -> midnd[k] -> id;
                if(nd != node && nd > 0){
                  fe_face -> midnd[k] -> id = -nd;
                  nd_con_nodes[nd_num_nodes[i]++] = nd;
                }
              }
            }
            break;
          case QUAD_ELEM:
            elem = local_quad_id(elem);                        /* macro */
            fe_quad = &(quad_array[elem - 1]);
            for(k = 0; k < 4; k++){
              nd = fe_quad -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_quad -> node[k] -> id = -nd;
                nd_con_nodes[nd_num_nodes[i]++] = nd;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 4; k++){
                if(fe_quad -> midnd[k] != NULL){               /* not interface */
                  nd = fe_quad -> midnd[k] -> id;
                  if(nd != node && nd > 0){
                    fe_quad -> midnd[k] -> id = -nd;
                    nd_con_nodes[nd_num_nodes[i]++] = nd;
                  }
                }
              }
            }
            break;
          case TETRA_ELEM:
            elem = local_tetra_id(elem);                       /* macro */
            fe_tetra = &(tetra_array[elem - 1]);
            for(k = 0; k < 4; k++){
              nd = fe_tetra -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_tetra -> node[k] -> id = -nd;
                nd_con_nodes[nd_num_nodes[i]++] = nd;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 6; k++){
                nd = fe_tetra -> midnd[k] -> id;
                if(nd != node && nd > 0){
                  fe_tetra -> midnd[k] -> id = -nd;
                  nd_con_nodes[nd_num_nodes[i]++] = nd;
                }
              }
            }
            break;
          case PYRAM_ELEM:
            elem = local_pyram_id(elem);                       /* macro */
            fe_pyram = &(pyram_array[elem - 1]);
            for(k = 0; k < 5; k++){
              nd = fe_pyram -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_pyram -> node[k] -> id = -nd;
                nd_con_nodes[nd_num_nodes[i]++] = nd;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 8; k++){
                nd = fe_pyram -> midnd[k] -> id;
                if(nd != node && nd > 0){
                  fe_pyram -> midnd[k] -> id = -nd;
                  nd_con_nodes[nd_num_nodes[i]++] = nd;
                }
              }
            }
            break;
          case WEDGE_ELEM:
            elem = local_wedge_id(elem);                       /* macro */
            fe_wedge = &(wedge_array[elem - 1]);
            for(k = 0; k < 6; k++){
              nd = fe_wedge -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_wedge -> node[k] -> id = -nd;
                nd_con_nodes[nd_num_nodes[i]++] = nd;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 9; k++){
                if(fe_wedge -> midnd[k] != NULL){               /* not interface */
                  nd = fe_wedge -> midnd[k] -> id;
                  if(nd != node && nd > 0){
                    fe_wedge -> midnd[k] -> id = -nd;
                    nd_con_nodes[nd_num_nodes[i]++] = nd;
                  }
                }
              }
            }
            break;
          case HEXA_ELEM:
            elem = local_hexa_id(elem);                        /* macro */
            fe_hexa = &(hexa_array[elem - 1]);
            for(k = 0; k < 8; k++){
              nd = fe_hexa -> node[k] -> id;
              if(nd != node && nd > 0){
                fe_hexa -> node[k] -> id = -nd;
                nd_con_nodes[nd_num_nodes[i]++] = nd;
              }
            }
            if(midnodes != 0){
              for(k = 0; k < 12; k++){
                if(fe_hexa -> midnd[k] != NULL){               /* not interface */
                  nd = fe_hexa -> midnd[k] -> id;
                  if(nd != node && nd > 0){
                    fe_hexa -> midnd[k] -> id = -nd;
                    nd_con_nodes[nd_num_nodes[i]++] = nd;
                  }
                }
              }
            }
            break;
          default:
            error_message("Invalid element number", GENERAL_ERROR);
            break;
        }
      }

      /* restore connected node id to positive value */

      for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
        elem = node_con_elems[j];
        type = elem_type(elem);                              /* macro */
        switch(type){
          case EDGE_ELEM:
            elem = local_edge_id(elem);                        /* macro */
            fe_edge = &(edge_array[elem - 1]);
            for(k = 0; k < 2; k++)fe_edge -> node[k] -> id = abs(fe_edge -> node[k] -> id);
            if(midnodes != 0){
              if(fe_edge -> midnd[0] != NULL){               /* not interface */
                fe_edge -> midnd[0] -> id = abs(fe_edge -> midnd[0] -> id);
              }
            }
            break;
          case FACE_ELEM:
            elem = local_face_id(elem);                        /* macro */
            fe_face = &(face_array[elem - 1]);
            for(k = 0; k < 3; k++)fe_face -> node[k] -> id = abs(fe_face -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 3; k++)fe_face -> midnd[k] -> id = abs(fe_face -> midnd[k] -> id);
            }
            break;
          case QUAD_ELEM:						
            elem = local_quad_id(elem);                        /* macro */
            fe_quad = &(quad_array[elem - 1]);
            for(k = 0; k < 4; k++)fe_quad -> node[k] -> id = abs(fe_quad -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 4; k++){
                if(fe_quad -> midnd[k] != NULL){               /* not interface */
                  fe_quad -> midnd[k] -> id = abs(fe_quad -> midnd[k] -> id);
                }
              }
            }
            break;
          case TETRA_ELEM:
            elem = local_tetra_id(elem);                       /* macro */
            fe_tetra = &(tetra_array[elem - 1]);
            for(k = 0; k < 4; k++)fe_tetra -> node[k] -> id = abs(fe_tetra -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 6; k++)fe_tetra -> midnd[k] -> id = abs(fe_tetra -> midnd[k] -> id);
            }
            break;
          case PYRAM_ELEM:
            elem = local_pyram_id(elem);                       /* macro */
            fe_pyram = &(pyram_array[elem - 1]);
            for(k = 0; k < 5; k++)fe_pyram -> node[k] -> id = abs(fe_pyram -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 8; k++)fe_pyram -> midnd[k] -> id = abs(fe_pyram -> midnd[k] -> id);
            }
            break;
          case WEDGE_ELEM:
            elem = local_wedge_id(elem);                       /* macro */
            fe_wedge = &(wedge_array[elem - 1]);
            for(k = 0; k < 6; k++)fe_wedge -> node[k] -> id = abs(fe_wedge -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 9; k++){
                if(fe_wedge -> midnd[k] != NULL){               /* not interface */
                  fe_wedge -> midnd[k] -> id = abs(fe_wedge -> midnd[k] -> id);
                }
              }
            }
            break;
          case HEXA_ELEM:
            elem = local_hexa_id(elem);                        /* macro */
            fe_hexa = &(hexa_array[elem - 1]);
            for(k = 0; k < 8; k++)fe_hexa -> node[k] -> id = abs(fe_hexa -> node[k] -> id);
            if(midnodes != 0){
              for(k = 0; k < 12; k++){
                if(fe_hexa -> midnd[k] != NULL){               /* not interface */
                  fe_hexa -> midnd[k] -> id = abs(fe_hexa -> midnd[k] -> id);
                }
              }
            }
            break;
          default:
            error_message("Invalid element number", GENERAL_ERROR);
            break;
        }
      }
    }

    /* recalculate the addresses to address of the first node */

    pos = 0;
    for(i = 0; i < nodes + midnodes; i++){
      number = nd_num_nodes[i] - pos;
      nd_num_nodes[i] = pos;
      pos += number;
    }

#ifdef DEBUG
    for(i = 0; i < nodes + midnodes; i++){
      fprintf(stdout, "node %ld: %ld:", i + 1, nd_num_nodes[i + 1] - nd_num_nodes[i]);
      for(j = nd_num_nodes[i]; j < nd_num_nodes[i + 1]; j++){
        fprintf(stdout, " %ld", (long)(nd_con_nodes[j]));
      }
      fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
#endif

  }

#ifdef METIS
  if(nparts > 1){
    if(elem_cut == YES || profile_opt == YES){
      if((node_num_nodes = (idxtype *)calloc(nodes + 1, sizeof(idxtype))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);
    }
    if(node_cut == YES || profile_opt == YES){
      if((elem_num_elems = (idxtype *)calloc(elems + 1, sizeof(idxtype))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);
    }

    if((node_part = (idxtype *)calloc(nodes + midnodes, sizeof(idxtype))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
    if((elem_part = (idxtype *)calloc(elems, sizeof(idxtype))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    if((remote_node = (int *)calloc(nodes + midnodes, sizeof(int))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
    if((remote_elem = (int *)calloc(elems, sizeof(int))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    if((part_flag = (logic *)calloc(nparts, sizeof(logic))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);

    for(i = 0; i < nparts; i++)part_flag[i] = NO;

    if(elem_cut == YES || profile_opt == YES){

      /* count number of edges incident to nodes */

      for(i = 0; i < nodes; i++){
        node = i + 1;
        for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
          elem = node_con_elems[j];
          type = elem_type(elem);                              /* macro */
          switch(type){
            case EDGE_ELEM:
              elem = local_edge_id(elem);                        /* macro */
              fe_edge = &(edge_array[elem - 1]);
              for(k = 0; k < 2; k++){
                nd = fe_edge -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_edge -> node[k] -> id = -nd;
                  node_num_nodes[i]++;
                }
              }
              break;
            case FACE_ELEM:
              elem = local_face_id(elem);                        /* macro */
              fe_face = &(face_array[elem - 1]);
              for(k = 0; k < 3; k++){
                nd = fe_face -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_face -> node[k] -> id = -nd;
                  node_num_nodes[i]++;
                }
              }
              break;
            case QUAD_ELEM:
              elem = local_quad_id(elem);                        /* macro */
              fe_quad = &(quad_array[elem - 1]);
              for(k = 0; k < 4; k++){
                nd = fe_quad -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_quad -> node[k] -> id = -nd;
                  node_num_nodes[i]++;
                }
              }
              break;
            case TETRA_ELEM:
              elem = local_tetra_id(elem);                       /* macro */
              fe_tetra = &(tetra_array[elem - 1]);
              for(k = 0; k < 4; k++){
                nd = fe_tetra -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_tetra -> node[k] -> id = -nd;
                  node_num_nodes[i]++;
                }
              }
              break;
            case PYRAM_ELEM:
              elem = local_pyram_id(elem);                       /* macro */
              fe_pyram = &(pyram_array[elem - 1]);
              for(k = 0; k < 5; k++){
                nd = fe_pyram -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_pyram -> node[k] -> id = -nd;
                  node_num_nodes[i]++;
                }
              }
              break;
            case WEDGE_ELEM:
              elem = local_wedge_id(elem);                       /* macro */
              fe_wedge = &(wedge_array[elem - 1]);
              for(k = 0; k < 6; k++){
                nd = fe_wedge -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_wedge -> node[k] -> id = -nd;
                  node_num_nodes[i]++;
                }
              }
              break;
            case HEXA_ELEM:
              elem = local_hexa_id(elem);                        /* macro */
              fe_hexa = &(hexa_array[elem - 1]);
              for(k = 0; k < 8; k++){
                nd = fe_hexa -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_hexa -> node[k] -> id = -nd;
                  node_num_nodes[i]++;
                }
              }
              break;
            default:
              error_message("Invalid element number", GENERAL_ERROR);
              break;
          }
        }

        /* restore connected node id to positive value */

        for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
          elem = node_con_elems[j];
          type = elem_type(elem);                              /* macro */
          switch(type){
            case EDGE_ELEM:
              elem = local_edge_id(elem);                        /* macro */
              fe_edge = &(edge_array[elem - 1]);
              for(k = 0; k < 2; k++)fe_edge -> node[k] -> id = abs(fe_edge -> node[k] -> id);
              break;
            case FACE_ELEM:
              elem = local_face_id(elem);                        /* macro */
              fe_face = &(face_array[elem - 1]);
              for(k = 0; k < 3; k++)fe_face -> node[k] -> id = abs(fe_face -> node[k] -> id);
              break;
            case QUAD_ELEM:						
              elem = local_quad_id(elem);                        /* macro */
              fe_quad = &(quad_array[elem - 1]);
              for(k = 0; k < 4; k++)fe_quad -> node[k] -> id = abs(fe_quad -> node[k] -> id);
              break;
            case TETRA_ELEM:
              elem = local_tetra_id(elem);                       /* macro */
              fe_tetra = &(tetra_array[elem - 1]);
              for(k = 0; k < 4; k++)fe_tetra -> node[k] -> id = abs(fe_tetra -> node[k] -> id);
              break;
            case PYRAM_ELEM:
              elem = local_pyram_id(elem);                       /* macro */
              fe_pyram = &(pyram_array[elem - 1]);
              for(k = 0; k < 5; k++)fe_pyram -> node[k] -> id = abs(fe_pyram -> node[k] -> id);
              break;
            case WEDGE_ELEM:
              elem = local_wedge_id(elem);                       /* macro */
              fe_wedge = &(wedge_array[elem - 1]);
              for(k = 0; k < 6; k++)fe_wedge -> node[k] -> id = abs(fe_wedge -> node[k] -> id);
              break;
            case HEXA_ELEM:
              elem = local_hexa_id(elem);                        /* macro */
              fe_hexa = &(hexa_array[elem - 1]);
              for(k = 0; k < 8; k++)fe_hexa -> node[k] -> id = abs(fe_hexa -> node[k] -> id);
              break;
            default:
              error_message("Invalid element number", GENERAL_ERROR);
              break;
          }
        }
      }

      /* recalculate the number of edges to current addresses */

      pos = 0;
      for(i = 0; i < nodes; i++){
        number = node_num_nodes[i];
        node_num_nodes[i] = pos;
        pos += number;
      }

      node_num_nodes[nodes] = size = pos;
      if((node_con_nodes = (idxtype *)calloc(size, sizeof(idxtype))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);

      /* store other node numbers of edges incident to nodes (Fortran style !!!) */

      for(i = 0; i < nodes; i++){
        node = i + 1;
        for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
          elem = node_con_elems[j];
          type = elem_type(elem);                              /* macro */
          switch(type){
            case EDGE_ELEM:
              elem = local_edge_id(elem);                        /* macro */
              fe_edge = &(edge_array[elem - 1]);
              for(k = 0; k < 2; k++){
                nd = fe_edge -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_edge -> node[k] -> id = -nd;
                  node_con_nodes[node_num_nodes[i]++] = nd;
                }
              }
              break;
            case FACE_ELEM:
              elem = local_face_id(elem);                        /* macro */
              fe_face = &(face_array[elem - 1]);
              for(k = 0; k < 3; k++){
                nd = fe_face -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_face -> node[k] -> id = -nd;
                  node_con_nodes[node_num_nodes[i]++] = nd;
                }
              }
              break;
            case QUAD_ELEM:
              elem = local_quad_id(elem);                        /* macro */
              fe_quad = &(quad_array[elem - 1]);
              for(k = 0; k < 4; k++){
                nd = fe_quad -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_quad -> node[k] -> id = -nd;
                  node_con_nodes[node_num_nodes[i]++] = nd;
                }
              }
              break;
            case TETRA_ELEM:
              elem = local_tetra_id(elem);                       /* macro */
              fe_tetra = &(tetra_array[elem - 1]);
              for(k = 0; k < 4; k++){
                nd = fe_tetra -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_tetra -> node[k] -> id = -nd;
                  node_con_nodes[node_num_nodes[i]++] = nd;
                }
              }
              break;
            case PYRAM_ELEM:
              elem = local_pyram_id(elem);                       /* macro */
              fe_pyram = &(pyram_array[elem - 1]);
              for(k = 0; k < 5; k++){
                nd = fe_pyram -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_pyram -> node[k] -> id = -nd;
                  node_con_nodes[node_num_nodes[i]++] = nd;
                }
              }
              break;
            case WEDGE_ELEM:
              elem = local_wedge_id(elem);                       /* macro */
              fe_wedge = &(wedge_array[elem - 1]);
              for(k = 0; k < 6; k++){
                nd = fe_wedge -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_wedge -> node[k] -> id = -nd;
                  node_con_nodes[node_num_nodes[i]++] = nd;
                }
              }
              break;
            case HEXA_ELEM:
              elem = local_hexa_id(elem);                        /* macro */
              fe_hexa = &(hexa_array[elem - 1]);
              for(k = 0; k < 8; k++){
                nd = fe_hexa -> node[k] -> id;
                if(nd != node && nd > 0){
                  fe_hexa -> node[k] -> id = -nd;
                  node_con_nodes[node_num_nodes[i]++] = nd;
                }
              }
              break;
            default:
              error_message("Invalid element number", GENERAL_ERROR);
              break;
          }
        }

/* restore connected node id to positive value */

				for(j = node_num_elems[i]; j < node_num_elems[node]; j++){
					elem = node_con_elems[j];
					type = elem_type(elem);                              /* macro */
					switch(type){
					case EDGE_ELEM:
						elem = local_edge_id(elem);                        /* macro */
						fe_edge = &(edge_array[elem - 1]);
						for(k = 0; k < 2; k++)fe_edge -> node[k] -> id = abs(fe_edge -> node[k] -> id);
						break;
					case FACE_ELEM:
						elem = local_face_id(elem);                        /* macro */
						fe_face = &(face_array[elem - 1]);
						for(k = 0; k < 3; k++)fe_face -> node[k] -> id = abs(fe_face -> node[k] -> id);
						break;
					case QUAD_ELEM:
						elem = local_quad_id(elem);                        /* macro */
						fe_quad = &(quad_array[elem - 1]);
						for(k = 0; k < 4; k++)fe_quad -> node[k] -> id = abs(fe_quad -> node[k] -> id);
						break;
					case TETRA_ELEM:
						elem = local_tetra_id(elem);                       /* macro */
						fe_tetra = &(tetra_array[elem - 1]);
						for(k = 0; k < 4; k++)fe_tetra -> node[k] -> id = abs(fe_tetra -> node[k] -> id);
						break;
					case PYRAM_ELEM:
						elem = local_pyram_id(elem);                       /* macro */
						fe_pyram = &(pyram_array[elem - 1]);
						for(k = 0; k < 5; k++)fe_pyram -> node[k] -> id = abs(fe_pyram -> node[k] -> id);
						break;
					case WEDGE_ELEM:
						elem = local_wedge_id(elem);                       /* macro */
						fe_wedge = &(wedge_array[elem - 1]);
						for(k = 0; k < 6; k++)fe_wedge -> node[k] -> id = abs(fe_wedge -> node[k] -> id);
						break;
					case HEXA_ELEM:
            elem = local_hexa_id(elem);                        /* macro */
            fe_hexa = &(hexa_array[elem - 1]);
            for(k = 0; k < 8; k++)fe_hexa -> node[k] -> id = abs(fe_hexa -> node[k] -> id);
            break;
          default:
            error_message("Invalid element number", GENERAL_ERROR);
            break;
          }
        }
      }

      /* recalculate the addresses to address of the first node (Fortran style !!!!) */

      pos = 0;
      for(i = 0; i < nodes; i++){
        number = node_num_nodes[i] - pos;
        node_num_nodes[i] = pos + 1;
        pos += number;
      }
      node_num_nodes[nodes]++;

#ifdef DEBUG
      for(i = 0; i < nodes; i++){
        fprintf(stdout, "node %ld: %ld:", i + 1, (long)(node_num_nodes[i + 1] - node_num_nodes[i]));
        for(j = node_num_nodes[i] - 1; j < node_num_nodes[i + 1] - 1; j++){
          fprintf(stdout, " %ld", (long)(node_con_nodes[j]));
        }
        fprintf(stdout, "\n");
      }
      fprintf(stdout, "\n");
#endif

    } /* elem_cut */

    if(node_cut == YES || profile_opt == YES){

      /* count number of elements neighbouring with elements */

      fe_edge = edge_array;                                       /* all elements */
      for(i = 0; i < edges; i++, fe_edge++){
        elem1 = global_edge_id(i) + 1;                            /* macro */
        node1 = fe_edge -> node[0] -> id;
        node2 = fe_edge -> node[1] -> id;
        for(j = node_num_elems[node1 - 1]; j < node_num_elems[node1]; j++){
          elem2 = node_con_elems[j];
          if(elem2 == elem1)continue;
          if(is_edge(elem2) == YES){
            elem_num_elems[elem1 - 1]++;
            elem_num_elems[elem2 - 1]++;
            continue;
          }
          for(k = node_num_elems[node2 - 1]; k < node_num_elems[node2]; k++){
            if(elem2 == node_con_elems[k]){
              elem_num_elems[elem1 - 1]++;
              elem_num_elems[elem2 - 1]++;
              break;
            }
          }
        }
      }

      fe_face = face_array;                                        /* only tetras, pyrams and wedges sharing face */
      for(i = 0; i < faces; i++, fe_face++){
        if(fe_face -> entity -> boundary == NO)continue;
        count = 0;
        elem1 = global_face_id(i) + 1;                             /* macro */
        node1 = fe_face -> node[0] -> id;
        node2 = fe_face -> node[1] -> id;
        node3 = fe_face -> node[2] -> id;
        for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
          elem2 = node_con_elems[k];
          if(is_tetra(elem2) == YES){
            fe_tetra = &(tetra_array[local_tetra_id(elem2) - 1]);                       /* macro */
            if(fe_face -> entity -> ngb_ent[0] != fe_tetra -> entity && 
                fe_face -> entity -> ngb_ent[1] != fe_tetra -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_num_elems[elem1 - 1]++;
                  elem_num_elems[elem2 - 1]++;
                  count++;
                  break;
                }
              }
              break;
            }
          }
          if(is_pyram(elem2) == YES){
            fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                       /* macro */
            if(fe_face -> entity -> ngb_ent[0] != fe_pyram -> entity && 
                fe_face -> entity -> ngb_ent[1] != fe_pyram -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_num_elems[elem1 - 1]++;
                  elem_num_elems[elem2 - 1]++;
                  count++;
                  break;
                }
              }
              break;
            }
          }
          if(is_wedge(elem2) == YES){
            fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                       /* macro */
            if(fe_face -> entity -> ngb_ent[0] != fe_wedge -> entity && 
                fe_face -> entity -> ngb_ent[1] != fe_wedge -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_num_elems[elem1 - 1]++;
                  elem_num_elems[elem2 - 1]++;
                  count++;
                  break;
                }
              }
              break;
            }
          }
          if(count == 2)break;
        }
      }

      fe_face = face_array;                             /* all except edges and except tetras, pyrams and wedges sharing face */
      for(i = 0; i < faces; i++, fe_face++){
        elem1 = global_face_id(i) + 1;                        /* macro */
        elem_num_elems[elem1 - 1] += 3;
        for(j = 0; j < 3; j++){
          if((fe_face -> bflag & (1 << j)) == 0)continue;
          elem_num_elems[elem1 - 1]--;
          j1 = face_ed_nd[j][0];
          j2 = face_ed_nd[j][1];
          node1 = fe_face -> node[j1] -> id;
          node2 = fe_face -> node[j2] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_edge(elem2) == YES)continue;
            if(is_tetra(elem2) == YES){
              fe_tetra = &(tetra_array[local_tetra_id(elem2) - 1]);                        /* macro */
              if(fe_face -> entity -> ngb_ent[0] == fe_tetra -> entity ||
                  fe_face -> entity -> ngb_ent[1] == fe_tetra -> entity)continue;
            }
            if(is_pyram(elem2) == YES){
              fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                        /* macro */
              if(fe_face -> entity -> ngb_ent[0] == fe_pyram -> entity ||
                  fe_face -> entity -> ngb_ent[1] == fe_pyram -> entity)continue;
            }
            if(is_wedge(elem2) == YES){
              fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                        /* macro */
              if(fe_face -> entity -> ngb_ent[0] == fe_wedge -> entity ||
                  fe_face -> entity -> ngb_ent[1] == fe_wedge -> entity)continue;
            }
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 == node_con_elems[m]){
                elem_num_elems[elem1 - 1]++;
                if(is_edge(elem2) == NO && is_face(elem2) == NO)elem_num_elems[elem2 - 1]++;
                break;
              }
            }
          }
        }
      }

      fe_quad = quad_array;                                        /* only pyrams, wedges and hexas sharing quad */
      for(i = 0; i < quads; i++, fe_quad++){
        if(fe_quad -> entity -> boundary == NO)continue;
        count = 0;
        elem1 = global_quad_id(i) + 1;                             /* macro */
        node1 = fe_quad -> node[0] -> id;
        node2 = fe_quad -> node[1] -> id;
        node3 = fe_quad -> node[2] -> id;
        node4 = fe_quad -> node[3] -> id;
        for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
          elem2 = node_con_elems[k];
          if(is_pyram(elem2) == YES){
            fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                        /* macro */
            if(fe_quad -> entity -> ngb_ent[0] != fe_pyram -> entity && 
                fe_quad -> entity -> ngb_ent[1] != fe_pyram -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_num_elems[elem1 - 1]++;
                    elem_num_elems[elem2 - 1]++;
                    count++;
                    break;
                  }
                }
                break;
              }
              break;
            }
          }
          if(is_wedge(elem2) == YES){
            fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                        /* macro */
            if(fe_quad -> entity -> ngb_ent[0] != fe_wedge -> entity && 
                fe_quad -> entity -> ngb_ent[1] != fe_wedge -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_num_elems[elem1 - 1]++;
                    elem_num_elems[elem2 - 1]++;
                    count++;
                    break;
                  }
                }
                break;
              }
              break;
            }
          }
          if(is_hexa(elem2) == YES){
            fe_hexa = &(hexa_array[local_hexa_id(elem2) - 1]);                        /* macro */
            if(fe_quad -> entity -> ngb_ent[0] != fe_hexa -> entity && 
                fe_quad -> entity -> ngb_ent[1] != fe_hexa -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_num_elems[elem1 - 1]++;
                    elem_num_elems[elem2 - 1]++;
                    count++;
                    break;
                  }
                }
                break;
              }
              break;
            }
          }
          if(count == 2)break;
        }
      }

      fe_quad = quad_array;                       /* all except edges, faces and except pyram, wedges and hexas sharing quad */
      for(i = 0; i < quads; i++, fe_quad++){
        elem1 = global_quad_id(i) + 1;                        /* macro */
        elem_num_elems[elem1 - 1] += 4;
        for(j = 0; j < 4; j++){
          if((fe_quad -> bflag & (1 << j)) == 0)continue;
          elem_num_elems[elem1 - 1]--;
          j1 = quad_ed_nd[j][0];
          j2 = quad_ed_nd[j][1];
          node1 = fe_quad -> node[j1] -> id;
          node2 = fe_quad -> node[j2] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_edge(elem2) == YES || is_face(elem2) == YES)continue;
            if(is_pyram(elem2) == YES){
              fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                        /* macro */
              if(fe_quad -> entity -> ngb_ent[0] == fe_pyram -> entity ||
                  fe_quad -> entity -> ngb_ent[1] == fe_pyram -> entity)continue;
            }
            if(is_wedge(elem2) == YES){
              fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                        /* macro */
              if(fe_quad -> entity -> ngb_ent[0] == fe_wedge -> entity ||
                  fe_quad -> entity -> ngb_ent[1] == fe_wedge -> entity)continue;
            }
            if(is_hexa(elem2) == YES){
              fe_hexa = &(hexa_array[local_hexa_id(elem2) - 1]);                           /* macro */
              if(fe_quad -> entity -> ngb_ent[0] == fe_hexa -> entity ||
                  fe_quad -> entity -> ngb_ent[1] == fe_hexa -> entity)continue;
            }
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 == node_con_elems[m]){
                elem_num_elems[elem1 - 1]++;
                if(is_edge(elem2) == NO && is_face(elem2) == NO && is_quad(elem2) == NO)elem_num_elems[elem2 - 1]++;
                break;
              }
            }
          }
        }
      }

      fe_tetra = tetra_array;                                          /* only tetras */
      for(i = 0; i < tetras; i++, fe_tetra++){
        elem1 = global_tetra_id(i) + 1;                                /* macro */
        elem_num_elems[elem1 - 1] += 4;
        for(j = 0; j < 4; j++){
          if((fe_tetra -> bflag & (1 << j)) == 0)continue;
          count = 0;
          elem_num_elems[elem1 - 1]--;
          j1 = tetra_fc_nd[j][0];
          j2 = tetra_fc_nd[j][1];
          j3 = tetra_fc_nd[j][2];
          node1 = fe_tetra -> node[j1] -> id;
          node2 = fe_tetra -> node[j2] -> id;
          node3 = fe_tetra -> node[j3] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_tetra(elem2) == NO && is_pyram(elem2) == NO && is_wedge(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_num_elems[elem1 - 1]++;
                  count++;
                  break;
                }
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      fe_pyram = pyram_array;                                         /* only pyrams */
      for(i = 0; i < pyrams; i++, fe_pyram++){
        elem1 = global_pyram_id(i) + 1;                                /* macro */
        elem_num_elems[elem1 - 1] += 5;
        j = 0;
        if((fe_pyram -> bflag & (1 << j)) != 0){
          count = 0;
          elem_num_elems[elem1 - 1]--;
          j1 = pyram_fc_nd[j][0];
          j2 = pyram_fc_nd[j][1];
          j3 = pyram_fc_nd[j][2];
          j4 = pyram_fc_nd[j][3];
          node1 = fe_pyram -> node[j1] -> id;
          node2 = fe_pyram -> node[j2] -> id;
          node3 = fe_pyram -> node[j3] -> id;
          node4 = fe_pyram -> node[j4] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_pyram(elem2) == NO && is_wedge(elem2) == NO && is_hexa(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_num_elems[elem1 - 1]++;
                    count++;
                    break;
                  }
                }
                if(count == 1)break;
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
        for(j = 1; j < 5; j++){
          if((fe_pyram -> bflag & (1 << j)) == 0)continue;
          count = 0;
          elem_num_elems[elem1 - 1]--;
          j1 = pyram_fc_nd[j][0];
          j2 = pyram_fc_nd[j][1];
          j3 = pyram_fc_nd[j][2];
          node1 = fe_pyram -> node[j1] -> id;
          node2 = fe_pyram -> node[j2] -> id;
          node3 = fe_pyram -> node[j3] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_tetra(elem2) == NO && is_pyram(elem2) == NO && is_wedge(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_num_elems[elem1 - 1]++;
                  count++;
                  break;
                }
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      fe_wedge = wedge_array;                                         /* only wedges */
      for(i = 0; i < wedges; i++, fe_wedge++){
        elem1 = global_wedge_id(i) + 1;                                /* macro */
        elem_num_elems[elem1 - 1] += 5;
        for(j = 0; j < 2; j++){
          if((fe_wedge -> bflag & (1 << j)) == 0)continue;
          count = 0;
          elem_num_elems[elem1 - 1]--;
          j1 = wedge_fc_nd[j][0];
          j2 = wedge_fc_nd[j][1];
          j3 = wedge_fc_nd[j][2];
          node1 = fe_wedge -> node[j1] -> id;
          node2 = fe_wedge -> node[j2] -> id;
          node3 = fe_wedge -> node[j3] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_tetra(elem2) == NO && is_pyram(elem2) == NO && is_wedge(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_num_elems[elem1 - 1]++;
                  count++;
                  break;
                }
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
        for(j = 2; j < 5; j++){
          if((fe_wedge -> bflag & (1 << j)) == 0)continue;
          count = 0;
          elem_num_elems[elem1 - 1]--;
          j1 = wedge_fc_nd[j][0];
          j2 = wedge_fc_nd[j][1];
          j3 = wedge_fc_nd[j][2];
          j4 = wedge_fc_nd[j][3];
          node1 = fe_wedge -> node[j1] -> id;
          node2 = fe_wedge -> node[j2] -> id;
          node3 = fe_wedge -> node[j3] -> id;
          node4 = fe_wedge -> node[j4] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_pyram(elem2) == NO && is_wedge(elem2) == NO && is_hexa(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_num_elems[elem1 - 1]++;
                    count++;
                    break;
                  }
                }
                if(count == 1)break;
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      fe_hexa = hexa_array;                                            /* only hexas  */
      for(i = 0; i < hexas; i++, fe_hexa++){
        elem1 = global_hexa_id(i) + 1;                                 /* macro */
        elem_num_elems[elem1 - 1] += 6;
        for(j = 0; j < 6; j++){
          if((fe_hexa -> bflag & (1 << j)) == 0)continue;
          count = 0;
          elem_num_elems[elem1 - 1]--;
          j1 = hexa_fc_nd[j][0];
          j2 = hexa_fc_nd[j][1];
          j3 = hexa_fc_nd[j][2];
          j4 = hexa_fc_nd[j][3];
          node1 = fe_hexa -> node[j1] -> id;
          node2 = fe_hexa -> node[j2] -> id;
          node3 = fe_hexa -> node[j3] -> id;
          node4 = fe_hexa -> node[j4] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_pyram(elem2) == NO && is_wedge(elem2) == NO && is_hexa(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_num_elems[elem1 - 1]++;
                    count++;
                    break;
                  }
                }
                if(count == 1)break;
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      /* recalculate the number of elements to current addresses */

      pos = 0;
      for(i = 0; i < elems; i++){
        number = elem_num_elems[i];
        elem_num_elems[i] = pos;
        pos += number;
      }

      if(pos / 2 * 2 != pos)
        error_message("Internal error", GENERAL_ERROR);

      elem_num_elems[elems] = size = pos;
      if((elem_con_elems = (idxtype *)calloc(size, sizeof(idxtype))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);

      /* store element numbers neighbouring with elements (Fortran style !!!) */

      fe_edge = edge_array;                                       /* all elements */
      for(i = 0; i < edges; i++, fe_edge++){
        elem1 = global_edge_id(i) + 1;                            /* macro */
        node1 = fe_edge -> node[0] -> id;
        node2 = fe_edge -> node[1] -> id;
        for(j = node_num_elems[node1 - 1]; j < node_num_elems[node1]; j++){
          elem2 = node_con_elems[j];
          if(elem2 == elem1)continue;
          if(is_edge(elem2) == YES){
            elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
            elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
            continue;
          }
          for(k = node_num_elems[node2 - 1]; k < node_num_elems[node2]; k++){
            if(elem2 == node_con_elems[k]){
              elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
              elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
              break;
            }
          }
        }
      }

      fe_face = face_array;                                        /* only tetras, pyrams and wedges sharing face */
      for(i = 0; i < faces; i++, fe_face++){
        if(fe_face -> entity -> boundary == NO)continue;
        count = 0;
        elem1 = global_face_id(i) + 1;                             /* macro */
        node1 = fe_face -> node[0] -> id;
        node2 = fe_face -> node[1] -> id;
        node3 = fe_face -> node[2] -> id;
        for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
          elem2 = node_con_elems[k];
          if(is_tetra(elem2) == YES){
            fe_tetra = &(tetra_array[local_tetra_id(elem2) - 1]);                      /* macro */
            if(fe_face -> entity -> ngb_ent[0] != fe_tetra -> entity && 
                fe_face -> entity -> ngb_ent[1] != fe_tetra -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                  elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
                  count++;
                  break;
                }
              }
              break;
            }
          }
          if(is_pyram(elem2) == YES){
            fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                      /* macro */
            if(fe_face -> entity -> ngb_ent[0] != fe_pyram -> entity && 
                fe_face -> entity -> ngb_ent[1] != fe_pyram -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                  elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
                  count++;
                  break;
                }
              }
              break;
            }
          }
          if(is_wedge(elem2) == YES){
            fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                      /* macro */
            if(fe_face -> entity -> ngb_ent[0] != fe_wedge -> entity && 
                fe_face -> entity -> ngb_ent[1] != fe_wedge -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                  elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
                  count++;
                  break;
                }
              }
              break;
            }
          }
          if(count == 2)break;
        }
      }

      fe_face = face_array;                      /* all except edges and except tetras, pyrams and wedges sharing face */
      for(i = 0; i < faces; i++, fe_face++){
        elem1 = global_face_id(i) + 1;                        /* macro */
        for(j = 0; j < 3; j++){
          if((fe_face -> bflag & (1 << j)) == 0)
            bflag = NO;
          else
            bflag = YES;
          count = 0;
          j1 = face_ed_nd[j][0];
          j2 = face_ed_nd[j][1];
          node1 = fe_face -> node[j1] -> id;
          node2 = fe_face -> node[j2] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_edge(elem2) == YES)continue;
            if(is_tetra(elem2) == YES){
              fe_tetra = &(tetra_array[local_tetra_id(elem2) - 1]);                              /* macro */
              if(fe_face -> entity -> ngb_ent[0] == fe_tetra -> entity ||
                  fe_face -> entity -> ngb_ent[1] == fe_tetra -> entity)continue;
            }				
            if(is_pyram(elem2) == YES){
              fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                              /* macro */
              if(fe_face -> entity -> ngb_ent[0] == fe_pyram -> entity ||
                  fe_face -> entity -> ngb_ent[1] == fe_pyram -> entity)continue;
            }				
            if(is_wedge(elem2) == YES){
              fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                              /* macro */
              if(fe_face -> entity -> ngb_ent[0] == fe_wedge -> entity ||
                  fe_face -> entity -> ngb_ent[1] == fe_wedge -> entity)continue;
            }				
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 == node_con_elems[m]){
                elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                if(is_edge(elem2) == NO && is_face(elem2) == NO)
                  elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
                count++;
                break;
              }
            }
            if(count == 1 && bflag == NO)break;
          }
        }
      }

      fe_quad = quad_array;                                        /* only pyrams, wedges and hexas sharing quad */
      for(i = 0; i < quads; i++, fe_quad++){
        if(fe_quad -> entity -> boundary == NO)continue;
        count = 0;
        elem1 = global_quad_id(i) + 1;                             /* macro */
        node1 = fe_quad -> node[0] -> id;
        node2 = fe_quad -> node[1] -> id;
        node3 = fe_quad -> node[2] -> id;
        node4 = fe_quad -> node[3] -> id;
        for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
          elem2 = node_con_elems[k];
          if(is_pyram(elem2) == YES){
            fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                        /* macro */
            if(fe_quad -> entity -> ngb_ent[0] != fe_pyram -> entity && 
                fe_quad -> entity -> ngb_ent[1] != fe_pyram -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                    elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
                    count++;
                    break;
                  }
                }
                break;
              }
              break;
            }
          }
          if(is_wedge(elem2) == YES){
            fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                        /* macro */
            if(fe_quad -> entity -> ngb_ent[0] != fe_wedge -> entity && 
                fe_quad -> entity -> ngb_ent[1] != fe_wedge -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                    elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
                    count++;
                    break;
                  }
                }
                break;
              }
              break;
            }
          }
          if(is_hexa(elem2) == YES){
            fe_hexa = &(hexa_array[local_hexa_id(elem2) - 1]);                        /* macro */
            if(fe_quad -> entity -> ngb_ent[0] != fe_hexa -> entity && 
                fe_quad -> entity -> ngb_ent[1] != fe_hexa -> entity)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                    elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1; 
                    count++;
                    break;
                  }
                }
                break;
              }
              break;
            }
          }
          if(count == 2)break;
        }
      }

      fe_quad = quad_array;                     /* all except edges, faces and except pyrams, wedges and hexas sharing quad */
      for(i = 0; i < quads; i++, fe_quad++){
        elem1 = global_quad_id(i) + 1;                        /* macro */
        for(j = 0; j < 4; j++){
          if((fe_quad -> bflag & (1 << j)) == 0)
            bflag = NO;
          else
            bflag = YES;
          count = 0;
          j1 = quad_ed_nd[j][0];
          j2 = quad_ed_nd[j][1];
          node1 = fe_quad -> node[j1] -> id;
          node2 = fe_quad -> node[j2] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_edge(elem2) == YES || is_face(elem2) == YES)continue;
            if(is_pyram(elem2) == YES){
              fe_pyram = &(pyram_array[local_pyram_id(elem2) - 1]);                                         /* macro */
              if(fe_quad -> entity -> ngb_ent[0] == fe_pyram -> entity ||
                  fe_quad -> entity -> ngb_ent[1] == fe_pyram -> entity)continue;
            }				
            if(is_wedge(elem2) == YES){
              fe_wedge = &(wedge_array[local_wedge_id(elem2) - 1]);                                         /* macro */
              if(fe_quad -> entity -> ngb_ent[0] == fe_wedge -> entity ||
                  fe_quad -> entity -> ngb_ent[1] == fe_wedge -> entity)continue;
            }				
            if(is_hexa(elem2) == YES){
              fe_hexa = &(hexa_array[local_hexa_id(elem2) - 1]);                                         /* macro */
              if(fe_quad -> entity -> ngb_ent[0] == fe_hexa -> entity ||
                  fe_quad -> entity -> ngb_ent[1] == fe_hexa -> entity)continue;
            }				
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 == node_con_elems[m]){
                elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                if(is_edge(elem2) == NO && is_face(elem2) == NO && is_quad(elem2) == NO)
                  elem_con_elems[elem_num_elems[elem2 - 1]++] = elem1;
                count++;
                break;
              }
            }
            if(count == 1 && bflag == NO)break;
          }
        }
      }

      fe_tetra = tetra_array;                                          /* only tetras */
      for(i = 0; i < tetras; i++, fe_tetra++){
        elem1 = global_tetra_id(i) + 1;                                /* macro */
        for(j = 0; j < 4; j++){
          count = 0;
          j1 = tetra_fc_nd[j][0];
          j2 = tetra_fc_nd[j][1];
          j3 = tetra_fc_nd[j][2];
          node1 = fe_tetra -> node[j1] -> id;
          node2 = fe_tetra -> node[j2] -> id;
          node3 = fe_tetra -> node[j3] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(is_tetra(elem2) == NO && is_pyram(elem2) == NO && is_wedge(elem2) == NO)continue;
            if(elem2 == elem1)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                  count++;
                  break;
                }
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      fe_pyram = pyram_array;                                          /* only pyrams */
      for(i = 0; i < pyrams; i++, fe_pyram++){
        elem1 = global_pyram_id(i) + 1;                                /* macro */
        j = 0;
        count = 0;
        j1 = pyram_fc_nd[j][0];
        j2 = pyram_fc_nd[j][1];
        j3 = pyram_fc_nd[j][2];
        j4 = pyram_fc_nd[j][3];
        node1 = fe_pyram -> node[j1] -> id;
        node2 = fe_pyram -> node[j2] -> id;
        node3 = fe_pyram -> node[j3] -> id;
        node4 = fe_pyram -> node[j4] -> id;
        for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
          elem2 = node_con_elems[k];
          if(elem2 == elem1)continue;
          if(is_pyram(elem2) == NO && is_wedge(elem2) == NO && is_hexa(elem2) == NO)continue;
          for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
            if(elem2 != node_con_elems[m])continue;
            for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
              if(elem2 != node_con_elems[n])continue;
              for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                if(elem2 == node_con_elems[p]){
                  elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                  count++;
                  break;
                }
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
          if(count == 1)break;
        }
        for(j = 1; j < 5; j++){
          count = 0;
          j1 = pyram_fc_nd[j][0];
          j2 = pyram_fc_nd[j][1];
          j3 = pyram_fc_nd[j][2];
          node1 = fe_pyram -> node[j1] -> id;
          node2 = fe_pyram -> node[j2] -> id;
          node3 = fe_pyram -> node[j3] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(is_tetra(elem2) == NO && is_pyram(elem2) == NO && is_wedge(elem2) == NO)continue;
            if(elem2 == elem1)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                  count++;
                  break;
                }
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      fe_wedge = wedge_array;                                          /* only wedges */
      for(i = 0; i < wedges; i++, fe_wedge++){
        elem1 = global_wedge_id(i) + 1;                                /* macro */
        for(j = 0; j < 2; j++){
          count = 0;
          j1 = wedge_fc_nd[j][0];
          j2 = wedge_fc_nd[j][1];
          j3 = wedge_fc_nd[j][2];
          node1 = fe_wedge -> node[j1] -> id;
          node2 = fe_wedge -> node[j2] -> id;
          node3 = fe_wedge -> node[j3] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(is_tetra(elem2) == NO && is_pyram(elem2) == NO && is_wedge(elem2) == NO)continue;
            if(elem2 == elem1)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 == node_con_elems[n]){
                  elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                  count++;
                  break;
                }
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
        for(j = 2; j < 5; j++){
          count = 0;
          j1 = wedge_fc_nd[j][0];
          j2 = wedge_fc_nd[j][1];
          j3 = wedge_fc_nd[j][2];
          j4 = wedge_fc_nd[j][3];
          node1 = fe_wedge -> node[j1] -> id;
          node2 = fe_wedge -> node[j2] -> id;
          node3 = fe_wedge -> node[j3] -> id;
          node4 = fe_wedge -> node[j4] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_pyram(elem2) == NO && is_wedge(elem2) == NO && is_hexa(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                    count++;
                    break;
                  }
                }
                if(count == 1)break;
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      fe_hexa = hexa_array;                                            /* only hexas */
      for(i = 0; i < hexas; i++, fe_hexa++){
        elem1 = global_hexa_id(i) + 1;                                 /* macro */
        for(j = 0; j < 6; j++){
          count = 0;
          j1 = hexa_fc_nd[j][0];
          j2 = hexa_fc_nd[j][1];
          j3 = hexa_fc_nd[j][2];
          j4 = hexa_fc_nd[j][3];
          node1 = fe_hexa -> node[j1] -> id;
          node2 = fe_hexa -> node[j2] -> id;
          node3 = fe_hexa -> node[j3] -> id;
          node4 = fe_hexa -> node[j4] -> id;
          for(k = node_num_elems[node1 - 1]; k < node_num_elems[node1]; k++){
            elem2 = node_con_elems[k];
            if(elem2 == elem1)continue;
            if(is_pyram(elem2) == NO && is_wedge(elem2) == NO && is_hexa(elem2) == NO)continue;
            for(m = node_num_elems[node2 - 1]; m < node_num_elems[node2]; m++){
              if(elem2 != node_con_elems[m])continue;
              for(n = node_num_elems[node3 - 1]; n < node_num_elems[node3]; n++){
                if(elem2 != node_con_elems[n])continue;
                for(p = node_num_elems[node4 - 1]; p < node_num_elems[node4]; p++){
                  if(elem2 == node_con_elems[p]){
                    elem_con_elems[elem_num_elems[elem1 - 1]++] = elem2;
                    count++;
                    break;
                  }
                }
                if(count == 1)break;
              }
              if(count == 1)break;
            }
            if(count == 1)break;
          }
        }
      }

      /* recalculate the addresses to address of the first element (Fortran style !!!!) */

      pos = 0;
      for(i = 0; i < elems; i++){
        number = elem_num_elems[i] - pos;
        elem_num_elems[i] = pos + 1;
        pos += number;
      }
      elem_num_elems[elems]++;

#ifdef DEBUG
      for(i = 0; i < elems; i++){
        elem_id = i + 1;
        type = elem_type(elem_id);                                  /* macro */
        switch(type){
          case EDGE_ELEM:
            fprintf(stdout, "edge %ld:", local_edge_id(i) + 1);       /* macro */
            break;
          case FACE_ELEM:
            fprintf(stdout, "face %ld:", local_face_id(i) + 1);       /* macro */
            break;
          case QUAD_ELEM:
            fprintf(stdout, "quad %ld:", local_quad_id(i) + 1);       /* macro */
            break;
          case TETRA_ELEM:
            fprintf(stdout, "tetra %ld:", local_tetra_id(i) + 1);     /* macro */
            break;
          case PYRAM_ELEM:
            fprintf(stdout, "pyram %ld:", local_pyram_id(i) + 1);     /* macro */
            break;
          case WEDGE_ELEM:
            fprintf(stdout, "wedge %ld:", local_wedge_id(i) + 1);     /* macro */
            break;
          case HEXA_ELEM:
            fprintf(stdout, "hexa %ld:", local_hexa_id(i) + 1);       /* macro */
            break;
        }

        for(j = elem_num_elems[i] - 1; j < elem_num_elems[i + 1] - 1; j++){
          elem_id = elem_con_elems[j];
          type = elem_type(elem_id);                                /* macro */
          switch(type){
            case EDGE_ELEM:
              fprintf(stdout, " %ld(e)", local_edge_id(elem_id));     /* macro */
              break;
            case FACE_ELEM:
              fprintf(stdout, " %ld(f)", local_face_id(elem_id));     /* macro */
              break;
            case QUAD_ELEM:
              fprintf(stdout, " %ld(q)", local_quad_id(elem_id));     /* macro */
              break;
            case TETRA_ELEM:
              fprintf(stdout, " %ld(t)", local_tetra_id(elem_id));    /* macro */
              break;
            case PYRAM_ELEM:
              fprintf(stdout, " %ld(p)", local_pyram_id(elem_id));    /* macro */
              break;
            case WEDGE_ELEM:
              fprintf(stdout, " %ld(w)", local_wedge_id(elem_id));    /* macro */
              break;
            case HEXA_ELEM:
              fprintf(stdout, " %ld(h)", local_hexa_id(elem_id));     /* macro */
              break;
          }
        }
        fprintf(stdout, "\n");
      }
      fprintf(stdout, "\n");
#endif

    } /* node_cut */

#ifdef ELIXIR
    if(elixir == YES){
      if(nparts + 1 > MAX_LAYER){          /* I do not draw in layer 0 */
        virtual_layers = YES;              /* set the flag before ESICustomize is called */
      }

      if(ESIBuildInterface(ESI_GRAPHIC_EDITOR_MASK, 1, &app_nm) != 0){
        fprintf(stderr, "Graphic interface error\n");
        exit(1);
      }
      ESIPopup();

      view = ElixirNewView("Metis", "Domain decomposition", "midnightblue", "white", 400, 400);
      EMAttachView(ESIModel(), view);
      EVSetViewOrientation(view, VIEW_ORIENT_ISO);
      EVShowAxes(view, NO);
      ESIHandleCmd("render ambient 0.1");
      EVSetRenderMode(view, WIRE_RENDERING);

      black = ColorGetPixelFromString("black", &success);
      white = ColorGetPixelFromString("white", &success);

      colors[0] = green = ColorGetPixelFromString("green1", &success);
      colors[1] = red = ColorGetPixelFromString("red1", &success);
      colors[2] = yellow = ColorGetPixelFromString("yellow1", &success);
      colors[3] = magenta = ColorGetPixelFromString("magenta1", &success);
      colors[4] = cyan = ColorGetPixelFromString("cyan1", &success);
      colors[5] = blue = ColorGetPixelFromString("blue1", &success);
      colors[6] = orange = ColorGetPixelFromString("orange1", &success);
      colors[7] = purple = ColorGetPixelFromString("purple1", &success);
      colors[8] = pink = ColorGetPixelFromString("pink1", &success);

#ifdef MORE_COLORS
      colors[9] = ColorGetPixelFromString("green4", &success);
      colors[10] = ColorGetPixelFromString("red4", &success);
      colors[11] = ColorGetPixelFromString("yellow4", &success);
      colors[12] = ColorGetPixelFromString("magenta4", &success);
      colors[13] = ColorGetPixelFromString("cyan4", &success);
      colors[14] = ColorGetPixelFromString("blue4", &success);
      colors[15] = ColorGetPixelFromString("orange4", &success);
      colors[16] = ColorGetPixelFromString("purple4", &success);
      colors[17] = ColorGetPixelFromString("pink4", &success);
#endif
      font = FontGetFontFromString(FONT, &success);
      if(success == NO)font = FontDefaultFont();

      if((layers = (logic *)calloc(nparts + 1, sizeof(logic))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);

      if(virtual_layers == YES){
        if((layer_obj = (GraphicObj ***)calloc(nparts + 1, sizeof(GraphicObj **))) == NULL)
          error_message("Memory allocation error", MEMORY_ERROR);
        if((layer_obj_num = (long *)calloc(nparts + 1, sizeof(long))) == NULL)
          error_message("Memory allocation error", MEMORY_ERROR);
      }
      else{
        if(nparts <= NUM_COLORS){
          for(i = 0; i < nparts; i++){
            argn = 0;
            XtSetArg(args[argn], XtNbackground, colors[i]); argn++;
            XtSetArg(args[argn], XtNforeground, black); argn++;
            XtSetValues(set_button[i], args, (Cardinal)argn);
            XtSetValues(add_button[i], args, (Cardinal)argn);
            XtSetValues(del_button[i], args, (Cardinal)argn);
          }
        }
      }
    }
#endif

    options[0] = 0;
    num_flag = 1;
    weight_flag = 0;

#ifdef PAWBAL

    /* profile optimized partitioning must be called before normal call to Metis */

    if(profile_opt == YES){
      size = edges * 2 + faces * 3 + quads * 4 + tetras * 4 + pyrams * 5 + wedges * 6 + hexas * 8;

      if((elem_num_con = (idxtype *)calloc(elems + 1, sizeof(idxtype))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);
      if((elem_connect = (idxtype *)calloc(size, sizeof(idxtype))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);
      if((node_prop = (idxtype *)calloc(nodes, sizeof(idxtype))) == NULL)
        error_message("Memory allocation error", MEMORY_ERROR);

      elem_num_con[0] = 0;
      pos = 0;

      /* eventually it may be possible to fill node_prop when reading nodes;
         this would enable to use property not only based on classification to model entities */

      fe_Node = node_array;
      for(i = 0; i < nodes; i++, fe_Node++){
        node_prop[i] = fe_Node -> entity -> property_id;
      }

      fe_edge = edge_array;
      for(i = 0; i < edges; i++, fe_edge++){
        for(j = 0; j < 2; j++)elem_connect[pos++] = fe_edge -> node[j] -> id;
        elem_num_con[i + 1] = elem_num_con[i] + 2;
      }

      fe_face = face_array;
      for(i = 0; i < faces; i++, fe_face++){
        for(j = 0; j < 3; j++)elem_connect[pos++] = fe_face -> node[j] -> id;
        elem_num_con[i + 1] = elem_num_con[i] + 3;
      }

      fe_quad = quad_array;
      for(i = 0; i < quads; i++, fe_quad++){
        for(j = 0; j < 4; j++)elem_connect[pos++] = fe_quad -> node[j] -> id;
        elem_num_con[i + 1] = elem_num_con[i] + 4;
      }

      fe_tetra = tetra_array;
      for(i = 0; i < tetras; i++, fe_tetra++){
        for(j = 0; j < 4; j++)elem_connect[pos++] = fe_tetra -> node[j] -> id;
        elem_num_con[i + 1] = elem_num_con[i] + 4;
      }

      fe_pyram = pyram_array;
      for(i = 0; i < pyrams; i++, fe_pyram++){
        for(j = 0; j < 5; j++)elem_connect[pos++] = fe_pyram -> node[j] -> id;
        elem_num_con[i + 1] = elem_num_con[i] + 5;
      }

      fe_wedge = wedge_array;
      for(i = 0; i < wedges; i++, fe_wedge++){
        for(j = 0; j < 6; j++)elem_connect[pos++] = fe_wedge -> node[j] -> id;
        elem_num_con[i + 1] = elem_num_con[i] + 6;
      }

      fe_hexa = hexa_array;
      for(i = 0; i < hexas; i++, fe_hexa++){
        for(j = 0; j < 8; j++)elem_connect[pos++] = fe_hexa -> node[j] -> id;
        elem_num_con[i + 1] = elem_num_con[i] + 8;
      }

      mesh_connectivity = pawbal_simpleMeshCreateFromParams(elems, nodes, elem_num_con, elem_connect, node_prop);

      /* note: node_num_nodes and node_con_nodes are build for linear elements */

      nodal_graph = pawbal_simpleGraphCreateFromParams(nodes, node_num_nodes, node_con_nodes);
      dual_graph = pawbal_simpleGraphCreateFromParams(elems, elem_num_elems, elem_con_elems);

      pawbal_partMeshKwaySloan(mesh_connectivity, dual_graph, nodal_graph, nparts, node_part, elem_part);

      free(elem_num_con);
      free(elem_connect);
      free(node_prop);

#ifdef DEBUG			
      for(i = 0; i < elems; i++){
        if(elem_part[i] <= 0 || elem_part[i] > nparts)
          error_message("Invalid partitioning detected", PARTITIONING_ERROR);

        part_id = 0;
        glob_elem_id = i + 1;
        type = elem_type(glob_elem_id);                        /* macro */
        switch(type){
          case EDGE_ELEM:
            elem = local_edge_id(glob_elem_id);                  /* macro */
            fe_edge = &(edge_array[elem - 1]);

            for(j = 0; j < 2; j++){
              node_id = fe_edge -> node[j] -> id;
              if(node_part[node_id - 1] == 0)continue;
              if(part_id == 0)
                part_id = node_part[node_id - 1];
              else{
                if(part_id != node_part[node_id - 1])
                  error_message("Invalid partitioning detected", PARTITIONING_ERROR);
              }
            }
            if(part_id != 0){
              if(elem_part[i] != part_id)
                error_message("Invalid partitioning detected", PARTITIONING_ERROR);
            }
            break;
          case FACE_ELEM:
            elem = local_face_id(glob_elem_id);                  /* macro */
            fe_face = &(face_array[elem - 1]);

            for(j = 0; j < 3; j++){
              node_id = fe_face -> node[j] -> id;
              if(node_part[node_id - 1] == 0)continue;
              if(part_id == 0)
                part_id = node_part[node_id - 1];
              else{
                if(part_id != node_part[node_id - 1])
                  error_message("Invalid partitioning detected", PARTITIONING_ERROR);
              }
            }
            if(part_id != 0){
              if(elem_part[i] != part_id)
                error_message("Invalid partitioning detected", PARTITIONING_ERROR);
            }
            break;
          case QUAD_ELEM:
            elem = local_quad_id(glob_elem_id);                  /* macro */
            fe_quad = &(quad_array[elem - 1]);

            for(j = 0; j < 4; j++){
              node_id = fe_quad -> node[j] -> id;
              if(node_part[node_id - 1] == 0)continue;
              if(part_id == 0)
                part_id = node_part[node_id - 1];
              else{
                if(part_id != node_part[node_id - 1])
                  error_message("Invalid partitioning detected", PARTITIONING_ERROR);
              }
            }
            if(part_id != 0){
              if(elem_part[i] != part_id)
                error_message("Invalid partitioning detected", PARTITIONING_ERROR);
            }
            break;
          case TETRA_ELEM:
            elem = local_tetra_id(glob_elem_id);                 /* macro */
            fe_tetra = &(tetra_array[elem - 1]);

            for(j = 0; j < 4; j++){
              node_id = fe_tetra -> node[j] -> id;
              if(node_part[node_id - 1] == 0)continue;
              if(part_id == 0)
                part_id = node_part[node_id - 1];
              else{
                if(part_id != node_part[node_id - 1])
                  error_message("Invalid partitioning detected", PARTITIONING_ERROR);
              }
            }
            if(part_id != 0){
              if(elem_part[i] != part_id)
                error_message("Invalid partitioning detected", PARTITIONING_ERROR);
            }
            break;
          case PYRAM_ELEM:
            elem = local_pyram_id(glob_elem_id);                 /* macro */
            fe_pyram = &(pyram_array[elem - 1]);

            for(j = 0; j < 5; j++){
              node_id = fe_pyram -> node[j] -> id;
              if(node_part[node_id - 1] == 0)continue;
              if(part_id == 0)
                part_id = node_part[node_id - 1];
              else{
                if(part_id != node_part[node_id - 1])
                  error_message("Invalid partitioning detected", PARTITIONING_ERROR);
              }
            }
            if(part_id != 0){
              if(elem_part[i] != part_id)
                error_message("Invalid partitioning detected", PARTITIONING_ERROR);
            }
            break;
          case WEDGE_ELEM:
            elem = local_wedge_id(glob_elem_id);                 /* macro */
            fe_wedge = &(wedge_array[elem - 1]);

            for(j = 0; j < 6; j++){
              node_id = fe_wedge -> node[j] -> id;
              if(node_part[node_id - 1] == 0)continue;
              if(part_id == 0)
                part_id = node_part[node_id - 1];
              else{
                if(part_id != node_part[node_id - 1])
                  error_message("Invalid partitioning detected", PARTITIONING_ERROR);
              }
            }
            if(part_id != 0){
              if(elem_part[i] != part_id)
                error_message("Invalid partitioning detected", PARTITIONING_ERROR);
            }
            break;
          case HEXA_ELEM:
            elem = local_hexa_id(glob_elem_id);                 /* macro */
            fe_hexa = &(hexa_array[elem - 1]);

            for(j = 0; j < 8; j++){
              node_id = fe_hexa -> node[j] -> id;
              if(node_part[node_id - 1] == 0)continue;
              if(part_id == 0)
                part_id = node_part[node_id - 1];
              else{
                if(part_id != node_part[node_id - 1])
                  error_message("Invalid partitioning detected", PARTITIONING_ERROR);
              }
            }
            if(part_id != 0){
              if(elem_part[i] != part_id)
                error_message("Invalid partitioning detected", PARTITIONING_ERROR);
            }
            break;
        }
      }
#endif

#ifdef DEBUG
      fprintf(stdout, "Partitions (nodes)5:");
      for(i = 0; i < nodes; i++)fprintf(stdout, " %ld", (long)(node_part[i]));
      fprintf(stdout, "\n");
      fprintf(stdout, "Partitions (elems)6:");
      for(i = 0; i < elems; i++)fprintf(stdout, " %ld", (long)(elem_part[i]));
      fprintf(stdout, "\n");
#endif

    }
#endif

    if(node_cut == YES){
      if(profile_opt == NO){
        graph_size = elems;

        if(mixed == YES && node_weight == YES){
          weight_flag = 2;

          if(dimension == 1){
            dim_coeff[0] = 1;
          }
          if(dimension == 2){
            dim_coeff[0] = 1;
            dim_coeff[1] = 3;
          }
          if(dimension == 3){
            dim_coeff[0] = 1;
            dim_coeff[1] = 3;
            dim_coeff[2] = 2;
          }

          if(elem_degree == LINEAR){
            weight_coeff[EDGE_ELEM] = EDGE_NODE_WEIGHT_LINEAR * dim_coeff[0];
            weight_coeff[FACE_ELEM] = FACE_NODE_WEIGHT_LINEAR * dim_coeff[1];
            weight_coeff[QUAD_ELEM] = QUAD_NODE_WEIGHT_LINEAR * dim_coeff[1];
            weight_coeff[TETRA_ELEM] = TETRA_NODE_WEIGHT_LINEAR * dim_coeff[2];
            weight_coeff[PYRAM_ELEM] = PYRAM_NODE_WEIGHT_LINEAR * dim_coeff[2];
            weight_coeff[WEDGE_ELEM] = WEDGE_NODE_WEIGHT_LINEAR * dim_coeff[2];
            weight_coeff[HEXA_ELEM] = HEXA_NODE_WEIGHT_LINEAR * dim_coeff[2];
          }
          if(elem_degree == QUADRATIC){
            weight_coeff[EDGE_ELEM] = EDGE_NODE_WEIGHT_QUADRATIC * dim_coeff[0];
            weight_coeff[FACE_ELEM] = FACE_NODE_WEIGHT_QUADRATIC * dim_coeff[1];
            weight_coeff[QUAD_ELEM] = QUAD_NODE_WEIGHT_QUADRATIC * dim_coeff[1];
            weight_coeff[TETRA_ELEM] = TETRA_NODE_WEIGHT_QUADRATIC * dim_coeff[2];
            weight_coeff[PYRAM_ELEM] = PYRAM_NODE_WEIGHT_QUADRATIC * dim_coeff[2];
            weight_coeff[WEDGE_ELEM] = WEDGE_NODE_WEIGHT_QUADRATIC * dim_coeff[2];
            weight_coeff[HEXA_ELEM] = HEXA_NODE_WEIGHT_QUADRATIC * dim_coeff[2];
          }

          if((weight_elem = (idxtype *)calloc(elems, sizeof(idxtype))) == NULL)
            error_message("Memory allocation error", MEMORY_ERROR);

          for(i = 0; i < elems; i++){
            elem_id = i + 1;
            type = elem_type(elem_id);                                  /* macro */
            weight_elem[i] = weight_coeff[type];
          }
        }
        if(elem_weight == YES){
          weight_flag = 2;

          if((weight_elem = (idxtype *)calloc(elems, sizeof(idxtype))) == NULL)
            error_message("Memory allocation error", MEMORY_ERROR);

          for(i = 0; i < elems; i++){
            elem_id = i + 1;
            type = elem_type(elem_id);                        /* macro */
            switch(type){
              case EDGE_ELEM:
                elem = local_edge_id(elem_id);                  /* macro */
                fe_edge = &(edge_array[elem - 1]);
                entity = fe_edge -> entity;
                weight = entity -> edge_weight;
                break;
              case FACE_ELEM:
                elem = local_face_id(elem_id);                  /* macro */
                fe_face = &(face_array[elem - 1]);
                entity = fe_face -> entity;
                weight = entity -> face_weight;
                break;
              case QUAD_ELEM:
                elem = local_quad_id(elem_id);                  /* macro */
                fe_quad = &(quad_array[elem - 1]);
                entity = fe_quad -> entity;
                weight = entity -> quad_weight;
                break;
              case TETRA_ELEM:
                elem = local_tetra_id(elem_id);                 /* macro */
                fe_tetra = &(tetra_array[elem - 1]);
                entity = fe_tetra -> entity;
                weight = entity -> tetra_weight;
                break;
              case PYRAM_ELEM:
                elem = local_pyram_id(elem_id);                 /* macro */
                fe_pyram = &(pyram_array[elem - 1]);
                entity = fe_pyram -> entity;
                weight = entity -> pyram_weight;
                break;
              case WEDGE_ELEM:
                elem = local_wedge_id(elem_id);                 /* macro */
                fe_wedge = &(wedge_array[elem - 1]);
                entity = fe_wedge -> entity;
                weight = entity -> wedge_weight;
                break;
              case HEXA_ELEM:
                elem = local_hexa_id(elem_id);                  /* macro */
                fe_hexa = &(hexa_array[elem - 1]);
                entity = fe_hexa -> entity;
                weight = entity -> hexa_weight;
                break;
            }

            if(weight == 0.0){
              weight = entity -> elem_weight;
              if(weight == 0.0){
                switch(type){
                  case EDGE_ELEM:
                    switch(entity -> type){
                      case CURVE_ENTITY:
                        sprintf(err_msg, "Element weight specification for edges on %s %ld missing", keyword[CURVE], entity -> id);
                        break;
                      case INTERFACE_ENTITY:
                        sprintf(err_msg, "Element weight specification for edge on %s %ld missing", keyword[INTERFACE], entity -> id);
                        break;
                      default:
                        break;
                    }
                    break;
                  case FACE_ELEM:
                    switch(entity -> type){
                      case SURFACE_ENTITY:
                        sprintf(err_msg, "Element weight specification for faces on %s %ld missing", keyword[SURFACE], entity -> id);
                        break;
                      case PATCH_ENTITY:
                        sprintf(err_msg, "Element weight specification for faces on %s %ld missing", keyword[PATCH], entity -> id);
                        break;
                      case SHELL_ENTITY:
                        sprintf(err_msg, "Element weight specification for faces on %s %ld missing", keyword[SHELL], entity -> id);
                        break;
                      default:
                        break;
                    }
                    break;
                  case QUAD_ELEM:
                    switch(entity -> type){
                      case SURFACE_ENTITY:
                        sprintf(err_msg, "Element weight specification for quads on %s %ld missing", keyword[SURFACE], entity -> id);
                        break;
                      case PATCH_ENTITY:
                        sprintf(err_msg, "Element weight specification for quads on %s %ld missing", keyword[PATCH], entity -> id);
                        break;
                      case SHELL_ENTITY:
                        sprintf(err_msg, "Element weight specification for quads on %s %ld missing", keyword[SHELL], entity -> id);
                        break;
                      case INTERFACE_ENTITY:
                        sprintf(err_msg, "Element weight specification for quads on %s %ld missing", keyword[INTERFACE], entity -> id);
                        break;
                      default:
                        break;
                    }
                    break;
                  case TETRA_ELEM:
                    sprintf(err_msg, "Element weight specification for tetras on %s %ld missing", keyword[REGION], entity -> id);
                    break;
                  case PYRAM_ELEM:
                    sprintf(err_msg, "Element weight specification for pyrams on %s %ld missing", keyword[REGION], entity -> id);
                    break;
                  case WEDGE_ELEM:
                    switch(entity -> type){
                      case REGION_ENTITY:
                        sprintf(err_msg, "Element weight specification for wedges on %s %ld missing", keyword[REGION], entity -> id);
                        break;
                      case INTERFACE_ENTITY:
                        sprintf(err_msg, "Element weight specification for wedges on %s %ld missing", keyword[INTERFACE], entity -> id);
                        break;
                      default:
                        break;
                    }
                    break;
                  case HEXA_ELEM:
                    switch(entity -> type){
                      case REGION_ENTITY:
                        sprintf(err_msg, "Element weight specification for hexas on %s %ld missing", keyword[REGION], entity -> id);
                        break;
                      case INTERFACE_ENTITY:
                        sprintf(err_msg, "Element weight specification for hexas on %s %ld missing", keyword[INTERFACE], entity -> id);
                        break;
                      default:
                        break;
                    }
                    break;
                }
                error_message(err_msg, GENERAL_ERROR);
              }
            }
            weight_elem[i] = weight;
          }
        }

#ifdef CHECK_GRAPH
#ifdef METIS_CHECK
        if((graph_file = fopen("metis_graph", "w")) == NULL){
          sprintf(err_msg, "File %s opening error", "metis_graph");
          error_message(err_msg, FILE_OPEN_ERROR);
        }
        active_out_file = graph_file;

        if(weight_flag == 0){
          fprintf(graph_file, "%ld %ld %d\n", elems, (long)(elem_num_elems[elems] / 2), 0);

          for(i = 0; i < elems; i++){
            for(j = elem_num_elems[i] - 1; j < elem_num_elems[i + 1] - 1; j++){
              fprintf(graph_file, "%ld ", (long)elem_con_elems[j]);
            }
            fprintf(graph_file, "\n");
          }
        }
        if(weight_flag == 2){
          fprintf(graph_file, "%ld %ld %d\n", elems, (long)(elem_num_elems[elems] / 2), 10);

          for(i = 0; i < elems; i++){
            fprintf(graph_file, "%d ", (int)weight_elem[i]);
            for(j = elem_num_elems[i] - 1; j < elem_num_elems[i + 1] - 1; j++){
              fprintf(graph_file, "%ld ", (long)elem_con_elems[j]);
            }
            fprintf(graph_file, "\n");
          }
        }

        fclose(graph_file);

        /* in graphchk.c no value is return;
           unless the source is modified, then the exit code of the following command is uncertain */

        if(system("/home/dr/Metis/graphchk metis_graph") != 0)exit(1);
#endif
#ifdef MY_CHECK
        if((graph_file = fopen("my_graph", "w")) == NULL){
          sprintf(err_msg, "File %s opening error", "my_graph");
          error_message(err_msg, FILE_OPEN_ERROR);
        }
        active_out_file = graph_file;

        fprintf(graph_file, "%ld %ld\n", elems, (long)(elem_num_elems[elems]));

        for(i = 0; i < elems; i++){
          fprintf(graph_file, "%ld ", (long)(elem_num_elems[i + 1] - elem_num_elems[i]));
          for(j = elem_num_elems[i] - 1; j < elem_num_elems[i + 1] - 1; j++){
            fprintf(graph_file, "%ld ", (long)elem_con_elems[j]);
          }
          fprintf(graph_file, "\n");
        }

        fclose(graph_file);

        if(system("/home/dr/Convert/t3d2sifel/check_graph my_graph") != 0)exit(1);
#endif
#endif

        //ksaha: until before partition starts  
        double t2 = omp_get_wtime();
        printf("Pre-partition = %f seconds\n", t2-t1);


        // WHERE WE NEED TO CHANGE THE CODE
        // partition:

        //ksaha: starts - partitioning (case-1) 
        if(nparts <= 8){
          METIS_PartGraphRecursive(&graph_size, elem_num_elems, elem_con_elems,
              weight_elem, NULL, &weight_flag,
              &num_flag, &nparts, options, &edge_cut,
              elem_part);
        } else {
          // Specifically here
          // Rank 0 broadcast needed info to slaves
          // slaves allocate some information

          // change to ParMETIS
          METIS_PartGraphKway(&graph_size, elem_num_elems, elem_con_elems,
              weight_elem, NULL, &weight_flag,
              &num_flag, &nparts, options, &edge_cut,
              elem_part);

          // new partition in elem_part
          // Deallocate any allocated information.
        }
        double t2b = omp_get_wtime();
        printf("Partition partI = %f seconds\n", t2b-t2);
        //ksaha: ends - partitioning (case-1) 

        // if rank != 0 goto finalize:

        if(mixed == YES && node_weight == YES){
          if(weight_elem != NULL)free(weight_elem);
        }
        if(elem_weight == YES){
          if(weight_elem != NULL)free(weight_elem);
        }

#ifdef DEBUG
        fprintf(stdout, "Partitions (elems)1:");
        for(i = 0; i < elems; i++)fprintf(stdout, " %ld", (long)(elem_part[i]));
        fprintf(stdout, "\n");
#endif

      }   // ends "profile_opt == NO"

      double t2c = omp_get_wtime();

      if(shared_partition == YES){
        if((part_nodes = (long *)calloc(nparts, sizeof(long))) == NULL)
          error_message("Memory allocation error", MEMORY_ERROR);
      }

      /* node cut - node_part: positive partition number for nodes inside partition
         negative number of shared partitions for shared nodes;
         positive number of partitions + 1 for isolated nodes */

      fe_Node = node_array;
      for(i = 0; i < nodes + midnodes; i++, fe_Node++){
        if(node_num_elems[i + 1] - node_num_elems[i] == 0){
          node_part[i] = nparts + 1;         /* isolated nodes */
          fe_Node -> id = ++isolated;
          continue;
        }
        count = 0;
        elem = node_con_elems[node_num_elems[i]];
        part = elem_part[elem - 1];
        for(j = node_num_elems[i] + 1; j < node_num_elems[i + 1]; j++){
          elem = node_con_elems[j] - 1;
          if(elem_part[elem] != part){
            if(part_flag[elem_part[elem] - 1] == NO){
              count++;
              part_flag[elem_part[elem] - 1] = YES;
            }
          }
        }
        if(count == 0){
          node_part[i] = part;
          if(shared_partition == YES)part_nodes[part - 1]++;
        }
        else{
          node_part[i] = -count - 1;
          for(j = 0; j < nparts; j++)part_flag[j] = NO;
          if(shared_partition == YES)shared_nodes++;
        }
      }


      double t2d = omp_get_wtime();
      printf("Postpartition - not parallelizable = %f seconds\n", t2d-t2c);

      if(shared_partition == YES){
        if(shared_nodes != 0){

          postpartition(shared_nodes, part_nodes);

        }  

#ifdef DEBUG
        fprintf(stdout, "Partitions (nodes total):\n");
        for(part = 0; part < nparts; part++)fprintf(stdout, " %3d: %ld\n", part + 1, part_nodes[part]);
        fprintf(stdout, "\n");
#endif

        free(part_nodes);

      }   //ends "shared_partition == YES"

#ifdef DEBUG
      fprintf(stdout, "Partitions (nodes)2:");
      for(i = 0; i < nodes + midnodes; i++)fprintf(stdout, " %ld", (long)(node_part[i]));
      fprintf(stdout, "\n");
#endif

    } /* node_cut */

    //ksaha: takeoff1.c	-- starts

    if(elem_cut == YES && profile_opt == NO){
      graph_size = nodes;

      //ksaha: starts - partitioning (case-2) 
      clock_t time2c = clock();
      if(nparts <= 8) {
        METIS_PartGraphRecursive(&graph_size, node_num_nodes, node_con_nodes,
            NULL, NULL, &weight_flag,
            &num_flag, &nparts, options,
            &edge_cut, node_part);
      } else {
        METIS_PartGraphKway(&graph_size, node_num_nodes, node_con_nodes,
            NULL, NULL, &weight_flag,
            &num_flag, &nparts, options,
            &edge_cut, node_part);
      }
      clock_t time2d = clock();
      printf("Partition partII: starting, end time, runtime = %f %f %f (in seconds) \n", ((double)time2c)/CLOCKS_PER_SEC,
          ((double)time2d)/CLOCKS_PER_SEC, ((double)(time2d - time2c))/CLOCKS_PER_SEC);
      //ksaha: ends - partitioning (case-2) 

      //ksaha: starts - part4
      clock_t time_4a = clock();
      printf("+++++++time_4a = %f (in seconds) \n", ((double)time_4a)/CLOCKS_PER_SEC);

#ifdef DEBUG
      fprintf(stdout, "Partitions (nodes)3:");
      for(i = 0; i < nodes; i++)fprintf(stdout, " %ld", (long)(node_part[i]));
      fprintf(stdout, "\n");
#endif

      for(i = 0; i < elems; i++){
        count = 0;
        elem = i + 1;
        type = elem_type(elem);                        /* macro */
        switch(type){
          case EDGE_ELEM:
            elem = local_edge_id(elem);                  /* macro */
            fe_edge = &(edge_array[elem - 1]);
            part = node_part[fe_edge -> node[0] -> id - 1];
            for(j = 1; j < 2; j++){
              node = fe_edge -> node[j] -> id - 1;
              if(node_part[node] != part){
                if(part_flag[node_part[node] - 1] == NO){
                  count++;
                  part_flag[node_part[node] - 1] = YES;
                }
              }
            }
            break;
          case FACE_ELEM:
            elem = local_face_id(elem);                  /* macro */
            fe_face = &(face_array[elem - 1]);
            part = node_part[fe_face -> node[0] -> id - 1];
            for(j = 1; j < 3; j++){
              node = fe_face -> node[j] -> id - 1;
              if(node_part[node] != part){
                if(part_flag[node_part[node] - 1] == NO){
                  count++;
                  part_flag[node_part[node] - 1] = YES;
                }
              }
            }
            break;
          case QUAD_ELEM:
            elem = local_quad_id(elem);                  /* macro */
            fe_quad = &(quad_array[elem - 1]);
            part = node_part[fe_quad -> node[0] -> id - 1];
            for(j = 1; j < 4; j++){
              node = fe_quad -> node[j] -> id - 1;
              if(node_part[node] != part){
                if(part_flag[node_part[node] - 1] == NO){
                  count++;
                  part_flag[node_part[node] - 1] = YES;
                }
              }
            }
            break;
          case TETRA_ELEM:
            elem = local_tetra_id(elem);                 /* macro */
            fe_tetra = &(tetra_array[elem - 1]);
            part = node_part[fe_tetra -> node[0] -> id - 1];
            for(j = 1; j < 4; j++){
              node = fe_tetra -> node[j] -> id - 1;
              if(node_part[node] != part){
                if(part_flag[node_part[node] - 1] == NO){
                  count++;
                  part_flag[node_part[node] - 1] = YES;
                }
              }
            }
            break;
          case PYRAM_ELEM:
            elem = local_pyram_id(elem);                 /* macro */
            fe_pyram = &(pyram_array[elem - 1]);
            part = node_part[fe_pyram -> node[0] -> id - 1];
            for(j = 1; j < 5; j++){
              node = fe_pyram -> node[j] -> id - 1;
              if(node_part[node] != part){
                if(part_flag[node_part[node] - 1] == NO){
                  count++;
                  part_flag[node_part[node] - 1] = YES;
                }
              }
            }
            break;
          case WEDGE_ELEM:
            elem = local_wedge_id(elem);                 /* macro */
            fe_wedge = &(wedge_array[elem - 1]);
            part = node_part[fe_wedge -> node[0] -> id - 1];
            for(j = 1; j < 6; j++){
              node = fe_wedge -> node[j] -> id - 1;
              if(node_part[node] != part){
                if(part_flag[node_part[node] - 1] == NO){
                  count++;
                  part_flag[node_part[node] - 1] = YES;
                }
              }
            }
            break;
          case HEXA_ELEM:
            elem = local_hexa_id(elem);                  /* macro */
            fe_hexa = &(hexa_array[elem - 1]);
            part = node_part[fe_hexa -> node[0] -> id - 1];
            for(j = 1; j < 8; j++){
              node = fe_hexa -> node[j] -> id - 1;
              if(node_part[node] != part){
                if(part_flag[node_part[node] - 1] == NO){
                  count++;
                  part_flag[node_part[node] - 1] = YES;
                }
              }
            }
            break;
        }

        if(count == 0)
          elem_part[i] = part;
        else{
          elem_part[i] = -count - 1;
          for(j = 0; j < nparts; j++)part_flag[j] = NO;
        }
      }

#ifdef DEBUG
      fprintf(stdout, "Partitions (elems)4:");
      for(i = 0; i < elems; i++)fprintf(stdout, " %ld", (long)(elem_part[i]));
      fprintf(stdout, "\n");
#endif

    } /* elem_cut */

    //ksaha: takeoff1.c	-- ends

  }
#endif

  if(renum == YES || interface_last == YES){
    if((nd_array = (node_rec *)calloc(nodes + midnodes, sizeof(node_rec))) == NULL)
      error_message("Memory allocation error", MEMORY_ERROR);
  }

  // nparts == 1 starts here	
  if(nparts == 1){
    //ksaha - takeoff2.c -- starts
    if(renum == YES){
      for(i = 0; i < nodes + midnodes; i++){
        nd_array[i].id = i + 1;
        nd_array[i].pos = i;
        nd_array[i].contact = nd_num_nodes[i + 1] - nd_num_nodes[i];
        nd_array[i].connection = &(nd_con_nodes[nd_num_nodes[i]]);
      }

      status = renumber_mesh(nodes + midnodes, nd_array, 0, &old_profile, &new_profile);
      if(status != 0)
        error_message("Renumbering error", RENUMBER_ERROR);

      fprintf(stdout, "Nominal profile: %.0f (old) --> %.0f (new)\n", old_profile, new_profile);

      fe_Node = node_array;
      for(i = 0; i < nodes + midnodes; i++, fe_Node++)fe_Node -> id = nd_array[i].id;
    }  // ends of renum == YES

    if((sifel_in_file = fopen(sifel_in_name, "w")) == NULL){
      sprintf(err_msg, "File %s opening error", sifel_in_name);
      error_message(err_msg, FILE_OPEN_ERROR);
    }
    active_out_file = sifel_in_file;
    sprintf(out_err_msg, "File %s writing error", sifel_in_name);

    if(fprintf(sifel_in_file, " %9d %9d %9d %9d %9d\n", mesh_type, elem_degree, renum_type, output_type, 0) < 0)
      error_message(out_err_msg, FILE_WRITE_ERROR);
    switch(mesh_type){
      case TRI_TETRA:
        if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld\n\n", nodes + midnodes, edges, faces, tetras) < 0)
          error_message(out_err_msg, FILE_WRITE_ERROR);
        break;
      case QUAD_HEXA:
        if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld\n\n", nodes + midnodes, edges, quads, hexas) < 0)
          error_message(out_err_msg, FILE_WRITE_ERROR);
        break;
      case TRI_TETRA + QUAD_HEXA:
        if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld\n\n", 
              nodes + midnodes, edges, faces, quads, tetras, pyrams, wedges, hexas) < 0)
          error_message(out_err_msg, FILE_WRITE_ERROR);
    }

    fe_Node = node_array;
    for(i = 0; i < nodes + midnodes; i++, fe_Node++){
      if(renum == YES)
        fe_nd = node_array + nd_array[i].pos;
      else
        fe_nd = fe_Node;
      entity = fe_nd -> entity;
      if(fprintf(sifel_in_file, " %9ld %14.6e %14.6e %14.6e %4d %9ld %9ld\n", 
            fe_nd -> id, fe_nd -> x, fe_nd -> y, fe_nd -> z,
            entity -> type, entity -> id, entity -> property_id) < 0)
        error_message(out_err_msg, FILE_WRITE_ERROR);
    }

    last_type = -1;
    for(i = 0; i < elems; i++){  // starts loop i
      glob_elem_id = i + 1;
      type = elem_type(glob_elem_id);                        /* macro */
      if(last_type != type){
        if(fprintf(sifel_in_file, "\n") < 0)error_message(out_err_msg, FILE_WRITE_ERROR);
      }
      switch(type){
        case EDGE_ELEM:
          elem = local_edge_id(glob_elem_id);                  /* macro */
          fe_edge = &(edge_array[elem - 1]);
          entity = fe_edge -> entity;

          for(j = 0; j < 2; j++)nd_id[j] = fe_edge -> node[j] -> id;

          if(elem_degree == LINEAR){
            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %4d %9ld %9ld\n", 
                  glob_elem_id, nd_id[0], nd_id[1],
                  entity -> type, entity -> id, entity -> property_id) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          if(elem_degree == QUADRATIC){
            if(fe_edge -> midnd[0] != NULL)
              midnd_id[0] = fe_edge -> midnd[0] -> id;
            else
              midnd_id[0] = 0;

            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %4d %9ld %9ld\n", 
                  glob_elem_id, nd_id[0], nd_id[1], midnd_id[0],
                  entity -> type, entity -> id, entity -> property_id) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          break;
        case FACE_ELEM:
          elem = local_face_id(glob_elem_id);                  /* macro */
          fe_face = &(face_array[elem - 1]);
          entity = fe_face -> entity;

          for(j = 0; j < 3; j++)nd_id[j] = fe_face -> node[j] -> id;
          for(j = 0; j < 3; j++){
            curve_id[j] = 0;
            if((fe_face -> bflag & (1 << j)) != 0)curve_id[j] = fe_face -> data[j].bound_ent -> id;
          }

          if(elem_degree == LINEAR){
            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2],
                  entity -> type, entity -> id, entity -> property_id,
                  curve_id[0], curve_id[1], curve_id[2]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          if(elem_degree == QUADRATIC){
            for(j = 0; j < 3; j++)midnd_id[j] = fe_face -> midnd[j] -> id;

            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], midnd_id[0], midnd_id[1], midnd_id[2],
                  entity -> type, entity -> id, entity -> property_id,
                  curve_id[0], curve_id[1], curve_id[2]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          break;
        case QUAD_ELEM:
          elem = local_quad_id(glob_elem_id);                  /* macro */
          fe_quad = &(quad_array[elem - 1]);
          entity = fe_quad -> entity;

          for(j = 0; j < 4; j++)nd_id[j] = fe_quad -> node[j] -> id;
          for(j = 0; j < 4; j++){
            curve_id[j] = 0;
            if((fe_quad -> bflag & (1 << j)) != 0)curve_id[j] = fe_quad -> data[j].bound_ent -> id;
          }

          if(elem_degree == LINEAR){
            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3],
                  entity -> type, entity -> id, entity -> property_id,
                  curve_id[0], curve_id[1], curve_id[2], curve_id[3]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          if(elem_degree == QUADRATIC){
            for(j = 0; j < 4; j++){
              if(fe_quad -> midnd[j] != NULL)
                midnd_id[j] = fe_quad -> midnd[j] -> id;
              else
                midnd_id[j] = 0;
            }

            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], 
                  midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3],
                  entity -> type, entity -> id, entity -> property_id,
                  curve_id[0], curve_id[1], curve_id[2], curve_id[3]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          break;
        case TETRA_ELEM:
          elem = local_tetra_id(glob_elem_id);                 /* macro */
          fe_tetra = &(tetra_array[elem - 1]);
          entity = fe_tetra -> entity;

          for(j = 0; j < 4; j++)nd_id[j] = fe_tetra -> node[j] -> id;
          for(j = 0; j < 4; j++){
            bound_ent_id[j] = 0;
            bound_ent_tp[j] = 0;
            if((fe_tetra -> bflag & (1 << j)) != 0){
              bound_ent_id[j] = fe_tetra -> data[j].bound_ent -> id;
              bound_ent_tp[j] = fe_tetra -> data[j].bound_ent -> type;
            }
          }

          if(elem_degree == LINEAR){
            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          if(elem_degree == QUADRATIC){
            for(j = 0; j < 6; j++)midnd_id[j] = fe_tetra -> midnd[j] -> id;

            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3],
                  midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], midnd_id[5],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          break;
        case PYRAM_ELEM:
          elem = local_pyram_id(glob_elem_id);                 /* macro */
          fe_pyram = &(pyram_array[elem - 1]);
          entity = fe_pyram -> entity;

          for(j = 0; j < 5; j++)nd_id[j] = fe_pyram -> node[j] -> id;
          for(j = 0; j < 5; j++){
            bound_ent_id[j] = 0;
            bound_ent_tp[j] = 0;
            if((fe_tetra -> bflag & (1 << j)) != 0){
              bound_ent_id[j] = fe_tetra -> data[j].bound_ent -> id;
              bound_ent_tp[j] = fe_tetra -> data[j].bound_ent -> type;
            }
          }

          if(elem_degree == LINEAR){
            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          if(elem_degree == QUADRATIC){
            for(j = 0; j < 8; j++)midnd_id[j] = fe_pyram -> midnd[j] -> id;

            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4],
                  midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], midnd_id[5], midnd_id[6], midnd_id[7],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          break;
        case WEDGE_ELEM:
          elem = local_wedge_id(glob_elem_id);                 /* macro */
          fe_wedge = &(wedge_array[elem - 1]);
          entity = fe_wedge -> entity;

          for(j = 0; j < 6; j++)nd_id[j] = fe_wedge -> node[j] -> id;
          for(j = 0; j < 5; j++){
            bound_ent_id[j] = 0;
            bound_ent_tp[j] = 0;
            if((fe_wedge -> bflag & (1 << j)) != 0){
              bound_ent_id[j] = fe_wedge -> data[j].bound_ent -> id;
              bound_ent_tp[j] = fe_wedge -> data[j].bound_ent -> type;
            }
          }

          if(elem_degree == LINEAR){
            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          if(elem_degree == QUADRATIC){
            for(j = 0; j < 9; j++){
              if(fe_wedge -> midnd[j] != NULL)
                midnd_id[j] = fe_wedge -> midnd[j] -> id;
              else
                midnd_id[j] = 0;
            }

            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5],
                  midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], 
                  midnd_id[5], midnd_id[6], midnd_id[7], midnd_id[8],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          break;
        case HEXA_ELEM:
          elem = local_hexa_id(glob_elem_id);                 /* macro */
          fe_hexa = &(hexa_array[elem - 1]);
          entity = fe_hexa -> entity;

          for(j = 0; j < 8; j++)nd_id[j] = fe_hexa -> node[j] -> id;
          for(j = 0; j < 6; j++){
            bound_ent_id[j] = 0;
            bound_ent_tp[j] = 0;
            if((fe_hexa -> bflag & (1 << j)) != 0){
              bound_ent_id[j] = fe_hexa -> data[j].bound_ent -> id;
              bound_ent_tp[j] = fe_hexa -> data[j].bound_ent -> type;
            }
          }

          if(elem_degree == LINEAR){
            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5], nd_id[6], nd_id[7],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4], bound_ent_id[5],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4], bound_ent_tp[5]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          if(elem_degree == QUADRATIC){
            for(j = 0; j < 12; j++){
              if(fe_hexa -> midnd[j] != NULL)
                midnd_id[j] = fe_hexa -> midnd[j] -> id;
              else
                midnd_id[j] = 0;
            }

            if(fprintf(sifel_in_file, " %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %9ld %9ld %9ld %9ld %9ld %9ld %9ld %9ld %4d %4d %4d %4d %4d %4d\n", 
                  glob_elem_id, nd_id[0], nd_id[1], nd_id[2], nd_id[3], nd_id[4], nd_id[5], nd_id[6], nd_id[7],
                  midnd_id[0], midnd_id[1], midnd_id[2], midnd_id[3], midnd_id[4], midnd_id[5], 
                  midnd_id[6], midnd_id[7], midnd_id[8], midnd_id[9], midnd_id[10], midnd_id[11],
                  entity -> type, entity -> id, entity -> property_id,
                  bound_ent_id[0], bound_ent_id[1], bound_ent_id[2], bound_ent_id[3], bound_ent_id[4], bound_ent_id[5],
                  bound_ent_tp[0], bound_ent_tp[1], bound_ent_tp[2], bound_ent_tp[3], bound_ent_tp[4], bound_ent_tp[5]) < 0)
              error_message(out_err_msg, FILE_WRITE_ERROR);
          }
          break;
      }
      last_type = type;
    } // ends loop i 

    fclose(sifel_in_file);

    //ksaha - takeoff2.c -- ends

  } /* end nparts == 1 */

#ifdef METIS
  else{
    double tt4 = omp_get_wtime();
    mthread(isolated, 
            old_profile, 
            new_profile, 
            mesh_type, 
            elem_degree, 
            renum_type, 
            output_type, 
            elixir, 
            white, 
            black, 
            colors, 
            sifel_in_name,
            print_map, 
            map_filename, 
            nthreads);
            
    double tt5 = omp_get_wtime();
    printf("Postpartition3 - parallelized = %f seconds\n", tt5-tt4);
  }   // ends - else
#endif

  vertex_list = deep_destroy_list(vertex_list);
  curve_list = deep_destroy_list(curve_list);
  surface_list = deep_destroy_list(surface_list);
  patch_list = deep_destroy_list(patch_list);
  shell_list = deep_destroy_list(shell_list);
  region_list = deep_destroy_list(region_list);
  interface_list = deep_destroy_list(interface_list);

  if(node_array != NULL)free(node_array);
  if(edge_array != NULL)free(edge_array);
  if(face_array != NULL)free(face_array);
  if(quad_array != NULL)free(quad_array);
  if(tetra_array != NULL)free(tetra_array);
  if(pyram_array != NULL)free(pyram_array);
  if(wedge_array != NULL)free(wedge_array);
  if(hexa_array != NULL)free(hexa_array);

  if(renum == YES || nparts > 1){
    free(node_num_elems);
    free(node_con_elems);
  }

  if(renum == YES){
    free(nd_num_nodes);
    free(nd_con_nodes);
    if(nparts > 1)
      free(nd_con_nds);
  }

  if(renum == YES || interface_last == YES)
    free(nd_array);

#ifdef METIS	
  if(nparts > 1){
    if(node_cut == YES || profile_opt == YES){
      free(node_num_nodes);
      free(node_con_nodes);
    }
    if(elem_cut == YES || profile_opt == YES){
      free(elem_num_elems);
      free(elem_con_elems);
    }

    free(node_part);
    free(elem_part);

    free(remote_node);
    free(remote_elem);

    free(part_flag);

#ifdef ELIXIR
    if(elixir == YES){
      if(virtual_layers == NO){
        for(i = 1; i <= nparts; i++){
          EVSetLayerOnOff(view, i, YES);
          layers[i] = YES;
        }
      }
      else{
        EVSetLayerOnOff(view, ACTIVE_LAYER, YES);
        for(i = 1; i <= nparts; i++)layers[i] = YES;
      }
      EVFitAllIntoView(view);

      ERptStatusMessage(1, 1, "Domain decomposition");
      ESIEventLoop(YES, NULL);
    }
#endif

  }
#endif
  printf("Total Computation Time = %f seconds \n", omp_get_wtime()-t1);

  return(0);
}

// end of main code


  static list *
add_item_to_list_head(list *any_list, void *item)
{
  list *lst = NULL;

  lst = (list *)malloc(sizeof(struct list));
  if(lst == NULL)error_message("Memory allocation error", MEMORY_ERROR);

  lst -> item = item;
  lst -> next = any_list;

  return(lst);
}

  static list *
deep_destroy_list(list *any_list)
{
  list *lst = NULL;

  while(any_list != NULL){
    if(any_list -> item != NULL)free(any_list -> item);
    any_list = (lst = any_list) -> next;
    free(lst);
  }
  return(NULL);
}

  static list *
destroy_list(list *any_list)
{
  list *lst = NULL;

  while(any_list != NULL){
    any_list = (lst = any_list) -> next;
    free(lst);
  }
  return(NULL);
}

  static list *
reverse_list(list *any_list)
{
  list *head = NULL, *lst = NULL;

  if(any_list == NULL)return(NULL);
  if(any_list -> next == NULL)return(any_list);
  head = NULL;
  while((lst = any_list) != NULL){
    any_list = any_list -> next;
    lst -> next = head;
    head = lst;
  }
  return(head);
}

  static long
get_list_size(list *any_list)
{
  long count = 0L;

  while(any_list != NULL){
    any_list = any_list -> next;
    count++;
  }
  return(count);
}




  static char *
get_next_record(char *err_msg)
{
  char *buf = NULL;

  if((buf = fgets(line_buffer, BUFFER_SIZE, active_in_file)) == NULL){
    if(!feof(active_in_file))error_message("Reading error", FILE_READ_ERROR);
    if(err_msg == NULL)return(NULL);
    error_message(err_msg, GENERAL_ERROR);
  }
  return(buf);
}

  static char *
get_next_relevant_record(char *err_msg)
{
  char *buf = NULL, *character = NULL;

  do{
    if((buf = get_next_record(err_msg)) == NULL)return(NULL);
    character = line_buffer;
    while(*character == ' ')character++;
  }while(*character == '\n' || *character == REMARK);
  return(buf);
}

  static char *
get_first_token(char *buffer)
{
  return(strtok(buffer, SEPARATOR));
}

  static char *
get_next_first_token(char *err_msg)
{
  char *token = NULL, *buf = NULL;

  while(YES){
    if((buf = get_next_relevant_record(err_msg)) == NULL)return(NULL);
    if((token = strtok(buf, SEPARATOR)) == NULL)continue;
    break;
  }
  return(token);
}

  static char *
get_next_token(char *err_msg)
{
  char *token = NULL;

  if((token = strtok(NULL, SEPARATOR)) == NULL){
    if(err_msg == NULL)return(NULL);
    error_message(err_msg, FILE_READ_ERROR);
  }
  if(*token == REMARK)return(NULL);
  return(token);
}


  static int
get_token_key(char *token, char keyword[1][KEY_SIZE], int keynum, key_case type)
{
  char *t = token;
  int key;

  if(type == UPPER_CASE){
    while(*t != '\0'){
      *t = (char)toupper((int)*t);
      t++;
    }
  }
  if(type == LOWER_CASE){
    while(*t != '\0'){
      *t = (char)tolower((int)*t);
      t++;
    }
  }

  for(key = 0; key < keynum; key++){
    if(strcmp(token, keyword[key]) == 0)
      return(key);
  }
  return(keynum);
}


  long
get_next_int(char *err_msg)
{
  return(get_int_from_token(get_next_token(err_msg)));
}

  double
get_next_fpn(char *err_msg)
{
  return(get_fpn_from_token(get_next_token(err_msg)));
}


  long
get_int_from_token(char *token)
{
  char *t_ptr = token, *t = token, err_msg[128];
  long int_num;

  int_num = strtol(token, &t_ptr, 10);
  if(token == t_ptr || *t_ptr != '\0'){
    sprintf(err_msg, "Integer number expected (\"%s\" - read)", t);
    error_message(err_msg, GENERAL_ERROR);
  }
  return(int_num);
}

  double
get_fpn_from_token(char *token)
{
  char *t_ptr = token, *t = token, err_msg[128];
  double fpn_num;

  fpn_num = strtod(token, &t_ptr);
  if(token == t_ptr || *t_ptr != '\0'){
    sprintf(err_msg, "Floating point number expected (\"%s\" - read)", t);
    error_message(err_msg, GENERAL_ERROR);
  }
  return(fpn_num);
}


  static void
write_current_record(char *err_msg)
{
  if(fprintf(active_out_file, "%s", line_buffer) < 0)
    error_message(err_msg, FILE_WRITE_ERROR);
}

  static void
read_write_next_record(char *in_err_msg, char *out_err_msg)
{
  get_next_record(in_err_msg);
  write_current_record(out_err_msg);
}

  static void
read_write_next_relevant_record(char *in_err_msg, char *out_err_msg)
{
  get_next_relevant_record(in_err_msg);
  write_current_record(out_err_msg);
}

  static void
read_write_unrelevant_records(char *in_err_msg, char *out_err_msg)
{
  char *character = NULL;

  while(YES){
    get_next_record(in_err_msg);
    character = line_buffer;
    while(*character == ' ')character++;
    if(*character != '\n' && *character != REMARK)break;
    write_current_record(out_err_msg);
  }
}

  static void
read_write_until_next_relevant_record(char *in_err_msg, char *out_err_msg)
{
  read_write_unrelevant_records(in_err_msg, out_err_msg);
  write_current_record(out_err_msg);
}



  static entity *
get_entity_id(list *entity_list, long id)
{
  entity *entity = NULL;

  while(entity_list != NULL){
    entity = (struct entity *)(entity_list -> item);
    entity_list = entity_list -> next;
    if(entity -> id == id)return(entity);
  }
  return(NULL);
}

  static entity *
create_entity_id(long id)
{
  entity *entity = NULL;

  entity = (struct entity *)malloc(sizeof(struct entity));
  if(entity == NULL)error_message("Memory allocation error", MEMORY_ERROR);

  entity -> id = id;
  entity -> type = 0;
  entity -> property_id = 0;
  entity -> nodes = 0;
  entity -> elems = 0;
  entity -> first_node = 0;
  entity -> first_edge = 0;
  entity -> first_face = 0;
  entity -> first_quad = 0;
  entity -> first_tetra = 0;
  entity -> first_pyram = 0;
  entity -> first_wedge = 0;
  entity -> first_hexa = 0;
  entity -> elem_weight = 0.0;
  entity -> face_weight = 0.0;
  entity -> quad_weight = 0.0;
  entity -> tetra_weight = 0.0;
  entity -> pyram_weight = 0.0;
  entity -> wedge_weight = 0.0;
  entity -> hexa_weight = 0.0;
  entity -> width = 0.0;
  entity -> boundary = NO;
  entity -> output = NO;
  entity -> ngb_ent[0] = NULL;
  entity -> ngb_ent[1] = NULL;
  entity -> selected = NO;

  return(entity);
}


  static entity *
create_missing_entity_id(long id, int entity_type)
{
  entity *entity = NULL;

  entity = create_entity_id(id);
  entity -> type = entity_type;
  switch(entity_type){
    case VERTEX_ENTITY:
      vertex_list = add_item_to_list_head(vertex_list, entity);
      break;
    case CURVE_ENTITY:
      curve_list = add_item_to_list_head(curve_list, entity);
      break;
    case SURFACE_ENTITY:
      surface_list = add_item_to_list_head(surface_list, entity);
      break;
    case PATCH_ENTITY:
      patch_list = add_item_to_list_head(patch_list, entity);
      break;
    case SHELL_ENTITY:
      shell_list = add_item_to_list_head(shell_list, entity);
      break;
    case REGION_ENTITY:
      region_list = add_item_to_list_head(region_list, entity);
      break;
    case INTERFACE_ENTITY:
      interface_list = add_item_to_list_head(interface_list, entity);
      break;
  }
  return(entity);
}



  static void
exit_error_message(char *message, int exit_code, int line)
{
  if(exit_code == FILE_READ_ERROR)
    fprintf(stderr, "\nt3d2psifel: line %d\n%s\n\n", line, message);
  else
    fprintf(stderr, "\nt3d2psifel: line %d\n%s\n\n", line, message);
  exit(exit_code);
}


  static void
mark_remote_elems(fe_node *node, long node_id, int part_id, long property_id, double width)
{
  int type;
  long i, j, k, elem, el, nd_id;
  fe_node *nd = NULL;
  fe_edge *fe_edge = NULL;
  fe_face *fe_face = NULL;
  fe_quad *fe_quad = NULL;
  fe_tetra *fe_tetra = NULL;
  fe_pyram *fe_pyram = NULL;
  fe_wedge *fe_wedge = NULL;
  fe_hexa *fe_hexa = NULL;
  double dist;

  /* for node_cut it is possible to use negative remote_elem as flag for processed elements */

  for(i = node_num_elems[node_id - 1]; i < node_num_elems[node_id]; i++){
    elem = node_con_elems[i] - 1;
    if(node_cut == YES){
      if(abs(elem_part[elem]) == part_id)continue;
    }
    if(elem_cut == YES){
      if(elem_part[elem] < 0 && remote_elem[elem] < 0)continue;
    }

    elem++;
    type = elem_type(elem);                        /* macro */
    switch(type){
      case EDGE_ELEM:
        elem = local_edge_id(elem);                  /* macro */
        fe_edge = &(edge_array[elem - 1]);
        if(fe_edge -> entity -> property_id != property_id)continue;
        for(j = 0; j < 2; j++){
          nd_id = fe_edge -> node[j] -> id;
          if(nd_id < 0)continue;
          fe_edge -> node[j] -> id = -nd_id;

          if(nd_id == node_id)
            dist = 0.0;
          else{
            nd = &(node_array[nd_id - 1]);
            dist = calculate_distance_square(node, nd);
          }
          if(dist < width){
            for(k = node_num_elems[nd_id - 1]; k < node_num_elems[nd_id]; k++){
              el = node_con_elems[k];
              mark_remote_elem(el, part_id);
            }
            if(nd_id != node_id)mark_remote_elems(node, nd_id, part_id, property_id, width);
          }
        }
        break;
      case FACE_ELEM:
        elem = local_face_id(elem);                 /* macro */
        fe_face = &(face_array[elem - 1]);
        if(fe_face -> entity -> property_id != property_id)continue;
        for(j = 0; j < 3; j++){
          nd_id = fe_face -> node[j] -> id;
          if(nd_id < 0)continue;
          fe_face -> node[j] -> id = -nd_id;

          if(nd_id == node_id)
            dist = 0.0;
          else{
            nd = &(node_array[nd_id - 1]);
            dist = calculate_distance_square(node, nd);
          }
          if(dist < width){
            for(k = node_num_elems[nd_id - 1]; k < node_num_elems[nd_id]; k++){
              el = node_con_elems[k];
              mark_remote_elem(el, part_id);
            }
            if(nd_id != node_id)mark_remote_elems(node, nd_id, part_id, property_id, width);
          }
        }
        break;
      case QUAD_ELEM:
        elem = local_quad_id(elem);                 /* macro */
        fe_quad = &(quad_array[elem - 1]);
        if(fe_quad -> entity -> property_id != property_id)continue;
        for(j = 0; j < 4; j++){
          nd_id = fe_quad -> node[j] -> id;
          if(nd_id < 0)continue;
          fe_quad -> node[j] -> id = -nd_id;

          if(nd_id == node_id)
            dist = 0.0;
          else{
            nd = &(node_array[nd_id - 1]);
            dist = calculate_distance_square(node, nd);
          }
          if(dist < width){
            for(k = node_num_elems[nd_id - 1]; k < node_num_elems[nd_id]; k++){
              el = node_con_elems[k];
              mark_remote_elem(el, part_id);
            }
            if(nd_id != node_id)mark_remote_elems(node, nd_id, part_id, property_id, width);
          }
        }
        break;
      case TETRA_ELEM:
        elem = local_tetra_id(elem);               /* macro */
        fe_tetra = &(tetra_array[elem - 1]);
        if(fe_tetra -> entity -> property_id != property_id)continue;
        for(j = 0; j < 4; j++){
          nd_id = fe_tetra -> node[j] -> id;
          if(nd_id < 0)continue;
          fe_tetra -> node[j] -> id = -nd_id;

          if(nd_id == node_id)
            dist = 0.0;
          else{
            nd = &(node_array[nd_id - 1]);
            dist = calculate_distance_square(node, nd);
          }
          if(dist < width){
            for(k = node_num_elems[nd_id - 1]; k < node_num_elems[nd_id]; k++){
              el = node_con_elems[k];
              mark_remote_elem(el, part_id);
            }
            if(nd_id != node_id)mark_remote_elems(node, nd_id, part_id, property_id, width);
          }
        }
        break;
      case PYRAM_ELEM:
        elem = local_pyram_id(elem);               /* macro */
        fe_pyram = &(pyram_array[elem - 1]);
        if(fe_pyram -> entity -> property_id != property_id)continue;
        for(j = 0; j < 5; j++){
          nd_id = fe_pyram -> node[j] -> id;
          if(nd_id < 0)continue;
          fe_pyram -> node[j] -> id = -nd_id;

          if(nd_id == node_id)
            dist = 0.0;
          else{
            nd = &(node_array[nd_id - 1]);
            dist = calculate_distance_square(node, nd);
          }
          if(dist < width){
            for(k = node_num_elems[nd_id - 1]; k < node_num_elems[nd_id]; k++){
              el = node_con_elems[k];
              mark_remote_elem(el, part_id);
            }
            if(nd_id != node_id)mark_remote_elems(node, nd_id, part_id, property_id, width);
          }
        }
        break;
      case WEDGE_ELEM:
        elem = local_wedge_id(elem);               /* macro */
        fe_wedge = &(wedge_array[elem - 1]);
        if(fe_wedge -> entity -> property_id != property_id)continue;
        for(j = 0; j < 6; j++){
          nd_id = fe_wedge -> node[j] -> id;
          if(nd_id < 0)continue;
          fe_wedge -> node[j] -> id = -nd_id;

          if(nd_id == node_id)
            dist = 0.0;
          else{
            nd = &(node_array[nd_id - 1]);
            dist = calculate_distance_square(node, nd);
          }
          if(dist < width){
            for(k = node_num_elems[nd_id - 1]; k < node_num_elems[nd_id]; k++){
              el = node_con_elems[k];
              mark_remote_elem(el, part_id);
            }
            if(nd_id != node_id)mark_remote_elems(node, nd_id, part_id, property_id, width);
          }
        }
        break;
      case HEXA_ELEM:
        elem = local_hexa_id(elem);                /* macro */
        fe_hexa = &(hexa_array[elem - 1]);
        if(fe_hexa -> entity -> property_id != property_id)continue;
        for(j = 0; j < 8; j++){
          nd_id = fe_hexa -> node[j] -> id;
          if(nd_id < 0)continue;
          fe_hexa -> node[j] -> id = -nd_id;

          if(nd_id == node_id)
            dist = 0.0;
          else{
            nd = &(node_array[nd_id - 1]);
            dist = calculate_distance_square(node, nd);
          }
          if(dist < width){
            for(k = node_num_elems[nd_id - 1]; k < node_num_elems[nd_id]; k++){
              el = node_con_elems[k];
              mark_remote_elem(el, part_id);
            }
            if(nd_id != node_id)mark_remote_elems(node, nd_id, part_id, property_id, width);
          }
        }
        break;
    }
  }
}




  static void
mark_remote_elem(long elem_id, int part_id)
{
  int type, i, j;
  long elem = elem_id - 1;
  fe_edge *fe_edge = NULL;
  fe_face *fe_face = NULL;
  fe_quad *fe_quad = NULL;
  fe_tetra *fe_tetra = NULL;
  fe_pyram *fe_pyram = NULL;
  fe_wedge *fe_wedge = NULL;
  fe_hexa *fe_hexa = NULL;

  if(node_cut == YES){
    if(abs(elem_part[elem]) == part_id)return;
  }
  if(elem_cut == YES){
    if(elem_part[elem] < 0 && remote_elem[elem] < 0)return;
  }
  if(remote_elem[elem] == elem_part[elem])return;

  if((remote_elem[elem] = elem_part[elem]) < 0)remote_elem[elem] = part_id;
  num_elems++;

  type = elem_type(elem_id);                        /* macro */
  switch(type){
    case EDGE_ELEM:
      elem = local_edge_id(elem_id);                  /* macro */
      fe_edge = &(edge_array[elem - 1]);
      for(i = 0; i < 2; i++){
        j = abs(fe_edge -> node[i] -> id) - 1;
        if(remote_node[j] == 0){
          if((remote_node[j] = node_part[j]) < 0)remote_node[j] = part_id;
        }
      }
      break;
    case FACE_ELEM:
      elem = local_face_id(elem_id);                  /* macro */
      fe_face = &(face_array[elem - 1]);
      for(i = 0; i < 3; i++){
        j = abs(fe_face -> node[i] -> id) - 1;
        if(remote_node[j] == 0){
          if((remote_node[j] = node_part[j]) < 0)remote_node[j] = part_id;
        }
      }
      break;
    case QUAD_ELEM:
      elem = local_quad_id(elem_id);                  /* macro */
      fe_quad = &(quad_array[elem - 1]);
      for(i = 0; i < 4; i++){
        j = abs(fe_quad -> node[i] -> id) - 1;
        if(remote_node[j] == 0){
          if((remote_node[j] = node_part[j]) < 0)remote_node[j] = part_id;
        }
      }
      break;
    case TETRA_ELEM:
      elem = local_tetra_id(elem_id);                 /* macro */
      fe_tetra = &(tetra_array[elem - 1]);
      for(i = 0; i < 4; i++){
        j = abs(fe_tetra -> node[i] -> id) - 1;
        if(remote_node[j] == 0){
          if((remote_node[j] = node_part[j]) < 0)remote_node[j] = part_id;
        }
      }
      break;
    case PYRAM_ELEM:
      elem = local_pyram_id(elem_id);                 /* macro */
      fe_pyram = &(pyram_array[elem - 1]);
      for(i = 0; i < 5; i++){
        j = abs(fe_pyram -> node[i] -> id) - 1;
        if(remote_node[j] == 0){
          if((remote_node[j] = node_part[j]) < 0)remote_node[j] = part_id;
        }
      }
      break;
    case WEDGE_ELEM:
      elem = local_wedge_id(elem_id);                 /* macro */
      fe_wedge = &(wedge_array[elem - 1]);
      for(i = 0; i < 6; i++){
        j = abs(fe_wedge -> node[i] -> id) - 1;
        if(remote_node[j] == 0){
          if((remote_node[j] = node_part[j]) < 0)remote_node[j] = part_id;
        }
      }
      break;
    case HEXA_ELEM:
      elem = local_hexa_id(elem_id);                 /* macro */
      fe_hexa = &(hexa_array[elem - 1]);
      for(i = 0; i < 8; i++){
        j = abs(fe_hexa -> node[i] -> id) - 1;
        if(remote_node[j] == 0){
          if((remote_node[j] = node_part[j]) < 0)remote_node[j] = part_id;
        }
      }
      break;
  }
}




  static void
unmark_nodes(long node_id, int part_id, long property_id)
{
  int type;
  long i, j, elem, nd_id;
  fe_edge *fe_edge = NULL;
  fe_face *fe_face = NULL;
  fe_quad *fe_quad = NULL;
  fe_tetra *fe_tetra = NULL;
  fe_pyram *fe_pyram = NULL;
  fe_wedge *fe_wedge = NULL;
  fe_hexa *fe_hexa = NULL;

  /* for node_cut it is possible to use negative remote_elem as flag for processed elements */

  for(i = node_num_elems[node_id - 1]; i < node_num_elems[node_id]; i++){
    elem = node_con_elems[i] - 1;
    if(node_cut == YES){
      if(abs(elem_part[elem]) == part_id)continue;
    }
    if(elem_cut == YES){
      if(elem_part[elem] < 0 && remote_elem[elem] < 0)continue;
    }

    elem++;
    type = elem_type(elem);                        /* macro */
    switch(type){
      case EDGE_ELEM:
        elem = local_edge_id(elem);                  /* macro */
        fe_edge = &(edge_array[elem - 1]);
        if(fe_edge -> entity -> property_id != property_id)continue;
        for(j = 0; j < 2; j++){
          nd_id = -(fe_edge -> node[j] -> id);
          if(nd_id < 0)continue;
          fe_edge -> node[j] -> id = nd_id;
          if(nd_id != node_id)unmark_nodes(nd_id, part_id, property_id);
        }
        break;
      case FACE_ELEM:
        elem = local_face_id(elem);                  /* macro */
        fe_face = &(face_array[elem - 1]);
        if(fe_face -> entity -> property_id != property_id)continue;
        for(j = 0; j < 3; j++){
          nd_id = -(fe_face -> node[j] -> id);
          if(nd_id < 0)continue;
          fe_face -> node[j] -> id = nd_id;
          if(nd_id != node_id)unmark_nodes(nd_id, part_id, property_id);
        }
        break;
      case QUAD_ELEM:
        elem = local_quad_id(elem);                  /* macro */
        fe_quad = &(quad_array[elem - 1]);
        if(fe_quad -> entity -> property_id != property_id)continue;
        for(j = 0; j < 4; j++){
          nd_id = -(fe_quad -> node[j] -> id);
          if(nd_id < 0)continue;
          fe_quad -> node[j] -> id = nd_id;
          if(nd_id != node_id)unmark_nodes(nd_id, part_id, property_id);
        }
        break;
      case TETRA_ELEM:
        elem = local_tetra_id(elem);                 /* macro */
        fe_tetra = &(tetra_array[elem - 1]);
        if(fe_tetra -> entity -> property_id != property_id)continue;
        for(j = 0; j < 4; j++){
          nd_id = -(fe_tetra -> node[j] -> id);
          if(nd_id < 0)continue;
          fe_tetra -> node[j] -> id = nd_id;
          if(nd_id != node_id)unmark_nodes(nd_id, part_id, property_id);
        }
        break;
      case PYRAM_ELEM:
        elem = local_pyram_id(elem);                 /* macro */
        fe_pyram = &(pyram_array[elem - 1]);
        if(fe_pyram -> entity -> property_id != property_id)continue;
        for(j = 0; j < 5; j++){
          nd_id = -(fe_pyram -> node[j] -> id);
          if(nd_id < 0)continue;
          fe_pyram -> node[j] -> id = nd_id;
          if(nd_id != node_id)unmark_nodes(nd_id, part_id, property_id);
        }
        break;
      case WEDGE_ELEM:
        elem = local_wedge_id(elem);                 /* macro */
        fe_wedge = &(wedge_array[elem - 1]);
        if(fe_wedge -> entity -> property_id != property_id)continue;
        for(j = 0; j < 6; j++){
          nd_id = -(fe_wedge -> node[j] -> id);
          if(nd_id < 0)continue;
          fe_wedge -> node[j] -> id = nd_id;
          if(nd_id != node_id)unmark_nodes(nd_id, part_id, property_id);
        }
        break;
      case HEXA_ELEM:
        elem = local_hexa_id(elem);                  /* macro */
        fe_hexa = &(hexa_array[elem - 1]);
        if(fe_hexa -> entity -> property_id != property_id)continue;
        for(j = 0; j < 8; j++){
          nd_id = -(fe_hexa -> node[j] -> id);
          if(nd_id < 0)continue;
          fe_hexa -> node[j] -> id = nd_id;
          if(nd_id != node_id)unmark_nodes(nd_id, part_id, property_id);
        }
        break;
    }
  }
}



  static double
calculate_distance_square(fe_node *node1, fe_node *node2)
{
  double dx, dy, dz;

  dx = node1 -> x - node2 -> x;
  dy = node1 -> y - node2 -> y;
  dz = node1 -> z - node2 -> z;

  return(dx * dx + dy * dy + dz * dz);
}



  static void
process_entity_list(void)
{
  entity *entity = NULL, *ent = NULL;
  list *entity_list = NULL, *ent_list = NULL;
  char *token = NULL, err_msg[128];
  long id = 0;

  switch(last_key = key){
    case VERTEX:
      vertex_select = YES;
      entity_list = ent_list = vertex_list;
      break;
    case CURVE:
      curve_select = YES;
      entity_list = ent_list = curve_list;
      break;
    case SURFACE:
      surface_select = YES;
      entity_list = ent_list = surface_list;
      break;
    case PATCH:
      patch_select = YES;
      entity_list = ent_list = patch_list;
      break;
    case SHELL:
      shell_select = YES;
      entity_list = ent_list = shell_list;
      break;
    case REGION:
      region_select = YES;
      entity_list = ent_list = region_list;
      break;
    case INTERFACE:
      interface_select = YES;
      entity_list = ent_list = interface_list;
      break;
    default:
      error_message("Unexpected situation", GENERAL_ERROR);
  }

  while((token = get_next_token(NULL)) != NULL){
    if((key = get_token_key(token, keyword, KEY_NUM, KEY_CASE)) == ALL){
      while(ent_list != NULL){
        entity = (struct entity *)(ent_list -> item);
        ent_list = ent_list -> next;
        entity -> selected = YES;
      }

      if((token = get_next_token(NULL)) != NULL){
        if((key = get_token_key(token, keyword, KEY_NUM, KEY_CASE)) != EXCEPT){
          sprintf(err_msg, "Invalid or unexpected keyword \"%s\" encountered", token);
          error_message(err_msg, GENERAL_ERROR);
        }
        while((token = get_next_token(NULL)) != NULL){
          if((id = get_int_from_token(token)) <= 0){
            sprintf(err_msg, "Invalid %s id (%ld)", keyword[last_key], id);
            error_message(err_msg, GENERAL_ERROR);
          }
          if((entity = get_entity_id(entity_list, id)) == NULL)
            fprintf(stderr, "WARNING: %s %ld does not exist ==> created\n", keyword[last_key], id);
          else
            entity -> selected = NO;
        }

        if(id == 0){
          sprintf(err_msg, "Missing %s id", keyword[last_key]);
          error_message(err_msg, GENERAL_ERROR);
        }
      }
      return;
    }

    if((id = get_int_from_token(token)) <= 0){
      sprintf(err_msg, "Invalid %s id (%ld)", keyword[last_key], id);
      error_message(err_msg, GENERAL_ERROR);
    }
    if((ent = get_entity_id(entity_list, id)) == NULL)
      fprintf(stderr, "WARNING: %s %ld does not exist ==> created\n", keyword[last_key], id);
    else
      ent -> selected = YES;
  }

  if(id == 0){
    sprintf(err_msg, "Missing %s id", keyword[last_key]);
    error_message(err_msg, GENERAL_ERROR);
  }
}


  static void
apply_weight(void)
{
  entity *entity = NULL;
  list *entity_list = NULL;

  if(curve_select == YES){
    entity_list = curve_list;
    while(entity_list != NULL){
      entity = (struct entity *)(entity_list -> item);
      entity_list = entity_list -> next;
      if(entity -> selected == NO)continue;
      entity -> selected = NO;
      entity -> edge_weight = edge_weight;
    }
    curve_select = NO;
  }
  if(surface_select == YES){
    entity_list = surface_list;
    while(entity_list != NULL){
      entity = (struct entity *)(entity_list -> item);
      entity_list = entity_list -> next;
      if(entity -> selected == NO)continue;
      entity -> selected = NO;
      entity -> face_weight = face_weight;
      entity -> quad_weight = quad_weight;
    }
    surface_select = NO;
  }
  if(patch_select == YES){
    entity_list = patch_list;
    while(entity_list != NULL){
      entity = (struct entity *)(entity_list -> item);
      entity_list = entity_list -> next;
      if(entity -> selected == NO)continue;
      entity -> selected = NO;
      entity -> face_weight = face_weight;
      entity -> quad_weight = quad_weight;
    }
    patch_select = NO;
  }
  if(shell_select == YES){
    entity_list = shell_list;
    while(entity_list != NULL){
      entity = (struct entity *)(entity_list -> item);
      entity_list = entity_list -> next;
      if(entity -> selected == NO)continue;
      entity -> selected = NO;
      entity -> face_weight = face_weight;
      entity -> quad_weight = quad_weight;
    }
    shell_select = NO;
  }
  if(region_select == YES){
    entity_list = region_list;
    while(entity_list != NULL){
      entity = (struct entity *)(entity_list -> item);
      entity_list = entity_list -> next;
      if(entity -> selected == NO)continue;
      entity -> selected = NO;
      entity -> tetra_weight = tetra_weight;
      entity -> pyram_weight = pyram_weight;
      entity -> wedge_weight = wedge_weight;
      entity -> hexa_weight = hexa_weight;
    }
    region_select = NO;
  }
  if(interface_select == YES){
    entity_list = interface_list;
    while(entity_list != NULL){
      entity = (struct entity *)(entity_list -> item);
      entity_list = entity_list -> next;
      if(entity -> selected == NO)continue;
      entity -> selected = NO;
      entity -> tetra_weight = tetra_weight;
      entity -> pyram_weight = pyram_weight;
      entity -> wedge_weight = wedge_weight;
      entity -> hexa_weight = hexa_weight;
    }
    interface_select = NO;
  }
}

#ifdef ELIXIR

  static void
draw_node(fe_node *node, int layer, EPixel color, int type, int msize)
{
  WCRec point[1];
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;

  if(virtual_layers == YES)return;

  point[0].x = node -> x;
  point[0].y = node -> y;
  point[0].z = node -> z;

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= MTYPE_MASK;
  mask |= MSIZE_MASK;

  EASValsSetLayer(layer);
  EASValsSetColor(color);
  EASValsSetMType(type);
  EASValsSetMSize(msize);	

  if((gr_obj = CreateMarker3D(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj);
  EMAddGraphicsToModel(ESIModel(), gr_obj);
}


  static void
draw_edge(fe_edge *edge, int layer, EPixel color)
{
  WCRec point[2];	
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;
  fe_node *node = NULL;
  int i;

  for(i = 0; i < 2; i++){
    node = edge -> node[i];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= STYLE_MASK;
  mask |= SHRINK_MASK;

  if(virtual_layers == NO)
    EASValsSetLayer(layer);
  else
    EASValsSetLayer(ACTIVE_LAYER);
  EASValsSetColor(color);
  EASValsSetFillStyle(SOLID_STYLE);
  EASValsSetShrink(shrink);

  if((gr_obj = CreateLine3D(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);

  if(virtual_layers == YES)layer_obj[layer][layer_obj_num[layer]++] = gr_obj;
}


  static void
draw_face(fe_face *face, int layer, EPixel color, EPixel edge_color)
{
  WCRec point[3];	
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;
  fe_node *node = NULL;
  int i;

  for(i = 0; i < 3; i++){
    node = face -> node[i];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= FILL_MASK;
  mask |= SHRINK_MASK;
  mask |= EDGE_COLOR_MASK;
  mask |= EDGE_FLAG_MASK;

  if(virtual_layers == NO)
    EASValsSetLayer(layer);
  else
    EASValsSetLayer(ACTIVE_LAYER);
  EASValsSetColor(color);
  EASValsSetFillStyle(FILL_SOLID);
  EASValsSetShrink(shrink);
  EASValsSetEdgeColor(edge_color);
  EASValsSetEdgeFlag(1);

  if((gr_obj = CreateTriangle3D(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);

  if(virtual_layers == YES)layer_obj[layer][layer_obj_num[layer]++] = gr_obj;
}


  static void
draw_quad(fe_quad *quad, int layer, EPixel color, EPixel edge_color)
{
  WCRec point[4];	
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;
  fe_node *node = NULL;
  int i;

  for(i = 0; i < 4; i++){
    node = quad -> node[i];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= FILL_MASK;
  mask |= SHRINK_MASK;
  mask |= EDGE_COLOR_MASK;
  mask |= EDGE_FLAG_MASK;

  if(virtual_layers == NO)
    EASValsSetLayer(layer);
  else
    EASValsSetLayer(ACTIVE_LAYER);
  EASValsSetColor(color);
  EASValsSetFillStyle(FILL_SOLID);
  EASValsSetShrink(shrink);
  EASValsSetEdgeColor(edge_color);
  EASValsSetEdgeFlag(1);

  if((gr_obj = CreateQuad3D(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);

  if(virtual_layers == YES)layer_obj[layer][layer_obj_num[layer]++] = gr_obj;
}


  static void
draw_tetra(fe_tetra *tetra, int layer, EPixel color, EPixel edge_color)
{
  WCRec point[4];	
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;
  fe_node *node = NULL;
  int i;

  for(i = 0; i < 4; i++){
    node = tetra -> node[i];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= FILL_MASK;
  mask |= SHRINK_MASK;
  mask |= EDGE_COLOR_MASK;
  mask |= EDGE_FLAG_MASK;

  if(virtual_layers == NO)
    EASValsSetLayer(layer);
  else
    EASValsSetLayer(ACTIVE_LAYER);
  EASValsSetColor(color);
  EASValsSetFillStyle(FILL_SOLID);
  EASValsSetShrink(shrink);
  EASValsSetEdgeColor(edge_color);
  EASValsSetEdgeFlag(1);

  if((gr_obj = CreateTetra(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);

  if(virtual_layers == YES)layer_obj[layer][layer_obj_num[layer]++] = gr_obj;
}


  static void
draw_pyram(fe_pyram *pyram, int layer, EPixel color, EPixel edge_color)
{
  WCRec point[8];	
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;
  fe_node *node = NULL;
  int i;

  for(i = 0; i < 5; i++){
    node = pyram -> node[i];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }
  for(i = 5; i < 8; i++){
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= FILL_MASK;
  mask |= SHRINK_MASK;
  mask |= EDGE_COLOR_MASK;
  mask |= EDGE_FLAG_MASK;

  if(virtual_layers == NO)
    EASValsSetLayer(layer);
  else
    EASValsSetLayer(ACTIVE_LAYER);
  EASValsSetColor(color);
  EASValsSetFillStyle(FILL_SOLID);
  EASValsSetShrink(shrink);
  EASValsSetEdgeColor(edge_color);
  EASValsSetEdgeFlag(1);

  if((gr_obj = CreateHexahedron(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);

  if(virtual_layers == YES)layer_obj[layer][layer_obj_num[layer]++] = gr_obj;
}


  static void
draw_wedge(fe_wedge *wedge, int layer, EPixel color, EPixel edge_color)
{
  WCRec point[8];	
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;
  fe_node *node = NULL;
  int i;

  for(i = 0; i < 3; i++){
    node = wedge -> node[i];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }
  point[3].x = node -> x;
  point[3].y = node -> y;
  point[3].z = node -> z;

  for(i = 4; i < 7; i++){
    node = wedge -> node[i - 1];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }
  point[7].x = node -> x;
  point[7].y = node -> y;
  point[7].z = node -> z;

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= FILL_MASK;
  mask |= SHRINK_MASK;
  mask |= EDGE_COLOR_MASK;
  mask |= EDGE_FLAG_MASK;

  if(virtual_layers == NO)
    EASValsSetLayer(layer);
  else
    EASValsSetLayer(ACTIVE_LAYER);
  EASValsSetColor(color);
  EASValsSetFillStyle(FILL_SOLID);
  EASValsSetShrink(shrink);
  EASValsSetEdgeColor(edge_color);
  EASValsSetEdgeFlag(1);

  if((gr_obj = CreateHexahedron(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);

  if(virtual_layers == YES)layer_obj[layer][layer_obj_num[layer]++] = gr_obj;
}


  static void
draw_hexa(fe_hexa *hexa, int layer, EPixel color, EPixel edge_color)
{
  WCRec point[8];	
  GraphicObj *gr_obj = NULL;
  unsigned long mask = 0L;
  fe_node *node = NULL;
  int i;

  for(i = 0; i < 8; i++){
    node = hexa -> node[i];
    point[i].x = node -> x;
    point[i].y = node -> y;
    point[i].z = node -> z;
  }

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= FILL_MASK;
  mask |= SHRINK_MASK;
  mask |= EDGE_COLOR_MASK;
  mask |= EDGE_FLAG_MASK;

  if(virtual_layers == NO)
    EASValsSetLayer(layer);
  else
    EASValsSetLayer(ACTIVE_LAYER);
  EASValsSetColor(color);
  EASValsSetFillStyle(FILL_SOLID);
  EASValsSetShrink(shrink);
  EASValsSetEdgeColor(edge_color);
  EASValsSetEdgeFlag(1);

  if((gr_obj = CreateHexahedron(point)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);

  if(virtual_layers == YES)layer_obj[layer][layer_obj_num[layer]++] = gr_obj;
}


  static void
draw_number(double x, double y, double z, int layer, EPixel color, Font font, long number, char *prefix)
{
  WCRec point;
  GraphicObj *gr_obj;
  unsigned long mask = 0L;
  char string[16];

  if(virtual_layers == YES)return;

  point.x = x;
  point.y = y;
  point.z = z;

  sprintf(string, "%s%ld", prefix, number);

  mask |= LAYER_MASK;
  mask |= COLOR_MASK;
  mask |= FONT_MASK;

  EASValsSetLayer(layer);
  EASValsSetColor(color);
  EASValsSetFontId(font);

  if((gr_obj = CreateAnnText3D(&point, string)) == NULL){
    ERptErrMessage(ELIXIR_ERROR_CLASS, 2, "Failure of graphics creation", ERROR_GRADE);
    exit(ESISetExitCode(2));
  }

  EGWithMaskChangeAttributes(mask, gr_obj); 
  EMAddGraphicsToModel(ESIModel(), gr_obj);
}


  void
ESICustomize(Widget parent_pane)
{
  Widget domain_palette, render_palette, view_palette, color_palette;
  Widget set_palette, add_palette, del_palette;
  int i;
  char string[16];

  ESIAddPalette("Pallette", "  Domain  ", parent_pane, NULL, 0, &domain_palette);

  if(virtual_layers == NO){
    ESIAddButton("Button", "  All domains  ", commandWidgetClass, domain_palette, NULL, 0, all_domains, NULL);
    ESIAddPopupDialog(NULL, "  Set domain  ", "Set domain #", NULL, domain_palette, 
        NULL, 0, set_domain, NULL, ESIDialogValueNumber, NULL);
    ESIAddPopupDialog(NULL, "  Add domain  ", "Add domain #", NULL, domain_palette, 
        NULL, 0, add_domain, NULL, ESIDialogValueNumber, NULL);
    ESIAddPopupDialog(NULL, "  Del domain  ", "Del domain #", NULL, domain_palette, 
        NULL, 0, del_domain, NULL, ESIDialogValueNumber, NULL);

    if(nparts <= NUM_COLORS){
      ESIAddPalette("Palette", "  By color  ", domain_palette, NULL, 0, &color_palette);
      ESIAddPalette("Palette", "  Set domain  ", color_palette, NULL, 0, &set_palette);
      ESIAddPalette("Palette", "  Add domain  ", color_palette, NULL, 0, &add_palette);
      ESIAddPalette("Palette", "  Del domain  ", color_palette, NULL, 0, &del_palette);

      ESIAddButton("label", " Set ", labelWidgetClass, set_palette, NULL, 0, NULL, NULL);
      ESIAddButton("label", " Add ", labelWidgetClass, add_palette, NULL, 0, NULL, NULL);
      ESIAddButton("label", " Del ", labelWidgetClass, del_palette, NULL, 0, NULL, NULL);

      for(i = 0; i < nparts; i++){
        sprintf(string, "  %d  ", i + 1);
        set_button[i] = ESIAddButton("Button", string, commandWidgetClass, set_palette, NULL, 0, set_nth_domain, (XtPointer)(i + 1));
        add_button[i] = ESIAddButton("Button", string, commandWidgetClass, add_palette, NULL, 0, add_nth_domain, (XtPointer)(i + 1));
        del_button[i] = ESIAddButton("Button", string, commandWidgetClass, del_palette, NULL, 0, del_nth_domain, (XtPointer)(i + 1));
      }
    }
  }
  else{
    ESIAddButton("Button", "  All domains  ", commandWidgetClass, domain_palette, NULL, 0, all_domains_virtual, NULL);
    ESIAddPopupDialog(NULL, "  Set domain  ", "Set domain #", NULL, domain_palette, 
        NULL, 0, set_domain_virtual, NULL, ESIDialogValueNumber, NULL);
    ESIAddPopupDialog(NULL, "  Add domain  ", "Add domain #", NULL, domain_palette, 
        NULL, 0, add_domain_virtual, NULL, ESIDialogValueNumber, NULL);
    ESIAddPopupDialog(NULL, "  Del domain  ", "Del domain #", NULL, domain_palette, 
        NULL, 0, del_domain_virtual, NULL, ESIDialogValueNumber, NULL);
  }

  ESIAddButton("Button", "  Toggle shrink  ", commandWidgetClass, parent_pane, NULL, 0, toggle_shrink, NULL);
  if(virtual_layers == NO && draw_nodes == YES){
    ESIAddButton("Button", "  Toggle nodes  ", commandWidgetClass, parent_pane, NULL, 0, toggle_msize, NULL);
  }

  ESIAddPalette("Palette", "  View  ", parent_pane, NULL, 0, &view_palette);

  ESIAddButton("Button", "  New view  ", commandWidgetClass, view_palette, NULL, 0, new_view, NULL);
  ESIAddButton("Button", "  Origin  ", commandWidgetClass, view_palette, NULL, 0, view_origin, NULL);
  ESIAddButton("Button", "  Fit all  ", commandWidgetClass, view_palette, NULL, 0, fit_all, NULL);
  ESIAddButton("Button", "  Redraw  ", commandWidgetClass, view_palette, NULL, 0, redraw_view, NULL);

  ESIAddPalette("Palette", "  Render  ", view_palette, NULL, 0, &render_palette);

  ESIAddButton("Button", "  Wired view  ", commandWidgetClass, render_palette, NULL, 0, render_view, (XtPointer)WIRE_RENDERING);
  ESIAddButton("Button", "  Normal view  ", commandWidgetClass, render_palette, NULL, 0, render_view, (XtPointer)NORMAL_RENDERING);
  ESIAddButton("Button", "  Hidden view  ", commandWidgetClass, render_palette, NULL, 0, render_view, (XtPointer)FILLED_HIDDEN_RENDERING);
  ESIAddButton("Button", "  Shaded view  ", commandWidgetClass, render_palette, NULL, 0, render_view, (XtPointer)CONST_SHADING_RENDERING);
}


  static void
new_view(Widget w, XtPointer ptr, XtPointer call_data)
{
  int i;
  EView *view = NULL;

  view = ElixirNewView("Metis", "Domain decomposition", "midnightblue", "white", 400, 400);
  EMAttachView(ESIModel(), view);
  EMRegenerateGraphics(ESIModel(), view);
  EVSetViewOrientation(view, VIEW_ORIENT_ISO);
  EVShowAxes(view, NO);
  ESIHandleCmd("render ambient 0.1");
  EVSetRenderMode(view, WIRE_RENDERING);

  if(virtual_layers == NO){
    for(i = 1; i <= nparts; i++){
      if(layers[i] == YES)EVSetLayerOnOff(view, i, YES);
    }
  }
  else{
    EVSetLayerOnOff(view, ACTIVE_LAYER, YES);
  }

  EVFitAllIntoView(view);
}



  static void
all_domains(Widget w, XtPointer ptr, XtPointer call_data)
{
  int i;

  EMDispatchToDependentViews(ESIModel(), view_all_domains, NULL);
  for(i = 1; i <= nparts; i++)layers[i] = YES;
}		


  static void
set_domain(Widget w, XtPointer ptr, XtPointer call_data)
{
  int i, layer;

  layer = TypeInGetTokenAsInt(1);
  if(layer <= 0 || layer > nparts)return;

  EMDispatchToDependentViews(ESIModel(), view_set_domain, (caddr_t)layer);
  for(i = 1; i <= nparts; i++)layers[i] = NO;
  layers[layer] = YES;
}		


  static void
add_domain(Widget w, XtPointer ptr, XtPointer call_data)
{
  int layer;

  layer = TypeInGetTokenAsInt(1);
  if(layer <= 0 || layer > nparts)return;

  EMDispatchToDependentViews(ESIModel(), view_add_domain, (caddr_t)layer);
  layers[layer] = YES;
}		


  static void
del_domain(Widget w, XtPointer ptr, XtPointer call_data)
{
  int layer;

  layer = TypeInGetTokenAsInt(1);
  if(layer <= 0 || layer > nparts)return;

  EMDispatchToDependentViews(ESIModel(), view_del_domain, (caddr_t)layer);
  layers[layer] = NO;
}		


  static void
all_domains_virtual(Widget w, XtPointer ptr, XtPointer call_data)
{
  int i, j;
  GraphicObj **gr_obj = NULL;

  for(i = 1; i <= nparts; i++){
    gr_obj = layer_obj[i];
    for(j = 0; j < layer_obj_num[i]; j++, gr_obj++){
      if(*gr_obj != NULL)EGSetLayer(*gr_obj, ACTIVE_LAYER);
    }
    layers[i] = YES;
  }

  EMDispatchToDependentViews(ESIModel(), reset_active_layer, NULL);
}		


  static void
set_domain_virtual(Widget w, XtPointer ptr, XtPointer call_data)
{
  int i, j, layer;
  GraphicObj **gr_obj = NULL;

  layer = TypeInGetTokenAsInt(1);
  if(layer <= 0 || layer > nparts)return;

  for(i = 1; i <= nparts; i++){
    if(i == layer){
      if(layers[i] == NO){
        gr_obj = layer_obj[i];
        for(j = 0; j < layer_obj_num[i]; j++, gr_obj++){
          if(*gr_obj != NULL)EGSetLayer(*gr_obj, ACTIVE_LAYER);
        }
        layers[i] = YES;
      }
    }
    else{
      if(layers[i] == YES){
        gr_obj = layer_obj[i];
        for(j = 0; j < layer_obj_num[i]; j++, gr_obj++){
          if(*gr_obj != NULL)EGSetLayer(*gr_obj, TRASH_LAYER);
        }
        layers[i] = NO;
      }
    }
  }

  EMDispatchToDependentViews(ESIModel(), reset_active_layer, NULL);
}		


  static void
add_domain_virtual(Widget w, XtPointer ptr, XtPointer call_data)
{
  int j, layer;
  GraphicObj **gr_obj = NULL;

  layer = TypeInGetTokenAsInt(1);
  if(layer <= 0 || layer > nparts)return;

  if(layers[layer] == NO){
    gr_obj = layer_obj[layer];
    for(j = 0; j < layer_obj_num[layer]; j++, gr_obj++){
      if(*gr_obj != NULL)EGSetLayer(*gr_obj, ACTIVE_LAYER);
    }
    layers[layer] = YES;
  }

  EMDispatchToDependentViews(ESIModel(), reset_active_layer, NULL);
}		


  static void
del_domain_virtual(Widget w, XtPointer ptr, XtPointer call_data)
{
  int j, layer;
  GraphicObj **gr_obj = NULL;

  layer = TypeInGetTokenAsInt(1);
  if(layer <= 0 || layer > nparts)return;

  if(layers[layer] == YES){
    gr_obj = layer_obj[layer];
    for(j = 0; j < layer_obj_num[layer]; j++, gr_obj++){
      if(*gr_obj != NULL)EGSetLayer(*gr_obj, TRASH_LAYER);
    }
    layers[layer] = NO;
  }

  EMDispatchToDependentViews(ESIModel(), reset_active_layer, NULL);
}		


  static void
set_nth_domain(Widget w, XtPointer ptr, XtPointer call_data)
{
  int i, layer = (int)ptr;

  if(layer <= 0 || layer > nparts)return;

  EMDispatchToDependentViews(ESIModel(), view_set_domain, (caddr_t)layer);
  for(i = 1; i <= nparts; i++)layers[i] = NO;
  layers[layer] = YES;
}


  static void
add_nth_domain(Widget w, XtPointer ptr, XtPointer call_data)
{
  int layer = (int)ptr;

  if(layer <= 0 || layer > nparts)return;

  EMDispatchToDependentViews(ESIModel(), view_add_domain, (caddr_t)layer);
  layers[layer] = YES;
}


  static void
del_nth_domain(Widget w, XtPointer ptr, XtPointer call_data)
{
  int layer = (int)ptr;

  if(layer <= 0 || layer > nparts)return;

  EMDispatchToDependentViews(ESIModel(), view_del_domain, (caddr_t)layer);
  layers[layer] = NO;
}


  static int
view_all_domains(NODE data, NODE v)
{
  int i;
  EView *view = (EView *)v;

  for(i = 1; i <= nparts; i++){
    if(EVGetLayerOnOff(view, i) == NO)
      EVSetLayerOnOff(view, i, YES);
  }

  return(1);
}		


  static int
view_set_domain(NODE data, NODE v)
{
  int i, layer = (int)data;
  EView *view = (EView *)v;

  for(i = 1; i <= nparts; i++){
    if(i == layer){
      if(EVGetLayerOnOff(view, i) == NO)
        EVSetLayerOnOff(view, i, YES);
    }
    else{
      if(EVGetLayerOnOff(view, i) == YES)
        EVSetLayerOnOff(view, i, NO);
    }
  }
  return(1);
}		


  static int
view_add_domain(NODE data, NODE v)
{
  int layer = (int)data;
  EView *view = (EView *)v;

  if(EVGetLayerOnOff(view, layer) == NO)
    EVSetLayerOnOff(view, layer, YES);
  return(1);
}		


  static int
view_del_domain(NODE data, NODE v)
{
  int layer = (int)data;
  EView *view = (EView *)v;

  if(EVGetLayerOnOff(view, layer) == YES)
    EVSetLayerOnOff(view, layer, NO);
  return(1);
}		



  static void
toggle_shrink(Widget w, XtPointer ptr, XtPointer call_data)
{
  if(shrink == 1.0)
    shrink = SHRINK;
  else
    shrink = 1.0;

  EMDispatchToDependentViews(ESIModel(), view_normalize_off, NULL);
  EMDispatchToGraphicObjects(ESIModel(), shrink_graphics, NULL);
  EMDispatchToDependentViews(ESIModel(), view_normalize_on, NULL);
}


  static int
shrink_graphics(NODE data, NODE gr_obj)
{
  EMEraseGraphics(ESIModel(), (struct GraphicObj *) gr_obj);
  EASValsSetShrink(shrink);
  EGWithMaskChangeAttributes(SHRINK_MASK, (struct GraphicObj *) gr_obj); 
  EMDrawGraphics(ESIModel(), (struct GraphicObj *) gr_obj);
  return(1);
}


  static void
toggle_msize(Widget w, XtPointer ptr, XtPointer call_data)
{
  if(msize == 0)
    msize = MSIZE;
  else
    msize = 0;

  EMDispatchToDependentViews(ESIModel(), view_normalize_off, NULL);
  EMDispatchToGraphicObjects(ESIModel(), msize_graphics, NULL);
  EMDispatchToDependentViews(ESIModel(), view_normalize_on, NULL);
}


  static int
msize_graphics(NODE data, NODE gr_obj)
{
  EMEraseGraphics(ESIModel(), (struct GraphicObj *) gr_obj);
  EASValsSetMSize(msize);
  EGWithMaskChangeAttributes(MSIZE_MASK, (struct GraphicObj *) gr_obj); 
  EMDrawGraphics(ESIModel(), (struct GraphicObj *) gr_obj);
  return(1);
}


  static int
view_normalize_off(NODE data, NODE v)
{
  EView *view = (EView *)v;
  ERenderingType render_md;
  int i;

  render_md = EVGetRenderMode(view);
  if(render_md == FILLED_HIDDEN_RENDERING || render_md == CONST_SHADING_RENDERING){
    for(i = 1; i <= nparts; i++){
      if(EVGetLayerOnOff(view, i) == YES)
        EVSetLayerOnOff(view, i, NO);
    }
  }
  return(1);
}		


  static int
view_normalize_on(NODE data, NODE v)
{
  EView *view = (EView *)v;
  ERenderingType render_md;
  int i;

  render_md = EVGetRenderMode(view);
  if(render_md == FILLED_HIDDEN_RENDERING || render_md == CONST_SHADING_RENDERING){
    for(i = 1; i <= nparts; i++){
      if(EVGetLayerOnOff(view, i) == NO && layers[i] == YES)
        EVSetLayerOnOff(view, i, YES);
    }
  }
  return(1);
}		


  void 
apply_redraw_view(EView *v_p, caddr_t data, WCRec *p)
{
  /* Doing nothing: redraw is the default of apply_to_view input handler. */
}


  static void 
set_render_mode(EView *v_p, caddr_t data, WCRec *p)
{
  EVSetRenderMode(v_p, render_mode);
  EVSetShadeMode(v_p, shade_mode);
}

  static void 
uninstall_apply_to_view(EView *v_p, caddr_t data)
{
  EMUninstallHandler(EVGetModel(v_p));
}


  static void 
redraw_view(Widget w, XtPointer client_data, XtPointer call_data)
{
  EVSetApplyToViewFunction(apply_redraw_view, NULL, uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}


  static void 
render_view(Widget w, XtPointer ptr, XtPointer call_data)
{
  render_mode = (ERenderingType) ptr;
  EVSetApplyToViewFunction(set_render_mode, NULL, uninstall_apply_to_view);
  EMPushHandler(ESIModel(), EVApplyToViewHandler, NULL);
}


  static void 
fit_all(Widget w, XtPointer cmdtext_ptr, XtPointer call_data)
{
  ESIHandleCmd("fit");
}


  static void 
view_origin(Widget w, XtPointer cmdtext_ptr, XtPointer call_data)
{
  ESIHandleCmd("view origin");
}


  static int
reset_active_layer(NODE data, NODE v)
{
  EView *view = (EView *)v;

  EVSetLayerOnOff(view, ACTIVE_LAYER, NO);
  EVSetLayerOnOff(view, ACTIVE_LAYER, YES);

  return(1);
}

#endif


#define LN2       0.693147180559945309417232121458176568

  static void
sort_array(long n, long *key, long *pos)
{
  long i, j, d = 1;
  long tmp_key, tmp_pos;

  if(n == 1)return;

  i = (long)(log((double)n + 0.5) / LN2);
  while(i--)d *= 2;
  d -= 1;

  while(d != 0){
    for(i = 0; i < n - d; i++){
      if(key[i] <= key[i + d])continue;
      tmp_key = key[i + d];
      key[i + d] = key[i];
      tmp_pos = pos[i + d];
      pos[i + d] = pos[i];
      if(i <= d){
        key[i] = tmp_key;
        pos[i] = tmp_pos;
        continue;
      }
      for(j = i - d; j >= 0; j -= d){
        if(tmp_key >= key[j])break;
        key[j + d] = key[j];
        pos[j + d] = pos[j];
      }
      key[j + d] = tmp_key;
      pos[j + d] = tmp_pos;
    }
    d /= 2;
  }
}

int make_dir(const char *path, mode_t mode)
{
  struct stat st;
  int status = 0;

  /* check the status of the file in path */
  if(stat(path,&st) != 0){ /* file does not exist */

    if(mkdir(path,mode) != 0 && errno != EEXIST){
      /* The directory could not be created and the error is something
     other than the file already existing */
      status = -1;
    }

  } else if(!S_ISDIR(st.st_mode)){ /* file exists and is not a directory */
    status = -1;
  }
  return status;
}

int make_path(const char *path, mode_t mode)
{

  struct stat st;
  char *pp;
  char *sp;
  int status, full_path;
  char *copypath = malloc(sizeof(char)*(strlen(path)+1));
  strcpy(copypath,path);

  status = 0;
  full_path = 0;
  pp = copypath;

  /* first check to see if the path up to the last dir exists */
  if((sp = strrchr(pp, '/')) == NULL){ /* path in current directory */
    full_path = 1;
  } else if(sp != pp){
    /* not root dir */
    *sp = '\0';
    if(stat(copypath,&st) == 0 && S_ISDIR(st.st_mode)){
      full_path = 1;
    }
    *sp = '/';
  }

  switch (full_path){
  case 0:
    while (status == 0 && (sp = strchr(pp, '/')) != 0){
      if (sp != pp){
    /* Neither root nor double slash in path */

    *sp = '\0'; /* replace '/' with '\0' to terminate the string */

    status = make_dir(copypath, mode);

    *sp = '/'; /* replace the '/' character to regain the full string */
      }
      pp = sp + 1; /* advance the pointer to after the '/' */
    }
    /* Drop through to case 1 */

  case 1:
    if (status == 0)
      status = make_dir(path, mode);
    free(copypath);
    break;
  }

  return (status);
}

