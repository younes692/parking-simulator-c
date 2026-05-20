#ifndef VOITURE_H
#define VOITURE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "parking.h"

// la fonction que chaque thread voiture va executer
void* voiture_thread(void* arg);

#endif
