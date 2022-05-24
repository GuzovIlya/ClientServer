#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

#define BUF_SIZE    30000
#define PORT        5050
#define MAX_CLIENTS 100000

static int count_client = 0;
static bool send_start = false;

typedef struct threadArgs{
	int fd;
	char *buf;
	ssize_t nread;
} threadArgs_t;

struct timeval tv1, tv2, dtv;

struct timezone tz;

static void time_start()
{ 
	gettimeofday(&tv1, &tz); 
}

static long time_stop() 
{ 
	gettimeofday(&tv2, &tz);
	dtv.tv_sec= tv2.tv_sec - tv1.tv_sec;
  	dtv.tv_usec=tv2.tv_usec - tv1.tv_usec;
  	if(dtv.tv_usec < 0){ 
		dtv.tv_sec--; 
		dtv.tv_usec += 1000000; 
	}
  	return dtv.tv_sec*1000+dtv.tv_usec/1000;
}
/*
static void* client_thread(void *args)
{
	while(!send_start)
		continue;
	threadArgs_t *arg = (threadArgs_t*) args;
	write(arg->fd, arg->buf, arg->nread);
	
	pthread_exit(0);
}
*/
static int init_server(struct sockaddr_in addr)
{
	int server = socket(AF_INET, SOCK_STREAM, 0); // TCP/IP
	addr.sin_family = AF_INET; //IPv4
	addr.sin_port = htons(PORT);
	bind(server, (struct sockaddr *) &addr, sizeof addr);
	return server;
}

int main()
{
	int fd_clients[MAX_CLIENTS];
	int socket_server;
//	pthread_t threads[MAX_CLIENTS];
	threadArgs_t args[MAX_CLIENTS];
	struct sockaddr_in adr = {0};
	socklen_t adrlen = sizeof adr;
	socket_server = init_server(adr);
	
	while(1)
	{
		listen(socket_server, MAX_CLIENTS);
		int fd = accept(socket_server, (struct sockaddr *) &adr, &adrlen);
		if(!fd)
		{
			perror("Server - Accept error");
			break;
		}
		time_start();
		fd_clients[count_client] = fd;
		char buf[BUF_SIZE];
		ssize_t nread = read(fd, buf, BUF_SIZE); //read from socket to buf
		args[count_client].fd = fd;
		
		for(int i = 0; i <= count_client; i++)
		{
			args[i].buf = buf;
			args[i].nread = nread;	
		}
/*
		for(int i = 0; i < count_client; i++)
		{
			pthread_create(&threads[i], NULL, client_thread, (void*) &args[i]);
		}
*/
		for(int i = 0; i < count_client; i++)
		{
			write(args[i].fd, args[i].buf, args[i].nread);
		}
/*		send_start = true;
		for(int i = 0; i < count_client; i++)
		{
			pthread_join(threads[i], NULL);
		}
		send_start = false;
*/		int long diff_time = time_stop();
		
		char buf_time[32];
		sprintf(buf_time, "%ld\n", diff_time);
		write(fd_clients[count_client], buf_time, strlen(buf_time));
		count_client++;
	}
	
	for(int i = 0; i <= count_client; i++)
	{
		close(fd_clients[i]); //close connection
	}
	
	close(socket_server); //close socket
	return EXIT_SUCCESS;
}
