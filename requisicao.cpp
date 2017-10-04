#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

int port;

void *connection_handler(void *);

void readFile(char *name, char *res);

int main(int argc , char *argv[]) {

    //Se não houver entrada de argumento via terminal
    if(argc < 2) {
        printf("[SERVIDOR]: Forma de uso: ./server PORT\n");
        exit (EXIT_FAILURE);
    }

    int socket_desc; //Socket utilizado pelo servidor para escuta
    int client_sock; //Socket recebido do cliente para comunicação
    int c = sizeof(struct sockaddr_in); //Armazenará tamanho da estrutura 'sockaddr_in'
    struct sockaddr_in server;
    struct sockaddr_in client;
    pthread_t thread_id;
    port = atoi(argv[1]); //Porta de escuta do servidor recebido via terminal

    //Criação de socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        printf("[SERVIDOR %i]: Socket não pode ser criado\n", port);
    }
    printf("[SERVIDOR %i]: Socket criado\n", port);

    //Prepara a estrutura 'sockaddr_in'
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("[SERVIDOR]: Erro ao aplicar 'bind'");
        return 1;
    }
    printf("[SERVIDOR %i]: Bind concluído\n", port);

    //Início do processo de escuta
    listen(socket_desc , 3);
    printf("[SERVIDOR %i]: Aguardando por requisições de conexão...\n", port);

    //Chegada e aceitação de conexão
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        printf("[SERVIDOR %i]: Conexão aceita\n", port);

        //Criação de thread para atender requisição
        if( pthread_create( &thread_id, NULL,  connection_handler, (void*) &client_sock) < 0) {
            perror("[SERVIDOR]: Não foi possível criar a thread");
            return 1;
        }

        pthread_join(thread_id , NULL);
        printf("[SERVIDOR %i]: Thread designada\n", port);
    }

    //Erro ao obter socket do cliente
    if (client_sock < 0) {
        perror("[SERVIDOR]: Aceitação de conexão falhou");
        return 1;
    }

    return 0;
}

void *connection_handler(void *socket_desc) {

    int sock = *(int*)socket_desc; //Socket do cliente
    int read_size; //Variável para definir recebeu a mensagem
    int sys; //Armazenará estado da consulta do comando do cliente (com ou sem sucesso)
    char serverReply[10000]; //Resultado da busca do cliente obtido por este servidor
    char client_cmd[5000]; //Comando oriundo do cliente
    char redirect[3000];
    char redirectOut[100];
    char redirectErr[100];
    char stOut[100];
    char stErr[100];
    char rmOut[100]="rm ";
    char rmErr[100]="rm ";
    char str[100]; //Armazenará o nome do arquivo de saída

    //Define-se o nome do arquivo de saída
    sprintf(str,"%p",socket_desc);
    strcat(str, ".txt");
   // cout << "[SERVIDOR " << port << "]: " << str << endl;
    string aux(str);

    //Prefixos para os arquivos
    string stdOut = "stdOut-", stdErr="stdErr-",
    //prefixo par redirecionamento
    redStdOut = " 1> ", redStdErr = " 2> ";
    stdErr+=aux;//nome do arquivo com resultado da saída de erro padrão.
    stdOut+=aux;//nome do arquivo com resultado da saída padrão.
    redStdOut += stdOut;
    redStdErr += stdErr;


    strcat(redirectOut, redStdOut.c_str());
    strcat(redirectErr, redStdErr.c_str());
    strcat(redirect, redirectOut);
    strcat(redirect, redirectErr);
    strcat(stOut, stdOut.c_str());
    strcat(stErr, stdErr.c_str());
    strcat(rmOut, stdOut.c_str());//comando para deletar arquivos criados
    strcat(rmErr, stdErr.c_str());//comando para deletar arquivos criados


    //Recebe mensagem do cliente
    while( (read_size = recv(sock , client_cmd , 2000 , 0)) > 0 ) {

        //Limita-se o vetor
        client_cmd[read_size] = '\0';

        cout << "[SERVIDOR " << port << "]: Comando a ser executado: " << client_cmd << endl;


        /*

          PARTE A ALTERAR, EXECUTAR O FIND OU O STORE E ESCREVE A RESPOSTA DE ACORDO

        //Concatena o comando do cliente com a variável 'redirect'
        strcat(client_cmd, redirect);


        //Executa-se o comando do cliente
        sys = system(client_cmd);


        STORE VAI RESPONDER ASSIM MESMO, ENVIANDO UM ARQUIVO !!

        //Define-se qual arquivo será lido
        (!sys) ? readFile(stOut, serverReply) : readFile(stErr, serverReply);

        if(strlen(serverReply) == 0)
            strcat(serverReply, "Não há resultados relacionados!");
        //Responde o cliente
        write(sock , serverReply , strlen(serverReply));


        */

        //Deleta-se os arquivos
        system(rmOut);
        system(rmErr);

        //Limpa-se os buffers
        memset(client_cmd, 0, 5000);
        memset(serverReply, 0, 10000);


    }

    if(read_size == 0) {
        printf("[SERVIDOR %i]: Cliente desconectou\n", port);
        fflush(stdout);
    }
    else if(read_size == -1) {
        perror("[SERVIDOR]: Falha ao executar 'recv'");
    }

    //Encerra o socket
    close(sock);

    return 0;
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
