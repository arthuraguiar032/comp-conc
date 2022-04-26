/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 2 */
/* Codigo: Multiplicação de matrizes quadradas */

#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include <time.h>
#include <pthread.h>

int *matriz1; //matriz 1 de entrada
int *matriz2; // matriz 2 de entrada
int *saidaConcorrente;  //mresultado da matriz multiplicada de forma concorrente
int nthreads; //numero de threads

typedef struct{
	int id;
	int dim;
}tArgs;

//funcao que as threads vao executar. solucao em que cada thread faz uma linha da matriz de forma intercalada
void * tarefa(void *arg){
	tArgs *args = (tArgs*) arg;
	
	for(int i=args->id; i<args->dim; i+=nthreads){
		for(int j = 0; j<args->dim; j++){
			int somaproduto =0;
			for(int k=0; k<args->dim; k++){
				somaproduto += matriz1[i*(args->dim) + k]*matriz2[k*(args->dim) + j];
			}
			saidaConcorrente[i*(args->dim) + j] = somaproduto;
		}
	}
	pthread_exit(NULL);
}

//multplicação de matrizes, com um unico fluxo de execucao, passado onde vai ser escrito o resultado e o tamanho da matriz
void multiplicaMatrizSequencial(int *resultado, int dim){ 
	for(int i =0; i<dim; i++){
		for(int j=0; j<dim; j++){
			int somaproduto = 0;
			for(int k=0; k<dim; k++){
				somaproduto += matriz1[i*dim + k]*matriz2[k*dim + j];
			}
			resultado[i*dim + j] = somaproduto;
		}
	}
}

//funcao que verifica corretude da solucao
int verificaCorretude(int *sequencial, int dim){
	for(int i =0; i<dim; i++){
		for(int j =0; j<dim; j++){
			if(saidaConcorrente[i*dim + j]!=sequencial[i*dim + j]){
				return 4;
			}
		}
	}
	return 0;
}

//fluxo principal
int main(int argc, char* argv[]){
	int dim; //dimensao das matrizes da entrada
	pthread_t *tid; //identificadores das threads no sistema
	tArgs *args; //identificadores locais das thread e dimensao da matriz
	
	double inicio, fim, difSeq, difConc; //marcadores de tempo sequencial
	
	//leitura dos parametros de entrada
	if(argc<3){
		printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
		return 1;
	}
	dim = atoi(argv[1]);
	nthreads = atoi(argv[2]);
	if(nthreads>dim){
		nthreads = dim;
	}
	
	//alocacao de memoria para as matrizes
	matriz1 = (int *) malloc(sizeof(int) * dim * dim);
	if(matriz1 == NULL){
		printf("ERRO --- MALLOC\n");
		return 2;
	}
	matriz2 = (int *) malloc(sizeof(int) * dim * dim);
	if(matriz2 == NULL){
		printf("ERRO --- MALLOC\n");
		return 2;
	}
	saidaConcorrente = (int *) malloc(sizeof(int) * dim * dim);
	if(saidaConcorrente == NULL){
		printf("ERRO --- MALLOC\n");
		return 2;
	}
	
	int *saidaSequencial; //resultado da multiplicação de matrizes sequencial
	saidaSequencial = (int *) malloc(sizeof(int) * dim * dim);
	if(saidaSequencial == NULL){
		printf("ERRO --- MALLOC\n");
		return 2;
	}
	
	//inicializacao das matrizes de entrada e saida
	srand(time(NULL)); //passada a semente para gerar numeros aleatorios
	for(int i =0; i<dim; i++){
		for(int j = 0; j<dim; j++){
			saidaConcorrente[i*dim +j] = 0;
			saidaSequencial[i*dim +j] = 0;
			
			//gerando numeros aleatorios na faixa de 0 a 10
			matriz1[i*dim +j] = (rand() % 10);
			matriz2[i*dim +j] = (rand() % 10);
		}
	}
	
	//multiplicacao sequencial
	GET_TIME(inicio);
	multiplicaMatrizSequencial(saidaSequencial, dim);
	GET_TIME(fim);
	difSeq = fim - inicio;
	
	//multiplicacao concorrente
	
	//alocacao das estruturas
	GET_TIME(inicio);
	tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
	if(tid==NULL){
		printf("ERRO --- MALLOC\n");
		return 2;
	}
	args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
	if(args==NULL){
		printf("ERRO --- MALLOC\n");
		return 2;
	}
	//criacao das threads
	for(int i =0; i<nthreads; i++){
		(args+i)->id = i;
		(args+i)->dim = dim;
		
		if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
			puts("ERRO--pthread_create");
			return 3;
		}
	}
	
	//espera pelo termino das threads
	for(int i =0; i<nthreads; i++){
		pthread_join(*(tid+i), NULL);
	}
	GET_TIME(fim);
	difConc = fim - inicio;
	
	
	//verifica corretude da solucao concorrente
	if(verificaCorretude(saidaSequencial, dim)!=0){
		printf("ERRO--corretude\n");
		return 4;
	}
	//printf("OK!");
	
	//comparacao do tempo de execucao entre concorrente e sequencial
	printf("Multiplicação de matrizes de dimensao %d com %d thread(s):\n", dim, nthreads);
	printf("Tempo Sequencial: %lf \n", difSeq);
	printf("Tempo Concorrente: %lf \n", difConc);
	printf("Desempenho: %lf \n\n", difSeq/(difConc*1.0));
	
	//liberacao de memoria
	free(matriz1);
	free(matriz2);
	free(saidaConcorrente);
	free(saidaSequencial);
	free(args);
	free(tid);
	

	return 0;
}
