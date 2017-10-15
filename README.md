/******************************************************************************************/
/** CENTRO FEDERAL DE EDUCAÇÃO TECNOLÓGICA DE MINAS GERAIS                              ***/
/** ENGENHARIA DE COMPUTAÇÃO 					                        ***/
/** Sistemas Distribuídos								***/
/** Prof. Anolan Barrientos								***/
/** Alunos: Arley dos Santos Ribeiro, Bruno Marques Maciel, Cassiano de Brito Andrade	***/
/** 2017-2										***/
/*******************************************************************************************

Trabalho Prático 2: P2P

## Ambiente de Execução

Desenvolveu-se o projeto na máquina virtual VirtualBox 5.1.28 com o sistema operacional Linux mint 17.3 Cinnamon OEM (64 bits versão Ubuntu). O compilador utilizado foi o g++. Para instalá-lo, deve-se executar o comando abaixo no terminal.

	$ sudo apt-get install g++

## Execução

Para executar o arquivo servidores, execute:

	$ g++  requisicao.cpp  -w  -lpthread  -o  req  -std=c++11
	
	$ ./req  porta  sucessor.txt

Em seguida, deve-se atualizar o arquivo "sucessor.txt" com o endereço IP seguido da porta refrente à cada próximo nó.

### Exemplo do arquivo servidores.txt com 4 peers

1 4 192.168.1.114 8880
2 4 192.168.1.109 8881
3 4 192.168.1.110 8882
4 4 192.168.1.111 8883

Para executar o cliente, execute: 

	$ g++  teclado.cpp  -w  -o  noTeclado  -std=c++11
	
	$ ./noTeclado  sucessor.txt

