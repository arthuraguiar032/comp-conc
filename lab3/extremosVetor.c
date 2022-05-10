/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 3 */
/* Codigo: Encontrar o maior e o menor valor de um vetor de números reais*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "timer.h"
#include <pthread.h>

long long int dim;	//dimensao do vetor
int nthreads; 		//numero de thrads
float *vetor;

//estrutura para armazenar os argumentos que a thread vai usar
typedef struct{
	float maior;
	float menor;
} t_Args;

//funcao que encontrar o maior e menor valor do vetor de forma sequencial
void encontraExtremosSequencial(float *vetor, float *max, float *min){
	long long int i =0;
	*min = vetor[i];
	*max = vetor[i];
	
	for(i=0; i<dim; i++){
		if(vetor[i]> *max){
			*max = vetor[i];
		}
		
		if(vetor[i]< *min){
			*min = vetor[i];
		}
	}
}

void * tarefa(void *arg){
	long long int id = (long long int) arg;
	
	t_Args *extremosLocal; //variavel local para armazenar max e o min
	extremosLocal = (t_Args*) malloc(sizeof(t_Args));
	if(extremosLocal==NULL){
		exit(1);
	}
	
	//define os limites do bloco do vetor a ser percorrido
	long long int tamBloco = dim/nthreads;
	long long int inicio = id*tamBloco;
	long long int fim;
	if(id == nthreads -1){
		fim = dim;
	}else {
		fim = inicio + tamBloco;
	}
	
	//percorre o bloco do vetor para encontrar os extremos
	extremosLocal->maior = vetor[inicio];
	extremosLocal->menor = vetor[inicio];
	for(long long int i =inicio; i<fim; i++){
		if(vetor[i]> extremosLocal->maior){
			extremosLocal->maior = vetor[i];
		}
		
		if(vetor[i]< extremosLocal->menor){
			extremosLocal->menor = vetor[i];
		}
	}
	
	//retorna o resultado dos extremos locais
	pthread_exit((void *) extremosLocal);
}

//fluxo principal
int main(int argc, char *argv[]){
	
	float maxSeq, minSeq;	//extremos sequencial
	float maxConc, minConc;//extremos Concorrente
	double inicio, fim;	//tomada de tempo
	
	t_Args *retorno;	//valor de retorno das threads
	
	
	//argumentos a serem passados p as threads
	pthread_t *tid;
	
	
	//recebe e valida os parametros de entrada
	if(argc<3){
		fprintf(stderr, "Digite: %s <dimensao do vetor> <numero threads>\n", argv[0]);
      		return 1; 
	}
	dim = atoll(argv[1]);
	nthreads = atoi(argv[2]);
	
	//aloca o vetor de entrada
	vetor = (float*) malloc(sizeof(float)*dim);
	if(vetor == NULL) {
      		fprintf(stderr, "ERRO--malloc\n");
      		return 2;
	}
	//preenche o vetor de entrada com numeros aleatorios entre 0 e 1000
	srand(time(NULL));
	for(long long int i =0; i<dim; i++){
		vetor[i] = (rand() % 10000);
	}
	
	//acha os extremos de forma sequencial
	GET_TIME(inicio);
	encontraExtremosSequencial(vetor, &maxSeq, &minSeq);
	GET_TIME(fim);
	
	//exibe resultados
	puts("-----SEQUENCIAL-----");
	printf("O maior: %f \nO menor: %f \n", maxSeq, minSeq);
	printf("Tempo sequencial:  %lf\n", fim-inicio);
	puts("");
	
	//acha os extremos de forma concorrente
	GET_TIME(inicio);
	
	//alocacao de memoria para as estruturas
	tid = (pthread_t *) malloc(sizeof(pthread_t) *nthreads);
	if(tid == NULL){
		fprintf(stderr, "ERRO--malloc\n");
      		return 2;
	}
	
	//criar as threads
	for(long int i =0; i<nthreads; i++){
		//passa para a thread i como identificador
		if(pthread_create(tid+i, NULL, tarefa, (void*) i)){
			fprintf(stderr, "ERRO--pthread_create\n");
         		return 3;
		}
	}
	
	//aguardar o termino das threads
	for(long int i=0; i<nthreads; i++){
		if(pthread_join(*(tid+i), (void **) &retorno)){
			fprintf(stderr, "ERRO--pthread_join\n");
         		return 3;
		}
		
		//extremos globais
		if(retorno->maior > maxConc){
			maxConc = retorno->maior;
		}
		
		if(retorno->menor > minConc){
			minConc = retorno->menor;
		}
	}
	
	//compara o resultado de casa thread p verificar o max e o min geral
	
	GET_TIME(fim);
	
	//exibe resultados
	puts("-----CONCORRENTE-----");
	printf("O maior: %f \nO menor: %f \n", maxConc, minConc);
	printf("Tempo Concorrente:  %lf\n", fim-inicio);
	puts("");
	
	
	//liberacao da memoria
	free(vetor);
	free(tid);
	free(retorno);
	
	return 0;
}
