/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: Elevar ao quadrado cada elemento de um vetor de 10000 elementos. */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <math.h>

#define N_THREADS  2			//total de threads a serem criadas
#define N_ELEMENTOS 10000		//tamanho do vetor a ser criado

int vetor[N_ELEMENTOS];
int vetorTeste[N_ELEMENTOS];

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct{
	int inicio, fim;
} t_Args;

//funcao a ser executada pelas threads
void *elevaAoQuadrado(void *arg){
	t_Args *args = (t_Args *) arg;
	
	for(int i = args->inicio; i<=args->fim; i++){
		vetor[i] = pow(vetor[i], 2);
	}
	
	pthread_exit(NULL);
}

//funcao que testa a veracidade dos valores finais do vetor
void testaVetor(){
	for(int i =0; i<N_ELEMENTOS; i++){
		if(vetor[i]!=pow(vetorTeste[i], 2)){
			printf("Valores errados\n");
			exit(-1);
		}
	}
	printf("Valores corretos!\n");
}


int main(){
	//inicializa os dois vetores com numeros de 0 a 9
	for(int i =0; i<N_ELEMENTOS; i++){
		vetor[i] = i % 10;
		vetorTeste[i] = i % 10;
	}
	
	//identificadores das threads no sistema
	pthread_t tid_sistema[N_THREADS];
	t_Args *arg;
	int thread;
	
	for(thread = 0; thread<N_THREADS; thread++){
		//aloca e preenche os argumentos para serem executados pela thread
		arg = malloc(sizeof(t_Args));
		if(arg==NULL){
			printf("--ERRO: malloc()\n");
			exit(-1);
		}
		
		if(thread==0){
			arg->inicio = 0;
			arg->fim = 4999;
		} else{
			arg->inicio = 5000;
			arg->fim = 9999;
		}
		
		//cria a thread
		if(pthread_create(&tid_sistema[thread], NULL, elevaAoQuadrado, (void*) arg)){
			printf("--ERRO: CREATE()\n");
			exit(-1);
		}
	}
	
	//aguarda todas as threads terminarem
	for(thread = 0; thread<N_THREADS; thread++){
		if (pthread_join(tid_sistema[thread], NULL)) {
         		printf("--ERRO: pthread_join() \n");
         		exit(-1); 
    		} 
	}
	
	//testa os valores calculados
	testaVetor();
	
	return 0;
}
