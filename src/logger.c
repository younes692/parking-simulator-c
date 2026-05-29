#include "logger.h"
#include "parking.h"
#include "affichage.h"

void ecrire_log(int id, char* evenement, char* details) {
    FILE* f;
    time_t maintenant;
    char timestamp[32];
    char ligne[128];

    time(&maintenant);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localtime(&maintenant));

    snprintf(ligne, sizeof(ligne), "[%s] V%02d %-8s %s",
             timestamp, id, evenement, details);

    // ajouter au panneau journal de l affichage
    ajouter_ligne_log(ligne);

    // mode append sinon le fichier est ecrase a chaque run
    f = fopen("logs/parking.log", "a");
    if (f == NULL) {
        perror("fopen log");
        return;
    }
    fprintf(f, "%s\n", ligne);
    fclose(f);
}
