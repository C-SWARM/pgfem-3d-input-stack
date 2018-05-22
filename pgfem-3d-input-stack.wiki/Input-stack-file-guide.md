### JSON Files ###
JSON (JavaScript Object Notation) is a standardized human-readable file format that is language independent. Data can be listed as an unordered collection of key-value pairs contained in an object (which use { }), or in an ordered list of values contained in an array (which use [ ]). Listed elements are separated by commas. More information can be found at the official JSON website: http://www.json.org/

### Duplicate Tags and Comments ###
While it varies based on JSON implementation, this input stack’s JSON reader (https://github.com/nlohmann/json#json-as-first-class-data-type) accepts duplicate tags in the same object, but they will overwrite the previous duplicate’s value. To avoid any issues with overwriting, only the `"#"` tag should be used to denote a comment in these input files.

For example: `"#": "This is a comment"`

The file descriptions in this wiki use "//" as a comment, but these aren’t used in actual JSON files. Actual example JSON files for the input stack can be found here: https://gitlab-cswarm.crc.nd.edu/pgfem_3d/pgfem3d_input/tree/develop/Run

### Verifying JSON Files for Correct Formatting ###
Because JSON is a standard format, files can be checked if they are in the correct format using online tools such as https://jsonlint.com/. However, some of these online tools may give errors if duplicate tags (such as "#" comments) are used, even though the input stack’s JSON reader accepts duplicate tags.

Sample JSON files that demonstrates formatting can be found here: [Example1](/Simple json example) [Example2](/Large json example)


### Optional Looping Syntax ###
To reduce the length of the load and timestep files, some parameters in these files allow for an optional looping syntax. 

This syntax is a string in the following format: `"element, element, element, ..."`, where each `element` can either be a single value or a series of 3 colon-separated values representing a for-loop using the following format: `"starting_value : ending_value : iteration_amount"` (spaces between tokens are ignored).

For example, the string `"1:10:2, 15, 18, 20:25:1, 15"` is equivalent to the array: `[1, 3, 5, 7, 9, 15, 18, 20, 21, 22, 23, 24, 25, 15]`.
The parameters where this syntax is allowed is specified on the file's wiki page.

### Input File Description ###

[multiphysics_in.json](/Multiphysics_in)

[material_mat.json](/Material_mat)

[numerical_param.json](/Numerical_param)

[timesteps.json](/Timesteps)

### Physics-specific Input File Description (Up to 1 File per Physics) ###

[_load.json](/load)

[_initial.json](/initial)

[_bc.json](/bc)

### Multiscale-specific Input File Description ###

[co_props.json](/co_props)

[normal_in.json](/normal_in)

[periodic.json](/periodic)
