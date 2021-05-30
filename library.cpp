#include "library.h"
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
using namespace std;

struct Trabalho {
    int trabalhoID;
    void* (*funcao)(void*);
    int* pEntrada; // Parâmetro de entrada para f
    int* resultado; // Retorno de f
};

//Struct Trabalho *ListaTrabalhosProntos, *ListaTrabalhosTerminados; // PRONTO significa: pronto para executar
std::vector<struct Trabalho> ListaTrabalhosProntos, ListaTrabalhosTerminados; // Estrutura de das listas de trabalhos


pthread_t *pvs; //Lista de PVs:
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Varivel de controle para o Mutex
pthread_mutexattr_t attr;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // Condicional para o consumidor(PV) esperar


int idTrabalhoAtual = 0;
int m;
bool fim;

// Retira um trabalho da lista de trabalhos prontos para serem feitos
struct Trabalho* pegaUmTrabalho() {
    struct Trabalho* t = new Trabalho();

    if(ListaTrabalhosProntos.empty()) {
        pthread_cond_wait(&cond,&mutex);
    }
    else{
        t = &ListaTrabalhosProntos[ListaTrabalhosProntos.size()-1]; // Pega o primeiro da "fila" por padrão ARRUMAR
        ListaTrabalhosProntos.erase(ListaTrabalhosProntos.end()-1);
    }

    return t;
}

//Armazena na lista de trabalhos finalizados
void armazenaResultados(struct Trabalho *t, int * resultadoT){
    pthread_mutex_lock(&mutex); // Inicio secao critica
    t->resultado = resultadoT;
    ListaTrabalhosTerminados.push_back(*t);
    pthread_mutex_unlock(&mutex); // Fim secao critica
}

//Processadores Virtuais:
void* MeuPV(void* dta) {
    int* res;
    struct Trabalho* t;

    if(ListaTrabalhosProntos.empty()) pthread_cond_wait(&cond, &mutex);

    while(fim==false && !ListaTrabalhosProntos.empty()) {
        pthread_mutex_lock(&mutex);
        t = pegaUmTrabalho(); //--->> Aqui o PV tem comportamento de consumidor
        res = (int*)t->funcao( (void*)t->pEntrada );
        armazenaResultados(t, res); //--->> Coloca na Lista de Terminados
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}

// Implementação das PRIMITIVAS
int start(int m){
    int ret = 0;
    pvs = (pthread_t*) malloc(m*sizeof(pthread_t));

    fim = false;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
    for( int i = 0 ; i < m; i++ ) {
        ret |= pthread_create(&(pvs[i]), NULL, MeuPV, NULL);
    }
    return ret;
}

int finish() {
    int ret = 0;
    fim = true;
    for( int i = 0 ; i < m; i++ )
        ret = pthread_join(pvs[i], NULL);
    return ret;
}

int spawn(struct Atrib *atrib, void *(*t)(void *), void *dta) {

    pthread_mutex_lock(&mutex); // Inicio secao critica
    struct Trabalho* umTrabalho = new struct Trabalho();
    umTrabalho->trabalhoID = idTrabalhoAtual;
    idTrabalhoAtual++;
    umTrabalho->funcao = t;
    umTrabalho->pEntrada = (int*)dta;
    ListaTrabalhosProntos.push_back(*umTrabalho);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex); // Fim secao critica

    return idTrabalhoAtual-1;
}

int sync(int tId, void** res){
    int it1, it2;
    struct Trabalho* t = new struct Trabalho();
    pthread_mutex_lock(&mutex); // Inicio secao critica

    int flag = 0;
    while(flag == 0) {
        for (it1 = 0; it1 <= ListaTrabalhosProntos.size(); it1++) {

            if(ListaTrabalhosProntos[it1].trabalhoID == tId) {
                if(!ListaTrabalhosProntos.empty()) {
                    t = &ListaTrabalhosProntos[it1]; // Pega o trabalho it, selecionado pelo tId
                    *res = (int *)t->funcao(t->pEntrada);
                    ListaTrabalhosProntos.erase(ListaTrabalhosProntos.begin()+it1);

                    flag = 1;

                }
            }
        }

        for (it2 =0; it2 < ListaTrabalhosTerminados.size(); it2++) {
            if (ListaTrabalhosTerminados[it2].trabalhoID == tId) {
                t = &ListaTrabalhosTerminados[it2]; // Pega o trabalho it, selecionado pelo tId
                *res = (int *) t->resultado;
                ListaTrabalhosTerminados.erase(ListaTrabalhosTerminados.begin()+it2);
                flag = 1;
            }
        }

        if(ListaTrabalhosProntos.empty() and ListaTrabalhosTerminados.empty()) flag = 1;
        if(flag == 0)std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    pthread_mutex_unlock(&mutex); // Fim secao critica
    return 1;
}