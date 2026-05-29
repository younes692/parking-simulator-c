#include "voiture.h"
#include "parking.h"
#include "logger.h"

// variables globales definies dans parking.c
extern int places[NB_PLACES];

void* voiture_thread(void* arg) {
    int voiture_id = *((int*)arg);
    int duree_parking;
    int ma_place = -1;
    int i;
    char details[64];

    pthread_mutex_lock(&mutex_affichage);
    printf("[voiture %02d] arrive au parking\n", voiture_id);
    printf("test %d\n", voiture_id);
    pthread_mutex_unlock(&mutex_affichage);

    ecrire_log(voiture_id, "ARRIVEE", "");

    // on attend qu'une place se libere
    ecrire_log(voiture_id, "ATTENTE", "");
    sem_wait(&places_dispo);

    // on cherche la premiere place libre et on la prend
    pthread_mutex_lock(&mutex_affichage);
    for (i = 0; i < NB_PLACES; i++) {
        if (places[i] == 0) {
            places[i] = 1;
            ma_place = i;
            break;
        }
    }
    printf("[voiture %02d] se gare a la place %d\n", voiture_id, ma_place);
    pthread_mutex_unlock(&mutex_affichage);

    sprintf(details, "place %d", ma_place);
    ecrire_log(voiture_id, "GAREE", details);

    // la voiture reste garee un moment
    duree_parking = rand() % DUREE_MAX + 1;
    sleep(duree_parking);

    // la voiture part et libere sa place
    pthread_mutex_lock(&mutex_affichage);
    places[ma_place] = 0;
    printf("[voiture %02d] quitte la place %d (etait la %d sec)\n", voiture_id, ma_place, duree_parking);
    pthread_mutex_unlock(&mutex_affichage);

    ecrire_log(voiture_id, "DEPART", "");
    sem_post(&places_dispo);

    return NULL;
}
