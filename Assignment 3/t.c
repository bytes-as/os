#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct process_time{
	float iat;
	float bt;
  float at;
};
void printStructure(struct process_time *process, int n)  {
  int i;
  for(i=0; i<n; i++)
      printf("%f :: %f :: %f\n", process[i].iat, process[i].bt, process[i].at);
}

double dmod(double x, double y)	{
	return x - (int)(x/y) * y;
}

double randomExponent(double lambda)	{
	double random, result;
	random = (double)rand() / RAND_MAX;
  // set up the values to get the random variable value b/w 0 to 10
	random = dmod(random, 1 - exp(-10*lambda)) + exp(-10 * lambda);
	// printf("%f :: ", random);
	result = (-log(random) / lambda);
	return result;
}

double randomUniform(double max)	{
	double random;
	random = ((double)rand() / RAND_MAX) * max;
	return random;
}

void swapStructProcess(struct process_time* a, struct process_time* b)
{
    float t = a->iat;
    a->iat = b->iat;
    b->iat = t;

    t = a->bt;
    a->bt = b->bt;
    b->bt = t;

    t = a->at;
    a->at = b->at;
    b->at = t;
}

int partitionStructProcess (struct process_time arr[], int low, int high){
    float pivot = arr[high].iat;
    int i = (low - 1);
    for (int j = low; j <= high- 1; j++)  {
        if (arr[j].iat <= pivot) {
            i++;
            swapStructProcess(&arr[i], &arr[j]);
        }
    }
    swapStructProcess(&arr[i + 1], &arr[high]);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSortStructProcess(struct process_time arr[], int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partitionStructProcess(arr, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSortStructProcess(arr, low, pi - 1);
        quickSortStructProcess(arr, pi + 1, high);
    }
}

int main(int argc, char const *argv[]) {
  int n, i;
  printf("Enter the number of process : ");
  scanf("%d", &n);
  struct process_time *process;
  process = (struct process_time *)malloc(n * sizeof(struct process_time));
  // first process arrival time is to be considered 0
  i=0;
  process[i].iat = process[i].at = 0.0;
  process[i].bt = randomUniform(20.0);
  printf("%f :: %f :: %f\n", process[i].iat, process[i].bt, process[i].at);
  for(i=1; i<n ;i++){
    process[i].iat = randomExponent(10);
    process[i].bt = randomUniform(20.0);
    process[i].at = process[i-1].at + process[i].iat;
    printf("%f :: %f :: %f\n", process[i].iat, process[i].bt, process[i].at);
  }
  quickSortStructProcess(process, 0, n-1);
  printf("sorted array is : \n");
  printStructure(process, n);
  return 0;
}
