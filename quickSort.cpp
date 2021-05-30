#include <iostream>
#include <pthread.h>
#include "library.h"
using namespace std;

void printArray(int *array, int n)
{
    for (int i = 0; i < n; ++i) {
        cout << array[i] << " ";
    }
}


struct Params{
    int *array;
    int low;
    int high;
    Params(int* array_, int low_, int high_){
        array = array_;
        low = low_;
        high = high_;
    }
};


//void quickSort(int *array, int low, int high)
void* quickSort(void* params)

{
    Params paramsLocal = (*((Params*)params));
    int i = paramsLocal.low;
    int j = paramsLocal.high;
    int pivot = paramsLocal.array[(i + j) / 2];
    int* array = paramsLocal.array;
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
    Params *b, *c;
    int k = -1, l = -1;

    if (j > paramsLocal.low){

        b = new Params(array, paramsLocal.low, j);
        k = spawn(NULL, quickSort, b);
    }

    if (i < paramsLocal.high){
        c = new Params(array, i, paramsLocal.high);
        l = spawn(NULL, quickSort, c);
    }
    int *rFat;
    if(k != -1) sync(k, (void**)&rFat);
    if(l != -1) sync(l, (void**)&rFat);

    free(c);
    free(b);

    return (void*)2;
}

int main()
{

    int array[] = {95, 45, 48, 98, 1, 485, 65, 478, 10, 2325};
    int n = sizeof(array)/sizeof(array[0]);

    int thread;
    Params params(array,0,n);

    cout << "Before Quick Sort :" << endl;
    printArray(array, n);

    cout << "Qtde thread: " << endl;
    int u = 4;
    //cin >> m;
    start(u);

    thread = spawn(NULL, quickSort, &params);
    // quickSort(array, 0, n);
    int *rFat;
    sync(thread, (void**)&rFat);

    cout << "After Quick Sort :" << endl;

    printArray(array, n);

    finish();

    return (0);
}
