/* ma

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)
#define SOURCEFILE      argv[1]
#define OUTFILE         argv[2]
#define THREAD_NUMBER 	argv[3]

char *input_buffer = NULL, *output_buffer = NULL;
char num_of_threads; //suppose n of thread < 255 :)
off_t file_lenght;

void *thread_function(void *param){
	char index = *(char*)param;
	
	off_t slice_size = file_lenght / num_of_threads;
	off_t myslice_size = slice_size;
	off_t start_byte;
	
	if(index == (num_of_threads-1))
		myslice_size += (file_lenght % num_of_threads); 
	
	start_byte = index * slice_size;
	printf("thread %d -> ss : %d -> sb : %d\n",index,myslice_size,start_byte);

	for(off_t i=0; i<myslice_size; i++){
		off_t source_byte = start_byte + i;
		off_t destination_byte = file_lenght - source_byte -1 ;
		output_buffer[destination_byte] = input_buffer [source_byte];
	}

	return NULL;
}

int main(int argc, char const *argv[])
{

	// checking command line arguments
	if(argc != 4)  abort("use : exe2 <sourcefile> <outfile> <thread_number>\n");
	num_of_threads = atoi(THREAD_NUMBER);

	// try to open the input file, 2nd commline argument
	int ifd = open(SOURCEFILE, O_RDONLY);
	if(ifd == -1) abort("open error...\n");

	file_lenght = lseek(ifd, 0, SEEK_END); // get num of bytes inside file
										  // num of bytes, not max index!!
	lseek(ifd,0, SEEK_SET);
	input_buffer = malloc(file_lenght);
	output_buffer = malloc (file_lenght);
	printf("file_lenght: %d\n", file_lenght);

	// read the input file and store data in input_buffer
	ssize_t read_bytes=0;
	while(read_bytes < file_lenght){
		ssize_t curr_read = read(ifd, input_buffer+read_bytes, file_lenght);
		if(curr_read < 0) abort("Error while reading input file\n");
		read_bytes += curr_read;
	}

	printf("nbytes read: %d\n",read_bytes);
	printf("bytes read: %s\n",input_buffer);
	
	pthread_t *tids 	 = malloc(num_of_threads * sizeof(pthread_t));
	char 	  *tindexes  = malloc(num_of_threads * sizeof(char));

	// allocate memory for thread ids
	for(char i=0; i<num_of_threads; i++){
		tindexes[i] = i;
		pthread_create(tids+i, NULL, thread_function, tindexes+i);
		//printf("create thread with index %d\n", tindexes[i]);
	}

	for(int i=0; i<num_of_threads; i++){
		pthread_join(tids[i], NULL);
		//printf("join thread with index %d\n", i);
	}

	// close file descriptor and free the memory of tids
	close(ifd);
	free(tids);
	free(tindexes);
	free(input_buffer);

	int ofd = open(OUTFILE, O_WRONLY | O_CREAT | O_TRUNC, 0660);
	if(ofd == -1) abort("creation file error\n");

	ssize_t write_bytes = write(ofd, output_buffer, file_lenght);
	if(write_bytes != file_lenght) abort("write output error\n");

	printf("output_buffer: %s\n",output_buffer);
	close(ofd);	
	free(output_buffer);
	printf("all smooth :) \n");
	return 0;
}










