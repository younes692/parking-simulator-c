#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include <pthread.h>

extern int nb_attente_sem;
extern double temps_total_sem;
extern int nb_attente_busy;
extern double temps_total_busy;
extern pthread_mutex_t mutex_stats;

void init_stats();
void enregistrer_attente(int strat, double temps);
void afficher_stats();
void exporter_csv();

#endif
