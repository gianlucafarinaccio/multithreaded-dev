/* ma

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)
#define SIZE 512
#define STDOUT 1

void *thread_function(void *param){
	int fd     = *(int*)param;
	char *data = malloc(SIZE * sizeof(char));
	ssize_t read_bytes = 0, write_bytes = 0;

	do {
		read_bytes = read(fd, data, SIZE);
		if(read_bytes > 0){
			write_bytes = write(STDOUT, data, read_bytes);
			if(read_bytes != write_bytes) abort("write error\n");		
		}
	} while(read_bytes > 0);
 
 	free(data);
	return NULL;
}


void tunnel(int descriptors[], int count){
	
	pthread_t *tids = malloc(count * sizeof(pthread_t)); 

	for (int i = 0; i < count; ++i)
		pthread_create(tids+i, NULL, thread_function, descriptors+i);

	for(int i=0; i < count; i++){
		pthread_join(tids[i],NULL);
		close(descriptors[i]);
	}
	free(tids);
}


int main(int argc, char const *argv[])
{
	int count;
	
	// checking command line arguments
	if(argc <= 1)  abort("use : exe2 <sourcefile> <...>\n");
	count = argc-1;

	int *descriptors = malloc(count * sizeof(int));
	for(int i=0; i<count; i++){
		descriptors[i] = open(argv[i+1], O_RDONLY);
		if(descriptors[i] == -1) abort("open file error\n");
	}

	tunnel(descriptors,count);
	free(descriptors);
	printf("\nall went smooth :)\n");
	return 0;
}










