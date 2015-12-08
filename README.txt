Integrantes:
Alexandre Correia Mesquita de Oliveira - 12/0151294
Marina Andrade Lucena Holanda - 09/79091
Sinayra Pascoal Cotts Moreira - 10/0020666

Resumo:
	Este trabalho implementa uma chat entre servidor e cliente. A conexão só é iniciada quando o servidor recebe a mensagem "HELLO SRV" do cliente. Enquanto um está digitando a mensagem, o outro tenta capturar a mensagem por um tempo de 15 segundos. Se a mensagem não for enviada dentro deste tempo, alterna quem manda e quem recebe mensagem. Apenas o servidor pode encerrar a conexão com "BYE CLT".

Dependências:
	sudo apt-get install g++
	sudo apt-get install libnet1-dev
	sudo apt-get install libpcap-dev

Compilação:
	make gossip

Execução servidor (terminal 1):
	sudo ./gossip -S [porta]
Execução cliente (terminal 2):
	sudo ./gossip host [porta]
