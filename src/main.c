#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

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
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t threads[NB_VOITURES];
    pthread_t t_affichage;
    int ids[NB_VOITURES];
    int i;

    srand((unsigned int)time(NULL));

    // lecture des arguments si fournis
    if (argc >= 3) {
        nb_places   = atoi(argv[1]);
        nb_voitures = atoi(argv[2]);
        if (nb_places < 1 || nb_places > NB_PLACES) {
            printf("nb_places invalide, max %d\n", NB_PLACES);
            return 1;
        }
        if (nb_voitures < 1 || nb_voitures > NB_VOITURES) {
            printf("nb_voitures invalide, max %d\n", NB_VOITURES);
            return 1;
        }
    }

    init_stats();
    init_affichage();

    // menu de selection : retourne 0, 1 ou 2
    strategie = afficher_menu();

    pthread_create(&t_affichage, NULL, thread_affichage, NULL);

    // run unique avec la strategie choisie
    init_parking();
    for (i = 0; i < nb_voitures; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, voiture_thread, &ids[i]);
    }
    for (i = 0; i < nb_voitures; i++) {
        pthread_join(threads[i], NULL);
    }
    destroy_parking();

    simulation_active = 0;
    pthread_join(t_affichage, NULL);

    end_affichage();
    afficher_stats();
    exporter_csv();

    return 0;
}
