#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "servidor.h"

int trataMensagem(char* msg_cliente);
void autenticacao(int cliente_sock, int thread_id);
void* trata_conexao(void *cliente_sock);

sem_t s_leitura, s_escrita;
char clientes_online[10][50];
int contador_clientes_online = 0;
int pode_autenticar = 1;
pasta* RAIZ;

int main()
{

	RAIZ = (pasta*)calloc(1, sizeof(pasta));
	RAIZ->nome = (char*)calloc(4, sizeof(char));
	strcpy(RAIZ->nome, "RAIZ");
	RAIZ->listaSubPastas = NULL;
	RAIZ->listaPastasVizinhas = NULL;
	RAIZ->listaDeArquivos = NULL;

    int sock_fd, cliente_sock, sock_len;
    struct sockaddr_in sock_server, sock_cli;
    char resp_server[2000];
    pthread_t tid = 0;

    sem_init(&s_escrita, 0, 1);
	sem_init(&s_leitura, 0, 10);

    sock_fd = socket(AF_INET, SOCK_STREAM , 0);

    if (sock_fd < 0)
    {
        printf("Erro ao criar o socket!");
    }
    puts("Socket criado com sucesso!");

    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = INADDR_ANY;
    sock_server.sin_port = htons(9999);

    if(bind(sock_fd,(struct sockaddr*)&sock_server, sizeof(sock_server)) < 0)
    {
        perror("Erro ao associar nome ao socket! Erro");
        return 1;
    }

    puts("Bind feito com sucesso!");
    puts("Servidor online!");

	while(1)
	{
	    listen(sock_fd, 5);

	    sock_len = sizeof(struct sockaddr_in);

	    cliente_sock = accept(sock_fd, (struct sockaddr*)&sock_cli, (socklen_t*)&sock_len);

	    if (cliente_sock < 0)
	    {
	        perror("Erro ao aceitar conexão do cliente!");
	        return 1;
	    }

	    pthread_create(&tid, NULL, trata_conexao, (void *)&cliente_sock);
   }

    return 0;
}

int trataMensagem(char* msg_cliente)
{
	char *parte;
	int valorSemaforo = 0, i, j;
	pasta* pastaTemp = NULL;
	char comando[10], path_1[200], path_2[200], nome[50];

	//Lista dados da pasta: "ls"

		parte = (char*)strtok(msg_cliente, "|");

		//Criar diretório: comando + path + nome mkdir|raiz|nome
		if (strcmp(parte, "mkdir") == 0)
		{
			sem_getvalue(&s_leitura, &valorSemaforo);
			if (valorSemaforo < 10) //Alguem está usando
				return -1;

			strcpy(comando, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(path_1, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(nome, parte);

			sem_wait(&s_escrita);

			printf("\nComando: %s\n", comando);
			printf("Path_1: %s\n", path_1);
			printf("Nome: %s\n", nome);

			pastaTemp = navegarParaPasta(RAIZ, path_1);

			if (pastaTemp == NULL)
			{ //-4 Path inválido
				sem_post(&s_escrita);
				return -4;
			}

			if(!criarPasta(pastaTemp, nome))
			{//Pasta ja existente
				sem_post(&s_escrita);
				return -7;
			}

			sem_post(&s_escrita);
		}

		//Criar arquivo: comando + path + nome mkfile|raiz|nomedapasta|nomearquivo
		else if (strcmp(parte, "mkfile") == 0)
		{
			sem_getvalue(&s_leitura, &valorSemaforo);
			if (valorSemaforo < 10) //Alguem está usando
				return -1;

			strcpy(comando, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(path_1, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(nome, parte);

			sem_wait(&s_escrita);

			printf("\nComando: %s\n", comando);
			printf("Path_1: %s\n", path_1);
			printf("Nome: %s\n", nome);

			pastaTemp = navegarParaPasta(RAIZ, path_1);

			if (pastaTemp == NULL)
			{ //-4 Path inválido
				sem_post(&s_escrita);
				return -4;
			}

			if(!criarArquivo(pastaTemp, nome))
			{ //Arquivo ja existente
				sem_post(&s_escrita);
				return -8;
			}

			sem_post(&s_escrita);
		}
		//Remover diretório: comando + path + nome rmdiz |raiz|nomepasta
		else if (strcmp(parte, "rmdir") == 0)
		{
			sem_getvalue(&s_leitura, &valorSemaforo);
			if (valorSemaforo < 10) //Alguem está usando
				return -1;

			strcpy(comando, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(path_1, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(nome, parte);

			sem_wait(&s_escrita);

			printf("\nComando: %s\n", comando);
			printf("Path_1: %s\n", path_1);
			printf("Nome: %s\n", nome);

			pastaTemp = navegarParaPasta(RAIZ, path_1);

			if (pastaTemp == NULL) //-4 Path inválido
			{
				sem_post(&s_escrita);
				return -4;
			}

			if(!removerPasta(pastaTemp, nome)) //Pasta a ser removida nao existe
			{
				sem_post(&s_escrita);
				return -5;
			}

			sem_post(&s_escrita);
		}
		//Remover arquivo: comando + path + nome
		else if (strcmp(parte, "rmfile") == 0)
		{
			sem_getvalue(&s_leitura, &valorSemaforo);
			if (valorSemaforo < 10) //Alguem está usando
				return -1;

			strcpy(comando, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(path_1, parte);
			parte = (char*)strtok(NULL, "|");
			strcpy(nome, parte);

			sem_wait(&s_escrita);

			printf("\nComando: %s\n", comando);
			printf("Path_1: %s\n", path_1);
			printf("Nome: %s\n", nome);

			pastaTemp = navegarParaPasta(RAIZ, path_1);

			if (pastaTemp == NULL)
			{
				sem_post(&s_escrita);
				return -4; //-4 Path inválido
			}

			if(!removerArquivo(pastaTemp, nome))
			{
				sem_post(&s_escrita);
				return -6;	//Arquivo a ser removido nao existe
			}

			sem_post(&s_escrita);
		}


void autenticacao(int cliente_sock, int thread_id)
{
	char login[50], senha[50];
	char pipe_cliente[50], pipe_servidor[50];
	char msg_server[2000];
	int fd_server, fd_client;
	int autenticado = 0;
	int i;

	while(!autenticado)
	{
		recv(cliente_sock, login, 50, 0);
		recv(cliente_sock, senha, 50, 0);

		pode_autenticar = 1;

		for(i = 0; i < 10; i++)
		{
			if (strcmp(clientes_online[i], login) == 0)
			{
				write(cliente_sock, "is_online", strlen("is_online") + 1);
				pode_autenticar = 0;
				autenticado = 0;
			}
		}

		if (pode_autenticar == 1)
		{
			sprintf(pipe_cliente, "pc.%u", thread_id);
			sprintf(pipe_servidor, "ps.%u", thread_id);

			unlink(pipe_cliente);
			unlink(pipe_servidor);

			if(mkfifo(pipe_cliente, 0777) < 0) { //Parâmetros: nome; permissão;
			    printf("Erro ao criar pipe do cliente...\n");
			    exit(0);
			}

		  	if(mkfifo(pipe_servidor, 0777) < 0) {
		    	printf("Erro ao criar pipe do servidor...\n");
		    	exit(0);
		  	}

			if(fork() == 0)
			{
				execl("autenticacao", "autenticacao", pipe_cliente, pipe_servidor, NULL);
			}

			fd_server = open(pipe_servidor, O_RDONLY);
			fd_client = open(pipe_cliente, O_WRONLY);

			if(fd_server < 0) {
				printf("Erro ao abrir pipe do servidor...\n");
			    exit(0);
			}

			if(fd_client < 0) {
			    printf("Erro ao abrir pipe do cliente...\n");
			    exit(0);
			}

			write(fd_client, login, strlen(login) + 1);
			read(fd_server, msg_server, sizeof(msg_server));

			write(fd_client, senha, strlen(senha) + 1);
			read(fd_server, msg_server, sizeof(msg_server));

			if (strcmp(msg_server, "true") == 0)
			{
				strcpy(clientes_online[contador_clientes_online], login);
				contador_clientes_online++;
				autenticado = 1;
				write(cliente_sock, "true", strlen("true") + 1);
			}
			else
			{
				autenticado = 0;
				write(cliente_sock, "false", strlen("false") + 1);
			}

			close(fd_server);
			close(fd_client);
		}

	}
}

void* trata_conexao(void *cliente_sock)
{
	int resposta;
	int tam_msg;
	char* msg_cliente = (char*)calloc(2000, sizeof(char));
	int socket_recebido = *((int*)cliente_sock);

	puts("\nConexão aceita!");

	autenticacao(socket_recebido, pthread_self());

    while((tam_msg = recv(socket_recebido, msg_cliente, 2000, 0)) > 0)
    {
    	//printf("msg cliente: %s\n", msg_cliente);
	    resposta = trataMensagem(msg_cliente);

	    switch(resposta)
	    {
	    	case 0:
	    	{
	    		pasta* pastaTemp = navegarParaPasta(RAIZ, &msg_cliente[3]);
	    		if(pastaTemp == NULL)
	    		{
	    			write(socket_recebido, "Pasta nao encontrada!", strlen("Pasta nao encontrada!") + 1);
	    			break;
	    		}
	    		char* msg = listarDadosPasta(pastaTemp);
	    		write(socket_recebido, msg, strlen(msg) + 1);
	    		break;
	    	}
	    	case 1:
	    		listarDadosPasta(RAIZ);
	    		write(socket_recebido, "Sucesso!", strlen("Sucesso!") + 1);
	    		break;
	    	case -1:
	    		write(socket_recebido, "Leitura em Andamento", strlen("Leitura em Andamento") + 1);
	    		break;
	    	case -2:
	    		write(socket_recebido, "Escrita em Andamento", strlen("Escrita em Andamento") + 1);
	    		break;
	    	case -3:
	    		write(socket_recebido, "Comando Invalido!", strlen("Comando Invalido!") + 1);
	    		break;
	    	case -4:
	    		write(socket_recebido, "Path Invalido!", strlen("Path Invalido!") + 1);
	    		break;
	    	case -5:
	    		write(socket_recebido, "Pasta nao encontrada!", strlen("Pasta nao encontrada!") + 1);
	    		break;
	    	case -6:
	    		write(socket_recebido, "Arquivo nao encontrado!", strlen("Arquivo nao encontrado!") + 1);
	    		break;
	    	case -7:
	    		write(socket_recebido, "Pasta ja Existente!", strlen("Pasta ja Existente!") + 1);
	    		break;
	    	case -8:
	    		write(socket_recebido, "Arquivo ja Existente!", strlen("Pasta ja Existente!") + 1);
	    		break;
	    }

	   	free(msg_cliente);
	    msg_cliente = (char*)calloc(2000, sizeof(char));
    }

    if(tam_msg == 0)
    {
        puts("\nCliente Desconectado!");
        fflush(stdout);
    }
    else if(tam_msg == -1)
    {
        perror("Falha ao receber comando do cliente!");
    }
}
