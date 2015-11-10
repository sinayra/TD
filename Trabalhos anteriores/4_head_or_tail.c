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
              "head_or_tail host [port]");
    
    return 0; /* Should never reach here */
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
    char xaux[10], *tok;
    struct sockaddr_in remote_address;
    socklen_t address_size = sizeof(remote_address);
    int aux = 0, S[32], x[32], MSG1[32], i=0, quo, Saux;
    unsigned long long int aux2;

    for(i = 0; i<32; i++)
		{
			S[i] = 0;
		}

  /* initialize random seed: */
    srand (time(NULL));
    aux2 = rand() % 4294967295;
    Saux = aux2;
    i = 0;
    quo = 1;
    while((quo!= 0) ){
	quo = aux2 / 2;	
	S[31-i] = (aux2 % 2) ;
	aux2 = quo;
	i++;
	}
    printf("\n");
    aux = rand() % 2;
    if(aux == 1){strcpy(xaux, "HEAD"); 
		x[31] = 0;x[30] = 1;x[29] = 0;x[28] = 0;
		x[27] = 1;x[26] = 0;x[25] = 0;x[24] = 0;

		x[23] = 0;x[22] = 1;x[21] = 0;x[20] = 0;
		x[19] = 0;x[18] = 1;x[17] = 0;x[16] = 1;

		x[15] = 0;x[14] = 1;x[13] = 0;x[12] = 0;
		x[11] = 0;x[10] = 0;x[9] = 0;x[8] = 1;

		x[7] = 0;x[6] = 1;x[5] = 0;x[4] = 0;
		x[3] = 0;x[2] = 1;x[1] = 0;x[0] = 0;}
    else{strcpy(xaux, "TAIL"); 
		x[31] = 0;x[30] = 1;x[29] = 0;x[28] = 1;
		x[27] = 0;x[26] = 1;x[25] = 0;x[24] = 0;

		x[23] = 0;x[22] = 1;x[21] = 0;x[20] = 0;
		x[19] = 0;x[18] = 0;x[17] = 0;x[16] = 1;

		x[15] = 0;x[14] = 1;x[13] = 0;x[12] = 0;
		x[11] = 1;x[10] = 0;x[9] = 0;x[8] = 1;

		x[7] = 0;x[6] = 1;x[5] = 0;x[4] = 0;
		x[3] = 1;x[2] = 1;x[1] = 0;x[0] = 0;}

    printf("Inicializando servidor\n");
     
    while (1)
    {
        printf("\nServidor FSM: %d\n", server_state);
        
        switch (server_state)
        {
            case 0:

                receive_message(local_socket, buffer, &remote_address, &address_size);
                printf("x: %s, S: %d\nS(bin):\n",xaux, Saux);
		for(i = 0; i<32; i++)
		{
			printf("%d ", S[i]);
			if(i == 7 || i == 15 || i == 23 || i == 31){
				printf("\n");}
		}
		printf("\nX(bin):\n");
		for(i = 0; i<32; i++)
		{
			printf("%d ", x[31 - i]);
			if(i == 7 || i == 15 || i == 23 || i == 31){
				printf("\n");}
		}
		printf("\nMSG1:\n");

		for(i = 0; i<32; i++)
		{
			MSG1[i] = x[31-i] ^ S[i];
			printf("%d ", MSG1[i]);
			if(i == 7 || i == 15 || i == 23 || i == 31){
				printf("\n");}
          	}
		printf("\n");

                sprintf(buffer, "%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n", MSG1[0], MSG1[1],MSG1[2], MSG1[3],MSG1[4], MSG1[5],MSG1[6], MSG1[7],MSG1[8], MSG1[9],MSG1[10], MSG1[11],MSG1[12], MSG1[13],MSG1[14], MSG1[15], MSG1[16], MSG1[17],MSG1[18], MSG1[19],MSG1[20], MSG1[21],MSG1[22], MSG1[23],MSG1[24], MSG1[25],MSG1[26], MSG1[27],MSG1[28], MSG1[29],MSG1[30], MSG1[31]);

                send_message(local_socket, buffer, strlen(buffer), &remote_address, address_size);
                ++server_state;
                
                break;
	    case 1:
		receive_message(local_socket, buffer, &remote_address, &address_size);

		sprintf(buffer, "%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n", S[0], S[1],S[2], S[3],S[4], S[5],S[6], S[7],S[8], S[9],S[10], S[11],S[12], S[13],S[14], S[15], S[16], S[17],S[18], S[19],S[20], S[21],S[22], S[23],S[24], S[25],S[26], S[27],S[28], S[29],S[30], S[31]);
		printf("Enviando S\n");
		send_message(local_socket, buffer, strlen(buffer), &remote_address, address_size);

                ++server_state;
                
                break;	
	    case 2:
		receive_message(local_socket, buffer, &remote_address, &address_size);
		
		tok = strtok(buffer, " ");
		if((strcmp(tok, "OK")) == 0){
			sprintf(buffer, "OK - Bye");
}
		else{
			sprintf(buffer, "NOK");
}				
		send_message(local_socket, buffer, strlen(buffer), &remote_address, address_size);

            	++server_state;
                
                break;	
            default:
                printf("Fim da troca de chaves\n");
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
    char yaux[10], *tok, *tok2;
    int y[32], S2[32], S[32],MSG2[32], MSG1[32], MSGVer[32], aux, aux2, i, Saux, quo, NotOk;
    
    printf("Inicializando cliente\n");
    
    while (1)
    {
        printf("\nCliente FSM: %d\n", client_state);
        
        switch (client_state)
        {
            case 0:
                sprintf(buffer, "Hello");
                send_message(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                ++client_state;
                    
                break;
            case 1:
		receive_message(local_socket, buffer, remote_address, address_size);
		
		tok2 = strtok (buffer," ");
		
		i=0;		
		while (tok2 != NULL)
  		{
    			MSG1[i] = atoi(tok2);
    			tok2 = strtok (NULL, " \n\0");
			i++;
 		}

		for(i = 0; i<32; i++)
		{
			S2[i] = 0;
		}

		srand (time(NULL));
    		aux2 = rand() % 4294967295;
    		Saux = aux2;

   

  
  
		i = 0;
		quo = 1;
   		while((quo!= 0) ){
			quo = aux2 / 2;	
			S2[31-i] = (aux2 % 2) ;
			aux2 = quo;
			i++;
			}	
		printf("\n");
    		aux = rand() % 2;
    		if(aux == 1){strcpy(yaux, "HEAD"); 
			y[31] = 0;y[30] = 1;y[29] = 0;y[28] = 0;
			y[27] = 1;y[26] = 0;y[25] = 0;y[24] = 0;

			y[23] = 0;y[22] = 1;y[21] = 0;y[20] = 0;
			y[19] = 0;y[18] = 1;y[17] = 0;y[16] = 1;

			y[15] = 0;y[14] = 1;y[13] = 0;y[12] = 0;
			y[11] = 0;y[10] = 0;y[9] = 0;y[8] = 1;

			y[7] = 0;y[6] = 1;y[5] = 0;y[4] = 0;
			y[3] = 0;y[2] = 1;y[1] = 0;y[0] = 0;}

    		else{strcpy(yaux, "TAIL"); 
			y[31] = 0;y[30] = 1;y[29] = 0;y[28] = 1;
			y[27] = 0;y[26] = 1;y[25] = 0;y[24] = 0;

			y[23] = 0;y[22] = 1;y[21] = 0;y[20] = 0;
			y[19] = 0;y[18] = 0;y[17] = 0;y[16] = 1;

			y[15] = 0;y[14] = 1;y[13] = 0;y[12] = 0;
			y[11] = 1;y[10] = 0;y[9] = 0;y[8] = 1;

			y[7] = 0;y[6] = 1;y[5] = 0;y[4] = 0;
			y[3] = 1;y[2] = 1;y[1] = 0;y[0] = 0;}

		printf("y: %s, S: %d\nS(bin):\n",yaux, Saux);
		
		for(i = 0; i<32; i++)
		{
			printf("%d ", S2[i]);
			if(i == 7 || i == 15 || i == 23 || i == 31){
				printf("\n");}
		}
		printf("\nY(bin):\n");
		for(i = 0; i<32; i++)
		{
			printf("%d ", y[31 - i]);
			if(i == 7 || i == 15 || i == 23 || i == 31){
				printf("\n");}
		}
		printf("\nMSG2:\n");

		for(i = 0; i<32; i++)
		{
			MSG2[i] = y[31-i] ^ S2[i];
			printf("%d ", MSG2[i]);
			if(i == 7 || i == 15 || i == 23 || i == 31){
				printf("\n");}
          	}

		printf("\n");

                sprintf(buffer, "%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n", MSG2[0], MSG2[1],MSG2[2], MSG2[3],MSG2[4], MSG2[5],MSG2[6], MSG2[7],MSG2[8], MSG2[9],MSG2[10], MSG2[11],MSG2[12], MSG2[13],MSG2[14], MSG2[15], MSG2[16], MSG2[17],MSG2[18], MSG2[19],MSG2[20], MSG2[21],MSG2[22], MSG2[23],MSG2[24], MSG2[25],MSG2[26], MSG2[27],MSG2[28], MSG2[29],MSG2[30], MSG2[31]);

                send_message(local_socket, buffer, strlen(buffer), remote_address, *address_size);


   			++client_state;
			break;
	    case 2:
		receive_message(local_socket, buffer, remote_address, address_size);

		tok = strtok (buffer," ");
		i=0;		
		while (tok != NULL)
  		{
    			S[i] = atoi(tok);
    			tok = strtok (NULL, " \n\0");
			i++;
 		}
		printf("\nMSGVer:\n");

		for(i = 0; i<32; i++)
		{
			MSGVer[i] = y[31-i] ^ S[i];
			printf("%d ", MSGVer[i]);
			if(i == 7 || i == 15 || i == 23 || i == 31){
				printf("\n");}
          	}
		for(i= 0 ; i<32 ; i++)
		{
			if(MSGVer[i] != MSG1[i])
			{
			NotOk = 1;
			}
		}
		
		if(NotOk == 1)
		{
		sprintf(buffer,"NOK");
}
		else
		{
		sprintf(buffer, "OK %d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n", S2[0], S2[1],S2[2], S2[3],S2[4], S2[5],S2[6], S2[7],S2[8], S2[9],S2[10], S2[11],S2[12], S2[13],S2[14], S2[15], S2[16], S2[17],S2[18], S2[19],S2[20], S2[21],S2[22], S2[23],S2[24], S2[25],S2[26], S2[27],S2[28], S2[29],S2[30], S2[31]);
}
		send_message(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                ++client_state;
		break;
	    case 3:
		receive_message(local_socket, buffer, remote_address, address_size);
		++client_state;
		break;
		

            default:
                printf("Fim da troca de chaves\n");
                return;
        }
    }
}

int open_socket(char *ip, int port)
{
    struct sockaddr_in address;
    int socket_id;
    int bind_ret;
    
    printf("Criando comunicacao para endereço %s:%d\n", ip, port);
    
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_id < 0)
        error("Erro ao abrir o socket");

    set_address(&address, ip, port);

    bind_ret = bind(socket_id, (struct sockaddr *)&address, sizeof(address));
    if (bind_ret < 0)
        error("Não foi possível abrir a porta %d", port);
    
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
   
    close(socket_id);
    
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

