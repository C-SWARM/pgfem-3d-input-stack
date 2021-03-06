
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gaussP.h"


static GAUSS_error_t error = GAUSS_OK;


#define RAISE_ERR(err) { error = err; }


#define FOR(_Counter_, _Up_) for (_Counter_ = 0; _Counter_ < _Up_; _Counter_++)



#define gauss_new(T)                 ((T *)malloc(sizeof(T)))
#define gauss_new_array(N, T)        ((T *)calloc((N), (size_t) sizeof(T)))
#define gauss_realloc_array(P, N, T) ((T *)realloc(P, (size_t)(N * sizeof(T))))
#define gauss_free(PTR) free(PTR)
#define CHECK_MALLOC(ptr, if_fail)                       \
   {                                                     \
     if (ptr == NULL) {                                  \
       { RAISE_ERR(GAUSS_NO_MEMORY); }                   \
       { if_fail; };                                     \
     }                                                   \
   }



#define GQR_NAME(dim, order) GQR_##dim##d_##order
#define DCL_GQR(order)                                                  \
   static GAUSS_2d_integration_rule_t GQR_NAME(2, order) = {       \
                order, 2, 0, NULL                                       \
   };                                                                   \
   static GAUSS_3d_integration_rule_t GQR_NAME(3, order) = {       \
                order, 3, 0, NULL                                       \
   }

#define GP_NAME(order) GP_##order
#define DCL_GP(order) static GAUSS_1d_point_data_t GP_NAME(order)[order]

DCL_GP(1) = {
  {0, 2}
};
DCL_GQR(1);

DCL_GP(2) = {
  {-0.577350269189626, 1},
  { 0.577350269189626, 1},
};
DCL_GQR(2);

DCL_GP(3) = {
  {-0.774596669241483, 0.5555555555555556},
  {                 0, 0.8888888888888889},
  { 0.774596669241483, 0.5555555555555556}
};
DCL_GQR(3);

DCL_GP(4) = {
  {-0.861136311594053, 0.347854845137454},
  {-0.339981043584856, 0.652145154862546},
  { 0.339981043584856, 0.652145154862546},
  { 0.861136311594053, 0.347854845137454}
};
DCL_GQR(4);

DCL_GP(5) = {
  {-0.906179845938664, 0.236926885056189},
  {-0.538469310105683, 0.478628670499366},
  {                 0, 0.568888888898889},
  { 0.538469310105683, 0.478628670499366},
  { 0.906179845938664, 0.236926885056189}
};
DCL_GQR(5);

DCL_GP(6) = {
  {-0.932469514203152, 0.171324492379170},
  {-0.661209386466265, 0.360761573048139},
  {-0.238619186083197, 0.467913934572691},
  { 0.238619186083197, 0.467913934572691},
  { 0.661209386466265, 0.360761573048139},
  { 0.932469514203152, 0.171324492379170}
};
DCL_GQR(6);

DCL_GP(7) = {
  {-0.949107912300000, 0.129484966200000},
  {-0.741531185600000, 0.279705391500000},
  {-0.405845151400000, 0.381830050500000},
  {                 0, 0.417959183700000},
  { 0.405845151400000, 0.381830050500000},
  { 0.741531185600000, 0.279705391500000},
  { 0.949107912300000, 0.129484966200000}
};
DCL_GQR(7);

DCL_GP(8) = {
  {-0.960289856500000, 0.101228536300000},
  {-0.796666477400000, 0.222381034500000},
  {-0.525532409900000, 0.313706645900000},
  {-0.183434642500000, 0.362683783400000},
  { 0.183434642500000, 0.362683783400000},
  { 0.525532409900000, 0.313706645900000},
  { 0.796666477400000, 0.222381034500000},
  { 0.960289856500000, 0.101228536300000}
};
DCL_GQR(8);

DCL_GP(9) = {
  {-0.968160239507600, 0.081274388361570},
  {-0.836031107326600, 0.180648160694900},
  {-0.613371432700600, 0.260610696402900},
  {-0.324253423403800, 0.312347077040000},
  {                 0, 0.330239355012600},
  { 0.324253423403800, 0.312347077040000},
  { 0.613371432700600, 0.260610696402900},
  { 0.836031107326600, 0.180648160694900},
  { 0.968160239507600, 0.081274388361570}
};
DCL_GQR(9);

DCL_GP(10) = {
  {-0.973906528500000, 0.066671344308700},
  {-0.865063366700000, 0.149451349150600},
  {-0.679409568300000, 0.219086362516000},
  {-0.433395394100000, 0.295524224714800},
  {-0.148874339000000, 0.295524224714800},
  { 0.148874339000000, 0.295524224714800},
  { 0.433395394100000, 0.295524224714800},
  { 0.679409568300000, 0.219086362516000},
  { 0.865063366700000, 0.149451349150600},
  { 0.973906528500000, 0.066671344308700},
};
DCL_GQR(10);

DCL_GP(11) = {
  {-0.978228658146100, 0.055668567116170},
  {-0.887062599768100, 0.125580369464900},
  {-0.730152005574000, 0.186290210927700},
  {-0.519096129206800, 0.233193764591990},
  {-0.269543155952300, 0.262804544510200},
  {                 0, 0.272925086777900},
  { 0.269543155952300, 0.262804544510200},
  { 0.519096129206800, 0.233193764591990},
  { 0.730152005574000, 0.186290210927700},
  { 0.887062599768100, 0.125580369464900},
  { 0.978228658146100, 0.055668567116170},
};
DCL_GQR(11);

DCL_GP(12) = {
  {-0.981560634246719, 0.047175336386512},
  {-0.904117256370475, 0.106939325995318},
  {-0.769902674194305, 0.160078328543346},
  {-0.587317954286617, 0.203167426723066},
  {-0.367831498918180, 0.233492536538355},
  {-0.125333408511469, 0.249147045813403},
  { 0.125333408511469, 0.249147045813403},
  { 0.367831498918180, 0.233492536538355},
  { 0.587317954286617, 0.203167426723066},
  { 0.769902674194305, 0.160078328543346},
  { 0.904117256370475, 0.106939325995318},
  { 0.981560634246719, 0.047175336386512},
};
DCL_GQR(12);

DCL_GP(14) = {
  {-0.986283808696812, 0.035119460331752},
  {-0.928434883663574, 0.080158087159760},
  {-0.827201315069765, 0.121518570687903},
  {-0.687292904811685, 0.157203167158194},
  {-0.515248636358154, 0.185538397477938},
  {-0.319112368927890, 0.205198463721296},
  {-0.108054948707344, 0.215263853463158},
  { 0.108054948707344, 0.215263853463158},
  { 0.319112368927890, 0.205198463721296},
  { 0.515248636358154, 0.185538397477938},
  { 0.687292904811685, 0.157203167158194},
  { 0.827201315069765, 0.121518570687903},
  { 0.928434883663574, 0.080158087159760},
  { 0.986283808696812, 0.035119460331752},
}; 
DCL_GQR(14);
 
DCL_GP(16) = {
  {-0.989400934991650, 0.027152459411754},
  {-0.944575023073233, 0.062253523938648},
  {-0.865631202387832, 0.095158511682493},
  {-0.755404408355003, 0.124628971255534},
  {-0.617876244402644, 0.149595988816577},
  {-0.458016777657227, 0.169156519395003},
  {-0.281603550779259, 0.182603415044924},
  {-0.095012509837637, 0.189450610455069},
  { 0.095012509837637, 0.189450610455069},
  { 0.281603550779259, 0.182603415044924},
  { 0.458016777657227, 0.169156519395003},
  { 0.617876244402644, 0.149595988816577},
  { 0.755404408355003, 0.124628971255534},
  { 0.865631202387832, 0.095158511682493},
  { 0.944575023073233, 0.062253523938648},
  { 0.989400934991650, 0.027152459411754},
};
DCL_GQR(16);

DCL_GP(24) = {
  {-0.9951872199, 0.01234122979},
  {-0.9747285559, 0.02853138862},
  {-0.9382745520, 0.04427743881},
  {-0.8864155270, 0.05929858491},
  {-0.8200019859, 0.07334648141},
  {-0.7401241915, 0.08619016153},
  {-0.6480936519, 0.09761865210},
  {-0.5454214713, 0.1074442701},
  {-0.4337935076, 0.1155056680},
  {-0.3150426796, 0.1216704729},
  {-0.1911188674, 0.1258374563},
  {-0.0640568929, 0.1279381953},
  { 0.0640568929, 0.1279381953},
  { 0.1911188674, 0.1258374563},
  { 0.3150426796, 0.1216704729},
  { 0.4337935076, 0.1155056680},
  { 0.5454214713, 0.1074442701},
  { 0.6480936519, 0.09761865210},
  { 0.7401241915, 0.08619016153},
  { 0.8200019859, 0.07334648141},
  { 0.8864155270, 0.05929858491},
  { 0.9382745520, 0.04427743881},
  { 0.9747285559, 0.02853138862},
  { 0.9951872199, 0.01234122979},
};
DCL_GQR(24);



static int
reconstruct_3d(GAUSS_3d_integration_rule_t *r,
               GAUSS_1d_point_data_t *ip);


GAUSS_3d_integration_rule_t *
gauss_rule_3d(int order)
{
#define GQR_CASE(o)                                     \
 case o:                                                \
    if (GQR_NAME(3, o).ip == NULL) {                    \
      if (!reconstruct_3d(&GQR_NAME(3, o), GP_NAME(o))) \
        return NULL;                                    \
    }                                                   \
    return &GQR_NAME(3, o);                             \
    break

  switch (order) {
  GQR_CASE(1);
  GQR_CASE(2);
  GQR_CASE(3);
  GQR_CASE(4);
  GQR_CASE(5);
  GQR_CASE(6);
  GQR_CASE(7);
  GQR_CASE(8);
  GQR_CASE(9);
  GQR_CASE(10);
  GQR_CASE(11);
  GQR_CASE(12);
  GQR_CASE(14);
  GQR_CASE(16);
  GQR_CASE(24);
  default:
     RAISE_ERR(GAUSS_BAD_GAUSS_RULE);
     break;
  }
  return NULL;
#undef GQR_CASE
}


static int
reconstruct_3d(GAUSS_3d_integration_rule_t *r,
               GAUSS_1d_point_data_t *ip)
{
  int i, j, k, c;

  r->npoints = r->order * r->order * r->order;
  r->ip = gauss_new_array(r->npoints, GAUSS_3d_point_data_t);
  CHECK_MALLOC(r->ip, return 0);

  c = 0;
  FOR(i, r->order) {
    FOR(j, r->order) {
      FOR(k, r->order) {
        r->ip[c].xi    = ip[i].Loc;
        r->ip[c].eta   = ip[j].Loc;
        r->ip[c].theta = ip[k].Loc;
        r->ip[c].Wgt   = ip[i].Wgt * ip[j].Wgt * ip[k].Wgt;
        c++;
      }
    }
  }
  return 1;
}



static int
reconstruct_2d(GAUSS_2d_integration_rule_t *r,
               GAUSS_1d_point_data_t *ip);


GAUSS_2d_integration_rule_t *
gauss_rule_2d(int order)
{
#define GQR_CASE(o)                                     \
 case o:                                                \
    if (GQR_NAME(2, o).ip == NULL) {                    \
      if (!reconstruct_2d(&GQR_NAME(2, o), GP_NAME(o))) \
        return NULL;                                    \
    }                                                   \
    return &GQR_NAME(2, o);                             \
    break

  switch (order) {
  GQR_CASE(1);
  GQR_CASE(2);
  GQR_CASE(3);
  GQR_CASE(4);
  GQR_CASE(5);
  GQR_CASE(6);
  GQR_CASE(7);
  GQR_CASE(8);
  GQR_CASE(9);
  GQR_CASE(10);
  GQR_CASE(11);
  GQR_CASE(12);
  GQR_CASE(14);
  GQR_CASE(16);
  GQR_CASE(24);
  default:
     RAISE_ERR(GAUSS_BAD_GAUSS_RULE);
     break;
  }
  return NULL;
}


static int
reconstruct_2d(GAUSS_2d_integration_rule_t *r,
               GAUSS_1d_point_data_t *ip)
{
  int i, j, c;

  r->npoints = r->order * r->order;
  r->ip = gauss_new_array(r->npoints, GAUSS_2d_point_data_t);
  CHECK_MALLOC(r->ip, return 0);

  c = 0;
  FOR(i, r->order) {
    FOR(j, r->order) {
      r->ip[c].xi    = ip[i].Loc;
      r->ip[c].eta   = ip[j].Loc;
      r->ip[c].Wgt   = ip[i].Wgt * ip[j].Wgt;
      c++;
    }
  }
  return 1;
}



GAUSS_error_t
gauss_status(void)
{
  return error;
}
