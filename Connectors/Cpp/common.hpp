#include <iostream>
#include <sstream>
#include <string>
#include <csignal>
#include <atomic>
#include <vector>

#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>

#define MSG_SIZE_MULT 256
#define PUBLISH_POINTS_SHM_SEM "/pp_sem"
#define UPDATE_POINTS_SHM_SEM "/up_sem"
#define STOP_SEM "/stop"

#define MAX_IO 1000

key_t keyp = 10618;
key_t keyu = 10619;
key_t msg_key = 10620;

typedef struct {
    char Name[128];
    char Type[50];
    double Value;
    double Time;
} DATA;
        
typedef struct {
    int PUB;
    int UP;
    double TimeStep;
} MSG_DATA;

DATA UP_DATA[MAX_IO]; // Upper limit of IO = 1000
DATA PUB_DATA[MAX_IO];