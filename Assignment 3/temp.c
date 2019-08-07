#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct process_time{
	float iat;	// intern arival time
	float bt;		// cpu burst time
  float at;		// arival time
	float wt;		// wait time
	float tn;		// turn around time
	float rr;		// response time
	_Bool status;	// complete/pending
};

void printStructure(struct process_time *process, int n)  {
  int i;
  printf("interAT :: BurstTime :: ActualAT\n");
  for(i=0; i<n; i++)
      printf("%f :: %f :: %f\n", process[i].iat, process[i].bt, process[i].at);
}

double dmod(double x, double y)	{
	return x - (int)(x/y) * y;
}

void swapStructProcess(struct process_time* a, struct process_time* b){
    float t = a->iat;
    a->iat = b->iat;
    b->iat = t;

    t = a->bt;
    a->bt = b->bt;
    b->bt = t;

    t = a->at;
    a->at = b->at;
    b->at = t;

		t = a->wt;
		a->wt = b->wt;
		b->wt = t;

		t = a->tn;
		a->tn = b->tn;
		b->tn = t;

		t = a->rr;
		a->rr = b->rr;
		b->rr = t;
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
// choice = 1 for sorting based on burst time and = 0 for sorting based on Response ratio
int partitionStructProcess (struct process_time arr[], int low, int high, int choice){
	float pivot;
	if(choice == 1)
		 pivot= arr[high].bt;
	else
		pivot = arr[high].rr;
  int i = (low - 1);
  for (int j = low; j <= high- 1; j++)  {
		if(choice == 1){
			if(arr[j].bt <= pivot) {
	      i++;
	      swapStructProcess(&arr[i], &arr[j]);
	    }
		}	else	{
			if(arr[j].rr <= pivot)	{
				i++;
	      swapStructProcess(&arr[i], &arr[j]);
			}
		}
  }
  swapStructProcess(&arr[i + 1], &arr[high]);
  return (i + 1);
}

void quickSortStructProcess(struct process_time arr[], int low, int high, int choice){
  // printf("low = %d && high = %d\n", low, high);
  if(low < high){
    int pi = partitionStructProcess(arr, low, high, choice);
    quickSortStructProcess(arr, low , pi - 1, choice);
    quickSortStructProcess(arr, pi + 1, high, choice);
  }
}

double npFCFS(struct process_time *process, int n){
  int i;
  float *wt, *tn, average_turnaround_time = 0.0;
  wt = (float *)malloc(n * sizeof(float));
  tn = (float *)malloc(n * sizeof(float));
  wt[0] = 0;
  tn[0] = process[0].bt;
  average_turnaround_time += (float)tn[0]/(float)n;
  for(i=1; i<n; i++){
    wt[i] = wt[i-1] + process[i-1].bt - process[i].iat;
    if(wt[i] < 0.0)
      wt[i] = 0.0;
    tn[i] = wt[i] + process[i].bt;
    average_turnaround_time += (float)tn[i]/(float)n;
  }
  return average_turnaround_time;
}

double npSJF(struct process_time *process, int n){
  int i, j;
  // create seperate structure as it will be sorted later
  struct process_time *tempSJF;
  // initialize the new structure variable
  tempSJF = (struct process_time *)malloc(n * sizeof(struct process_time));
  tempSJF[0].iat = tempSJF[0].at = 0;
  tempSJF[0].bt = process[0].bt;
  for(i=1; i<n; i++){
    tempSJF[i].iat = process[i].iat;
    // calculate the actual arival time
    tempSJF[i].at = tempSJF[i-1].at + process[i].iat;
    tempSJF[i].bt = process[i].bt;
  }
  // float array to store wait time and turnover time
  float average_turnaround_time = 0.0, temp_min;
  i=0;
  // to decide the first process
  // printf("while loop starting\n");
  temp_min = tempSJF[0].bt;
  while(i<n) {
    if(tempSJF[i].at != 0.0)
      break;
    if(tempSJF[i].bt < temp_min)
      temp_min = tempSJF[i].bt;
    i++;
  }
  quickSortStructProcess(tempSJF, 0, i-1, 1);
  // printStructure(tempSJF, n);
  // initialize the value for the first process after sorting
  i=0;
  tempSJF[i].wt = 0;
  tempSJF[i].tn = tempSJF[i].wt + temp_min;
  // printf("tn[%d] = %f\n", i, tempSJF[i].tn);
  average_turnaround_time = (float)tempSJF[i].tn/n;
  int left, right;
  for(i=1; i<n; i++){
    left = i; right = i;
    // for each instance sorting the process which are in ready state to get the shortest job
    while(right+1 < n && tempSJF[right + 1].at <= tempSJF[right].at + tempSJF[right].wt + tempSJF[right].bt)  {
      // printf("%d\t", right);
      right++;
    }
    // printf("\nquick sort :: %d\nleft : %d ===== right : %d\n", i, left, right);
    quickSortStructProcess(tempSJF, left, right, 1);
    // printStructure(tempSJF, n);
    // printf("quick sort copmlete\n");
    // calculate wait time for each process which is the shortest job at that instance
    tempSJF[i].wt = tempSJF[i-1].wt + tempSJF[i-1].bt - tempSJF[i].at + tempSJF[i-1].at;
    // calculate turnover for each finally scheduled process
    tempSJF[i].tn = tempSJF[i].wt + tempSJF[i].bt;
    // printf("tn[%d] = %f\n", i, tempSJF[i].tn);
    // finally adding to the ATNtime
    average_turnaround_time += (float)tempSJF[i].tn/n;
    // printf("\nloop : %d | completed\n", i);
  }
  return average_turnaround_time;
}

double pSJF(struct process_time *process, int n){
  int i, j, min;
  struct process_time *tempSJF;
  tempSJF = (struct process_time *)malloc(n * sizeof(struct process_time));
  float *bt, temp_min, global_total_time = 0.0, current_time = 0.0, average_turnaround_time = 0.0;
	bt = (float *)malloc(n * sizeof(float));
	// intializing the temeporary structure
  i=0;
  tempSJF[i].iat = tempSJF[i].at = tempSJF[i].wt = 0;
  tempSJF[i].bt = global_total_time = process[i].bt;
	tempSJF[i].status = 0;
  for(i=1; i<n; i++){
    bt[i] = tempSJF[i].bt = process[i].bt;
    global_total_time += tempSJF[i].bt;
    tempSJF[i].iat = process[i].iat;
    tempSJF[i].at = tempSJF[i-1].at + tempSJF[i].iat;
		tempSJF[i].wt = 0;
		tempSJF[i].status = 0;
  }

	current_time = 0.0;
  while (current_time < global_total_time) {
		// printf("current time = %f\n", current_time);
    i=0, j=0;
		temp_min = 21;	// max burst time is 20 assumtion in question
		while(i < n && tempSJF[i].at <= current_time)	{
			if(temp_min > tempSJF[i].bt && !tempSJF[i].status){
				temp_min = tempSJF[i].bt;
				min = i;
			}
			i++;
		}
		// i is the index of the process which is about to arrive
		// printf("minimum : %d\n", min);
		if(i == n){		// if all the process are in ready state
			// printf("a\n");
			current_time += tempSJF[min].bt;
			tempSJF[min].wt = current_time - tempSJF[i].at - bt[i];	// have to take the original bt
			tempSJF[min].tn = tempSJF[min].wt + bt[min];
			if(tempSJF[min].wt < 0)	tempSJF[min].wt = 0;
			tempSJF[min].bt = 0;
			tempSJF[min].status = 1;
		}	else	{		// if process a new process is about to arrive before completion of process
								// having minimum burst time at the moment
			// printf("b\n");
			if(tempSJF[i].at < current_time + tempSJF[min].bt){
				tempSJF[min].bt -= tempSJF[i].at - current_time;
				current_time = tempSJF[i].at;
			}	else	{	// if the new process is arriving after the completion of current running process
				// case when tempSJF[i].at >= current_time + tempSJF[min].bt
				// printf("c\n");
				tempSJF[min].wt = current_time + tempSJF[min].bt - bt[min] - tempSJF[min].at;
				tempSJF[min].tn = tempSJF[min].wt + bt[min];
				current_time += tempSJF[i].bt;
				tempSJF[min].bt = 0;
				tempSJF[min].status = 1;
			}
		}
		// for(j=0; j<n; j++){
		// 	printf("%f || ", tempSJF[j].bt);
		// }
		// printf("\n");
		printf("pSJF --> current time = %f\t :: Global total time = %f\n", current_time, global_total_time);
  }
	// printf("all done\n");
	for(i=0; i<n; i++){
		average_turnaround_time += (float)tempSJF[i].tn/n;
	}
	return average_turnaround_time;
}

double roundRobin(struct process_time *process, int n){
	int i, j, k;
	int process_complete, ready_process;
	float *wt, *tn, *bt, current_time = 0.0, quantum_time = 2.0, average_turnaround_time = 0.0;
	wt = (float *)malloc(n * sizeof(float));
	tn = (float *)malloc(n * sizeof(float));
	bt = (float *)malloc(n * sizeof(float));
	struct process_time *tempRoundRobin;
	tempRoundRobin = (struct process_time *)malloc(n * sizeof(struct process_time));
	// initialization of the temporary values
	i=0;
	tempRoundRobin[i].at = tempRoundRobin[i].iat = 0;
	bt[i] = tempRoundRobin[i].bt = process[i].bt;
	wt[i] = 0;
	for(i=1; i<n; i++){
		tempRoundRobin[i].iat = process[i].iat;
		tempRoundRobin[i].at = tempRoundRobin[i-1].at + tempRoundRobin[i].iat;
		bt[i] = tempRoundRobin[i].bt = process[i].bt;
		wt[i] = 0;
	}
	i=0;	j=0; k=0;
	ready_process = 0;
	process_complete = 0;
	// _Bool first = 1;
	// until all the process has completed
	while(process_complete < n){
		j = i;
		// get the last index of the process in ready state
		while(tempRoundRobin[j].at  <= current_time && j<n) j++;
		// go to process next to the process that just executed
		i++;
		// check if the process is in the ready state or not,... and validity
		// if yes then check the burst time if zero move to the next process
		// if the last process is reached and has zero burst time then move to
		// first process having non-zero burst time.
		while (tempRoundRobin[i].bt <= 0.0 && i<j) i++;
		// incase i reached up to j means all the process has parsed already
		// then move back to zero process and find the first non-zero process
		if(i >= j)	{
			i = 0;
			// found the first process having non-zero burst time starting from zero index
			while (tempRoundRobin[i].bt <= 0.0 && i<j)	i++;
		}
		// printf("%d :: %d :: %d\nstart\n", i, j , k);

		// checking wether the process require whole quantum time or not
		if(tempRoundRobin[i].bt <= quantum_time){
			// add tempRoundRobin[i].bt to tvery potential proess's wait time
			for(k=0; k<j; k++){
				if(k == i) continue;
				if(tempRoundRobin[k].bt <= 0)	continue;
				wt[k] += tempRoundRobin[i].bt;
			}
			// update the current time
			current_time += tempRoundRobin[i].bt;
			tempRoundRobin[i].bt = 0;
			// process has been completed at this point
			process_complete++;
		}	else{
			tempRoundRobin[i].bt -= quantum_time;
			// wt[i] -= quantum_time;
			// add quatum number to every potential process's wait time
			for(k=0; k<j; k++){
				if(k == i) continue;
				if(tempRoundRobin[k].bt <= 0)	continue;
				wt[k] += quantum_time;
			}
			// update the current time
			current_time += quantum_time;
		}
		// printf("%d :: %d :: %d\nend\n", i, j , k);
		// printStructure(tempRoundRobin, n);
		printf("round robin --> number of process completed = %d\n", process_complete);
	}
	// printf("\n%f is the current time\n", current_time);
	// calculating the turn around time and adding it to the average turn around time
	for(i=0; i<n; i++)	{
		tn[i] = wt[i] + bt[i];
		average_turnaround_time += (float)tn[i]/(float)n;
	}
	// for(i=0;i<n;i++)	printf("%f\t", wt[i]);
	// printf("\n");
	return average_turnaround_time;
}

double hRRF(struct process_time *process, int n){
	int i, j, k, process_complete, index_of_minimum_burst;
	float current_time = 0.0, average_turnaround_time = 0.0;
	struct process_time *tempHRRF;
	tempHRRF = (struct process_time *)malloc(n * sizeof(struct process_time));
	// initialize all the values
	i = 0;
	tempHRRF[i].at = tempHRRF[i].iat = tempHRRF[i].wt = 0;
	tempHRRF[i].bt = process[i].bt;
	tempHRRF[i].rr = 1;
	for(i=1; i<n; i++){
		tempHRRF[i].iat = process[i].iat;
		tempHRRF[i].at = tempHRRF[i-1].at + process[i].iat;
		tempHRRF[i].bt = tempHRRF[i].rr = process[i].bt;
		tempHRRF[i].wt = 0;	tempHRRF[i].rr = 1;
	}
	int index;
	i=0; // process count
	while(i < n){
		// printf("%d.start current time = %f-->\n",i, current_time);
		// for(j=0;j<n;j++)	printf("%f | ", tempHRRF[j].rr);
		// printf("\n");
		j = 0;
		while (tempHRRF[j].rr == 0 && j < n)	j++;
		k = j;
		// printf("k = %d\n", k);
		while (tempHRRF[k].at <= current_time && k<n) k++;
		// printf("%d :: %d :: %d \n",i, j, k-1 );
		quickSortStructProcess(tempHRRF, j , k-1, 0);
		// for(index=0;index<n;index++)	printf("%f | ", tempHRRF[index].rr);
		// printf("\n");
		// printf("sorting loop :: %d\n", i);
		tempHRRF[k-1].wt = current_time - tempHRRF[k-1].at;
		tempHRRF[k-1].tn = tempHRRF[k-1].wt + tempHRRF[k-1].bt;
		tempHRRF[k-1].rr = 0;
		current_time += tempHRRF[k-1].bt;
		for(++j;j<n;j++){
			if(tempHRRF[j].at > current_time)	break;
			if(j == k-1)	continue;
			tempHRRF[j].wt += current_time - tempHRRF[j].at;
			tempHRRF[j].rr = (float)(tempHRRF[j].wt + tempHRRF[j].bt)/tempHRRF[j].bt;
		}
		// for(index=0;index<n;index++)	printf("%f | ", tempHRRF[index].rr);
		// printf("\n");
		// printf("current time = %f\n\n\n", current_time);
		printf("highest response ratio first --> number of process completed = %d\n", i);
		i++;
	}
	// printf("all done\n");
	for(i=0; i<n; i++){
		average_turnaround_time += (float)tempHRRF[i].tn/n;
	}
	return average_turnaround_time;
}

int main(int argc, char const *argv[]) {
	if(argc < 3){
		printf("missing input : number of process\nprogram halting...\n");
		exit(1);
	}
	printf("number of process entered is : %d\n", atoi(argv[1]));
	srand(time(0));
	int n = atoi(argv[1]), i;
  // printf("Enter the number of process : ");
  // scanf("%d", &n);
  struct process_time *process;
  process = (struct process_time *)malloc(n * sizeof(struct process_time));
  // first process arrival time is to be considered 0
  i=0;
  process[i].iat = process[i].at = 0.0;
  process[i].bt = randomUniform(20.0);
  // printf("%f :: %f :: %f\n", process[i].iat, process[i].bt, process[i].at);
  for(i=1; i<n ;i++){
    process[i].iat = randomExponent(1);
    process[i].bt = randomUniform(20.0);
    process[i].at = process[i-1].at + process[i].iat;
    // printf("%f :: %f :: %f\n", process[i].iat, process[i].bt, process[i].at);
  }
  // opening a file to write the data generated randomly
  FILE *fptr;
  fptr = fopen("data.csv" , "a");
  // writing the arrival time in the file
	fprintf(fptr, "Inter Arival Time,CPU Burst time\n");
  for(i=0 ;i<n; i++)
    fprintf(fptr, "%f,%f\n", process[i].iat, process[i].bt);
	fprintf(fptr, "\n");
	fclose(fptr);
  // separating the exponential random numbers from others by the token '##'
	// fprintf(fptr, "\n##\n");
  // writing the burst time in the file
  // for(i=0; i<n; i++)
    // fprintf(fptr, "%f\t", process[i].bt);
  // seperating random number following uniform distribution by the token '**'
	// fprintf(fptr, "\n**\n");
  printf("Wrinting in file is completed\n");

  // printing the structure generated from the random numbers
  printStructure(process, n);
  printf("\n");

	float npFCFS_r = npFCFS(process, n);
	float npSJF_r = npSJF(process, n);
	float pSJF_r = pSJF(process, n);
	float roundRobin_r = roundRobin(process, n);
	float hRRF_r = hRRF(process, n);

	printf("Average turnaround time for NP-FCFS = %f\n", npFCFS_r);
  printf("Average turnaround time for NP_SJF  = %f\n", npSJF_r);
  printf("Average turnaround time for P_SJF  = %f\n", pSJF_r);
  printf("Average turnaround time for PRR  = %f\n" , roundRobin_r);
	printf("Average turnaround time for HRRF  = %f\n", hRRF_r);

	fptr = fopen("result.csv" , "a");
	// fprintf(fptr, "%s,%s,%s,%s,%s,%s\n", "n", "npFCFS_r", "npSJF_r", "pSJF_r", "roundRobin_r", "hRRF_r");
	fprintf(fptr, "%d,%f,%f,%f,%f,%f\n", n, npFCFS_r, npSJF_r, pSJF_r, roundRobin_r, hRRF_r);
  return 0;
}
