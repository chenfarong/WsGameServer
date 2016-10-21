
#include "server.h"
#include "mcs.h"

#ifndef __WINDOWS__

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h> 

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <type_traits>
#include <utility>
#include <iostream>
#include <list>

#include <sys/epoll.h>

#define MAX_EPOLL_EVENT  100
#define MAX_EVENTS 1000

int gs_listen_socket;
int gs_epf;
struct epoll_event gs_events[MAX_EPOLL_EVENT];

#define BUFFER_SIZE 8192
char buffer[8192+1];

void setnonblocking(int sock) {
	int opts;
	opts = fcntl(sock, F_GETFL);

	if (opts < 0) {
		perror("fcntl(sock, GETFL)");
		exit(1);
	}

	opts = opts | O_NONBLOCK;

	if (fcntl(sock, F_SETFL, opts) < 0) {
		perror("fcntl(sock, SETFL, opts)");
		exit(1);
	}
}

void proc_client_close(int sockfd)
{
	printf("client disconnect:%d",sockfd);
	subscriber_remove(sockfd);
//	struct epoll_event ev;
//	ev.data.fd = sockfd;
//	ev.events = EPOLLOUT | EPOLLET;
	epoll_ctl(gs_epf, EPOLL_CTL_DEL, sockfd, NULL);
	close(sockfd);
}

void proc_client_read(int sockfd) {

	ssize_t n = 0;

	if ((n = recv(sockfd, buffer, BUFFER_SIZE, 0)) < 0)
	{
		if (errno == ECONNRESET) {
			proc_client_close(sockfd);
			return;
			//close(sockfd);
			//			events[i].data.fd = -1;
		}
		else {
			printf("readline error");
		}
	}
	else if (n == 0) {
		proc_client_close(sockfd);
		return;
		//close(sockfd);
		//		events[i].data.fd = -1;
	}

	printf("received data size: %ld\n", n);
	struct epoll_event ev;
	ev.data.fd = sockfd;
	ev.events = EPOLLOUT | EPOLLET;
	epoll_ctl(gs_epf, EPOLL_CTL_MOD, sockfd, &ev);

	//
	if (n > 0) {
		xmsg_command(sockfd,buffer,n);
	}
}

void proc_client_accept(int lifd) {
	struct epoll_event ev;
	socklen_t clilen = 0;
	struct sockaddr_in clientaddr;

	//	printf("accept connection, fd is %d\n", eventLoop->listenfd);
	int connfd = accept(lifd, (struct sockaddr *) &clientaddr,
		&clilen);
	if (connfd < 0) {
		perror("connfd < 0");
		exit(1);
	}

	setnonblocking(connfd);

	char *str = inet_ntoa(clientaddr.sin_addr);
	printf("fd:%d connect from %s\n", connfd, str);


	ev.data.fd = connfd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(gs_epf, EPOLL_CTL_ADD, connfd, &ev);

	subscriber_add(connfd);
}


int ServerInit()
{
	gs_epf = epoll_create(MAX_EVENTS);
	return 0;
}

int ServerListen(std::string ipAddr, int port)
{
	struct sockaddr_in serveraddr;
	struct epoll_event ev;
	int sockfd = 0;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	setnonblocking(sockfd);

	ev.data.fd = sockfd;
	ev.events = EPOLLIN | EPOLLET;

	epoll_ctl(gs_epf, EPOLL_CTL_ADD, sockfd, &ev);

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	//	char *local_addr = "192.168.199.8";
	inet_aton(ipAddr.c_str(), &(serveraddr.sin_addr));
	//inet_aton("192.168.50.67",&(serveraddr.sin_addr));
	serveraddr.sin_port = htons(port);

	bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));

	listen(sockfd, 100);

	printf("%s ip:%s port:%d\n", __FUNCTION__, ipAddr.c_str(), port);

	gs_listen_socket=sockfd;

	return 0;
}


int ServerStep()
{
	//	socklen_t clilen=0;
	int sockfd = 0;
	//	struct sockaddr_in clientaddr;
	//	struct fe_event_loop_s *eventLoop = (struct fe_event_loop_s *) arg;

	int connfd = 0;
	int nfds = epoll_wait(gs_epf, gs_events, MAX_EPOLL_EVENT, 200);
	int i = 0;
	struct epoll_event ev;

	for (i = 0; i < nfds; ++i)
	{
		connfd = gs_events[i].data.fd;
		if (connfd == gs_listen_socket)
		{
			proc_client_accept(connfd);

			//			printf("accept connection, fd is %d\n", eventLoop->listenfd);
			//			connfd = accept(eventLoop->listenfd, (struct sockaddr *)&clientaddr, &clilen);
			//			if (connfd < 0) {
			//				perror("connfd < 0");
			//				exit(1);
			//			}
			//			setnonblocking(connfd);
			//			char *str = inet_ntoa(clientaddr.sin_addr);
			//			printf("connect from %s\n", str);
			//			ev.data.fd = connfd;
			//			ev.events = EPOLLIN | EPOLLET;
			//			epoll_ctl(eventLoop->epfd, EPOLL_CTL_ADD, connfd, &ev);
		}
		else if (gs_events[i].events & EPOLLIN)
		{
			if ((sockfd = gs_events[i].data.fd) < 0)
				continue;
			/*
			if ((n = read(sockfd, line, MAXLINE)) < 0)
			{
			if (errno == ECONNRESET) {
			close(sockfd);
			events[i].data.fd = -1;
			}
			else {
			printf("readline error");
			}
			}
			else if (n == 0) {
			close(sockfd);
			eventLoop->events[i].data.fd = -1;
			}
			printf("received data: %s\n", line);

			ev.data.fd = sockfd;
			ev.events = EPOLLOUT | EPOLLET;
			epoll_ctl(eventLoop->epfd, EPOLL_CTL_MOD, sockfd, &ev);
			*/

			proc_client_read(sockfd);
			printf("EPOLLIN %d\n", sockfd);
			//			gs_thread_pool.add(proc_client_read,sockfd);

		}
		else if (gs_events[i].events & EPOLLOUT) {
			sockfd = gs_events[i].data.fd;
			//			write(sockfd, line, n);
			//			printf("written data: %s\n", line);

			ev.data.fd = sockfd;
			ev.events = EPOLLIN | EPOLLET;
			epoll_ctl(gs_epf, EPOLL_CTL_MOD, sockfd, &ev);
		}
		else if (gs_events[i].events & EPOLLHUP)
		{
			proc_client_close(gs_events[i].data.fd);
		}

	}

	return 0;
}

void ServerClean()
{

}
int ServerCore()
{
	return 0;
}

void OnShutdown()
{

}


#endif


