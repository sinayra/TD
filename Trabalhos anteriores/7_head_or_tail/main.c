/*Universidade de Brasilia
* Disciplina: Transmissao de Dados
* Aline Lais Gomes Tavares
* Pedro Henrique Leal
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "udp-recv.h"
#include "udp-send.h"
#include "global.h"

int usedport = 22222;
char localXOR_server[20], S_Server[20];
char localXOR_cliente[20], S_Cliente[20];


int main (int argc, char *argv[]) {

	if (argc == 3) {
	usedport = atoi(argv[2]);
	} 
	if (strcmp(argv[1], "-s") == 0) {  /*LADO DO SERVIDOR*/
		udpServidor();
	} else if (strcmp(argv[1], "host") == 0) { /*LADO DO CLIENTE*/
		udpCliente();
	} else {
		printf ("Execute o programa e escolhe entre servidor (-s) ou cliente (host)\n");
	}

	return 0;
}
