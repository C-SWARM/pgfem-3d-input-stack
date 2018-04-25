/* HEADER */
/**
 * AUTHORS:
 * Aaron Howell
 * Matt Mosby
 * Ivan Viti
 */

#include "initial_conditions.h"
#include "utils.h"
#include <iomanip>
#include <algorithm>
#include <fstream>
#include "structures.h"


/*======= BaseIC ======*/
template<typename T>
const std::vector<T>& BaseIC<T>::ic()const
{return _ic;}

template<typename T>
void  BaseIC<T>::set_ic(const std::vector<T> &ic)
{_ic = ic;}

template<typename T>
void  BaseIC<T>::set_ic_0(T x0)
{_ic[0] = x0;}
template<typename T>
T BaseIC<T>::ic_0() const
{return _ic[0];}

template<typename T>
void  BaseIC<T>::set_ic_1(T x1)
{_ic[1] = x1;}
template<typename T>
T BaseIC<T>::ic_1() const
{return _ic[1];}

template<typename T>
void  BaseIC<T>::set_ic_2(T x2)
{_ic[2] = x2;}
template<typename T>
T BaseIC<T>::ic_2() const
{return _ic[2];}

template<typename T>
void  BaseIC<T>::set_ic_3(T x3)
{_ic[3] = x3;}
template<typename T>
T BaseIC<T>::ic_3() const
{return _ic[3];}

template<typename T>
void  BaseIC<T>::set_ic_4(T x4)
{_ic[4] = x4;}
template<typename T>
T BaseIC<T>::ic_4() const
{return _ic[4];}

template<typename T>
void  BaseIC<T>::set_ic_5(T x5)
{_ic[5] = x5;}
template<typename T>
T BaseIC<T>::ic_5() const
{return _ic[5];}




/*======= ModelIC/IC ======*/
template<typename T>
void ModelIC<T>::set_model_type(const size_t t)
{
  _model.set_type(t);
}


template<typename T>
void ModelIC<T>::set_model_id(const size_t i)
{
  _model.set_id(i);
}



template<typename T>
size_t ModelIC<T>::model_type() const
{
  return _model.type();
}


template<typename T>
size_t ModelIC<T>::model_id() const
{
  return _model.id();
}


template<typename T>
T3dModel ModelIC<T>::model() const
{
  return _model;
}


template<typename T>
void ModelIC<T>::read(std::istream &in)
{
  in >> _model >> this->_ic;
}

template<typename T>
void ModelIC<T>::write(std::ostream &out) const
{
  out << _model << "\t" << this->_ic;
}

template<typename T>
bool ModelIC<T>::compare_model(const ModelIC<T> &a, const ModelIC<T> &b)
{
  return T3dModel::compare(a.model(),b.model());
}



template<typename T>
std::vector<double> ModelIC<T>::grabT3d(std::vector <double> claw) {

        claw.push_back(this->_model.type());
        claw.push_back(this->_model.id());
printf("the values in this t3d-model are %d and %d \n",this->_model.type(),this->_model.id());

      return claw;
}



template<typename T>
void ModelIC<T>::putIC(Input_Data inputs, int i, int physics) {
  this->_model.first  = inputs.physics_list[physics].g_id[i][0];
  this->_model.second = inputs.physics_list[physics].g_id[i][1];

  //add all remaining IC input data (it's variable due to multiphysics)
  for (int j = 2; j < inputs.physics_list[physics].g_id[i].size(); ++j)
    this->_ic.push_back(inputs.physics_list[physics].g_id[i][j]);
  
}


/*======= NodeIC ======*/
template<typename T>
void NodeIC<T>::set_node_id(size_t i)
{
  _node_id = i;
}

template<typename T>
size_t NodeIC<T>::node_id() const
{
  return _node_id;
}

template<typename T>
void NodeIC<T>::write(std::ostream &out) const
{
  out << std::setw(4) << _node_id << "  " << this->_ic;
}


template<typename T>
bool NodeIC<T>::compare_node_id(const NodeIC<T> &a, const NodeIC<T> &b)
{
  return (a.node_id() < b.node_id());
}

/*======= ModelICList ======*/
template<typename T>
void ModelICList<T>::read(std::istream &in)
{
  typename ModelICList<T>::iterator it;
  for(it = this->begin(); it != this->end(); ++it) in >> *it;
}

template<typename T>
void ModelICList<T>::write(std::ostream &out) const
{
  typename ModelICList<T>::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    out << *it << std::endl;
}

template<typename T>
void ModelICList<T>::sort_model()
{
  std::sort(this->begin(), this->end(), ModelIC<T>::compare_model);
}

/*======= NodeICList ======*/
template<typename T>
void NodeICList<T>::write(std::ostream &out) const
{
  typename NodeICList<T>::const_iterator it;
  for(it = this->begin(); it != this->end(); ++it)
    out << *it << std::endl;
}

template<typename T>
void NodeICList<T>::sort_node_id()
{
  std::sort(this->begin(), this->end(), NodeIC<T>::compare_node_id);
}


template<typename T>
void NodeICList<T>::write_replacements(Input_Data inputs, int phys, std::ofstream &out)
{
  typename NodeICList<T>::iterator it;
  for(it = this->begin(); it != this->end(); ++it){
    out << std::setw(4) << it->node_id() << "  ";
    for (uint j = 0; j < it->_ic.size(); ++j) {
      //replace negative numbers using the replacement list
      //for example, -2 will be replaced with the 2nd number in the physics.replacement vector
      if(it->_ic[j] >= 0) {  //don't replace non negatives
	out << it->_ic[j] << " ";
	continue;
      }

      //check if replacement wasn't provided (eg: replacement array doesn't have that many elements)
      if ( (uint)abs(it->_ic[j]) > inputs.physics_list[phys].ic_replacements.size() ) {
	std::cout << "No replacement match for " << it->_ic[j] << std::endl;
	return;
      }
      //if a replacement was found, write it instead
      else{
	int replacement_num = abs(it->_ic[j]) - 1;  //subtract 1 since indexing starts at 0
        out << inputs.physics_list[phys].ic_replacements[replacement_num] << " ";               //replace the IC value with the replacement
      }
      
    }
    out << std::endl;
  }
}


// instantiate [T = int] [T = double]
template class BaseIC<int>;
template class BaseIC<double>;
template class ModelIC<int>;
template class ModelIC<double>;
template class NodeIC<int>;
template class NodeIC<double>;
template class ModelICList<int>;
template class ModelICList<double>;
template class NodeICList<int>;
template class NodeICList<double>;
