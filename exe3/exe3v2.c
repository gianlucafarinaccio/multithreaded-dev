/* ma

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

char *input_buffer, *output_buffer;
char num_of_threads;

int file_lenght;

void *thread_function(void *param){

	return NULL;
}

int main(int argc, char const *argv[])
{

	// checking command line arguments
	if(argc != 4)  abort("use : exe2 <sourcefile> <outfile> <thread_number>\n");
	num_of_threads = atoi(argv[3]);

	// try to open the input file, 2nd commline argument
	int ifd = open(argv[1], O_RDONLY);
	if(ifd == -1) abort("open error...\n");

	file_lenght = lseek(ifd, 0, SEEK_END);
	lseek(ifd,0, SEEK_SET);
	input_buffer = malloc(file_lenght);

	printf("file_lenght: %d\n", file_lenght);

	int read_bytes = 0;
	while(read_bytes < file_lenght){
		int curr_read = read(ifd, input_buffer+read_bytes, file_lenght);
		printf("curr_read: %d\n", curr_read);
		if(curr_read < 0) abort("Error while reading input file\n");
		read_bytes += curr_read;
	}

	printf("bytes read: %d\n",read_bytes);
	printf("bytes read: %s\n",input_buffer);
	printf("last byte : %c\n",input_buffer[file_lenght-1]);

	return 0;
}










