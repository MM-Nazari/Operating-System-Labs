#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMBER_OF_RESOURCES 5
#define NUMBER_OF_CUSTOMERS 5

	sem_t mutex;	//lock

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

bool need_lt_work(int ith_need[], int work[]);
bool is_safe();
int request_resources(int customer_num, int source_nums[]);
int release_resources(int customer_num, int source_nums[]);
void *customer_thread(int customer_num);
void print_state();
char *to_str(int source_nums[]);

int main(int argc, char const *argv[]){

 //available
	if (argc < NUMBER_OF_RESOURCES + 1) {
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
	available[i] = atoi(argv[i + 1]);
	}

	srand(time(NULL));
 //maximum and need and allocation
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++){
			maximum[i][j]=rand()%available[j] + 1;
			need[i][j] = maximum[i][j];
			allocation[i][j] = 0;
		}
	}

	print_state();
	sem_init(&mutex, 0, 1);

 //customers
	pthread_t customer_threads[NUMBER_OF_CUSTOMERS];
 //create customers
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		pthread_create(&customer_threads[i], NULL, (void *) customer_thread, (void *)(intptr_t) i);
	}
 //join customers
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		pthread_join(customer_threads[i], NULL);
	}
	return 0;
 }

 //customer
 void *customer_thread(int customer_num) {
	int source_nums[NUMBER_OF_RESOURCES];
	bool req_or_rel;
	int done;
 //requesting	
		for (int i = 0; i < 30 ; i++){	
			req_or_rel = rand() % 2;	
		//	request	
		if (req_or_rel == 0) {	
			for (int j = 0; j < NUMBER_OF_RESOURCES ; j++){	
				source_nums[j] = rand() % (need[customer_num][j] + 1);	
			}
		done = request_resources(customer_num, source_nums);
		printf("Customer %d Requests [%s\17)]	: %s\n ", customer_num + 1,	to_str(source_nums), done == 0
		?	"accepted"	:	"not accepted");	
		}
// release
	else {
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++){
			source_nums[j] = rand() % (allocation[customer_num][j] + 1);
		}
		done = release_resources(customer_num, source_nums);
		printf( "Customer %d Releases [%s\b)]\n ", customer_num + 1, to_str(source_nums));
	}
		}
 }


 int request_resources(int customer_num, int source_nums[]) {
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
	//(1 & 2)
	if (source_nums[i] > need[customer_num][i] || source_nums[i] > available[i])
	return -1;
	}
	sem_wait(&mutex);
	//(3)
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
		available[i] -= source_nums[i];
		allocation[customer_num][i] += source_nums[i];
		need[customer_num][i] -= source_nums[i];
	}
	if (is_safe()) {
		sem_post(&mutex);
		return 0;
	}
	//undo if not safe
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
		available[i] += source_nums[i];
		allocation[customer_num][i] -= source_nums[i];
		need[customer_num][i] += source_nums[i];
	}
	sem_post(&mutex);
	return -1;
 }

 int release_resources(int customer_num, int source_nums[]) {
	sem_wait(&mutex);
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
		available[i] += source_nums[i];
		allocation[customer_num][i] -= source_nums[i];
	}
	sem_post(&mutex);
 }

 bool is_safe() {
	int work[NUMBER_OF_RESOURCES];
	//(1)
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
		work[i] = available[i];
	}
	bool finish[NUMBER_OF_CUSTOMERS] = {0};
	int finished = 0;
	bool flag;
	while (finished != NUMBER_OF_CUSTOMERS) {
		flag = false;
		for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
			//(2)
			if (!finish[i] && need_lt_work(need[i], work)) {
				//(3)
				for (int j = 0 ; j < NUMBER_OF_RESOURCES; j++){
					work[j] += allocation[i][j];
				}
				finish[i] =	true;
				finished++;
				flag = true;
			}
		}
		if (!flag)
			return false;
	
	}
 //(4)
	return true;
 }

 //is the need of one P less than work (for each resource)
 bool need_lt_work(int ith_need[], int work[]) {
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
		if (ith_need[i] > work[i]) {
		return false;
		}
	}
	return true;
 }

 //string of requests
 char *to_str(int source_nums[]) {
	char *str = malloc(103);
	char buf[NUMBER_OF_RESOURCES]= {0};
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
		sprintf(buf, "%d" , source_nums[i]);
		strcat(str, buf);
		strcat(str, " ");
	}
	return str;
}

 //print state
 void print_state() {
	printf("Available:\n");
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
		printf("%d ", available[i]);
	}
	printf("\n");

	printf("Maximum:\n");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++){
			printf("%d ", maximum[i][j]);
		}
		printf("\n");
	}

	printf("Allocation:\n");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++){
			printf("%d ", allocation[i][j]);
		}
		printf("\n");
	}

	printf("Need:\n");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++){
			printf("%d ", need[i][j]);
		}
		printf("\n");
	}
	printf( "************************\n");
}