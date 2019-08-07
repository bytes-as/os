#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

int main()
{
    // vector container stores threads
    std::vector<std::thread> workers;
    for (int i = 0; i < 5; i++) {
        workers.push_back(std::thread ([]()
        {
            std::cout << "thread function\n";
        }));
    }
    std::cout << "main thread\n";

    // Looping every thread via for_each
    // The 3rd argument assigns a task
    // It tells the compiler we're using lambda ([])
    // The lambda function takes its argument as a reference to a thread, t
    // Then, joins one by one, and this works like barrier
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    return 0;
}

int main(){
	int i,x;
	pthread_t  *worker_id,sch_id,rep_id;
	pthread_attr_t *attr;
	//struct sigaction usr_action;
	srand(time(0));
	// printf("enter the number of thread: \n");
	// scanf("%d",&N);
	printf("enter the no.of concurrent threads to be created N : \n");
	scanf("%d",&N);

	printf("enter the size of BUFFER M: \n");
	scanf("%d",&M);

	BUFFER = (int*)malloc(M*sizeof(int));
	STATUS = (int*)malloc(N*sizeof(int));

	worker_id=(pthread_t*)malloc(N*sizeof(pthread_t));
	//attr=(pthread_attr_t*)malloc(N*sizeof(pthread_attr_t));

	for(i=0;i<N;i++){
		//set_sig_handler();
		pthread_attr_init(attr);
	}

	for(i=0;i<N;i++){
		x=rand()%2;
		if(x==0) pthread_create(&worker_id[i],&attr,P,(void*)i);
		else pthread_create(&worker_id[i],&attr,C,(void*)i);

	}
	pthread_create(&sch_id , &attr , Scheduler ,NULL);
	pthread_create(&rep_id , &attr , Reporter , NULL);
	// for(i=0;i<N;i++){
	// 	pthread_join(worker_id[i],NULL);
	// }

	for(i=0;i<N;i++){
		printf("%d\t",BUFFER[i]);
	}
	printf("\n");

	return 0;
}
