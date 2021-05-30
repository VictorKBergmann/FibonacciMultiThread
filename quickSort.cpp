#include <iostream>
#include <pthread.h>
#include "library.h"

void printArray(int *array, int n)
{
    for (int i = 0; i < n; ++i)
        std::cout << array[i] << std::endl;
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
} ;


//void quickSort(int *array, int low, int high)
void* quickSort(void* params)

{
    Params paramsLocal = (*((Params*)params))
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
        c = new Params(array, paramsLocal.low, j);
        l = spawn(NULL, quickSort, c);
    }
    if(k != -1){sync(k, NULL);}
    if(l != -1){sync(l, NULL);}

    free(b);
    free(c);
    return NULL;
}

int main()
{
    
    int array[] = {95, 45, 48, 98, 1, 485, 65, 478, 1, 2325};
    int n = sizeof(array)/sizeof(array[0]);

    int thread;
    Params params(array,0,n);

    std::cout << "Before Quick Sort :" << std::endl;
    printArray(array, n);

    cout << "Qtde thread: ";
    int m;
    cin >> m;
    start(m);

    thread = spawn(NULL, quickSort, &params)
    // quickSort(array, 0, n);

    sync(thread, NULL);

    std::cout << "After Quick Sort :" << std::endl;
    printArray(array, n);

    finish();

    return (0);
}
