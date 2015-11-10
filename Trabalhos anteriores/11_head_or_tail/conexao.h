/*
 * conexao.h
 *
 *  Created on: Nov 25, 2014
 *      Author: guilherme
 */

#ifndef CONEXAO_H_
#define CONEXAO_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 22222

void error(const char* format, ...);
int open_socket(char *ip, int port);
void set_address(struct sockaddr_in *address, char *ip, int port);
void close_socket(int socket_id);
void recebeMensagem(int local_socket, char *buffer, struct sockaddr_in *remote_address, socklen_t *address_size);
void enviaMensagem(int local_socket, char *buffer, int message_size, struct sockaddr_in *remote_address, socklen_t remote_address_size);



#endif /* CONEXAO_H_ */
