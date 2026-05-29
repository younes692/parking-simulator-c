#include "affichage.h"
#include "parking.h"

extern int places[NB_PLACES];
extern int nb_en_attente;

void init_affichage() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    // pour que getch() soit non bloquant
    nodelay(stdscr, TRUE);
}

void afficher_parking() {
    int i;
    printf("ici\n");

    clear();
    mvprintw(0, 0, "=== SIMULATEUR PARKING ===");
    mvprintw(2, 0, "Etat des places :");

    // on affiche chaque place
    for (i = 0; i < NB_PLACES; i++) {
        if (places[i] == 1) {
            mvprintw(4, i * 7, "[  V  ]");
        } else {
            mvprintw(4, i * 7, "[     ]");
        }
        mvprintw(5, i * 7 + 2, "%d", i);
    }

    mvprintw(7, 0, "Voitures en attente : %d", nb_en_attente);
    mvprintw(9, 0, "Appuyer sur q pour quitter");

    refresh();
}

void end_affichage() {
    endwin();
}
