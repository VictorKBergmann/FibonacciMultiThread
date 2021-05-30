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
    void* pEntrada; // Parâmetro de entrada para f
    void* resultado; // Retorno de f
};

//Struct Trabalho *ListaTrabalhosProntos, *ListaTrabalhosTerminados; // PRONTO significa: pronto para executar
std::vector<struct Trabalho> ListaTrabalhosProntos, ListaTrabalhosTerminados; // Estrutura de das listas de trabalhos


static pthread_t *pvs; //Lista de PVs:
pthread_mutex_t mutex; // Varivel de controle para o Mutex
pthread_mutexattr_t attr;
pthread_cond_t cond; // Condicional para o consumidor(PV) esperar


int idTrabalhoAtual = 0;
int fim;

// Retira um trabalho da lista de trabalhos prontos para serem feitos
struct Trabalho* pegaUmTrabalho() {
    struct Trabalho* t = new Trabalho();
    pthread_mutex_lock(&mutex); // Inicio secao critica
    if(ListaTrabalhosProntos.empty()) {
        pthread_cond_wait(&cond,&mutex);
        cout << "dentro wait pegaUmTrabalho" << endl;
    }
    else{
        t = &ListaTrabalhosProntos[ListaTrabalhosProntos.size()-1]; // Pega o primeiro da "fila" por padrão ARRUMAR
        cout << "pegou umTrabalho" << endl;
        ListaTrabalhosProntos.erase(ListaTrabalhosProntos.end()-1); //potencial seg fault
        cout << "apagou o Trabalho" << endl;
    }
    pthread_mutex_unlock(&mutex); // Fim secao critica
    cout << "unlock mutex pegaUmTrabalho" << endl;
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
            pthread_cond_wait(&cond, &mutex);
        }
        else {
            cout << "entrou else" << endl;
            pthread_mutex_lock(&mutex);
            t = pegaUmTrabalho(); //--->> Aqui o PV tem comportamento de consumidor -- dando loop infinito
            cout << "antes fibo" << t->trabalhoID<< endl;
            res = t->funcao( t->pEntrada );
            cout << "depois fibo" <<t->trabalhoID<< endl;
            armazenaResultados(t, res); //--->> Coloca na Lista de Terminados
            pthread_mutex_unlock(&mutex);
            cout << "armazenou resultado" << endl;
        }
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
    for( int i = 0 ; i < m; i++ )
        ret |= pthread_create(&(pvs[i]),NULL,MeuPV,NULL);
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
    pthread_mutex_lock(&mutex); // Inicio secao critica -- loop infinito
    struct Trabalho* umTrabalho = (struct Trabalho*) malloc(sizeof(struct Trabalho));
    umTrabalho->trabalhoID = idTrabalhoAtual;
    idTrabalhoAtual++;
    umTrabalho->funcao = t;
    umTrabalho->pEntrada = (int*)dta;
    cout << "antes push_back trabPronto" << endl;
    ListaTrabalhosProntos.push_back(*umTrabalho);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex); // Fim secao critica
    cout << "unlock mutex spawn" << endl;
    return idTrabalhoAtual-1;
}

int sync(int tId, void** res){
    cout << "entrou sync" << endl;
    int it1, it2;
    struct Trabalho* t = new struct Trabalho();
    //std::vector<struct Trabalho>::iterator it1, it2; // iterador da lista
    pthread_mutex_lock(&mutex); // Inicio secao critica
    /*
    if(ListaTrabalhosProntos.empty()) {
        cout << "Lista vazia";
        return -1;
    }
     */
    int flag = 0;
    while(flag == 0) {
    for (it1 = 0; it1 < ListaTrabalhosProntos.size(); it1++) {
        //for (std::list<struct T>::iterator; it1 != ListaTrabalhosProntos.end(); it1++) {
        if(ListaTrabalhosProntos[it1].trabalhoID == tId) {
            if(!ListaTrabalhosProntos.empty()) {
                t = &ListaTrabalhosProntos[it1]; // Pega o trabalho it, selecionado pelo tId ARRUMAR
                ListaTrabalhosProntos.erase(ListaTrabalhosProntos.begin()+it1);
                *res = (int *)t->funcao(t->pEntrada); //deu seg fault com 2 threads
                flag = 1;
            }

        }
    }
    /*
    if(ListaTrabalhosTerminados.empty()) {
        cout << "Lista vazia" << endl;
        return -2;
    }
     */

        for (it2 =0; it2 < ListaTrabalhosTerminados.size(); it2++) {
            cout <<"pelo menos entrou!"<< endl;
            if (ListaTrabalhosTerminados[it2].trabalhoID == tId) {
                t = &ListaTrabalhosTerminados[it2]; // Pega o trabalho it, selecionado pelo tId ARRUMAR
                *res = (int *) t->resultado;
                ListaTrabalhosTerminados.erase(ListaTrabalhosTerminados.begin()+it2);
                flag = 1;
                cout <<"feito uma!!"<< endl;
            }
        }
        //string h = NULL;
        //for(int k = 0; k != ListaTrabalhosProntos.size(); i++){h = h.concat }
        //cout << "trabalho pronto id: " << ListaTrabalhosProntos[0].trabalhoID << " size:"<<ListaTrabalhosProntos.size() << " tId: "<< tId << endl;
        if(flag == 0)std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    pthread_mutex_unlock(&mutex); // Fim secao critica
    cout << "liberou mutex sync" << endl;
    return 1;
}