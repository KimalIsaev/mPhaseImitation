#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "avl_tree.h"
#include "frm.h"
#include "read_double_pair_from_file.h"
//indexes for recognition in avl tree
const char I_FLOW = 0;
const char I_ORBIT = 1;
const char I_DEVICE = 2;
const char N_VARIABLES = 8;

unsigned int N_DEVICE = 2;
double X_FLOW = 0.8;
double X_ORBIT = 0.2;
double Q_Q = 0.25;
double X_PHASE_1 = 0.6;
double X_PHASE_2 = 1.5;
double Q_R0 = 0.7;
double Q_R1 = 0.2;
double Q_R2;
char* RNG_FILE = "big_test.txt";

unsigned int m_step = 0;
int number_of_request_in_devices = 0;
int number_of_request_in_orbit = 0;
unsigned int k_step;
double* orbit_times;
struct double_pair* double_pairs; //definition in read_double_pair_from_file

struct indexed_time{
	unsigned char index;
	double time;
	struct avl_tree_node node;
};
#define INDEXED_TIME(__node) avl_tree_entry(__node, struct indexed_time, node) 
#define TIME_VALUE(node) INDEXED_TIME(node)->time

static struct avl_tree_node *tree = NULL;

int cmp_func(const struct avl_tree_node *node1, 
		const struct avl_tree_node *node2){
	return (TIME_VALUE(node1) > TIME_VALUE(node2)) ? 1: -1;
}

void wait_new(char index, double time){
	struct indexed_time* to_insert = 
		(struct indexed_time *)malloc(sizeof(struct indexed_time));
	to_insert->index = index;
	to_insert->time = time;
	//printf("%f\n", time);
	assert(NULL == avl_tree_insert(&tree, &to_insert->node, cmp_func));
}

void wait_new_from_flow(double a){
	wait_new(I_FLOW, exp_dist(a, X_FLOW));
}

void wait_new_from_orbit(double a){
	wait_new(I_ORBIT, exp_dist(a, X_ORBIT));
}

void wait_new_from_device(double a, double q){
	wait_new(I_DEVICE, hyperexp_dist(a, q, n, V_Q, V_X));
}

void wait_sampled_flow(){
	wait_new_from_flow(double_pairs[k_step].first);
}

void wait_sampled_orbit(){
	wait_new_from_orbit(double_pairs[k_step].first);
}

void wait_sampled_device(){
	wait_new_from_device(double_pairs[k_step].first,
			double_pairs[k_step].second);
}

void add_first_from_flow(){
	wait_new_from_flow(double_pairs[0].first); 
}

void set_up(unsigned int n){
	orbit_times = malloc(sizeof(double)*n);
	for (int i=0; i < n; orbit_times[i++] = 0);
	add_first_from_flow();
}

void free_avl_tree(struct avl_tree_node** tree){
	struct indexed_time* to_delete;
	struct avl_tree_node* current = 
		avl_tree_first_in_order(*tree);

    while(current) {
        to_delete = INDEXED_TIME(current);
        current = avl_tree_next_in_order(current);
        avl_tree_remove(tree, &to_delete->node);
        free(to_delete);
    }
	*tree = NULL;
}

void clean_up(){
	free_avl_tree(&tree);
	free(orbit_times);
	free(double_pairs);
}

void decrease_time_from(struct avl_tree_node* current, double min){
	struct indexed_time* to_decrease_time;
	while(current){
		to_decrease_time = INDEXED_TIME(current);
		to_decrease_time->time -= min;
		current = avl_tree_next_in_order(current);
	}
}

void min_change(char* index_of_deleted, double* min){
	struct avl_tree_node* current = 
		avl_tree_first_in_order(tree);
 	struct indexed_time* to_delete = INDEXED_TIME(current); 
	*min = to_delete->time; //set returned values
	*index_of_deleted = to_delete->index; //set returned values
	decrease_time_from(avl_tree_next_in_order(current), *min); 
	avl_tree_remove(&tree, &to_delete->node);
	free(to_delete);
}

void new_request_in_orbit(){
	number_of_request_in_orbit++;
	wait_sampled_orbit();
}

void new_request_in_devices(){
	number_of_request_in_devices++;
	wait_sampled_device();
}

void new_request_to_execute(){
	char is_devices_full = (N_DEVICE == number_of_request_in_devices);
	if (is_devices_full) {
		new_request_in_orbit();
	} else {
		new_request_in_devices();
	}
}
void new_request_executed(){
	double uni = double_pairs[k_step].first; 
	if (uni < Q_R2) new_request_in_orbit();
	if (uni > (1 - Q_R1)) new_request_in_devices();
}

double step(){
	char index_of_worked_stream;
	double time_of_step;
	min_change(&index_of_worked_stream, &time_of_step);
	switch (index_of_worked_stream) {
		case I_FLOW:
			m_step++;
			wait_sampled_flow();
			new_request_to_execute();
			break;
		case I_ORBIT:
			number_of_request_in_orbit--;
			new_request_to_execute();
			break;
		case I_DEVICE:
			number_of_request_in_devices--;
			new_request_executed();
			break;
	}
	return time_of_step;
}

void set_variables(int argc, char *argv[]){
	if (argc == 1){
		Q_R2 = 1 - Q_R0 - Q_R1;
		return;
	}
	if (argc == (N_VARIABLES + 2)){
		N_DEVICE = strtoul(argv[1], NULL, 10); 
		X_FLOW = strtod(argv[2], NULL); 
		X_ORBIT = strtod(argv[3], NULL); 
		Q_Q = strtod(argv[4], NULL); 
		X_PHASE_1 = strtod(argv[5], NULL); 
		X_PHASE_2 = strtod(argv[6], NULL); 
		Q_R0 = strtod(argv[7], NULL); 
		Q_R1 = strtod(argv[8], NULL); 
		Q_R2 = 1 - Q_R0 - Q_R1;
		RNG_FILE = argv[9];
		return;
	} 
	assert(0);
}

void printf_orbit(unsigned int n) {
	for(int i = 0; i < n; printf("%g\n", orbit_times[i++]));
}

void printf_orbit_inline(unsigned int n) {
	for(int i = 0; i < n; printf("%g ", orbit_times[i++]));
	printf("\n");
}

void printf_tree() {
	struct indexed_time* node;
	struct avl_tree_node* current = avl_tree_first_in_order(tree);
	
    while(current) {
        node = INDEXED_TIME(current);
        printf("%d:%g ", node->index, node->time);
        current = avl_tree_next_in_order(current);
    }
	printf("\n");
}

void printf_variables(){
	printf("%d\n", N_DEVICE);
	printf("%g\n", X_FLOW); 
	printf("%g\n", X_ORBIT); 
	printf("%g\n", Q_Q); 
	printf("%g\n", X_PHASE_1); 
	printf("%g\n", X_PHASE_2); 
	printf("%g\n", Q_R0); 
	printf("%g\n", Q_R1);
}

int main(int argc, char *argv[]){
	set_variables(argc, argv);
	unsigned int n_step = file_to_array(RNG_FILE, &double_pairs);
	set_up(n_step);
	int old_number_requst_in_orbit;
	//for(k_step = 1; k_step < M_STEP; k_step++) step();
	for(k_step = 1; k_step < n_step; k_step++){
		old_number_requst_in_orbit = number_of_request_in_orbit;
		orbit_times[old_number_requst_in_orbit] += step();
		//printf("%d\n", k_step);
		//printf_tree();
		
		//printf_cvector(&orbit_times);
	}
	printf_orbit(n_step);
	//printf_variables();
	return 0;
}

