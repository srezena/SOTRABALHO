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

void autenticacao(int sock_fd);

int main(int argc, char *argv[])
{
	while(1)
	{
		//configuração de usuarios
		char banco_login[10][50], banco_senha[10][50]; 
		char nome_pc[50], nome_ps[50];
		char login[50], senha[50];
		int i = 0;
		int fd_server, fd_client; //File descriptor do PIPE

		//Inserindo usuários
		strcpy(banco_login[0], "admin");
		strcpy(banco_senha[0], "admin");
		strcpy(banco_login[1], "sthefanie");
		strcpy(banco_senha[1], "paula");
		


		strcpy(nome_pc, argv[1]);
		strcpy(nome_ps, argv[2]);

		fd_server = open(nome_ps, O_WRONLY);
		fd_client = open(nome_pc, O_RDONLY);

		if(fd_server < 0) {
			printf("Erro ao abrir pipe do servidor...\n");
			exit(0);
		}
			  
		if(fd_client < 0) {
			printf("Erro ao abrir pipe do cliente...\n");
			exit(0); 
		}

		read(fd_client, login, sizeof(login)); 				  //Recebe o login vindo do cliente
		write(fd_server, "received", strlen("received") + 1); //Envia uma confiração de que recebeu
		read(fd_client, senha, sizeof(senha)); 				  //Recebe a senha do cliente

		for (i = 0; i < 10; i++)
		{
			if (strcmp(banco_login[i], login) == 0)
			{
				if (strcmp(banco_senha[i], senha) == 0)
				{
					write(fd_server, "true", strlen("true") + 1);
					printf("\nUsuário Online: %s\n", banco_login[i]);
					return 0;
				} 
			}
		}	
		//Erro de senha
		write(fd_server, "false", strlen("false") + 1);

		close(fd_server);
		close(fd_client);
	}
	return 0;
}
