/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby, Ivan Viti, Aaron Howell
 */

#include "header.h"
#include "material.h"
#include "boundary_conditions.h"
#include "initial_conditions.h"
#include "basis.h"
#include "utils.h"
#include <fstream>
#include "structures.h"

Header::Header() : _quadradic(0), _ndim(3)
{
  _mat_list = new MaterialList;
  _heat_list = new HeatList;
  _mat_assign = new MatAssignList;
  _pre_disp = new ModelBCList<int>;
  _pre_force = new ModelBCList<double>;
  _basis_list = new BasisList;
  _thermal = new ModelThermList<int>; 
}

Header::~Header()
{
  delete _mat_list;
  delete _mat_assign;
  delete _pre_disp;
  delete _pre_force;
  delete _basis_list;
  for (const auto& bc : boundary_conditions) //clean up vector of pointers
    delete bc;
  for (const auto& ic : initial_conditions)  //clean up vector of pointers
    delete ic;
}


//hr passers
void Header::passQuadratic(int input) {	
	_quadradic = input;
}

void Header::passNDim(int input) {
        _ndim = input;
}

void Header::passNiter(int input) {
        _niter = input;
}

void Header::passLintol(double input) {
	_lin_tol = input;
}

void Header::passEpsilon(double input) {
	_epsilon = input;
}


/*
  Read into header directly and blindly.
  Only called if not using human readable files from the input library
*/
void Header::read(std::istream &in)
{
  // read first line and allocate appropriately
  {
    size_t n_materials;
    size_t n_concentrations;
    size_t n_bases;
    in >> _quadradic >> _ndim >> n_materials >> n_concentrations >> n_bases;//1
   
    _mat_list->resize(n_materials);
    _con.resize(n_concentrations);
    _basis_list->resize(n_bases);
  }

  // read the solver information
  in >> _niter >> _lin_tol >> _epsilon; //2

  // read the materials
  _mat_list->read(in); //3

  // read the concentrations
  in >> _con;//  -IV  //not used? // 4?

  // read the orientations
  _basis_list->read(in);// e_ij? -IV //5

  // read the material assignments
  _mat_assign->read(in);// # of region? -IV //6 & 7 (n_assignments = #_of_region)

  // read the prescribed displacement assignments
  {
    size_t n_pre_disp, n_disp;
    in >> n_pre_disp; //  -IV //8
    _pre_disp->resize(n_pre_disp);
    _pre_disp->read(in);//    //9

    // read the displacements
    in >> n_disp; //10 
    _disp.resize(n_disp);
    in >> _disp; //11
  }

  // read the prescribed forces
  {
    size_t n_pre_force;
    in >> n_pre_force; //12 
    _pre_force->resize(n_pre_force);
    _pre_force->read(in); //also 12?
  }

  // the remainder of the file is deprecated/unsupported and is
  // therefore not read.
}

void Header::write(std::ostream &out) const
{
  char s = ' ';
  out << quadradic() << s << n_dim() << s << n_materials() << s
      << n_concentrations() << s << n_orientations()
      << std::endl << std::endl

      << max_lin_iter() << s << lin_tol() << s << epsilon()
      << std::endl << std::endl

      << *materials() << std::endl

      << concentrations() << std::endl
      << *orientations() << std::endl

      << mat_assignment()->size() << std::endl
      << *mat_assignment() << std::endl
    
      << disp_assignment()->size() << std::endl
      << *disp_assignment() << std::endl

      << displacements().size() << std::endl
      << displacements() << std::endl << std::endl

      << force_assignment()->size() << std::endl
      << *force_assignment() << std::endl;
}

size_t Header::quadradic() const
{
  return _quadradic;
}

size_t Header::n_dim() const
{
  return _ndim;
}

size_t Header::n_materials() const
{
  return _mat_list->size();
}
//Human-readable Functions


double Header::readMaterials(int i,int j) 
{
	double element;
	Material temp;
	temp = _mat_list->at(i);
	element = temp.readMat(j);
	return element;
}

void Header::fillMaterialList(Header &header, Input_Data inputs, int n_materials) {
  int i;
  Material tempMat;
  std::vector<double> tempVec;
  std::vector<double> tempFlags;
  for (i = 0; i < n_materials; i++) {   //loop over number of materials
    tempVec.push_back(inputs.material_list[i].youngs_modulus[0]);
    tempVec.push_back(inputs.material_list[i].youngs_modulus[1]);
    tempVec.push_back(inputs.material_list[i].youngs_modulus[2]);
    tempVec.push_back(inputs.material_list[i].shear_modulus[0]);
    tempVec.push_back(inputs.material_list[i].shear_modulus[1]);
    tempVec.push_back(inputs.material_list[i].shear_modulus[2]);
    tempVec.push_back(inputs.material_list[i].poissons_ratio[0]);
    tempVec.push_back(inputs.material_list[i].poissons_ratio[1]);
    tempVec.push_back(inputs.material_list[i].poissons_ratio[2]);
    tempVec.push_back(inputs.material_list[i].coefficient_of_thermal_expansion[0]);
    tempVec.push_back(inputs.material_list[i].coefficient_of_thermal_expansion[1]);
    tempVec.push_back(inputs.material_list[i].coefficient_of_thermal_expansion[2]);
    tempVec.push_back(inputs.material_list[i].sig);
    tempFlags.push_back(inputs.material_list[i].strain_energy_function_dev);
    tempFlags.push_back(inputs.material_list[i].strain_energy_function_vol);

    tempMat = *tempMat.fillMaterial(tempVec,tempFlags);
    header._mat_list->push_back(tempMat); 	

    tempVec.clear();
    tempFlags.clear();


  }
}

double Header::readHeat(int i,int j)
{
  double element;
  Heat tempHeat;
  tempHeat = _heat_list->at(i);
  element = tempHeat.readHeat(j);
  return element; 

}

void Header::fillHeatList(Header &header, Input_Data inputs, int n_materials) {
  int i;
  Heat tempHeat;
  std::vector<double> tempVec;
  for (i = 0; i < n_materials; i++) {                                                             //loop over number of materials
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[0]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[1]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[2]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[3]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[4]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[5]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[6]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[7]);
    tempVec.push_back(inputs.material_list[i].thermal_conductivity[8]);
    tempVec.push_back(inputs.material_list[i].heat_capacity);

    tempHeat = *tempHeat.fillHeat(tempVec);
    header._heat_list->push_back(tempHeat);
    
    tempVec.clear();
  }
}

void Header::fillCon(Header &header, Input_Data inputs) {
	int i;
	for (i = 0; i < inputs.number_of_volume_fraction; i++) {
		header._con.push_back(inputs.volume_fraction[i]);
	}
}



void Header::fillBasisList(Header &header, Input_Data inputs, int n_bases) {
        int i;
	Basis tempBasis;
	std::vector<double> tempDat;
	for (i = 0; i < n_bases; i++) {
		tempDat.push_back(inputs.basis_vectors[i].e1[0]);
		tempDat.push_back(inputs.basis_vectors[i].e1[1]);
                tempDat.push_back(inputs.basis_vectors[i].e1[2]);
                tempDat.push_back(inputs.basis_vectors[i].e2[0]);
                tempDat.push_back(inputs.basis_vectors[i].e2[1]);
                tempDat.push_back(inputs.basis_vectors[i].e2[2]);
                tempDat.push_back(inputs.basis_vectors[i].e3[0]);
                tempDat.push_back(inputs.basis_vectors[i].e3[1]);
                tempDat.push_back(inputs.basis_vectors[i].e3[2]);
	
	tempBasis = *tempBasis.fillBasis(tempDat);	
	header._basis_list->push_back(tempBasis);

	tempDat.clear();
	}
	

}

std::vector<double> Header::printT3d(){
  std::vector<double> tempVec;
	ModelBC<int> tempMod;

	tempMod = this->_pre_disp->at(17);
	tempVec = tempMod.grabT3d(tempVec);

return tempVec;
}



void Header::fillT3d(Input_Data inputs) { //should actually be called fill-materials
	MaterialAssignment tempMat;
	int i;
	for(i = 0; i < inputs.number_of_regions_to_set_material; i++) {
	  tempMat.putT3d(inputs, i);		
		this->_mat_assign->push_back(tempMat);
	}
}

//supports multiple physics
void Header::fillBC(Input_Data inputs, int physics) {

	//for each bc data vector
	for(int i = 0; i < inputs.physics_list[physics].bc_data.size(); i++) {
	  ModelBC<int> tempMod;   
	  tempMod.putT3d(inputs, i, physics);
	  this->boundary_conditions[physics]->push_back(tempMod);
	}

	this->boundary_conditions[physics]->sort_model();
}

//supports multiple physics
void Header::fillIC(Input_Data inputs, int physics) {

	//for each ic data vector
	for(int i = 0; i < inputs.physics_list[physics].g_id.size(); i++) {
	  ModelIC<int> tempMod;	
	  tempMod.putIC(inputs, i, physics);
	  this->initial_conditions[physics]->push_back(tempMod);
	}

	this->initial_conditions[physics]->sort_model();
}



void Header::fillPrescribed(std::vector<double> old_disp, int n_disp) {
	int i;
	for (i = 0; i < n_disp; i++) {
	  this->_disp.push_back(old_disp[i]);
	}
}
//supports multiple physics
void Header::fillPrescribed(std::vector<double> old_disp, int n_disp, int physics) {
  std::vector<double> disp;
  int i;
  for (i = 0; i < n_disp; i++) {
    disp.push_back(old_disp[i]);
  }
  this->bc_replacements.push_back(disp);
}

void Header::fillPrescribedThermal(double *old_thermal, int n_therm_types) {
  int i;
  for (i = 0; i < n_therm_types; i++) {
    this->_therm_types.push_back(old_thermal[i]);
  }
}

/*
  BCs and ICs are now vectors to support multiple physics, so each BC and IC
  pointer needs to be allocated
*/
void Header::allocateBCIC(int physics_num) {
  for (int physics = 0; physics < physics_num; ++physics){
    ModelBCList<int> *bc = new ModelBCList<int>;
    ModelICList<int> *ic = new ModelICList<int>;
    this->boundary_conditions.push_back(bc);
    this->initial_conditions.push_back(ic);
  }
}


//end HR Functions
size_t Header::n_concentrations() const
{
  return _con.size();
}

size_t Header::n_orientations() const
{
  return _basis_list->size();
}

size_t Header::max_lin_iter() const
{
  return _niter;
}

double Header::lin_tol() const
{
  return _lin_tol;
}

double Header::epsilon() const
{
  return _epsilon;
}


const MaterialList* Header::materials() const
{
  return _mat_list;
}

const HeatList* Header::heat() const
{
  return _heat_list;
}

const std::vector<double>& Header::concentrations() const
{
  return _con;
}

const BasisList* Header::orientations() const
{
  return _basis_list;
}


const MatAssignList* Header::mat_assignment() const
{
  return _mat_assign;
}

const ModelBCList<int>* Header::disp_assignment() const
{
  return _pre_disp;
}

const ModelBCList<int>* Header::disp_assignment(int i) const
{
  return boundary_conditions[i];
}

const ModelICList<int>* Header::get_initial_conditions(int i) const
{
  return initial_conditions[i];
}

const ModelThermList<int> * Header::therm_list() const
{
  return _thermal;
}

const std::vector<double>& Header::displacements() const
{
  return _disp;
}

const std::vector<double>& Header::therm_types() const
{
  return _therm_types;
}

const ModelBCList<double>* Header::force_assignment() const
{
  return _pre_force;
}


void Header::sort_and_unique_assignments() const
{
  this->sort_and_unique_mat_assignment();
  this->sort_and_unique_disp_assignment();
  this->sort_and_unique_force_assignment();
}

void Header::sort_and_unique_mat_assignment() const
{
  this->_mat_assign->sort_model();
  // unique
}

void Header::sort_and_unique_disp_assignment() const
{
  this->_pre_disp->sort_model();
  // unique
}

void Header::sort_and_unique_force_assignment() const
{
  this->_pre_disp->sort_model();
  // unique
}
