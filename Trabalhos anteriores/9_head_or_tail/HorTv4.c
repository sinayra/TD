/*
Trabalho de Transmissão de Dados


[1] tutorial LIBPCAP :http://www.tcpdump.org/pcap.html
[2] tutorial LIBNET  :http://repolinux.wordpress.com/2011/09/18/libnet-1-1-tutorial/

Maiores informações, leia o ReadMe
*/
#include <pcap.h>
#include <stdio.h>
#include <libnet.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


/*As structs abaixo são declaradas para se tornar mais fácil a
extração dos dados dos pacotes recebidos pela LIBPCAP
Esses trechos de código que definem as structs foram retirados
do tutorial que pode ser encontrado em [1]
*/
/////////////////////////////////////////////////
	struct sniff_ethernet {
		u_char ether_dhost[ETHER_ADDR_LEN]; 
		u_char ether_shost[ETHER_ADDR_LEN]; 
		u_short ether_type; 
	};
	struct sniff_udp {
   	    u_short uh_sport;               
       	u_short uh_dport;               
       	u_short uh_ulen;                
       	u_short uh_sum;                 
	
	};
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
////////////////////////////////////////////////
/*Variáveis globais utilizadas pelo
programa. Algumas servem para identificar
parametros que são comuns para todas as funções
e outras servem de flags para ativar determinadas
ações que são desencadeadas dependendo do modo
que o programa está rodando.
*/
uint16_t srcPort = 0;
char *port_str;
int port_int;
char global_ip[16];
int ServerOUClient;
/*
As variaveis abaixo são para utilização
da biblioteca LIBNET, o contexto, e duas
tags, que vão permitir a movimentação e
utilização dos IPv4 e do UDP
*/
libnet_t *l; //contexto para a libnet
libnet_ptag_t tag_udp; //tag UDP
libnet_ptag_t tag_ip;  //tag IPv4
//Todas essas declarações foram retiradas de exemplos
//de [2]


/*A função abaixo serve para a criação de uma unidade
que facilite a criação de novos pacotes que serão
enviados pela rede através da biblioteca
LIBNET. Possui como parametros uma fonte,
um destino, a carga a ser transmitida (payload) e
uma string carregando o endereço de IP*/
void createPacket(uint16_t src, uint16_t dst, char* payload,char* ip_str){
//abaixo estão listadas as declarações das variaveis
//utilizadas na função.
	char broadcast[20];//string que carregará o endeteço de IP	para broadcast
	int i = 0; // variavel contadora para percorrimento da string
	int c = 0; // contadora de pontos dentro do endereço
	
	//o seguinte laço de repetição tem a função de
	//percorrer toda a string que contem oendereço
	//de IP e contar em qual posição da string que
	//termina o terceiro endereço da notação quad dot 
	while(c<3){
	   if(ip_str[i] == '.'){c++;}
	   i++;
	}
	//após detectar o endereço/24, copiar para a
	//string broadcast. 
	strncpy(broadcast, ip_str, i);//copia como mencionado
	broadcast[i] = '\0'; //adiciona caracter de final de string
	strcat(broadcast, "255"); // concatena final .255 no broadcast
    
    //define o tag_udp conforme os parametros padrões, como descrito no tutorial de [2]
	tag_udp = libnet_build_udp(src, dst, LIBNET_UDP_H + sizeof(payload), 0, (uint8_t*)(payload), sizeof(payload),l,tag_udp);
	
	//caso ocorra algum problema, a tag receberá o valor -1,
	//e a mensagem de erro pode ser encontrada conforme descrito abaixo.
	if(tag_udp == -1){
		printf("Erro ao construir o pacote %s\n", libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	} 
    
    //declaração de um inteiro que contem o endereço de IP
	uint32_t ip_addr = libnet_name2addr4(l, broadcast, LIBNET_DONT_RESOLVE);//função de converter str para int [2]
	
	//abaixo encontra-se a construção do header IPv4
	//procedimento também descrito em [2]
	tag_ip = libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_UDP_H + sizeof(payload), 0, (uint16_t)0, 0, 127, IPPROTO_UDP, 0, libnet_get_ipaddr4(l), ip_addr, NULL,(uint32_t)0, l, tag_ip);
    //caso aconteça algum erro, o mesmo pode ser identificado
    //pelo valor negativo em tag_ip, abaixo se encontra
    //o tratamento, como sugerido em [2]
	if ( tag_ip == -1 )
	{
	      printf("Erro no header do IP : %s\n", libnet_geterror(l));
	      libnet_destroy(l);
	      exit(EXIT_FAILURE);
	} 
	//depois de tudo pronto e inicializado,
	//é hora de se escrever no contexto.
	libnet_write(l);
}


/*
A função abaixo tem um papel semelhante ao da função
anterior, só que enquanto a outra criava pacotes novos
gerenciando os headers por meio da LIBNET, a proxima
função tem a tarefa de abrir os pacotes recebidos pela
rede atraves da LIBPCAP, e imprimir na tela os dados 
transportados
*/
//os parametros são uma função contadora que vai determinar o que
//procurar nos dados do pacote, se uma definição de cara ou coroa,
//se um ok ou nok... o cabeçalho header da libpcap, conforme
//descrito em [1] e a variavel que apontará para o pacote
//propriamente dito.
char* openPacket(int count, const struct pcap_pkthdr *header, const u_char *packet){
	//variaveis que são utilizadas para
	//extrair os dados dos pacotes [1]
	const struct sniff_ip *ip;             
	const struct sniff_udp *udp; 
    //algumas variaveis que são usadas na função
	int size_ip; //carrega o tamanho do IP
	char *payload; //aponta para a string que será o payload
	int size_payload; //inteiro que carrega a o tamanho do pauload

	
	//de acordo com [1], se seguirmos os seguinte procedimento
	//teremos condições de extrair do pacote os dados que queremos
	//como por exemplo:
	ip = (struct sniff_ip*)(packet + 14);//o ip
	size_ip = (((ip)->ip_vhl) & 0x0f)*4;//p tamanho do ip
	if (size_ip < 20) {//podemos utilizar essa mensagem de erro para tamanhos ilegais de ip
		printf("IP com header apresentando tamanho ilegal de %u bytes\n", size_ip);
	}

	//da mesma forma, podemos utilizar dessa técnica para
	//extrair o UDP
	udp = (struct sniff_udp*)(packet + 14 + 8);
	
	//e por fim o payload do pacote propriamente dito.
	payload = (u_char *)(packet + 14 + size_ip + 8);
	    
	//ainda seguindo [1], podemos extrair os tamanhos do payload    
    size_payload = ntohs(ip->ip_len) - (size_ip + 8);
    if (size_payload > ntohs(udp->uh_ulen))
        size_payload = ntohs(udp->uh_ulen);
	
	//o payload pode assumir tamanho 0 ou maior que zero
	//0 no caso de nao haver payload, caso haja, será maior
	// que zero.
	if (size_payload > 0) {
		//mas nesse caso, pode ser ainda chamado pelo server
		if(ServerOUClient == 0){
		    printf("\t-----------------------------------------------------\n\n");
		    printf("\n\tMensagem recebida:\n");
		    printf("\t%s <-------------------\n", payload);//imprime na tela o payload recebido
		}
		//ou chamado pelo client 
		if(ServerOUClient == 1){
		    printf("\t--------------------------------------------\n\n");
		    printf("\n\tMensagem recebida:\n");
		    printf("\t-------------------> %s\n", payload);//imprime na tela o payload recebido
		}
		
		
	}
    //para finalizar a função de le os payloads dos pacotes
    //recebidos pela libpcap, o seu retorno é justamente o
    //proprio payload encontrado.
	return payload;
}


/*
A função abaixo declarada tem como objetivo tratar
os acontecimentos que devem seguir um recebimento 
de um pacote pela função de loop da libpcap. Os
passos seguidos foram extraidos em sua maioria
da descrição de como funcionam as funções callbacks
em [1]
*/
//os parametros são padrões, não se pode mudar
void serverCallback(u_char *args, const struct pcap_pkthdr *header,const u_char *packet){
	//declaração de variaveis que
	//serão utilizadas ao longo 
	//desta função que trata do 
	//recebimento de um pacote pela libpcap
	static int count = 0;   //variavel contadora que define algumas atribuições da função                
	const char *payload;    //string que aponta para o payload               
	char* ip_str;           //string que carrega o endereço de IP
	static uint8_t x;       //variavel "aleatoria" definida pelo server toda vez que o client disser hello
	static uint16_t	HorT;	//variavel que carrega se foi cara ou coroa
	static uint16_t S;      //variavel aleatoria S q "encripta" o valor de X para a trasnmissao
    char answ[9];           //variavel que carrega o resultado da decriptação da msg recebida
	uint16_t sORx;          //variavel que carrega o valor de SxorX
	
	
	//o trecho abaixo foi baseado nas instruções
	//de [1] para que sea possível destrinchar o
	//pacote
	struct sniff_ip *ip = (struct sniff_ip*)(packet + 14);
	ip_str = inet_ntoa(ip->ip_src);
    //e assim ser possivel pegar seu payload
	payload = openPacket(count, header, packet);
    //o presente contador serve para que a função
    //callback saiba qual a vez que está sendo
    //chamada, e assim desempenhar o correto papel
    //para que o programa tenha o efeito desejado.
	if(count == 0){
	    //abaixo estamos determinando os numeros
	    //aleatorios conforme mostrou o tutorial [2]
		libnet_seed_prand(l);
		x = (uint8_t)libnet_get_prand(LIBNET_PR8);
		S = (uint16_t)libnet_get_prand(LIBNET_PR32);
        //determina-se X como 0 ou 1 (cara ou coroa)
		if(x%2==0) x= 0;
		else x = 1;
        //calcula-se o resultado de sORx
		sORx = x ^ S;
        //armazena o resultado na variavel
        //answ q será enviada como resposta
        //para o client
		sprintf(answ, "%d", sORx);
        //imprime na tela os valores das variaveis
		printf("\n\tValor aleatorio S = %d\n\tValor de X = %d\n\tValor da comparação S XOR x = %s\n",S, x, answ);

	}//caso seja a proxima vez que a função está sendo chamada
	else if(count == 1){
	    //os calculos são diferentes
		HorT = atoi(payload);//variavel que carrega o resultado de cara ou coroa.
		sprintf(answ,"%d",S);
		printf("\n\tMensagem cifrada recebida = %d\n\tS a ser enviado = %s\n", HorT, answ);
		//assim como as mensagens impressas na tela
	}
	else if(count == 2){
	//caso seja a terceira vez que a função está sendo chamada
	//o procedimento é ainda outro.	
		int i = 0;
		uint8_t result;
		char aux[10];
		uint16_t Slinha_inteiro;
		char c = 'a';
		//as variaveis acima apresentam os
		//valores que serão necessários para esta
		//etapa do procedimento
		strncpy(aux, payload, 1);
		while(c!='\0'){
			c= payload[i+1];
			aux[i] = c;
			i++;			
		}
		Slinha_inteiro = atoi(aux);
		//coloca-se em Slinha_inteiro o
		//equivalente inteiro ao valor
		//que corresponde ao SLinha
		//do problema
		result = HorT ^ Slinha_inteiro;
        //result carrega o valor encontrado para
        //o cara ou coroa do outro lado da comunicação.
		printf("\n\tAuxiliar para desvendar S' = %s\n\tS' = %d\n\tY desvendado = %d\n",aux, Slinha_inteiro, result);
        //auxiliar para desvendar S' nada mais eh que o resultado
        //da comparação enviada pelo outro lado da comunicação
        
        //aqui vamos descobrir se o cara ou coroa
        //foi bem ou mal sucedido.
		if(result == x){
		    sprintf(answ,"Bye_OK");//escreve OK bye_ok pra mandar de resposta 
		    printf("\n\tOK\n");//e printa ok caso seja bem sucedido
		}
		else {
		    sprintf(answ,"Bye_NOK");//escreve bye_nok para enviar de resposta 
		    printf("\n\tNOK\n");//e printa nok caso seja mal sucedido
		}
	}

	
    //aumenta o contador para que seja possivel executar a
    //proxima ação na proxima vez que a função for chamada
	count++;
	
	//as sequencias abaixo são para imprimir na tela o
	//visual da recepção da mensagem dependendo do lado
	//a ser executado
	if(ServerOUClient == 0){
        printf("\n\tMensagem enviada:\n");
        printf("\t-------------------> %s\n\n\n", answ);
    }
        
    //as sequencias abaixo são para imprimir na tela o
	//visual da recepção da mensagem dependendo do lado
	//a ser executado
    if(ServerOUClient == 1){
        printf("\n\tMensagem enviada:\n");
        printf("\t%s <------------------- \n\n\n", answ);
    }
    //se cria um pacote com as informações captadas para que depois de tudo
    //pronto, seja transmitida a mesagem para o outro lado.
	createPacket((uint16_t) 111, (uint16_t) 1010, answ,ip_str);
	return;
}



/*
A função abaixo eh o callback
da parte HOST, ou client
do programa.

Toda vez que o lado host estiver
em um loop de captura de mensagens
como descrito em [1], e o mesmo
conseguir captar uma mensagem que
venha do do server, ele executará
os seguintes passos
*/
void hostCallback(u_char *args, const struct pcap_pkthdr *header,const u_char *packet){
//a função se parece muito com a callbackserver
    //as variaveis abaixo descritas servem
    //tanto para armazenar os dados que precisam
    //ser tratados como os S e o Y, bem como
    //algumas variaveis que servem para guiar a execução
    //do programa como flags.
	static uint16_t S_Linha;
	static uint8_t y;
	char answ[9];	
	static uint16_t inRes;
	const char *payload;
	static int count = 0;
	uint16_t SLinhaORy;
    
    //o payload eh carregado como apresentado 
    //em [1]
	payload = openPacket(count, header, packet);

	if(count == 0){
	//como na função anterior, o que a função irá executar depende
	//da flag que está armazenada em count que representa a vez
	//que a função está sendo chamada.
		// gerando valor aleatório
		//conforme descrito em [2]
		libnet_seed_prand(l);
	    //representação inteira do payload carregado
		inRes = atoi(payload);
        //alocando os valores aleatorios em y e S'
		y = (uint8_t)libnet_get_prand(LIBNET_PR8);
		S_Linha = (uint16_t)libnet_get_prand(LIBNET_PR16);
		
		//definindo o cara ou coroa
		if(y%2==0) y= 0;
		else y = 1;
        //calculando o resultado do XOR
		SLinhaORy = y ^ S_Linha;
        //imprimindo na string que será enviada como resposta
        //para o outro lado da rede o valor encontrado no calculo.
		sprintf(answ, "%d", SLinhaORy);
        //uma vez impressa na string, a função imprime
        //na tela o resultado das variaveis calculadas
		printf("\n\tMensagem cifrada recebida = %d\n\tS' = %d\n\tValor de Y = %d\n\tS' XOR y = %s\n",inRes,S_Linha, y, answ);
		
	}//caso seja a segunda execução os procedimentos são outros
	else if(count == 1){
		uint8_t result;
		char aux[10];
		uint16_t Sint;
		//variaveis que serão utilizadas para realizar os calculos e
		//o envio dos parametros para o outro lado da rede.
		Sint = atoi(payload);
        //sabendo q o envio corresponde ao S
        //capturar o equivalente inteiro ao seu valor
        
        //salvar em result o a decriptação do
        //valor enviado pelo outro lado da rede
        //na interação anterior.
		result = inRes ^ Sint;

        //caso sejam identicos os resultados, imprimri na tela
        //o ok para o resultado
		if(result == y){
		    sprintf(answ, "%d", 1); 
		    printf("\n\tResultado do cara ou coroa: OK\n");
		}
		else {//ou o NOK para o caso contrário.
		    sprintf(answ, "%d", 0); 
		    printf("\n\tResultado do cara ou coroa: NOK\n");
		}
        
        //salvar em aux o que estava contido em S'
		sprintf(aux,"%d",S_Linha);
        
        //concatenar à resposta o conteudo de aux
		strcat(answ, aux);

        //por fim, imprimir na tela todas as variaveis que foram utilizadas para determinar se o resultado
        //do cara ou coroa foi correto ou nao
		printf("\n\tS desvendado = %d\n\tY = %d\n\tX desvendado = %d\n\tS' = %d\n\tResposta a ser enviada = %s\n", Sint, y,result, S_Linha, answ);
			
	}


	count++;
	//incrementa-se o contador
	// para que a proxima chamada da função
	//venha com execuções de diferentes tarefas.


    if(ServerOUClient == 0){//mais uma vez o layout do envio da mensagem
        printf("\n\tMensagem enviada:\n");
        printf("\t-------------------> %s\n\n\n", answ);
    } 
    if(ServerOUClient == 1){//para ambos os lados da execução, host e server
        printf("\n\tMensagem enviada:\n");
        printf("\t%s <------------------- \n\n\n", answ);
    }
    //uma vez estando tudo bem impresso na tela, realmente enviar a
    //mensagem conforme [2]
	createPacket((uint16_t) 111, (uint16_t) port_int, answ, global_ip);

	return;
}



/*
Abaixo se encontra-se o trecho do codigo
referente a função main que governa o funcionamento
de todo o programa.
é aqui onde todas as demais funções são chamadas
*/
int main(int argc, char *argv[]){


    //trecho abaixo imprime a parte grafica no caso de estamor
    //executando um server
	if(strcmp(argv[1],"-S")==0){
	    ServerOUClient = 0;
	    printf("\t _____   _____   _____    _     _   _____   _____      \n");
        printf("\t/  ___/ | ____| |  _  \\  | |   / / | ____| |  _  \\    \n");
        printf("\t| |___  | |__   | |_| |  | |  / /  | |__   | |_| |      \n");
        printf("\t\\___  \\ |  __|  |  _  /  | | / /   |  __|  |  _  /    \n");
        printf("\t ___| | | |___  | | \\ \\  | |/ /    | |___  | | \\ \\  \n");
        printf("\t/_____/ |_____| |_|  \\_\\ |___/     |_____| |_|  \\_\\  \n\n");
        printf("\t-----------------------------------------------------\n\n");
	}
	//mesmo que o trecho acima só que dessa vez para o CLIENT.
    if(strcmp(argv[1],"host")==0){
        ServerOUClient = 1;
        printf("\t _____   _       _   _____   __   _   _____  \n");
        printf("\t/  ___| | |     | | | ____| |  \\ | | |_   _|\n");
        printf("\t| |     | |     | | | |__   |   \\| |   | | \n");
        printf("\t| |     | |     | | |  __|  | |\\   |   | |\n");
        printf("\t| |___  | |___  | | | |___  | | \\  |   | | \n");
        printf("\t\\_____| |_____| |_| |_____| |_|  \\_|   |_|\n\n");
        printf("\t--------------------------------------------\n\n");
    }


    //o trecho abaixo serve para que possamos 
    //definir as portas que desejamos usar
    //caso nao passemos argumento algum para
    //a porta, o programa utiliza a padrão
    //22222 UDP
	if((argv[2]!=NULL) && (strcmp(argv[2], "111")) && (strcmp(argv[2], "1010"))){
		port_str = argv[2];
		port_int = atoi(argv[2]);
	}//definição da padrao
	else{
		port_str = "22222";
		port_int = 22222;
	}

    //caso o usuario entre com uma entrada diferente de host ou -S
    //o programa o ensina a utilizar o programa da forma correta.
	if((strcmp(argv[1],"host") != 0) && (strcmp(argv[1],"-S")!=0)) {
		printf("O argumento apresentado na entrada não é válido...\n");
		printf("para rodar como server, o argumento é -S\n");
		printf("para rodar como client, o argumento é host");
		return 1;
	}


/*
Os trechos abaixo fazem o uso do LIBNET para enviar pacotes.
*/

	//variaveis utilizadas para a primeira mensagem a ser entregue
	//o hello
	char msg[] = "Hello";
	char errbuf[LIBNET_ERRBUF_SIZE];
	uint32_t ip_addr;
	
	//conforme apresentado em [2] a maneira
	//de inicializar o libnet
	l = libnet_init(LIBNET_RAW4, NULL, errbuf);
	if(l ==NULL){
		printf("Erro no init: %s\n", errbuf);
	}
    //abaixa encontra-se a definição da tag udp conforme [2]
	tag_udp = libnet_build_udp((uint16_t)111, (uint16_t)port_int, LIBNET_UDP_H + sizeof(msg), 0, (uint8_t*)msg, sizeof(msg),l,0);
	if(tag_udp == -1){
		printf("Erro ao construir o pacote %s\n", libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}  //e seu devido tratamento de erros


    //trecho para obter o endereço de IP do servidor
    //que eh perguntado como entrrada do usuário
    //do lado do HOST para o user.s
	if(strcmp(argv[1],"host") == 0){	
		printf("\n\n\tDigite o endereco de IP do destino: ");
		scanf("%15s", global_ip);
		printf("\n");
		printf("\t--------------------------------------------\n\n");
		
	}
	else{
		sprintf(global_ip, "10.0.0.1");
	}
	
	//variavel que será o endereço global
	//alterado no ultimo byte para que
	//carregue o numero 255
	char broadcast[20];	
	char* IPorig_str = global_ip;
	int i = 0;
	int c = 0;
	while(c<3){
	   if(IPorig_str[i] == '.'){c++;}
	   i++;
	}
	//esse procedimento foi executado anteriormente em outra função acima
	strncpy(broadcast, IPorig_str, i);
	broadcast[i] = '\0'; 
	strcat(broadcast, "255");
    //converte o valor do endereço ip de string para int
    //conforme [2]
	ip_addr = libnet_name2addr4(l, broadcast, LIBNET_DONT_RESOLVE);
	//cria a tag de ipv4 para que seja possivel
	//enviar todos os headers
	tag_ip = libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_UDP_H + sizeof(msg), 0, (uint16_t)0, 0, 127, IPPROTO_UDP, 0, libnet_get_ipaddr4(l), ip_addr, NULL,(uint32_t)0, l, 0);
    //tratamento de erro descrito abaixo
	if ( tag_ip == -1 ){
	      printf("Erro no IP header: %s\n",
	      libnet_geterror(l));
	      libnet_destroy(l);
	      exit(EXIT_FAILURE);
	} 
	
	/*
	Os trechos abaixo fazem uso da LIBPCAP 
	para que seja possível receber os pacotes 
	enviados pela rede
	*/
	//variaveis utilizadas para dominio e
	//controle das funçoes apresentadas
	// em [1] para libpcap
	 pcap_t *handle;
	 char *dev;
	 struct bpf_program filtro;
	 char filterExpression[] = "port ";
	 bpf_u_int32 mask;
	 bpf_u_int32 IP_rede;
	

	dev = "wlan0";
    //nesse trecho pequeno
    //setamos o dispositivo
    //para a porta wireless
    
    //trecho de [1] para conferir se a mascara do ip
    //foi corretamente extraida
	if (pcap_lookupnet(dev, &IP_rede, &mask, errbuf) == -1) {
		printf("Nao pegou a mask do device %s: %s\n", dev, errbuf);
		IP_rede = 0;
		mask = 0;
	}
        //inicio do pcap [1]
    	handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuf);
	if(handle == NULL){
		printf("Nao conseguiu iniciar sessao no device %s: %s\n", dev, errbuf);
		return 1;
	}
    
    //criação do adendo que será utilizado
    //para compor o filtro do libpcap [1]
	 char adendo[16];
	 if(strcmp(argv[1], "host") == 0){//caso seja host
	    //escrita em adendo
		sprintf(adendo,"%d",1010);
        //terminando de escrever o filtro
	 	strcat(filterExpression, adendo);
     }
	 else{//caso seja um server
		strcat(filterExpression, port_str);
	 }
	 //compilação do filtro juntamente com seu tratamento
	 //de erros
	 if (pcap_compile(handle, &filtro, filterExpression, 1, IP_rede) == -1) {
		printf("Erro compilando filtro %s: %s\n", filterExpression, pcap_geterr(handle));
		return 1;
	 }
	 //setando o filtro, mais uma vez
	 // junto com sua checagem de erros
	 if (pcap_setfilter(handle, &filtro) == -1) {
	 	printf("Erro colocando o filtro %s: %s\n", filterExpression, pcap_geterr(handle));
		return 1;
	 }

	if(strcmp(argv[1], "host") == 0){
    //procedimentos a serem adotados para o caso de ser o host
		 if(libnet_write(l)==-1){//escrever na rede, e ja emendar a verificação de erros
			printf("Erro na escrita: %s\n", libnet_geterror(l));
		
		 }
		else{//caso nao tenha erros, depois de enviar entrar no loop de pegar
		     //proxima mensagem
		 pcap_loop(handle, 3, hostCallback, NULL);
		}
	}
    //para o case de ser um server, o programa já começa
    //esperando pela recepção de mensagens (hello no caso)
	else if(strcmp(argv[1], "-S") == 0){
	 	pcap_loop(handle, 3, serverCallback, NULL);
	}
    //destruição do contexto libnet
	libnet_destroy(l);
	//encerramento do hendle para o libpcap
	pcap_close(handle);

    //mensagens de finaçização da execução do programa.
    //printf("\n\tACABOU O PROGRAMA!!! VLW FLW\n");
    printf("\n\n-----------------------------------------------------------\n");
    printf("|¯ | |\\/|   |¯\\ |¯|   |¯| |¯| |¯| |¯  |¯| /¯\\ |\\/| /¯\\ \n");
    printf("|¯ | |  |   |_/ |_|   |¯  |¯\\ |_| | | |¯\\ |¯| |  | |¯| \n");
    printf("                                   ¯                   \n");


	return 0;
}

