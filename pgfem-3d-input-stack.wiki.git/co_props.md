### co_props.json ###

This file is used for the macro portion of multiscale. 
It follows the following format:
```c++
{
    "number_of_cohesive_potentials": (int),

    "cohesive_potentials": [
	{   //one object per number_of_cohesive_potentials
	    "type_of_potential": (int),               //-1 for multiscale
	    "phenomenological_potentials": [(double1), (double2), etc...] //not used for multiscale
	},
     ...
    ]
}