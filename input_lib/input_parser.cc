/* AUTHOR:
 * Aaron Howell
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"
#include "input_parser.h"
#include <iostream>
#include <vector>



//Iterates through the input string to see if the current token is singular
//A token is singular if there's no ":" before the end of the token
int singular_token(const char *input)
{
  int iter = 0;
  
  while (input[iter] != '\0')   //reads until end of string
    {
      if (input[iter] == ':')
	return 0;

      iter++;
    }
  
  return 1;
}
