/* HEADER */
/**
 * AUTHORS:
 * Matt Mosby
 * Ivan Viti
 */

#include "boundary_conditions.h"
#include "utils.h"
#include <iomanip>
#include <algorithm>
#include <fstream>
#include "structures.h"


/*======= BaseBC ======*/
template<typename T>
void  BaseBC<T>::set_bc(const std::vector<T> &bc)
{_bc = bc;}

template<typename T>
void  BaseBC<T>::set_bc_x(T x)
{_bc[0] = x;}

template<typename T>
void  BaseBC<T>::set_bc_y(T y)
{_bc[1] = y;}

template<typename T>
void  BaseBC<T>::set_bc_z(T z)
{_bc[2] = z;}

template<typename T>
const std::vector<T>& BaseBC<T>::bc()const
{return _bc;}

template<typename T>
T BaseBC<T>::bc_x() const
{return _bc[0];}

template<typename T>
T  BaseBC<T>::bc_y() const
{return _bc[1];}

template<typename T>
T  BaseBC<T>::bc_z() const
{return _bc[2];}


/*====== BaseTherm ========*/

template<typename T>
void  BaseTherm<T>::set_th(const std::vector<T> &th)
{_th = th;}

template<typename T>
const std::vector<T>& BaseTherm<T>::th()const
{return _th;}


template<typename T>
void  BaseTherm<T>::set_th_1(T th_1)
{_th[0] = th_1;}
template<typename T>
T BaseTherm<T>::th_1() const
{return _th[0];}



/*======= ModelBC ======*/
template<typename T>
void ModelBC<T>::set_model_type(const size_t t)
{
  _model.set_type(t);
}


template<typename T>
void ModelBC<T>::set_model_id(const size_t i)
{
  _model.set_id(i);
}



template<typename T>
size_t ModelBC<T>::model_type() const
{
  return _model.type();
}


template<typename T>
size_t ModelBC<T>::model_id() const
{
  return _model.id();
}


template<typename T>
T3dModel ModelBC<T>::model() const
{
  return _model;
}



template<typename T>
T3dModel ModelThermal<T>::model() const
{
  return _model;
}

template<typename T>
void ModelBC<T>::read(std::istream &in)
{
  this->_bc.resize(3);
  in >> _model >> this->_bc;
}

template<typename T>
void ModelBC<T>::write(std::ostream &out) const
{
  out << _model << "\t" << this->_bc;
}

template<typename T>
bool ModelBC<T>::compare_model(const ModelBC<T> &a, const ModelBC<T> &b)
{
  return T3dModel::compare(a.model(),b.model());
}



template<typename T>
std::vector<double> ModelBC<T>::grabT3d(std::vector <double> claw) {

        claw.push_back(this->_model.type());
        claw.push_back(this->_model.id());
printf("the values in this t3d-model are %d and %d \n",this->_model.type(),this->_model.id());

      return claw;
}




template<typename T>
void ModelBC<T>::putT3d(Input_Data inputs, int i, int physics) {
  this->_model.first  = inputs.physics_list[physics].bc_data[i][0];
  this->_model.second = inputs.physics_list[physics].bc_data[i][1];

  //add all remaining BC input data (it's variable due to multiphysics)
  for (int j = 2; j < inputs.physics_list[physics].bc_data[i].size(); ++j)
    this->_bc.push_back(inputs.physics_list[physics].bc_data[i][j]);
  
}




template<typename T>
bool ModelThermal<T>::compare_model(const ModelThermal<T> &a, const ModelThermal<T> &b)
{
  return T3dModel::compare(a.model(),b.model());
}


//fills the thermal class
/*
template<typename T>
void ModelThermal<T>::putTherm(struct data_header header_struct, int i) {
  this->_model.first = header_struct.geometry_type_t[i];
  this->_model.second = header_struct.geometry_id[i];
  this->set_th_1(header_struct.temperature[i]);                          //not actual value, just marker (0,1,-1, etc...)
                                                                            //actual values go in bcv
}
*/

/*======= NodeBC ======*/
template<typename T>
void NodeBC<T>::set_node_id(size_t i)
{
  _node_id = i;
}

template<typename T>
size_t NodeBC<T>::node_id() const
{
  return _node_id;
}

template<typename T>
void NodeBC<T>::write(std::ostream &out) const
{
  out << std::setw(4) << _node_id << "  " << this->_bc;
}

template<typename T>
bool NodeBC<T>::compare_node_id(const NodeBC<T> &a, const NodeBC<T> &b)
{
  return (a.node_id() < b.node_id());
}

/*======= ModelBCList ======*/
template<typename T>
void ModelBCList<T>::read(std::istream &in)
{
  typename ModelBCList<T>::iterator it;
  for(it = this->begin(); it != this->end(); ++it) in >> *it;
}

template<typename T>
void ModelBCList<T>::write(std::ostream &out) const
{
  typename ModelBCList<T>::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    out << *it << std::endl;
}

template<typename T>
void ModelBCList<T>::sort_model()
{
  std::sort(this->begin(), this->end(), ModelBC<T>::compare_model);
}

/*====== ThermalBCList ====*/
template<typename T>
void ModelThermList<T>::sort_model()
{
  std::sort(this->begin(),this->end(), ModelThermal<T>::compare_model);
}

/*======= NodeBCList ======*/
template<typename T>
void NodeBCList<T>::write(std::ostream &out) const
{
  typename NodeBCList<T>::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    out << *it << std::endl;
}

template<typename T>
void NodeBCList<T>::sort_node_id()
{
  std::sort(this->begin(), this->end(), NodeBC<T>::compare_node_id);
}



// instantiate [T = int] [T = double]
template class BaseBC<int>;
template class BaseBC<double>;
template class BaseTherm<int>;
template class BaseTherm<double>;
template class ModelBC<int>;
template class ModelBC<double>;
template class ModelThermal<int>;
template class ModelThermal<double>;
template class NodeBC<int>;
template class NodeBC<double>;
template class ModelBCList<int>;
template class ModelBCList<double>;
template class NodeBCList<int>;
template class NodeBCList<double>;
template class ModelThermList<int>;
template class ModelThermList<double>;
