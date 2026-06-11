#include "affichage.h"
#include "parking.h"

extern int places[NB_PLACES];
extern int nb_en_attente;
extern int nb_places_occupees;
extern int strategie;

// buffer circulaire pour le journal a droite
#define NB_LOG_LIGNES 12
static char log_lignes[NB_LOG_LIGNES][80];
static int log_idx = 0;
static int nb_lignes = 0;
pthread_mutex_t mutex_log;

void ajouter_ligne_log(char* ligne) {
    pthread_mutex_lock(&mutex_log);
    strncpy(log_lignes[log_idx % NB_LOG_LIGNES], ligne, 79);
    log_lignes[log_idx % NB_LOG_LIGNES][79] = '\0';
    log_idx++;
    if (nb_lignes < NB_LOG_LIGNES) nb_lignes++;
    pthread_mutex_unlock(&mutex_log);
}

void init_affichage() {
    int i;
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    // getch() non bloquant
    nodelay(stdscr, TRUE);

    // couleurs
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);  // place libre
    init_pair(2, COLOR_WHITE, COLOR_RED);    // place occupee
    init_pair(3, COLOR_BLACK, COLOR_YELLOW); // titre

    pthread_mutex_init(&mutex_log, NULL);
    for (i = 0; i < NB_LOG_LIGNES; i++)
        log_lignes[i][0] = '\0';
}

int afficher_menu() {
    int sel = 0;
    int ch;
    int i;
    const char* noms[3] = {
        "Semaphore          ",
        "Attente active     ",
        "Variable condition "
    };
    const char* desc[3] = {
        "thread dort, reveille par sem_post",
        "boucle while, verifie sous mutex",
        "pthread_cond_wait, reveille par signal"
    };

    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);

    while (1) {
        clear();

        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(0, 1, " SIMULATEUR DE PARKING ");
        attroff(COLOR_PAIR(3) | A_BOLD);

        mvprintw(2, 2, "Choisissez une strategie de synchronisation :");

        for (i = 0; i < 3; i++) {
            if (i == sel) {
                mvprintw(4 + i, 2, ">");
                attron(COLOR_PAIR(1) | A_BOLD);
                mvprintw(4 + i, 4, "[ %s]", noms[i]);
                attroff(COLOR_PAIR(1) | A_BOLD);
            } else {
                mvprintw(4 + i, 2, " ");
                mvprintw(4 + i, 4, "[ %s]", noms[i]);
            }
            mvprintw(4 + i, 28, "  %s", desc[i]);
        }

        mvprintw(9, 2, "Fleche haut/bas pour naviguer   ENTREE pour confirmer");

        refresh();

        ch = getch();
        if (ch == KEY_UP && sel > 0)
            sel--;
        else if (ch == KEY_DOWN && sel < 2)
            sel++;
        else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER)
            break;
    }

    nodelay(stdscr, TRUE);
    return sel;
}

void afficher_parking() {
    int i, row, col, pct, bar;
    int col_d = 42; // colonne de depart panneau droit

    clear();

    // titre
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(0, 1, " SIMULATEUR DE PARKING ");
    attroff(COLOR_PAIR(3) | A_BOLD);

    // separateur vertical
    for (i = 1; i < 23; i++)
        mvprintw(i, 40, "|");

    // --- PANNEAU GAUCHE ---
    mvprintw(2, 1, "PLACES :");

    for (i = 0; i < nb_places; i++) {
        row = 4 + (i / 5) * 3;
        col = 1 + (i % 5) * 7;

        if (places[i] == 1) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(row,     col, "[  V  ]");
            attroff(COLOR_PAIR(2) | A_BOLD);
        } else {
            attron(COLOR_PAIR(1));
            mvprintw(row,     col, "[     ]");
            attroff(COLOR_PAIR(1));
        }
        mvprintw(row + 1, col + 2, "#%d", i);
    }

    // voitures en attente
    if (nb_en_attente > 0) {
        attron(A_BOLD);
        mvprintw(11, 1, "En attente : %d voiture(s) !", nb_en_attente);
        attroff(A_BOLD);
    } else {
        mvprintw(11, 1, "En attente : aucune         ");
    }

    // barre d occupation
    pct = (nb_places > 0) ? (nb_places_occupees * 100 / nb_places) : 0;
    bar = pct / 10;
    mvprintw(13, 1, "Occupation : [");
    for (i = 0; i < 10; i++) {
        if (i < bar) mvprintw(13, 15 + i, "#");
        else         mvprintw(13, 15 + i, ".");
    }
    mvprintw(13, 26, "] %3d%%", pct);

    // strategie active
    mvprintw(15, 1, "Strategie  : ");
    if (strategie == 0) {
        attron(COLOR_PAIR(1));
        mvprintw(15, 14, "SEMAPHORE        ");
        attroff(COLOR_PAIR(1));
    } else if (strategie == 1) {
        attron(COLOR_PAIR(2));
        mvprintw(15, 14, "ATTENTE ACTIVE   ");
        attroff(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(3));
        mvprintw(15, 14, "VAR. CONDITION   ");
        attroff(COLOR_PAIR(3));
    }

    mvprintw(18, 1, "[q] quitter");

    // --- PANNEAU DROIT : journal ---
    mvprintw(2, col_d, "JOURNAL :");
    mvprintw(3, col_d, "----------------------------");

    pthread_mutex_lock(&mutex_log);
    for (i = 0; i < nb_lignes; i++) {
        int idx = (log_idx - nb_lignes + i + NB_LOG_LIGNES) % NB_LOG_LIGNES;
        mvprintw(4 + i, col_d, "%-36.36s", log_lignes[idx]);
    }
    pthread_mutex_unlock(&mutex_log);

    refresh();
}

void end_affichage() {
    endwin();
    pthread_mutex_destroy(&mutex_log);
}
