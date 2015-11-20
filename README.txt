Integrantes:
Alexandre Correia Mesquita de Oliveira - 12/0151294
Marina Andrade Lucena Holanda - 09/79091
Sinayra Pascoal Cotts Moreira - 10/0020666

Resumo:

Dependências:
	sudo apt-get install libnet1-dev
	sudo apt-get install libpcap-dev

Compilação:
	make gossip

Execução servidor (terminal 1):
	sudo ./gossip -S [porta]
Execução cliente (terminal 2):
	sudo ./gossip host [porta]
