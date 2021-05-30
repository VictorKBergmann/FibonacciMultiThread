#include "library.h"
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <iostream>
#include <vector>
using namespace std;

struct Trabalho {
    int trabalhoID;
    void* (*funcao)(void*);
    void* pEntrada; // Parâmetro de entrada para f
    void* resultado; // Retorno de f
};

//Struct Trabalho *ListaTrabalhosProntos, *ListaTrabalhosTerminados; // PRONTO significa: pronto para executar
std::vector<struct Trabalho> ListaTrabalhosProntos, ListaTrabalhosTerminados; // Estrutura de das listas de trabalhos


static pthread_t *pvs; //Lista de PVs:
pthread_mutex_t mutex; // Varivel de controle para o Mutex
pthread_cond_t cond; // Condicional para o consumidor(PV) esperar


int idTrabalhoAtual = 0;
int fim;

// Retira um trabalho da lista de trabalhos prontos para serem feitos
struct Trabalho* pegaUmTrabalho() {
    struct Trabalho* t = new Trabalho();
    pthread_mutex_lock(&mutex); // Inicio secao critica
    if(ListaTrabalhosProntos.empty()) {
        pthread_cond_wait(&cond,&mutex);
    }
    else if(!ListaTrabalhosProntos.empty()) {
        t = &ListaTrabalhosProntos[ListaTrabalhosProntos.size()-1]; // Pega o primeiro da "fila" por padrão ARRUMAR
        ListaTrabalhosProntos.erase(ListaTrabalhosProntos.end());
    }
    pthread_mutex_unlock(&mutex); // Fim secao critica
    return t;
}

//Armazena na lista de trabalhos finalizados
void armazenaResultados(struct Trabalho *t, void * resultadoT){
    pthread_mutex_lock(&mutex); // Inicio secao critica
    t->resultado = resultadoT;
    ListaTrabalhosTerminados.push_back(*t); // era t
    pthread_mutex_unlock(&mutex); // Fim secao critica
}

//Processadores Virtuais:
void* MeuPV(void* dta) {
    void* res;
    struct Trabalho* t;
    while(fim==false) {
        if(ListaTrabalhosProntos.empty()) {
            cout << "entrou cond wait" << endl;
            pthread_cond_wait(&cond, &mutex);
        }
        else {
            cout << "entrou else" << endl;
            t = pegaUmTrabalho(); //--->> Aqui o PV tem comportamento de consumidor
            cout << "antes fibo" << endl;
            res = t->funcao( t->pEntrada );  // --->> vai para AAA
            cout << "depois fibo" << endl;
            armazenaResultados(t, res); //--->> Coloca na Lista de Terminados
        }
    }
    return 0;
}

// Implementação das PRIMITIVAS

int start(int m){
    int ret = 0;
    pvs = (pthread_t*) malloc(m*sizeof(pthread_t));
    fim = false;
    cout << "antes for start"  << endl;
    pthread_mutex_init(&mutex, NULL);
    for( int i = 0 ; i < m; i++ )
        ret |= pthread_create(&(pvs[i]),NULL,MeuPV,NULL);
    cout << "alguma thread criada? " << ret << endl;
    cout << "depois for start" << endl;
    return ret;

}

int finish(int m) {
    fim = true;
    int ret = 0;
    for( int i = 0 ; i < m; i++ )
        ret |= pthread_join(pvs[i],NULL);
    return ret;

}

int spawn(struct Atrib *atrib, void *(*t)(void *), void *dta) {
     pthread_mutex_lock(&mutex); // Inicio secao critica
    struct Trabalho* umTrabalho = (struct Trabalho*) malloc(sizeof(struct Trabalho));
    umTrabalho->trabalhoID = idTrabalhoAtual;
    idTrabalhoAtual++;
    umTrabalho->funcao = t;
    umTrabalho->pEntrada = dta;
    cout << "antes push_back trabPronto" << endl;
    ListaTrabalhosProntos.push_back(*umTrabalho);
    cout << "depois push_back trabPronto" << endl;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex); // Fim secao critica

    return idTrabalhoAtual-1;
}

int sync(int tId, void* res){
    int* resposta;
    int it1, it2;
    struct Trabalho* t = new Trabalho();
    //std::vector<struct Trabalho>::iterator it1, it2; // iterador da lista
    pthread_mutex_lock(&mutex); // Inicio secao critica
    if(ListaTrabalhosProntos.empty()) {
        cout << "Lista vazia";
        return -1;
    }
    for (it1 = 0; it1 < ListaTrabalhosProntos.size(); it1++) {
    //for (std::list<struct T>::iterator; it1 != ListaTrabalhosProntos.end(); it1++) {
        if(ListaTrabalhosProntos[it1].trabalhoID == tId) {
            if(!ListaTrabalhosProntos.empty()) {
                t = &ListaTrabalhosTerminados[it1]; // Pega o trabalho it, selecionado pelo tId ARRUMAR
                ListaTrabalhosProntos.erase(ListaTrabalhosProntos.begin()+it1);
                resposta = (int *)t->funcao(t->pEntrada);
            }

        }
    }
    if(ListaTrabalhosTerminados.empty()) {
        cout << "Lista vazia" << endl;
        return -2;
    }
    for (it2 =0; it2 < ListaTrabalhosTerminados.size(); it2++) {
        if (ListaTrabalhosTerminados[it2].trabalhoID == tId) {
            if(!ListaTrabalhosTerminados.empty()) {
                t = &ListaTrabalhosTerminados[it2]; // Pega o trabalho it, selecionado pelo tId ARRUMAR
                resposta = (int *) t->resultado;
                ListaTrabalhosTerminados.erase(ListaTrabalhosTerminados.begin()+it2);
            }
        }
    }
    pthread_mutex_unlock(&mutex); // Fim secao critica
    return *resposta;
}