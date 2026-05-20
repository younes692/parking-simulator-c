/*
 * TP Parking - Simulation de parking avec threads
 * Auteur  : Younes B.
 * Date    : 2024-02-05
 * Cours   : Systemes d'exploitation - L2 Info
 *
 * On simule un parking avec N places et M voitures
 * chaque voiture est un thread qui essaye d'entrer dans le parking
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "parking.h"
#include "voiture.h"

int main(int argc, char* argv[]) {
    pthread_t threads[NB_VOITURES];
    int ids[NB_VOITURES];
    int i;

    printf("debut simulation parking\n");

    // on cree les threads voitures
    for (i = 0; i < NB_VOITURES; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, voiture_thread, &ids[i]);
    }

    // on attend que tout le monde ait fini
    for (i = 0; i < NB_VOITURES; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("simulation terminee\n");
    return 0;
}
