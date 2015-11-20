#include "../include/lpcap.h"


lpcapType lpcap_init(string port){
	lpcapType p;
	char errbuf[PCAP_ERRBUF_SIZE];			/*Buffer de erros*/

	p.dev = "lo";						  		/*Loopback: si proprio*/
	p.filter_exp = "udp dst port " + port; 	  	/*protocolo UDP na porta indicada*/

	//Cria um pacote que consegue capturar outro pacote que esta na rede
	//device
	//tamanho dos pacotes em bytes
	//promiscuo: porque uso vm
	//timeout em segundos
	//mensagem de erro
	p.handle = pcap_open_live(p.dev.c_str(), 65536, 1, 1000, errbuf);
	if (p.handle == NULL) {
		cout << endl << "[ERROR] Nao foi possivel abrir o dispositivo " << p.dev << " : " << errbuf << endl;
		exit(EXIT_FAILURE);
	}

	cout << endl << "[DEBUG] Dispositivo pronto para uso" << endl;

	//Prepara para limitar o trafico que vai ser inspecionado
	//sessao do pcap do pacote com todas as caracteristicas da funçao compile, que vai capturar pacote
	//filtro compilado
	//string do filtro
	//otimizaçao
	//ip da rede
	if (pcap_compile(p.handle, &(p.fp), p.filter_exp.c_str(), 0, p.net) == -1) {
		cout << endl << "[ERROR] Erro ao realizar parsing da expressao '" << p.filter_exp << "' : " << pcap_geterr(p.handle) << endl;
		cout << "Encerrando..." << endl;
		exit(EXIT_FAILURE);
	}
	cout << endl << "[DEBUG] Rede pronta para ser filtada" << endl;
	
	return p;
}


string lpcap_process(lpcapType p){
	string message;
	const u_char *packet;

	//Limita o trafico inspecionado
	if (pcap_setfilter(p.handle, &(p.fp)) == -1) {
		cout << endl << "[ERROR] Nao foi possivel inspecionar o trafico : " << pcap_geterr(p.handle) << endl;
		cout << "Encerrando..." << endl;
		exit(EXIT_FAILURE);
	}
	cout << endl << "[DEBUG] Rede filtrada" << endl;

	while(!pcap_next_ex(p.handle, &(p.header), &packet)) //Enquanto nao conseguiu ler pacote, continue tentando
		cout << "[DEBUG] Nao conseguiu capturar pacote." << endl;

	cout << endl << "[DEBUG] Terminou captura" << endl;
	cout << endl << "[DEBUG] Header len: " << p.header->len << " Header caplen: " << p.header->caplen << endl;

    message.clear();
    for( int i = PACKAGE_HEADER ; i < p.header->caplen ; i++ ) {
    	char c;
    	c = (char) *(packet+i);
        message += c;
    }

    return message;
	
}

void lpcap_free(lpcapType p){
	pcap_freecode(&(p.fp)); //limpa buffer de filtro
	pcap_close(p.handle); //deleta pacote resposavel de capturar outros pacotes
}
