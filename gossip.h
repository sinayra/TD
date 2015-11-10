#include "include/lpcap.h"
#include <locale>
#include <string>

#define DEFAULT_PORT "10101"

using namespace std;

string buffer_payload; //Buffer que vai ter as mensagens desempacotadas

void host(int port); //Realiza operaçoes de cliente
void server(int port); //Realiza operacoes de servidor
void getMessage(string &message); //Retorna uma mensagem digitada no teclado

//lpcap
void waitPackage(string port); //Espera receber o pacote

//lnet
void sendPackage(string message, string port); //Enpacota mensagem e a envia
