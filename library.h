#ifndef TRABALHOPTHREADS_LIBRARY_H
#define TRABALHOPTHREADS_LIBRARY_H

int start(int m);
int finish(int m);
int spawn(struct Atrib* atrib, void *(*t) (void *), void* dta);
int sync(int tId, void* res);

struct Atrib {
    int p;
    int c;
};
struct Atrib;

#endif //TRABALHOPTHREADS_LIBRARY_H
