#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

#define NUMBER_PRODUCED 1000
#define THREAD_DELAY 10
#define TIME_QUANTUM 500

// (convention : g_ stands for global variable, s_ for state, w_ for worker type)
// enum for different workers
enum Work{
    w_producer,
    w_consumer
};

// enum for different states 
enum State{
    s_running,
    s_stopped,
    s_complete
};


// structure to store the thread_state 
// storing things like pthread_t (would have been obtained by pthread_self())
struct thread_state {
    static int thread_count;            // total thread count
    static int producer_thread_count;   // total producer thread count
    static bool consumer_thread_dead;   // consumer thread states (are they waiting because no input and producers all terminated)
    int index;                          // current thread index
    pthread_t thread_id;                // current thread id 
    int work;                           // current worker type 
    State state;                        // current state 
    int ret;                            // return value 

    // constructor for creating the thread_state instance 
    thread_state(){                 
        this->index = thread_count;
        thread_count++;
        this->work  = rand() % 2;
        if(this->work == w_producer){
            producer_thread_count++;
        }
        this->state = s_stopped;
        this->ret = 0 ;
    }

    // function for displaying details regarding current thread 
    // (should be called through printthread for aesthetics)
    void print(){
        std::cout << std::setw(10) << index;
        std::cout << " ";
        std::cout << std::setw(15) << thread_id ;
        std::cout << ' ' ;
        if(work == w_producer){
            std::cout << std::setw(10) << "producer" ;
        }else{
            std::cout << std::setw(10) << "consumer" ;
        }
        std::cout << ' '; 
        if(state == s_complete){
            std::cout << std::setw(15) << "complete" ; 
        }else if(state == s_stopped){
            std::cout << std::setw(15) << "stopped"  ;
        }else {
            std::cout << std::setw(15) << "running" ; 
        }
        std::cout << std::endl;
    }
};
// initialising static data members of the class 
int thread_state::thread_count =  0;
int thread_state::producer_thread_count = 0 ;
bool thread_state::consumer_thread_dead = false;

// state variable for checking the program termination state (important for reporter)
bool program_reaching_termination = false;
bool schedular_initializes = false;
// attributes for pthread_create() function call 
pthread_attr_t attributes;

// mapping from pthread_t to thread_state instance (address references)
std::map <pthread_t , thread_state *> g_thread_map;

// array of all the thread_state instances in the order of their states
std::vector <thread_state *> g_thread_array;

// shared buffer (still confused about the implementation though)
int * g_buffer;
int g_buffer_size = 0;
int g_count = 0;
int g_buffer_in = 0 ;
int g_buffer_out =  0;

// helper function for printing threads 
void print_threads(int sig);
// function for signal handeling
void signal_handler(int sig);
// wrapper function around nanosleep() for allowing it to specifiy time in miliseconds 
void sleep_miliseconds(int milliseconds);

// void * functions (to be called as threads entry point)
void * job ( void * p);
void * reporter_job(void * args);
void * schedular_job(void * job);

// inline function for setting up the signals 
inline void install_signal_handler(){
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
}


// entry point of program
int main(){
    // input 
    int n,m;
    printf("Enter Number of threads : ");
    std::cin >> n;
    printf("Enter Buffer size       : ");
    std::cin >> m;

    // inialising attributes
    if(pthread_attr_init(&attributes)){
        perror("Error Occured\n");
        exit(EXIT_SUCCESS);
    }

    // declaring the schedular and reporter threads 
    pthread_t schedular_t , reporter_t ;

    // setting system such that pressing Ctrl+C causes the state of all the threads to display 
    // (exit using Ctrl+\)
    signal(SIGQUIT, print_threads);
    
    // initializing everything
    printf("Updating Buffer Resources \n");
    g_buffer = new int[m];
    g_buffer_size = m;


    // creates all the required thread ( the constructor has them default in stopped condition)
    printf("Generating Worker Threads\n");
    int i=0 ;
    while(i < n){
        thread_state * ptr = new thread_state;
        ptr->ret = pthread_create(&ptr->thread_id, &attributes, job, (void *)ptr);
        g_thread_map[ptr->thread_id] = ptr;
        g_thread_array.push_back(ptr);
        i++;
    }

    if(thread_state::producer_thread_count == g_thread_array.size()){
        printf("Exiting Program (All the worker created were producers)\n");
        exit(EXIT_FAILURE);
    }

    printf("Initial State of threads \n");

    print_threads(0);

    printf("\n");

    // creates reporter thread 
    printf("Generating Reporter Thread\n");
    int repo_ret = pthread_create(&reporter_t, &attributes, reporter_job, (void *)NULL);

    // waiting for reporter to get initialized 
    while(schedular_initializes == false);

    // creates schedular thread 
    printf("Generating Schedular Thread\n");
    int sch_ret = pthread_create(&schedular_t, &attributes, schedular_job, (void *)NULL);

    // joins the schedular thread 
    pthread_join(schedular_t, NULL);
    
    // joins the worker threads
    i=0;
    while(i < n){
        pthread_join(g_thread_array[i]->thread_id, NULL);
        i++;
    }

    // making program ready for termination 
    program_reaching_termination = true;

    // joins the reporter thread
    pthread_join(reporter_t, NULL);

    return 0;
}

// helper function for printing the thread 
void print_threads(int sig){
    std::cout << "\n";
    std::cout << std::setfill(' ');
    std::cout << std::setw(10) << "index";
    std::cout << ' ';
    std::cout << std::setw(15) << "pthread_t";
    std::cout << " " ;
    std::cout << std::setw(10) << "work";
    std::cout << " " ;
    std::cout << std::setw(15) << "state" << std::endl;
    std::cout << " ---------------------------------------------------------------" << std::endl;
    int i =0 ; 
    while( i < g_thread_array.size()){
        g_thread_array[i]->print();
        i++;
    }
    std::cout << "\n";
}

// entry function for worker threads
void * job ( void * p){
    // installing signal handler and getting the function argument 
    install_signal_handler();
    thread_state * ptr = (thread_state *) p;

    // if worker 
    if(ptr->work == w_producer){
        int counter = 0;
        // printf("Thread : %d , work : %d" , ptr->index, ptr->work);

        // produce NUMBER_PRODUCES number of pseudoreandoms
        while(counter < NUMBER_PRODUCED){
            // in waiting condition due to either full buffer or schedular 
            while(ptr->state == s_stopped || g_count == g_buffer_size);
            
            // generating random number and storing them while updating the counter, g_buffer_in, and g_count variables
            g_buffer[g_buffer_in] = rand()%1000;
            // printf("Thread Index : %d, Adds : %d, Index : %d\n",ptr->index, 
            //                                                     g_buffer[(g_buffer_in)], 
            //                                                     g_buffer_in);
            g_buffer_in = (g_buffer_in + 1 ) %g_buffer_size;
            g_count++;
            sleep_miliseconds(THREAD_DELAY);
            counter++;
        }
        // thread_complete 
        ptr->state = s_complete;
        // printf("Thread : %d completed\n", ptr->index);

        // recoding the termination of a producer thread 
        thread_state::producer_thread_count-- ;
    }
    else if(ptr->work == w_consumer){
        // printf("Thread : %d , work : %d" , ptr->index, ptr->work);
        // looping until there are producer threads or the death of a consumer thread has not occured 
        // (consumer thread will die only if the producers are all terminated and the buffer is empty)
        // (making it pointless to iterate through all the C threads)
        while((thread_state::producer_thread_count > 0 || g_count > 0)&&(thread_state::consumer_thread_dead == false)){
            while(g_count == 0 || ptr->state == s_stopped){
                // condition for checking if the consumer thread dies when other was in this loop (waiting)
                if(thread_state::consumer_thread_dead == true){
                    ptr->state = s_complete;
                    thread_state::consumer_thread_dead = true;
                    // printf("Thread : %d completed\n", ptr->index);
                    return NULL;
                }
            }
            // removing random number generated by producer and  updating the counter, g_buffer_in, and g_count variables
            int out = g_buffer[g_buffer_out];
            // printf("Thread Index : %d, Removes : %d, Index : %d\n", ptr->index, 
            //                                                         out, 
            //                                                         (g_buffer_out));
            g_buffer_out = (g_buffer_out + 1 )% g_buffer_size;
            g_count--;
            sleep_miliseconds(THREAD_DELAY);
        }
        // process complete 
        ptr->state = s_complete;

        // consumer termination flag set true
        thread_state::consumer_thread_dead = true;
    }
    return NULL;
}

void * schedular_job( void * ){
    // generating a queue for storing the processes for RR 
    std::queue<thread_state *> rr_queue;
    int i = 0 ;
    while(i < g_thread_array.size()){
        rr_queue.push(g_thread_array[i]);
        i++;
    }
    // current thread and next thread holder 
    thread_state *current_thread, *next_thread;
    current_thread = rr_queue.front(); rr_queue.pop();
    pthread_kill(current_thread->thread_id, SIGUSR2);

    // looping until the quit is empty and the death of a consumer thread has not occured 
    // (consumer thread will die only if the producers are all terminated and the buffer is empty)
    // (making it pointless to iterate through all the C threads)
    while(rr_queue.size() > 0 && thread_state::consumer_thread_dead == false){
        next_thread = rr_queue.front(); rr_queue.pop();

        //time quantum of TIME_QUANTUM millisecond 
        sleep_miliseconds(TIME_QUANTUM);
        pthread_kill(current_thread->thread_id, SIGUSR1);
        // printf("switching from %d to %d\n", current_thread->index, next_thread->index);
        pthread_kill(next_thread->thread_id, SIGUSR2);
        
        // checking the state of current thread and appending it back to rr_queue only if it is not complete 
        if(current_thread->state != s_complete){
            rr_queue.push(current_thread);
        }
        // setting next process as current process 
        current_thread = next_thread;
    }
    return NULL;
}


void * reporter_job(void * args){
    std::string initial_string, final_string;
    thread_state t1 , t2 ;
    int i;
    // temp vector to store the initial state of the status 
    std::vector<thread_state> prev_state;
    for(int i=0 ; i < g_thread_array.size() ; i++){
        prev_state.push_back(*g_thread_array[i]);
    }
    // setting the initlizer state true 
    schedular_initializes = true;

    // checking termination condition
    while(!program_reaching_termination){
        i=0 ; 
        while(i < g_thread_array.size()){
            t1 = prev_state[i];
            t2 = *g_thread_array[i];
            // checking state change 
            if(t1.state != t2.state){
                printf("[Thread(%d)] ", t1.index);
                fflush(stdout);
                if(t1.state == s_complete){
                    initial_string = "complete";
                }else if(t1.state == s_running){
                    initial_string = "running";
                }else{
                    initial_string = "stopped";
                }

                if(t2.state == s_complete){
                    final_string = "complete";
                }else if(t2.state == s_running){
                    final_string = "running";
                }else{
                    final_string = "stopped";
                }
                printf(" State Change : %s -> %s\n", initial_string.c_str(), final_string.c_str());
                fflush(stdout);
                prev_state[i].state = g_thread_array[i]->state;
            }
            i++;
        }
    }
    return NULL;
}

// warpper function around nanosleep()
void sleep_miliseconds(int milliseconds){
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

// signal handler 
void signal_handler(int sig){
    if(sig == SIGUSR1){
        g_thread_array[g_thread_map[pthread_self()]->index]->state = s_stopped;
    }
    else if (sig == SIGUSR2){
        g_thread_array[g_thread_map[pthread_self()]->index]->state = s_running;
    }
    install_signal_handler();
}
