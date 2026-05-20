#include "voiture.h"

// chaque voiture est un thread qui arrive, se gare et repart
void* voiture_thread(void* arg) {
    int voiture_id = *((int*)arg);
    int duree_parking;

    printf("[voiture %02d] arrive au parking\n", voiture_id);

    // la voiture occupe une place un moment
    duree_parking = rand() % DUREE_MAX + 1;
    sleep(duree_parking);

    printf("[voiture %02d] quitte le parking (etait la %d sec)\n", voiture_id, duree_parking);

    return NULL;
}
