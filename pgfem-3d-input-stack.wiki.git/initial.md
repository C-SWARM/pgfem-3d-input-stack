### _initial.json ###

These files (named physics_name_initial.json) follow the following format:
```c++
{
    // mid point rule: alpha
    // 0: backward Euler 
    // 0.5: second order implicit
    // 1: forward Euler
    "time_integration_rule": (double),  //only used for momentum equation; defaults to 0.5

    //If <10^-16, quasi-static. Otherwise, transient
    "inertial_density": [list of comma-separated doubles],    //one double per material

    "reference value": (double)         //defaults to 0 for momentum equation, and 300 for energy equation

    //The following parameters are optional
	"g_id": [
	           //momentum g_id: [geom_type, t3dID, initial_displacement_x, initial_displacement_y, initial_displacement_z, initial_velocity_x, initial_velocity_y, initial_velocity_z]
               //  geometry type: 1=vertex, 2=curve, 3=surface, 4=region, 5=patch, 7=interface
               //  the geometry type can be a "string" or an int
               [list of comma-separated integers (first can optionally be a "string"],   //8 elements per array for momentum, 2 elements per array for energy
               ...
             ],

    //"replacements" is used to replace the negative values in "g_id"
    //the 1st element in "replacements" replaces each -1 in "g_id", the 2nd element replaces -2 in "g_id", etc…
	"replacements": [list of comma-separated doubles]
}
```
sample _initial.json file: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/blob/aaron_input/Run/mechanical_initial.json