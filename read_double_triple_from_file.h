#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

struct double_triple{
	double first;
	double second;
    double third;
};

unsigned int filename_to_double_pair_array(char* filename,
		struct double_triple** array);

