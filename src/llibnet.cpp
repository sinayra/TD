#include "../include/llibnet.h"

void prepareAndSendPackage(string &message, string &port){


	libnet_t *l; //CONTEXTO LIBNET
	
	char errbuf[LIBNET_ERRBUF_SIZE], ip_addr_str[16] = "127.0.0.1";
	
	// INICIALIZAÇÃO DO CONTEXTO LIBNET
	l = libnet_init(
					LIBNET_RAW4, // TIPO DE INJEÇÃO
					"lo",  //INTERFACE
					errbuf // MENSAGEM DE ERRO
					);
	if ( l == NULL ) {
		//fprintf(stderr, "libnet_init() failed: %s\n", errbuf);
		cout << endl << "[ERROR] Falha ao inicializar libnet: " << errbuf << endl << "Encerrando..." << endl;
		exit(EXIT_FAILURE);
	}

	
	//PORTAS SERVIDOR E HOST
	u_short host_port, 
			server_port;

	// MENSSAGEM
	u_short payload_s;
	payload_s = message.length();
	int bytes_escritos;


	//Sring "127.0.0.1" convertida para 4 bytes 
	u_int32_t ip_addr = libnet_name2addr4(l, ip_addr_str, LIBNET_DONT_RESOLVE);
	if ( ip_addr == -1 ) {
		//printf("Erro na conversao do endereco.\n");
		cout << endl << "[ERROR] Erro na conversao do endereco. Encerrando..."  << endl;
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	//VARIAVEIS AUXILIAR NA CRIAÇÃO DOS CABEÇALHOS
	
	libnet_ptag_t udp = 0;


	//CABEÇALHO UDP ------------------------------------------------------------------------
	
	host_port = (u_short)atoi(port.c_str());
	//server_port = (u_short)atoi(port);
	server_port = host_port;

	udp = libnet_build_udp(server_port, host_port, LIBNET_UDP_H + payload_s, 0, (uint8_t*)message.c_str(), payload_s, l, udp);
	if (udp == -1)
	{
		//printf("Nao pode construir o cabecalho UDP: %s\n", libnet_geterror(l));
		cout << endl << "[WARNING] Nao pode construir o cabecalho UPD: " << libnet_geterror(l) << endl;
	}

	//----------------------------------------------------------------------------------------



	//CABEÇALHO IP----------------------------------------------------------------------------

	if (libnet_autobuild_ipv4(LIBNET_IPV4_H + payload_s + LIBNET_UDP_H, IPPROTO_UDP, ip_addr, l) == -1 ) {
		//printf("Nao pode construir o cabecalho IP: %s\n",\
		//libnet_geterror(l));
		cout << endl << "[ERROR] Nao pode construir o cabecalho IP: " << libnet_geterror(l) << endl << "Encerrando..." << endl;
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}
	//----------------------------------------------------------------------------------------



	//ESCREVE NO CANAL

	bytes_escritos = libnet_write(l);
	if ( bytes_escritos != -1 )
		//printf("%d bytes foram enviados.\n", bytes_escritos);
		cout << endl << "[DEBUG] " << bytes_escritos << " bytes foram enviados." << endl;
	else
		cout << endl << "[WARNING] Erro no envio do pacote: " << libnet_geterror(l) << endl;
		//printf("Erro no envio do pacote: %s\n", libnet_geterror(l));

	libnet_destroy(l);

}