/*
 * head_or_tail.c
 *
 *  Created on: Nov 25, 2014
 *      Author: guilherme
 */

#include "conexao.h"

#define HEAD 0
#define TAIL 1

int parse_arguments(int argc, char **argv, int *port);
void executarAplicacaoServidor(int port);
void executarAplicacaoServidor_loop(int local_socket);
void executarAplicacaoCliente(int port);
void executarAplicacaoCliente_loop(int local_socket, struct sockaddr_in *remote_address, socklen_t *address_size);




int main(int argc, char **argv)
{
    int port;
    int program_type = parse_arguments(argc, argv, &port);

    srand(time(NULL));

    if (program_type == 1)
        executarAplicacaoServidor(port);
    else if (program_type == 2)
        executarAplicacaoCliente(port);
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


void executarAplicacaoServidor(int port)
{
    int local_socket;

    printf("Aplicação head_or_tail - Modo Servidor selecionado.\n");

    local_socket = open_socket(DEFAULT_IP, port);
    executarAplicacaoServidor_loop(local_socket);
    close_socket(local_socket);
}

void executarAplicacaoServidor_loop(int local_socket)
{
    int server_state = 0;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in remote_address;
    socklen_t address_size = sizeof(remote_address);
    int S, x, yXorS1, S1,y;

    printf("Inicializando servidor\n");

    while (1)
    {
        printf("\n\n");

        switch (server_state)
        {
            case 0:
                recebeMensagem(local_socket, buffer, &remote_address, &address_size);

               if (strcmp(buffer,"Hello"))
                {
                    sprintf(buffer, "Hello(nok)");
                    enviaMensagem(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);
                    server_state = -1;
                }
                else
                {
                    x = (int)(rand()%2);
					S = rand();
					printf("S: %x\n",S);
					sprintf(buffer, "Msg(%d)", (x ^ S));

					if(x==HEAD)
					printf("x: HEAD\n");
					else
					printf("x: TAIL\n");

				    printf("x XOR S: %x\n",(x ^ S));
                    enviaMensagem(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);
                    ++server_state;
                }
                break;

            case 1:


                recebeMensagem(local_socket, buffer, &remote_address, &address_size);
                sscanf(buffer, "Msg(%d)", &yXorS1);
                printf("y XOR S': %x\n", yXorS1);
                sprintf(buffer, "Msg(%d)", S);
		        printf("S: %x\n", S);
                enviaMensagem(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);
                ++server_state;
                break;

            case 2:



                recebeMensagem(local_socket, buffer, &remote_address, &address_size);

                if(strcmp(buffer, "Msg(nok)") != 0){


                sscanf(buffer, "Msg(S':%d,ok)", &S1);
                printf("S' recebido: %x\n", S1);

                y = yXorS1 ^ S1;

            	if(y==HEAD)
            	   printf("y calculado = HEAD\n");
            	else
            	   printf("y calculado = TAIL\n");

                if (y == x)
                   sprintf(buffer, "Msg(Bye,ok)");
                else
                   sprintf(buffer, "Msg(nok)");
                }

                else

                  sprintf(buffer, "Msg(nok)");

                  enviaMensagem(local_socket, &buffer[0], strlen(buffer), &remote_address, address_size);


                ++server_state;
                break;

            default:
                return;
        }
      sleep(10);
    }
}

void executarAplicacaoCliente(int port)
{
    int local_socket;
    struct sockaddr_in remote_address;
    socklen_t address_size = sizeof(remote_address);

    printf("Aplicação head_or_tail - Modo Cliente selecionado.\n");

    local_socket = open_socket(DEFAULT_IP, 0);
    set_address(&remote_address, DEFAULT_IP, port);
    executarAplicacaoCliente_loop(local_socket, &remote_address, &address_size);
    close_socket(local_socket);
}

void executarAplicacaoCliente_loop(int local_socket, struct sockaddr_in *remote_address, socklen_t *address_size)
{
    int client_state = 0;
    char buffer[BUFFER_SIZE];
    int y, S1, xXorS,S,x;

    printf("Inicializando cliente\n");

    while (1)
    {
        printf("\n\n");

        switch (client_state)
        {
            case 0:
                sprintf(buffer, "Hello");
                enviaMensagem(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                recebeMensagem(local_socket, buffer, remote_address, address_size);

                if (strcmp(buffer, "Hello(nok)") == 0)
                    client_state = -1;
                else
		            sscanf(buffer, "Msg(%d)", &xXorS);
                    printf("x XOR S: %x\n", xXorS);
                    ++client_state;

                break;

            case 1:

		        y = (int)(rand()%2);
		        S1 = rand();
		        printf("S': %x\n",S1);
		        sprintf(buffer, "Msg(%d)", (y ^ S1));

		        if(y==HEAD)
		          printf("y: HEAD\n");
		        else
		          printf("y: TAIL\n");


		        printf("y XOR S': %x\n",(y ^ S1));

                enviaMensagem(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                recebeMensagem(local_socket, buffer, remote_address, address_size);

                sscanf(buffer, "Msg(%d)", &S);
                printf("S recebido: %x\n", S);
                ++client_state;
                break;

            case 2:


                x = xXorS ^ S;

                if(x==HEAD)
                  printf("x calculado = HEAD\n");
                else
                  printf("x calculado = TAIL\n");


                if (y == x)
                    sprintf(buffer, "Msg(S':%d,ok)",S1);
                else
                    sprintf(buffer, "Msg(nok)");

                enviaMensagem(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                recebeMensagem(local_socket, buffer, remote_address, address_size);

                ++client_state;
                break;

            default:
                return;
        }

        sleep(10);
    }
}











