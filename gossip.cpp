#include "include/lpcap.h"
#include "include/llibnet.h"
#include "include/log.h"
#include <locale>
#include <string>
#include <poll.h>

#define DEFAULT_PORT "10101"
#define TIMEOUT 15

using namespace std;

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
    string message_aux;

    cout << endl << "Digite a mensagem: ";
    cout.flush(); //forca escrever na tela o que ta em cout
    if( poll(&mypoll, 1, timeout) ) //espera por 15 segundos para receber mensagem
        getline(cin, message_aux);
    else
        showLog(warning, "Timeout");

    message = "1" + message_aux;
    
}

void server(string &port){
    string message_tx, message_rx;
    
    message_rx = waitPackage(port); //espera mensagens de boas vindas

    if(!message_rx.compare("0HELLO SRV")){ //se a primeira mensagem for HELLO SRV e for automatica
        message_rx.erase(0, 1); //apaga identificador de mensagem
        cout << "[HOST]: " << message_rx << endl; //Cliente mandou hello

        sendPackage("0HELLO CLT", port);

        do{
            message_rx.clear();
            message_rx = waitPackage(port); //espera nova mensagem

            if(!message_rx.empty()){
                message_rx.erase(0, 1);
                cout << "[HOST]: " << message_rx << endl; 
            }

            message_tx.clear();
            getMessage(message_tx); //pega mensagem
            sendPackage(message_tx, port); //envia mensagem
        }while(message_tx.compare( "1BYE CLT")); //enquanto nao for bye
        
            do{
                message_rx.clear();
                message_rx = waitPackage(port);
            }while(message_rx.compare("0BYE SRV"));
            message_rx.erase(0, 1); //apaga identificador de mensagem
            cout << "[HOST]: " << message_rx << endl;

    }
    else{
        showLog(error, "HOST nao encontrado. Encerrando...");
    }

}

void host(string &port){
    string message_rx, message_tx;

    sendPackage("0HELLO SRV", port); //inicia conexao
    message_rx = waitPackage(port); //espera mensagens de boas vindas

    if(!message_rx.compare("0HELLO CLT") ){ //se a primeira mensagem for HELLO CLT
        message_rx.erase(0, 1);
        cout << "[SERVER]: " << message_rx << endl;

        do{
            message_tx.clear();
            getMessage(message_tx); //pega mensagem
            sendPackage(message_tx, port); //envia mensagem
            
            message_rx.clear();

            message_rx = waitPackage(port); //espera echo
            if(!message_rx.empty()){
                message_rx.erase(0, 1);
                cout << "[SERVER]: " << message_rx << endl;
            }

        }while(message_rx.compare("BYE CLT")); //enquanto nao for bye e enquanto servidor esta escrevendo mensagens como usuario

        sendPackage("0BYE SRV", port); //envia bye
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
