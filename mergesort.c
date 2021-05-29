#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int low, mid, high, size;
    int* arr;
    //pthread_mutex_t m;
}Array;

void merge(int low, int mid, int high, int* arr);
void* merge_sort_thread(void* arg);
void merge_sort(int low, int high, int* arr);

int part = 0;
int numThreads;

int main(int argc, char** argv)
{
    Array myArr;
    numThreads = atoi(argv[1]);
    int arrSize = atoi(argv[2]);

    myArr.size = arrSize;
    myArr.arr = (int*)malloc(arrSize*sizeof(int));

    // insere valores random no array
    srand(time(NULL));
    for (int i = 0; i < arrSize; i++)
        myArr.arr[i] = rand() % 100;
  
    printf("Unsorted array: ");
    for (int i = 0; i < arrSize; i++)
        printf("%d ",myArr.arr[i]);
    printf("\n");

    pthread_t tid[numThreads];
    //pthread_mutex_init(&myArr.m,NULL);
  
    //creating threads
    for (int i = 0; i < numThreads; i++)
        pthread_create(&(tid[i]), NULL, merge_sort_thread, (void*)&myArr);
  
    // joining all threads
    for (int i = 0; i < numThreads; i++)
       pthread_join(tid[i], NULL);
   
    //merging the final parts
    merge(0, (arrSize / 2 - 1) / 2, arrSize / 2 - 1, myArr.arr);
    merge(arrSize / 2, arrSize/2 + (arrSize-1-arrSize/2)/2, arrSize - 1, myArr.arr);
    merge(0, (arrSize - 1)/2, arrSize - 1, myArr.arr);

    // displaying sorted array
    printf("Sorted array: ");
    for (int i = 0; i < arrSize; i++)
        printf("%d ",myArr.arr[i]);
    printf("\n");

    return 0;
}

void merge(int low, int mid, int high, int* a) {
    int count = high - low + 1;
    int sorted[count];
    int i = low, k = mid + 1, m = 0;
    while (i <= mid && k <= high) {
        if (a[i]<a[k])
            sorted[m++] = a[i++];
        else if (a[k]<a[i])
            sorted[m++] = a[k++];
        else if (a[i] == a[k]) {
            sorted[m++] = a[i++];
            sorted[m++] = a[k++];
        }
    }
    while (i <= mid)
        sorted[m++] = a[i++];

    while (k <= high)
        sorted[m++] = a[k++];

    int arr_count = low;
    for (i = 0; i<count; i++, low++)
        a[low] = sorted[i];
}

// merge sort function
void merge_sort(int low, int high, int* a) {
    // calculating mid point of array
    int mid = low + (high - low) / 2;
    if (low < high) {

        // calling first half
        merge_sort(low, mid, a);

        // calling second half
        merge_sort(mid + 1, high, a);

        // merging the two halves
        merge(low, mid, high, a);
    }
}

// thread function for multi-threading
void* merge_sort_thread(void* arg) {
    Array* a = (Array*) arg;
    int thread_part = part++;

    // calculating low and high
    int low = thread_part * (a->size / numThreads);
    int high = (thread_part + 1) * (a->size / numThreads) - 1;

    // allocate the rest part of original array to the last thread
    if(thread_part == numThreads -1){
        high = a->size - 1;
    }

    // evaluating mid point
    int mid = low + (high - low) / 2;

    merge_sort(low, mid, a->arr);
    merge_sort(mid + 1, high, a->arr);
    merge(low, mid, high, a->arr);
    return 0;
}