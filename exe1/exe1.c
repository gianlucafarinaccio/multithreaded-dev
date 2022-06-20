/* 

Nei sistemi operativi UNIX, /dev/urandom è un dispositivo a 
caratteri (char device) virtuale in grado di generare numeri casuali. 
Nello specifico, l’operazione di lettura dal relativo file produce 
byte casuali. Scrivere un programma C che genera un file con 
contenuto interamente randomico. Il programma:

    prende come parametri da linea di comando: un numero N e 
    una stringa S da usare come nome del file da creare;

    crea un file S contenente N byte randomici;

    utilizza il dispositivo /dev/random come sorgente di numeri 
    pseudo-casuali.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

 #define abort(msg) do{printf(msg);exit(1);}while(0)

int main(int argc, char const *argv[])
{
	int ifd, ofd;
	int size_r, size_w;	
	int n;

	if(argc != 3) abort("usage: ex1 <num of byte> <filename>\n");
	n = atoi(argv[1]);
	char data[n];
	//char *data = malloc(n);

	ifd = open("/dev/urandom", O_RDONLY);
	if(ifd == -1) abort("open error...\n");

	ofd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0660);
	if(ofd == -1) abort("creation file error...\n");

	size_r = read(ifd, data, n);
	if(size_r == -1) abort("reading input file error...\n");
	printf("read: %d byte\n", size_r);

	size_w = write(ofd, data, size_r);
	if(size_w == -1) abort("writing output file error...\n");
	printf("write: %d byte\n", size_w);

	close(ifd);
	close(ofd);
	return 0;
}



