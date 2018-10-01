#include <stdio.h> 
#include <string.h>    
#include <stdlib.h>
#include <sys/socket.h>    
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
void autenticacao(int sock_fd);

//Var globais 
char login[50], senha[50];

int main()
{
	    int sock_fd;
	    struct sockaddr_in sock_cli;
	    char comando[1000] , msg_server[2000];
	  
	    sock_fd = socket(AF_INET, SOCK_STREAM , 0);
	    if (sock_fd == -1)
	    {
	        printf("Erro ao criar o socket!");
	    }
	    puts("Socket criado!");
	     
	    sock_cli.sin_addr.s_addr = inet_addr("127.0.0.1");
	    sock_cli.sin_family = AF_INET;
	    sock_cli.sin_port = htons(9999);
	 
	    if (connect(sock_fd, (struct sockaddr*)&sock_cli , sizeof(sock_cli)) < 0)
	    {
	        perror("Falha na conexão! Erro: ");
	        return 1;
	    }
	     
	  	autenticacao(sock_fd);

		    while(1)
		    {
		        printf("\nDigite seu comando > ");
		        scanf("%s" , comando);
		         
		        if(strcmp(comando, "sair") == 0) //Para o cliente sair
		        {    
		        	strcat(comando, "|");
		        	strcat(comando, login);
		        	send(sock_fd, comando, strlen(comando) + 1, 0);
		            close(sock_fd);              //Finaliza o socket
		            printf("Cliente saiu do sistema!\n");
		            exit(0);
		        }
		        else 
		        {
		            if (send(sock_fd, comando, strlen(comando) + 1, 0) < 0)
		            {
		                puts("Falha ao enviar comando!");
		                return 1;
		            }

		            if(recv(sock_fd, msg_server, 2000, 0) < 0)
		            {
		                puts("Falha ao receber comando!");
		                break;
		            }
		        }
		         
		        printf("Servidor: ");
		        puts(msg_server);
		    }
		     
		    close(sock_fd);

    return 0;
}

void autenticacao(int sock_fd)
{
	int autenticado = 0;
	char msg_server[2000];
	int fd_server, fd_client; //File descriptor do PIPE

	puts("AUTENTICAÇÃO");

	while(!autenticado)
	{
		printf("\nDigite seu login: ");
		scanf("%s" , login);
		send(sock_fd, login, strlen(login) + 1, 0); 

		printf("Digite sua senha: ");
		scanf("%s" , senha);
		send(sock_fd, senha, strlen(senha) + 1, 0);

		recv(sock_fd, msg_server, 2000, 0);
		
		if (strcmp(msg_server, "true") == 0)
		{
			autenticado = 1; 
			printf("Autenticação feita com sucesso!\n");
		}
		else if (strcmp(msg_server, "is_online") == 0)
		{
			autenticado = 0; 
			printf("O usuário %s já está online!\n", login);
		}
		else
		{
			autenticado = 0;
			printf("Login e/ou senha incorretos. Tente novamente...\n");
		}
	}
}
