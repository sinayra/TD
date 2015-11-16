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
void waitPackage(string &port){ //Espera receber o pacote
    lpcapType package;

    package = lpcap_init(port); 
    lpcap_process(package); 
    lpcap_free(package); 
}

void getMessage(string &message){
    
    if(message.empty())
        getline(cin, message);
}

void server(string &port){

}

void host(string &port){
    sendPackage("HELLO", port);
    waitPackage(port);
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
