#include <iostream>
#include <pthread.h>
#include "library.h"
#include <time.h>
using namespace std;

void printArray(int *array, int n)
{
    for (int i = 0; i < n; ++i) {
        cout << array[i] << " ";
    }
    cout << endl;
}

struct Params{
    int low;
    int* array;
    int high;
};


//void quickSort(int *array, int low, int high)
void* quickSort(void* params)

{
    //Params paramsLocal = (*((Params*)params));
    Params* paramsLocal = (Params*)params;
    int i = paramsLocal->low;
    int j = paramsLocal->high;
    int pivot = paramsLocal->array[(i + j) / 2];
    int* array = paramsLocal->array;
    int temp;

    while (i <= j)
    {
        while (array[i] < pivot)
            i++;
        while (array[j] > pivot)
            j--;
        if (i <= j)
        {
            temp = array[i];
            array[i] = array[j];
            array[j] = temp;
            i++;
            j--;
        }
    }
    Params *b = new Params();
    Params *c = new Params();
    int k = -1, l = -1;

    if (j > paramsLocal->low){
        b->array = array;
        b->low = paramsLocal->low;
        b->high = j;
        k = spawn(NULL, quickSort, (void*)b);
    }

    if (i < paramsLocal->high){
        c->array = array;
        c->low = i;
        c->high = paramsLocal->high;
        l = spawn(NULL, quickSort, (void*)c);
    }
    int *rFat;
    if(k != -1) sync(k, (void**)&rFat);
    if(l != -1) sync(l, (void**)&rFat);


    return (void*)&paramsLocal->high;
}

int main()
{
    int *array, tId, k=4, tam=10;

//    cout << "Qtde thread: ";
//    cin >> tam;
//    cout << "Informe o tamanho do array para Quick Sort: ";
//    cin >> k;

    array = (int*)malloc(tam*sizeof(int));

    srand(time(NULL));
    for(int i=0;i<tam;i++){
        array[i] = rand() % 100;
    }

    Params *params = new Params();
    params->array = array;
    params->low = 0;
    params->high = tam;

    cout << "Before Quick Sort :" << endl;
    printArray(array, tam);

    start(k);

    tId = spawn(NULL, quickSort, (void*)params);
    int* ret;
    sync(tId, (void**)&ret);

    cout << "After Quick Sort :" << endl;
    printArray(array, tam);

    finish();
    free(array);

    return 0;
}
