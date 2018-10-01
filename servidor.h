typedef struct p
{
	char* nome;
	struct p* listaSubPastas;
	struct p* listaPastasVizinhas;
	struct a* listaDeArquivos;
} pasta;

typedef struct a
{
	char* nome;
	struct a* arquivoVizinho;
} arquivo;

/*
 * Definição dos protótipos de funções
*/

int criarArquivo(pasta* pastaDestino, char* nome);
int criarPasta(pasta* pastaDestino, char* nome);
char* listarDadosPasta(pasta* pastaFonte);
pasta* navegarParaPasta(pasta* raiz, char* caminho);
int removerPasta(pasta* pastaPai, char* nomePastaRemovida);
void removerTodosArquivos(arquivo* arquivoExcluido);
void removerTodasPastas(pasta* pastaPai);
void removerTodasPastasVizinhas(pasta* pastaPai);
int removerArquivo(pasta* pastaPai, char* nomeArquivoRemovido);
int existeArquivo(pasta* pastaPai, char* nomeArquivo);
