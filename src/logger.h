#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

// ecrit une ligne dans le fichier de log
void ecrire_log(int id, char* evenement, char* details);

#endif
