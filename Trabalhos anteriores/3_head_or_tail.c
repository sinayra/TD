/*
	Programa: Head or Tail. Trabalho pratico de programacao da disciplina de Transferencia de Dados 2- 2014.
	Autor: Vitor de Alencastro Lacerda
	Matricula: 11/0067142
	
	Autor: Vinicius Fernandes de Souza
	Matricula: 09/0135016

	
	Para compilar: $gcc head_or_tail.c -o headortail -lnet -lpcap

	Como utilizar: $headortail -S [port] - Servidor que escuta em [port]
		       $headortail -host [port] - Cliente que envia os pacotes na porta [port]
		Rodar o programa como cliente vai pedir que o usuario insira o IP da maquina que esta rodando o programa como servidor.


*/






#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <pcap.h>
#include <stdint.h>
#include <string.h>
	
#define SIZE_ETHERNET	14
#define SIZE_UDP        8  
#define SRC_PORT  111
#define HOST_PORT 1010


	/*Structs para dividir o pacote recebido. Grande parte dessas structs ficou meio inutil porque agora eu so pego o ip de origem e o payload basicamente*/
	/*Mas deixa ai*/
	/* Struct do header de Ethernet */
	struct sniff_ethernet {
		u_char ether_dhost[ETHER_ADDR_LEN]; 
		u_char ether_shost[ETHER_ADDR_LEN]; 
		u_short ether_type; 
	};

	#define SIZE_ETHERNET	14

	/* Struct do header do IP */
	struct sniff_ip {
		u_char ip_vhl;		
		u_char ip_tos;		
		u_short ip_len;		
		u_short ip_id;		
		u_short ip_off;		
		#define IP_RF 0x8000		
		#define IP_DF 0x4000		
		#define IP_MF 0x2000		
		#define IP_OFFMASK 0x1fff	
		u_char ip_ttl;		
		u_char ip_p;		
		u_short ip_sum;		
		struct in_addr ip_src,ip_dst; 
	};

	#define IP_HL(ip)		(((ip)->ip_vhl) & 0x0f)
	#define IP_V(ip)		(((ip)->ip_vhl) >> 4)


	/* UDP header */
	
	struct sniff_udp {
        	u_short uh_sport;               
        	u_short uh_dport;               
        	u_short uh_ulen;                
        	u_short uh_sum;                 
	
	};
	
	#define SIZE_UDP        8               




uint16_t pOrig = 0;
char *portS;
int portI;

char ip_global[16];

libnet_t *l; //Contexto da libnet. O treco que usa pra tudo.
libnet_ptag_t tagUDP; //Tag pra modificar o header UDP;
libnet_ptag_t tagIPv4; //Tag pra modificar o header IPv4;



void novoPacote(uint16_t src, uint16_t dst, char* payload,char* ip_str){


	char bCast[20];	
	int i = 0;
	int c = 0;
	while(c<3){
	   if(ip_str[i] == '.'){
		c++;
	   }
	   i++;
	}
	strncpy(bCast, ip_str, i);
	bCast[i] = '\0'; 
	strcat(bCast, "255");

	tagUDP = libnet_build_udp(src, dst, LIBNET_UDP_H + sizeof(payload), 0, (uint8_t*)(payload), sizeof(payload),l,tagUDP);
	if(tagUDP == -1){
		printf("Erro construindo o pacote %s\n", libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	} 

	uint32_t ip_addr = libnet_name2addr4(l, bCast, LIBNET_DONT_RESOLVE);
	//Constroi o ipv4 e pula fora se der erro
	tagIPv4 = libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_UDP_H + sizeof(payload), 0, (uint16_t)0, 0, 127, IPPROTO_UDP, 0, libnet_get_ipaddr4(l), ip_addr, NULL,(uint32_t)0, l, tagIPv4);

	if ( tagIPv4 == -1 )
	{
	      printf("Erro no IP header: %s\n",
	      libnet_geterror(l));
	      libnet_destroy(l);
	      exit(EXIT_FAILURE);
	} 
	
	libnet_write(l);



}

char* ImprimePacote(int count, const struct pcap_pkthdr *header, const u_char *packet){
	
	const struct sniff_ethernet *ethernet;  
	const struct sniff_ip *ip;             
	const struct sniff_udp *udp; 

	char *payload;  

	int size_ip;
	int size_payload;

	ethernet = (struct sniff_ethernet*)(packet);
	
	
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("Tamanho Invalido do header IP: %u bytes\n", size_ip);
		return;
	}

	
	udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + SIZE_UDP);
	
	payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + SIZE_UDP);
	
       	 size_payload = ntohs(ip->ip_len) - (size_ip + SIZE_UDP);
         if (size_payload > ntohs(udp->uh_ulen))
                 size_payload = ntohs(udp->uh_ulen);
	
	
	if (size_payload > 0) {
		printf("   Count: %d\n", count);
		printf("   Mensagem Recebida: %s\n", payload);
		
	}

	return payload;



}

void callbackhost(u_char *args, const struct pcap_pkthdr *header,const u_char *packet){
	
	static int count = 0;                   
	const char *payload;                   
	char* ip_str;
	char resposta[9];
	static uint16_t _S;
	static uint8_t y;	
	static uint16_t moeda;

	//printf("\nPacket number %d:\n", count);
	//count++;
	

	payload = ImprimePacote(count, header, packet);

	if(count == 0){
		uint16_t _Sxy; // _S XOR y
		libnet_seed_prand(l);
	
		moeda = atoi(payload);

		y = (uint8_t)libnet_get_prand(LIBNET_PR8);
		_S = (uint16_t)libnet_get_prand(LIBNET_PR16);
		
		if(y%2==0) y= 0;
		else y = 1;

		_Sxy = y ^ _S;

		sprintf(resposta, "%d", _Sxy);
		/*printf("Escolheu: ");
		if(y == 1) printf("cara\n");
		else printf("coroa\n"); */
		printf("Moeda:%d\n_S:%d\nValor de Y:%d\n _S XOR y:%s\n",moeda,_S, y, resposta);
		

		
		
	}
	else if(count == 1){
		uint8_t result;
		uint16_t Sint;
		char aux[10];
		
		Sint = atoi(payload);

		result = moeda ^ Sint;


		if(result == y){sprintf(resposta, "%d", 1); printf("OK\n");}
		else {sprintf(resposta, "%d", 0); printf("NOK\n");}

		sprintf(aux,"%d",_S);

		strcat(resposta, aux);

		printf("S int:%d\nY:%d\nX:%d\n_S:%d\nResposta:%s\n", Sint, y,result, _S, resposta);
		
			
	}

	count++;



	printf("	Mensagem enviada:%s\n", resposta);
	novoPacote((uint16_t) SRC_PORT, (uint16_t) portI, resposta, ip_global);


	return;
}
void callbackserver(u_char *args, const struct pcap_pkthdr *header,const u_char *packet){
	
	static int count = 0;                   
	
	     
	const char *payload;                   

	char* ip_str;
	char resposta[9];
	static uint16_t S;
	static uint8_t x;
	static uint16_t	escolha;	
	

	//Pega o ip de origem do pacote
	struct sniff_ip *ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	ip_str = inet_ntoa(ip->ip_src);

	payload = ImprimePacote(count, header, packet);

	if(count == 0){
		uint16_t Sxx; // S XOR x
		libnet_seed_prand(l);
		x = (uint8_t)libnet_get_prand(LIBNET_PR8);
		S = (uint16_t)libnet_get_prand(LIBNET_PR32);

		if(x%2==0) x= 0;
		else x = 1;

		Sxx = x ^ S;

		sprintf(resposta, "%d", Sxx);
		/*printf("Moeda deu: ");
		if(x == 1) printf("cara\n");
		else printf("coroa\n");*/
		printf("S:%d\nValor de X:%d\nS XOR x:%s\n",S, x, resposta);

	}
	else if(count == 1){
		escolha = atoi(payload);
		sprintf(resposta,"%d",S);
		printf("Escolha:%d\nS:%s\n", escolha, resposta);
	}
	else if(count == 2){
		char ok;
		char aux[10];
		uint16_t _Sint;
		uint8_t result;
		
		ok = payload[0];
		strncpy(aux, payload, 1);

		char c = 'a';
		int i = 0;
		
		while(c!='\0'){
			c= payload[i+1];
			aux[i] = c;
			i++;			
		}
		_Sint = atoi(aux);
		result = escolha ^ _Sint;

		printf("Aux:%s\n_Sint:%d\n Y:%d\n",aux, _Sint, result);


		/*if(ok == '1') printf("Disse que ganhou\n");
		else printf("Disse que perdeu\n");*/

		if(result == x){sprintf(resposta,"OKBye"); printf("OK\n");}
		else {sprintf(resposta,"NOKBye"); printf("NOK\n");}

		
	}

	

	count++;
	printf("	Mensagem enviada:%s\n", resposta);
	novoPacote((uint16_t) SRC_PORT, (uint16_t) HOST_PORT, resposta,ip_str);


	return;
}




int main(int argc, char *argv[]){

	

	if((argv[2]!=NULL) && (strcmp(argv[2], "111")) && (strcmp(argv[2], "1010"))){
		portS = argv[2];
		portI = atoi(argv[2]);
	}
	else{
		portS = "22222";
		portI = 22222;
	}


	if((strcmp(argv[1],"host") != 0) && (strcmp(argv[1],"-S")!=0)) {
		printf("Argumento invalido\n");
		return 1;
	}

	/********************
		LIBNET1
	*********************/


	/*Inicializa um pacote inicial no contexto*/
	char errbuf[LIBNET_ERRBUF_SIZE]; //Uma string so pra retornar o erro  no init.
	char conteudo[] = "Hello";
	char ip_addr_str[16];
	uint32_t ip_addr;
	
	l = libnet_init(LIBNET_RAW4, NULL, errbuf); //Inicializa o contexto que eu vou usar.
	//Checa erro
	if(l ==NULL){
		printf("Erro no init: %s\n", errbuf);
	}

	/*Gera a porta fonte aleatoriamente*/
	/*libnet_seed_prand(l);
	//pOrig = (uint16_t)libnet_get_prand(LIBNET_PR16);
	pOrig = (uint16_t)1010;
	printf("Porta Origem: %d\n", pOrig);*/

	/*Constroi o header udp e pula fora se der algum erro.*/
	tagUDP = libnet_build_udp((uint16_t)SRC_PORT, (uint16_t)portI, LIBNET_UDP_H + sizeof(conteudo), 0, (uint8_t*)conteudo, sizeof(conteudo),l,0);
	if(tagUDP == -1){
		printf("Erro construindo o pacote %s\n", libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}  

	/*Endereco ip*/
	if(strcmp(argv[1],"host") == 0){	
		printf("Endereco IP destino: \n");
		scanf("%15s", ip_global);
	}
	else{
		sprintf(ip_global, "10.0.0.1");
	}
	char bCast[20];	
	char* srcIPstr = ip_global;
	int i = 0;
	int c = 0;
	while(c<3){
	   if(srcIPstr[i] == '.'){
		c++;
	   }
	   i++;
	}
	strncpy(bCast, srcIPstr, i);
	bCast[i] = '\0'; 
	strcat(bCast, "255");

	//novoPacote((uint16_t) SRC_PORT, (uint16_t) portI, conteudo, ip_global);

	ip_addr = libnet_name2addr4(l, bCast, LIBNET_DONT_RESOLVE);
	//Constroi o ipv4 e pula fora se der erro
	tagIPv4 = libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_UDP_H + sizeof(conteudo), 0, (uint16_t)0, 0, 127, IPPROTO_UDP, 0, libnet_get_ipaddr4(l), ip_addr, NULL,(uint32_t)0, l, 0);

	if ( tagIPv4 == -1 )
	{
	      printf("Erro no IP header: %s\n",
	      libnet_geterror(l));
	      libnet_destroy(l);
	      exit(EXIT_FAILURE);
	} 
	
	/*
		if ( libnet_autobuild_ipv4(LIBNET_IPV4_H + LIBNET_UDP_H +
	                        sizeof(conteudo), IPPROTO_UDP, ip_addr, l) == -1 )
	        {
	                printf("Erro no IP header: %s\n",
	                                libnet_geterror(l));
	                libnet_destroy(l);
	                exit(EXIT_FAILURE);
	        }
*/
	//printf("Fez pacote\n");


	/********************
		LIBPCAP
	*********************/
	 pcap_t *handle; //handle do pcap. Meio que tipo o contexto do libnet no sentido de que e o que eu uso pra tudo.
	 char *dev; //O device que eu vou escutar
	 char pcaperrbuf[PCAP_ERRBUF_SIZE]; //Mesmo esquema
	 struct bpf_program filtro; //O filtro pra escutar
	 char exp_filtro[] = "port "; //A expressao pra construir o filtro;
	 bpf_u_int32 mascara; //Mascara da rede
	 bpf_u_int32 netip; //IP
	 struct pcap_pkthdr header; //O header que o pcap retorna
	 const u_char *packet; //O pacote que eu capturei com a pcap
	
	/*Pega o device*/
	dev = pcap_lookupdev(errbuf);
	if(dev == NULL){
		printf("Erro ao pegar o dispositivo: %s\n", errbuf);
		return 1; 
	}

	//printf("Device\n");
	   
	/*Pega as propriedades do device. Popula mascara e netip*/
	if (pcap_lookupnet(dev, &netip, &mascara, errbuf) == -1) {
		printf("Nao pegou a mascara do device %s: %s\n", dev, errbuf);
		netip = 0;
		mascara = 0;
	}

	/*Inicializa a handle da sessao pcap*/
    	handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuf);
	if(handle == NULL){
		printf("Nao conseguiu iniciar sessao no device %s: %s\n", dev, errbuf);
		return 1;
	}
	//printf("Handle\n");

	 /*Faz o filtro. Primeiro compila ele e depois aplica*/
	 char adendo[16];
	 if(strcmp(argv[1], "host") == 0){
		//printf("Entrou aqui\n");
		sprintf(adendo,"%d",HOST_PORT);
		//printf("sprintf\n");
	 	strcat(exp_filtro, adendo); //Coloca a porta desejada na expressao
		//printf("Exp filtro: %s\n", exp_filtro);
         }
	 else{
		strcat(exp_filtro, portS);
	 }
	 
	 //printf("Exp filtro: %s\n", exp_filtro);

	 if (pcap_compile(handle, &filtro, exp_filtro, 1, netip) == -1) {
		printf("Erro compilando filtro %s: %s\n", exp_filtro, pcap_geterr(handle));
		return 1;
	 }
	 if (pcap_setfilter(handle, &filtro) == -1) {
	 	printf("Erro colocando o filtro %s: %s\n", exp_filtro, pcap_geterr(handle));
		return 1;
	 }
	

   
	if(strcmp(argv[1], "host") == 0){
	 	/*Se for host escreve o primeiro pacote*/
		 if(libnet_write(l)==-1){
			printf("Erro na escrita: %s\n", libnet_geterror(l));
		
		 }
		else{
		 pcap_loop(handle, 3, callbackhost, NULL);
		}
	}


	else if(strcmp(argv[1], "-S") == 0){
	 	pcap_loop(handle, 3, callbackserver, NULL);
	}

	pcap_close(handle);
	libnet_destroy(l);







	return 0;
}

