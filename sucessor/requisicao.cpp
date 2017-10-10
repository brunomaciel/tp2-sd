#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "../bits/stdc++.h"
#define FIND 0
#define STORE 1
#define ERROR -2
#define MAX_ARQUIVOS 10
#define NOME_ARQ_LISTA_DISPONIVEIS "arquivos.txt"
using namespace std;

/*
$ g++ requisicao.cpp -w -lpthread -o req

$ ./req porta sucessor.txt
*/

int port;
char suc_reply[10000];

void *connection_handler(void *);
void readFile(char *name, char *res);
int identificaComando(char cmd[]);
int realizaStore(int K, string value, string conteudoArq);
int chavePertenceMeuRange(int key);
int join(char* nomeArquivo);
int armazenaArquivo(int key, string value);
string consultaMeusArquivos(int key);
void criaArquivo(string value, string conteudoArq);
int enviaComandoSucessor(int key, string value, char arquivo[], int comando);
int createSocket(int *sock);
int realizaFind(int key);

int K,num_nos,ID;
string valor;

typedef struct a {
	char IP[25];
	int PORT;
} HOST;

HOST sucessor;

int main(int argc , char *argv[]) {

    //Se não houver entrada de argumento via terminal
    if(argc < 3) {
        printf("[REQUISICAO]: Forma de uso: ./req PORT sucessor.txt\n");
        exit (EXIT_FAILURE);
    }

    ID = join(argv[2]);

    int socket_desc; //Socket utilizado pelo REQUISICAO para escuta
    int client_sock; //Socket recebido do cliente para comunicação
    int c = sizeof(struct sockaddr_in); //Armazenará tamanho da estrutura 'sockaddr_in'
    struct sockaddr_in server;
    struct sockaddr_in client;
    pthread_t thread_id;
    port = atoi(argv[1]); //Porta de escuta do REQUISICAO recebido via terminal

    //Criação de socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        printf("[REQUISICAO %i]: Socket não pode ser criado\n", port);
    }
    printf("[REQUISICAO %i]: Socket criado\n", port);

    //Prepara a estrutura 'sockaddr_in'
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("[REQUISICAO]: Erro ao aplicar 'bind'");
        return 1;
    }
    printf("[REQUISICAO %i]: Bind concluído\n", port);

    //Início do processo de escuta
    listen(socket_desc , 3);
    printf("[REQUISICAO %i]: Aguardando por requisições de conexão...\n", port);

    //Chegada e aceitação de conexão
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        printf("[REQUISICAO %i]: Conexão aceita\n", port);

        //Criação de thread para atender requisição
        if( pthread_create( &thread_id, NULL,  connection_handler, (void*) &client_sock) < 0) {
            perror("[REQUISICAO]: Não foi possível criar a thread");
            return 1;
        }

        pthread_join(thread_id , NULL);
        printf("[REQUISICAO %i]: Thread designada\n", port);
    }

    //Erro ao obter socket do cliente
    if (client_sock < 0) {
        perror("[REQUISICAO]: Aceitação de conexão falhou");
        return 1;
    }

    return 0;
}

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

void criaArquivo(string value, string conteudoArq) {
  ofstream FILE;
	FILE.open(value,std::ofstream::out); // cria o arquivo se não existe, se existe adiciona texto no final
	if (FILE.is_open()) {
		FILE << conteudoArq;
	} else {
		cout << "\nErro ao abrir arquivo " << value;
		exit(1);
	}
	FILE.close();
}

int realizaStore(int K, string value, string conteudoArq) {
  if (chavePertenceMeuRange(K)) {
    if (armazenaArquivo(K,value) == 0) {
        return 0;
    } else {
      criaArquivo(value,conteudoArq);
      return 1;
    }
  } else {
      char arq[conteudoArq.length()+1];
      strcpy(arq,conteudoArq.c_str());
      return enviaComandoSucessor(K,value,arq,STORE);
  }
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
		cout << "\nNenhum arquivo.txt disponível!\n\n";
		exit(0);
	}

	FILE.close();
	return "ERRO";
}

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

void *connection_handler(void *socket_desc) {

    int sock = *(int*)socket_desc; //Socket do cliente
    int read_size; //Variável para definir recebeu a mensagem
    char serverReply[10000]; //Resultado da busca do cliente obtido por este REQUISICAO
    char client_cmd[5000]; //Comando oriundo do cliente

    //Recebe mensagem do cliente
    while( (read_size = recv(sock , client_cmd , 2000 , 0)) > 0 ) {

        //Limita-se o vetor
        client_cmd[read_size] = '\0';
				string aux(client_cmd);

        int comando = client_cmd[0] - '0';
        if (comando == STORE) {

          string param1 = aux.substr(aux.find("(")+1);
    	    int K =  stoi(param1.substr(0, param1.find(",")));
          string param2 = param1.substr(param1.find(",")+1);
    	    string valor = param2.substr(0, param2.find(")"));
          string conteudoArq = aux.substr(aux.find(")")+1);

          cout << "[REQUISICAO " << port << "]: Comando a ser executado: STORE("+to_string(K)+","+valor+")\n";

          if (realizaStore(K,valor,conteudoArq)) {
            strcat(serverReply, "1 - Arquivo armazenado com sucesso!\n\0");
          } else {
            strcat(serverReply, "0 - Chave duplicada!\n\0");
          }

        } else if (comando == FIND) {
					string parametros = aux.substr(aux.find("(")+1);
	        K =  stoi(parametros.substr(0, parametros.find(")")));
					cout << "[REQUISICAO " << port << "]: Comando a ser executado: FIND("+to_string(K)+")\n";

					if (realizaFind(K)) {
            strcat(serverReply, "1 - Arquivo encontrado com sucesso!\n");
						strcat(serverReply,suc_reply);
						//strcat(serverReply,"\0");
          } else {
            strcat(serverReply, "0 - Nao encontrei arquivo com esta chave!\n\0");
          }

        }

				write(sock , serverReply , strlen(serverReply));

        //Limpa-se os buffers
        memset(client_cmd, 0, 5000);
        memset(serverReply, 0, 10000);
				memset(suc_reply, 0, 10000);

    }

    if(read_size == 0) {
        printf("[REQUISICAO %i]: Cliente desconectou\n", port);
        fflush(stdout);
    }
    else if(read_size == -1) {
        perror("[REQUISICAO]: Falha ao executar 'recv'");
    }

    //Encerra o socket
    close(sock);

    return 0;
}

int realizaFind(int key) {
	if (chavePertenceMeuRange(key)) {
		string caminho = consultaMeusArquivos(key);
		if (!caminho.compare("ERRO")) {
			return 0;
		} else {
			ifstream arq;
		  arq.open(caminho);
			if (arq.is_open()) {
				  suc_reply[0] = '#';
					strcat(suc_reply,caminho.c_str());
					int i=caminho.length()+1;
					suc_reply[i]='#';
					i++;
					char c;
	  			while (arq.get(c)) {
						suc_reply[i] = c;
						i++;
					}
					suc_reply[i]='\0';
			} else return 0;
			return 1;
		}
	} else {
		char arquivo[1];
		//cout << "Enviou para sucessor!";
		//return 0;
	  return enviaComandoSucessor(key,"",arquivo,FIND);
	}
}

void readFile(char *name, char *res) {
    int c;
    FILE *file;
    file = fopen(name, "r");
    if (file) {
        string aux;
        while ((c = getc(file)) != EOF){
            aux+=c;
        }
        strcpy(res, aux.c_str());
        fclose(file);
    }
}

int chavePertenceMeuRange(int key) {
  if (key > MAX_ARQUIVOS*(ID-1) && key <= MAX_ARQUIVOS*ID)
		return 1;
	else
		return 0;
}

int enviaComandoSucessor(int key, string value, char arquivo[], int comando) {
	struct sockaddr_in suc;
	char cmd[1000]; //Armazena-se comando obtido via terminal
	clock_t t1; //variáveis para medir tempo de latência
	clock_t t2;
	string aux = to_string(comando)+"("+to_string(key)+","+value+")";
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

					cout << "\nResposta do sucessor: " << suc_reply;

					memset(suc_reply,0,10000); //Enche o vetor suc_reply com 2 mil zeros

					close(sock); //Fecha-se o socket
					cout << "Tempo de latência: " << timeT <<"s"<< endl;

          return suc_reply[0] - '0';
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
