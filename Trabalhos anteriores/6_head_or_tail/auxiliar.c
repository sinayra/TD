/*
	* AUXILIAR
	* 
	* Implementa funcoes auxiliares ao cliente e ao servidor
	* 
    * Gessica Neves Sodre da Silva - 11/0146115
    * Andreia de Carvalho - 10/0025153
	* 25/11/2014
	* 
*/
#include "auxiliar.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

//#define HEAD (72 * 11) + (69 * 13) + (65 * 17) + (68 * 19)
//#define TAIL (84 * 11) + (65 * 15) + (73 * 17) + (76 * 19)


/* FUNCAO initSRand -----------------------------------*/
void initSRand(int a){
	/* inicializa srand */
	srand (a);
}

/* FUNCAO headOrTail ---------------------------------
 * 0 = HEAD ; 1 = TAIL
 * Multiplico o valor na tabela ascii de cada letra pela 
 * sequencia de primos 11, 13, 17, 19, respectivamente 
 * definidos acima
 * ---------------------------------------------------*/
 int headOrTail(){
 	int aleatorio;
 	aleatorio = rand() % 2;

 	if (aleatorio == 0){ /* head */
 		//printf("HEAD\n");
 		return HEAD;
 	}else{ /* tail */
 		//printf("TAIL\n");
 		return TAIL;
 	}
 }

/* FUNCAO sequencia ----------------------------------
 * retorna uma sequencia de tamanho S_SIZE 
 * --------------------------------------------------*/
int sequencia(){
	int randomico = 0;
	//printf("RAND = %d\n", rand());
 	randomico = rand() % RAND_MAX + 1;
 	//printf("SEQUENCIA %d\n", randomico);
	return randomico;
}

/* FUNCAO CIFRAR USANDO XOR --------------------------*/
int xorcipher(int cara_ou_coroa, int sequencia){
	return (cara_ou_coroa ^ sequencia);
}

/* FUNCAO inicializa-----------------------------------*
 * ----------------------------------------------------*/
void inicializa (char *c_auxC) {
	int i;
	
	for (i=0; i<100; i++) {	
		c_auxC[i] = 0;
	}
}	

/* FUNCAO buffParaInteiro ------------------------------*
 * -----------------------------------------------------*/
int buffParaInteiro(void *buffer){
	char *ptr, auxC[100], *paux, c;
	int i = 0;

	inicializa(auxC);
	ptr = strchr(buffer, '(');
	if (*ptr){
		ptr++;
		paux = ptr;
		ptr = strchr(ptr, ')');
		i = 0;
		
		while (paux < ptr){
			c = *paux;
			auxC[i] = c;
			i++;
			paux++;
			 
		}
		return atoi(auxC);
	}
	return 0;
}