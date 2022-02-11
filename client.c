#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "erproc.h"


pid_t pid;

int main(){
	pid_t pid = fork();
	if(pid == -1){
		perror(">Client --- Fork failed");
		exit(EXIT_FAILURE);
	}
//If child start server	
	if(pid == 0){
		char* dir = getcwd(NULL,0); //get current dir
		if(dir == NULL){
			perror(">Client --- Getcwd failed");
			exit(EXIT_FAILURE);
		}
        	char* nameprog = "/server";
		char* program = calloc(strlen(dir) + strlen(nameprog), sizeof (char));
		strcat(program, dir);
		free(dir);
		strcat(program, nameprog); //'dir + /server' for execve()
		char* argv[] = {program, NULL, NULL};
		char* envp[]  = {NULL};
		int ret = execve(program, argv, envp);
		if(ret == -1){
			perror(">Client --- Execve failed");
			exit(EXIT_FAILURE);
		}
		free(program);
	}
//If parrent then connect to server	
	else{
		sleep(1);
		int fd = Socket(AF_INET, SOCK_STREAM, 0); // TCP/IP
		struct sockaddr_in adr = {0};
		adr.sin_family = AF_INET; //IPv4
		adr.sin_port = htons(12345);
		Inet_Pton(AF_INET, "127.0.0.1", &adr.sin_addr);
		Connect(fd, (struct sockaddr *) &adr, sizeof adr);
		printf("Enter full path to .txt file\n");
		char inpname[256];
		fgets(inpname, 256, stdin);
		inpname[strlen(inpname) - 1] = '\0'; //delete '\n'
		int dfile = open(inpname, O_RDONLY); //open file
		if(dfile == -1){
			perror(">Client --- Open failed");
			exit(EXIT_FAILURE);
		}	
		char buf[256];
		ssize_t nread = read(dfile, buf, 256); //read from file to buf
		if(nread == -1){
			perror(">Client --- Read failed");
			exit(EXIT_FAILURE);
		}
		if(nread == 0){
			printf("EOF occured\n");
		}
		int nclose = close(dfile); //close file
		if(nclose == -1){
			perror(">Client --- Close failed");
			exit(EXIT_FAILURE);
		}
		write(fd, buf, nread); //write from buf to socket
		close(fd); //close socket
		return EXIT_SUCCESS;
	}
}
