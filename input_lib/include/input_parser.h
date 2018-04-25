#pragma once

/* AUTHOR:
 * Aaron Howell
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"
#include <string>
#include <iostream>
#include <vector>
#include <cassert>

typedef struct{
  char *token;

  double start = 0;
  double end = 0;
  double increment = 1;
} input_parser;



//Iterates through the input string to see if the current token is singular
//A token is singular if there's no ":" before the end of the token
int singular_token(const char *input);




//Using input.start, input.end, and index input.increment, 
//computes the steps and stores the result in destination vector
//The current destination index is returned by reference and is retained between repeated function calls
template <class T>
void compute_values(const input_parser input, std::vector<T> &destination, int &current_index)
{
  if (input.start <= input.end)              //increasing loops
    assert(input.increment > 0 && "looping syntax: invalid loop increment");    
  else                                       //decreasing loops
    assert(input.increment < 0 && "looping syntax: invalid loop increment");    

  double DBL_stepno = ((input.end - input.start)/input.increment) + 1;
  long stepno = (long) DBL_stepno;

  current_index += stepno;
  
  for(long ia=0; ia<stepno; ia++)
    destination.push_back(input.start + ia*input.increment);

  //this solution seems to work for handling floating point errors
  if (stepno < DBL_stepno){ //if a floating point error resulted in the stepno being truncated...
    destination.push_back(input.end);
    ++current_index;
  }
}

  /*
    This function parses a string representing a list of values in a 
    simple looping syntax and saves it to a list of values. 
    The string is first divided between "," into smaller string tokens
    The tokens are then divided between ":" into start, end, and iteration values
    The function compute_values is then called to loop through and store 
    the values into the passed in list of values

    input:
      char *buffer: a string in the following format: start1:end1:increment1,start2:end2:increment2,...
      (spaces between elements are allowed)
      buffer will be modified in this function.

      Individual numbers can also be listed, and combined with the loop syntax. eg: 1,3,7, 10:20:1
      dest_size: the size of the destination array

    output:
      vector<Template T> destination: the location where the parsed values are to be saved
  */
template <class T>
void parse_input(char *buffer, std::vector<T> &destination, const int dest_size, const int string_size)
{
  char comma[2] = ",";
  char colon[2] = ":";
  
  input_parser parser;

  int current_index = 0;

  //strtock can't normally be nested, so this pointer is needed to 
  //interate through buffers with multiple tokens
  char *next_token_start = (char*)memchr(buffer, ',', string_size);    //next_token_start points to the first ","
  if (next_token_start != NULL) next_token_start = &(next_token_start[1]);     //move next_token_start to char after ","

  //parsing syntax found at: https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
  parser.token = strtok(buffer, comma);

  while (parser.token != NULL)          //checks if token is blank
  {
    
    if (singular_token(parser.token))   //singular number token. eg: 1,2,3,5
      {
	sscanf(strtok(parser.token, colon), "%lf", &(parser.start)); 
	parser.end = parser.start;
	parser.increment = 1;           //arbitrary (but needs to be positive to pass assert)
      }
    else                                //contains ":" separated sub-tokens. eg: 1:10:2,20:30:1
      {
	sscanf(strtok(parser.token, colon), "%lf", &(parser.start)); 
	sscanf(strtok(NULL, colon), "%lf", &(parser.end));
	sscanf(strtok(NULL, colon), "%lf", &(parser.increment));
      }

    compute_values (parser, destination, current_index);
    
    if (next_token_start == NULL) break;                      //no more tokens to parse

    buffer = next_token_start;                                //set buffer to the start of the next token

    next_token_start = (char*)memchr(buffer, ',', string_size);  //next_token_start points to the next ","
    if (next_token_start != NULL) next_token_start = &(next_token_start[1]);   //move next_token_start to char after ","

    parser.token = strtok(buffer, comma);                 //set next token
  }


  //****finished parsing****
  if (current_index < dest_size) {   //check if destination size is incorrect
    std::cerr << "Error: Destination size is larger than number of values\n";
    exit (EXIT_FAILURE);
  }
  else if (current_index > dest_size) {
    std::cerr << "Error: There are more values than are able to fit in the destination list\n";
    exit (EXIT_FAILURE);
  }
}

