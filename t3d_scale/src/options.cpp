#include <iostream>
#include <cstring>
#include <cstdlib>

#include "options.h"

Options::Options()
{
  num_proc = 1;
  dx = dy = dz = 0;
  BC = false;
}

Options::Options(int argc, char **argv)
{
  num_proc = 1;
  dx = dy = dz = 0;
  BC = false;

  for(int i=1;i<argc;i++){
    if(strcmp(argv[i],"-np") == 0){
      num_proc = atoi(argv[i+1]);
      i++;
      continue;
    } else if(strcmp(argv[i],"-f") == 0){
      Filebase = argv[i+1];
      input = output = Filebase; // This is to aid in obsoleting old format
      i++;
      continue;
      /* 
    } else if(strcmp(argv[i],"-i") == 0){
      input = argv[i+1];
      i++;
      continue;
    } else if(strcmp(argv[i],"-o") == 0){
      output = argv[i+1];
      i++;
      continue;
      */
    } else if(strcmp(argv[i],"-x") == 0){
      dx = atof(argv[i+1]);
      i++;
      continue;
    } else if(strcmp(argv[i],"-y") == 0){
      dy = atof(argv[i+1]);
      i++;
      continue;
    } else if(strcmp(argv[i],"-z") == 0){
      dz = atof(argv[i+1]);
      i++;
      continue;
    } else if(strcmp(argv[i],"-xyz") == 0){
      dx = atof(argv[i+1]);
      dy = atof(argv[i+2]);
      dz = atof(argv[i+3]);
      i += 3;
      continue;
    } else if (strcmp(argv[i],"-bc") == 0){
      BC = true;
    } else {
      std::cout << "Ignoring unrecognized option '" << argv[i] << "'.\n";
      continue;
    } // Switch

  } // for i<argv
}

void Options::print()
{
  std::cout << "Creating new computational domains with the following options:\n"
	    << "Number of domains:\t" << num_proc << std::endl
	    << "File base:        \t" << Filebase << std::endl
    // << "Input filename:   \t" << input << std::endl
    // << "Output filename:  \t" << output << std::endl
	    << "Increment in x:   \t" << dx << std::endl
	    << "Increment in y:   \t" << dy << std::endl
	    << "Increment in z:   \t" << dz << std::endl;
  if (this->bc()) {
    std::cout << "Using BC file to generate boundary conditions." << std::endl;
  }
}

void Options::useage()
{
  std::cout << "Useage: T3dScale [options]\n\n"
	    << "REQUIRED OPTIONS\n"
	    << "   -f <filebase> : Specify the base filename to use for read/write\n"
    // << "   -i <filename> : Specify t3d mesh file to use\n"
    // << "   -o <filename> : Specify base filename for output\n"
	    << "\nOTHER OPTIONS\n"
	    << "   -np <integer> : Specify the number of domains to create\n"
	    << "                   (Default is 1)\n"
	    << "   -x, -y, -z <float> : Specify translations in each direction\n"
	    << "                        (Default is 0 in each direction)\n"
	    << "   -xyz <3 #> : Specify the translations with 1 command\n"
	    << "   -bc      : use <filebase>.bc to generate BC"
	    << std::endl;
}
