#include <iostream>

#include "utils.h"
#include "t3d_node.h"

using namespace std;

T3d_Node::T3d_Node()
{
  l_id = g_id = ent_type = ent_id = ent_did = ent_prop = domain = 0;
  x = y = z = 0;
  global = false;
}

void T3d_Node::read(istream *file)
{
  check_stream(file);
  *file >> l_id >> x >> y >> z >> ent_type >> ent_id >> ent_prop;
}

void T3d_Node::print(ostream *file)
{
  check_stream(file);
  (*file).precision(7);
  (*file).setf(ios::scientific,ios::floatfield);
  *file << "\t" << l_id << "\t" << x << "\t" << y << "\t" << z << "\t" 
	<< ent_type << "\t" << ent_id << "\t" << ent_prop << endl;
}

void T3d_Node::write(ostream *file)
{
  check_stream(file);

  (*file).precision(6);
  (*file).setf(ios::scientific,ios::floatfield);
  *file << "\t" << l_id << "\t";
  
  // Global ID
  if(g_id <= 0)
    *file << 0 << "\t";
  else
    *file << g_id << "\t";

  // Domain
  if(domain <= 0)
    *file << 1 << "\t";
  else
    *file << domain << "\t";

  // Coords and props
  *file	<< x << "\t" << y << "\t" << z << "\t" << ent_type
	<< "\t" << ent_did << "\t" << ent_prop << endl;
}

void T3d_Node::translate(double ddx, double ddy, double ddz)
{
  x -= ddx;
  y += ddy;
  z += ddz;
}

void T3d_Node::translate(double delta[3])
{
  x -= delta[0];
  y += delta[1];
  z += delta[2];
}

T3d_Node& T3d_Node::operator=(const T3d_Node &node)
{
  l_id = node.l_id;
  g_id = node.g_id;
  ent_type = node.ent_type;
  ent_id = node.ent_id;
  ent_did = node.ent_did;
  ent_prop = node.ent_prop;
  domain = node.domain;

  x = node.x;
  y = node.y;
  z = node.z;

  global = node.global;
  return *this;
}
