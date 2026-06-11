#include "voiture.h"
#include "parking.h"
#include "logger.h"
#include "stats.h"
#include <time.h>
#include <errno.h>

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
    int tentatives;
    char details[64];
    struct timespec debut, fin, ts_timeout, t_garee, t_depart;
    double temps_attente;
    double duree_garee;

    ecrire_log(voiture_id, "ARRIVEE", "");
    clock_gettime(CLOCK_MONOTONIC, &debut);

    pthread_mutex_lock(&mutex_compteurs);
    strat_local = strategie;
    nb_en_attente++;
    pthread_mutex_unlock(&mutex_compteurs);

    if (strat_local == 0) {
        ecrire_log(voiture_id, "ATTENTE", "sem");

        /* si le semaphore n est jamais poste (interblocage), le thread
         * sort proprement apres 30 sec au lieu de bloquer indefiniment */
        clock_gettime(CLOCK_REALTIME, &ts_timeout);
        ts_timeout.tv_sec += 30;
        if (sem_timedwait(&places_dispo, &ts_timeout) != 0) {
            pthread_mutex_lock(&mutex_compteurs);
            nb_en_attente--;
            pthread_mutex_unlock(&mutex_compteurs);
            ecrire_log(voiture_id, "TIMEOUT", "");
            return NULL;
        }

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
                usleep(150000);
                clock_gettime(CLOCK_MONOTONIC, &t_garee);
                break;
            }
        }
        pthread_mutex_unlock(&mutex_affichage);

    } else if (strat_local == 1) {
        ecrire_log(voiture_id, "ATTENTE", "busy");

        /* si aucune place ne se libere apres 60 sec (20 x 3s), le thread
         * sort proprement pour eviter une boucle infinie */
        tentatives = 0;
        while (ma_place == -1) {
            pthread_mutex_lock(&mutex_affichage);
            for (i = 0; i < nb_places; i++) {
                if (places[i] == 0) {
                    places[i] = 1;
                    ma_place = i;
                    nb_places_occupees++;
                    clock_gettime(CLOCK_MONOTONIC, &fin);
                    usleep(150000);
                    clock_gettime(CLOCK_MONOTONIC, &t_garee);
                    break;
                }
            }
            pthread_mutex_unlock(&mutex_affichage);

            if (ma_place == -1) {
                tentatives++;
                if (tentatives >= 20) {
                    pthread_mutex_lock(&mutex_compteurs);
                    nb_en_attente--;
                    pthread_mutex_unlock(&mutex_compteurs);
                    ecrire_log(voiture_id, "TIMEOUT", "");
                    return NULL;
                }
                sleep(3);
            }
        }

        pthread_mutex_lock(&mutex_compteurs);
        nb_en_attente--;
        pthread_mutex_unlock(&mutex_compteurs);

        temps_attente = (fin.tv_sec - debut.tv_sec)
                      + (fin.tv_nsec - debut.tv_nsec) / 1e9;
        enregistrer_attente(strat_local, temps_attente);

    } else {
        ecrire_log(voiture_id, "ATTENTE", "cond");

        pthread_mutex_lock(&mutex_affichage);
        while (nb_places_occupees >= nb_places)
            pthread_cond_wait(&cond_place_dispo, &mutex_affichage);
        for (i = 0; i < nb_places; i++) {
            if (places[i] == 0) {
                places[i] = 1;
                ma_place = i;
                nb_places_occupees++;
                clock_gettime(CLOCK_MONOTONIC, &fin);
                usleep(150000);
                clock_gettime(CLOCK_MONOTONIC, &t_garee);
                break;
            }
        }
        pthread_mutex_unlock(&mutex_affichage);

        pthread_mutex_lock(&mutex_compteurs);
        nb_en_attente--;
        pthread_mutex_unlock(&mutex_compteurs);

        temps_attente = (fin.tv_sec - debut.tv_sec)
                      + (fin.tv_nsec - debut.tv_nsec) / 1e9;
        enregistrer_attente(strat_local, temps_attente);
    }

    sprintf(details, "place %d", ma_place);
    ecrire_log(voiture_id, "GAREE", details);

    duree_parking = 5;
    sleep(duree_parking);

    pthread_mutex_lock(&mutex_affichage);
    clock_gettime(CLOCK_MONOTONIC, &t_depart);
    duree_garee = (t_depart.tv_sec - t_garee.tv_sec)
                + (t_depart.tv_nsec - t_garee.tv_nsec) / 1e9;
    pthread_mutex_lock(&mutex_stats_temps);
    temps_occupe_total += duree_garee;
    pthread_mutex_unlock(&mutex_stats_temps);
    places[ma_place] = 0;
    nb_places_occupees--;
    if (strat_local == 2)
        pthread_cond_signal(&cond_place_dispo);
    pthread_mutex_unlock(&mutex_affichage);

    ecrire_log(voiture_id, "DEPART", "");

    if (strat_local == 0)
        sem_post(&places_dispo);

    return NULL;
}
