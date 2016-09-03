#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUF_SIZE 3

int buffer[BUF_SIZE];  	// share buffer
int add = 0; 			// place to add next element
int rem = 0;			// place to remove next element
int num = 0;			// number elements in buffer

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER;

void *producer (void *param);
void *consumer (void *param);

int main(int argc, char *argv[]) {
	pthread_t tid1, tid2;

	if (pthread_create(&tid1, NULL, producer, NULL) != 0) {
		fprintf(stderr, "Unable to create producer thread\n");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, consumer, NULL) != 0) {
		fprintf(stderr, "Unable to create consumar thread\n");
		exit(1);
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	printf("Exiting...\n"); fflush(stdout);
}

void *producer(void *param) {
	int i = 0;
	for (i = 1; i < 20; i++) {
		pthread_mutex_lock(&m); 
			if (num > BUF_SIZE) {
				exit(1); 	// overflow
			}

			while(num == BUF_SIZE) {
				pthread_cond_wait(&c_prod, &m);
			}

			buffer[add] = i;
			add = (add+1) % BUF_SIZE;
			num++;
		pthread_mutex_unlock(&m);

		pthread_cond_signal(&c_cons);
		printf("PRODUCER\tinserted %d\n", i); fflush(stdout);
	}

	printf("Producer quiting\n"); fflush(stdout);
	return 0;
}

void *consumer(void *param) {
	int i;
	while(1) {
		pthread_mutex_lock(&m);
			if (num < 0) {
				exit(1);  	// underflow
			}

			while(num == 0) {
				pthread_cond_wait(&c_cons, &m);
			}

			i = buffer[rem];
			rem = (rem+1) % BUF_SIZE;
			num--;
		pthread_mutex_unlock(&m);
		
		pthread_cond_signal(&c_prod);
		printf("CONSUMER\tvalue is %d\n", i); fflush(stdout);	
	}
}