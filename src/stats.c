#include "stats.h"
#include <time.h>

extern int strategie;
extern int nb_places;
extern struct timespec simulation_debut;
extern double temps_occupe_total;

// compteurs par strategie
int nb_attente_sem   = 0;
double temps_total_sem  = 0.0;
int nb_attente_busy  = 0;
double temps_total_busy = 0.0;
int nb_attente_cond  = 0;
double temps_total_cond = 0.0;

// entree directe (< 0.1 sec) vs entree apres attente
int nb_entree_directe = 0;
int nb_entree_attente = 0;

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
    if (temps > 0.1)
        nb_entree_attente++;
    else
        nb_entree_directe++;
    pthread_mutex_unlock(&mutex_stats);
}

void afficher_stats() {
    struct timespec maintenant;
    double duree_sim;
    double taux;
    const char* nom;
    int nb;
    double total;
    double moy;

    clock_gettime(CLOCK_MONOTONIC, &maintenant);
    duree_sim = (maintenant.tv_sec - simulation_debut.tv_sec)
              + (maintenant.tv_nsec - simulation_debut.tv_nsec) / 1e9;
    taux = (duree_sim > 0 && nb_places > 0)
           ? (temps_occupe_total / ((double)nb_places * duree_sim)) * 100.0
           : 0.0;
    if (taux > 100.0) taux = 100.0;

    if (strategie == 0) {
        nom   = "SEMAPHORE";
        nb    = nb_attente_sem;
        total = temps_total_sem;
    } else if (strategie == 1) {
        nom   = "ATTENTE ACTIVE";
        nb    = nb_attente_busy;
        total = temps_total_busy;
    } else {
        nom   = "VARIABLE CONDITION";
        nb    = nb_attente_cond;
        total = temps_total_cond;
    }

    moy = (nb > 0) ? total / nb : 0.0;

    printf("\n============================================\n");
    printf("            STATISTIQUES FINALES            \n");
    printf("============================================\n");
    printf("Strategie utilisee : %s\n", nom);
    printf("Entree directe     : %d voiture(s)\n", nb_entree_directe);
    printf("Entree en attente  : %d voiture(s)\n", nb_entree_attente);
    printf("Temps moy attente  : %.2f sec\n", moy);
    printf("Taux d utilisation : %.1f%%\n", taux);
    printf("============================================\n");
}

void exporter_csv() {
    struct timespec maintenant;
    double duree_sim;
    double taux;
    const char* nom;
    int nb;
    double total;
    FILE* f;

    clock_gettime(CLOCK_MONOTONIC, &maintenant);
    duree_sim = (maintenant.tv_sec - simulation_debut.tv_sec)
              + (maintenant.tv_nsec - simulation_debut.tv_nsec) / 1e9;
    taux = (duree_sim > 0 && nb_places > 0)
           ? (temps_occupe_total / ((double)nb_places * duree_sim)) * 100.0
           : 0.0;
    if (taux > 100.0) taux = 100.0;

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

    f = fopen("logs/stats.csv", "a");
    if (f == NULL) {
        perror("fopen stats.csv");
        return;
    }
    fprintf(f, "strategie,nb_entree_directe,nb_entree_attente,temps_moyen_sec,taux_utilisation\n");
    fprintf(f, "%s,%d,%d,%.2f,%.2f\n",
            nom, nb_entree_directe, nb_entree_attente,
            (nb > 0) ? total / nb : 0.0, taux);
    fclose(f);
}
