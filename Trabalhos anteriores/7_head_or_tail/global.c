/*Contem funcoes utilizadas pelas duas partes*/
#include <stdlib.h>
#include <stdio.h>
#include "global.h"
#include <string.h>
#include <time.h>

char randomchar(int i) {
	srand((unsigned)time(NULL)+i);
	int aux = (rand() % (90+1-65))+65;
	int c = aux;
   return (c);
}

void gerarS (char S[]) {
	int x, i;
	char c, temp[5];

	for (i=0; i<4; i++){
		temp[i] = randomchar(i);
	}
	temp [i] = '\0';
	strcpy (S, temp);
}

char* xorencrypt(char message[], char key[]) {
	char aux[20];
	strcpy (aux, message);
    int messagelen = strlen(aux);
    int keylen = strlen(key);
    char *encrypted = calloc(messagelen, sizeof(char));
    int i;

    for(i = 0; i < messagelen; i++) {
        encrypted[i] = aux[i] ^ key[i % keylen];
    }
    encrypted[messagelen] = '\0';
    return encrypted;
}


void checarValores (char *x, char *y, char *result) {
	if (strcmp(x,y) == 0) {
	strcpy(result, "OK");
	} else {
	strcpy(result, "NOK");
	}

}

