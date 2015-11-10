/***************************************************************************************************

Universidade de Brasília
Departamento de Ciência da Computação
Transmissão de Dados 2014/2

Trabalho de implementação

Alunos: Keyla Kiyoku - 11/0126548
		Raissa Almeida Alves - 09/0138392
		
Para compilar: gcc head_or_tail.c -lnet -lpcap -o head_or_tail

Para executar: modo servidor: ./head_or_tail -s [port]
		modo cliente: ./head_or_tail host [port]

***************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libnet.h>
#include <pcap.h>
#include <stdint.h>
#include <time.h>

uint32_t src_ip, dst_ip;
uint16_t src_udp_port, dst_udp_port;
char *message;


int servidor (int src_port);
int cliente (int src_port);

int getDevideList ()
{
	int i = 0;
	char pcap_errbuf[PCAP_ERRBUF_SIZE];		/* pcap error message */ 
	pcap_if_t *alldevs, *d; 				/* device list pointers */
	
	
	/* Get device list */
	if (pcap_findalldevs (&alldevs, pcap_errbuf) == -1)	
    {
        fprintf (stderr,"pcap_findalldevs() failed: %s\n", pcap_errbuf);
        return 1;
    }    
    
    /* Print device list */
    
    printf ("\nLista de dispositivos disponíveis no seu sistema:\n\n"); 
    for (d = alldevs; d != NULL ; d = d->next)
    {
        printf ("%d. %s", ++i, d->name);
        if (d->description)
        	printf (" (%s)\n", d->description);
        else
            printf (" (Nenhuma descrição disponível)\n");
    }
	pcap_freealldevs (alldevs);
	return 0;
}

int setFilter (pcap_t *p, char dev[], char filter_exp[])
{
	struct bpf_program filter;				/* The compiled filter expression */
	
	
	if (pcap_compile(p, &filter, filter_exp, 0, 0) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(p));
		return 1;
	}
	
	if (pcap_setfilter(p, &filter) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(p));
		return 1;
	}
	
	return 0;
}

void gotPacket (char *user,struct pcap_pkthdr *hdr, const char *packet)
{
	//printf("Grabbed packet of length %d\n",hdr->len);

	
	
	//int i;	
	//struct libnet_ethernet_hdr *eptr;	/* ethernet header */
	//uint8_t *ptr; /* printing out hardware header info */
	
	struct libnet_ipv4_hdr *ip_hdr;
	
	struct libnet_udp_hdr *udp_hdr;
	
	
	
	/* lets start with the ether header... */
    /*eptr = (struct libnet_ethernet_hdr *) packet;

	ptr = eptr->ether_shost;
    i = ETHER_ADDR_LEN;
    printf("\nSource MAC Address:  ");
    do{
        printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
    }while(--i>0);
    printf("\n");

    ptr = eptr->ether_dhost;
    i = ETHER_ADDR_LEN;
    printf("\nDestination MAC Address:  ");
    do{
        printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
    }while(--i>0);
    printf("\n");
	*/
    
    
    /*******************************************************/
    ip_hdr = (struct libnet_ipv4_hdr *) (packet + LIBNET_ETH_H);
    
    src_ip = ip_hdr->ip_src.s_addr;
    //printf("\nSource IP address: %s\n", libnet_addr2name4 (src_ip, LIBNET_DONT_RESOLVE));
        
    dst_ip = ip_hdr->ip_dst.s_addr;
    //printf("\nDestination IP address: %s\n", libnet_addr2name4 (dst_ip, LIBNET_DONT_RESOLVE));
    
    /**********************************************************/    
    udp_hdr = (struct libnet_udp_hdr *) (packet + LIBNET_ETH_H + LIBNET_IPV4_H);
    
    src_udp_port = udp_hdr->uh_sport;
    src_udp_port = (src_udp_port>>8) | (src_udp_port<<8);
    //printf("\nSource port: %d\n", src_udp_port);
    dst_udp_port = udp_hdr->uh_dport;
    dst_udp_port = (dst_udp_port>>8) | (dst_udp_port<<8);
    //printf("\nDestination port: %d\n", dst_udp_port);
	
    /**********************************************************/
    message = (char *)(packet + LIBNET_ETH_H + LIBNET_IPV4_H + LIBNET_UDP_H);
    //printf ("\nMenssagem recebida: %s %d\n", message, (int)strlen(message));
	
}

libnet_ptag_t buildUDP (libnet_t *lnet, int src_p, int dst_p, char msg[], libnet_ptag_t udp_id)
{
	udp_id = libnet_build_udp (src_p,
							dst_p,
							LIBNET_UDP_H + (uint16_t)strlen(msg),
							0,
							(uint8_t*)msg,
							(uint16_t)strlen(msg),
							lnet,
							udp_id);
	
	return udp_id;
}

void sendPacket (libnet_t *lnet)
{
	int bytes_written;
	bytes_written = libnet_write(lnet);
    if (bytes_written != -1);
		//printf ("\n%d bytes written.\n\n", bytes_written);
    else
        fprintf (stderr, "libnet_write() failed: %s\n", libnet_geterror(lnet));
}

int main (int argc, char *argv[]) {

	int port;
	int r;
	
	if (argc == 1)
	{
		printf ("É necessário indicar o modo:\nServidor: -s [port]\nCliente: host [port]");
		return 1;
	}
	
	else
	{
		if (argc == 2)
			port = 22222;
		else if (argc == 3)
			port = atoi (argv[2]);
		
		
		if ((!strcmp (argv[1], "-S")) || (!strcmp (argv[1], "-s")))
		{
			r = servidor (port);
			return r;
		}
		else if (!strcmp (argv[1], "host"))
		{
			r = cliente (port);
			return r;
		}
		else
		{
			printf ("Argumento inválido\nÉ necessário indicar o modo:\nServidor: -s [port]\nCliente: host [port]");
			return 1;
		}
	}

}

int servidor (int src_port)
{
	char HEAD_OR_TAIL[5];
	char HEAD[5] = "HEAD";
	char TAIL[5] = "TAIL";
	
	HEAD[4] = '\0';
	TAIL[4] = '\0';
	
	char xXORs[5];
	int x, j;
    int s1[4];
    char s[5], s_[5];
    char aux[10], aux1[20];
    int gotHello = 0, gotSim = 0, chegou = 0;
    char yXORs_[5], y[5];

	/* Variáveis para uso da rede */
	char dev[20];							/* network interface name */
	//char dst_ip_addr_str[20], 
	char *src_ip_addr_str;				/* destination IP address name */
	uint32_t src_ip_addr, dst_ip_addr;  	/* source and destination IP address that libnet can read */
	struct libnet_ether_addr *mac_addr; 	/* source MAC address */
	
	/* Variáveis para criar um packet (libnet) */	
	libnet_t *l;							/* libnet context */
	libnet_ptag_t udp;						/* tag for udp header */
	char libnet_errbuf[LIBNET_ERRBUF_SIZE];	/* libnet error message */
	uint16_t dst_port;
	char payload[100];
	u_int16_t id;

	/* Variáveis para criar uma sessão de captura (libpcap) */
	pcap_t *p;								/* pcap session */
	char pcap_errbuf[PCAP_ERRBUF_SIZE];		/* pcap error message */ 
	char filter_exp[20];					/* The filter expression */
	
	printf ("\n%s\n",libnet_version());
	
	if (getDevideList()) 
		return 1;
	
	/* Ask interface */	
	printf ("\nEscolha um dispositivo com Ethernet header: ");		
	scanf ("%s", dev);
	
	//strcpy (dev, "wlan0");
	
	/* Open pcap session */
	p = pcap_open_live (dev, BUFSIZ, 0, 1000, pcap_errbuf);	
	if (p == NULL) {
		fprintf (stderr, "pcap_open_live() failed: %s\n", pcap_errbuf);
		return 1;
	}
	
	/* This program supports only Ethernet headers */
	if (pcap_datalink(p) != DLT_EN10MB) {	
		fprintf (stderr, "Dispositivo %s não possui suporte para Ethernet headers\n", dev);
		return 1;
	}

	/* Create libnet context for network layer */
	l = libnet_init(LIBNET_RAW4, dev, libnet_errbuf);		
	if (l == NULL) {
		fprintf (stderr, "libnet_init() failed: %s\n", libnet_errbuf);
		return 1;
	}

	printf ("\nConectado no modo SERVIDOR\n\n");

	/* Get source IP address */
	src_ip_addr = libnet_get_ipaddr4(l);	
	if (src_ip_addr != -1)
	{
		src_ip_addr_str = libnet_addr2name4 (src_ip_addr, LIBNET_DONT_RESOLVE);
		printf ("Endereço IP: %s\n", src_ip_addr_str);
	}
	else {
		fprintf (stderr, "libnet_get_ipaddr4() failed: %s\n", libnet_geterror(l));
		return 1;
	}

	/* Get destination IP address */
	/*
	printf ("Choose destination IP address (xxx.xxx.xxx.xxx): ");		
	scanf ("%s", dst_ip_addr_str);
	dst_ip_addr = libnet_name2addr4 (l, dst_ip_addr_str, LIBNET_DONT_RESOLVE);
	printf("\nDestination IP address: %s\n\n", libnet_addr2name4 (dst_ip_addr, LIBNET_DONT_RESOLVE));
	*/

	/*Get MAC address*/
	mac_addr = libnet_get_hwaddr(l);	
	if (mac_addr != NULL)
		printf ("Endereço MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
		mac_addr->ether_addr_octet[0],
		mac_addr->ether_addr_octet[1],
		mac_addr->ether_addr_octet[2],
		mac_addr->ether_addr_octet[3],
		mac_addr->ether_addr_octet[4],
		mac_addr->ether_addr_octet[5]);
	else
		fprintf (stderr, "libnet_get_hwaddr() failed: %s\n", libnet_geterror(l));

	printf ("Porta: %d\n\n", src_port);

/************************************ SERVIDOR ****************************************************/
	
/********************************* WAIT FOR HELLO *************************************************/
	/* Set filter */
	strcpy (aux1, "dst host ");
    strcat (aux1, src_ip_addr_str);
    strcpy (aux, " and dst port ");
    strcat (aux1, aux);
    sprintf (aux,"22222");
    strcat (aux1, aux);
    //printf ("%s\n", aux1);
    
	strcpy (filter_exp, aux1);
	if (setFilter (p, dev, filter_exp) == 1)
		return 1;
	/* Grab a packet */
	while (!gotHello)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
	
		if (!strncmp (message, "Hello",5))
		{
			printf ("Cliente: %s\n", message);
			gotHello = 1;
		}
	}
	dst_ip_addr = src_ip;
	//printf("\ngotHello Destination IP address: %s\n", libnet_addr2name4 (dst_ip_addr, LIBNET_DONT_RESOLVE));
	dst_port = src_udp_port;
/*****************************************Iniciar**************************************************/
	strcpy (payload, "Vamos começar?");
	
	/* Build UDP packet with payload */	
	udp = buildUDP (l, src_port, dst_port, payload, 0);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
	/* Build IPv4 packet */
    libnet_seed_prand(l);
	id = (u_int16_t)libnet_get_prand (LIBNET_PR16);

    if (libnet_build_ipv4 (LIBNET_IPV4_H + LIBNET_UDP_H + (uint16_t)strlen(payload),
    						0,
    						id,
    						0,
    						64,
    						IPPROTO_UDP,
    						0,
    						src_ip_addr,
    						dst_ip_addr,
    						NULL,
    						0,
    						l,
    						0) == -1)
    {
    	fprintf (stderr, "libnet_autobuild_ipv4() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
    /* Write into network */    
    sendPacket (l);
    printf ("Servidor: %s\n", payload);
    
/********************************** WAIT FOR REPLY ************************************************/
	/* Set filter */
    strcpy (aux1, "dst host ");
    strcat (aux1, src_ip_addr_str);
    strcpy (aux, " and dst port ");
    strcat (aux1, aux);
    sprintf (aux,"%d",src_port);
    strcat (aux1, aux);
    //printf ("%s\n", aux1);
    
	strcpy (filter_exp, aux1);
	if (setFilter (p, dev, filter_exp) == 1)
		return 1;
	/* Grab a packet */
	while (!gotSim)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);  
		if (!strncmp (message, "Sim", 3))
		{
			printf ("Cliente: %s\n", message);
			gotSim = 1;
		}
	}
    
/********************************** CHOOSE H OR T *************************************************/
	srand (time(NULL));
	x = rand() % 2;
    switch (x)
    {
    	case 0:
    		strncpy (HEAD_OR_TAIL, HEAD,4);
    		HEAD_OR_TAIL[4] = '\0';
    	break;
    	
    	case 1:
    		strncpy (HEAD_OR_TAIL, TAIL,4);
    		HEAD_OR_TAIL[4] = '\0';
    	break;
    	
    	default:
    		printf ("Erro na escolha de head_or_tail\n");
    	break;
    }

    for (j=3; j>=0; j--)
    {
		s1[j] = rand() % 256;
		s[j] = (char)s1[j];
		//printf ("\ns[%d]: %d",j, s[j]);
		xXORs[j] = s[j] ^ HEAD_OR_TAIL[j];
		//printf ("\nxXORs[%d]: %d", j, xXORs[j]);
	}
	
	s[4] = '\0';
	xXORs[4] = '\0';

/**************************************************************************************************/
	strcpy (payload, "Head or tail?");
	
	udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
    /* Write into network */    
    sendPacket (l);
    printf ("Servidor: %s\n", payload);
	
/******************************* SEND H OR T xor S ************************************************/
	strcpy (payload, xXORs);
	
	udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
    /* Write into network */    
    sendPacket (l);
    printf ("Servidor: xXORs: %s (0x%X%X%X%X)\n", payload, (uint8_t)xXORs[0],(uint8_t)xXORs[1],(uint8_t)xXORs[2],(uint8_t)xXORs[3]);

/********************************** WAIT FOR REPLY ************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			printf ("Cliente: %s\n", message);
			chegou = 1;
		}
	}
	chegou = 0;   
    
/********************************** WAIT FOR REPLY ************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			strncpy (yXORs_, message,4);
			printf ("Cliente: yXORs': %s (0x%X%X%X%X)\n", message, (uint8_t)yXORs_[0],(uint8_t)yXORs_[1],(uint8_t)yXORs_[2],(uint8_t)yXORs_[3]);
			yXORs_[4] = '\0';
			chegou = 1;
		}
	}
	chegou = 0;   
   
/*********************************** SEND S *******************************************************/
	strcpy (payload, HEAD_OR_TAIL);
	
	udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
    /* Write into network */    
    sendPacket (l);
    printf ("Servidor: %s\n", payload);
   
/*********************************** SEND S *******************************************************/
	strcpy (payload, s);
	
	udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
    /* Write into network */    
    sendPacket (l);
    printf ("Servidor: s: %s (0x%X%X%X%X)\n", payload, (uint8_t)s[0],(uint8_t)s[1],(uint8_t)s[2],(uint8_t)s[3]);
 
/********************************** WAIT FOR REPLY ************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			strncpy (s_, message,4);
			printf ("Cliente: s': %s (0x%X%X%X%X)\n", message, (uint8_t)s_[0],(uint8_t)s_[1],(uint8_t)s_[2],(uint8_t)s_[3]);
			s_[4] = '\0';
			chegou = 1;
		}
	}
	chegou = 0;   
 
/**************************************************************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			printf ("Cliente: %s\n", message);
			chegou = 1;
		}
	}
	chegou = 0;  
    

/**************************************************************************************************/
	for (j=0; j<4; j++)
    {
		y[j] = (char)s_[j] ^ yXORs_[j];
	}
	y[4] = '\0';
	
	if (!strncmp (HEAD_OR_TAIL, y, 4))
	   	strcpy (payload, "OK");
	else
		strcpy (payload, "NOK");
	
	udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
    /* Write into network */    
    sendPacket (l);
    printf ("Servidor: %s\n", payload);
    
/**************************************************************************************************/
	strcpy (payload, "Bye");
	
	udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
    /* Write into network */    
    sendPacket (l);
    printf ("Servidor: %s\n", payload);
    
/**************************************************************************************************/
  
    	printf ("\n");
	pcap_close(p);
	libnet_destroy(l);
	return 0;
}

int cliente (int src_port)
{
	char HEAD_OR_TAIL[5];
	char HEAD[5] = "HEAD";
	char TAIL[5] = "TAIL";
	
	HEAD[4] = '\0';
	TAIL[4] = '\0';
	
	char yXORs_[5];
	int x, j;
	int s1_[4];
	char s_[5], s[5];
	char aux[10], aux1[20];
	int gotComecar = 0, chegou = 0;
	char xXORs[5], xis[5];

	/* Variáveis para uso da rede */
	char dev[20];								/* network interface name */
	char dst_ip_addr_str[20], *src_ip_addr_str;					/* destination IP address name */
	uint32_t src_ip_addr, dst_ip_addr;  		/* source and destination IP address that libnet can read */
	struct libnet_ether_addr *mac_addr; 		/* source MAC address */
	
	/* Variáveis para criar um packet (libnet) */	
	libnet_t *l;							/* libnet context */
	libnet_ptag_t udp;						/* tag for udp header */
	char libnet_errbuf[LIBNET_ERRBUF_SIZE];	/* libnet error message */
	int dst_port = 22222;
	char payload[100];
	u_int16_t id;

	/* Variáveis para criar uma sessão de captura (libpcap) */
	pcap_t *p;								/* pcap session */
	char pcap_errbuf[PCAP_ERRBUF_SIZE];		/* pcap error message */ 
	char filter_exp[20];						/* The filter expression */
	
	printf ("\n%s\n",libnet_version());
		
	if (getDevideList()) 
		return 1;
	
	/* Ask interface */	
	printf ("\nEscolha um dispositivo com Ethernet header: ");		
	scanf ("%s", dev);
	
	//strcpy (dev, "wlan0");
	
	/* Open pcap session */
	p = pcap_open_live (dev, BUFSIZ, 0, 1000, pcap_errbuf);	
	if (p == NULL) {
		fprintf (stderr, "pcap_open_live() failed: %s\n", pcap_errbuf);
		return 1;
	}
	
	/* This program supports only Ethernet headers */
	if (pcap_datalink(p) != DLT_EN10MB) {	
		fprintf (stderr, "Dispositivo %s não possui suporte para Ethernet headers\n", dev);
		return 1;
	}

	/* Create libnet context for network layer */
	l = libnet_init(LIBNET_RAW4, dev, libnet_errbuf);		
	if (l == NULL) {
		fprintf (stderr, "libnet_init() failed: %s\n", libnet_errbuf);
		return 1;
	}

	/* Get destination IP address */
	printf ("\nEscolha endereço IP de destino (xxx.xxx.xxx.xxx): ");		
	scanf ("%s", dst_ip_addr_str);
	dst_ip_addr = libnet_name2addr4 (l, dst_ip_addr_str, LIBNET_DONT_RESOLVE);
	
	printf ("\nConectado no modo CLIENTE\n\n");

	/* Get source IP address */
	src_ip_addr = libnet_get_ipaddr4(l);	
	if (src_ip_addr != -1)
	{
		src_ip_addr_str = libnet_addr2name4 (src_ip_addr, LIBNET_DONT_RESOLVE);
		printf ("Endereço IP: %s\n", src_ip_addr_str);
	}
	else {
		fprintf (stderr, "libnet_get_ipaddr4() failed: %s\n", libnet_geterror(l));
		return 1;
	}

	

	/*Get MAC address*/
	mac_addr = libnet_get_hwaddr(l);	
	if (mac_addr != NULL)
		printf ("Endereço MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
		mac_addr->ether_addr_octet[0],
		mac_addr->ether_addr_octet[1],
		mac_addr->ether_addr_octet[2],
		mac_addr->ether_addr_octet[3],
		mac_addr->ether_addr_octet[4],
		mac_addr->ether_addr_octet[5]);
	else
		fprintf (stderr, "libnet_get_hwaddr() failed: %s\n", libnet_geterror(l));

	printf ("Porta: %d\n", src_port);


	printf("Endereço IP de destino: %s\n\n", libnet_addr2name4 (dst_ip_addr, LIBNET_DONT_RESOLVE));
		
/************************************** CLIENTE ***************************************************/

/************************************* SEND HELLO *************************************************/
	strcpy (payload, "Hello");
	
	/* Build UDP packet with payload */
	udp = buildUDP (l, src_port, dst_port, payload, 0);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
	/* Build IPv4 packet */
    libnet_seed_prand(l);
	id = (u_int16_t)libnet_get_prand (LIBNET_PR16);

    if (libnet_build_ipv4 (LIBNET_IPV4_H + LIBNET_UDP_H + (uint16_t)strlen(payload),
    						0,
    						id,
    						0,
    						64,
    						IPPROTO_UDP,
    						0,
    						src_ip_addr,
    						dst_ip_addr,
    						NULL,
    						0,
    						l,
    						0) == -1)
    {
    	fprintf (stderr, "libnet_autobuild_ipv4() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
    
	/* Write into network */
    sendPacket (l);
    printf ("Cliente: %s\n", payload);
    
/************************************* WAIT REPLY *************************************************/
    /* Set filter */
    strcpy (aux1, "dst host ");
    strcat (aux1, src_ip_addr_str);
    strcpy (aux, " and dst port ");
    strcat (aux1, aux);
    sprintf (aux,"%d",src_port);
    strcat (aux1, aux);
    
	strcpy (filter_exp, aux1);
	if (setFilter (p, dev, filter_exp) == 1)
		return 1;
	/* Grab a packet */
	while (!gotComecar)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if (!strncmp (message, "Vamos começar?",14))
		{
			printf ("Servidor: %s\n", message);
			gotComecar = 1;
		}
	}
	
	dst_port = src_udp_port;
	//printf ("UDP PORT: %d\n", dst_port);
	
	
/****************************** sim ************************************************/
	strcpy (payload, "Sim");
	
    udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
        
	/* Write into network */
    sendPacket (l);
    printf ("Cliente: %s\n", payload);
    
/********************************** WAIT FOR REPLY ************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			printf ("Servidor: %s\n", message);
			chegou = 1;
		}
	}
	chegou = 0;
	
/********************************** WAIT FOR REPLY ************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			strncpy (xXORs, message,4);
			printf ("Servidor: xXORs: %s (0x%X%X%X%X)\n", message, (uint8_t)xXORs[0],(uint8_t)xXORs[1],(uint8_t)xXORs[2],(uint8_t)xXORs[3]);
			xXORs[4] = '\0';
			chegou = 1;
		}
	}
	chegou = 0;
	
/********************************** CHOOSE H OR T *************************************************/
    srand (time(NULL));
    x = rand() % 2;
    switch (x)
    {
    	case 0:
    		strcpy (HEAD_OR_TAIL, HEAD);
    		HEAD_OR_TAIL[4] = '\0';
    	break;
    	
    	case 1:
    		strcpy (HEAD_OR_TAIL, TAIL);
    		HEAD_OR_TAIL[4] = '\0';
    	break;
    	
    	default:
    		printf ("Erro na escolha de head_or_tail\n");
    	break;
    }

    for (j=0; j<4; j++)
    {
		s1_[j] = rand() % 256;
		s_[j] = (char)s1_[j];
		//printf ("\ns_[%d]: %d",j, s_[j]);
		yXORs_[j] = (char)s_[j] ^ HEAD_OR_TAIL[j];
		//printf ("\nyXORs_[%d]: %d", j, yXORs_[j]);
	}
	s_[4] = '\0';
	yXORs_[4] = '\0';

/****************************** SEND H OR T xor S_ ************************************************/
	strcpy (payload, HEAD_OR_TAIL);
	
    udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
        
	/* Write into network */
    sendPacket (l);
    printf ("Cliente: %s\n", payload);

/****************************** SEND H OR T xor S_ ************************************************/
	strcpy (payload, yXORs_);
	
    udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
    {
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
        
	/* Write into network */
    sendPacket (l);
    printf ("Cliente: yXORs': %s (0x%X%X%X%X)\n", payload, (uint8_t)yXORs_[0],(uint8_t)yXORs_[1],(uint8_t)yXORs_[2],(uint8_t)yXORs_[3]);
    
/**************************************************************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			printf ("Servidor: %s\n", message);
			chegou = 1;
		}
	}
	chegou = 0;   

/********************************** WAIT FOR REPLY ************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			strncpy (s, message,4);
			printf ("Servidor: s: %s (0x%X%X%X%X)\n", message, (uint8_t)s[0],(uint8_t)s[1],(uint8_t)s[2],(uint8_t)s[3]);
			s[4] = '\0';
			chegou = 1;
		}
	}
	chegou = 0;

/********************************* SEND S_ ********************************************************/
	strcpy (payload, s_);
	
    udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
   	{
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
        
	/* Write into network */
    sendPacket (l);
    printf ("Cliente: s': %s (0x%X%X%X%X)\n", payload, (uint8_t)s_[0],(uint8_t)s_[1],(uint8_t)s_[2],(uint8_t)s_[3]);

/**************************************************************************************************/	
	for (j=0; j<4; j++)
    {
		xis[j] = (char)s[j] ^ xXORs[j];
	}
	xis[4] = '\0';
	
	if (!strncmp (HEAD_OR_TAIL, xis, 4))
	   	strcpy (payload, "OK");
	else
		strcpy (payload, "NOK");
	
    udp = buildUDP (l, src_port, dst_port, payload, udp);
	if (udp == -1)
   	{
    	fprintf (stderr, "libnet_build_udp() failed: %s\n", libnet_geterror(l));
    	return 1;
    }
        
	/* Write into network */
    sendPacket (l);
    printf ("Cliente: %s\n", payload);
	
/**************************************************************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			printf ("Servidor: %s\n", message);
			chegou = 1;
		}
	}
	chegou = 0;   
/**************************************************************************************************/
	/* Grab a packet */
	while (!chegou)
	{
		pcap_loop(p, 1, (pcap_handler)gotPacket, NULL);
		if ((src_ip == dst_ip_addr) && (src_udp_port == dst_port))
		{
			printf ("Servidor: %s\n", message);
			chegou = 1;
		}
	}
	chegou = 0;   

/**************************************************************************************************/
   
	printf ("\n");
	pcap_close(p);
	libnet_destroy(l);
	return 0;
}
