#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>

void swap(int* a, int* b){
    int t = *a;
    *a = *b;
    *b = t;
}

void printArray(int arr[], int size){
    int i;
    for (i=0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int partition (int arr[], int low, int high){
    int pivot = arr[high];    // pivot
    int i = (low - 1), j;  // Index of smaller element

    for (j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int arr[], int low, int high){
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int *random_array(int n){
	int *res, i;
	res = (int *)malloc(n * sizeof(int));
	for(i = 0; i<n; i++)
  		res[i] = rand()%1000+1;
  	return res;
}

int *merge(int *A, int *B, int n, int m){
	int *res = (int *)malloc(n * sizeof(int));
	int i=0,j=0,k=0;
	while(j<n && k<n){
    if(A[j] <= B[k]){
			res[i] = A[j];
			j++;
		}	else	{
			res[i] = B[k];
			k++;
		}
    i++;
  }
  while(j<n){
      res[i] = A[j];
      j++;i++;
  }
  while(k<n){
    res[i] = B[k];
    k++;i++;
  }
	// printArray(res, 100);
	// printf("\n");
  return res;
}

int main()
{
	// srand(time(0));
  // int *arrE = (int *)malloc(150 * sizeof(int));
  // int *temp = (int *)malloc(100 * sizeof(int));
	int i, AD[2], BD[2], DE[2], CE[2];
  // int fork1, fork2, fork3, size;


	//int inbuf[INT_MAX];
	if(pipe(AD) < 0 || pipe(BD) < 0 || pipe(DE) < 0 || pipe(CE) < 0)
		exit(1);

  if(!fork()){
      if(!fork()){
        if(!fork()){          //////////////     # Process D
          printf("Process D\n");
          int *A, *B, *D;
          A = (int *)malloc(50 * sizeof(50));
          B = (int *)malloc(50 * sizeof(50));
          for(i=0; i<50; i++){
            read(AD[0], &A[i], sizeof(int));
            read(BD[0], &B[i], sizeof(int));
          }
          close(AD[0]);
          close(BD[0]);

          D = merge(A, B, 50, 50); // merge number of the array of A and B in sorted manner
          // printArray(D, 100);
          for(i=0;i<100;i++)
            write(DE[1], &D[i], sizeof(int));
          close(DE[1]);
          // printf("process D completed\n");
        } else  {             //////////////     # Process C
            printf("Process C\n");
            int *arrC = (int *)malloc(50 * sizeof(int));
            arrC = random_array(50);
            quickSort(arrC, 0 , 50);
            // printArray(arrC, 50);
            for(i=0;i<50;i++)
              write(CE[1], &arrC[i], sizeof(int));
            close(CE[1]);
            // printf("process C completed\n");
        }
      } else  {
        if(!fork()){          //////////////     # Process A
          printf("Process A\n");
          int *arrA = (int *)malloc(50 * sizeof(int));
          arrA = random_array(50);
          quickSort(arrA, 0 , 50);
          // printArray(arrA, 50);
          for(i=0;i<50;i++)
        		write(AD[1], &arrA[i], sizeof(int));
          close(AD[1]);
          // printf("process A completed\n");
        } else  {             //////////////      # Process B
            printf("Process B\n");
            int *arrB = (int *)malloc(50 * sizeof(int));
            arrB = random_array(50);
            quickSort(arrB, 0 , 50);
            // printArray(arrB, 50);
            for(i=0;i<50;i++)
        		    write(BD[1], &arrB[i], sizeof(int));
            close(BD[1]);
            // printf("process B completed\n");
        }
      }
  } else  {                   //////////////        # Process E
    printf("Process E\n");
    int *arrAB, *arrC, *arrE;
    arrAB = (int *)malloc(100 * sizeof(int));
    arrC = (int *)malloc(50 * sizeof(int));
    for(i=0; i<100; i++)
      read(DE[0], &arrAB[i], sizeof(int));
    for(i=0; i<50; i++)
      read(CE[0], &arrC[i], sizeof(int));
    close(DE[0]);
    close(CE[0]);
    arrE = merge(arrAB, arrC, 100, 50);
    printArray(arrE, 150);
    printf("All process completed\n");
  }
  return 0;
}
