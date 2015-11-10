/* Trabalho de Transmissão de Dados - 2°/2014 - UnB */
/* João A. Ribeiro 		12/0014491 */
/* Pedro Paulo S. Lima 	11/0073983 */

/* Instruções para compilar e executar: 
compilar: gcc -Wall decifrador_XOR.c -o decifrador
executar: ./decifrador */

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <string.h>     /* strlen, strcpy */
#include <time.h>       /* time */

#define HEAD 1
#define TAIL 2
#define TAM_VET 5

/* XOR DECYPHER 
	 
	 	01010111 01101001 01101011 01101001 -> ESCOLHA
		11110011 11110011 11110011 11110011 -> CHAVE 
	XOR ___________________________________
	  = 10100100 10011010 10011000 10011010 -> mensagem encriptada
	 
	 
	 
	 	10100100 10011010 10011000 10011010 -> mensagem encriptada
		11110011 11110011 11110011 11110011 -> CHAVE
	XOR ___________________________________
	  = 01010111 01101001 01101011 01101001 -> ESCOLHA ORIGINAL.

	  
		HEAD = 72       69       65       68 
			 = 01001000 01000101 01000001 01000100
		
		TAIL = 84       65       73       76
			 = 01010100 01000001 00000000 00000000
			 
*/

void gera(char escolha[], char chave[]) {
	/*Gerar randomicamente x e S, onde x pode ser {1=HEAD,2=TAIL} e sequencia possui 4 bytes*/
	/*y e S',onde y pode ser {1=HEAD,2=TAIL} e sequencia' possui 4 bytes*/

	static const char alphanum[] =
        "0123456789"
        "BCFGJKMNOPQRSUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

	int i;
	
	/* iniacializa com uma semente aleatória: */
	srand (time(NULL));
	i = rand() % 2 + 1; /* i recebe 1 ou 2. Head ou Tail. */
	
	if (i == HEAD) {
		strcpy(escolha, "HEAD");
	}
	else {
		strcpy(escolha, "TAIL");
	}
	
	
	for (i = 0; i < TAM_VET-1; i++) {
		chave[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	chave[i] = '\0';
}

void encripta (char escolha[], char chave[], char criptograma[]) {
	int i;
	char res_xor; /*resultado do xor para comparação.*/
	
	for (i=0; i < TAM_VET-1; i++) {
		res_xor = escolha[i]^chave[i];
		criptograma[i] = res_xor;
	}
	criptograma[i] = '\0';
}

void desencripta (char criptograma[], char chave[], char escolha[]) {
	int i;
	
	for(i=0; i<TAM_VET-1; i++) {
		escolha[i] = criptograma[i]^chave[i];
	}
	escolha[i] = '\0';
}

int main ()
{
	char x[TAM_VET];
	char S[TAM_VET];
	char criptograma[TAM_VET];
	
	gera(x, S); /* char escolha[], char chave[] */
	printf("\nGERA");
	printf("\nEscolha: [%s]", x);
	printf("\nChave  : [%s]", S);
	
	printf("\n\nEncripta");
	encripta(x, S, criptograma); /* char escolha[], char chave[], char criptograma[] */
	printf("\nEscolha    : [%s]", x);
	printf("\nChave      : [%s]", S);
	printf("\nCriptograma: [%s]", criptograma);
	printf("\nTamanho do criptograma: %d bytes", strlen(criptograma));
	
	printf("\n\nDesencripta");
	desencripta(criptograma, S, x); /* char criptograma[], char chave[], char escolha[] */
	printf("\nCriptograma: [%s]", criptograma);
	printf("\nChave      : [%s]", S);
	printf("\nEscolha    : [%s]", x);
	
	printf("\n\n");
	return 0;
}


