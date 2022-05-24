#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

#define BUF_SIZE  30000
#define FULL_NAME 512
#define PORT      5050

static int init_client(char *ip_server){
	int fd = socket(AF_INET, SOCK_STREAM, 0); // TCP/IP
	struct sockaddr_in adr = {0};
	adr.sin_family = AF_INET; //IPv4
	adr.sin_port = htons(PORT);
	inet_pton(AF_INET, ip_server, &adr.sin_addr);
	int rc_connect = connect(fd, (struct sockaddr *) &adr, sizeof adr);
	if(rc_connect == -1)
	{
		perror("Client - Connection error");
		exit(EXIT_FAILURE);
	}
	return fd;
}

int main(int argc, char *argv[])
{
        char *ip_server = "127.0.0.1";
        char *file_name;
        char buffer[BUF_SIZE];
        char* dir = getcwd(NULL, 0); //get current dir
        char fullname[FULL_NAME];
        bool its_time = true;
        
        if(argc == 3)
        {
        	ip_server = argv[1];
        	file_name = argv[2];
        }
        else if(argc == 2)
        {
        	file_name = argv[1];
        }
        sprintf(fullname, "%s/%s", dir, file_name);
        int client_socket = init_client(ip_server);
        
	int dfile = open(fullname, O_RDONLY); //open file
	ssize_t nread = read(dfile, buffer, BUF_SIZE); //read from file to buffer
	int nclose = close(dfile); //close file
	write(client_socket, buffer, nread); //write from buffer to socket
	
	while(1)
	{
		if(!client_socket)
			break;
		char fullname[FULL_NAME];
		char buf[BUF_SIZE];
		ssize_t nread = read(client_socket, buf, BUF_SIZE);
		if((nread > 0) && (its_time)){
			sprintf(fullname, "%s/%s%s", dir, "time", ".txt");
			FILE *fd = fopen (fullname, "a+");
			if (fd){
				fwrite(buf, 1, strlen(buf), fd);
				fclose(fd);
			}
			its_time = false;
		}
		else if((nread > 0) && (!its_time))
		{
			sprintf(fullname, "%s/%s/%d%s", dir, "out", rand(), ".txt");
			remove(fullname);
			int dfile = open(fullname, O_RDWR|O_CREAT, S_IREAD);
			write(dfile, buf, nread);
			close(dfile);
		}
	}
	
	close(client_socket); //close socket
	return EXIT_SUCCESS;
}