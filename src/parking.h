#ifndef PARKING_H
#define PARKING_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NB_PLACES 5
#define NB_VOITURES 10
#define DUREE_MAX 3

// le semaphore compte les places disponibles
// quand il est a 0 les voitures attendent
extern sem_t places_dispo;

// mutex pour pas que les printf se melangent
extern pthread_mutex_t mutex_affichage;

// compte les voitures qui attendent une place
extern int nb_en_attente;

void init_parking();
void destroy_parking();

#endif
