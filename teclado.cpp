#include <iostream>
#include <string>

#define MAX_NOS 32
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
void imprimeArquivo (char* path);
char* consultaMeusArquivos(int key);
void store(int key, string value, int ID);
string maiusculo(string str);
string retiraEspaco(string str);

int K;
string valor;

//Composto de 'endereço IP' e 'porta'
typedef struct a {
	char IP[25];
	int PORT;
} HOST;

HOST sucessor;

/*****
sucessor.txt

N K
IP1 PORT1
IP2 PORT2
...
IPN PORTN
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

int gerarChave() {
  //https://github.com/orcaman/chord/tree/master/src
  return 0;
}

int join(char* nomeArquivo) {
    int key = gerarChave();
    //Lê do arquivo os dados do nó sucessor
    FILE *file;
    file = fopen(nomeArquivo, "r");
    if (file) {
        char IP[25]; int PORT;
        while(fscanf(file,"%s %d", IP, &PORT) != EOF){
            strcpy(sucessor.IP,IP);
            sucessor.PORT = PORT;
        }
        fclose(file);
    } else {
			cout << "\n Não foi possível ler o arquivo: "<< nomeArquivo << "!\n\n";
			exit(1);
		}

    return key;
}

int chavePertenceMeuRange(int key, int ID) {
  //definido a partir do metodo de geracao de chave escolhido
  return 1;
}

void imprimeArquivo (char* path) {
	FILE *file;
  file = fopen(path, "r");
  if (file) {
      char* aux;
      while(fscanf(file, "%s", aux) != EOF){
          cout << aux;
      }
      fclose(file);
  } else {
		cout << "\nErro ao abrir arquivo: "<<path;
	}
}

char* consultaMeusArquivos(int key) {
  // Vai ao arquivo, lê tuplas <key,path>, retorna path da key passada parametro
	return NULL;
}

void find(int key, int ID) {

    /*if (chavePertenceMeuRange(key,ID)) {
      char* caminho = consultaMeusArquivos(key); // não compete com requisicao.cpp rodando localmente, pois é leitura
      if (caminho == NULL) {
        cout << "\nArquivo não encontrado!";
      } else {
        imprimeArquivo(caminho);
      }
    } else {
      if (sucessor.find(key) == 0) cout << "\nArquivo não encontrado!";
    }*/

}

void store(int key, string value, int ID) {

  /*if (chavePertenceMeuRange(key,ID)) {
    if (consultaMeusArquivos(key) == NULL) {
        //adiciona linha final arquivo com tupla <key,value> - compete com requisicao.cpp ?
    } else cout << "\nJá existe arquivo com essa chave!";
  } else {
        if (sucessor.store(key,value) == 0) cout << "\nJá existe arquivo com essa chave!";
  }*/

}
