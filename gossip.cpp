#include "include/lpcap.h"
#include "include/llibnet.h"
#include "include/log.h"
#include <locale>
#include <string>

#define DEFAULT_PORT "10101"

using namespace std;


//llibnet
void sendPackage(string message, string &port){ //Enpacota mensagem e a envia
    prepareAndSendPackage(message, port);
}

//lpcap
string waitPackage(string &port){ //Espera receber o pacote
    lpcapType package;
    string message;
    int timeout = 15; //timeout em segundos

    package = lpcap_init(port); 
    message = lpcap_process(package, timeout); 
    lpcap_free(package); 

    return message;
}

void getMessage(string &message){
    
    cout << endl << "Digite a mensagem: ";
    if(message.empty())
        getline(cin, message);
}

void server(string &port){
    string message_tx, message_rx;
    
    message_rx = waitPackage(port); //espera mensagens de boas vindas

    if(!message_rx.compare("HELLO SRV")){ //se a primeira mensagem for HELLO SRV
        cout << "[HOST]: " << message_rx << endl; //Cliente mandou hello

        sendPackage("HELLO CLT", port);

        do{
            message_rx.clear();
            message_rx = waitPackage(port); //espera nova mensagem

            if(!message_rx.empty())
                cout << "[HOST]: " << message_rx << endl; 

            message_tx.clear();
            getMessage(message_tx); //pega mensagem
            sendPackage(message_tx, port); //envia mensagem
        
            
        }while(message_tx.compare( "BYE CLT")); //enquanto nao for bye

        do{
            message_rx.clear();
            message_rx = waitPackage(port);
        }while(message_rx.compare("BYE SRV")); //enquanto nao for bye, espera mensagem
        cout << "[HOST]: " << message_rx << endl;
    }
    else{
        showLog(error, "HOST nao encontrado. Encerrando...");
    }

}

void host(string &port){
    string message_rx, message_tx;

    sendPackage("HELLO SRV", port); //inicia conexao
    message_rx = waitPackage(port); //espera mensagens de boas vindas

    if(!message_rx.compare("HELLO CLT")){ //se a primeira mensagem for HELLO CLT
        cout << "[SERVER]: " << message_rx << endl;

        do{
            message_tx.clear();
            getMessage(message_tx); //pega mensagem
            sendPackage(message_tx, port); //envia mensagem
            
            message_rx.clear();

            message_rx = waitPackage(port); //espera echo
            if(!message_rx.empty())
                cout << "[SERVER]: " << message_rx << endl;
        }while(message_rx.compare("BYE CLT")); //enquanto nao for bye

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
