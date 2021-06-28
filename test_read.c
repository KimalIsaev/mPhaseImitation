#include "read_float_pair_from_file.h"

int main(){
	struct float_pair* ffs;
	printf("no defined N \n");
	unsigned int N = file_to_array("big_test.txt", &ffs);
	printf("N is %d\n", N);
	if (!N) {
		printf("no such file\n");
		return 0;
	}
	for(long long i = 0; i < N; i++)
		printf("%f %f\n", ffs[i].first, ffs[i].second);
	return 0;
}
