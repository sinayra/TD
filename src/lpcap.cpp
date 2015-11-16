#include "../include/lpcap.h"

lpcapType lpcap_init(string port){
	lpcapType p;
	char errbuf[PCAP_ERRBUF_SIZE];			/*Buffer de erros*/

	p.dev = "lo";						  		/*Loopback: si proprio*/
	p.filter_exp = "udp dst port " + port; 	  	/*protocolo UDP na porta indicada*/

	//Cria um pacote que consegue capturar outro pacote que esta na rede
	//device
	//tamanho dos pacotes em bytes
	//promiscuo?
	//timeout em segundos
	//mensagem de erro
	p.handle = pcap_open_live(p.dev.c_str(), 1000000, 1, 180, errbuf);
	if (p.handle == NULL) {
		cout << endl << "[ERROR] Nao foi possivel abrir o dispositivo " << p.dev << " : " << errbuf << endl;
		exit(EXIT_FAILURE);
	}

	cout << endl << "[DEBUG] Dispositivo pronto para uso" << endl;

	/* Obtém número da rede e máscara associada ao dispostivo*/
	if (pcap_lookupnet(p.dev.c_str(), &(p.net), &(p.mask), errbuf) == -1) {
 		cout << endl << "[WARNING] Nao pode obter mascara de rede para o dispositivo" << p.dev << " : " << errbuf << endl;
 		p.net = 0;
 		p.mask = 0;
 	}

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

void lpcap_process(lpcapType p){
	stringstream ss;

	//Limita o trafico inspecionado
	if (pcap_setfilter(p.handle, &(p.fp)) == -1) {
		cout << endl << "[ERROR] Nao foi possivel inspecionar o trafico : " << pcap_geterr(p.handle) << endl;
		cout << "Encerrando..." << endl;
		exit(EXIT_FAILURE);
	}
	cout << endl << "[DEBUG] Rede filtrada" << endl;

	//pega proximo pacote disponivel
	ss << pcap_next(p.handle, &(p.header));
	p.package = ss.str();

	cout << endl << "[DEBUG] Tamanho do header: " << p.header.len << endl << "Pacote: " << p.package << endl;
	
}

void lpcap_free(lpcapType p){
	pcap_freecode(&(p.fp)); //limpa buffer de filtro
	pcap_close(p.handle); //deleta pacote resposavel de capturar outros pacotes
}
