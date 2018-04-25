# T3D2PSIFEL #
This is a tool to decompose T3d meshes as part of the PGFem3D pre-processing pipeline.

## INSTALLATION ###
1. build libraries in `./contrib`.
2. run `make` in `./src`

### Building ./contrib/Ckit ###
    $ cd ./contrib/Ckit/src
    $ ./configure --x-include=/usr/include --x-lib=/usr/lib64
    $ make CC=gcc CFLAGS='-O2 -g'

### Building ./contrib/Elixir ###
    $ cd ./contrib/Elixir/src
    $ ./configure --with-x CC=gcc CFLAGS='-O2 -g'
    $ make

### Contributers ###
* Daniel Rypl, Ph.D.
* Matt Mosby, University of Notre Dame, <mmosby1@nd.edu>
* Kamal K Saha, University of Notre Dame, <ksaha@nd.edu>
  * (README and build instructions)
