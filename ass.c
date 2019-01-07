#include<time.h>
#include<stdio.h> 
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
  
void swap(int* a, int* b) 
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 

void printArray(int arr[], int size) 
{ 
    int i; 
    for (i=0; i < size; i++) 
        printf("%d ", arr[i]); 
    printf("\n"); 
} 
  
int partition (int arr[], int low, int high) 
{ 
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

void quickSort(int arr[], int low, int high) 
{ 
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

int *processA(){
	int *res, i;
	res = (int *)malloc(50 * sizeof(int));
	for(i = 0; i<50; i++) 
  		res[i] = rand()%1000+1;
  	return res; 
}mbers in sorted order to proces

int *processB(){
	int *res, i;
	res = (int *)malloc(50 * sizeof(int));
	for(i = 0; i<50; i++) 
  		res[i] = rand()%1000+1;
  	return res; 
}

int *processC(){
	int *res, i;
	res = (int *)malloc(50 * sizeof(int));
	for(i = 0; i<50; i++) 
  		res[i] = rand()%1000+1;
  	return res; 
}

void processD(int *A, int *B){
	int *res = (int *)malloc(100 * sizeof(int));
	int i,j=0,k=0;
	for(i=0;i<100;i++){
		if(A[j] <= B[k]){
			res[i] = A[j];
			j++;
		}	else	{
			res[i] = B[k];
			k++;
		}
	}
	printArray(res, 100);
	printf("\n");
}

void processE(int *D, int*C){
	int *res = (int *)malloc(150 * sizeof(int));
	int i,j=0,k=0;
	for(i=0;i<150;i++){
		if(A[j] <= B[k]){
			res[i] = A[j];
			j++;
		}	else	{
			res[i] = B[k];
			k++;
		}
	}
	printArray(res, 100);
	printf("\n");
}


int main() 
{  
	srand(time(0));
	int i;
	int *arrA = processA();
	int *arrB = processA();
	int *arrC = processA();
	
	int A[2], B[2], i;
	
	int inbuf[INT_MAX];
	if(pipe(A) < 0 || pipe(B))
		exit(1);
	
	for(i=0;i<50;i++){
		write(A[1], arrA[i], INT_MAX);
		write(B[1], arrB[i], INT_MAX);	
	}
	
	
	quickSort(arrA, 0, 50);
	quickSort(arrB, 0, 50);
	
	processD(arrA, arrB);
	return 0;
} 

