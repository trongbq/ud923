#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4

void *hello(void *args) {
	printf("%s\n", "Hello thread");
}

void *threadFunc(void *parg) {
	int *p = (int*)parg;
	int num = *p;
	printf("Thread number %d\n", num);
	return 0;
}

int main(void) {
	int i;
	pthread_t tid[NUM_THREADS];
	int arr[NUM_THREADS];
	for(i = 0; i < NUM_THREADS; i++) {
		arr[i] = i;
		pthread_create(&tid[i], NULL, threadFunc, &arr[i]);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}