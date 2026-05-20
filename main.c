/*
 * Projet : Simulation du Probleme de Parking
 * Auteur : Younes B.
 * Date   : 2024-02-01
 * Cours  : Systemes d'exploitation - L2 Info
 *
 * Simulation d'un parking avec threads POSIX
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "parking.h"

int main() {
    pthread_t threads[NB_VOITURES];
    int ids[NB_VOITURES];
    int i;

    printf("Simulation parking - debut\n");

    // on cree les threads
    for (i = 0; i < NB_VOITURES; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, voiture_thread, &ids[i]);
    }

    // on attend que tous finissent
    for (i = 0; i < NB_VOITURES; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Simulation terminee\n");
    return 0;
}
