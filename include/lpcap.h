#pragma once

#include <stdlib.h>
#include <pcap.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

typedef struct {
	string dev;					  				/*Dispositivo*/
	pcap_t *handle;				              	/*packet capture handle: o pacote para capturar pacote*/
	string filter_exp; 	  						/*Expressão de filtro*/
	struct bpf_program fp;					  	/*Expressão de filtro compilada*/
	bpf_u_int32 mask;						  	/*Máscara de Subrede*/
	bpf_u_int32 net;						  	/*Ip da rede*/
	struct pcap_pkthdr header;					/*Cabeçalho fornecido do pcap*/
	string package;								/*O pacote mesmo */

}lpcapType;

lpcapType lpcap_init(string port); //Seta headers
void lpcap_process(lpcapType p); //Captura mensagem e retorna string da mensagem
void lpcap_free(lpcapType p); //Libera recursos de lpcap
