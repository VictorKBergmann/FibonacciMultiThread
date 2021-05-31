#include "library.h"
#include <malloc.h>
#include <iostream>

using namespace std;
extern int m;

void* fat( void* dta ) {
    long int *n = (long int *) dta;
    long int *rFat = (long int *) malloc(sizeof(long int)),
        *n1, *r1, t1;

    if(*n <= 1) *rFat = 1;
    else {
        n1 = (long int *) malloc(sizeof(long int));
        *n1 = *n - 1;
        t1 = spawn( NULL, fat, (void*) n1 );

        sync(t1, (void**) &r1);

        *rFat = (*n) * (*r1);

        free(n1);
        free(r1);
    }
    return rFat;
}

int main() {
    int n, tId;
    long int *rFat;
    //struct Atrib a;
    cout << "Qtde thread: ";
    cin >> m;
    cout << "Digite o numero para Fatorial: ";
    cin >> n;

    start(m);

    tId = spawn(NULL, fat, (void*) &n);
    sync(tId,(void**)&rFat);

    cout << "Fatorial(" << n << "): " << *rFat << endl;

    finish();
    free(rFat);

    return 0;
}
