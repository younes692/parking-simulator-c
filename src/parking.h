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

// semaphore : compte les places disponibles
extern sem_t places_dispo;

// mutex pour proteger places[] et les affichages
extern pthread_mutex_t mutex_affichage;

// voitures qui attendent dehors
extern int nb_en_attente;

// places actuellement occupees
extern int nb_places_occupees;

// strategie active : 0 = semaphore, 1 = attente active
extern int strategie;

void init_parking();
void destroy_parking();

#endif
