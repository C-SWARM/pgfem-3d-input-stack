### material_mat.json ###

This file follows the following format:
```c++
{
    //general material data
    "number_of_materials": (int),                //defaults to 1
    "number_of_volume_fraction": (int),          //defaults to size of "volume_fraction"
    "volume_fraction": [(int1),(int2),etc...],   //list of integers
    "density": [(double1), (double2), etc...],   //One per material; each defaults to 0
         
    "number_of_regions_to_set_material": (int),  //defaults to size of "material_regions"
    "material_regions": [                        //list of size-4 integer arrays
	  [(int OR "string"), (int), (int), (int)],          //[geometry type, geometry id, material ID 1, material ID 2]
	  ...                                    // geometry type: 1=vertex, 2=curve, 3=surface, 4=region, 5=patch, 7=interface
    ],                                           //end of "material_regions"

    "number_of_bases": (int),                    //defaults to size of "basis_vectors"
    "basis_vectors": [                           //list of objects with elements "e1", "e2", and "e3"
      {                                          //"e1", "e2", and "e3" are size-3 arrays
	    "e1": [(int), (int), (int)],         //each size-3 array defaults to [0,0,0]
	    "e2": [(int), (int), (int)],
	    "e3": [(int), (int), (int)]
      },
	  ...   //additional basis vectors
    ],                                           //end of "basis_vectors"

    "materials": [
           (list of materials, separated by commas)
        ]
}
```
Each `"materials"` element has the following format:	
```c++
     {
        //momentum
        "name": (string),                                                     //defaults to “ID”
        "ID": (int),                                                          //defualts to -1
        "youngs_modulus": [(double), (double), (double)],                     //defaults to [0, 0, 0]
        "shear_modulus": [(double), (double), (double)],                      //defaults to [0, 0, 0]
        "poissons_ratio": [(double), (double), (double)],                     //defaults to [0, 0, 0]
        "coefficient_of_thermal_expansion": [(double), (double), (double)],   //defaults to [0, 0, 0]
	    "mooney_rivlin": [(double), (double)]                                 //defaults to [0, 0]
	    "sig": (double),                                                      //defaults to 0
        "strain_energy_function_dev": (double),                               //defaults to 1
        "strain_energy_function_vol": (double),                               //defaults to 2

	    //energy
	    "heat_capacity": (double),                                            //defaults to 0
        "thermal_conductivity": [(double), (double), (double),                //array is size 9; defaults to all 0
                                 (double), (double), (double),
                                 (double), (double), (double)],
        "fraction_of_heat_from_mechanical": (double)                          //defaults is 0.8
	}
```

sample material_mat.json file: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/blob/aaron_input/Run/material_mat.json