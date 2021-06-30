#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "avl_tree.h"
#include "distribution_functions.h"
#include "read_double_pair_from_file.h"
//indexes for recognition in avl tree
const char I_FLOW = 0;
const char I_ORBIT = 1;
const char I_DEVICE = 2;
const char N_NON_EXECUTION_VARIABLES = 7;

int N_PHASE;
unsigned int N_DEVICE = 2;
double X_FLOW = 0.8;
double X_ORBIT = 0.2;
double Q_R0 = 0.7;
double Q_R1 = 0.2;
double Q_R2;
char* RNG_FILE = "big_test.txt";

double* Q_EXECUTION = NULL;
double* X_EXECUTION = NULL;

unsigned int count_flow_request = 0;
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
#define avl_tree_node_to_indexed_time(__node) \
	avl_tree_entry(__node, struct indexed_time, node) 
#define avl_tree_node_to_time(node) avl_tree_node_to_indexed_time(node)->time

static struct avl_tree_node *tree = NULL;

int cmp_func(const struct avl_tree_node *node1, 
		const struct avl_tree_node *node2){
	return (avl_tree_node_to_time(node1) > avl_tree_node_to_time(node2))?1:-1;
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
	wait_new(I_DEVICE, hyperexp_dist(a, q, N_PHASE, 
		Q_EXECUTION, X_EXECUTION));
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
	get_consecative_sums(Q_EXECUTION, N_PHASE, Q_EXECUTION); 
	add_first_from_flow();
}

void free_avl_tree(struct avl_tree_node** tree){
	struct indexed_time* to_delete;
	struct avl_tree_node* current = 
		avl_tree_first_in_order(*tree);

	while(current) {
		to_delete = avl_tree_node_to_indexed_time(current);
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
	free(X_EXECUTION);
	free(Q_EXECUTION);
}

void decrease_time_from(struct avl_tree_node* current, double min){
	struct indexed_time* to_decrease_time;
	while(current){
		to_decrease_time = avl_tree_node_to_indexed_time(current);
		to_decrease_time->time -= min;
		current = avl_tree_next_in_order(current);
	}
}

void min_change(char* index_of_deleted, double* min){
	struct avl_tree_node* current = 
		avl_tree_first_in_order(tree);
 	struct indexed_time* to_delete = avl_tree_node_to_indexed_time(current); 
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
			count_flow_request++;
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
	int n_execution_variables = argc - N_NON_EXECUTION_VARIABLES;
	if ((n_execution_varianles > 0) && !(n_execution_variables % 2)){
		N_DEVICE = strtoul(argv[1], NULL, 10); 
		X_FLOW = strtod(argv[2], NULL); 
		X_ORBIT = strtod(argv[3], NULL); 
		Q_R0 = strtod(argv[4], NULL); 
		Q_R1 = strtod(argv[5], NULL); 
		Q_R2 = 1 - Q_R0 - Q_R1;
		RNG_FILE = argv[6];
		N_PHASE = n_execution_variables / 2;
		Q_EXECUTION = malloc(sizeof(double)*N_PHASE);
		X_EXECUTION = malloc(sizeof(double)*N_PHASE);
		for(int i = 0; i < N_PHASE; i++)
			Q_EXECUTION[i] = 
				strtod(argv[i + argc]i, NULL);
		for(int i = 0; i < N_PHASE; i++)
			X_EXECUTION[i] = 
				strtod(argv[i + argc + N_PHASE], NULL);
	} else { 
		assert(0);
	}
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
        node = avl_tree_node_to_indexed_time(current);
        printf("%d:%g ", node->index, node->time);
        current = avl_tree_next_in_order(current);
    }
	printf("\n");
}

void printf_variables(){
	printf("%d\n", N_DEVICE);
	printf("%g\n", X_FLOW); 
	printf("%g\n", X_ORBIT); 
	printf("%g\n", Q_R0); 
	printf("%g\n", Q_R1);
}

int main(int argc, char *argv[]){
	set_variables(argc, argv);
	unsigned int n_step = filename_to_double_pair_array(RNG_FILE, &double_pairs);
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

