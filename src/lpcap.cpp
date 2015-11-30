#include "../include/lpcap.h"


lpcapType lpcap_init(string port){
	lpcapType p;
	char errbuf[PCAP_ERRBUF_SIZE];			/*Buffer de erros*/
	stringstream s;

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
		s.clear();
		s << "Nao foi possivel abrir o dispositivo " << p.dev << " : " << errbuf;
		showLog(error, s.str());
		exit(EXIT_FAILURE);
	}

	showLog(debug, "Dispositivo pronto para uso");

	//Prepara para limitar o trafico que vai ser inspecionado
	//sessao do pcap do pacote com todas as caracteristicas da funçao compile, que vai capturar pacote
	//filtro compilado
	//string do filtro
	//otimizaçao
	//ip da rede
	if (pcap_compile(p.handle, &(p.fp), p.filter_exp.c_str(), 0, p.net) == -1) {
		s.clear();
		s << "Erro ao realizar parsing da expressao '" << p.filter_exp << "' : " << pcap_geterr(p.handle);
		showLog(error, s.str());
		exit(EXIT_FAILURE);
	}
	showLog(debug, "Rede pronta para ser filtada");
	
	return p;
}


string lpcap_process(lpcapType p, int timeout){
	string message;
	time_t start, end, diff;
	const u_char *packet;
	stringstream s;

	//Limita o trafico inspecionado
	if (pcap_setfilter(p.handle, &(p.fp)) == -1) {
		s.clear();
		s << "Nao foi possivel inspecionar o trafico : " << pcap_geterr(p.handle);
		showLog(error, s.str());
		exit(EXIT_FAILURE);
	}
	showLog(debug, "Rede filtrada");

	time(&start);
	do{
		time(&end);
		diff = difftime(end, start);
		showLog(debug, "Nao conseguiu capturar pacote.");
	}while(!pcap_next_ex(p.handle, &(p.header), &packet) && diff < timeout);//Enquanto nao conseguiu ler pacote e nao deu timeout, continue tentando

	showLog(debug, "Terminou captura");
	s.clear();
	s << "Header len: " << p.header->len << " Header caplen: " << p.header->caplen;
	showLog(debug, s.str());

	message.clear();
	if(diff < timeout){
	    for( int i = PACKAGE_HEADER ; i < p.header->caplen ; i++ ) {
	    	char c;
	    	c = (char) *(packet+i);
	        message += c;
	    }
	}
	else{
		showLog(warning, "Timeout");
	}

    return message;
	
}

void lpcap_free(lpcapType p){
	pcap_freecode(&(p.fp)); //limpa buffer de filtro
	pcap_close(p.handle); //deleta pacote resposavel de capturar outros pacotes
}
