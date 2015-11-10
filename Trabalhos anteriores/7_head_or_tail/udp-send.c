/*Lado do Cliente*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include "udp-send.h"
#include "global.h"

#define MSGLEN 2048

extern int usedport;
extern char localXOR_server[20];
extern char localXOR_cliente[20];
extern char S_Cliente[20];
extern char S_Server[20];

void randHeadTailCliente (char *escolha) {
	int opcao;

	srand((unsigned)time(NULL)+1);
	opcao = rand() % 2;

	if (opcao == 0) {
		strcpy (escolha, "HEAD");
	} else {
		strcpy (escolha, "TAIL");
	}
}


void udpCliente ()
{
	struct sockaddr_in end_local;	/* endereco local */
	struct sockaddr_in end_dest;	/* endereco do cliente */
	int sockt1, i;
	socklen_t tamanho = sizeof(end_dest); /*Tamanho do end de destino*/		
	char *server = "127.0.0.1";	/* IP servidor */
	char msg_recebe[MSGLEN], msg_envia[MSGLEN];	
	char escolha[5], ServidorXOR[7], localXOR[8], resultado[8], OK[4];
	char aux[30], x[5], y[5];


	/*Criando o SOCKET */
	if ((sockt1=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket criado.\n");

	/* bind */
	memset((char *)&end_local, 0, sizeof(end_local));
	end_local.sin_family = AF_INET;
	end_local.sin_addr.s_addr = htonl(INADDR_ANY);
	end_local.sin_port = htons(0);

	if (bind(sockt1, (struct sockaddr *)&end_local, sizeof(end_local)) < 0) {
		perror("bind failed");

	}       

	memset((char *) &end_dest, 0, sizeof(end_dest));
	end_dest.sin_family = AF_INET;
	end_dest.sin_port = htons(usedport);
	if (inet_aton(server, &end_dest.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}


	printf (" ################## CLIENTE ################## \n");  
	/*1. Envia Hello*/
	strncpy((char *)msg_envia, "Hello", sizeof(msg_envia));
	sendto (sockt1, msg_envia, sizeof(msg_envia), 0, (struct sockaddr*)&end_dest, sizeof(end_dest));
		
	/*2. Espera receber */ 
	printf ("\nServidor diz: X XOR S enviado.\n");

	/*3. Escolhe head or tail aleatoriamente e envia com Y XOR S' */
	randHeadTailCliente(escolha);
	strcpy(y, escolha);
	gerarS(S_Cliente);
	printf ("\nHEAD OR TAIL: %s\n", escolha);
	strcpy (localXOR_cliente, xorencrypt(escolha, S_Cliente));

	/*4. Recebe o verdadeiro S, calcula o valor de x*/
	recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
	printf ("\nServidor diz: S = %s\n",msg_recebe);
	recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
	strcpy (x, msg_recebe);

	/*5. Checagem de x = y e envia S'*/
	strcpy(resultado, xorencrypt(localXOR_server, S_Server));
	checarValores (x,y,OK);
	strncpy((char *)msg_envia, OK, sizeof(msg_envia));
	sendto (sockt1, msg_envia, strlen(msg_envia), 0, (struct sockaddr*)&end_dest, sizeof(end_dest));
	strncpy((char *)msg_envia, S_Cliente, sizeof(msg_envia));
	sendto (sockt1, msg_envia, strlen(msg_envia), 0, (struct sockaddr*)&end_dest, sizeof(end_dest));
	strncpy((char *)msg_envia, escolha, sizeof(msg_envia));
	sendto (sockt1, msg_envia, sizeof(msg_envia), 0, (struct sockaddr*)&end_dest, tamanho);

	/*6 Recebe */
	recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
	printf ("\nServidor diz: %s\n",msg_recebe);
	recvfrom (sockt1, msg_recebe, sizeof(msg_recebe), 0, (struct sockaddr*)&end_dest, &tamanho);
	printf ("\nServidor diz: %s\n",msg_recebe);

	close(sockt1);
}
