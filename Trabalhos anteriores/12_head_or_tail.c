#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <netinet/if_ether.h>

#include <pcap.h>
#include <libnet.h>

#define DEFAULT_PORT 22222
#define AUX_PORT 11111
#define BUFSIZE 16
#define MSGSIZE 4
#define MAXBYTES2CAPTURE 2048

char *ok = "OK";
char *nok = "NOK";
char *msgHeadOrTail[MSGSIZE] = {"HEAD","TAIL"};

/************************* Struct Types *************************/

struct UDP_hdr {
	u_short	uh_sport;		/* source port */
	u_short	uh_dport;		/* destination port */
	u_short	uh_ulen;		/* datagram length */
	u_short	uh_sum;			/* datagram checksum */
};

struct Retorno {
	u_short	src_port;               		/* source port */
	u_short	dst_port;               		/* destination port */
	int udp_pckt;                           /* read sucessfull a udp packet? */
    char src_ip[INET_ADDRSTRLEN];           /* source ip in the packet */
	char dst_ip[INET_ADDRSTRLEN];           /* destine ip in the packet? */
	char *Msg;		                        /* Msg received */
};

/************************* Receiver Function*************************/
void processPacket(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet){ 
	struct ip *ip;
	struct UDP_hdr *udp;
	unsigned int IP_header_length;

    unsigned int capture_len = pkthdr->len;

    struct Retorno *rsp = (struct Retorno *)arg;
    
    //printf("\nFull packet: %d \n", capture_len);
	/* For simplicity, we assume Ethernet encapsulation. */
	if (capture_len < sizeof(struct ether_header))
		{
		/* We didn't even capture a full Ethernet header, so we
		 * can't analyze this any further.
		 */
		//fprintf(stderr, "Too short Ethernet header \n");
		return;
		}

	/* Skip over the Ethernet header. */
	packet += sizeof(struct ether_header);
	capture_len -= sizeof(struct ether_header);

	if (capture_len < sizeof(struct ip))
		{ /* Didn't capture a full IP header */
		//fprintf(stderr, "Too short IP header \n");
		return;
		}

    //printf("Without EtherHeader: %d \n", capture_len);
	ip = (struct ip*) packet;
	IP_header_length = ip->ip_hl * 4;	/* ip_hl is in 4-byte words */

	if (capture_len < IP_header_length)
		{ /* didn't capture the full IP header including options */
		//fprintf(stderr, "Too short IP header with options \n");
		return;
		}

	if (ip->ip_p != IPPROTO_UDP)
		{
		//fprintf(stderr, "non-UDP packet \n");
		return;
		}

	/* Skip over the IP header to get to the UDP header. */
	packet += IP_header_length;
	capture_len -= IP_header_length;

	if (capture_len < sizeof(struct UDP_hdr))
		{
		//fprintf(stderr, "Too short UDP header \n");
		return;
		}

	udp = (struct UDP_hdr*) packet;

/*	printf("UDP src_port=%d dst_port=%d length=%d\n",
		ntohs(udp->uh_sport),
		ntohs(udp->uh_dport),
		ntohs(udp->uh_ulen));
*/	
	
	packet += sizeof(struct UDP_hdr*);
	rsp->udp_pckt = 1;
	rsp->src_port = ntohs(udp->uh_sport);
	rsp->dst_port = ntohs(udp->uh_dport);
	rsp->Msg = (char*) packet;
	inet_ntop(AF_INET, &(ip->ip_src), rsp->src_ip, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(ip->ip_dst), rsp->dst_ip, INET_ADDRSTRLEN);
}

void RcvMsg(pcap_t *descr, int port, struct Retorno *rsp){    
    int found = 0;
   
    while(!found){
        rsp->udp_pckt = 0;
        pcap_loop(descr, 1, processPacket, (u_char *)rsp);
        if ((rsp->udp_pckt==1) && (rsp->dst_port == port)){
            found = 1;
            //printf("Src Ip: %s \n", rsp->src_ip);
            //printf("Dst Ip: %s \n", rsp->dst_ip);
        }
    }
}

/*************************Sender Function*************************/
void SendMsg(libnet_t *l, char *payload, long unsigned int pay_len, int src_port, int dst_port, char *ip_addr_str){
    //printf("Sendinging on src_port: %d e dst_port: %d \n", src_port, dst_port);
    u_int32_t ip_addr;
	u_int16_t id;
    int bytes_written;
   	//char ip_addr_str[16] = "127.0.0.1";
	
	libnet_seed_prand (l);
	id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

	/* Getting destination IP address */
	ip_addr = libnet_name2addr4(l, ip_addr_str, LIBNET_DONT_RESOLVE);

	if ( ip_addr == -1 ) {
		fprintf(stderr, "Error converting IP address.\n");
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Building UDP header */
    if (libnet_build_udp(
        src_port, /* source port */
        dst_port, /* destination port */
        LIBNET_UDP_H + pay_len, /* packet length */
        0, /* checksum */
        (uint8_t*)payload, /* payload */
        pay_len, /* payload size */
        l, /* libnet handle */
        0) /* libnet id */ == -1)
    {
        fprintf(stderr, "Can't build UDP header: %s\n", libnet_geterror(l));
        libnet_destroy(l);
        exit(EXIT_FAILURE);
    }

	if ( libnet_autobuild_ipv4(LIBNET_IPV4_H + LIBNET_UDP_H +\
			pay_len, IPPROTO_UDP, ip_addr, l) == -1 )
	{
		fprintf(stderr, "Error building IP header: %s\n",\
				libnet_geterror(l));
		libnet_destroy(l);
		exit(EXIT_FAILURE);
	}

	/* Writing packet */

	bytes_written = libnet_write(l);
	if ( bytes_written == -1 )
		fprintf(stderr, "Error writing packet: %s\n",\
				libnet_geterror(l));
}

/*************************Server*************************/


/*
  Cliente                                               Servidor
Envia Hello            ------------------------->
                              	                  Escolhe aleatoriamente x = HEAD ou TAIL
                                                  e uma sequencia de 4 bytes S
                       <------------------------  Envia Msg = x XOR S
Escolhe aleatoriamente
y = HEAD ou TAIL,
e uma sequencia de
4 bytes S´
Envia Msg´ = y XOR S´  ------------------------>
                       <------------------------  Envia S
Envia S´ e OK se y=x ou
NOK caso contrário     ------------------------>
                       <------------------------  Envia Bye e OK se y=x ou NOK caso contrário
 */

void sendHeadOrTail(pcap_t *descr, int src_port, libnet_t *l, char *ip)
{
        int m;
	    char S[BUFSIZE];
	    char msg[BUFSIZE];
	    char hostMsg[BUFSIZE];
        char receivedBuf[BUFSIZE];
        int recvlen;
		int i;

        
        struct Retorno rsp;
        
        //Recebe Hello
        RcvMsg(descr, src_port, &rsp);
        strcpy(receivedBuf, rsp.Msg);
	    printf("Received: %s (%d bytes)\n", receivedBuf, strlen(receivedBuf));
		
		int dst_port = rsp.src_port;
		ip = rsp.src_ip;
		
		//Escolhe aleatoriamente x = HEAD ou TAIL e uma sequencia de 4 bytes S
		m = rand()%2;
		for(i = 0; i < MSGSIZE; i++)
		{
		    S[i] = (char)(rand()%256);// char => 8bits => 256 values
		    msg[i] = msgHeadOrTail[m][i] ^ S[i];
		}

		S[MSGSIZE] = 0;/* final de string igual a 0 */
		msg[MSGSIZE] = 0;

		//Envia Msg = x XOR S
        printf("Sending: %s(%s) (%d bytes)\n", msg, msgHeadOrTail[m],strlen(msg));
        SendMsg(l,msg,sizeof(msg),src_port, dst_port, ip);

        //Recebe Msg´ = y XOR S´
        RcvMsg(descr, src_port, &rsp);
        strcpy(hostMsg, rsp.Msg);
	    printf("Received: %s (%d bytes)\n",  hostMsg, strlen(hostMsg));

        //Envia S
        printf("Sending: %s (%d bytes)\n", S, strlen(S));
        SendMsg(l,S,sizeof(S), src_port, dst_port, ip);

        //Recebe S´
        RcvMsg(descr, src_port, &rsp);
        strcpy(receivedBuf, rsp.Msg);
	    printf("Received: %s (%d bytes)\n", receivedBuf, strlen(receivedBuf));

		//x XOR S'
	    for(i = 0; i < MSGSIZE; i++)
	    {                     //x               //S'
		    msg[i] = msgHeadOrTail[m][i] ^ receivedBuf[i];
	    }
	    msg[MSGSIZE] = 0;/* final de string igual a 0 */

        //Recebe OK se y=x ou NOK caso contrário
        RcvMsg(descr, src_port, &rsp);
        strcpy(receivedBuf, rsp.Msg);
	    printf("Received: %s (%d bytes)\n", receivedBuf, strlen(receivedBuf));

        //Envia OK se y=x ou NOK caso contrário
	    if(strcmp(msg, hostMsg) == 0)
	    {
	        printf("Sending: %s (%d bytes)\n", ok,strlen(ok));
            SendMsg(l,ok,sizeof(ok), src_port, dst_port, ip);
	    }
	    else
	    {
	        printf("Sending: %s (%d bytes)\n", nok,strlen(nok));
            SendMsg(l,nok,sizeof(nok), src_port, dst_port, ip);
	    }

        //Envia Bye
        char *buf = "Bye";
        printf("Sending: %s (%d bytes)\n", buf,strlen(buf));
        SendMsg(l,buf,sizeof(buf), src_port, dst_port, ip);
}

/*
Configura o servidor e entra em um loop esperando o cliente

*/
int runServer(int port, char* device, char* ip)
{
    /* ==== Initialize LibPcap ==== */
    pcap_t *descr = NULL; 
    char errbuf[PCAP_ERRBUF_SIZE]; 
    memset(errbuf,0,PCAP_ERRBUF_SIZE); 

    printf("Opening device %s\n", device); 

    /* Open device in promiscuous mode */ 
    if ( (descr = pcap_open_live(device, MAXBYTES2CAPTURE, 1,  512, errbuf)) == NULL){
        fprintf(stderr, "ERROR: %s\n", errbuf);
        exit(1);
    }
    
    
    /* ==== Initialize LibNet ==== */
    libnet_t *l;	/* libnet context */
	char errlbuf[LIBNET_ERRBUF_SIZE];
    
	l = libnet_init(LIBNET_RAW4, device, errlbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errlbuf);
		exit(EXIT_FAILURE);
	}
    
	/* now loop, receiving data and printing what we received */
	while(1) {
	    printf("\nWaiting on port %d\n", port);
	    sendHeadOrTail(descr, port, l, ip);
	}
	 /* never exits */
    
    return 0;
}

/*************************Client*************************/

/*
  Cliente                                               Servidor
Envia Hello            ------------------------->
                              	                  Escolhe aleatoriamente x = HEAD ou TAIL
                                                  e uma sequencia de 4 bytes S
                       <------------------------  Envia Msg = x XOR S
Escolhe aleatoriamente
y = HEAD ou TAIL,
e uma sequencia de
4 bytes S´
Envia Msg´ = y XOR S´  ------------------------>
                       <------------------------  Envia S
Envia S´ e OK se y=x ou
NOK caso contrário     ------------------------>
                       <------------------------  Envia Bye e OK se y=x ou NOK caso contrário
 */

void sayHello(pcap_t *descr, int dst_port, libnet_t *l,char *ip)
{
	    int m;
	    char S[BUFSIZE];
	    char msg[BUFSIZE];
	    char serverMsg[BUFSIZE];
        char receivedBuf[BUFSIZE];
        int recvlen;
        int i;
        
        int src_port = AUX_PORT;
        //int src_port = dst_port;
        struct Retorno rsp;
	
        //Envia Hello
        char *buf = "Hello";
        printf("Sending: %s (%d bytes)\n", buf,strlen(buf));
        SendMsg(l,buf,sizeof(buf), src_port, dst_port, ip);
        
        //Recebe Msg = x XOR S
        RcvMsg(descr, src_port, &rsp);
        strcpy(serverMsg, rsp.Msg);
	    printf("Received: %s (%d bytes)\n", serverMsg, strlen(serverMsg));
		
		//Escolhe aleatoriamente x = HEAD ou TAIL e uma sequencia de 4 bytes S'
		m = rand()%2;
		for(i = 0; i < MSGSIZE; i++)
		{
		    S[i] = (char)(rand()%256);// char => 8bits => 256 values
		    msg[i] = msgHeadOrTail[m][i] ^ S[i];
		}
	    S[MSGSIZE] = 0;
	    msg[MSGSIZE] = 0;

		//Envia msg = x XOR S'
        printf("Sending: %s(%s) (%d bytes)\n", msg, msgHeadOrTail[m],strlen(msg));  
        SendMsg(l,msg,sizeof(msg), src_port, dst_port, ip);

        //Recebe S
        RcvMsg(descr, src_port, &rsp);
        strcpy(receivedBuf, rsp.Msg);
        printf("Received: %s (%d bytes)\n", receivedBuf, strlen(receivedBuf));
        
        //x XOR S
        for(i = 0; i < MSGSIZE; i++)
	    {                   //x                 //S
		    msg[i] = msgHeadOrTail[m][i] ^ receivedBuf[i];
	    }
	    msg[MSGSIZE] = 0;

        //Envia S´
        printf("Sending: %s (%d bytes)\n", S,strlen(S));
        SendMsg(l,S,sizeof(S), src_port, dst_port, ip);
        
	    //Envia OK se y=x ou NOK caso contrário
	    if(strcmp(msg, serverMsg) == 0)
	    {
	        printf("Sending: %s (%d bytes)\n", ok,strlen(ok));
            SendMsg(l,ok,sizeof(ok), src_port, dst_port, ip);
	    }
	    else
	    {
	        printf("Sending: %s (%d bytes)\n", nok,strlen(nok));
            SendMsg(l,nok,sizeof(nok), src_port, dst_port, ip);
	    }

        //Recebe OK se y=x ou NOK caso contrário
        RcvMsg(descr, src_port, &rsp);
        strcpy(receivedBuf, rsp.Msg);
	    printf("Received: %s (%d bytes)\n", receivedBuf, strlen(receivedBuf));

	    //Recebe Bye
        RcvMsg(descr, src_port, &rsp);
        strcpy(receivedBuf, rsp.Msg);
	    printf("Received: %s (%d bytes)\n", receivedBuf, strlen(receivedBuf));
}

/*
Configura o client e começa a comunicação
*/
int runClient(int port, char* device, char* ip)
{
    /* ==== Initialize LibPcap ==== */
    pcap_t *descr = NULL; 
    char errbuf[PCAP_ERRBUF_SIZE]; 
    memset(errbuf,0,PCAP_ERRBUF_SIZE); 

    printf("Opening device %s\n", device); 

    /* Open device in promiscuous mode */ 
    if ( (descr = pcap_open_live(device, MAXBYTES2CAPTURE, 1,  512, errbuf)) == NULL){
        fprintf(stderr, "ERROR: %s\n", errbuf);
        exit(1);
    }
    
    
    /* ==== Initialize LibNet ==== */
    libnet_t *l;	/* libnet context */
	char errlbuf[LIBNET_ERRBUF_SIZE];
    
	l = libnet_init(LIBNET_RAW4, device, errlbuf);
	if ( l == NULL ) {
		fprintf(stderr, "libnet_init() failed: %s\n", errlbuf);
		exit(EXIT_FAILURE);
	}
   
	/* now let's send the messages */
	printf("\nSending on port %d\n", port);
	sayHello(descr,port,l, ip);
	return 0;
}

int main(int argc, char **argv)
{
    // Ultimate -S src_port device ip /dst_port
    // Ultimate -H dst_port device ip /src_port
    
    int port = DEFAULT_PORT;
    char* host;
    char *ip = "127.0.0.1"; 
    char* device = "lo";
    
    switch (argc){ 
        case 5:
            ip = argv[4];
        case 4:
            device = argv[3];
        case 3:
            port = atoi(argv[2]);
        case 2:
            host = argv[1];
            break;
        default:
            printf("Invalid number of arguments.\n");
            return 0;
    }
    
    if(strcmp(host, "-S") == 0)
    {
        runServer(port,device, ip);
    }
    else if(strcmp(host, "Host") == 0)
    {
        runClient(port,device, ip);
    }
 
    return 0;
}
