#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "parking.h"
#include "voiture.h"
#include "affichage.h"

// flag pour dire au thread d'affichage d'arreter
int simulation_active = 1;

// thread qui rafraichit l'ecran en boucle
void* thread_affichage(void* arg) {
    while (simulation_active) {
        afficher_parking();
        usleep(100000);
        // gestion de la touche q pour quitter
        int ch = getch();
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

    init_parking();
    init_affichage();

    // on lance le thread d'affichage
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
    printf("simulation terminee\n");

    destroy_parking();
    return 0;
}
