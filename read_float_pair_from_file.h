#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

struct five_float{
	float flow;
	float orbit;
	float q;
	float phase;
	float r;
};

unsigned int file_to_array(char* filename, struct five_float** array);

