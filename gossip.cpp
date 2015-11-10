#include "gossip.h"

int main(int argc, char *argv[]){
    int port;
    string mode;

    if(argc < 2 || argc > 3){
        cout << "Quantidade de argumentos incorreta. Encerrando..." << endl;
        exit(EXIT_FAILURE);
    }

    port = (argc == 2 ? DEFAULT_PORT : atoi(argv[2]));

    mode.clear();
    for(int i = 0; argv[1][i] != '\0'; i++){
        mode += toupper(argv[1][i]);
    }

    if(mode.compare("-S")){
        cout << "Servidor" << endl;
        //server(port);
    }
    else if (mode.compare("HOST")){
        cout << "Cliente" << endl;
        //host(port);
    }
    else{
        cout << "Opcao invalida. Encerrando..." << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
