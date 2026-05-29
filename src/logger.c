#include "logger.h"
#include "parking.h"

void ecrire_log(int id, char* evenement, char* details) {
    FILE* f;
    time_t maintenant;
    char timestamp[64];

    time(&maintenant);
    // on formate la date lisiblement
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&maintenant));

    // mode append sinon le fichier est ecrase a chaque run
    f = fopen("logs/parking.log", "a");
    if (f == NULL) {
        perror("fopen log");
        return;
    }

    fprintf(f, "[%s] [voiture %02d] [%s] %s\n", timestamp, id, evenement, details);
    fclose(f);
}
