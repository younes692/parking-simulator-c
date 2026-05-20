#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "parking.h"

// fonction executee par chaque thread voiture
void* voiture_thread(void* arg) {
    int voiture_id = *((int*)arg);
    int duree;

    printf("Voiture %d arrive au parking\n", voiture_id);

    // la voiture se gare et reste un moment
    duree = rand() % 3 + 1;
    sleep(duree);

    printf("Voiture %d quitte le parking\n", voiture_id);

    return NULL;
}
