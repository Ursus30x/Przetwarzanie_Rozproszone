#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define CLOCK_MONOTONIC 1 // idk why but it doesnt see it

typedef int pipe_t;

typedef struct{
    size_t squarePoints;
    size_t circlePoints;
    char padding[64];  // Prevents cache line conflicts

}pointsInfo;

size_t  thread_num,
        subproc_num,
        totalPoints,
        pointsForThread;



void init_points_info(pointsInfo *info){
    info->squarePoints = 0;
    info->circlePoints = 0;
}


double get_random_coord(unsigned int *seed){
    return (double)(rand_r(seed) % (totalPoints + 1)) / totalPoints;
}

void *compute_pi_num(void* pointsInfo_ptr){
#ifdef DEBUG
    printf("Thread %ld of subproces %d started\n",pthread_self(),getpid());
#endif
    pointsInfo *info = (pointsInfo*) pointsInfo_ptr;
    size_t localPointsForThread = pointsForThread;
    unsigned int seed = getpid() + pthread_self() + time(NULL);

    for(int i = 0;i<localPointsForThread;i++){
        double  x = get_random_coord(&seed),
                y = get_random_coord(&seed),
                dist = x*x + y*y;

        if(dist <= 1){
            info->circlePoints++;
        }

        info->squarePoints++;
    }  

#ifdef DEBUG
    printf("Thread %ld of subproces %d finished\n", pthread_self(), getpid());
    printf("cricles %d sqares %d\n\n", info->circlePoints, info->squarePoints);
#endif
    return NULL;
}

void execute_fork(pointsInfo *forkInfo){
#ifdef DEBUG
    printf("Fork pid: %d starte\n",getpid() );
#endif
    pthread_t threads[thread_num];
    pointsInfo threadInfos[thread_num];


    for(int i = 0;i<thread_num;i++){
        init_points_info(&threadInfos[i]);
        pthread_create(&threads[i],NULL,compute_pi_num,&threadInfos[i]);
    }
    
    for(int i = 0;i<thread_num;i++){
        pthread_join(threads[i],NULL);
        forkInfo->circlePoints += threadInfos[i].circlePoints;
        forkInfo->squarePoints += threadInfos[i].squarePoints;
    }
#ifdef DEBUG
    printf("Fork pid: %d finished\n",getpid());
    printf("cricles %d sqares %d\n\n", forkInfo->circlePoints, forkInfo->squarePoints);
#endif
}

void read_args(int argc, char **argv){

    if(argc != 4){
        perror("Wrong number of arguments!");
        exit(1);
    }

    totalPoints = atoi(argv[1]);

    subproc_num = atoi(argv[2]);

    thread_num = atoi(argv[3]);

    pointsForThread = totalPoints/(subproc_num * thread_num);
}

long get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void printProcessStatus(){
#ifdef DEBUG
    const bool debug = true;
#else
    const bool debug = false;
#endif
    printf("\n\n");
    printf("*****PROC STATUS********\n\n");
    printf(" - totalPoints: %ld     \n",totalPoints);
    printf(" - subproc_num: %ld     \n",subproc_num);
    printf(" - threads_num: %ld     \n\n",thread_num);
    printf("   DEBUG:  %d           \n\n",debug);
}

int main(int argc, char **argv){
    read_args(argc,argv);
    printProcessStatus();

    long timeStart = get_time_ms();
    pid_t pid;
    pid_t child_pipes[subproc_num]; 
    pipe_t pipes[subproc_num][2];
    pointsInfo info;

    init_points_info(&info);


    for(int i = 0;i<subproc_num;i++){
        pipe(pipes[i]);
        pid = fork();
        child_pipes[i] = pid;

        if(pid == -1){
            perror("Cannot fork process");
            exit(1);
        }
        else if(pid == 0){
            execute_fork(&info);

            close(pipes[i][PIPE_READ]);
            write(pipes[i][PIPE_WRITE],&info,sizeof(pointsInfo));
            close(pipes[i][PIPE_WRITE]);

            return 0;
        }
    }

    for(int i = 0;i<subproc_num;i++){
        pointsInfo temp;
        waitpid(child_pipes[i],NULL,0);

        close(pipes[i][PIPE_WRITE]);
        read(pipes[i][PIPE_READ],&temp,sizeof(pointsInfo));
        close(pipes[i][PIPE_READ]);

        info.circlePoints += temp.circlePoints;
        info.squarePoints += temp.squarePoints;
    }

#ifdef DEBUG
    printf("cricles %d sqares %d for main proc\n\n", info.circlePoints, info.squarePoints);
#endif

    double pi = (double)(4 * info.circlePoints) / (double)(info.squarePoints);

    long timeEnd = get_time_ms();
    printf("Elapsed time: %ld ms\n",timeEnd - timeStart);

    printf("pi aprox: %lf\n",pi);


    return 0;
}