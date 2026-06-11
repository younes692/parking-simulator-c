#include "stats.h"

// compteurs par strategie
int nb_attente_sem   = 0;
double temps_total_sem  = 0.0;
int nb_attente_busy  = 0;
double temps_total_busy = 0.0;
pthread_mutex_t mutex_stats;

void init_stats() {
    pthread_mutex_init(&mutex_stats, NULL);
}

// appellee par chaque voiture apres avoir attendu
void enregistrer_attente(int strat, double temps) {
    pthread_mutex_lock(&mutex_stats);
    if (strat == 0) {
        nb_attente_sem++;
        temps_total_sem += temps;
    } else {
        nb_attente_busy++;
        temps_total_busy += temps;
    }
    pthread_mutex_unlock(&mutex_stats);
}

void afficher_stats() {
    double moy_sem  = (nb_attente_sem  > 0) ? temps_total_sem  / nb_attente_sem  : 0.0;
    double moy_busy = (nb_attente_busy > 0) ? temps_total_busy / nb_attente_busy : 0.0;

    printf("\n============================================\n");
    printf("            STATISTIQUES FINALES            \n");
    printf("============================================\n");
    printf("                   Semaphore   Attente active\n");
    printf("Voitures en att:   %-10d   %d\n",   nb_attente_sem, nb_attente_busy);
    printf("Temps moy att:     %-10.2f   %.2f sec\n", moy_sem, moy_busy);
    printf("============================================\n");
}

void exporter_csv() {
    FILE* f = fopen("logs/stats.csv", "a");
    if (f == NULL) {
        perror("fopen stats.csv");
        return;
    }
    fprintf(f, "strategie,nb_attente,temps_moyen_sec\n");
    fprintf(f, "semaphore,%d,%.2f\n", nb_attente_sem,
        (nb_attente_sem > 0) ? temps_total_sem / nb_attente_sem : 0.0);
    fprintf(f, "attente_active,%d,%.2f\n", nb_attente_busy,
        (nb_attente_busy > 0) ? temps_total_busy / nb_attente_busy : 0.0);
    fclose(f);
}
