#include "voiture.h"
#include "parking.h"
#include "logger.h"
#include "stats.h"
#include <time.h>

extern int places[NB_PLACES];
extern int nb_en_attente;
extern int nb_places_occupees;
extern int strategie;

void* voiture_thread(void* arg) {
    int voiture_id = *((int*)arg);
    int duree_parking;
    int ma_place = -1;
    int i;
    char details[64];
    struct timespec debut, fin;
    double temps_attente;

    // les voitures arrivent pas toutes en meme temps
    sleep(rand() % 4 + 1);

    ecrire_log(voiture_id, "ARRIVEE", "");

    // on mesure combien de temps la voiture attend
    clock_gettime(CLOCK_MONOTONIC, &debut);
    nb_en_attente++;

    if (strategie == 0) {
        // mode semaphore : le thread est mis en veille par l OS
        // il ne consomme pas de CPU pendant l attente
        ecrire_log(voiture_id, "ATTENTE", "sem");
        sem_wait(&places_dispo);
    } else {
        // mode attente active : on tourne en boucle
        // ca marche mais ca consomme du CPU pour rien
        ecrire_log(voiture_id, "ATTENTE", "busy");
        while (nb_places_occupees >= NB_PLACES) {
            sleep(1);
        }
        // on prend quand meme le semaphore pour pas le desynchroniser
        sem_wait(&places_dispo);
    }

    nb_en_attente--;

    // calcul du temps d attente
    clock_gettime(CLOCK_MONOTONIC, &fin);
    temps_attente = (fin.tv_sec - debut.tv_sec)
                  + (fin.tv_nsec - debut.tv_nsec) / 1e9;
    enregistrer_attente(strategie, temps_attente);

    // on cherche la premiere place libre et on la prend
    pthread_mutex_lock(&mutex_affichage);
    for (i = 0; i < NB_PLACES; i++) {
        if (places[i] == 0) {
            places[i] = 1;
            ma_place = i;
            nb_places_occupees++;
            break;
        }
    }
    pthread_mutex_unlock(&mutex_affichage);

    sprintf(details, "place %d", ma_place);
    ecrire_log(voiture_id, "GAREE", details);

    // la voiture reste garee un moment
    duree_parking = rand() % DUREE_MAX + 1;
    sleep(duree_parking);

    // la voiture repart et libere sa place
    pthread_mutex_lock(&mutex_affichage);
    places[ma_place] = 0;
    nb_places_occupees--;
    pthread_mutex_unlock(&mutex_affichage);

    ecrire_log(voiture_id, "DEPART", "");
    sem_post(&places_dispo);

    return NULL;
}
