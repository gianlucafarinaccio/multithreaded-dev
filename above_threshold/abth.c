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

int thresh = 0;
int threads = 0;
int *data = NULL;
off_t file_lenght = 0;


void *thread_function(void *param){

	int index = *(int*)param;
	int slice = (file_lenght/sizeof(int)) / threads;
	int myslice = slice;


	if(index == threads -1)
		myslice += ((file_lenght/sizeof(int)) % threads);

	printf("mytresh : %d\n", thresh);
	printf("thread %d , slice : %d\n", index, myslice);

	int counter = 0;
	int bytes_start = index * slice;
	
	for(int i=0; i < myslice; i++){
		if(data[bytes_start+i] > thresh) 
			counter++;
	}

	printf("thread %d , counter: %d , param : %p\n", index, counter,param);
	*(int*)param = counter;		
	pthread_exit(param);
	//return NULL;
}

int above_threshold(char *filename, int n, int threshold){

	thresh = threshold;
	int ifd = open(filename,O_RDONLY);
	if(ifd == -1) abort("open file error\n");

	file_lenght = lseek(ifd, 0, SEEK_END);
	lseek(ifd, 0, SEEK_SET);
	printf("file_lenght: %d\n",file_lenght);

	data = (int*)malloc(file_lenght);
	ssize_t read_bytes = 0;
	ssize_t curr_read = 0;
	while( (curr_read = read(ifd, data, file_lenght))  > 0)
		read_bytes += curr_read;
	if(read_bytes != file_lenght) abort("file read error\n");

	int k = (read_bytes/sizeof(int));
	if( k < n ) n = k;
	threads = n;

	pthread_t *tids = malloc(n * sizeof(pthread_t));
	int *tindexes = (int*)malloc(n * sizeof(int));

	for(int i=0; i<n; i++){
		tindexes[i] = i;
		printf("index[%d] -> add : %p -> val : %d\n",i,(tindexes+i),tindexes[i]);
		pthread_create(tids+i, NULL, thread_function, tindexes+i);
	}

	int out = 0;
	int *ptr = NULL;
	for(int i=0; i<n; i++){
		//pthread_join(tids[i], (void**)&ptr);
		pthread_join(tids[i], NULL);
		//printf("return thread %d --> %d \n", i, *ptr);
		printf("index[%d] -> %d\n", i, tindexes[i]);
		//out += *ptr ;
		out += tindexes[i];
	}
	
	free(data);
	free(tids);
	free(tindexes);
	close(ifd);
	return out;
}


int main(int argc, char const *argv[])
{
	if(argc != 4) abort("usage: <sourcefile> <nthread> <threshold>\n");

	int num_of_thread = atoi(argv[2]);
	int threshold = atoi(argv[3]);
	char *file = argv[1];
	printf("out %d : %d \n",threshold,above_threshold(file, num_of_thread, threshold));

	return 0;
}







