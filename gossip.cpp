#include "include/lpcap.h"
#include "include/llibnet.h"
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
    int timeout = 15;

    package = lpcap_init(port); 
    message = lpcap_process(package, timeout); 
    lpcap_free(package); 

    //cout << endl << "[DEBUG] Mensagem recebida: " << message << endl;
    return message;
}

void getMessage(string &message){
    
    cout << endl << "Digite a mensagem: ";
    if(message.empty())
        getline(cin, message);
}

void server(string &port){
    string message;
    
    message = waitPackage(port); //espera mensagens de boas vindas

    if(!message.compare("HELLO SRV")){ //se a primeira mensagem for HELLO SRV
        cout << "[HOST]: " << message << endl; //Cliente mandou hello

        sendPackage("HELLO CLT", port);

        do{
            
            message = waitPackage(port); //espera nova mensagem
            if(!message.empty()){
                cout << "[HOST]: " << message << endl; 
                sendPackage("<ECHO> '" + message + "'", port); //envia ECHO
            }
            
        }while(!message.empty());

        do{
            sendPackage("BYE CLT", port); //envia bye
            message.clear();
            message = waitPackage(port);
        }while(message.compare("BYE SRV")); //enquanto nao for bye, espera mensagem
        cout << "[HOST]: " << message << endl;
    }

}

void host(string &port){
    string message;

    sendPackage("HELLO SRV", port); //inicia conexao
    message = waitPackage(port); //espera mensagens de boas vindas

    if(!message.compare("HELLO CLT")){ //se a primeira mensagem for HELLO CLT
        cout << "[SERVER]: " << message << endl;

        do{
            message.clear();
            getMessage(message); //pega mensagem
            sendPackage(message, port); //envia mensagem
            message.clear();

            message = waitPackage(port); //espera echo
            cout << "[SERVER]: " << message << endl;
        }while(message.compare("BYE CLT")); //enquanto nao for bye
    }
    sendPackage("BYE SRV", port); //envia bye
}

//argv[0]: Nome do programa
//argv[1]: modo de operaçao
//argv[2]: porta
int main(int argc, char *argv[]){
    string mode, port;

    if(argc < 2 || argc > 3){
        cout << endl << "[ERROR] Quantidade de argumentos incorreta. Encerrando..." << endl;
        exit(EXIT_FAILURE);
    }

    port.clear();
    port = (argc == 2 ? DEFAULT_PORT : argv[2]);

    mode.clear();
    for(int i = 0; argv[1][i] != '\0'; i++){
        mode += toupper(argv[1][i]);
    }

    if(!mode.compare("-S")){
        cout << endl << "[DEBUG] Modo servidor escolhido. Porta: " << port << endl;
        server(port);
    }
    else if (!mode.compare("HOST")){
        cout << endl << "[DEBUG] Modo cliente escolhido. Porta: " << port << endl;
        host(port);
    }
    else{
        cout << endl << "[ERROR] Opcao invalida. Encerrando..." << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
