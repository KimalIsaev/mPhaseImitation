#include "read_float_pair_from_file.h"

#define NUM_128BIT(high,low) ((((__uint128_t)high) << 64) + low)

static struct float_pair new_float_pair(__uint128_t r)
{
	struct float_pair ff;
    const unsigned int FOR_FLOAT = 0x3F800000;
    union {
        unsigned int u;
        float d;
    } conv;
    conv.u = (r >> 105u) | FOR_FLOAT;
    ff.flow = conv.d - 1;
    conv.u = ((r << 23u) >> 105u) | FOR_FLOAT;
    ff.orbit = conv.d - 1;
    conv.u = ((r << 46u) >> 105u) | FOR_FLOAT;
    ff.phase = conv.d - 1;
    conv.u = ((r << 69u) >> 105u) | FOR_FLOAT;
    ff.q = conv.d - 1;
    conv.u = ((r << 92u) >> 105u) | FOR_FLOAT;
    ff.r = conv.d - 1;
	return ff;
}

unsigned int file_to_array(char* filename, struct float_pair** array){
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
	//count--;//last line is empty
	*array = malloc(sizeof(struct float_pair)*count);
	for (long long i = 0; i < count; i++){
		read = getline(&line, &len, fp);
		high = strtoull(line, &space, 16);
		low = strtoull(++space, NULL,  16);
		(*array)[i] = new_float_pair(NUM_128BIT(high, low));
	}

	free(line);
	fclose(fp);
	return count;
}
