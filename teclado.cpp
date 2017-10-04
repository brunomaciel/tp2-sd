#include <iostream>
#include <string>
//#include <boost/algorithm/string.hpp>

#define MAX_NOS 32
#define FIND 0
#define STORE 1
#define EXIT -1

using namespace std;
//using namespace boost;

int join();
int menu();
void find();
void store();
void aguardandoOpcao();

int K;
string valor;

//Composto de 'endereço IP' e 'porta'
typedef struct a {
	char IP[25];
	int PORT;
} HOST;

HOST sucessor;

int main(int argc , char *argv[]) {

  //Se a quantidade de parâmetros via terminal for insuficiente
  if(argc < 2) {
      printf("[CLIENTE]: Forma de uso: ./noTeclado sucessor.txt\n");
      exit (EXIT_FAILURE);
  }

	int ID = join();
	cout << "\n\nNovo Nó adicionado! ID: " << ID;

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

int menu() {
    string comando;
    //fflush(stdin);
	cout << "\nInforme o comando (EXIT para encerrar):";
	//fflush(stdin);
	cin >> comando;
	//fflush(stdin);

	//trim(comando); //removendo espaços
	string funcao = comando.substr(0, comando.find("(")); // pegando função
	for (auto & c: funcao) c = toupper(c); // Texto em Maiúsculo

	if (funcao.compare("EXIT") == 0) {
	    return EXIT;
	} else if (funcao.compare("FIND") == 0) {
	    try {
	        string parametros = comando.substr(comando.find("(")+1);
	        K =  stoi(parametros.substr(0, parametros.find(")")));
	        return FIND;
	    } catch (...) {
	        cout << "\nFormato: FIND(K)\n";
	        return 2;
	    }
	} else if (funcao.compare("STORE") == 0) {
	    try {
    	    string param1 = comando.substr(comando.find("(")+1);
    	    K =  stoi(param1.substr(0, param1.find(",")));
    	    string param2 = param1.substr(param1.find(",")+1);
    	    valor = param2.substr(0, param2.find(")"));
    	    return STORE;
    	} catch (...) {
    	    cout << "\nFormato: STORE(K,V)\n";
    	    return 2;
	    }
	}
}

int gerarChave() {
  //https://github.com/orcaman/chord/tree/master/src
  return 0;
}

int join() {
    int key = gerarChave();

    //Lê do arquivo os dados do nó sucessor

    file = fopen(argv[1], "r");
    if (file) {
        string IP; int PORT;
        while(fscanf(file, "%s %d", IP, &PORT) != EOF){
            strcpy(sucessor.IP,IP);
            sucessor.PORT = PORT;
        }
        fclose(file);
    }

    return key;
}

int chavePertenceMeuRange(int key, int ID) {
  //definido a partir do metodo de geracao de chave escolhido
  return 1;
}

void imprimeArquivo (char* path) {
  file = fopen(path, "r");
  if (file) {
      string aux;
      while(fscanf(file, "%s", aux) != EOF){
          cout << aux;
      }
      fclose(file);
  }
}

char* consultaMeusArquivos(int key) {
  // Vai ao arquivo, lê tuplas <key,path>, retorna path da key passada parametro
}

void find(int key, int ID) {

    if (chavePertenceMeuRange(key,ID)) {
      char* caminho = consultaMeusArquivos(key); // não compete com requisicao.cpp rodando localmente, pois é leitura
      if (caminho == NULL) {
        cout << "\nArquivo não encontrado!";
      } else {
        imprimeArquivo(caminho);
      }
    } else {
      if (sucessor.find(key) == 0) cout << "\nArquivo não encontrado!";
    }

}

void store(int key, string value, int ID) {

  if (chavePertenceMeuRange(key,ID)) {
    if (consultaMeusArquivos(key) == NULL) {
        //adiciona linha final arquivo com tupla <key,value> - compete com requisicao.cpp ?
    } else cout << "\nJá existe arquivo com essa chave!";
  } else {
        if (sucessor.store(key,value) == 0) cout << "\nJá existe arquivo com essa chave!";
  }

}
