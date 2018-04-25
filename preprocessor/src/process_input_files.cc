/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 * Aaron Howell
 */

#include "process_input_files.h"
#include "utils.h"
#include "omp_param.h"
#include <string>
#include <sstream>
#include <fstream>
#include "structures.h"
#include "material.h"
#include <string.h>

void process_header_file(const Options &opt,
			 Header &header)
{

  std::string fname(opt.base_fname());
  fname += ".out.header";

  std::ifstream in(fname.c_str());
  file_check_error(in,fname.c_str());
  header.read(in);
  in.close();
  header.sort_and_unique_assignments();
}


/*
  Read the human-readable input library's data into header class
*/
void process_inputs(Header &header, Input_Data inputs) {

  header.passNDim(inputs.number_of_spatial_dimensions);
  header.passNiter(inputs.solver_max_num_of_iterations);
  header.passLintol(inputs.solver_tolerance);
  header.passEpsilon(inputs.absolute_tolerance);
  header.fillMaterialList(header, inputs, inputs.material_list.size());	
  header.fillCon(header, inputs);
  header.fillBasisList(header, inputs, inputs.number_of_bases);
  header.fillT3d(inputs);


  header.allocateBCIC(inputs.number_of_physics);

  for (int physics = 0; physics < inputs.number_of_physics; ++physics){
    header.fillBC(inputs, physics);
    header.fillPrescribed(inputs.physics_list[physics].bc_replacements, inputs.physics_list[physics].bc_replacements.size(), physics);
    header.fillIC(inputs, physics);
  }

}




int scan_for_valid_line(FILE *in) {
  static const size_t line_length = 1024;
  static const char delim[] = " \t\n";
  int err = 0;
  char *line = new char[line_length];
  char *tok = NULL;
  fpos_t pos;
  do{
    err += fgetpos(in,&pos);
    if ( fgets(line,line_length,in) == NULL) {
      err++;      
      goto exit_err;
    }
    if ( line[strlen(line) - 1] != '\n' && !feof(in)) {
      fprintf(stderr,"ERROR: line too long (>%zd chars)! %s(%s)\n",
              line_length, __func__, __FILE__);

      err++;
      goto exit_err;
    }
    tok = strtok(line,delim);
    if (tok == NULL) tok = line + strlen(line);
  } while ( tok[0] == '#' || tok[0] == '\0');
  err += fsetpos(in,&pos);
 exit_err:
  free(line);
  return err;
}

int set_a_physics(MULTIPHYSICS *mp,int obj_id,int mp_id,int n_dof,char *name) {
  int err = 0;
  mp->physics_ids[obj_id] = mp_id; 
  mp->ndim[obj_id]        = n_dof;
  sprintf(mp->physicsname[obj_id], "%s", name);
  return err = 0;  
}




int construct_multiphysics(MULTIPHYSICS *mp, int physicsno) {
  int err = 0;
  mp->physicsno   = physicsno;
  mp->physicsname = (char **) malloc(sizeof(char *)*physicsno);
  mp->physics_ids = (int*) malloc(sizeof(int)*physicsno);
  mp->ndim        = (int*) malloc(sizeof(int)*physicsno);
  mp->write_no    = (int*) malloc(sizeof(int)*physicsno);
  mp->write_ids   = (int**) malloc(sizeof(int *)*physicsno);
  mp->coupled_ids = (int**) malloc(sizeof(int *)*physicsno);
  for(int ia=0; ia<physicsno; ia++)
  {
    mp->physicsname[ia] = (char *) malloc(sizeof(char)*1024);
    mp->physics_ids[ia] = 0;
    mp->ndim[ia]        = 0;
    mp->write_no[ia]    = 0;
    mp->write_ids[ia]   = NULL;
    mp->coupled_ids[ia] = NULL;
  }
  mp->total_write_no  = 0;
  return err = 0;  
}


int multiphysics_initialization(MULTIPHYSICS *mp) {
   int err = 0;
   mp->physicsno   = 0;
   mp->physicsname = NULL;
   mp->physics_ids = NULL;
   mp->ndim        = NULL;
   mp->write_no    = NULL;
   mp->write_ids   = NULL;
   mp->coupled_ids = NULL;
   mp->total_write_no = 0;
    return err = 0; }

int read_multiphysics_settings(MULTIPHYSICS *mp,const Options &opt ) {
  int err = 0;
  int physicsno = 0;
  char filename[1024];

  sprintf(filename,"multiphysics.in");
  FILE *in = NULL;
  in = fopen(filename,"r");
  {        
    err += scan_for_valid_line(in);
    fscanf(in, "%d", &physicsno);
    if(physicsno>0)
    {  
      err += construct_multiphysics(mp, physicsno);
      int physics_id, ndof;
      int n_couple;
      char name[1024];
      int cnt_pmr = 0;
      for(int ia=0; ia<physicsno; ia++)
      {
        err += scan_for_valid_line(in);
        fscanf(in, "%d%s%d", &physics_id,name,&ndof);
        err += set_a_physics(mp, ia,physics_id,ndof, name);
        err += scan_for_valid_line(in);
        fscanf(in, "%d", &n_couple);
        if(n_couple<0)
          n_couple = 0;
        mp->coupled_ids[ia] = (int *) malloc(sizeof(int)*(n_couple + 1));        
        mp->coupled_ids[ia][0] = n_couple;
        for(int ib = 0; ib<n_couple; ib++)
          fscanf(in, "%d", (mp->coupled_ids[ia])+(ib+1));
        err += scan_for_valid_line(in);
        fscanf(in, "%d", mp->write_no+ia);
        if(mp->write_no[ia]>0) 
        {
          mp->write_ids[ia] = (int *) malloc(sizeof(int)*(mp->write_no[ia]));
          err += scan_for_valid_line(in);
          cnt_pmr += mp->write_no[ia];
          for(int ib=0; ib<mp->write_no[ia]; ib++)
            fscanf(in, "%d", mp->write_ids[ia]+ib);
        }
        if(mp->write_no[ia]==-1)
        {
          switch(physics_id)
          {
            case MULTIPHYSICS_MECHANICAL:
              mp->write_no[ia] = MECHANICAL_Var_NO;
              break;
            case MULTIPHYSICS_THERMAL:
              mp->write_no[ia] = Thermal_Var_NO;
              break;
            case MULTIPHYSICS_CHEMICAL:
              mp->write_no[ia] = CHEMICAL_Var_NO;
              break;
            default:
              mp->write_no[ia] = MECHANICAL_Var_NO;
          }    

          mp->write_ids[ia] = (int *) malloc(sizeof(int)*(mp->write_no[ia]));
          err += scan_for_valid_line(in);
          cnt_pmr += mp->write_no[ia];
          for(int ib=0; ib<mp->write_no[ia]; ib++)
            mp->write_ids[ia][ib] = ib;
        }                    
      }
      mp->total_write_no  = cnt_pmr;
    }
    fclose(in); // close file
  }
  if(physicsno<=0)
  {  
    err += construct_multiphysics(mp, 1);
    char mechanical[11] = "Mechanical";
    err += set_a_physics(mp, 0, MULTIPHYSICS_MECHANICAL, 3, mechanical);
    mp->coupled_ids[0] = (int *) malloc(sizeof(int));
    mp->coupled_ids[0][0] = 0;
    mp->write_no[0] = MECHANICAL_Var_NO;    
    mp->write_ids[0] = (int *) malloc(sizeof(int)*(mp->write_no[0]));
    for(int ib=0; ib<mp->write_no[0]; ib++)
      mp->write_ids[0][ib] = ib;
    mp->total_write_no  = MECHANICAL_Var_NO;                  
  }
  return err;
}






void process_periodic_file(const Options &opt,
			   Periodic &periodic)
{
  std::string fname(opt.base_fname());
  fname += ".out.periodic";
  std::ifstream in(fname.c_str());
  file_check_error(in,fname.c_str());
  periodic.read(in);
  in.close();
  periodic.set_bounds(opt.per_bounds());
  // do other stuff...
}

void process_mesh_files(const Options &opt,
			Domains &domains)
{
  if(opt.nproc() <= 1) domains.begin()->set_serial();

  size_t max_gid = 0;
  #pragma omp parallel num_threads(opt.nthreads()) shared(opt,domains,max_gid)
  {
    Domains::iterator it;
    size_t dom_id, tmp;
    #pragma omp for schedule(static,CHUNKSIZE)
    for(dom_id = 0; dom_id < domains.size(); dom_id++){      //loop over domains
      it = domains.begin()+dom_id;    
      std::stringstream mesh;
      mesh << opt.base_fname() << ".out." << dom_id;
      std::ifstream in(mesh.str().c_str());
      file_check_error(in,mesh.str().c_str());
      it->read(in);
      tmp = it->get_max_gid();
      max_gid = (max_gid > tmp)? max_gid : tmp;
      it->sort_model();
      in.close();
    }
  }  //end pragma
  domains.num_global_nodes = max_gid + 1;
}
