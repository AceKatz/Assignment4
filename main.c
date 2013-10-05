/**
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 *
 * Copyright 2012 by Gabriel Parmer.
 * Author: Gabriel Parmer, gparmer@gwu.edu, 2012
 */
/* 
 * This is a HTTP server.  It accepts connections on port 8080, and
 * serves a local static document.
 *
 * The clients you can use are 
 * - httperf (e.g., httperf --port=8080),
 * - wget (e.g. wget localhost:8080 /), 
 * - or even your browser.  
 *
 * To measure the efficiency and concurrency of your server, use
 * httperf and explore its options using the manual pages (man
 * httperf) to see the maximum number of connections per second you
 * can maintain over, for example, a 10 second period.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <pthread.h>

#include <util.h> 		/* client_process */
#include <server.h>		/* server_accept and server_create */

#include "cas.h"
#include "request.h"

#define MAX_DATA_SZ 1024
#define MAX_CONCURRENCY 256

volatile int var = 0; 

/* 
 * This is the function for handling a _single_ request.  Understand
 * what each of the steps in this function do, so that you can handle
 * _multiple_ requests.  Use this function as an _example_ of the
 * basic functionality.  As you increase the server in functionality,
 * you will want to probably keep all of the functions called in this
 * function, but define different code to use them.
 */
void
server_single_request(int accept_fd)
{
	int fd;

	/* 
	 * The server thread will always want to be doing the accept.
	 * That main thread will want to hand off the new fd to the
	 * new threads/processes/thread pool.
	 */
	fd = server_accept(accept_fd);
	client_process(fd);

	return;
}

void
server_multiple_requests(int accept_fd)
{
    for (;;) client_process(server_accept(accept_fd));
	return;
}

void
server_processes(int accept_fd)
{
    pid_t childID; 
    int fd, status, endID, i;
    int count = 0;
    int temp = 0;
    pid_t *connect = malloc(sizeof(pid_t)*MAX_CONCURRENCY);
    for(;;){
        while (count >= MAX_CONCURRENCY) {
            wait(&status);
            int tempCount = count;
            count = 0;
            for (i = 0 ; i < tempCount ; i++) {
                if (waitpid(connect[i], &status, WNOHANG|WUNTRACED)) {
                    connect[temp] = connect[i];
                    temp++;
                } else 
                    count++;
            }
        }
        if (count >= MAX_CONCURRENCY) 
            continue;
        else {
            fd = server_accept(accept_fd);
            if (childID == -1) {
                perror("FORK ERROR");
                break;
            }
            else if (childID == 0) {
                client_process(fd);
                break;
            }
            else {
                childID = fork();
                temp++;
                connect[temp] = childID;
                count++;
            }
        }
    }
	return;
}

void
server_dynamic(int accept_fd)
{
	return;
}


void
server_thread_per(int accept_fd)
{
    // 4!  Work here!
    pthread_t *threadArray = malloc(sizeof(pthread_t) * MAX_CONCURRENCY);
    int fd, i, count = 0;
    for(;;) {
        if (count < MAX_CONCURRENCY) 
        {
            fd = server_accept(accept_fd);
            if (!pthread_create(&threadArray[count], NULL, (void*)&client_process, (void*) fd)) 
                count++;
        }
        else 
            for (i = 0; i < MAX_CONCURRENCY ; i++) 
                if (!pthread_join(threadArray[i], NULL)) 
                    count--;
    }
	return;
}



void 
pthread_handle (void)
{
    int fd;
    for (;;) {
        struct request *get = get_request();
        if (get != NULL) {
            fd = (get->fd);
            client_process(fd);
        }
    }
    
}

void
server_task_queue(int accept_fd)
{
    //5!  Work here!
    int i;
    pthread_t *threads = malloc(sizeof(pthread_t) * MAX_CONCURRENCY);
    for (i = 0 ; i < MAX_CONCURRENCY ; i++)
        pthread_create(&threads[i], NULL, (void *)&pthread_handle, NULL);
    printf("ALL THREADS CREATED\n");
    for(;;) 
    {
        int fd = server_accept(accept_fd);
        put_request(req_create((void*)fd));
        printf("ADDING STUFF!\n");
    }
    
	return;
}

void
server_thread_pool(int accept_fd)
{
	return;
}

typedef enum {
	SERVER_TYPE_ONE = 0,
	SERVER_TYPE_SINGLET,
	SERVER_TYPE_PROCESS,
	SERVER_TYPE_FORK_EXEC,
	SERVER_TYPE_SPAWN_THREAD,
	SERVER_TYPE_TASK_QUEUE,
	SERVER_TYPE_THREAD_POOL,
} server_type_t;

int
main(int argc, char *argv[])
{
	server_type_t server_type;
	short int port;
	int accept_fd;

	if (argc != 3) {
		printf("Proper usage of http server is:\n%s <port> <#>\n"
		       "port is the port to serve on, # is either\n"
		       "0: serve only a single request\n"
		       "1: use only a single thread for multiple requests\n"
		       "2: use fork to create a process for each request\n"
		       "3: Extra Credit: use fork and exec when the path is an executable to run the program dynamically.  This is how web servers handle dynamic (program generated) content.\n"
		       "4: create a thread for each request\n"
		       "5: use atomic instructions to implement a task queue\n"
		       "6: use a thread pool\n"
		       "7: to be defined\n"
		       "8: to be defined\n"
		       "9: to be defined\n",
		       argv[0]);
		return -1;
	}

	port = atoi(argv[1]);
	accept_fd = server_create(port);
	if (accept_fd < 0) return -1;
	
	server_type = atoi(argv[2]);

	switch(server_type) {
	case SERVER_TYPE_ONE:
		server_single_request(accept_fd);
		break;
	case SERVER_TYPE_SINGLET:
		server_multiple_requests(accept_fd);
		break;
	case SERVER_TYPE_PROCESS:
		server_processes(accept_fd);
		break;
	case SERVER_TYPE_FORK_EXEC:
		server_dynamic(accept_fd);
		break;
	case SERVER_TYPE_SPAWN_THREAD:
		server_thread_per(accept_fd);
		break;
	case SERVER_TYPE_TASK_QUEUE:
		server_task_queue(accept_fd);
		break;
	case SERVER_TYPE_THREAD_POOL:
		server_thread_pool(accept_fd);
		break;
	}
	close(accept_fd);

	return 0;
}
