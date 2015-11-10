 _____    _____       ___   _____       ___  ___   _____  
|  _  \  | ____|     /   | |  _  \     /   |/   | | ____| 
| |_| |  | |__      / /| | | | | |    / /|   /| | | |__   
|  _  /  |  __|    / / | | | | | |   / / |__/ | | |  __|  
| | \ \  | |___   / /  | | | |_| |  / /       | | | |___  
|_|  \_\ |_____| /_/   |_| |_____/ /_/        |_| |_____| 

Trabalho de Transmissão de Dados.

Alunos: 

ARTUR BORGES PIO - 11/0058607
RODRIGO DE LIMA CARVALHO – 09/0131401

-------------------------------------------
Diretivas de compilação e execução:


Para compilar o programa corretamente, é necessário ter
em sua máquina as bibliotecas LIBNET e LIBPCAP devidamente
instaladas.

Para isso, basta rodar no terminal:

$sudo apt-get install libnet1-dev

$sudo apt-get install libpcap-dev

Uma vez com as bibliotecas instaladas, vá ao diretório onde
o arquivo HorT.c está armazenado e digite no terminal:

$gcc HorT.c -o HorT -lnet -lpcap -fno-stack-protector


Em seguida, execute o programa (como super usuário).

Para funcionamento ideal, utilizar dois terminais,
um rodando o programa em modo host

$sudo ./HorT host

e outro como server:

$sudo ./HorT -S

no terminal do CLIENT, digitar endereço IP da máquina que
está rodando o SERVER.
