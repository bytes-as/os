#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUMBER_PRODUCED 1000

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
    static int thread_count;
    static int producer_thread_count;
    static bool consumer_thread_dead;
    int index;
    pthread_t thread_id;
    int work;
    State state;
    int ret;

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

    void print(){
        cout<<std::setw(10) << index;
        cout<<" ";
        cout<<std::setw(15) << thread_id ;
        cout<<" ";
        if(work == w_producer)
            cout<<std::setw(10)<<"producer";
        else
            cout<<std::setw(10)<<"consumer";
        cout<<" ";
        if(state == s_complete)
            cout<<std::setw(15)<<"complete";
        else if(state == s_stopped)
            cout<<std::setw(15)<<"stopped";
        else
            cout<<std::setw(15)<<"running";
        cout<<endl;
    }
};
// initializing the counting variables
int thread_state::thread_count =  0;
int thread_state::producer_thread_count = 0 ;
bool thread_state::consumer_thread_dead = false;

// just a check for all the process
bool program_reaching_termination = false;
bool scheduler_initializes = false;

pthread_attr_t attributes;

// mapping the pthread_id to the their state parameter of the structure
std::map <pthread_t , thread_state *> g_thread_map;

// array to store the state of all the threads
std::vector <thread_state *> g_thread_array;

int * g_buffer;
int g_buffer_size = 0;
int g_count = 0;
int g_buffer_in = 0 ;
int g_buffer_out =  0;

// print the state of the threads
void printThreads(int sig);

void signal_handler(int sig);

void *job (void *p);
void *reporterJob(void *args);
void *schedulerJob(void *job);

// inline function for setting up the signals
inline void install_signal_handler(){
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
}

void signal_handler(int sig){
    if(sig == SIGUSR1){
        g_thread_array[g_thread_map[pthread_self()]->index]->state = s_stopped;
    }
    else if (sig == SIGUSR2){
        g_thread_array[g_thread_map[pthread_self()]->index]->state = s_running;
    }
    install_signal_handler();
}

// entry point of program
int main(){
    // input
    int N,M;
    printf("Enter the nuber of threads: ");
    cin>>N;
    printf("Enter the size of buffer: ");
    cin>>M;

    // inialising attributes
    if(pthread_attr_init(&attributes)){
        perror("Error setting attribute\n");
        exit(EXIT_SUCCESS);
    }

    pthread_t scheduler_t , reporter_t ;

    signal(SIGQUIT, printThreads);

    // initializing the buffer
    g_buffer = new int[M];
    g_buffer_size = M;


    printf("creating threads ...\n");
    int i=0 ;
    while(i < N){
        thread_state * ptr = new thread_state;
        ptr->ret = pthread_create(&ptr->thread_id, &attributes, job, (void *)ptr);
        g_thread_map[ptr->thread_id] = ptr;
        g_thread_array.push_back(ptr);
        i++;
    }

    if(thread_state::producer_thread_count == g_thread_array.size()){
        printf("no consumer is created ... \nexiting ...\n");
        exit(EXIT_FAILURE);
    }
    printf("\n");

    if(thread_state::producer_thread_count == 0){
        printf("no producers is created ... \nexiting ...\n");
        exit(EXIT_FAILURE);
    }
    // creates reporter thread
    printf("creating reporter thread ...\n");
    pthread_create(&reporter_t, &attributes, reporterJob, (void *)NULL);

    // waiting for reporter to get initialized
    while(scheduler_initializes == false);

    // creates scheduler thread
    printf("Generating scheduler Thread\n");
    int sch_ret = pthread_create(&scheduler_t, &attributes, schedulerJob, (void *)NULL);

    // joins the scheduler thread
    pthread_join(scheduler_t, NULL);

    // joins the worker threads
    i=0;
    while(i < N){
        pthread_join(g_thread_array[i]->thread_id, NULL);
        i++;
    }
    // making program ready for termination
    program_reaching_termination = true;

    // joins the reporter thread
    pthread_join(reporter_t, NULL);

    printThreads(0);
    return 0;
}

void printThreads(int sig){
    cout<<"\n";
    cout<<std::setfill(' ');
    cout<<std::setw(10)<<"index ";
    cout<<std::setw(15)<<"pthread_t ";
    cout<<std::setw(10)<<"work ";
    cout<<std::setw(15)<<"state"<<endl;
    cout<<endl;
    int i =0 ;
    while( i < g_thread_array.size()){
        g_thread_array[i]->print();
        i++;
    }
    cout<<endl;
}

// entry function for worker threads
void *job (void *p){
    // installing signal handler and getting the function argument
    install_signal_handler();
    thread_state * ptr = (thread_state *) p;

    // if worker
    if(ptr->work == w_producer){
        int counter = 0;
        while(counter < NUMBER_PRODUCED){
            while(ptr->state == s_stopped || g_count == g_buffer_size);
            g_buffer[g_buffer_in] = rand()%1000;
            g_buffer_in = (g_buffer_in + 1 ) %g_buffer_size;
            g_count++;
            usleep(500000);
            counter++;
        }
        ptr->state = s_complete;
        thread_state::producer_thread_count-- ;
    }
    else if(ptr->work == w_consumer){
        while((thread_state::producer_thread_count > 0 || g_count > 0)&&(thread_state::consumer_thread_dead == false)){
            while(g_count == 0 || ptr->state == s_stopped){
                    if(thread_state::consumer_thread_dead == true){
                    ptr->state = s_complete;
                    thread_state::consumer_thread_dead = true;
                    return NULL;
                }
            }
            int out = g_buffer[g_buffer_out];
            g_buffer_out = (g_buffer_out + 1 )% g_buffer_size;
            g_count--;
            usleep(500000);
        }
        ptr->state = s_complete;
        thread_state::consumer_thread_dead = true;
    }
    return NULL;
}

void *schedulerJob(void *){
    std::queue<thread_state *> rr_queue;
    int i = 0 ;
    while(i < g_thread_array.size()){
        rr_queue.push(g_thread_array[i]);
        i++;
    }
    thread_state *current_thread, *next_thread;
    current_thread = rr_queue.front(); rr_queue.pop();
    pthread_kill(current_thread->thread_id, SIGUSR2);

    // looping until the quit is empty and the death of a consumer thread has not occured
    // (consumer thread will die only if the producers are all terminated and the buffer is empty)
    // (making it pointless to iterate through all the C threads)
    while(rr_queue.size() > 0 && thread_state::consumer_thread_dead == false){
        cout<<endl<<endl;
        printThreads(0);
        next_thread = rr_queue.front(); rr_queue.pop();
        usleep(500000);
        pthread_kill(current_thread->thread_id, SIGUSR1);
        // printf("switching from %d to %d\n", current_thread->index, next_thread->index);
        pthread_kill(next_thread->thread_id, SIGUSR2);

        // checking the state of current thread and appending it back to rr_queue only if it is not complete
        if(current_thread->state != s_complete){
            rr_queue.push(current_thread);
        }
        current_thread = next_thread;
    }
    return NULL;
}


void *reporterJob(void *args){
    std::string initial_string, final_string;
    thread_state t1 , t2 ;
    int i;
    std::vector<thread_state> prev_state;
    for(int i=0 ; i < g_thread_array.size() ; i++){
        prev_state.push_back(*g_thread_array[i]);
    }
    scheduler_initializes = true;

    while(!program_reaching_termination){
        i=0 ;
        while(i < g_thread_array.size()){
            t1 = prev_state[i];
            t2 = *g_thread_array[i];
            // checking state change
            if(t1.state != t2.state){
                printf("thread index = %d ", t1.index);
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
