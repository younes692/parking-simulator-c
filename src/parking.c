#include "parking.h"

sem_t places_dispo;
pthread_mutex_t mutex_affichage;
pthread_mutex_t mutex_compteurs;
pthread_cond_t cond_place_dispo;

int places[NB_PLACES];
int nb_en_attente    = 0;
int nb_places_occupees = 0;
// 0 = semaphore, 1 = attente active, 2 = variable condition
int strategie        = 0;

// valeurs runtime (peuvent etre changees par argv)
int nb_places   = NB_PLACES;
int nb_voitures = NB_VOITURES;

void init_parking() {
    int i;
    nb_places_occupees = 0;
    nb_en_attente      = 0;
    sem_init(&places_dispo, 0, nb_places);
    pthread_mutex_init(&mutex_affichage, NULL);
    pthread_mutex_init(&mutex_compteurs, NULL);
    pthread_cond_init(&cond_place_dispo, NULL);
    for (i = 0; i < nb_places; i++)
        places[i] = 0;
}

void destroy_parking() {
    sem_destroy(&places_dispo);
    pthread_mutex_destroy(&mutex_affichage);
    pthread_mutex_destroy(&mutex_compteurs);
    pthread_cond_destroy(&cond_place_dispo);
}
