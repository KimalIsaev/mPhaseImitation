#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

struct double_pair{
	double first;
	double second;
};

unsigned int file_to_array(char* filename, struct double_pair** array);
