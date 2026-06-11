#ifndef PARKING_H
#define PARKING_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NB_PLACES   5
#define NB_VOITURES 10
#define DUREE_MAX   6

// valeurs utilisees a l execution (modifiables via argv)
extern int nb_places;
extern int nb_voitures;

// semaphore : compte les places disponibles
extern sem_t places_dispo;

// mutex pour proteger places[] et les affichages
extern pthread_mutex_t mutex_affichage;

// mutex pour proteger nb_en_attente et la lecture de strategie
extern pthread_mutex_t mutex_compteurs;

// variable de condition pour la strategie 2
extern pthread_cond_t cond_place_dispo;

// mutex pour proteger temps_occupe_total
extern pthread_mutex_t mutex_stats_temps;

// voitures qui attendent dehors
extern int nb_en_attente;

// places actuellement occupees
extern int nb_places_occupees;

// strategie active : 0 = semaphore, 1 = attente active, 2 = variable condition
extern int strategie;

// pour le calcul du taux d utilisation
extern struct timespec simulation_debut;
extern double temps_occupe_total;

void init_parking();
void destroy_parking();

#endif
