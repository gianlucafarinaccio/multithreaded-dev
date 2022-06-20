/* 
Dato un file binario contenente un sequenza di 2^15 interi di 
tipo short, scrivere un programma che crea N processi 
o threads, i quali leggono il contenuto del file ed individuano 
il valore minimo e massimo contenuto nel file. 
Nel fornire una soluzione rispettare i seguenti vincoli:

ciascun intero non può essere letto da più di un thread/processo;

ciascun thread/processo può leggere il medesimo intero al più 
una volta;

ciascun thread/processo può allocare memoria nell’heap per 
al più 512 byte;

N è un parametro definito a tempo di compilazione o tramite 
linea di comando;

N è minore o uguale a 8;

è ammesso allocare di variabili globali (data) e locali (stack) 
per memorizzare tipi primitivi (puntatori, int, short, char, 
long, etc.) per al più 128 byte.

Per generare il file è possibile utilizzare la soluzione 
dell’esercizio 1.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

// volatile short... os read every time this variable 
// from memory and not from cache
volatile short min = SHRT_MAX; 
volatile short max = SHRT_MIN;

int ifd; // global file descriptor

#define NNUMBERS 32768 //2^15 short inside the file
#define FILESIZE NNUMBERS*sizeof(short)

#define SIZE_MEM 512 //max byte of mem allocable for every thread
#define SIZE_DATA 512/sizeof(short) //max num of short readable

void *thread_function(){
	short local_min = SHRT_MAX;
	short local_max = SHRT_MIN;
	short *data = malloc(SIZE_DATA * sizeof(short));
	int size_r; // num of byte read

	while((size_r = read(ifd, data, SIZE_MEM)) > 0){
		for(int i=0; i < (size_r / sizeof(short)); i++){
			if(data[i] < local_min) local_min = data[i];
			if(data[i] > local_max) local_max = data[i];
		}
	}

	short global_min;
	do global_min = min;
	while((global_min > local_min) && !__sync_bool_compare_and_swap(&min, global_min, local_min));

	short global_max;
	do global_max = max;
	while((global_max < local_max) && !__sync_bool_compare_and_swap(&max, global_max, local_max));

	return NULL;
	//pthread_exit(param); si usa nel caso in cui dovessi ritornare 
	//				  un paramentro al thread padre
}

int main(int argc, char const *argv[])
{

	// checking command line arguments
	if(argc != 3)  abort("use : exe2 <thread_number> <filename>\n");
	char num_of_threads = atoi(argv[1]);

	// try to open the input file, 2nd commline argument
	ifd = open(argv[2], O_RDONLY);
	if(ifd == -1) abort("open error...\n");

	// allocate memory for thread ids
	pthread_t *tids = malloc(num_of_threads * sizeof(pthread_t));
	
	// create threads
	for(int i=0; i<num_of_threads; i++)
		pthread_create(tids+i, NULL, thread_function, NULL);
		
	//  for(int i=0; i<num_of_threads; i++)
	// 	printf("%x\n",tids[i]);

	// join every threads
	for(int i=0; i<num_of_threads; i++)
		pthread_join(*(tids+i), NULL); // also ok tids[i]

	// close file descriptor and free the memory of tids
	close(ifd);
	free(tids);

	printf("max: %d\n",max);
	printf("min: %d\n",min);
	return 0;
}










