#include <iostream>
#include <string>
#include <fstream>

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
int enviaStoreSucessor(int key, string value) ;
string enviaFindSucessor(int key);

int K,num_nos;
string valor;

//Composto de 'endereço IP' e 'porta'
typedef struct a {
	char IP[25];
	int PORT;
} HOST;

HOST sucessor;

/*****
sucessor.txt

Minha_Chave IP_Sucessor PORT_Sucessor

*****/

int main(int argc , char *argv[]) {

  //Se a quantidade de parâmetros via terminal for insuficiente
  if(argc < 2) {
      printf("\n[CLIENTE]: Forma de uso: ./noTeclado sucessor.txt\n\n");
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
	comando = maiusculo(comando);

	string funcao = comando.substr(0, comando.find("(")); // pegando função

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
				cout << "\nArquivo encontrado - Conteúdo: \n";
        imprimeArquivo(caminho);
				cout << "\n";
      }
    } else {
			string caminhoLocal = enviaFindSucessor(key);
			if (caminhoLocal.compare("ERRO")) cout << "\nArquivo não encontrado!\n";
			else imprimeArquivo(caminhoLocal);
    }

	}

}

string enviaFindSucessor(int key) {
	  //recupera o arquivo, cria um arquivo local que é cópia dele, e retorna caminho desse arquivo; se nao achar, retorna ERRO
		return "ERRO";
}

int enviaStoreSucessor(int key, string value) {
	//Envia o arquivo efetivamente
	return 0;
}

//retorna NULL se não encontrar
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
	        if (enviaStoreSucessor(key,value) == 0) cout << "\nJá existe arquivo com essa chave!\n";
					else cout << "\nArquivo disponibilizado com sucesso!\n";
	  }
  }
}
