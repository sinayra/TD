/* Trabalho de Transmissão de Dados - 2°/2014 - UnB */
/* João A. Ribeiro 		12/0014491 */
/* Pedro Paulo S. Lima 	11/0073983 */

/* Instruções para compilar e executar: 
compilar: gcc -Wall head_or_tail.c -o head_or_tail -lnet -lpcap
executar: sudo ./head_or_tail ["-S" para servidor ou "host" para cliente] ["nº da porta". Caso nula, porta = "22222"]
exemplo: sudo ./head_or_tail -S 123 - Executa o programa no modo Servidor, escutando na porta 123
*/

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libnet.h>
#include <pcap.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>

/* Constantes do programa */
#define VERDADEIRO 1
#define FALSO 0
#define HEAD 1
#define TAIL 2
#define TAM_VET 5
#define PORTA_PADRAO "22222"

/* Buffer global para printar o payload dos pacotes e debug. Única variável global do programa. */
char buffer_head_or_tail[1024];

/* *** FUNÇOES *** */

/*Funcao basica de envio de pacotes, UDP/IP "via" Ethernet, recebe apenas a mensagem/dados*/
void envia_pacote(char payload[], char porta[])
{
	libnet_t *l;
	char errbuf[LIBNET_ERRBUF_SIZE],
		 ip_addr_str[16] = "127.0.0.1";
	u_int32_t ip_addr;
	/*u_int16_t seq,id;*/
	libnet_ptag_t udp = 0;

	/*IPs e portas*/
	/*Src IP e Dst IP não usados (autobuild)*/
	/*u_long src_ip, dst_ip;*/
	u_short src_prt,
			dst_prt;

	/*Mensagem*/
	u_short payload_s;
	payload_s = strlen(payload);
	int bytes_escritos;

	/*Inicialização*/
	l = libnet_init(LIBNET_RAW4, "lo", errbuf);
	if ( l == NULL ) {
		printf("libnet_init() falhou: %s\n", errbuf);
		exit(EXIT_FAILURE);
	}

	/*Geração aleatória do ID*/
	/*libnet_seed_prand (l);
	id = (u_int16_t)libnet_get_prand(LIBNET_PR16);*/

	/*IP: 127.0.0.1 convertido para que possa ser tratado. Não há necessidade de DNS*/
	ip_addr = libnet_name2addr4(l, ip_addr_str, LIBNET_DONT_RESOLVE);
	if ( ip_addr == -1 ) {
		printf("Erro na conversao do endereco.\n");
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/*Construção do cabeçalho UDP */
	/*src_prt: porta de origem, dst_prt : porta de destino*/
	src_prt = (u_short)atoi(porta);
	dst_prt = (u_short)atoi(porta);
	
	udp = libnet_build_udp(src_prt, dst_prt, LIBNET_UDP_H + payload_s, 0, (uint8_t*)payload, payload_s, l, udp);
	if (udp == -1)
	{
		printf("Nao pode contruir o cabecalho UDP: %s\n", libnet_geterror(l));
	}
	
	/*Construção do cabeçalho IP*/	
	if (libnet_autobuild_ipv4(LIBNET_IPV4_H + 20 + payload_s + LIBNET_UDP_H, IPPROTO_UDP, ip_addr, l) == -1 ) {
		printf("Nao pode contruir o cabecalho IP: %s\n",\
		libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/*Envia o pacote*/
	bytes_escritos = libnet_write(l);
	if ( bytes_escritos != -1 )
		printf("%d bytes foram enviados.\n", bytes_escritos);
	else
		printf("Erro no envio do pacote: %s\n", libnet_geterror(l));

	libnet_destroy(l);
}

void imprime_dados (const u_char * dados, int Tamanho)
{
	int i=0, j=0;
	
	for(i=0-0%16; i<=Tamanho; i++)
	{
		if(dados[i] >= 32 && dados[i] <= 128)
		{
			buffer_head_or_tail[j]=(unsigned char)dados[i];
			j++;
		}
	}
	buffer_head_or_tail[j] = '\0';
	printf("%s\n",buffer_head_or_tail);
}

void processa_pacote(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
	int tamanho = header->len,
		udp=0,
		total=0;
	
	/*Exclui o cabeçalho Ethernet e obtém o cabeçalho IP do pacote*/
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	++total;
	
	/*Protocolo == UDP?*/
	if ((iph->protocol) == 17) {
		++udp;
	}
	
	unsigned short iphdrlen;
	iphdrlen = iph->ihl*4;
	
	struct udphdr *udph = (struct udphdr*)(buffer + iphdrlen  + sizeof(struct ethhdr));
	
	int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;
	
	imprime_dados(buffer + header_size, tamanho - header_size);
}

void aguarda_pacote(char porta[])
{
	char *dev = "lo";						  /*Dispositivo*/
	char errbuf[PCAP_ERRBUF_SIZE];			  /*Buffer de erros*/
	pcap_t *handle;				              /*packet capture handle*/
	char filter_exp[] = "udp dst port "; 	  /*Expressão de filtro, protocolo UDP na porta indicada*/
	struct bpf_program fp;					  /*Expressão de filtro compilada*/
	bpf_u_int32 mask;						  /*Máscara de Subrede*/
	bpf_u_int32 net;						  /*Ip da rede*/
	/*int num_pacotes = 1;*/                  /*Número de pacotes a capturar*/
	
	strcat(filter_exp, porta); /* Concatena a porta recebida com a expressão de filtro */
	
	/* Obtém número da rede e máscara associada ao dispostivo*/
	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Nao pode obter mascara de rede para o dispositivo %s : %s\n",dev, errbuf);
		net = 0;
		mask = 0;
	}

	/*Abre o dispositivo para escutar*/
	handle = pcap_open_live(dev, 65536, 1, 100000000, errbuf);
	if (handle == NULL) {
		printf("Couldn't open device %s: %s\n", dev, errbuf);
		exit(EXIT_FAILURE);
	}

	/*Garantir que o dispositivo suporta Ethernet*/
	if (pcap_datalink(handle) != DLT_EN10MB) {
		printf("%s nao suporta Ethernet\n", dev);
		exit(EXIT_FAILURE);
	}
	
	/*Compila a expressão de filtro */
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		printf("Erro no parsing do filtro %s: %s\n",filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}
	
	/*Aplica o filtro compilado*/
	if (pcap_setfilter(handle, &fp) == -1) {
		printf("Nao foi possivel aplicar filtro %s: %s\n",filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}
	
	/*Configurar o loop de escuta*/
	pcap_loop(handle, 1, processa_pacote, NULL);
	
	/*Libera memória*/
	pcap_freecode(&fp);
	pcap_close(handle);
}

/* Gera "randomicamente" (x e S) e (y e S'), onde x e y podem ser {1=HEAD,2=TAIL} e sequencias S e S' possuem 4 bytes */
void gera(char escolha[], char chave[])
{
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

/* Encripta a escolha e a chave em um criptograma usando o XOR (^) do C. */
void encripta (char escolha[], char chave[],char criptograma[])
{
	int i;
	
	for(i=0; i<TAM_VET-1; i++) {
		criptograma[i] = escolha[i]^chave[i];
	}
	criptograma[i] = '\0';
}

/* Desencripta um criptograma com uma chave e se obtem a escolha, HEAD ou TAIL com o XOR (^) do C. */
void desencripta (char criptograma[], char chave[], char escolha[])
{
	int i;
	
	for(i=0; i<TAM_VET-1; i++) {
		escolha[i] = criptograma[i]^chave[i];
	}
	escolha[i] = '\0';
}

void Cliente (char porta[])
{
	char mensagem_s[256],
		 mensagem_s_linha[256];
	char y[TAM_VET], S_linha[TAM_VET]; /* Cliente:  escolha y e chave S' */
	char x[TAM_VET], S[TAM_VET];	   /* Servidor: escolha x e chave S */

	/* 2) Envia "Hello" para o servidor */
	envia_pacote("Hello\n", porta);
	aguarda_pacote(porta);

	/* 4) Aguarda a mensagem_s e a copia para a variavel local. Gera y e S', encripta (mensagem_s_linha = y XOR S_linha) e manda para o servidor. */
	strcpy(mensagem_s, buffer_head_or_tail);
	gera(y, S_linha);
	encripta(y, S_linha, mensagem_s_linha);
	envia_pacote(mensagem_s_linha, porta);
	aguarda_pacote(porta);

	/* 6) Copia a chave S do servidor e a desencripta com a mensagem_s antes recebida. O resultado do servidor está em x. Envia a chave S' para o servidor. */
	strcpy(S, buffer_head_or_tail);
	desencripta(mensagem_s, S, x);
	envia_pacote(S_linha, porta);
	aguarda_pacote(porta);
	
	/* 8) Compara as escolhas e envia OK caso sejam iguais. */
	if(strcmp(x,y) == 0) {
		envia_pacote("OK", porta);
	}
	else {
		envia_pacote("NOK", porta);
	}

	/* 10) Espera o "Bye" do servidor e encerra-se a troca de mensagens. */
	aguarda_pacote(porta);
}

void Servidor(char porta[])
{
	char mensagem_s[256],
		 mensagem_s_linha[256];
	char y[TAM_VET], S_linha[TAM_VET]; 	/* Cliente:  escolha y e chave S' */
	char x[TAM_VET], S[TAM_VET];  		/* Servidor: escolha x e chave S */

	/* 1) Aguarda Hello do cliente */
	aguarda_pacote(porta);
	
	/* 3) Gera e encripta a mensagem_s (mensagem_s = x XOR S) e a envia para o cliente. */
	gera(x, S);
	encripta(x, S, mensagem_s);
	envia_pacote(mensagem_s, porta);
	aguarda_pacote(porta);
	
	/* 5) Recebe a mensagem_s_linha e a copia. Envia a chave S. */
	strcpy(mensagem_s_linha, buffer_head_or_tail);
	envia_pacote(S, porta);
	aguarda_pacote(porta);

	/* 7) Recebe a chave S', desencripta para a variavel y e compara com x, sua propria escolha. */
	strcpy(S_linha, buffer_head_or_tail);
	desencripta(mensagem_s_linha, S_linha, y);
	if(strcmp(x,y) == 0) {
		envia_pacote("OK", porta);
	}
	else {
		envia_pacote("NOK", porta);
	}
	aguarda_pacote(porta);

	/* 9) Termina a comunicação com um "Bye". */
	envia_pacote("Bye\n", porta);
}

int main ( int argc, char *argv[] )
{
	/* argv[0] = nome do programa 
	   argv[1] = modo de operação
	   argv[2] = porta. Se nulo, escolher PORTA_PADRAO, que é "22222" */
	char porta[20];
	
	if (argv[2] == NULL) {
		strcpy (porta, PORTA_PADRAO);
	}
	else {
		strcpy (porta, argv[2]);
	}
	
	/* Loop para pegar o modo de operação na forma exata. */
	while ((strcmp(argv[1], "host") != 0) && (strcmp(argv[1], "-S") != 0)) {
		printf("\nModo de operacao invalido.");
		printf("\nDigite \"-S\" para modo servidor ou \"host\" para modo cliente: ");
		scanf("%s", argv[1]);
	}
	
	if (strcmp(argv[1], "host") == 0) {
		printf("##### - Cliente - #####\n");
		printf("Cliente conectado na porta: %s\n", porta);
		Cliente(porta);
	}
	else {
		printf("##### - Servidor - #####\n");
		printf("Servidor escutando na porta: %s\n", porta);
		Servidor(porta);
	}
	return 0;
}



