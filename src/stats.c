#include "stats.h"

extern int strategie;

// compteurs par strategie
int nb_attente_sem   = 0;
double temps_total_sem  = 0.0;
int nb_attente_busy  = 0;
double temps_total_busy = 0.0;
int nb_attente_cond  = 0;
double temps_total_cond = 0.0;
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
    } else if (strat == 1) {
        nb_attente_busy++;
        temps_total_busy += temps;
    } else {
        nb_attente_cond++;
        temps_total_cond += temps;
    }
    pthread_mutex_unlock(&mutex_stats);
}

void afficher_stats() {
    const char* nom;
    int nb;
    double total;
    double moy;

    if (strategie == 0) {
        nom   = "Semaphore";
        nb    = nb_attente_sem;
        total = temps_total_sem;
    } else if (strategie == 1) {
        nom   = "Attente active";
        nb    = nb_attente_busy;
        total = temps_total_busy;
    } else {
        nom   = "Variable condition";
        nb    = nb_attente_cond;
        total = temps_total_cond;
    }

    moy = (nb > 0) ? total / nb : 0.0;

    printf("\n============================================\n");
    printf("            STATISTIQUES FINALES            \n");
    printf("============================================\n");
    printf("Strategie      : %s\n", nom);
    printf("Voitures       : %d\n", nb);
    printf("Temps moy att  : %.2f sec\n", moy);
    printf("============================================\n");
}

void exporter_csv() {
    const char* nom;
    int nb;
    double total;
    FILE* f = fopen("logs/stats.csv", "a");

    if (f == NULL) {
        perror("fopen stats.csv");
        return;
    }

    if (strategie == 0) {
        nom   = "semaphore";
        nb    = nb_attente_sem;
        total = temps_total_sem;
    } else if (strategie == 1) {
        nom   = "attente_active";
        nb    = nb_attente_busy;
        total = temps_total_busy;
    } else {
        nom   = "variable_condition";
        nb    = nb_attente_cond;
        total = temps_total_cond;
    }

    fprintf(f, "strategie,nb_attente,temps_moyen_sec\n");
    fprintf(f, "%s,%d,%.2f\n", nom, nb, (nb > 0) ? total / nb : 0.0);
    fclose(f);
}
