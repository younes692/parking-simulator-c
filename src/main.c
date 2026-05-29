#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "parking.h"
#include "voiture.h"
#include "affichage.h"
#include "stats.h"

extern int strategie;

int simulation_active = 1;

void* thread_affichage(void* arg) {
    int ch;
    while (simulation_active) {
        afficher_parking();
        usleep(100000);
        ch = getch();
        if (ch == 'q') {
            simulation_active = 0;
        } else if (ch == 's') {
            // changer de strategie en cours de simulation
            strategie = (strategie == 0) ? 1 : 0;
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t threads[NB_VOITURES];
    pthread_t t_affichage;
    int ids[NB_VOITURES];
    int i;

    init_parking();
    init_stats();
    init_affichage();

    pthread_create(&t_affichage, NULL, thread_affichage, NULL);

    // on cree les threads voitures
    for (i = 0; i < NB_VOITURES; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, voiture_thread, &ids[i]);
    }

    // on attend que tout le monde ait fini
    for (i = 0; i < NB_VOITURES; i++) {
        pthread_join(threads[i], NULL);
    }

    simulation_active = 0;
    pthread_join(t_affichage, NULL);

    end_affichage();

    // afficher et exporter les stats
    afficher_stats();
    exporter_csv();

    destroy_parking();
    return 0;
}
