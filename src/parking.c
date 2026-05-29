#include "parking.h"

// variables globales definies ici
sem_t places_dispo;
pthread_mutex_t mutex_affichage;

// tableau pour savoir quelles places sont libres (0=libre 1=occupee)
int places[NB_PLACES];

void init_parking() {
    int i;
    // on initialise le semaphore avec le nombre de places
    sem_init(&places_dispo, 0, NB_PLACES);
    pthread_mutex_init(&mutex_affichage, NULL);
    // toutes les places sont libres au debut
    for (i = 0; i < NB_PLACES; i++) {
        places[i] = 0;
    }
}

void destroy_parking() {
    sem_destroy(&places_dispo);
    pthread_mutex_destroy(&mutex_affichage);
}
