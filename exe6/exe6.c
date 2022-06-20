/*
Scrivere un programma C in cui dato un file A, una 
stringa B e un intero N, vengano creati N thread/processi 
che cerchino se all’interno del file A esiste una linea 
uguale a B.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0);

FILE* ifd = 0;
volatile int flag = 0;
volatile int lock = 0;
char *tofind = NULL;


void acquire(){
	while(__sync_lock_test_and_set(&lock, 1))
		while(lock);
}

void release(){
	lock = 0;
}
void *thread_function(void *param){

	char *data = (char*) malloc(100 * sizeof(char));

	while((fgets(data, 100, ifd) != NULL) && !flag){
		//printf("%s",data);
		data[strlen(data)-1] = '\0';
		if(!strcmp(data,tofind)){
			acquire();
			flag = 1;
			release();
		}
	}
	free(data);
	return NULL;
}

int main(int argc, char const *argv[])
{
	/* code */
	if(argc != 4) abort("use ..\n");

	ifd = fopen(argv[1],"r");
	if(ifd == NULL) abort("open error\n");

	int nthreads = atoi(argv[3]);
	tofind = argv[2];
	pthread_t *tids = malloc(nthreads * sizeof(pthread_t));

	for (int i = 0; i < nthreads; ++i)
		pthread_create(tids+i, NULL, thread_function, NULL);

	for (int i = 0; i < nthreads; ++i)
		pthread_join(tids[i],NULL);

	printf("input: %s ---> %d\n",tofind, flag);
	free(tids);
	fclose(ifd);
	return 0;
}



