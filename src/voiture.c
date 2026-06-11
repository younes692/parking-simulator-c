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
    int strat_local;
    char details[64];
    struct timespec debut, fin;
    double temps_attente;

    ecrire_log(voiture_id, "ARRIVEE", "");
    clock_gettime(CLOCK_MONOTONIC, &debut);

    pthread_mutex_lock(&mutex_compteurs);
    strat_local = strategie;
    nb_en_attente++;
    pthread_mutex_unlock(&mutex_compteurs);

    if (strat_local == 0) {
        ecrire_log(voiture_id, "ATTENTE", "sem");
        sem_wait(&places_dispo);

        pthread_mutex_lock(&mutex_compteurs);
        nb_en_attente--;
        pthread_mutex_unlock(&mutex_compteurs);

        clock_gettime(CLOCK_MONOTONIC, &fin);
        temps_attente = (fin.tv_sec - debut.tv_sec)
                      + (fin.tv_nsec - debut.tv_nsec) / 1e9;
        enregistrer_attente(strat_local, temps_attente);

        pthread_mutex_lock(&mutex_affichage);
        for (i = 0; i < nb_places; i++) {
            if (places[i] == 0) {
                places[i] = 1;
                ma_place = i;
                nb_places_occupees++;
                break;
            }
        }
        pthread_mutex_unlock(&mutex_affichage);

    } else {
        ecrire_log(voiture_id, "ATTENTE", "busy");

        while (ma_place == -1) {
            pthread_mutex_lock(&mutex_affichage);
            for (i = 0; i < nb_places; i++) {
                if (places[i] == 0) {
                    places[i] = 1;
                    ma_place = i;
                    nb_places_occupees++;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex_affichage);

            if (ma_place == -1)
                sleep(3);
        }

        pthread_mutex_lock(&mutex_compteurs);
        nb_en_attente--;
        pthread_mutex_unlock(&mutex_compteurs);

        clock_gettime(CLOCK_MONOTONIC, &fin);
        temps_attente = (fin.tv_sec - debut.tv_sec)
                      + (fin.tv_nsec - debut.tv_nsec) / 1e9;
        enregistrer_attente(strat_local, temps_attente);
    }

    sprintf(details, "place %d", ma_place);
    ecrire_log(voiture_id, "GAREE", details);

    duree_parking = 5;
    sleep(duree_parking);

    pthread_mutex_lock(&mutex_affichage);
    places[ma_place] = 0;
    nb_places_occupees--;
    pthread_mutex_unlock(&mutex_affichage);

    ecrire_log(voiture_id, "DEPART", "");

    if (strat_local == 0)
        sem_post(&places_dispo);

    return NULL;
}
