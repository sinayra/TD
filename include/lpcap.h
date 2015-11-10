#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <string>
#include <iostream>

using namespace std;

typedef struct {
	string dev;					  				/*Dispositivo*/
	pcap_t *handle;				              	/*packet capture handle: o pacote para capturar pacote*/
	string filter_exp; 	  						/*Expressão de filtro*/
	struct bpf_program fp;					  	/*Expressão de filtro compilada*/
	bpf_u_int32 mask;						  	/*Máscara de Subrede*/
	bpf_u_int32 net;						  	/*Ip da rede*/
}lpcapType;

lpcapType lpcap_init(string port); //Seta headers
void lpcap_process(lpcapType p); //Captura mensagem
void lpcap_free(); //Libera recursos de lpcap
