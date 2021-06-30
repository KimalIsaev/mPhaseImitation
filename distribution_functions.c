#include "distribution_functions.h"

double exp_dist(double a, double l){
	return -log(1-a)/l; 
}

double hyperexp_dist(double a, double p, unsigned int n,
		double* v_q, double* v_l){
	unsigned int i;
	for(i=0; i < n - 1; i++)
		if (v_q[i] > p)
			break;
	return exp_dist(a, v_l[i]);

}

void get_consecative_sums(double* original, unsigned int n, double* sums){
	double s = 0;
	for(unsigned int i = 0; i < n; i++){
		s += original[i];
		sums[i] = s;
	}	
}

