#include "frm.h"

double exp_dist(double a, double l){
	return -log(1-a)/l; 
}

double hyperexp2(double a, double p, double q, double l1, double l2){
	return exp_dist(a, (p < q) ? l1 : l2);
}

