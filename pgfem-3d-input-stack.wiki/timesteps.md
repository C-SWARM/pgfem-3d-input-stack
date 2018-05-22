### timesteps.json ###

This file follows the following format:
```c++
{ //“time_steps" and "list_of_steps_to_print" can either be an array, or a string following the looping syntax 
  "number_of_time_steps": (int),        //defaults to 0
  //has "number_of_time_steps” number of elements
  "time_steps": either [list of doubles, separated by commas] or "looping syntax string"

  "number_of_steps_to_print": (int),    //defaults to 0
  //has "number_of_steps_to_print" number of elements
  "list_of_steps_to_print": either [list of integers, separated by commas] or "looping syntax string"
}
```
sample timesteps.json file: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/blob/aaron_input/Run/timesteps.json