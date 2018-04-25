#ifndef GAUSSP_H
#   define GAUSSP_H


typedef struct GAUSS_3d_point_data_t {
  double  xi, eta, theta;
  double  Wgt;                  /* weight */
}              GAUSS_3d_point_data_t;

typedef struct GAUSS_2d_point_data_t {
  double  xi, eta;
  double  Wgt;                  /* weight */
}              GAUSS_2d_point_data_t;

typedef struct GAUSS_1d_point_data_t {
  double  Loc;                  /* location */
  double  Wgt;                  /* weight */
}              GAUSS_1d_point_data_t;


typedef struct GAUSS_3d_integration_rule_t {
  int                           order;
  int                           dim;
  int                           npoints; /* = order^dim */
  GAUSS_3d_point_data_t        *ip; /* ie, ip[npoints] */
}              GAUSS_3d_integration_rule_t;



typedef struct GAUSS_2d_integration_rule_t {
  int                           order;
  int                           dim;
  int                           npoints; /* = order^dim */
  GAUSS_2d_point_data_t        *ip; /* ie, ip[npoints] */
}              GAUSS_2d_integration_rule_t;




typedef enum GAUSS_error_t {
  GAUSS_OK                =  0,
  GAUSS_NO_MEMORY         =  1,
  GAUSS_BAD_GAUSS_RULE    =  2,
  GAUSS_SENTINEL        
}            GAUSS_error_t;



/* PUBLIC FUNCTIONS */

GAUSS_3d_integration_rule_t *
gauss_rule_3d(int order);

GAUSS_2d_integration_rule_t *
gauss_rule_2d(int order);

GAUSS_error_t
gauss_status(void);


#endif /*  GAUSSP_H */
