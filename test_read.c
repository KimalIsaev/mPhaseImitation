#include "read_rng_file.h"

int main(){
	struct five_float* ffs;
	printf("no N \n");
	unsigned int N = file_to_array("big_test.txt", &ffs);
	printf("N is %d\n", N);
	if (!N) {
		printf("no such file\n");
		return 0;
	}
	for(long long i = 0; i < N; i++)
		printf("%f %f %f %f %f\n", 
				ffs[i].flow, ffs[i].orbit, ffs[i].phase, ffs[i].q, ffs[i].r);
	return 0;
}
