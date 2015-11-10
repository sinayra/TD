#include "gossip.h"

void sendPackage(string message, string port){

}

void waitPackage(string port){
    lpcapType package;

    package = lpcap_init(port); 
    lpcap_process(package); 
    lpcap_free(); 
}

void getMessage(string &message){
    
    if(message.empty())
        getline(cin, message);
}

void server(string port){
    string buffer, message;

    //Aguarda HELLO do cliente
    waitPackage(port); 
    //Envia Hello
    sendPackage("HELLO CLT", port);

    /*POR ENQUANTO, DEFINIREMOS APENAS UMA MENSAGEM*/
    message.clear();
    waitPackage(port);  //Espera mensagem do cliente
    message = "ECHO: " + buffer_payload;

    buffer.clear();
    while(buffer.compare("OK")){ //enquanto mensagem nao for OK
        buffer.clear();
        sendPackage(message, port); //Envia mensagem
        waitPackage(port); //Espera OK/nova mensagem

        buffer = buffer_payload; //Mensagem que veio depois de esperar pacote
        message.clear();
        message = "ECHO: " + buffer_payload;
    }

    ///////////////////////////////////q
    sendPackage("BYE CLT", port);
    waitPackage(port); //Resposta do cliente
}

void host(string port){
    string buffer, message;

    sendPackage("HELLO SRV", port); //Envia HELLO para servidor
    waitPackage(port); //Aguarda resposta do servidor

    buffer.clear();
    //inicia modo de chat
    while(buffer.compare("BYE CLT")){ //enquanto nao for BYE
        buffer.clear();
        message.clear();
        getMessage(message);
        sendPackage(message, port); //envia mensagem
        waitPackage(port); //Espera resposta

        sendPackage("OK", port); //Recebeu resposta, envia que ta tudo ok
        waitPackage(port); //Espera resposta
        buffer = buffer_payload; //Mensagem que veio depois de esperar pacote
    }

    sendPackage("BYE SVR", port);
}

//argv[0]: Nome do programa
//argv[1]: modo de operaçao
//argv[2]: porta
int main(int argc, char *argv[]){
    string mode, port;

    if(argc < 2 || argc > 3){
        cout << "Quantidade de argumentos incorreta. Encerrando..." << endl;
        exit(EXIT_FAILURE);
    }

    port.clear();
    port = (argc == 2 ? DEFAULT_PORT : argv[2]);

    mode.clear();
    for(int i = 0; argv[1][i] != '\0'; i++){
        mode += toupper(argv[1][i]);
    }

    if(!mode.compare("-S")){
        cout << "Modo servidor escolhido. Porta: " << port << endl;
        server(port);
    }
    else if (!mode.compare("HOST")){
        cout << "Modo cliente escolhido. Porta: " << port << endl;
        host(port);
    }
    else{
        cout << "Opcao invalida. Encerrando..." << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
