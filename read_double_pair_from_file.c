#include "read_double_pair_from_file.h"

static double random_ull_to_random_double(unsigned long long ull)
{
	const unsigned long long DOUBLE_ONES = 0xFFF0000000000000;
	const unsigned long long DOUBLE_ZEROS = 0x3FFFFFFFFFFFFFFF;
	union {
		unsigned long long integer;
		double rational;
	} transition;
	transition.integer = ull;
	return DOUBLE_ZEROS & (DOUBLE_ONES | transition.double);
}



unsigned int file_to_array(char* filename, struct double_pair** array){
	FILE *fp;
	char *line = NULL;
	char *space = NULL;
	size_t len = 0;
	ssize_t read;
	unsigned long long high, low;
	unsigned int count = 0;
	fp = fopen(filename, "r");
	if (fp == NULL) return 0;
	while ((read = getline(&line, &len, fp)) > 0) count++;
	fclose(fp);

	fp = fopen(filename, "r");
	*array = malloc(sizeof(struct double_pair)*count);
	for (long long i = 0; i < count; i++){
		read = getline(&line, &len, fp);
		high = strtoull(line, &space, 16);
		low = strtoull(++space, NULL,  16);
		(*array)[i].first = random_ull_to_random_double(high);
		(*array)[i].second = random_ull_to_random_double(low);
	}

	free(line);
	fclose(fp);
	return count;
}
