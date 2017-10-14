
#include "bits/stdc++.h"
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_ARQUIVOS 10
#define NOME_ARQ_LISTA_DISPONIVEIS "arquivos.txt"
#define FIND 0
#define STORE 1
#define EXIT -1
#define ERROR -2

using namespace std;

//Funções
int join(char* nomeArquivo);
int menu();
void aguardandoOpcao(int ID);
void find(int key, int ID);
int gerarChave();
int chavePertenceMeuRange(int key, int ID);
void imprimeArquivo (string path);
string consultaMeusArquivos(int key);
void store(int key, string value, int ID);
string maiusculo(string str);
string retiraEspaco(string str);
void enviaStoreSucessor(int key, string value) ;
string enviaFindSucessor(int key);
int createSocket(int *sock);
void enviaComandoSucessor(int key, string value, char arquivo[], int comando);
int armazenaArquivo(int key, string value);

int K,num_nos;
string valor;
char resposta[10000];

//Composto de 'endereço IP' e 'porta'
typedef struct a {
	char IP[25];
	int PORT;
} HOST;

HOST sucessor;

/*****
sucessor.txt

Minha_Chave IP_Sucessor PORT_Sucessor

g++ teclado.cpp -w -o noTeclado -std=c++11, para executar c++11
./noTeclado sucessor.txt

*****/

int main(int argc , char *argv[]) {

  //Se a quantidade de parâmetros via terminal for insuficiente
  if(argc < 2) {
      printf("\nForma de uso: ./noTeclado sucessor.txt\n\n");
      exit (EXIT_FAILURE);
  }

	int ID = join(argv[1]);

	cout << "\nNovo Nó adicionado! ID: " << ID << "\n";

	aguardandoOpcao(ID);

}

void aguardandoOpcao(int ID) {
  int opc = menu();
	while(opc != EXIT) {
	    if (opc == FIND) {
	        try {
	            find(K,ID);
	        } catch (...) {
	            cout << "\nFormato: FIND(K)\n";
	        }
	    } else if (opc == STORE) {
	        try {
	            store(K,valor,ID);
	        } catch (...) {
	            cout << "\nFormato: STORE(K,V)\n";
	        }
	    }
	    opc = menu();
	}
}

string maiusculo(string str) {
	char *input;
	char *output;
	string out;

	input = new char[str.length()+1];
	output = new char[str.length()+1];
	strcpy(input,str.c_str());

	int i;

  for (i = 0; input[i]; i++) {
      output[i] = toupper(input[i]);
  }
  output[i] = '\0';

	out.clear();
	out.append(output);

	return out;
}


string retiraEspaco(string str) {
	char *input;
	char *output;
	string out;

	input = new char[str.length()+1];
	output = new char[str.length()+1];
	strcpy(input,str.c_str());

	int j = 0;
    for (int i = 0; input[i]; i++) {
        if (input[i] != ' ') {
					output[j] = input[i];
					j++;
				}
    }
    output[j] = '\0';

	out.clear();
	out.append(output);

	return out;
}

int menu() {
  string comando;
	cout << "\nInforme o comando (EXIT para encerrar): ";
	getline(cin,comando);

	comando = retiraEspaco(comando);

	string funcao = comando.substr(0, comando.find("(")); // pegando função

	funcao = maiusculo(funcao);

	if (funcao.compare("EXIT") == 0) {
	    return EXIT;
	} else if (funcao.compare("FIND") == 0) {
	    try {
	        string parametros = comando.substr(comando.find("(")+1);
	        K =  stoi(parametros.substr(0, parametros.find(")")));
	        return FIND;
	    } catch (...) {
	        cout << "\nFormato: FIND(K)\n";
	        return ERROR;
	    }
	} else if (funcao.compare("STORE") == 0) {
	    try {
    	    string param1 = comando.substr(comando.find("(")+1);
    	    K =  stoi(param1.substr(0, param1.find(",")));

					if (param1.find(",") == string::npos) {
						cout << "\nFormato: STORE(K,V)\n";
						return ERROR;
					}

    	    string param2 = param1.substr(param1.find(",")+1);
    	    valor = param2.substr(0, param2.find(")"));
    	    return STORE;

    	} catch (...) {
    	    cout << "\nFormato: STORE(K,V)\n";
    	    return ERROR;
	    }
	}
	//Não é nenhuma das opções válidas
	cout << "\nComando inválido!\n";
	return ERROR;
}

/*int gerarChave() {
  //https://github.com/orcaman/chord/tree/master/src
  return 0;
}*/

int join(char* nomeArquivo) {
		int chave = 0;
    //Lê do arquivo os dados de chave e do nó sucessor
    FILE *file;
    file = fopen(nomeArquivo, "r");
    if (file) {
        char IP[25]; int PORT;
				try {
				fscanf(file,"%d %d", &chave, &num_nos);
				fscanf(file,"%s %d", IP, &PORT);
        strcpy(sucessor.IP,IP);
        sucessor.PORT = PORT;
			} catch (...) {
				cout << "\nErro com a formatação do arquivo\n\n";
			}
        fclose(file);
    } else {
			cout << "\n Não foi possível ler o arquivo: "<< nomeArquivo << "!\n\n";
			exit(1);
		}

		return chave;

}

/* Considerando que o primeiro valor de chave é 1.
Assim, ex.:
N = 3
MAX_ARQUIVO = 10
Chaves de 01 a 10 pertencem ao nó com chave 1.
Chaves de 11 a 20 pertencem ao nó com chave 2.
Chaves de 21 a 30 pertencem ao nó com chave 3.
*/

int chavePertenceMeuRange(int key, int ID) {
  if (key > MAX_ARQUIVOS*(ID-1) && key <= MAX_ARQUIVOS*ID)
		return 1;
	else
		return 0;
}

void imprimeArquivo (string path) {
	ifstream FILE;
	FILE.open(path);
	if (FILE.is_open()) {
		string linha;
		while (!FILE.eof()) {
			getline(FILE,linha);
			if (!FILE.eof()) {
				cout << linha;
			}
	  }
	} else { cout << "\nNenhum arquivo no diretório!\n"; }
	FILE.close();
}

//Retorna 0 se já há um arquivo com essa chave, 1 se sucesso
int armazenaArquivo(int key, string value) {
	int statusRetorno = 0;
  ofstream FILE;
	FILE.open(NOME_ARQ_LISTA_DISPONIVEIS,std::ofstream::app); // cria o arquivo se não existe, se existe adiciona texto no final
	if (FILE.is_open()) {
		if (!consultaMeusArquivos(key).compare("ERRO")) { // se já não tem arquivo com essa chave salvo
			FILE << key << " " << value << endl;
			statusRetorno = 1;
		}
	} else {
		cout << "\nErro ao abrir arquivo " << NOME_ARQ_LISTA_DISPONIVEIS;
		exit(1);
	}
	FILE.close();
	return statusRetorno;
}

void find(int key, int ID) {

	if (key <= 0 || key>(num_nos*MAX_ARQUIVOS)) {
		cout << "\nChave dos arquivos deve ser maior que 0 e menor que "<< (num_nos*MAX_ARQUIVOS)+1 <<"!\n";
	}
	else {
    if (chavePertenceMeuRange(key,ID)) {
      string caminho = consultaMeusArquivos(key); // não compete com requisicao.cpp rodando localmente, pois é leitura
			if (!caminho.compare("ERRO")) {
        cout << "\nArquivo não encontrado! \n";
      } else {
				cout << "\nArquivo encontrado!";
				cout << "\nCaminho: " << caminho;
				cout << "\nConteúdo: \n";
        imprimeArquivo(caminho);
				cout << "\n";
      }
    } else {
			string caminhoLocal = enviaFindSucessor(key);
			if (!caminhoLocal.compare("ERRO")) cout << "\nArquivo não encontrado!\n";
			else {
				cout << "\nConteudo: ";
				imprimeArquivo(caminhoLocal);
			}
    }

	}

}

string enviaFindSucessor(int key) {
	  char arquivo[1];
	  enviaComandoSucessor(key,"",arquivo,FIND);
		int resp = resposta[0] - '0';
		if (resp == 0) {
			memset(resposta,0,10000);
			return "ERRO";
		} else if (resp == 1) {
			int i;
			for (i=0; resposta[i]!='#'; i++);
			i++;
			string caminhoLocal = "";
			for (i; resposta[i]!='#'; i++) {
				caminhoLocal += resposta[i];
			}
			i++;
			cout << "\nCaminho local: " << caminhoLocal << "\n";
			ofstream FILE;
			FILE.open(caminhoLocal,std::ofstream::out); // cria o arquivo se não existe, se existe adiciona texto no final
			if (FILE.is_open()) {
				for (i; resposta[i]!='\0'; i++){ // se já não tem arquivo com essa chave salvo
					FILE << resposta[i];
				}
			}
			memset(resposta,0,10000);
			return caminhoLocal;
	}
}

void enviaComandoSucessor(int key, string value, char arquivo[], int comando) {
	struct sockaddr_in suc;
	char cmd[1000]; //Armazena-se comando obtido via terminal
	char suc_reply[10000];
	clock_t t1; //variáveis para medir tempo de latência
	clock_t t2;
	string aux;
	if (comando == STORE) aux = to_string(comando)+"("+to_string(key)+","+value+")";
	else if (comando == FIND) aux = to_string(comando)+"("+to_string(key)+")";
	strcpy(cmd, aux.c_str());
	if (comando == STORE) strcat(cmd,arquivo);
	int sock;
	cout << "\nTentativa de conexão";
	cout << "\nEndereço IP: " << sucessor.IP << " Porta: " << sucessor.PORT << " Socket: "<< sock << endl;

	//Cria-se o socket
	createSocket(&sock);

	//Limpa-se estruturas
	bzero((char *) &suc, sizeof(suc));

	//Define-se dados da conexão
	suc.sin_addr.s_addr = inet_addr( sucessor.IP );
	suc.sin_family = AF_INET;
	suc.sin_port = htons( sucessor.PORT );

	//Tentativa de conexão com servidor remoto
	if (connect(sock , (struct sockaddr *)&suc , sizeof(suc)) < 0) {
			perror("Conexão falhou\n");
	} else { //Se bem sucedido

			puts("Conectou\n");

			t1 = clock(); //Armazena-se o tempo de início

			//Envio de mensagem para o servidor
			if( send(sock , cmd , strlen(cmd) , 0) < 0) {
					puts("Envio falhou");
			}

			//Recebe resposta do servidor
			if( recv(sock , suc_reply , 2000 , 0) < 0) {
					puts("Falha ao executar 'recv'");
			} else {
					//Bloco de sucesso: nada falhou!
					t2 = clock(); //Armazena-se tempo de fim

					//Calcula-se tempo de duração da conversa
					double timeT = (((double)t2 - (double)t1)/(double)CLOCKS_PER_SEC);

					string aux(suc_reply);
					string param1 = aux.substr(0, aux.find("!")+1);
					cout << "\nResposta do sucessor: \n" << param1;

					strcpy(resposta,suc_reply);

					memset(suc_reply,0,10000); //Enche o vetor suc_reply com 2 mil zeros

					close(sock); //Fecha-se o socket
					cout << "\nTempo de latência: " << timeT <<"s"<< endl;
			}
	}
}

void enviaStoreSucessor(int key, string value) {
		ifstream arq;
	  arq.open(value);
		if (arq.is_open()) {
				//Converter arquivo em char[]
				int max_length = 1000;
				char arquivo[max_length];
				int i=0;
				char c;
  			while (arq.get(c)) {
					if (i>=max_length) {
							cout << " \nTamanho maximo de caracteres para transmissao: " << max_length;
							exit(1);
					}
					arquivo[i] = c;
					i++;
				}
				arquivo[i]='\0';

				enviaComandoSucessor(key,value,arquivo,STORE);
		} else cout << "\nNão foi possível abrir arquivo para enviar para sucessor!\n";
}

string consultaMeusArquivos(int key) {
	ifstream FILE;
	FILE.open(NOME_ARQ_LISTA_DISPONIVEIS);
  int c;
	if (FILE.is_open()) {
		string linha;
		while (!FILE.eof()) {
			getline(FILE,linha);
			if (!FILE.eof()) {
				c = stoi(linha.substr(0, linha.find(" ")));
				if (c == key) return linha.substr(linha.find(" ") + 1);
			}
	  }
	} else {
		cout << "\nNenhum arquivo disponível!\n\n";
		exit(0);
	}

	FILE.close();
	return "ERRO";
}

void store(int key, string value, int ID) {

	if (key <= 0 || key>(num_nos*MAX_ARQUIVOS)) {
		cout << "\nChave dos arquivos deve ser maior que 0 e menor que "<< (num_nos*MAX_ARQUIVOS)+1 <<"!\n";
	}
	else {
	  if (chavePertenceMeuRange(key,ID)) {

	    if (armazenaArquivo(key,value) == 0) {
	      cout << "\nJá existe arquivo com essa chave!\n";
	    } else {
				cout << "\nArquivo disponibilizado com sucesso!\n";
			}
	  } else {
			    enviaStoreSucessor(key,value);
	  }
  }
}

int createSocket(int *sock) {
    *sock = socket(AF_INET , SOCK_STREAM , 0);
    if (*sock == -1) {
        printf("Não foi possível criar socket");
    }
    puts("Socket criado");
}
