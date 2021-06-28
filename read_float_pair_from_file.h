#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

struct float_pair{
	float first;
	float second;
};

unsigned int file_to_array(char* filename, struct float_pair** array);

