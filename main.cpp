#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

/* TMAB 2019/2 - Estudo Dirigido 3 / Aprendizes de C/C++

Integrantes do Grupo:

Nome:                      DRE:

Alexandre Cordeiro       118037282
Bernardo Mendonca        118038733
Caue Caviglioni          118058458
Claudio Teixeira         118049815
Marcos Godinho           118058864

*/


struct folha {
	int pai;
	int total_elementos;
	int max_elementos;
	int total_filhos;
	int max_filhos;
	int* filhos;
	int* elementos;
	int minha_pos;
};

typedef struct folha Folha;

struct arvore {
	int tam;
	Folha* folha;
	FILE* arquivo;
};

typedef struct arvore Arvore;
void imprime(Arvore* arvore, Folha* f);
int ler_disco(Arvore* arvore, int seek, Folha *folha);

// Inicializa a Folha da Arvore, definindo as variaveis criadas anteriormente no struct "folha"
Folha* inicializa_folha(int tam) {
	Folha* folha = (Folha*)malloc(sizeof(Folha));

	folha->max_elementos = 2 * tam - 1;
	folha->pai = -1;
	folha->filhos = (int*)malloc(sizeof(int) * 2 * tam);
	folha->elementos = (int*)malloc(sizeof(int)*(2 * tam - 1));
	folha->total_elementos = 0;
	folha->total_filhos = 0;
	folha->minha_pos = -1;
	folha->max_filhos = 2 * tam;

	for(int i = 0; i < folha->max_elementos; i++)
		folha->elementos[i] = -1;
	for(int i = 0; i < folha->max_filhos; i++)
		folha->filhos[i] = -1;

	return folha;
}

// Inicializa a Arvore, definindo as variaveis criadas anteriormente no struct "arvore"
Arvore* inicializa(int tam) {
	Arvore* arvore = (Arvore*)malloc(sizeof(Arvore));
	arvore->tam = tam;
	arvore->folha = NULL;
	arvore->arquivo = fopen("arquivo.bin", "wb+");

	return arvore;
}

// Imprime o elemento da arvore na posicao especificada pelo ponteiro
void imprime_filho(Arvore* arvore, int ponteiro) {

	Folha* temp = (Folha*)malloc(sizeof(Folha));
	ler_disco(arvore, ponteiro, temp);
	printf("[");
	for(int i = 0; i < temp->max_elementos; i++) {
		if(temp->elementos[i] != -1)
			printf(" %d ", temp->elementos[i]);
	}
	printf("]");
	for(int i = 0; i < temp->max_filhos; i++) {
		if(temp->filhos[i] != -1) {
			imprime_filho(arvore, temp->filhos[i]);
		}
	}

	return;
}

// Imprime todos os elementos presentes tanto na arvore inicial quanto nos filhos
void imprime(Arvore* arvore, Folha* f) {
	if(f == NULL)
        return;
	printf("[");
	for(int i = 0; i < f->max_elementos; i++) {
		if(f->elementos[i] != -1)
			printf(" %d ", f->elementos[i]);
	}
	printf("]\n");
	for(int i = 0; i < f->max_filhos; i++) {
		if(f->filhos[i] != -1) {
			imprime_filho(arvore, f->filhos[i]);
		}
	}
	return;
}

// Copia sizeof(Folha) bytes de memoria da folha para um buffer temporario
int ler_disco(Arvore* arvore, int seek, Folha *folha) {
    // Insere o ponteiro na posicao dada pelo parametro "seek"
	if(fseek(arvore->arquivo, seek, SEEK_SET) == -1) return 0;

	// Cria um buffer temporario
	char *buf = (char*)malloc(sizeof(Folha));
	fread(buf, sizeof(Folha), 1, arvore->arquivo);

	// Copia o buffer para a estrutura
	memcpy(folha, buf, sizeof(Folha));

	return 1;
}

// Busca o determinado elemento na folha. Retorna 1 se encontrar, e 0 caso contrario
int busca_elemento(Folha* f, int valor) {
	for(int i = 0; i < f->max_elementos; i++) {
		if(f->elementos[i] == valor) {
			return 1;
		}
	}
	return 0;
}

// Imprime os elementos dos filhos da arvore principal
void imprime_elementos(Folha* f) {
	printf("(");
	for(int i = 0; i < f->max_elementos; i++) {
		if(f->elementos[i] != -1) {
			printf(" %d ", f->elementos[i]);
		}
	}
	printf(")\n");
}

// Busca em uma folha recursivamente e armazenando em um no na memoria o valor dado como paramentro. Do mesmo modo que a funcao "busca_elemento", retorna 1 se o valor for encontrado e 0 caso contrario
int busca(Arvore* arvore, Folha* folha, int valor) {
	if(busca_elemento(folha, valor) == 1) {
		imprime_elementos(folha);
		printf(" Elemento encontrado\n");
		return 1;
	}else {
		for (int i = 0; i < folha->max_filhos; i++) {
			if (folha->filhos[i] != -1) {
				Folha* temp = (Folha*)malloc(sizeof(Folha));
				ler_disco(arvore, folha->filhos[i], temp);
				if (busca_elemento(temp, valor) == 1)
				{
					imprime_elementos(temp);
					free(temp);
					return 1;
				}
				if (busca(arvore, temp, valor) == 1)
					return 1;
				free(temp);
			}
		}
	}
	return 0;
}

// Escreve no arquivo os elementos da Folha
int escrever_disco(Arvore* arvore, int seek, Folha *folha) {
	// Insire o ponteiro na posicao dada pelo parametro "seek"
	if(fseek(arvore->arquivo, seek, SEEK_SET) == -1) return 0;

	if(folha->minha_pos == -1)
		folha->minha_pos = ftell(arvore->arquivo);
	fwrite((void *)folha, sizeof(Folha), 1, arvore->arquivo);

	return 1;
}

// Ordena os elementos da Folha
void ordenar(Folha* f) {
	int trocas = 1;
	while(trocas > 0) {
		trocas = 0;
		int i = 0;
		while(i < f->max_elementos - 1) {
			if(f->elementos[i] > f->elementos[i + 1]) {
				int temp = f->elementos[i];
				f->elementos[i] = f->elementos[i + 1];
				f->elementos[i + 1] = temp;
				trocas++;
			}
			i++;
		}
	}
}

// Adiciona o paramentro "elem" a folha e a ordena
void adicionar_elemento(Folha* folha, int elem) {
	for(int i = 0; i < folha->max_elementos; i++) {
		if(folha->elementos[i] == -1) {
			folha->elementos[i] = elem;
			break;
		}
	}
	ordenar(folha);
	folha->total_elementos++;
}

// Adiciona uma Folha vazia a arvore principal
int adicionar_filho(Folha* pai, int seek) {
	for(int i = 0; i < pai->max_filhos; i++) {
		if(pai->filhos[i] == -1) {
			pai->filhos[i] = seek;
			break;
		}
	}
	return 1;
}

// Retorna o numero total de elementos presentes na arvore
int total_elementos(Folha* f) {
	int total = 0;
	for(int i = 0; i < f->max_elementos; i++)
	{
		if(f->elementos[i] != -1)
			total++;
	}
	return total;
}

// Parte a folha pai em 2 folhas filhas quando aquela atingir o limite de elementos definido pela variavel "tam" presente no struct
int dividir(Arvore* arvore, Folha* pai, int valor) {
	Folha* filho1 = inicializa_folha(arvore->tam);
	Folha* filho2 = inicializa_folha(arvore->tam);

	int meio = (pai->max_elementos / 2);
	int val_meio = pai->elementos[meio];

	int val_meio2 = pai->elementos[meio + 1];

	for(int i = 0; i < pai->max_elementos; i++)
		filho1->elementos[i] = pai->elementos[i];

	for(int i = 0; i < pai->max_filhos; i++)
		filho1->filhos[i] = pai->filhos[i];

	filho1->elementos[meio] = valor;
	filho1->elementos[meio + 1] = -1;
	filho1->pai = pai->minha_pos;
	filho1->total_elementos = pai->total_elementos - 1;

	filho2->pai = pai->minha_pos;
	filho2->total_elementos = 1;

	for(int i = 0; i < pai->max_elementos; i++)
		pai->elementos[i] = -1;
	for(int i = 0; i < pai->max_filhos; i++)
		pai->filhos[i] = -1;

	pai->total_elementos = 1;
	adicionar_elemento(pai, val_meio);
	adicionar_elemento(filho2, val_meio2);
	ordenar(filho1);

	fseek(arvore->arquivo, 0, SEEK_END);
	int pos = ftell(arvore->arquivo);
	escrever_disco(arvore, pos, filho1);

	int pos2 = ftell(arvore->arquivo);
	escrever_disco(arvore, pos2, filho2);

	adicionar_filho(pai, pos);
	adicionar_filho(pai, pos2);

	escrever_disco(arvore, pai->minha_pos, pai);
	return 1;
}

// Muda o valor do elemento passado como paramentro para -1
void deleta_elemento(Folha* f, int valor) {
	for(int i = 0 ;i < f->max_elementos; i++) {
		if(f->elementos[i] == valor) {
			f->elementos[i] = -1;
		}
	}
}

// Deleta o elemento que foi trocado por -1 e reescreve os valores da arvore
int deletar(Arvore* arvore, Folha* folha, int valor) {
	if(busca_elemento(folha, valor) == 1) {
		deleta_elemento(folha, valor);
		return 1;
	}else{
		for(int i = 0; i < folha->max_filhos; i++) {
			if(folha->filhos[i] != -1) {
				Folha* temp = (Folha*)malloc(sizeof(Folha));
				ler_disco(arvore, folha->filhos[i], temp);

				if(busca_elemento(temp, valor) == 1) {
					deleta_elemento(folha, valor);
					free(temp);
					return 1;
				}

				if(deletar(arvore, temp, valor) == 1)
					return 1;
				free(temp);
			}
		}
	}
	return 0;
}

// Cria uma nova geracao de filhos quando a geracao anterior alcancar o limite de elementos suportados
int explodir(Arvore* arvore, Folha* folha, int valor) {
	Folha* pai = inicializa_folha(arvore->tam);
	int meio = folha->max_elementos / 2;
	int val_meio = folha->elementos[meio];
	folha->elementos[meio] = -1;
	adicionar_elemento(folha, valor);

	ler_disco(arvore, folha->pai, pai);
	if(total_elementos(pai) < folha->max_elementos) {
		adicionar_elemento(pai, val_meio);

		ordenar(pai);
		ordenar(folha);
		int index_maior = folha->max_elementos - 1;
		int i = 0;
		Folha* irmao = inicializa_folha(arvore->tam);
		while(i < folha->max_filhos) {
			if(pai->filhos[i] == folha->minha_pos) {
				if(i + 1 != folha->max_filhos && pai->filhos[i + 1] != -1) {
					ler_disco(arvore, pai->filhos[i + 1], irmao);
					adicionar_elemento(irmao, folha->elementos[index_maior]);
					folha->elementos[index_maior] = -1;
					ordenar(irmao);
				}
			}
			i += 1;
		}

		if(irmao->minha_pos != -1)
            escrever_disco(arvore, irmao->minha_pos, irmao);
            escrever_disco(arvore, folha->minha_pos, folha);
            escrever_disco(arvore, pai->minha_pos, pai);
        }else{
            Folha* nova_raiz = inicializa_folha(arvore->tam);
            Folha* irmao = inicializa_folha(arvore->tam);

            adicionar_filho(nova_raiz, pai->minha_pos);

            fseek(arvore->arquivo, 0, SEEK_END);
            int pos = ftell(arvore->arquivo);
            escrever_disco(arvore, pos, irmao);

            adicionar_filho(nova_raiz, irmao->minha_pos);

            fseek(arvore->arquivo, 0, SEEK_END);
            pos = ftell(arvore->arquivo);
            escrever_disco(arvore, pos, nova_raiz);
            pai->pai = nova_raiz->minha_pos;
            explodir(arvore, pai, val_meio);

            arvore->folha = nova_raiz;

        }
	return 1;
}

// Adiciona uma folha caso as anteriores ja estejam totalmente preenchidas. Caso contrario adiciona o paramentro "valor" a folha ja existente
int inserir_filho(Arvore* arvore, Folha* folha, int valor) {
	Folha* temp = inicializa_folha(arvore->tam);
	if (folha->filhos[0] != -1) {
		ler_disco(arvore, folha->filhos[0], temp);
		if (total_elementos(temp) > 0 ) {
				if (total_elementos(temp) == temp->max_elementos) {

					explodir(arvore, temp, valor);

					return 1;
				}
				adicionar_elemento(temp, valor);
				ordenar(temp);
				escrever_disco(arvore, folha->filhos[0], temp);
		}else{
			adicionar_elemento(temp, valor);
			ordenar(temp);
			escrever_disco(arvore, folha->filhos[0], temp);
		}
	}

	return 1;
}

// Insere o valor dado como parametro no arquivo binario
int insere(Arvore* arvore, int valor) {
	if(arvore->folha == NULL) {
		// Primeira folha a ser inserida na arvore
		arvore->folha = inicializa_folha(arvore->tam);

		Folha* folha = arvore->folha;
		adicionar_elemento(folha, valor);
		if (escrever_disco(arvore, 0, folha)) {
			cout << "Ok, escrita!" << endl;
		}

        }else{
		Folha* f = arvore->folha;
		if(f->filhos[0] != -1) {
			// Insere o valor na folha caso haja espaco disponivel
			inserir_filho(arvore, arvore->folha, valor);
		}else{
			if (total_elementos(f) == f->max_elementos) {
				dividir(arvore, arvore->folha, valor);
			}else{
				// Nao atingiu ainda o limite
				adicionar_elemento(f, valor);
				// Escreve no arquivo
				escrever_disco(arvore, f->pai, arvore->folha);

			}
		}
	}

	return 0;
}

int main() {
    // Define o tamanho de cada folha da arvore
    int tam = 4;
    Arvore* arvore = inicializa(tam);
	char input;
	cout << "Digite o que voce gostaria de fazer: \n" << "I para inserir; \n" << "P para imprimir; \n" << "B para buscar; \n" << "D para deletar; \n" << "S para sair do programa.: ";
	cin >> input;
	int val;
	while(input != 'S') {
		switch(input) {
			case 'I':
			    cout << "Digite o valor a ser adicionado: ";
			    cin >> val;
				insere(arvore, val);
				break;
			case 'P':
				imprime(arvore, arvore->folha);
				printf("\n");
				break;
			case 'B':
			    cout << "Digite o valor que voce deseja encontrar: ";
			    cin >> val;
				cout << busca(arvore, arvore->folha, val);
				printf("\n");
				break;
			case 'D':
				deletar(arvore, arvore->folha, val);
				break;
			case 'S':
				break;
			default:
				printf("Valor nao encontrado %c\n", input);
                cout << "Digite o que voce gostaria de fazer: \n" << "I para inserir; \n" << "P para imprimir; \n" << "B para buscar; \n" << "D para deletar; \n" << "S para sair do programa.: ";
                cin >> input;
		}
	}

	return 1;
}
