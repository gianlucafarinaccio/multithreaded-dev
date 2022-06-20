/* 
Si scriva una funzione di codice C con la seguente interfaccia:
int above_threshold(char *filename, int n, int threshold).
Tale funzione deve lanciare un numero di thread/processi pari 
a n per leggere dal file binario filename una sequenza di int 
e contare quanti di questi hanno un valore maggiore di threshold. 
Infine, la funzione stampa su standard output e ritorna il numero 
totale di int con valore maggiore di threshold contenuti nel file.
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#define abort(msg) do{printf(msg);exit(1);}while(0);
#define CHUNK_SIZE 512

int thresh = 0;
int ifd = 0;


void *thread_function(void *param){

	// printf("mytresh : %d\n", thresh);
	// printf("thread %d , slice : %d\n", index, myslice);

	int counter = 0;
	int read_bytes = 0;
	int *data = (int*)malloc(CHUNK_SIZE * sizeof(int));

	while((read_bytes = read(ifd, data, CHUNK_SIZE * sizeof(int))) > 0){
		for(int i=0; i < read_bytes; i++){
			if(data[i] > thresh) 
				counter++;
		}
	}

	//printf("thread %d , counter: %d , param : %p\n", index, counter,param);
	*(int*)param = counter;		
	//pthread_exit(param);
	return NULL;
}

int above_threshold(char *filename, int n, int threshold){

	thresh = threshold;
	ifd = open(filename,O_RDONLY);
	if(ifd == -1) abort("open file error\n");


	pthread_t *tids = malloc(n * sizeof(pthread_t));
	int *partials = (int*)malloc(n * sizeof(int));

	for(int i=0; i<n; i++){
		partials[i] = i;
		//printf("index[%d] -> add : %p -> val : %d\n",i,(partials+i),partials[i]);
		pthread_create(tids+i, NULL, thread_function, partials+i);
	}

	int out = 0;
	for(int i=0; i<n; i++){
		//pthread_join(tids[i], (void**)&ptr);
		pthread_join(tids[i], NULL);
		//printf("return thread %d --> %d \n", i, *ptr);
		//printf("index[%d] -> %d\n", i, partials[i]);
		//out += *ptr ;
		out += partials[i];
	}
	
	free(tids);
	free(partials);
	close(ifd);
	return out;
}


int main(int argc, char const *argv[])
{
	if(argc != 4) abort("usage: <sourcefile> <nthread> <threshold>\n");

	int num_of_thread = atoi(argv[2]);
	int threshold = atoi(argv[3]);
	void *file = argv[1];
	printf("out %d : %d \n",threshold,above_threshold(file, num_of_thread, threshold));

	return 0;
}







