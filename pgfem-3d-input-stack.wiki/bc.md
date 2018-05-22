### _bc.json ###

These files (named physics_name_bc.json) follow the following format:
```c++
{   //This file is optional
    //bc data format: [geom_type, geom_id, flag(u), flag(v), flag(w)]
    //  geometry type: 1=vertex, 2=curve, 3=surface, 4=region, 5=patch, 7=interface
    //  the geometry type can be a "string" or an int
	"bc_data": [
               [list of comma-separated integers (first can optionally be a "string"],
               ...
             ],

    //"replacements" represents displacements and is used to replace the negative values in "bc_data"
    //It will be stored in the .bcv file
    //the 1st element in "replacements" replaces each -1 in "bc_data", the 2nd element replaces -2 in "bc_data", etc…
	"replacements": [list of comma-separated doubles]
}
```
sample _bc.json file: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/blob/aaron_input/Run/mechanical_bc.json