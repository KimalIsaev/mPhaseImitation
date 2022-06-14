#include "read_double_quintet_from_file.h"
/*
#include <stdlib.h>
#include <stdio.h>

void printf_ull(unsigned long long ull){
	printf("%x %x\n", (unsigned int) (ull >> 32), (unsigned int) ull);
}
*/
const char SEVEN_BYTE_SIZE = 7;

unsigned int fsize(FILE *fp){
	unsigned int prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	unsigned int sz = ftell(fp);
	fseek(fp, prev, SEEK_SET);
	return sz;
}

static double random_ull_to_random_double(unsigned long long ull)
{
	const unsigned long long DOUBLE_ONES = 0xFFF0000000000000;
	const unsigned long long DOUBLE_ZEROS = 0x3FFFFFFFFFFFFFFF;
	union {
		unsigned long long integer;
		double rational;
	} transition;
	transition.integer = DOUBLE_ZEROS & (DOUBLE_ONES | ull);
	return transition.rational - 1;
}

unsigned int filename_to_double_quintet_array(char* filename,
		struct double_quintet** array){
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL) return 0;
	unsigned int file_size = fsize(fp);
	//printf("%d\n", file_size);
	unsigned int number_of_double_quintets = file_size / (5*SEVEN_BYTE_SIZE); 
	*array = malloc(sizeof(struct double_quintet)*number_of_double_quintets);
	unsigned long long buffer = 0;
	for(unsigned int i = 0; i < number_of_double_quintets; i++){
		fread(&buffer, SEVEN_BYTE_SIZE, 1, fp);
		//printf_ull(buffer);
		(*array)[i].flow = random_ull_to_random_double(buffer);
		fread(&buffer, SEVEN_BYTE_SIZE, 1, fp);
		//printf_ull(buffer);
		(*array)[i].orbit = random_ull_to_random_double(buffer);
		fread(&buffer, SEVEN_BYTE_SIZE, 1, fp);
		//printf_ull(buffer);
		(*array)[i].execution = random_ull_to_random_double(buffer);
		fread(&buffer, SEVEN_BYTE_SIZE, 1, fp);
		//printf_ull(buffer);
		(*array)[i].phase = random_ull_to_random_double(buffer);
		fread(&buffer, SEVEN_BYTE_SIZE, 1, fp);
		//printf_ull(buffer);
		(*array)[i].after_execution = random_ull_to_random_double(buffer);
	}   
	
	fclose(fp);
	return number_of_double_quintets;
}
/*
int main(){
	char filename[] = "test.txt";
	struct double_quintet* double_pairs;
	file_to_array(filename, &double_pairs);
	return 0;
}
*/

