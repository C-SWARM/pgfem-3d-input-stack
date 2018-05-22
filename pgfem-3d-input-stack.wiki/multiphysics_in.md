### multiphysics_in.json ###

This file follows the following format:
```c++
{
"number_of_physics": (int),   //defaults to 1
"physics_list": [             //array containing each physics
   (list of physics, separated by commas)
]
}
```
Each `"physcis_list"` element has the following format:
```c++
{
    "equation_id": (int),          //0 for momentum equation, 1 for energy equation, defaults to -1
    "physics_name": "(string)",    //defualts to "Name_not_set"
    "degree_of_freedom": (int),    //defaults to 3 for momentum equation, and 1 for energy equation
    "number_of_coupled_physics": (int),              //defaults to size of "coupled_physics_ids"
    "coupled_physics_ids": [(int1),(int2),etc...],   //list of integers

    "number_of_vars_to_print": (int),                //defaults to size of "vars_to_print"
    "vars_to_print": [(int1),(int2),etc...]          //list of integers
}
```
The meaning of the values in the `"vars_to_print"` array vary based on their equation_id:
```
Momentum equation:
0: Displacement
1: Macro Displacement for multi-scale simulation
2: Pressure for mixed method
3: Cauchy Stress
4: Euler Strain
5: Effective Strain
6: Effective Stress
7: Cell Property
8: Damage, if plasticity then hardening
9: Chi for damage, if plasticity stretch of plasticity strain
10: F
11: P (PK1)
12: W: strain energy
13: TF_Pressure: pressure for three field mixed method 
14: TF_Volume: volume for three field mixed method
15: Density
16: hydrostatic stress tr(sigma)/3

Energy equation:
0: temperature
1: heat flux
2: Heat generations
```
sample multiphysics.in file: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/blob/aaron_input/Run/multiphysics.json