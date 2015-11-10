/*Lado do Servidor*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "udp-recv.h"
#include "global.h"

#define MSGLEN 100
extern int usedport;
extern char localXOR_server[20];
extern char localXOR_cliente[20];
extern char S_Cliente[20];
extern char S_Server[20];

void randHeadTailServer (char *escolha) {
	int x;

	srand( (unsigned)time(NULL));
	x = rand() % 2;

	if (x == 0) {
		strcpy (escolha, "HEAD");
	} else {
		strcpy (escolha, "TAIL");
	}
}


void udpServidor() {
	struct sockaddr_in end_local;	/* endereco local */
	struct sockaddr_in end_dest;	/* endereco do cliente */
	socklen_t tamanho = sizeof(end_dest);		/* tamanho do endereco destino */
	int recvlen;			/* # bytes recebido */
	int sockt1;				/* o socket do servidor */
	char msg_recebe[MSGLEN], msg_envia[MSGLEN];
	char escolha[7], ClienteXOR[7], resultado[7], OK[4];
	char aux[7], x[5], y[5];


	/* cria um socket UDP */

	if ((sockt1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket nao criado.\n");
	
	}

	/* bind o socket */

	memset((char *)&end_local, 0, sizeof(end_local));
	end_local.sin_family = AF_INET;
	end_local.sin_addr.s_addr = htonl(INADDR_ANY);
	end_local.sin_port = htons(usedport);

	if (bind(sockt1, (struct sockaddr *)&end_local, sizeof(end_local)) < 0) {
		perror("bind falhou.\n");
	
	}
	
	printf (" ################## SERVIDOR ################## \n");   
	printf ("Servidor diz: Esperando mensagem na porta %d\n", usedport);
	for (;;) {
		/*1. Recebe Hello*/
		recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
		printf ("\nCliente diz: %s\n",msg_recebe);

		/*2. Escolhe head or tail aleatoriamente e envia com X XOR S */ 
		randHeadTailServer(escolha);
		strcpy(x, escolha);
		gerarS(S_Server);
		printf ("\nHEAD OR TAIL: %s\n", escolha);
		strcpy (aux, escolha);
		strcpy (localXOR_server, xorencrypt(escolha, S_Server));
		printf ("\nCliente diz: y XOR S' enviado. \n");
		strncpy((char *)msg_envia, S_Server, sizeof(msg_envia));
		sendto (sockt1, msg_envia, sizeof(msg_envia), 0, (struct sockaddr*)&end_dest, tamanho);
		strncpy((char *)msg_envia, escolha, sizeof(msg_envia));
		sendto (sockt1, msg_envia, sizeof(msg_envia), 0, (struct sockaddr*)&end_dest, tamanho);
		/*4. Recebe o verdadeiro S', calcula o valor de x*/
		recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
		printf ("\nCliente diz: %s\n",msg_recebe); 
		recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
		msg_recebe[4] = '\0';
		printf ("\nCliente diz: S' = %s\n",msg_recebe); 
		recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
		strcpy (y, msg_recebe);
		/*5. Checagem de x = y e envia S'*/
		strcpy(resultado, xorencrypt(localXOR_cliente, S_Cliente));
		checarValores (x,y, OK);
		strncpy((char *)msg_envia, OK, sizeof(msg_envia));		
		sendto (sockt1, msg_envia, sizeof(msg_envia), 0, (struct sockaddr*)&end_dest, tamanho);

		strncpy((char *)msg_envia, "bye", sizeof(msg_envia));
		sendto (sockt1, msg_envia, sizeof(msg_envia), 0, (struct sockaddr*)&end_dest, tamanho);
	}	 
	/*loop infinito do servidor*/
	close(sockt1);
}
