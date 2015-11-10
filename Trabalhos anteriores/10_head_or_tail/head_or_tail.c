/*
Universidade de Brasília
Transmissão de dados
Trabalho Prático - Head or Tail

11/0077067 - André Luiz Vidal Giampaolo
11/0035771 - Marcelo Henrique Rios dos Reis Filho
09/0142748 - Ytalo Carvalho

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
#include <unistd.h>

//incluindo arquivo de cabeçalho - interface
#include "header_file.h"

//método principal
int main(int argc, char **argv)
{
    srand(time(NULL));
    
    //inicia a porta padrão
    int porta = PORTA_PADRAO;
       
    //verifica se quantidade de argumentos informados é valido
    if ((argc != 2) && (argc != 3)){
        printf(
                "%s\n%s\n%s\n%s\n%s\n",
                "A quantidade de argumentos informados não equivale a quantidade esperada",
                "O programa deve ser executado da seguinte maneira:",
                "./nome_programa -S [port]",
                "ou",
                "./nome_programa host [port]"
              );
    }
    
    
    //verifica se ele informou uma porta
    //para isto ocorrer a quantidade de argumentos deve ser diferente de 3
    //argc | argv
    //1 | argv[0] - nome do programa
    //2 | argv[1] - apenas a definição de servidor (-S) ou cliente (host)
    //3 | argv[2] - número da porta
    
    //a quantidade de argumentos e 3?
    if (argc == 3){
        porta = atoi(argv[2]);
    }
    
    //verifica se o argumento enviado é -S (criar o servidor)
    if (strcmp(argv[1], "-S") == 0){
        printf("CRIANDO O SERVIDOR\n");
        executarServidor(porta);
    } else if (strcmp(argv[1], "host") == 0) {
        printf("CRIANDO UM CLIENTE\n");
        executarCliente(porta);
    } else {
        printf(
                "%s\n%s\n%s\n%s\n%s\n",
                "A quantidade de argumentos informados não equivale a quantidade esperada",
                "O programa deve ser executado da seguinte maneira:",
                "./nome_programa -S [port]",
                "ou",
                "./nome_programa host [port]"
              );
    }    
    
    
    return 0;  
   
}


//executa o servidor
void executarServidor(int porta)
{
    //inicia o socket
    int idSocketLocal = abrirSocket(IP_PADRAO, porta);
    printf("Identificador do Servidor: %d\n",idSocketLocal);
    //executa as ações do servidor
    controlerServidor(idSocketLocal);
    //fecha o socket
    close(idSocketLocal);

}

void controlerServidor(int local_socket)
{
    
    //variável que receberá um valor aleatório 0 ou 1
    //0 - selecionará HEAD
    //1 - selecionará TAIL:
    int headOrTail = 0;
    //variavel que recebera o texto 'HEAD' ou 'TAIL'
    char textHeadOrTail[10];
    
    
    int server_state = 0;
    char buffer[BUFFER_SIZE];
    char *tok;
    struct sockaddr_in remote_address;
    socklen_t address_size = sizeof(remote_address);
    int S[32], x[32], MSG1[32], i=0, quo, Saux;
    unsigned long long int aux2;

    
    
    
    srand (time(NULL));
    //selecionando um valor aleatório para HEAD ou TAIL
    headOrTail = rand() % 2;
    
        
    for(i = 0; i<32; i++){
        S[i] = 0;
    }

    /* initialize random seed: */
    
    aux2 = rand() % VALOR_S;
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
    
    
    
    if(headOrTail == 1){
        strcpy(textHeadOrTail, "HEAD");
        //define a variável com os valores de HEAD em binário
        arrayHEAD(x);
    } else {
        strcpy(textHeadOrTail, "TAIL"); 
        //define a variável com os valores de TAIL em binário
        arrayTAIL(x);
    }

    printf("Inicializando servidor\n");
     
    while (1)
    {
        printf("\nCenario Servidor: %d\n", server_state);
        
        switch (server_state)
        {
            case 0:

                receberMensagem(local_socket, buffer, &remote_address, &address_size);
                printf("x: %s, S: %d\nS - binario:\n",textHeadOrTail, Saux);
                
                for(i = 0; i<32; i++)
                {
                    printf("%d ", S[i]);
                    if(i == 7 || i == 15 || i == 23 || i == 31){
                        printf("\n");
                    }
                }
                printf("\nX - binario:\n");

                for(i = 0; i<32; i++)
                {
                    printf("%d ", x[31 - i]);
                    if(i == 7 || i == 15 || i == 23 || i == 31){
                        printf("\n");}
                }
                printf("\nMensagem:\n");

                for(i = 0; i<32; i++)
                {
                    MSG1[i] = x[31-i] ^ S[i];
                    printf("%d ", MSG1[i]);
                    if(i == 7 || i == 15 || i == 23 || i == 31){
                        printf("\n");}
                }
                printf("\n");

                sprintf(buffer, "%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n", MSG1[0], MSG1[1],MSG1[2], MSG1[3],MSG1[4], MSG1[5],MSG1[6], MSG1[7],MSG1[8], MSG1[9],MSG1[10], MSG1[11],MSG1[12], MSG1[13],MSG1[14], MSG1[15], MSG1[16], MSG1[17],MSG1[18], MSG1[19],MSG1[20], MSG1[21],MSG1[22], MSG1[23],MSG1[24], MSG1[25],MSG1[26], MSG1[27],MSG1[28], MSG1[29],MSG1[30], MSG1[31]);

                enviarMensagem(local_socket, buffer, strlen(buffer), &remote_address, address_size);
                ++server_state;
                
                break;
	    case 1:
                receberMensagem(local_socket, buffer, &remote_address, &address_size);

                sprintf(buffer, "%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n", S[0], S[1],S[2], S[3],S[4], S[5],S[6], S[7],S[8], S[9],S[10], S[11],S[12], S[13],S[14], S[15], S[16], S[17],S[18], S[19],S[20], S[21],S[22], S[23],S[24], S[25],S[26], S[27],S[28], S[29],S[30], S[31]);
                printf("Enviando S\n");
                enviarMensagem(local_socket, buffer, strlen(buffer), &remote_address, address_size);

                ++server_state;
                
                break;	
	    case 2:
                receberMensagem(local_socket, buffer, &remote_address, &address_size);

                tok = strtok(buffer, " ");
                if((strcmp(tok, "OK")) == 0){
                    sprintf(buffer, "OK");
                }else{
                    sprintf(buffer, "NOK");
                }				
                enviarMensagem(local_socket, buffer, strlen(buffer), &remote_address, address_size);

            	++server_state;
                
                break;	
            default:
                printf("Fim da troca de chaves\n");
                return;
        }
    }
}

void executarCliente(int port)
{
    int idSocketLocal;
    struct sockaddr_in remote_address;
    socklen_t address_size = sizeof(remote_address);
    
    idSocketLocal = abrirSocket(IP_PADRAO, 0);
    
    //definindo a configuração de endereço do socket
    remote_address.sin_family = AF_INET;
    remote_address.sin_addr.s_addr = inet_addr(IP_PADRAO);
    remote_address.sin_port = htons(port);
    
    controleCliente(idSocketLocal, &remote_address, &address_size);
    
    //fecha o socket
    close(idSocketLocal);
    
}

void controleCliente(int local_socket, struct sockaddr_in *remote_address, socklen_t *address_size)
{
    
    //variável que receberá um valor aleatório 0 ou 1
    //0 - selecionará HEAD
    //1 - selecionará TAIL:
    int headOrTail = 0;
    //variavel que recebera o texto 'HEAD' ou 'TAIL'
    char textHeadOrTail[10];
    
    
    int cenarioCliente = 0;
    
    
    
    
    
    char buffer[BUFFER_SIZE];
    char yaux[10], *tok, *tok2;
    int y[32], S2[32], S[32],MSG2[32], MSG1[32], MSGVer[32], aux, aux2, i, Saux, quo, NotOk;
    
    
    //criando uma semente para ser randomica
    srand (time(NULL));
    //selecionando um valor aleatório para HEAD ou TAIL
    headOrTail = rand() % 2;
    
    
    while (1)
    {
        printf("\nCenario cliente: %d\n", cenarioCliente);
        
        switch (cenarioCliente)
        {
            case 0:
                sprintf(buffer, "Hello");
                enviarMensagem(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                ++cenarioCliente;
                break;
            case 1:
                receberMensagem(local_socket, buffer, remote_address, address_size);

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

                //srand (time(NULL));
                aux2 = rand() % VALOR_S;
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
                    
            
            
                if(headOrTail == 1){
                    strcpy(yaux, "HEAD");
                    //define a variável com os valores de HEAD em binário
                    arrayHEAD(y);
                } else {
                    strcpy(yaux, "TAIL"); 
                    //define a variável com os valores de TAIL em binário
                    arrayTAIL(y);
                }
            
                

                printf("y: %s, S: %d\nS - binario:\n",yaux, Saux);

                for(i = 0; i<32; i++)
                {
                    printf("%d ", S2[i]);
                    if(i == 7 || i == 15 || i == 23 || i == 31){
                        printf("\n");}
                }
                printf("\nY- binario:\n");
                for(i = 0; i<32; i++)
                {
                    printf("%d ", y[31 - i]);
                    if(i == 7 || i == 15 || i == 23 || i == 31){
                        printf("\n");}
                }
                printf("\nMensagem 2:\n");

                for(i = 0; i<32; i++)
                {
                    MSG2[i] = y[31-i] ^ S2[i];
                    printf("%d ", MSG2[i]);
                    if(i == 7 || i == 15 || i == 23 || i == 31){
                        printf("\n");}
                    }

                printf("\n");

                sprintf(buffer, "%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d %d %d %d %d %d\n", MSG2[0], MSG2[1],MSG2[2], MSG2[3],MSG2[4], MSG2[5],MSG2[6], MSG2[7],MSG2[8], MSG2[9],MSG2[10], MSG2[11],MSG2[12], MSG2[13],MSG2[14], MSG2[15], MSG2[16], MSG2[17],MSG2[18], MSG2[19],MSG2[20], MSG2[21],MSG2[22], MSG2[23],MSG2[24], MSG2[25],MSG2[26], MSG2[27],MSG2[28], MSG2[29],MSG2[30], MSG2[31]);

                enviarMensagem(local_socket, buffer, strlen(buffer), remote_address, *address_size);


   			++cenarioCliente;
			break;
	    case 2:
		receberMensagem(local_socket, buffer, remote_address, address_size);

		tok = strtok (buffer," ");
		i=0;		
		while (tok != NULL)
  		{
    			S[i] = atoi(tok);
    			tok = strtok (NULL, " \n\0");
			i++;
 		}
		printf("\nMensagem:\n");

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
		enviarMensagem(local_socket, buffer, strlen(buffer), remote_address, *address_size);
                ++cenarioCliente;
		break;
	    case 3:
		receberMensagem(local_socket, buffer, remote_address, address_size);
		++cenarioCliente;
		break;
		

            default:
                printf("Fim da troca de chaves\n");
                return;
        }
    }
}

int abrirSocket(char *ip, int porta)
{
    struct sockaddr_in endereco;
    int socketId; //identificador do socket
    int retornoPortaSocket;
    
    printf("Endreço %s:%d\n", ip, porta);
    
    /*
    Primeiro parametro
        Tipo de familia de socket
        AF_INET - ARPA INTERNET PROTOCOLS
        AF_UNIX - UNIX INTERNET PROTOCOLS
        AF_ISSO - ISO PROTOCOLS
        AF_NS   - XEROX NETWORK SYSTEM PROTOCOLS
    Segundo parametro
        Tipo de socket
        TCP (Stream Sockets) - SOCK_STREAM 
        UDP (Datagram Sockets) - SOCK_DGRAM
    Terceiro parametro
        Número do protocolo
        0  - IP - INTERNET PROTOCOL
        1  - ICMP - INTERNET CONTROL MESSAGE PROTOCOL2 - IGMP - INTERNET GROUP MULTICAST PROTOCOL
        3  - GGP - GATEWAY-GATEWAY PROTOCOL
        6  - TCP - TRANSMISSION CONTROL PROTOCOL
        17 - UDP - USER DATAGRAMA PROTOCOL
    */
    socketId = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (socketId < 0){
        printf("Serviço indisponível no momento: erro ao abrir o socket");
    }

    //definindo a configuração de endereço do socket
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = inet_addr(ip);
    endereco.sin_port = htons(porta);
    
    
    //bind: função serve para associar uma porta em sua máquina local para o seu socket
    retornoPortaSocket = bind(socketId, (struct sockaddr *)&endereco, sizeof(endereco));
    if (retornoPortaSocket < 0){
        printf("Porta %d indisponível", porta);
    }
    
    return socketId;
}

void receberMensagem(int local_socket, char *buffer, struct sockaddr_in *remote_address, socklen_t *address_size)
{
    //sleep(DORMIR);
    int msg_size;
    
    printf("Esperando por mensagem\n");
    
    msg_size = recvfrom(local_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)remote_address, address_size);
    if (msg_size > 0) {
        buffer[msg_size] = '\0';
        printf("%c[1;34m", 27); // verde
        printf("Mensagem recebida de %s:%d:\n"
               "%s\n",
               inet_ntoa(remote_address->sin_addr), ntohs(remote_address->sin_port), buffer);
        printf("%c[1;30m", 27); // preto
    } else {
        printf("Mensagem recebida incorretamente\n Mensagem: %s\n Tamanho: %d",buffer, msg_size);
    }
}

void enviarMensagem(int local_socket, char *buffer, int message_size, struct sockaddr_in *remote_address, socklen_t remote_address_size)
{
    //sleep(DORMIR);
    int send_ret;
    
    printf("%c[1;33m", 27); // amarelo
    printf("Enviando mensagem para %s:%d:\n%s\n",
           inet_ntoa(remote_address->sin_addr), ntohs(remote_address->sin_port), buffer);
    printf("%c[1;30m", 27); // preto
    

    send_ret = sendto(local_socket, buffer, message_size, 0, (struct sockaddr *)remote_address, remote_address_size);
    
    if (send_ret == -1){
        printf("Erro ao enviar mensagem\n Buffer: %s\n Tamanho da mensagem:%d\n Erro: %d",buffer, message_size, errno);
    } else {
        printf("%c[1;32m", 27); // verde
        printf("Mensagem enviada com sucesso\n");
        printf("%c[1;30m", 27); // preto
        
    }
    
}

//define a variável com os valores de HEAD em binário
void arrayHEAD(int *array){
 
    //H
    array[31] = 0;array[30] = 1;array[29] = 0;array[28] = 0;
    array[27] = 1;array[26] = 0;array[25] = 0;array[24] = 0;

    //E
    array[23] = 0;array[22] = 1;array[21] = 0;array[20] = 0;
    array[19] = 0;array[18] = 1;array[17] = 0;array[16] = 1;

    //A
    array[15] = 0;array[14] = 1;array[13] = 0;array[12] = 0;
    array[11] = 0;array[10] = 0;array[9] = 0;array[8] = 1;

    //D
    array[7] = 0;array[6] = 1;array[5] = 0;array[4] = 0;
    array[3] = 0;array[2] = 1;array[1] = 0;array[0] = 0;
    
    
}
//define a variável com os valores de TAIL em binário
void arrayTAIL(int *array){
    //T
    array[31] = 0;array[30] = 1;array[29] = 0;array[28] = 1;
    array[27] = 0;array[26] = 1;array[25] = 0;array[24] = 0;

    //A
    array[23] = 0;array[22] = 1;array[21] = 0;array[20] = 0;
    array[19] = 0;array[18] = 0;array[17] = 0;array[16] = 1;

    //I
    array[15] = 0;array[14] = 1;array[13] = 0;array[12] = 0;
    array[11] = 1;array[10] = 0;array[9] = 0;array[8] = 1;

    //L
    array[7] = 0;array[6] = 1;array[5] = 0;array[4] = 0;
    array[3] = 1;array[2] = 1;array[1] = 0;array[0] = 0;
}