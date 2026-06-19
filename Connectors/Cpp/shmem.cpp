#include "shmem.hpp"

std::atomic<bool> run(true);

void signal_handler(int signal_num) {
    if (signal_num == SIGINT){
        run = false;
    }
}

static void initialize(){

    sem_t *semu;
    sem_t *semp;
    sem_t *msg_sem;

    int shmidp = shmget(keyp, 2 * sizeof(DATA), 0600|IPC_CREAT);
    if (shmidp == -1) {
        std::cout << "Failed to create shared memory for publish points." << std::endl;
        return;
    }
    int shmidu = shmget(keyu, 2 * sizeof(DATA), 0600|IPC_CREAT);
    if (shmidu == -1) {
        std::cout << "Failed to create shared memory for update points." << std::endl;
        return;
    }
    DATA *publishPointsShmAddress = (DATA *) shmat(shmidp,NULL,0);
    DATA *updatePointsShmAddress = (DATA *) shmat(shmidu,NULL,0); 

    std::snprintf(publishPointsShmAddress[0].Name,128,"Output_Value_1");
    std::snprintf(publishPointsShmAddress[1].Name,128,"Output_Value_2");

    std::snprintf(updatePointsShmAddress[0].Name,128,"Input_Value_1");
    std::snprintf(updatePointsShmAddress[1].Name,128,"Input_Value_2");
    updatePointsShmAddress[0].Value = -100000000000000.0;
    updatePointsShmAddress[1].Value = -100000000000000.0;
    updatePointsShmAddress[0].Time = 0.0;
    updatePointsShmAddress[1].Time = 0.0;
    std::snprintf(updatePointsShmAddress[0].Type,50,"DOUBLE");
    std::snprintf(updatePointsShmAddress[1].Type,50,"DOUBLE");          

    shmdt(publishPointsShmAddress);
    shmdt(updatePointsShmAddress);

    /* Send number of inputs and outputs to data broker */    
    msg_sem = sem_open("/msg", O_CREAT, 0644, 0);
    if (msg_sem == SEM_FAILED) {
        std::cout << "Failed to open semaphore for init messaging." << std::endl;
        return;
    }

    int shmdb = shmget(msg_key, sizeof(MSG_DATA), 0600|IPC_CREAT);
    if (shmdb == -1) {
        std::cout << "Failed to create shared memory for init messaging." << std::endl;
        return;
    }

    MSG_DATA *MSG_DB = (MSG_DATA *) shmat(shmdb,NULL,0); 
    if (MSG_DB == (void *)-1) {
        std::cout << "Failed to attach shared memory for init messaging." << std::endl;
        return;
    }
    MSG_DB->UP = 2;
    MSG_DB->PUB = 2;
    MSG_DB->TimeStep = 0.1;
    
    shmdt(MSG_DB);
    sem_post(msg_sem);
}

static void process_update() {

    sem_t *semu;
    sem_t *semp;

    semu = sem_open(UPDATE_POINTS_SHM_SEM, 0);
    if (semu == SEM_FAILED) {
        std::cout << "Failed to open semaphore for update points." << std::endl;
        return;
    }
    semp = sem_open(PUBLISH_POINTS_SHM_SEM, 0);
    if (semp == SEM_FAILED) {
        std::cout<< "Failed to open semaphore for publish points." << std::endl;
        return;
    }
    
    /* set up shared memory */
    
    int shmidu = shmget(keyu, 2 * sizeof(DATA), 0600|IPC_CREAT);
    if (shmidu == -1) {
    std::cout << "Failed to create shared memory for update points." << std::endl;
    return;
    }
    DATA *updatePointsShmAddress = (DATA *) shmat(shmidu,NULL,0); 
    if (updatePointsShmAddress == (void *)-1) {
    std::cout << "Failed to attach shared memory for update points." << std::endl;
    return;
    }
    
    int shmidp = shmget(keyp, 2 * sizeof(DATA), 0600|IPC_CREAT);
    if (shmidp == -1) {
    std::cout << "Failed to create shared memory for publish points." << std::endl;
    return;
    }
    DATA *publishPointsShmAddress = (DATA *) shmat(shmidp,NULL,0); 
    if (publishPointsShmAddress == (void *)-1) {
    std::cout << "Failed to attach shared memory for publish points." << std::endl;
    return;
    }  
       
    sem_wait(semu);
        
    double val1 = updatePointsShmAddress[0].Value * updatePointsShmAddress[1].Value;;
    double val2 = updatePointsShmAddress[0].Value * updatePointsShmAddress[0].Value;;

    publishPointsShmAddress[0].Value = val1;
    publishPointsShmAddress[1].Value = val2;
        
    shmdt(publishPointsShmAddress);
    shmdt(updatePointsShmAddress);
    
    sem_post(semp);
}

int terminate() {
    return 0;
}

int main(int argc, char** argv){

    std::signal(SIGINT, signal_handler);
    
    std::vector<std::string> args(argv + 1, argv + argc);
    for (const auto& arg: args){
        std::cout << "arg: " << arg << "\n"; 
    }   

    initialize();
    while(run){
        process_update();
    }

    return 0;


}