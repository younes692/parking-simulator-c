#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void init_affichage();
void afficher_parking();
void end_affichage();
// appelee par logger pour ajouter une ligne au panneau journal
void ajouter_ligne_log(char* ligne);

#endif
