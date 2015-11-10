#include "../include/lpcap.h"

lpcapType lpcap_init(string port){
	lpcapType p;
	char errbuf[PCAP_ERRBUF_SIZE];			/*Buffer de erros*/

	p.dev = "lo";						  		/*Loopback: si proprio*/
	p.filter_exp = "udp dst port " + port; 	  	/*protocolo UDP na porta indicada*/

	/* Obtém número da rede e máscara associada ao dispostivo*/
	if (pcap_lookupnet(p.dev.c_str(), &(p.net), &(p.mask), errbuf) == -1) {
		cout << "Nao pode obter mascara de rede para o dispositivo" << p.dev << " : " << errbuf << endl;
		p.net = 0;
		p.mask = 0;
	}
	cout << "Mascara de rede associada com sucesso" << endl;

	//Cria um pacote que consegue capturar outro pacote que esta na rede
	//device
	//tamanho dos pacotes em bytes
	//promiscuo?
	//timeout em segundos
	//mensagem de erro
	p.handle = pcap_open_live(p.dev.c_str(), 1000000, 0, 180, errbuf);
	if (p.handle == NULL) {
		cout << "Nao foi possivel abrir o dispositivo " << p.dev << " : " << errbuf << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Dispositivo pronto para uso" << endl;
	
	return p;
}

void lpcap_process(lpcapType p){


void lpcap_free(){

}
