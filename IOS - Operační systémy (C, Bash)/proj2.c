/**************************************************
 * Ukazkove kody do predmetu IOS na FIT VUT.
 * Prace se semafory a sdilenou pameti.
 * Kody jsou poskytnuty tak jak jsou.
 * (C) Radek Koci, 2016
***************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>   

#define semNAME "/xsztef02-ios"


#define semloading "/xsztef02-ios-Loading"
#define semcanLoad "/xsztef02-ios-CanLoad"
#define semisFull "/xsztef02-ios-IsFull"
#define semcanUnload "/xsztef02-ios-CanUnload"
#define semisEmpty "/xsztef02-ios-IsEmpty"

#define SHMItersNAME "/xsztef02-ios-shm-iters"
#define SHMItersSize sizeof(int)

#define SHMCapacityNAME "/xsztef02-ios-shm-capacity"
#define SHMCapacitySize sizeof(int)

int P=9, C=3, PT=2, RT=10;
FILE *f;
pid_t carID;
pid_t p_makerID;
pid_t *passengers;

/**
 * SEM
 * - load - povoleni nastupu 
 * - run vozik na trati - zakaz nastupu 
 * - full
 *  
 * - nastupuju
 */


/**
 *  SHM
 *  - iterace
 *  - pocet lidi ve voziku
 *  - 
 */



void car() {
    int id=1;

    srand(time(NULL));

    sem_t *canLoad;
    sem_t *isFull;
    sem_t *canUnload;
    sem_t *isEmpy;
    sem_t *end;

    end = sem_open(semNAME, O_RDWR);
    canLoad = sem_open(semcanLoad, O_RDWR);
    isFull = sem_open(semisFull, O_RDWR);
    canUnload = sem_open(semcanUnload, O_RDWR);
    isEmpy = sem_open(semisEmpty, O_RDWR);
    if(end == SEM_FAILED || 
        canLoad == SEM_FAILED ||
        isFull == SEM_FAILED ||
        canUnload == SEM_FAILED ||
        isEmpy == SEM_FAILED ){
        fprintf(stderr,"semaphore error\n");
        sem_close(canLoad);
        sem_close(isFull);
        sem_close(canUnload);
        sem_close(isEmpy);
        fclose(f);
        exit(2);
    }
    
    int shmItersID;
    int *shmIters;

    // iterace
    shmItersID = shm_open(SHMItersNAME, O_RDWR, S_IRUSR | S_IWUSR);
    shmIters = (int*)mmap(NULL, SHMItersSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmItersID, 0);
    close(shmItersID);

    fprintf(f, "%d\t: C %d\t: started\n",(*shmIters)++,id);
    fflush(f);

    for(int i=0;i<P/C;i++){
        //load - povoleni nastupovani, sem canLoad
        fprintf(f, "%d\t: C %d\t: load\n",(*shmIters)++,id);
        fflush(f);
        sem_post(canLoad);
        //wait to load - ceka na naplneni sem isFull
        sem_wait(isFull);
        //run + sleep - zakaz nastupovani(provadi passenger)
        fprintf(f, "%d\t: C %d\t: run\n",(*shmIters)++,id);
        fflush(f);
        if(RT)
            usleep(rand()%RT);
        //unload - povoleni vystupu
        fprintf(f, "%d\t: C %d\t: unload\n",(*shmIters)++,id);
        fflush(f);
        sem_post(canUnload);
        //wait to uload - cekani na vystup 
        sem_wait(isEmpy);
    }

    sem_post(end);
    fprintf(f, "%d\t: C %d\t: finished\n",(*shmIters)++,id);
    fflush(f);

    munmap(shmIters, SHMItersSize);

    sem_close(canLoad);
    sem_close(isFull);
    sem_close(canUnload);
    sem_close(isEmpy);
}


void passenger(int id){
    sem_t *loading;
    sem_t *canLoad;
    sem_t *isFull;
    sem_t *canUnload;
    sem_t *isEmpy;
    sem_t *end;

    end = sem_open(semNAME, O_RDWR);
    loading = sem_open(semloading, O_RDWR);
    canLoad = sem_open(semcanLoad, O_RDWR);
    isFull = sem_open(semisFull, O_RDWR);
    canUnload = sem_open(semcanUnload, O_RDWR);
    isEmpy = sem_open(semisEmpty, O_RDWR);

    if(end == SEM_FAILED || 
        loading == SEM_FAILED ||
        canLoad == SEM_FAILED ||
        isFull == SEM_FAILED ||
        canUnload == SEM_FAILED ||
        isEmpy == SEM_FAILED ){
        fprintf(stderr,"semaphore error\n");
        sem_close(loading);
        sem_close(canLoad);
        sem_close(isFull);
        sem_close(canUnload);
        sem_close(isEmpy);
        fclose(f);
        exit(2);
    }

 // iterace
    int shmItersID;
    int *shmIters;
    shmItersID = shm_open(SHMItersNAME, O_RDWR, S_IRUSR | S_IWUSR);
    shmIters = (int*)mmap(NULL, SHMItersSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmItersID, 0);
    close(shmItersID);

//kapacita
    int shmCapacityID;
    int *shmCapacity;
    shmCapacityID = shm_open(SHMCapacityNAME, O_RDWR, S_IRUSR | S_IWUSR);
    shmCapacity = (int*)mmap(NULL, SHMCapacitySize, PROT_READ | PROT_WRITE, MAP_SHARED, shmCapacityID, 0);
    close(shmCapacityID);

//START

    fprintf(f, "%d\t: P %d\t: started\n",(*shmIters)++,id);
    fflush(f);
    //wait to load - cekani na vozik sem canLoad
    sem_wait(canLoad);
        //load - nastupovani | iterace nastupovani | semafor??? 
    sem_wait(loading);
        fprintf(f, "%d\t: P %d\t: board\n",(*shmIters)++,id);
        fflush(f);
        //pokud je posledni bloku nastup - isFull
        if((*shmCapacity)<C){
            fprintf(f, "%d\t: P %d\t: board order %d\n",(*shmIters)++,id,*shmCapacity);
            fflush(f);
            (*shmCapacity)++;
            sem_post(canLoad);
        }else{
            fprintf(f, "%d\t: P %d\t: board last\n",(*shmIters)++,id);
            fflush(f);
            sem_post(isFull);
        }   
    sem_post(loading);
        //unload - cekani na unload | iterace vystoupenych
    sem_wait(canUnload);
        //cekani na unload vsech
    sem_wait(loading);
        fprintf(f, "%d\t: P %d\t: unboard\n",(*shmIters)++,id);
        fflush(f);
        if((*shmCapacity)>1){
            fprintf(f, "%d\t: P %d\t: unboard order %d\n",(*shmIters)++,id,C-(*shmCapacity)+1);
            fflush(f);
            (*shmCapacity)--;
            sem_post(canUnload);
        }else{
            fprintf(f, "%d\t: P %d\t: unboard last\n",(*shmIters)++,id);
            fflush(f);
            sem_post(isEmpy);
        } 
    sem_post(loading);  

    sem_wait(end);
    fprintf(f, "%d\t: P %d\t: finished\n",(*shmIters)++,id);
    fflush(f);
    sem_post(end);
//EDN
    munmap(shmIters, SHMItersSize);
    munmap(shmCapacity, SHMCapacitySize);
    sem_close(loading);
    sem_close(canLoad);
    sem_close(isFull);
    sem_close(canUnload);
    sem_close(isEmpy);
}

void p_maker() {
    //int pid;
    time_t t;
    srand((unsigned) time(&t));

    for(int people = 0;people < P; people++){
        //fork
        //child -> passenger, exit
        //parent - waitig, ulozeni pid
        
        if ((passengers[people] = fork()) < 0) {

            //kill all child
            //kill car
            
            for(int i = 0; i<people;i++){
                kill(passengers[i],SIGTERM);
            }

            kill(carID,SIGTERM);
            fprintf(stderr,"fork error\n");
            break;
        }

        if (passengers[people] == 0) { // child
            passenger(passengers[people]+1);
            exit(0);
        } else{ //parent
            if(PT){
                usleep(rand()%PT);
            }
        }
    }



    // wait to end process
    wait(NULL);
    exit(2);
}


int main(int argc, char *argv[]){
    int pid;
    sem_t *loading;
    sem_t *canLoad;
    sem_t *isFull;
    sem_t *canUnload;
    sem_t *isEmpy;
    sem_t *end;




// parametry
    char *err;
    if (argc == 5){
        P = (int) strtol(argv[1], &err, 10);
        C  = (int) strtol(argv[2], &err, 10);
        PT = (int) strtol(argv[3], &err, 10);
        RT = (int) strtol(argv[4], &err, 10);
        if(*err != 0 ||
            (P<=0 || P%C != 0 || P<=C) ||
            (C<=0) ||
            (PT<0 || PT > 5000) ||
            (RT<0 || RT > 5000)){
            fprintf(stderr,"argv error\n");
            return 1;
        }
    }
    else{
        fprintf(stderr,"argv error\n");
        return 1;
    }

    if((f=fopen("proj2.out","w")) == NULL){
        fprintf(stderr,"file error\n");
        return 2;   
    }

    passengers=malloc(sizeof(pid_t)*P);
    if(passengers == NULL){
        fclose(f);
        fprintf(stderr,"memory error\n");
        return 2; 
    }

//sem
    end = sem_open(semNAME, O_CREAT, 0666, 0);
    sem_close(end);
    loading = sem_open(semloading, O_CREAT, 0666, 1); //neblokovany
    sem_close(loading);
    canLoad = sem_open(semcanLoad, O_CREAT, 0666, 0);//blokovany
    sem_close(canLoad);
    isFull = sem_open(semisFull, O_CREAT, 0666, 0);//blokovany
    sem_close(isFull);
    canUnload = sem_open(semcanUnload, O_CREAT, 0666, 0);//blokovany
    sem_close(canUnload);
    isEmpy = sem_open(semisEmpty, O_CREAT, 0666, 0); //blokovany
    sem_close(isEmpy);

    if(end == SEM_FAILED || 
        loading == SEM_FAILED ||
        canLoad == SEM_FAILED ||
        isFull == SEM_FAILED ||
        canUnload == SEM_FAILED ||
        isEmpy == SEM_FAILED ){
        fprintf(stderr,"semaphore error\n");
        sem_close(loading);
        sem_close(canLoad);
        sem_close(isFull);
        sem_close(canUnload);
        sem_close(isEmpy);
        fclose(f);
        return 2;
    }

//iterace
    int *shmIters;
    int shmItersID;
    shmItersID = shm_open(SHMItersNAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if(shmItersID == -1){
        fprintf(stderr,"shared memory error\n");
        sem_unlink(semNAME);
        sem_unlink(semloading);
        sem_unlink(semisEmpty);
        sem_unlink(semcanUnload);
        sem_unlink(semcanLoad);
        sem_unlink(semisFull);
        fclose(f);
        return 2;
    }
    ftruncate(shmItersID, SHMItersSize);
    shmIters = (int*)mmap(NULL, SHMItersSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmItersID, 0);
    if(*shmIters == -1){
        fprintf(stderr,"shared memory error\n");
        sem_unlink(semNAME);
        sem_unlink(semloading);
        sem_unlink(semisEmpty);
        sem_unlink(semcanUnload);
        sem_unlink(semcanLoad);
        sem_unlink(semisFull);
        shm_unlink(SHMItersNAME);
        fclose(f);
        return 2;
    }
    close(shmItersID);
    *shmIters = 1;
    if(munmap(shmIters, SHMItersSize) == -1){
        fprintf(stderr,"shared memory error\n");
        sem_unlink(semNAME);
        sem_unlink(semloading);
        sem_unlink(semisEmpty);
        sem_unlink(semcanUnload);
        sem_unlink(semcanLoad);
        sem_unlink(semisFull);
        shm_unlink(SHMItersNAME);
        shm_unlink(SHMCapacityNAME);
        fclose(f);
        return 2;
    }

//kapacita
    int *shmCapacity;
    int shmCapacityID;
    shmCapacityID = shm_open(SHMCapacityNAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if(shmCapacityID == -1){
        fprintf(stderr,"shared memory error\n");
        sem_unlink(semNAME);
        sem_unlink(semloading);
        sem_unlink(semisEmpty);
        sem_unlink(semcanUnload);
        sem_unlink(semcanLoad);
        sem_unlink(semisFull);
        shm_unlink(SHMItersNAME);
        shm_unlink(SHMCapacityNAME);
        fclose(f);
        return 2;
    }
    ftruncate(shmCapacityID, SHMCapacitySize);
    shmCapacity = (int*)mmap(NULL, SHMCapacitySize, PROT_READ | PROT_WRITE, MAP_SHARED, shmCapacityID, 0);
    if(*shmCapacity == -1){
        fprintf(stderr,"shared memory error\n");
        sem_unlink(semNAME);
        sem_unlink(semloading);
        sem_unlink(semisEmpty);
        sem_unlink(semcanUnload);
        sem_unlink(semcanLoad);
        sem_unlink(semisFull);
        shm_unlink(SHMItersNAME);
        shm_unlink(SHMCapacityNAME);
        fclose(f);
        return 2;
    }
    close(shmCapacityID);
    *shmCapacity = 1;
    if(munmap(shmCapacity, SHMCapacitySize) == -1){
        fprintf(stderr,"shared memory error\n");
        sem_unlink(semNAME);
        sem_unlink(semloading);
        sem_unlink(semisEmpty);
        sem_unlink(semcanUnload);
        sem_unlink(semcanLoad);
        sem_unlink(semisFull);
        shm_unlink(SHMItersNAME);
        shm_unlink(SHMCapacityNAME);
        fclose(f);
        return 2;
    }


//program
    setbuf(f,NULL);
    setbuf(stderr,NULL);


//fork
    if ((pid = fork()) < 0) {
        perror("fork"); 
        sem_unlink(semNAME);
        sem_unlink(semloading);
        sem_unlink(semisEmpty);
        sem_unlink(semcanUnload);
        sem_unlink(semcanLoad);
        sem_unlink(semisFull);

        shm_unlink(SHMItersNAME);
        shm_unlink(SHMCapacityNAME);
        exit(2);
    }

    if (pid == 0) { // child
        car();
        exit(0);
    } else { // parent.
        carID = pid;
        //--
        if ((pid = fork()) < 0) {
            perror("fork"); 
            sem_unlink(semNAME);
            sem_unlink(semloading);
            sem_unlink(semisEmpty);
            sem_unlink(semcanUnload);
            sem_unlink(semcanLoad);
            sem_unlink(semisFull);

            shm_unlink(SHMItersNAME);
            shm_unlink(SHMCapacityNAME);
            exit(2);
        }
        //--
        if (pid == 0) { // child
            p_maker();
            exit(0);
        } else { // parent
            p_makerID = pid;
        }
    }


// pockame az vsichni skonci
    waitpid(carID, NULL, 0);
    waitpid(p_makerID, NULL, 0);

// zrusime zdroje
    sem_unlink(semNAME);
    sem_unlink(semloading);
    sem_unlink(semisEmpty);
    sem_unlink(semcanUnload);
    sem_unlink(semcanLoad);
    sem_unlink(semisFull);

    shm_unlink(SHMItersNAME);
    shm_unlink(SHMCapacityNAME);
    //printf("%d %d",carID,p_makerID);
    fclose(f);


    return 0;
}
