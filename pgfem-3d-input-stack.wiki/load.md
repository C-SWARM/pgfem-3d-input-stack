### _load.json ###

These files (named physics_name_load.json) follow the following format:
```c++
{ //"loading_time_steps" and "loading_increments" can either be an array, or a string following the looping syntax
  "number_of_loading_steps": (int),        //defaults to 0

  //The next 2 parameters don’t need to be listed if "number_of_loading_steps" is 0
  //has "number_of_loading_steps" number of elements
  "loading_time_steps": either [list of integers, separated by commas] or "looping syntax string"

  //each loading_increments list has "number_of_loading_steps" number of elements
  //supports multiple loads
  "loading_increments": [
    [list of doubles, separated by commas] or "looping syntax string",  //1st load
    ...                                                                 //Other loads
  ]
}
```
sample _load.json file: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/blob/aaron_input/Run/mechanical_load.json
