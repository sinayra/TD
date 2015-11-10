/*
 * testht.c
 *
 *  Created on: Dec 4, 2014
 *      Author: Matheus Rosa, Wesley Rocha
 */

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

#define BUFFER_SIZE 1024
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 22222



int parse_arguments(int argc, char **argv, int *port);
void error(const char* format, ...);
void run_server(int port);
void run_server_loop(int local_socket);
void run_client(int port);
void run_client_loop(int local_socket, struct sockaddr_in *remote_address, socklen_t *address_size);
int open_socket(char *ip, int port);
void set_address(struct sockaddr_in *address, char *ip, int port);
void close_socket(int socket_id);
void receive_message(int local_socket, char *buffer, struct sockaddr_in *remote_address, socklen_t *address_size);
void send_message(int local_socket, char *buffer, int message_size, struct sockaddr_in *remote_address, socklen_t remote_address_size);



int main(int argc, char **argv)
{
    int port;
    int program_type = parse_arguments(argc, argv, &port);

    srand(time(NULL));

    if (program_type == 1)
    	run_server(port);
    else if (program_type == 2)
    	run_client(port);
    else
        error("Tipo de programa desconhecido");

    return 0;
}

int parse_arguments(int argc, char **argv, int *port)
{
    if ((argc != 2) && (argc != 3))
        error("Número de argumentos deve ser 2 ou 3\n"
              "Utilização:\n"
              "head_or_tail -S [port]\n"
              "head_or_tail host [port]");

    if (argc == 2)
        *port = DEFAULT_PORT;
    else
        *port = atoi(argv[2]);

    if (strcmp(argv[1], "-S") == 0)
        return 1;
    else if (strcmp(argv[1], "host") == 0)
        return 2;
    else
        error("Opções disponíveis: -S (servidor) ou host (cliente)\n"
              "Utilização:\n"
              "head_or_tail -S [port]\n"
              "head_or_tail [port]");

    return 0;
}


void error(const char* format, ...)
{
    va_list args;

    fprintf(stderr, "ERRO: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");

    exit(-1);
}

void run_server(int port)
{
    int local_socket;

    printf("=== head_or_tail servidor ===\n");

    local_socket = open_socket(DEFAULT_IP, port);
    run_server_loop(local_socket);
    close_socket(local_socket);
}

void run_server_loop(int local_socket)
{
    int server_state = 0;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in remote_address;
    socklen_t address_size = sizeof(remote_address);
    int S, x, yXorS1, S1,y;

    printf("Inicializando servidor\n");

    while (1)
    {
        printf("\nServidor FSM: %d\n", server_state);

        switch (server_state)
        {
            case 0:
                receive_message(local_socket, buffer, &remote_address, &address_size);

               if (strcmp(buffer,"Hello"))
                {
                    sprintf(buffer, "Hello(nok)");
                    send_message(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);
                    server_state = -1;
                }
                else
                {
                    x = (int)(rand()%2);
		    S = rand();
		    printf("S: %d\n",S);
		    sprintf(buffer, "Msg(%d)", (x ^ S));
		    printf("x: %d\n", x);
		    printf("x XOR S: %d\n",(x ^ S));
                    send_message(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);
                    ++server_state;
                }
                break;

            case 1:


                receive_message(local_socket, buffer, &remote_address, &address_size);
                sscanf(buffer, "Msg(%d)", &yXorS1);
                printf("y XOR S': %d\n", yXorS1);
                sprintf(buffer, "Msg(%d)", S);
		printf("S: %d\n", S);
                send_message(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);
                ++server_state;
                break;

            case 2:



                receive_message(local_socket, buffer, &remote_address, &address_size);

                if(strcmp(buffer, "Msg(nok)") != 0){


                sscanf(buffer, "Msg(S':%d,ok)", &S1);
                printf("S' recebido: %d\n", S1);

                y = yXorS1 ^ S1;
                printf("y calculado = %d\n", y);

                if (y == x)
                   sprintf(buffer, "Msg(Bye,ok)");
                else
                   sprintf(buffer, "Msg(nok)");
                }

                else

                  sprintf(buffer, "Msg(nok)");

                  send_message(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);


                ++server_state;
                break;

            default:
                printf("Fim do cara ou coroa\n");
                return;
        }
    }
}

void run_client(int port)
{
    int local_socket;
    struct sockaddr_in remote_address;
    socklen_t address_size = sizeof(remote_address);

    printf("=== head_or_tail cliente ===\n");

    local_socket = open_socket(DEFAULT_IP, 0);
    set_address(&remote_address, DEFAULT_IP, port);
    run_client_loop(local_socket, &remote_address, &address_size);
    close_socket(local_socket);
}

void run_client_loop(int local_socket, struct sockaddr_in *remote_address, socklen_t *address_size)
{
    int client_state = 0;
    char buffer[BUFFER_SIZE];
    int y, S1, xXorS,S,x;

    printf("Inicializando cliente\n");

    while (1)
    {
        printf("\nCliente FSM: %d\n", client_state);

        switch (client_state)
        {
            case 0:
                sprintf(buffer, "Hello");
                send_message(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                receive_message(local_socket, buffer, remote_address, address_size);

                if (strcmp(buffer, "Hello(nok)") == 0)
                    client_state = -1;
                else
		            sscanf(buffer, "Msg(%d)", &xXorS);
                    printf("x XOR S: %d\n", xXorS);
                    ++client_state;

                break;

            case 1:

		        y = (int)(rand()%2);
		        S1 = rand();
		        printf("S': %d\n",S1);
		        sprintf(buffer, "Msg(%d)", (y ^ S1));
		        printf("y: %d\n", y);
		        printf("y XOR S': %d\n",(y ^ S1));
                send_message(local_socket, buffer, strlen(buffer), remote_address, *address_size);

                ++client_state;

                receive_message(local_socket, buffer, remote_address, address_size);
                sscanf(buffer, "Msg(%d)", &S);
                printf("S recebido: %d\n", S);
                break;

            case 2:


                x = xXorS ^ S;
                printf("x calculado = %d\n", x);

                if (y == x)
                    sprintf(buffer, "Msg(S':%d,ok)",S1);
                else
                    sprintf(buffer, "Msg(nok)");
                send_message(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                receive_message(local_socket, buffer, remote_address, address_size);

                ++client_state;
                break;

            default:
                printf("Fim do cara e coroa\n");
                return;
        }
    }
}

int open_socket(char *ip, int port)
{
    struct sockaddr_in address;
    int socket_id;
    int bind_ret;

    printf("Criando socket para endereço %s:%d\n", ip, port);

    socket_id = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_id < 0)
        error("Erro ao abrir o socket");

    set_address(&address, ip, port);

    bind_ret = bind(socket_id, (struct sockaddr *)&address, sizeof(address));
    if (bind_ret < 0)
        error("Não foi possível abrir a porta %d", port);

    printf("Socket criado (id: %d)\n", socket_id);

    return socket_id;
}

void set_address(struct sockaddr_in *address, char *ip, int port)
{
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(ip);
    address->sin_port = htons(port);
}

void close_socket(int socket_id)
{
    printf("Fechando socket (id: %d)\n", socket_id);
    close(socket_id);
    printf("Socket fechado\n");
}

void receive_message(int local_socket, char *buffer, struct sockaddr_in *remote_address, socklen_t *address_size)
{
    int msg_size;

    printf("Esperando por mensagem\n");

    msg_size = recvfrom(local_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)remote_address, address_size);
    if (msg_size > 0)
    {
        buffer[msg_size] = '\0';
        printf("Mensagem recebida de %s:%d:\n"
               "%s\n",
               inet_ntoa(remote_address->sin_addr), ntohs(remote_address->sin_port), buffer);
    }
    else
    {
        error("Mensagem recebida incorretamente\n"
              "Mensagem: %s\n"
              "Tamanho: %d",
              buffer, msg_size);
    }
}

void send_message(int local_socket, char *buffer, int message_size, struct sockaddr_in *remote_address, socklen_t remote_address_size)
{
    int send_ret;

    printf("Enviando mensagem para %s:%d:\n"
           "%s\n",
           inet_ntoa(remote_address->sin_addr), ntohs(remote_address->sin_port), buffer);

    send_ret = sendto(local_socket, buffer, message_size, 0, (struct sockaddr *)remote_address, remote_address_size);

    if (send_ret == -1)
        error("Erro ao enviar mensagem\n"
              "Buffer: %s\n"
              "Message size: %d\n"
              "Número do erro: %d",
              buffer, message_size, errno);

    printf("Mensagem enviada\n");
}

