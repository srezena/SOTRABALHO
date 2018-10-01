CC = gcc

all: cliente servidor autenticacao

cliente: cliente.c
	$(CC) cliente.c -o cliente
	
servidor: servidor.c server.c
	$(CC) servidor.c server.c -o servidor -lpthread 
	
autenticacao: autenticacao.c
	$(CC) autenticacao.c -o autenticacao
	
clean:
	rm cliente servidor autenticacao
