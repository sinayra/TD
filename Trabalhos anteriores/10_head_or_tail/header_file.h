//interfaces de funções
//int converterArgumentos(int argc, char **argv, int *porta);
void executarServidor(int porta);
void controlerServidor(int local_socket);
void executarCliente(int port);
void controleCliente(int local_socket, struct sockaddr_in *remote_address, socklen_t *address_size);
int  abrirSocket(char *ip, int port);
void receberMensagem(int local_socket, char *buffer, struct sockaddr_in *remote_address, socklen_t *address_size);
void enviarMensagem(int local_socket, char *buffer, int message_size, struct sockaddr_in *remote_address, socklen_t remote_address_size);
//void imprimirArray(int *array);
void arrayHEAD(int *array);
void arrayTAIL(int *array);

//definindo as constantes
//tamanho do buffer
#define BUFFER_SIZE 1024
//ip padrão
#define IP_PADRAO "127.0.0.1"
//número da porta padrão
#define PORTA_PADRAO 22222

//número aleatorio de s
//#define VALOR_S 4294967295
#define VALOR_S 1000000000