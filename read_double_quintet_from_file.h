#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

struct double_quintet{
	double flow;
	double orbit;
    double execution;
    double phase;
    double after_execution;
};

unsigned int filename_to_double_quintet_array(char* filename,
		struct double_quintet** array);

