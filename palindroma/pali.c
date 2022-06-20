#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)


FILE* ifd = 0;
int lock = 0;
int counter = 0;

void acquire(){
	while(__sync_lock_test_and_set(&lock, 1))
		while(lock);
}

void release(){
	lock = 0;
}


int palindroma(char *s1){
	int len = strlen(s1);
	s1[len-1] = '\0';
	char s2[len];
	s2[len-1] = '\0';

	for(int i=0; i<len; i++)
		s2[len-2-i] = s1[i];
	return(strcmp(s1,s2) == 0);
}

void *thread_function(void *param){

	char data[30];
	while(fgets(data, 30, ifd) != NULL){
		if(palindroma(data)){
			acquire();
			counter++;
			release();
		}
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	if(argc != 3) abort("use...");

	ifd = fopen(argv[1],"r");
	if(ifd == NULL) abort("open error\n");

	int num_of_thread = atoi(argv[2]);

	pthread_t *tids = malloc(num_of_thread * sizeof(pthread_t));


	for (int i = 0; i < num_of_thread; ++i)
	{
		pthread_create(tids+i, NULL, thread_function, NULL);
	}

	for (int i = 0; i < num_of_thread; ++i)
	{
		pthread_join(tids[i], NULL);
	}

	printf("palindrome : %d\n",counter);

	return 0;
}