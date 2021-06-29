#ifndef IMITATION_FORMULAS
#define IMITATION_FORMULAS
#include "math.h"

double exp_dist(double a, double l);
double hyperexp_dist(double a, double q, unsigned int n, 
		double* v_q, double* v_l );
void get_consecative_sums(double* original, unsigned int n, double* sums);

#endif /* IMITATION_FORMULAS */
