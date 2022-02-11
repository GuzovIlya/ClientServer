#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "erproc.h"

void signal_handler(int sig){
	switch(sig){
		case SIGHUP:
			printf(">Server --- Hang up signal catched\n");
			exit(EXIT_SUCCESS);
		case SIGTERM:
			printf(">Server --- Client pid %d send SIGTERM signal\n", getppid());
			exit(EXIT_SUCCESS);
	}
}

int main(int argc, char *argv[], char *envp[]){
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	int server = Socket(AF_INET, SOCK_STREAM, 0); // TCP/IP
	struct sockaddr_in adr = {0};
	adr.sin_family = AF_INET; //IPv4
	adr.sin_port = htons(12345);
	Bind(server, (struct sockaddr *) &adr, sizeof adr);

	struct sigaction sa;
	sa.sa_handler = signal_handler;
	sigaction(SIGTERM, &sa, 0);
	sigaction(SIGHUP, &sa, 0);
	while(1){ //While signals not catched
		Listen(server, 3); //max 3 connection
		socklen_t adrlen = sizeof adr;
		int fd = Accept(server, (struct sockaddr *) &adr, &adrlen);
		ssize_t nread;
		char buf[256];
		nread = read(fd, buf, 256); //read from socket to buf
		if(nread == -1){
			perror(">Server ---  Read failed");
			exit(EXIT_FAILURE);
		}
		if(nread == 0){
			printf(">Server --- EOF occured\n");
		}

		char* dir = getcwd(NULL,0); //get current dir
		if(dir == NULL){
			perror(">Client --- Getcwd failed");
			exit(EXIT_FAILURE);
		}
        	char* outname = "/output.txt";
		char* output = calloc(strlen(dir) + strlen(outname), sizeof (char));
		strcat(output, dir);
		free(dir);
		strcat(output, outname); //'dir + /output.txt' for open()
		remove(output); //remove output.txt

	        int dfile = open(output, O_RDWR|O_CREAT, S_IREAD); //create output.txt
		free(output);
		if(dfile == -1){
			perror(">Server --- Open failed");
			exit(EXIT_FAILURE);
		}
		ssize_t nwrite = write(dfile, buf, nread); //write from buf to file
		if(nwrite == -1){
			perror(">Server --- Write failed");
			exit(EXIT_FAILURE);
		}
		int nclose = close(dfile); //close file
		if(nclose == -1){
			perror(">Server --- Close failed");
			exit(EXIT_FAILURE);
		}
		close(fd); //close connection
	}
	close(server); //close socket
	return EXIT_SUCCESS;
}
