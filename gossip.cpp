#include "include/lpcap.h"
#include "include/llibnet.h"
#include "include/log.h"
#include <locale>
#include <string>
#include <poll.h>

#define DEFAULT_PORT "10101"
#define TIMEOUT 15

using namespace std;

bool g_user; //variavel global que define se a mensagem atual foi escrita por um usuario ou mandada automaticamente

//llibnet
void sendPackage(string message, string &port){ //Enpacota mensagem e a envia
    prepareAndSendPackage(message, port);
}

//lpcap
string waitPackage(string &port){ //Espera receber o pacote
    lpcapType package;
    string message;
    int timeout = TIMEOUT; //timeout em segundos

    package = lpcap_init(port); 
    message = lpcap_process(package, timeout); 
    lpcap_free(package); 

    return message;
}

void getMessage(string &message){
    int timeout = TIMEOUT * 1000; //timeout em milisegundos
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI};

    cout << endl << "Digite a mensagem: ";
    cout.flush(); //forca escrever na tela o que ta em cout
    if( poll(&mypoll, 1, timeout) ) //espera por 15 segundos para receber mensagem
        getline(cin, message);
    else
        showLog(warning, "Timeout");
    
}

void server(string &port){
    string message_tx, message_rx;
    
    message_rx = waitPackage(port); //espera mensagens de boas vindas

    if(!message_rx.compare("HELLO SRV") && !g_user){ //se a primeira mensagem for HELLO SRV e for automatica
        cout << "[HOST]: " << message_rx << endl; //Cliente mandou hello

        sendPackage("HELLO CLT", port);
        g_user = false;

        do{
            message_rx.clear();
            message_rx = waitPackage(port); //espera nova mensagem

            if(!message_rx.empty())
                cout << "[HOST]: " << message_rx << endl; 

            message_tx.clear();
            getMessage(message_tx); //pega mensagem
            g_user = 1; //define que um usuario ta mandando mensagem
            sendPackage(message_tx, port); //envia mensagem
        }while(message_tx.compare( "BYE CLT")); //enquanto nao for bye

        do{
            message_rx.clear();
            message_rx = waitPackage(port);
        }while(message_rx.compare("BYE SRV") && !g_user); //enquanto nao for bye, espera mensagem automatica
        cout << "[HOST]: " << message_rx << endl;
    }
    else{
        showLog(error, "HOST nao encontrado. Encerrando...");
    }

}

void host(string &port){
    string message_rx, message_tx;

    g_user = false;
    sendPackage("HELLO SRV", port); //inicia conexao
    message_rx = waitPackage(port); //espera mensagens de boas vindas

    if(!message_rx.compare("HELLO CLT") && !g_user){ //se a primeira mensagem for HELLO CLT e for automatica
        cout << "[SERVER]: " << message_rx << endl;

        do{
            g_user = true;
            message_tx.clear();
            getMessage(message_tx); //pega mensagem
            sendPackage(message_tx, port); //envia mensagem
            
            message_rx.clear();

            message_rx = waitPackage(port); //espera echo
            if(!message_rx.empty())
                cout << "[SERVER]: " << message_rx << endl;
        }while(message_rx.compare("BYE CLT") && g_user); //enquanto nao for bye e enquanto servidor esta escrevendo mensagens como usuario

        g_user = false;
        sendPackage("BYE SRV", port); //envia bye
    }
    else{
        showLog(error, "SERVER nao encontrado. Encerrando...");
    }
}

//argv[0]: Nome do programa
//argv[1]: modo de operaçao
//argv[2]: porta
int main(int argc, char *argv[]){
    string mode, port;
    stringstream s;

    if(argc < 2 || argc > 3){
        showLog(error, "Quantidade de argumentos incorreta. Encerrando...");
        exit(EXIT_FAILURE);
    }

    port.clear();
    port = (argc == 2 ? DEFAULT_PORT : argv[2]);

    mode.clear();
    for(int i = 0; argv[1][i] != '\0'; i++){
        mode += toupper(argv[1][i]);
    }

    if(!mode.compare("-S")){
        cout << endl << "Modo servidor escolhido. Porta: " << port << endl;
        server(port);
    }
    else if (!mode.compare("HOST")){
        cout << endl << "Modo cliente escolhido. Porta: " << port << endl;
        host(port);
    }
    else{
        showLog(error, "Opcao invalida. Encerrando...");
        exit(EXIT_FAILURE);
    }

    return 0;
}
