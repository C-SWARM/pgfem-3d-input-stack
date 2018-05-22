### numerical_param.json ###

This file follows the following format:
```c++
{
  "Newton_Raphson_tolerance": (double),           //defaults to 1.0e-5
  "Newton_Raphson_max_num_of_iterations": (int),  //defaults to 5
  "order_of_elements": (int),                     //defaults to 0
  "number_of_spatial_dimensions": (int),          //defaults to 3; NOTE: this should always be 3
  "solver_max_num_of_iterations": (int),          //defaults to 1000
  "solver_num_of_Krylov_subspaces": (int),        //defaults to 300
  "solver_tolerance": (double),                   //defaults to 1.0e-5
  "absolute_tolerance": (double),                 //defaults to 1.0e-15
  "pressure_num": (int),                          //defaults to 0 

  "multiphysics_staggering_method":
  {
    "maximum_num_of_staggering": (int),           //defaults to 5
    "solution_scheme": (int)                      //defaults to 4
  },
  "nonlinear_solution_method": {    
        "tolerance": (double),                    //defaults to 1.0e-5
        "max_num_of_iterations": (int),           //defaults to 5
        "physics": [                              //only needed for method 4 (advanced user options) 
          {                                       //maximum one object per physics, enclosed in {} and separated by a comma
          "physics_ID": 0,                        //refers to the order of the physics listed in multiphysics.in
          "max_num_of_iterations": 4,                 //defaults to 0
          "compute_initial_residuals": (true/false),  //defaults to false
		  "value_for_initial_residuals": (double)     //only used if "compute_initial_residuals" is true
          },
    	      ...  //up to 1 object per physics
      ]   //end of "physics"
  }       //end of "nonlinear_solution_method"
}
```
sample numerical_param.json file: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/blob/aaron_input/Run/numerical_param.json
