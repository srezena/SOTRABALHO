#include "servidor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int criarArquivo(pasta* pastaDestino, char* nome)
{
	arquivo* arquivoTemporario = (arquivo*)malloc(sizeof(arquivo));
	arquivoTemporario->nome = (char*)calloc(strlen(nome) + 1, sizeof(char));
	strcpy(arquivoTemporario->nome, nome);
	arquivoTemporario->arquivoVizinho = NULL;

	if (pastaDestino->listaDeArquivos == NULL) //Pasta sem arquivo
	{
		pastaDestino->listaDeArquivos = arquivoTemporario; //A pasta aponta pro primeiro arquivo
	}
	else
	{
		//Verificar se não há nenhum arquivo com o mesmo nome
		arquivo* aux = pastaDestino->listaDeArquivos;
		while (aux != NULL) //Até o fim da lista
		{
			if (strcmp(aux->nome, nome) == 0)
				return 0; //Se tiver, retorna 0 e não cria
			aux = aux->arquivoVizinho;
		}
		//Senão ele cria e retorna 1
		//Arquivos inseridos em pilha
		arquivoTemporario->arquivoVizinho = pastaDestino->listaDeArquivos;
		pastaDestino->listaDeArquivos = arquivoTemporario;
	}

	return 1;
}

int criarPasta(pasta* pastaDestino, char* nome)
{
	pasta* pastaTemporaria = (pasta*)malloc(sizeof(pasta));
	pastaTemporaria->nome = (char*)calloc(strlen(nome) + 1, sizeof(char));
	strcpy(pastaTemporaria->nome, nome);
	pastaTemporaria->listaDeArquivos = NULL;
	pastaTemporaria->listaPastasVizinhas = NULL;
	pastaTemporaria->listaSubPastas = NULL;

	if (pastaDestino->listaSubPastas == NULL) //Verifica se a pasta está vazia
	{
		pastaDestino->listaSubPastas = pastaTemporaria;
	}
	else
	{
		pasta* aux = pastaDestino->listaSubPastas;
		while (aux != NULL)
		{
			if (strcmp(aux->nome, nome) == 0)
				return 0;
			aux = aux->listaPastasVizinhas;
		}
		pastaTemporaria->listaPastasVizinhas = pastaDestino->listaSubPastas;
		pastaDestino->listaSubPastas = pastaTemporaria;
	}

	return 1;
}

char* listarDadosPasta(pasta* pastaFonte)
{
	arquivo* iteradorA = pastaFonte->listaDeArquivos;
	char* string = (char*)calloc(strlen("\nArquivos:") + 1, sizeof(char));
	strcpy(string, "\nArquivos: ");

	while (iteradorA != NULL)
	{
		string = (char*)realloc(string, strlen(string) + strlen(iteradorA->nome) + 2);
		strcat(string, " ");
		strcat(string, iteradorA->nome);
		iteradorA = iteradorA->arquivoVizinho;
	}

	string = (char*)realloc(string, strlen(string) + strlen("\nPastas:") + 1);
	strcat(string, "\nPastas: ");

	pasta* iteradorP = pastaFonte->listaSubPastas;
	while (iteradorP != NULL)
	{
		string = (char*)realloc(string, strlen(string) + strlen(iteradorP->nome) + 2);
		strcat(string, " ");
		strcat(string, iteradorP->nome);
		iteradorP = iteradorP->listaPastasVizinhas;
	}
	string = (char*)realloc(string, strlen(string) + 1);
	string[strlen(string)] = '\0';
	printf("%s\n", string);
	return string;
}

pasta* navegarParaPasta(pasta* raiz, char* caminho)
{
	if (strcmp(caminho, "raiz") == 0)
		return raiz;

	//Sessão para criar uma lista de nomes
	char* nomeTemp;

	struct lista
	{
		char* nome;
		struct lista* prox;
	};

	struct lista* ultimoLista;
	//usar a variável a baixo como começo da lista
	struct lista* listaNomes = (struct lista*)malloc(sizeof(struct lista));

	ultimoLista = listaNomes;

	nomeTemp = strtok (caminho, "/");

	while(nomeTemp != NULL)
	{
		ultimoLista->nome = nomeTemp;
		nomeTemp = strtok (NULL, "/");

		if (nomeTemp != NULL)
		{
			ultimoLista->prox = (struct lista*)malloc(sizeof(struct lista));
			ultimoLista = ultimoLista->prox;
		}
	}
	ultimoLista->prox = NULL;
	//fim sessão

	pasta* aux = raiz->listaSubPastas;
	nomeTemp = listaNomes->nome;

	while (aux != NULL)
	{
		if (strcmp(aux->nome, nomeTemp) == 0)
		{
			if (listaNomes->prox == NULL)
					return aux;

			aux = aux->listaSubPastas;
			listaNomes = listaNomes->prox;
			nomeTemp = listaNomes->nome;
		}
		else
			aux = aux->listaPastasVizinhas;
	}
	return aux;
}

int removerPasta(pasta* pastaPai, char* nomePastaRemovida)
{
	pasta* anterior = pastaPai->listaSubPastas;
	pasta* atual    = pastaPai->listaSubPastas;

	while(atual != NULL)
	{
		if (strcmp(atual->nome, nomePastaRemovida) == 0)
		{
			if (atual == anterior)
			{
				removerTodosArquivos(atual->listaDeArquivos);
				pastaPai->listaSubPastas = atual->listaPastasVizinhas;
				if (atual->listaSubPastas != NULL) //Se não tiver subpasta não precisa chamar
					removerTodasPastas(atual->listaSubPastas);
			}
			else
			{
				removerTodosArquivos(atual->listaDeArquivos);
				anterior->listaPastasVizinhas = atual->listaPastasVizinhas;
				if (atual->listaSubPastas != NULL)
					removerTodasPastas(atual->listaSubPastas);
			}

			free(atual);
			return 1;
		}
		anterior = atual;
		atual = atual->listaPastasVizinhas;
	}
	return 0;
}

void removerTodosArquivos(arquivo* arquivoExcluido)
{
	if (arquivoExcluido == NULL)
		return;

	if (arquivoExcluido->arquivoVizinho == NULL)
	{
		free(arquivoExcluido);
		return;
	}

	removerTodosArquivos(arquivoExcluido->arquivoVizinho);
	free(arquivoExcluido);
}

void removerTodasPastas(pasta* pastaPai)
{
	if (pastaPai->listaSubPastas == NULL)
	{
		removerTodosArquivos(pastaPai->listaDeArquivos);
		if (pastaPai->listaPastasVizinhas != NULL)
			removerTodasPastasVizinhas(pastaPai->listaPastasVizinhas);

		free(pastaPai);
		return;
	}

	removerTodosArquivos(pastaPai->listaDeArquivos);
	removerTodasPastas(pastaPai->listaSubPastas);
	if (pastaPai->listaPastasVizinhas != NULL)
		removerTodasPastasVizinhas(pastaPai->listaPastasVizinhas);

	free(pastaPai);
	pastaPai->listaSubPastas = NULL;
}

void removerTodasPastasVizinhas(pasta* pastaPai)
{
	if (pastaPai->listaPastasVizinhas == NULL)
	{
		removerTodosArquivos(pastaPai->listaDeArquivos);
		if(pastaPai->listaSubPastas != NULL)
			removerTodasPastas(pastaPai->listaSubPastas);

		free(pastaPai);
		return;
	}

	removerTodosArquivos(pastaPai->listaDeArquivos);
	removerTodasPastasVizinhas(pastaPai->listaPastasVizinhas);
	if(pastaPai->listaSubPastas != NULL)
		removerTodasPastas(pastaPai->listaSubPastas);

	free(pastaPai);
}

int removerArquivo(pasta* pastaPai, char* nomeArquivoRemovido)
{
	arquivo* anterior = pastaPai->listaDeArquivos;
	arquivo* atual = pastaPai->listaDeArquivos;

	while(atual != NULL)
	{
		if (strcmp(atual->nome, nomeArquivoRemovido) == 0)
		{
			if (atual == anterior) //arquivo removido é o primeiro da lista
			{
				pastaPai->listaDeArquivos = atual->arquivoVizinho;
			}
			else
			{
				anterior->arquivoVizinho = atual->arquivoVizinho;
			}
			free(atual);
			return 1;
		}

		anterior = atual;
		atual = atual->arquivoVizinho;
	}

	return 0;
}

int existeArquivo(pasta* pastaPai, char* nomeArquivo)
{
	arquivo* iterador = pastaPai->listaDeArquivos;
	while(iterador != NULL)
	{
		if (strcmp(iterador->nome, nomeArquivo) == 0)
		{
			return 1;
		}
		iterador = iterador->arquivoVizinho;
	}
	return 0;
}




