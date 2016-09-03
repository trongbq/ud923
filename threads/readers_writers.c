#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREAD 5
#define EXEC_TIMES 4
#define MAX_WAIT 4 	// 30 seconds
#define BASE_WAIT 2 // Wait at least 2 seconds

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_write = PTHREAD_COND_INITIALIZER;

int common_value = 0;
int readers = 0;

void *readerFunc(void *param);
void *writerFunc(void *param);

int main(int argc, char **argv) {
	pthread_t treaders[NUM_THREAD], twriters[NUM_THREAD];

	int i;
	for (i = 0; i < NUM_THREAD; i++) {
		if (pthread_create(&treaders[i], NULL, readerFunc, NULL)) {
			fprintf(stderr, "%s\n", "Unalble to create reader");
			exit(1);
		}

		if (pthread_create(&twriters[i], NULL, writerFunc, NULL)) { 
			fprintf(stderr, "%s\n", "Unalble to create writer");
			exit(1);
		}
	}

	for(int i = 0; i < NUM_THREAD; i++) {
		pthread_join(treaders[i], NULL);
		pthread_join(twriters[i], NULL);
	}

	return 0;
}

void *readerFunc(void *params) {
	int read_times = 0;
	while(read_times < EXEC_TIMES) {
		// random amount of time before executing
		struct timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		srand(t.tv_nsec);
		int random_wait = rand() % MAX_WAIT + BASE_WAIT;

		int start_time = time(NULL);
		sleep(random_wait);

		// acquire mutex to update number of readers
		pthread_mutex_lock(&m);
			readers++;
		pthread_mutex_unlock(&m);
			
		printf("READER\tvalue: %d, remaining readers: %d \n", common_value, readers); fflush(stdout);

		pthread_mutex_lock(&m);
			readers--;

			if (readers == 0) {
				pthread_cond_broadcast(&c_write);
			}
		pthread_mutex_unlock(&m);	

		read_times++;
	}
}

void *writerFunc(void *params) {
	int write_times = 0;
	while(write_times < EXEC_TIMES) {
		// random amount of time before executing
		struct timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		srand(t.tv_nsec);
		int random_wait = rand() % MAX_WAIT + BASE_WAIT;

		int start_time = time(NULL);
		sleep(random_wait);

		// acquire mutex to write new value
		pthread_mutex_lock(&m);
			while(readers != 0) {
				pthread_cond_wait(&c_write, &m);
			}

			// Due to multiple writers, must keep in critial section when writing
			common_value++;
			printf("WRITER\tvalue: %d, remaining readers: %d \n", common_value, readers); fflush(stdout);

			pthread_cond_broadcast(&c_write);
		pthread_mutex_unlock(&m);	

		write_times++;
	}
}