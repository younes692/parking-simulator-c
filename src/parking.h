#ifndef PARKING_H
#define PARKING_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// nombre de places dans le parking
#define NB_PLACES 5
// nombre de voitures qui vont essayer d'entrer
#define NB_VOITURES 10
// duree max qu'une voiture reste garee (secondes)
#define DUREE_MAX 3

#endif
