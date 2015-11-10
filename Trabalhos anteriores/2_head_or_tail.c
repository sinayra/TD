/*
 Alunos:    Helton Jose de Almeida Costa - 09/0116135
            Gabriel Coelho do Amaral     - 09/0114329
 
 Tramissão de Dados - 2/2014
 Professor: João Gondim
 
 Descrição: Implementação de aplicativo de Heads or Tail entre servidor e host utilizando
 protocolo de transporte UDP.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#define BUFFSIZE 2048
#define TRUE 1

/*  ======================================================================================
    ======================================================================================

                                ROTINA PARA SERVIDOR

    ======================================================================================
    ====================================================================================*/

int UDP_Server(int port){
    
    //declaracao de variaveis
    struct sockaddr_in serv_address, host_address;
    socklen_t sizehost = sizeof(host_address);
    char *temp1;
    int recover_length, server_socket, passo = 0, temp, i;
    char buff[BUFFSIZE], buf2[BUFFSIZE];
    char x[5],S[5], MSG1[5];
    
    // inicializa socket para sever
    if((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("Nao foi possivel criar o socket do server\n");
        return 0;
    }
    // inicializa server
    memset((char *)&serv_address, 0, sizeof(serv_address));
    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_address.sin_port = htons(port);
    
    //realiza bind do server com socket
    if(bind(server_socket, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0){
        printf("Processo de bind do server falhou\n");
        return 0;
    }
    
    while(TRUE){
        
        srand(time(NULL));
        
        /* =======================================================
                            PRIMEIRO PASSO
           =======================================================  */
        
        if(passo == 0){
            
            printf("==============================================================");
            printf("\n \t Aguardando comunicacao na porta %d ...\n", port);
            printf("==============================================================\n");
            
            recover_length = recvfrom(server_socket, buff, BUFFSIZE, 0, (struct sockaddr *)&host_address, &sizehost);   //recebe dados do host
            
            if(recover_length > 0){
                
                /* =======================================================
                                CHECAGEM PRIMEIRO PASSO
                 ======================================================= */
                
                if( strcmp(buff, "Hello") == 0){
                    
                    printf("PRIMEIRO PASSO \n");
                    
                    printf("Hello received!\n");
                    
                    temp = rand() % 101;    //obtem um valor aleatorio para gerar saida
                    if( temp >= 50 ){
                        
                        sprintf(x, "HEAD");
                        
                        printf("x = HEAD\n");
                    }else{
                        
                        sprintf(x, "TAIL");
                        
                        printf("x = TAIL\n");
                    }
                    
                    sprintf(S, "1234");
                    
                    printf("S = 1234\n");
                
                    for(i = 0; i<4; i++){
                        MSG1[i] = x[i] ^ S[i];
                    }
                
                    MSG1[4] = '\0';
                    
                    printf("MSG = %s\n", MSG1);

                    printf("================================================\n");
                    printf("\t\t ENVIANDO MENSAGEM ... ");
                    printf("\n ================================================\n");
                    
                    if(sendto(server_socket, MSG1, strlen(MSG1), 0, (struct sockaddr *)&host_address, sizehost) < 0){ //envia mensagem para host
                        printf("\n Falha ao enviar mensagem\n");
                        return 0;
                    }
                    
                    passo++;    // acrescenta passo
                
            
                
                }else{
                    printf("\t Erro ao receber mensagem. \n");
                    return 0;
                }
            }
            
            /* =======================================================
                            FIM DO PRIMEIRO PASSO
             =======================================================  */
            
            printf("==============================================================");
            printf("\n \t Aguardando resposta do host ...\n");
            printf("==============================================================\n");
            
            /* =======================================================
                                SEGUNDO PASSO
             =======================================================  */
            
        }else if(passo == 1){
            
            printf("SEGUNDO PASSO \n");
            
            // recebe mensagem do host
            recover_length = recvfrom(server_socket, buff, BUFFSIZE, 0, (struct sockaddr *)&host_address, &sizehost);
            
            if(recover_length > 0){
                
                buff[recover_length] = '\0';
                printf("Mensagem recebida: %s\n", buff);
                
                printf(" ================================================\n");
                printf("\t\t ENVIANDO MENSAGEM ... ");
                printf("\n ================================================\n");
                
                // enviar mensagem para host
                if(sendto(server_socket, S, strlen(S), 0, (struct sockaddr *)&host_address, sizehost) < 0){
                    
                    printf("FALHA AO ENVIAR MENSAGEM.");
                    return 0;
                }
                
            }else{
                printf("ERRO AO RECEBER MENSAGEM. \n\n");
                return 0;
            }
            
            passo++;    // incrementa o passo
            
            /* =======================================================
                                FIM DO SEGUNDO PASSO
             =======================================================  */
            
            printf("==============================================================");
            printf("\n \t Aguardando resposta do host ...\n");
            printf("==============================================================\n");
            
        }else if(passo == 2){
            
            /* =======================================================
                                    TERCEIRO PASSO
             =======================================================  */
            
            printf("TERCEIRO PASSO \n");
            
            recover_length = recvfrom(server_socket, buf2, BUFFSIZE, 0, (struct sockaddr *)&host_address, &sizehost);
            
            
            if(recover_length > 0){
                buf2[recover_length] = '\0';
            }else{
                printf("ERRO AO RECEBER MENSAEM\n");
                return 0;
            }
            
            printf("Mensagem Recebida: %s\n", buf2);
            
            
            recover_length = recvfrom(server_socket, buf2, BUFFSIZE, 0, (struct sockaddr *)&host_address, &sizehost);
            
            
            if(recover_length > 0){
                buf2[recover_length] = '\0';
            }else{
                printf("ERRO AO RECEBER MENSAEM\n");
                return 0;
            }
            
            temp1 = strtok(buf2, " ");
            if((strcmp(temp1, "OK")) == 0){
                sprintf(buff, "OK - Bye");
            }
            else{
                sprintf(buff, "NOK ");
            }
            
            printf("\n\tMENSAGEM FINAL: \"%s\"\n\n", buf2);
         
            if(sendto(server_socket, buff, strlen(buff), 0, (struct sockaddr *)&host_address, sizehost) < 0){ //envia mensagem para host
                printf("Falha ao enviar mensagem\n\n");
                return 0;
            }
            
            passo++;
            /* =======================================================
                            FIM DO TERCEIRO PASSO
             =======================================================  */
            
        }else if(passo == 3){
            return 0;
        }
    }
}

/*  ======================================================================================
    ======================================================================================
 
                                ROTINA PARA HOST
 
    ======================================================================================
    ====================================================================================*/

int UDP_Host(int port){
    
    // declaracao de variaveis
    struct sockaddr_in hostaddr, servaddr;
    char *temp1;
	int hostsock, recvlen, passo = 0;
    socklen_t sizeserv=sizeof(servaddr);
	char buf[BUFFSIZE], buf2[BUFFSIZE], *ip = "0.0.0.0";
    char x[5],S[5], S2[5], MSG1[5],MSG2[5], VER[5], i, NotOk, temp;
    
    // inicializa socket para host
    if((hostsock=socket(AF_INET, SOCK_DGRAM, 0))==-1){
		perror("\n Nao foi possivel criar o socket do host. \n");
		return 0;
    }
    
    // inicializa host
    memset((char *)&hostaddr, 0, sizeof(hostaddr));
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	hostaddr.sin_port = htons(0);
    
    // realiza bind do host com socket
    if(bind(hostsock, (struct sockaddr *)&hostaddr, sizeof(hostaddr)) < 0){
		perror("\n Processo de bind do host falhou. \n");
		return 0;
    }
    
    // inicializa server
    memset((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
    
    // atribuicao de ip para servidor
    if(inet_aton(ip, &servaddr.sin_addr)==0) {
		fprintf(stderr, "\n Falha ao atribuir endereco de ip ao servidor. \n");
		exit(1);
	}
    
    while(TRUE){
        if(passo == 0){
            
            /* =======================================================
                                PRIMEIRO PASSO
             =======================================================  */
            
            printf("PRIMEIRO PASSO ");
            
            // cria mensagem para ser enviada
            sprintf(buf, "Hello");
            
            printf("\n Enviando mensagem: %s", buf);
            printf("\n IP: %s", ip);
            printf("\n Porta: %d ", port);
            
            // envia mensagem para server
            if(sendto(hostsock, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeserv) <0){
                printf("FALHA AO ENVIAR MENSAGEM.\n");
            }
            
            /* =======================================================
                                FIM DO PRIMEIRO PASSO
             =======================================================  */
            
            printf("\n==============================================================");
            printf("\n \t Aguardando resposta do server ...\n");
            printf("==============================================================\n");
            
            passo++;
        }else if(passo == 1){
            /* =======================================================
                                    SEGUNDO PASSO
             =======================================================  */
            
            printf("SEGUNDO PASSO \n");
            
            
            recvlen = recvfrom(hostsock, buf, BUFFSIZE, 0, (struct sockaddr *)&servaddr, &sizeserv);
            
            if(recvlen > 0){
                buf[recvlen] = '\0';
                printf("Mensagem recebida: \"%s\" \n", buf);
            }else{
                printf("\n ERRO AO RECEBER MENSAEM\n");
                return 0;
            }
            
            for(i = 0; i<4; i++){
                MSG1[i] = buf[i];
            }
            
            MSG1[4] = '\0';
            
            /* =======================================================
                                CALCULO SEGUNDO PASSO
             =======================================================  */
            
            srand(time(NULL));
            
            temp = rand() % 101;    //obtem um valor aleatorio para gerar saida
            if( temp >= 50){
                sprintf(x, "HEAD");
                
                printf("y = HEAD\n");
            }else{
                sprintf(x, "TAIL");
                
                printf("y = TAIL\n");
            }
            
            sprintf(S, "4321");
            printf("S' = 4321\n");
            
            for(i=0; i<4; i++){
                MSG2[i] = x[i] ^ S[i];
            }
            
            MSG2[4] = '\0';
            
            printf("MSG' = %s\n", MSG2);
            
            printf(" ================================================\n");
            printf("\t\t ENVIANDO MENSAGEM ... \n");
            printf("\n ================================================\n");
            
            if(sendto(hostsock, MSG2, strlen(MSG2), 0, (struct sockaddr *)&servaddr, sizeserv) < 0){ //envia mensagem para host
                printf("\nFalha ao enviar mensagem\n\n");
                return 0;
            }
            
            printf("==============================================================");
            printf("\n \t Aguardando resposta do server ...\n");
            printf("==============================================================\n");
            
            /* =======================================================
                                FIM DO SEGUNDO PASSO
             =======================================================  */
            
            passo++;
        }else if(passo == 2){
            
            /* =======================================================
                                    TERCEIRO PASSO
             =======================================================  */
            
            printf("TERCEIRO PASSO \n");
            
            recvlen = recvfrom(hostsock, buf2, BUFFSIZE, 0, (struct sockaddr *)&servaddr, &sizeserv);
            
            if(recvlen > 0){
                buf[recvlen] = '\0';
                printf("Mensagem recebida: \"%s\"\n", buf2);
            }else{
                printf("ERRO AO RECEBER MENSAEM\n");
                return 0;
            }
            
            for(i=0; i<4; i++){
                VER[i] = x[i] ^ buf2[i];
            }
            
            for(i= 0 ; i<4 ; i++)
            {
                if(VER[i] != MSG1[i])
                {
                    NotOk = 1;
                }
            }
            
            printf("VER = %s\n", VER);
            
            if(NotOk == 1)
            {
                sprintf(buf,"NOK");
            }
            else
            {
                sprintf(buf, "OK ");
            }
            
            if(sendto(hostsock, S, strlen(S), 0, (struct sockaddr *)&servaddr, sizeserv) < 0){ //envia mensagem para server
                printf("\n\n Falha ao enviar mensagem\n\n");
                return 0;
            }
            
            
            if(sendto(hostsock, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeserv) < 0){ //envia mensagem para server
                printf("\n\n Falha ao enviar mensagem\n\n");
                return 0;
            }
            
            printf("\n==============================================================");
            printf("\n \t Aguardando resposta do server ...\n");
            printf("==============================================================\n");
            
            /* =======================================================
                                FIM DO TERCEIRO PASSO
             =======================================================  */
            passo++;
        }else if(passo == 3){
            /* =======================================================
                                QUARTO PASSO
             =======================================================  */
            
            printf("QUARTO PASSO\n");
            
            recvlen = recvfrom(hostsock, buf2, BUFFSIZE, 0, (struct sockaddr *)&servaddr, &sizeserv);
            
            if(recvlen > 0){
                buf[recvlen] = '\0';
                printf("\nMENSAGEM FINAL: \"%s\"\n\n", buf2);
            }else{
                printf("ERRO AO RECEBER MENSAEM\n");
                return 0;
            }
            
            /* =======================================================
                            FIM DO QUARTO PASSO
             =======================================================  */
            
            passo++;
        }else if(passo == 4){
            return 0;
        }
        
    }
    
}

/*  ======================================================================================
    ======================================================================================
 
                                ROTINA PRINCIPAL
 
    ======================================================================================
    ====================================================================================*/

int main(int argc, char *argv[]){
	
	int port;
	
	if(argc == 3)
		port = atoi(argv[2]);
	else
		port = 22222;
	
	if(!strcmp(argv[1], "-S")){
		if(!UDP_Server(port))
			return 0;
	}
	else{
		if(!strcmp(argv[1], "host")){
			if(!UDP_Host(port))
				return 0;
		}
		else{
			printf("\nO small-talk deve possuir a seguinte configuracao:\n'-S' para executar o servidor\nou\n'host' para executar o cliente\n");
			return 0;
		}
	}
	return 1;
}