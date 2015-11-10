/*
 * conexao.c
 *
 *  Created on: Nov 25, 2014
 *      Author: guilherme
 */



#include "conexao.h"

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

void recebeMensagem(int local_socket, char *buffer, struct sockaddr_in *remote_address, socklen_t *address_size)
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

void enviaMensagem(int local_socket, char *buffer, int message_size, struct sockaddr_in *remote_address, socklen_t remote_address_size)
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

