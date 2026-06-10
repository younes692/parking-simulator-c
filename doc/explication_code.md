# Explication du code ligne par ligne

---

## parking.h

| Ligne | Code | Explication |
|-------|------|-------------|
| 1 | `#ifndef PARKING_H` | Vérifie si la constante PARKING_H n'a pas encore été définie. Si elle l'est déjà, tout le bloc est ignoré. Évite de lire ce fichier deux fois quand plusieurs fichiers l'incluent. |
| 2 | `#define PARKING_H` | Marque le fichier comme "déjà lu" en définissant la constante PARKING_H. |
| 4 | `#include <stdio.h>` | Inclut la bibliothèque standard d'entrée/sortie. Donne accès à printf, fprintf, fopen, etc. |
| 5 | `#include <stdlib.h>` | Inclut la bibliothèque standard. Donne accès à malloc, atoi, rand, etc. |
| 6 | `#include <pthread.h>` | Inclut la bibliothèque POSIX des threads. Donne accès à pthread_create, pthread_mutex_t, etc. |
| 7 | `#include <semaphore.h>` | Inclut la bibliothèque des sémaphores POSIX. Donne accès à sem_t, sem_wait, sem_post, etc. |
| 8 | `#include <unistd.h>` | Inclut les appels système POSIX. Donne accès à sleep, usleep, etc. |
| 9 | `#include <time.h>` | Inclut la bibliothèque de gestion du temps. Donne accès à clock_gettime, time, strftime, etc. |
| 11 | `#define NB_PLACES   5` | Définit une constante de compilation valant 5. Le compilateur remplace chaque occurrence de NB_PLACES par 5 avant de compiler. Sert à dimensionner le tableau places[]. |
| 12 | `#define NB_VOITURES 10` | Même principe : 10 voitures maximum. Sert à dimensionner les tableaux de threads dans main.c. |
| 13 | `#define DUREE_MAX   6` | Constante gardée pour compatibilité mais plus utilisée directement dans le calcul de durée. |
| 16 | `extern int nb_places;` | Déclare que la variable nb_places existe dans un autre fichier (.c). extern = "elle est définie ailleurs, je veux y accéder". Sa vraie définition est dans parking.c. |
| 17 | `extern int nb_voitures;` | Même principe pour nb_voitures. Modifiable via les arguments du programme au lancement. |
| 20 | `extern sem_t places_dispo;` | Déclare le sémaphore partagé. sem_t est un type défini par semaphore.h. Ce sémaphore compte le nombre de places disponibles dans le parking. |
| 23 | `extern pthread_mutex_t mutex_affichage;` | Déclare le mutex partagé. Un mutex est un verrou : un seul thread peut le tenir à la fois. Protège les données partagées contre les accès simultanés. |
| 26 | `extern int nb_en_attente;` | Nombre de voitures qui attendent dehors (parking plein). Affiché en temps réel dans l'interface. |
| 29 | `extern int nb_places_occupees;` | Nombre de places actuellement prises. Utilisé pour la barre d'occupation et pour l'attente active. |
| 32 | `extern int strategie;` | Mode de synchronisation actif : 0 = sémaphore, 1 = attente active. Peut changer en cours de simulation avec la touche s. |
| 34 | `void init_parking();` | Déclaration de la fonction init_parking. Le corps est dans parking.c. Initialise le sémaphore, le mutex et le tableau des places. |
| 35 | `void destroy_parking();` | Déclaration de destroy_parking. Libère les ressources système à la fin du programme. |
| 37 | `#endif` | Ferme le bloc #ifndef du début. Tout ce qui est entre #ifndef et #endif est le contenu protégé du fichier. |

---

## parking.c

| Ligne | Code | Explication |
|-------|------|-------------|
| 1 | `#include "parking.h"` | Inclut notre propre fichier d'en-tête. Les guillemets (et non les chevrons) indiquent que c'est un fichier local du projet. |
| 3 | `sem_t places_dispo;` | Création réelle du sémaphore en mémoire. C'est ici qu'il existe vraiment. Les extern dans les autres fichiers pointent vers cette variable. |
| 4 | `pthread_mutex_t mutex_affichage;` | Création réelle du mutex en mémoire. Même principe. |
| 6 | `int places[NB_PLACES];` | Tableau de 5 entiers représentant les places du parking. places[i] = 0 signifie libre, places[i] = 1 signifie occupée. |
| 7 | `int nb_en_attente    = 0;` | Compteur initialisé à 0 : au départ aucune voiture n'attend. |
| 8 | `int nb_places_occupees = 0;` | Compteur initialisé à 0 : au départ toutes les places sont libres. |
| 10 | `int strategie        = 0;` | Stratégie par défaut : 0 = sémaphore. Le programme démarre en mode sémaphore sauf si argv[3] indique 1. |
| 13 | `int nb_places   = NB_PLACES;` | Valeur runtime initialisée à 5. Si l'utilisateur tape ./parking 3 8, main.c changera cette valeur à 3 avant d'appeler init_parking. |
| 14 | `int nb_voitures = NB_VOITURES;` | Même chose pour le nombre de voitures, initialisé à 10. |
| 16 | `void init_parking() {` | Début de la fonction d'initialisation. Appelée une seule fois au démarrage depuis main.c. |
| 17 | `    int i;` | Variable de boucle locale à la fonction. |
| 18 | `    sem_init(&places_dispo, 0, nb_places);` | Initialise le sémaphore. &places_dispo = adresse du sémaphore. 0 = sémaphore local (non partagé entre processus). nb_places = valeur initiale du compteur (nombre de places disponibles au départ). |
| 19 | `    pthread_mutex_init(&mutex_affichage, NULL);` | Initialise le mutex. NULL = attributs par défaut. Après cet appel le mutex est déverrouillé et prêt à être utilisé. |
| 20 | `    for (i = 0; i < nb_places; i++)` | Boucle sur toutes les places du parking. |
| 21 | `        places[i] = 0;` | Met chaque place à 0 = libre. Au départ le parking est vide. |
| 24 | `void destroy_parking() {` | Fonction de nettoyage appelée à la toute fin du programme. |
| 25 | `    sem_destroy(&places_dispo);` | Libère les ressources système allouées au sémaphore. Sans cet appel il y aurait une fuite de ressources. |
| 26 | `    pthread_mutex_destroy(&mutex_affichage);` | Libère les ressources du mutex. Même raison. |

---

## voiture.c

| Ligne | Code | Explication |
|-------|------|-------------|
| 1 | `#include "voiture.h"` | Inclut le fichier d'en-tête de ce module, qui contient la déclaration de voiture_thread. |
| 2 | `#include "parking.h"` | Donne accès aux variables globales du parking : places[], sémaphore, mutex, etc. |
| 3 | `#include "logger.h"` | Donne accès à ecrire_log pour enregistrer les événements. |
| 4 | `#include "stats.h"` | Donne accès à enregistrer_attente pour sauvegarder le temps d'attente. |
| 5 | `#include <time.h>` | Pour clock_gettime qui mesure le temps d'attente avec précision. |
| 7 | `extern int places[NB_PLACES];` | Redéclare le tableau places[] pour y accéder dans ce fichier. Il est défini dans parking.c. |
| 8 | `extern int nb_en_attente;` | Redéclare le compteur de voitures en attente. |
| 9 | `extern int nb_places_occupees;` | Redéclare le compteur de places occupées. |
| 10 | `extern int strategie;` | Redéclare la variable de stratégie pour savoir quel mode utiliser. |
| 12 | `void* voiture_thread(void* arg) {` | Fonction exécutée par chaque thread voiture en parallèle. Le type void* est obligatoire : pthread exige cette signature pour toutes les fonctions de thread. |
| 13 | `    int voiture_id = *((int*)arg);` | Récupère l'identifiant de cette voiture depuis l'argument. arg est void* (pointeur générique), on le cast en int* puis on déréférence avec * pour lire la valeur entière. |
| 14 | `    int duree_parking;` | Variable qui stockera combien de secondes cette voiture restera garée. |
| 15 | `    int ma_place = -1;` | Numéro de la place que cette voiture va occuper. -1 = pas encore trouvé. |
| 16 | `    int i;` | Variable de boucle pour parcourir le tableau des places. |
| 17 | `    char details[64];` | Chaîne de caractères pour construire des messages de log comme "place 3". |
| 18 | `    struct timespec debut, fin;` | Structures pour stocker des temps précis (secondes + nanosecondes). Utilisées pour mesurer le temps d'attente. |
| 19 | `    double temps_attente;` | Résultat du calcul de temps d'attente en secondes (avec décimales). |
| 22 | `    sleep(rand() % 4 + 1);` | Attend entre 1 et 4 secondes avant d'arriver au parking. rand()%4 donne 0,1,2 ou 3, +1 donne 1,2,3 ou 4. Sans ça toutes les voitures arriveraient exactement en même temps. |
| 24 | `    ecrire_log(voiture_id, "ARRIVEE", "");` | Écrit dans le journal que cette voiture est arrivée. Visible dans l'interface et dans le fichier logs/parking.log. |
| 27 | `    clock_gettime(CLOCK_MONOTONIC, &debut);` | Note l'heure exacte du début de l'attente. CLOCK_MONOTONIC est une horloge qui avance toujours, même si l'heure système change. Plus fiable que time(). |
| 28 | `    nb_en_attente++;` | Incrémente le compteur de voitures en attente. Affiché dans l'interface. Attention : pas protégé par mutex, accès concurrent possible (bug étudiant volontaire). |
| 30 | `    if (strategie == 0) {` | Choisit le mode de synchronisation selon la stratégie active. |
| 33 | `        ecrire_log(voiture_id, "ATTENTE", "sem");` | Log que cette voiture attend en mode sémaphore. |
| 34 | `        sem_wait(&places_dispo);` | Le thread se met en veille si le compteur du sémaphore est à 0 (parking plein). L'OS le sort du processeur. Il sera réveillé automatiquement quand une place se libère via sem_post. Ne consomme pas de CPU pendant l'attente. |
| 38 | `        ecrire_log(voiture_id, "ATTENTE", "busy");` | Log que cette voiture attend en mode attente active. |
| 39 | `        while (nb_places_occupees >= nb_places) {` | Boucle tant que le parking est plein. Le thread reste actif sur le processeur et vérifie en permanence. |
| 40 | `            sleep(1);` | Attend 1 seconde entre chaque vérification pour ne pas bloquer le CPU à 100%. Mais c'est quand même moins efficace que le sémaphore. |
| 43 | `        sem_wait(&places_dispo);` | Même si on a utilisé l'attente active, on prend quand même le sémaphore pour garder son compteur cohérent avec la réalité. |
| 46 | `    nb_en_attente--;` | La voiture a fini d'attendre, on décrémente le compteur. |
| 49 | `    clock_gettime(CLOCK_MONOTONIC, &fin);` | Note l'heure de fin d'attente. |
| 50 | `    temps_attente = (fin.tv_sec - debut.tv_sec)` | Calcule la différence en secondes entre fin et début. tv_sec = partie entière en secondes. |
| 51 | `                  + (fin.tv_nsec - debut.tv_nsec) / 1e9;` | Ajoute la partie nanoseconde convertie en secondes. tv_nsec = nanosecondes. 1e9 = 1 milliard. Division donne les fractions de seconde. |
| 52 | `    enregistrer_attente(strategie, temps_attente);` | Envoie le temps d'attente aux stats pour calculer la moyenne à la fin. |
| 55 | `    pthread_mutex_lock(&mutex_affichage);` | Prend le verrou. À partir d'ici, aucun autre thread ne peut entrer dans une section protégée par ce mutex. Les autres bloquent sur leur propre lock jusqu'à ce qu'on appelle unlock. |
| 56 | `    for (i = 0; i < nb_places; i++) {` | Parcourt toutes les places à la recherche d'une libre. |
| 57 | `        if (places[i] == 0) {` | Si cette place est libre (0 = libre). |
| 58 | `            places[i] = 1;` | Occupe la place : met 1 pour signaler qu'elle est prise. |
| 59 | `            ma_place = i;` | Mémorise le numéro de cette place pour pouvoir la libérer plus tard. |
| 60 | `            nb_places_occupees++;` | Met à jour le compteur global de places occupées. |
| 61 | `            break;` | Sort de la boucle dès qu'une place est trouvée. On ne cherche pas plus loin. |
| 64 | `    pthread_mutex_unlock(&mutex_affichage);` | Relâche le verrou. Les autres threads en attente peuvent maintenant entrer. |
| 66 | `    sprintf(details, "place %d", ma_place);` | Construit la chaîne "place 2" (par exemple) pour le log. |
| 67 | `    ecrire_log(voiture_id, "GAREE", details);` | Écrit que la voiture est garée avec le numéro de sa place. |
| 70 | `    duree_parking = rand() % 5 + 10;` | Tire au sort une durée entre 10 et 14 secondes. rand()%5 donne 0,1,2,3 ou 4. +10 décale vers 10,11,12,13 ou 14. |
| 71 | `    sleep(duree_parking);` | La voiture reste garée ce nombre de secondes. Le thread dort. |
| 74 | `    pthread_mutex_lock(&mutex_affichage);` | Reprend le verrou pour modifier les données partagées en sécurité. |
| 75 | `    places[ma_place] = 0;` | Libère la place : remet à 0. D'autres voitures pourront la prendre. |
| 76 | `    nb_places_occupees--;` | Décrémente le compteur de places occupées. |
| 77 | `    pthread_mutex_unlock(&mutex_affichage);` | Relâche le verrou. |
| 79 | `    ecrire_log(voiture_id, "DEPART", "");` | Log que la voiture est partie. |
| 80 | `    sem_post(&places_dispo);` | Incrémente le compteur du sémaphore de 1. Signale qu'une place est libre. Si un thread dormait dans sem_wait, l'OS le réveille maintenant. |
| 82 | `    return NULL;` | La fonction de thread doit retourner void*. NULL est le code de retour du thread, récupérable par pthread_join si besoin. |

---

## logger.c

| Ligne | Code | Explication |
|-------|------|-------------|
| 1 | `#include "logger.h"` | Inclut la déclaration de ecrire_log. |
| 2 | `#include "parking.h"` | Pour les types et constantes du projet. |
| 3 | `#include "affichage.h"` | Pour accéder à ajouter_ligne_log qui met à jour le panneau journal. |
| 5 | `void ecrire_log(int id, char* evenement, char* details) {` | Fonction appelée par chaque voiture pour enregistrer un événement. id = numéro voiture, evenement = "ARRIVEE"/"GAREE"/etc., details = infos supplémentaires. |
| 6 | `    FILE* f;` | Pointeur vers un fichier. Utilisé pour écrire dans parking.log. |
| 7 | `    time_t maintenant;` | Variable qui stocke l'heure actuelle (nombre de secondes depuis le 1er janvier 1970). |
| 8 | `    char timestamp[32];` | Tableau de caractères qui contiendra l'heure formatée comme "14:35:22". |
| 9 | `    char ligne[128];` | Tableau qui contiendra la ligne complète de log à écrire. |
| 11 | `    time(&maintenant);` | Récupère l'heure actuelle et la stocke dans maintenant. Le & passe l'adresse de la variable. |
| 12 | `    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localtime(&maintenant));` | Formate l'heure en chaîne lisible. %H = heures, %M = minutes, %S = secondes. localtime convertit en heure locale. sizeof(timestamp) = taille max pour éviter un dépassement. |
| 14 | `    snprintf(ligne, sizeof(ligne), "[%s] V%02d %-8s %s",` | Construit la ligne de log. snprintf = comme printf mais écrit dans une chaîne avec taille max pour la sécurité. |
| 15 | `             timestamp, id, evenement, details);` | Les valeurs à insérer dans le format. %02d = entier sur 2 chiffres avec zéro devant (V01 pas V1). %-8s = chaîne alignée à gauche sur 8 caractères. |
| 18 | `    ajouter_ligne_log(ligne);` | Envoie la ligne au buffer circulaire de l'affichage pour qu'elle apparaisse dans le panneau journal. |
| 21 | `    f = fopen("logs/parking.log", "a");` | Ouvre le fichier en mode append (a). Si le fichier n'existe pas il est créé. Si il existe on ajoute à la fin sans effacer le contenu. "w" aurait tout effacé à chaque run. |
| 22 | `    if (f == NULL) {` | Vérifie si l'ouverture a échoué. fopen retourne NULL en cas d'erreur (dossier manquant, permissions, etc.). |
| 23 | `        perror("fopen log");` | Affiche le message d'erreur système correspondant. perror ajoute automatiquement la raison (ex: "No such file or directory"). |
| 24 | `        return;` | Sort de la fonction si l'ouverture a échoué. Pas de crash, juste pas de log fichier. |
| 26 | `    fprintf(f, "%s\n", ligne);` | Écrit la ligne dans le fichier avec un retour à la ligne. |
| 27 | `    fclose(f);` | Ferme le fichier. Obliratoire pour que les données soient bien écrites sur le disque et pour libérer la ressource. |

---

## affichage.c

| Ligne | Code | Explication |
|-------|------|-------------|
| 1 | `#include "affichage.h"` | Inclut l'en-tête avec les déclarations des fonctions et l'include ncurses. |
| 2 | `#include "parking.h"` | Pour accéder aux variables du parking à afficher. |
| 4 | `extern int places[NB_PLACES];` | Accès au tableau des places défini dans parking.c. |
| 5 | `extern int nb_en_attente;` | Accès au compteur de voitures en attente. |
| 6 | `extern int nb_places_occupees;` | Accès au compteur de places occupées pour la barre de progression. |
| 7 | `extern int strategie;` | Pour afficher le nom de la stratégie active en bas de l'écran. |
| 10 | `#define NB_LOG_LIGNES 12` | Taille du buffer circulaire : 12 lignes de journal visibles maximum. |
| 11 | `static char log_lignes[NB_LOG_LIGNES][80];` | Tableau 2D : 12 lignes de 80 caractères chacune. static = visible uniquement dans ce fichier. C'est le buffer circulaire du journal. |
| 12 | `static int log_idx = 0;` | Index qui pointe sur le prochain emplacement où écrire dans le buffer. Augmente à chaque ligne ajoutée. |
| 13 | `static int nb_lignes = 0;` | Nombre de lignes effectivement remplies (0 au départ, jusqu'à 12 max). |
| 14 | `pthread_mutex_t mutex_log;` | Mutex pour protéger le buffer circulaire. Plusieurs threads écrivent des logs simultanément, il faut synchroniser. |
| 16 | `void ajouter_ligne_log(char* ligne) {` | Fonction appelée par logger.c pour ajouter une ligne au journal de l'interface. |
| 17 | `    pthread_mutex_lock(&mutex_log);` | Verrouille le mutex pour que deux threads ne modifient pas le buffer en même temps. |
| 18 | `    strncpy(log_lignes[log_idx % NB_LOG_LIGNES], ligne, 79);` | Copie la ligne dans le buffer. log_idx % 12 donne toujours un index entre 0 et 11, c'est le mécanisme circulaire. Quand log_idx atteint 12 on revient à 0. strncpy avec 79 évite le dépassement de tampon. |
| 19 | `    log_lignes[log_idx % NB_LOG_LIGNES][79] = '\0';` | Force la terminaison de la chaîne au dernier caractère. Sécurité au cas où la ligne source fait exactement 79 caractères. |
| 20 | `    log_idx++;` | Avance l'index d'écriture au prochain emplacement. |
| 21 | `    if (nb_lignes < NB_LOG_LIGNES) nb_lignes++;` | Compte les lignes remplies jusqu'à 12. Une fois à 12 on ne l'incrémente plus, le buffer tourne. |
| 22 | `    pthread_mutex_unlock(&mutex_log);` | Relâche le verrou. |
| 25 | `void init_affichage() {` | Initialisation de ncurses. Appelée une seule fois au démarrage. |
| 26 | `    int i;` | Variable de boucle pour initialiser le buffer. |
| 27 | `    setlocale(LC_ALL, "");` | Configure les paramètres régionaux. Nécessaire pour que ncurses gère correctement les caractères spéciaux selon la langue du système. |
| 28 | `    initscr();` | Démarre ncurses et prend le contrôle du terminal. À partir de là les printf normaux ne fonctionnent plus correctement, il faut utiliser mvprintw. |
| 29 | `    noecho();` | Les touches tapées par l'utilisateur ne s'affichent pas à l'écran. Sans ça chaque appui sur s ou q apparaîtrait au milieu de l'interface. |
| 30 | `    curs_set(0);` | Cache le curseur clignotant. 0 = invisible, 1 = normal, 2 = très visible. |
| 32 | `    nodelay(stdscr, TRUE);` | Rend getch() non bloquant. Sans ça getch() attendrait indéfiniment qu'une touche soit pressée, bloquant tout l'affichage. Avec TRUE il retourne immédiatement ERR si aucune touche n'est appuyée. |
| 35 | `    start_color();` | Active le support des couleurs dans ncurses. Doit être appelé avant init_pair. |
| 36 | `    init_pair(1, COLOR_BLACK, COLOR_GREEN);` | Définit la paire de couleurs numéro 1 : texte noir sur fond vert. Utilisée pour les places libres et le mode sémaphore. |
| 37 | `    init_pair(2, COLOR_WHITE, COLOR_RED);` | Paire 2 : texte blanc sur fond rouge. Utilisée pour les places occupées et le mode attente active. |
| 38 | `    init_pair(3, COLOR_BLACK, COLOR_YELLOW);` | Paire 3 : texte noir sur fond jaune. Utilisée pour le titre en haut. |
| 40 | `    pthread_mutex_init(&mutex_log, NULL);` | Initialise le mutex du journal. |
| 41 | `    for (i = 0; i < NB_LOG_LIGNES; i++)` | Boucle sur les 12 lignes du buffer. |
| 42 | `        log_lignes[i][0] = '\0';` | Met une chaîne vide dans chaque ligne. \0 est le caractère de fin de chaîne en C. |
| 45 | `void afficher_parking() {` | Fonction appelée toutes les 100ms par le thread d'affichage pour rafraîchir l'écran. |
| 46 | `    int i, row, col, pct, bar;` | Variables locales : i pour boucles, row/col pour positions, pct pour pourcentage, bar pour longueur de la barre. |
| 47 | `    int col_d = 42;` | Colonne de départ du panneau droit (journal). 42 = après le séparateur vertical à la colonne 40. |
| 49 | `    clear();` | Efface tout l'écran ncurses avant de redessiner. Sans ça les anciens caractères resteraient visibles. |
| 52 | `    attron(COLOR_PAIR(3) | A_BOLD);` | Active la paire de couleurs 3 (jaune) et le style gras. | signifie "combine les deux attributs". |
| 53 | `    mvprintw(0, 1, " SIMULATEUR DE PARKING ");` | mv = move : déplace le curseur à la ligne 0, colonne 1. printw : affiche le texte. mvprintw combine les deux en un seul appel. |
| 54 | `    attroff(COLOR_PAIR(3) | A_BOLD);` | Désactive la couleur et le gras. Tout ce qui vient après sera en style normal. |
| 57 | `    for (i = 1; i < 23; i++)` | Boucle de la ligne 1 à 22. |
| 58 | `        mvprintw(i, 40, "|");` | Dessine une barre verticale à la colonne 40 sur toute la hauteur. Sépare visuellement le panneau gauche du panneau droit. |
| 61 | `    mvprintw(2, 1, "PLACES :");` | Affiche le titre "PLACES :" à la ligne 2, colonne 1. |
| 63 | `    for (i = 0; i < nb_places; i++) {` | Boucle sur chaque place du parking pour l'afficher. |
| 64 | `        row = 4 + (i / 5) * 3;` | Calcule la ligne d'affichage. i/5 est une division entière : places 0-4 donnent 0, places 5-9 donnent 1. Multipliée par 3 = espacement entre rangées. +4 décale depuis le haut. |
| 65 | `        col = 1 + (i % 5) * 7;` | Calcule la colonne. i%5 donne 0,1,2,3,4 pour les 5 premières places. *7 = chaque place fait 7 caractères de large. +1 = marge gauche. |
| 67 | `        if (places[i] == 1) {` | Si la place est occupée. |
| 68 | `            attron(COLOR_PAIR(2) | A_BOLD);` | Active fond rouge + gras pour place occupée. |
| 69 | `            mvprintw(row, col, "[  V  ]");` | Affiche la place avec V pour Voiture sur fond rouge. |
| 70 | `            attroff(COLOR_PAIR(2) | A_BOLD);` | Désactive les attributs. |
| 72 | `            attron(COLOR_PAIR(1));` | Active fond vert pour place libre. |
| 73 | `            mvprintw(row, col, "[     ]");` | Affiche la place vide sur fond vert. |
| 74 | `            attroff(COLOR_PAIR(1));` | Désactive le fond vert. |
| 76 | `        mvprintw(row + 1, col + 2, "#%d", i);` | Affiche le numéro de la place juste en dessous (#0, #1, #2...). row+1 = ligne suivante, col+2 = légèrement décalé pour centrer. |
| 80 | `        attron(A_BOLD);` | Active le gras pour mettre en évidence qu'il y a des voitures en attente. |
| 81 | `        mvprintw(11, 1, "En attente : %d voiture(s) !", nb_en_attente);` | Affiche le nombre de voitures qui attendent dehors. |
| 82 | `        attroff(A_BOLD);` | Désactive le gras. |
| 85 | `        mvprintw(11, 1, "En attente : aucune         ");` | Quand personne n'attend. Les espaces à la fin effacent les caractères qui pourraient rester d'un affichage précédent. |
| 89 | `    pct = (nb_places > 0) ? (nb_places_occupees * 100 / nb_places) : 0;` | Calcule le pourcentage d'occupation. La vérification nb_places > 0 évite une division par zéro. Opérateur ternaire : condition ? valeur_si_vrai : valeur_si_faux. |
| 90 | `    bar = pct / 10;` | Convertit le pourcentage (0-100) en nombre de # à afficher (0-10). |
| 91 | `    mvprintw(13, 1, "Occupation : [");` | Affiche le début de la barre de progression. |
| 92 | `    for (i = 0; i < 10; i++) {` | Boucle pour dessiner les 10 caractères de la barre. |
| 93 | `        if (i < bar) mvprintw(13, 15 + i, "#");` | Les positions jusqu'à bar sont des # (rempli). |
| 94 | `        else         mvprintw(13, 15 + i, ".");` | Les positions après bar sont des . (vide). |
| 96 | `    mvprintw(13, 26, "] %3d%%", pct);` | Ferme la barre et affiche le pourcentage. %3d = entier sur 3 chiffres. %% = affiche le caractère % (il faut le doubler car % est spécial dans les formats). |
| 100 | `    if (strategie == 0) {` | Choisit quelle couleur afficher selon la stratégie active. |
| 101 | `        attron(COLOR_PAIR(1));` | Vert pour le mode sémaphore. |
| 102 | `        mvprintw(15, 14, "SEMAPHORE        ");` | Affiche le nom de la stratégie. Les espaces à la fin effacent "ATTENTE ACTIVE" si on vient de switcher. |
| 104 | `        attron(COLOR_PAIR(2));` | Rouge pour le mode attente active. |
| 105 | `        mvprintw(15, 14, "ATTENTE ACTIVE   ");` | Affiche le mode actif. |
| 110 | `    mvprintw(18, 1, "[s] changer strategie    [q] quitter");` | Affiche les commandes disponibles en bas du panneau gauche. |
| 113 | `    mvprintw(2, col_d, "JOURNAL :");` | Titre du panneau journal à droite. |
| 114 | `    mvprintw(3, col_d, "----------------------------");` | Ligne de séparation sous le titre. |
| 116 | `    pthread_mutex_lock(&mutex_log);` | Verrouille le buffer pendant la lecture pour éviter qu'un thread écrive pendant qu'on lit. |
| 117 | `    for (i = 0; i < nb_lignes; i++) {` | Boucle sur le nombre de lignes effectivement remplies dans le buffer. |
| 118 | `        int idx = (log_idx - nb_lignes + i + NB_LOG_LIGNES) % NB_LOG_LIGNES;` | Calcule l'index réel dans le buffer circulaire. La formule retrouve les lignes dans l'ordre chronologique (de la plus ancienne à la plus récente). +NB_LOG_LIGNES évite un résultat négatif avant le modulo. |
| 119 | `        mvprintw(4 + i, col_d, "%-36.36s", log_lignes[idx]);` | Affiche la ligne de log. %-36.36s = aligné à gauche sur 36 caractères, tronqué à 36 si trop long. Garantit que chaque ligne a la même largeur. |
| 121 | `    pthread_mutex_unlock(&mutex_log);` | Relâche le verrou. |
| 123 | `    refresh();` | Envoie tout ce qui a été dessiné à l'écran. ncurses travaille en mémoire, refresh() applique les changements. Sans ça rien ne s'afficherait. |
| 126 | `void end_affichage() {` | Nettoyage de ncurses à la fin. |
| 127 | `    endwin();` | Rend le contrôle du terminal au système. Remet le terminal en mode normal. Sans ça le terminal resterait en mode ncurses après la fin du programme. |
| 128 | `    pthread_mutex_destroy(&mutex_log);` | Libère le mutex du journal. |

---

## stats.c

| Ligne | Code | Explication |
|-------|------|-------------|
| 1 | `#include "stats.h"` | Inclut l'en-tête avec les déclarations des variables et fonctions. |
| 4 | `int nb_attente_sem   = 0;` | Compteur du nombre de voitures qui ont attendu en mode sémaphore. Incrémenté à chaque fois qu'une voiture passe par enregistrer_attente avec strat=0. |
| 5 | `double temps_total_sem  = 0.0;` | Somme de tous les temps d'attente en mode sémaphore. On divise par nb_attente_sem à la fin pour avoir la moyenne. |
| 6 | `int nb_attente_busy  = 0;` | Même chose pour le mode attente active. |
| 7 | `double temps_total_busy = 0.0;` | Somme des temps d'attente en mode attente active. |
| 8 | `pthread_mutex_t mutex_stats;` | Mutex pour protéger les compteurs. Plusieurs threads appellent enregistrer_attente en même temps, il faut éviter que deux threads modifient les mêmes variables simultanément. |
| 10 | `void init_stats() {` | Initialisation des stats. |
| 11 | `    pthread_mutex_init(&mutex_stats, NULL);` | Initialise le mutex. Les compteurs sont déjà à 0 grâce à leur initialisation ligne 4-7. |
| 15 | `void enregistrer_attente(int strat, double temps) {` | Appelée par chaque voiture après avoir attendu. strat = quelle stratégie était active, temps = durée d'attente en secondes. |
| 16 | `    pthread_mutex_lock(&mutex_stats);` | Verrouille les stats. Sans ça deux voitures pourraient incrémenter le compteur en même temps et une des deux incrémentations serait perdue. |
| 17 | `    if (strat == 0) {` | Si la stratégie était le sémaphore. |
| 18 | `        nb_attente_sem++;` | Compte une voiture de plus en mode sémaphore. |
| 19 | `        temps_total_sem += temps;` | Ajoute son temps d'attente à la somme totale. |
| 21 | `        nb_attente_busy++;` | Même chose pour le mode attente active. |
| 22 | `        temps_total_busy += temps;` | Cumule le temps d'attente en mode busy. |
| 24 | `    pthread_mutex_unlock(&mutex_stats);` | Relâche le verrou. |
| 27 | `void afficher_stats() {` | Affiche le tableau comparatif après la simulation, quand ncurses est déjà fermé (printf fonctionne à nouveau). |
| 28 | `    double moy_sem  = (nb_attente_sem  > 0) ? temps_total_sem  / nb_attente_sem  : 0.0;` | Calcule la moyenne : total divisé par nombre. Le test > 0 évite la division par zéro si aucune voiture n'a utilisé ce mode. |
| 29 | `    double moy_busy = (nb_attente_busy > 0) ? temps_total_busy / nb_attente_busy : 0.0;` | Même calcul pour le mode attente active. |
| 31 | `    printf("\n============================================\n");` | Ligne de séparation. |
| 34 | `    printf("                   Semaphore   Attente active\n");` | En-têtes des colonnes du tableau. |
| 35 | `    printf("Voitures en att:   %-10d   %d\n", nb_attente_sem, nb_attente_busy);` | Affiche les compteurs. %-10d = entier aligné à gauche sur 10 caractères pour que les colonnes s'alignent. |
| 36 | `    printf("Temps moy att:     %-10.2f   %.2f sec\n", moy_sem, moy_busy);` | Affiche les moyennes. .2f = 2 décimales après la virgule. |
| 38 | `    printf("Le semaphore est plus efficace...\n");` | Conclusion pédagogique expliquant pourquoi le sémaphore est meilleur. |
| 43 | `void exporter_csv() {` | Écrit les résultats dans un fichier CSV lisible dans Excel. |
| 44 | `    FILE* f = fopen("logs/stats.csv", "a");` | Ouvre le fichier en mode append. |
| 45 | `    if (f == NULL) {` | Vérifie si l'ouverture a réussi. |
| 46 | `        perror("fopen stats.csv");` | Affiche l'erreur système si l'ouverture a échoué. |
| 47 | `        return;` | Sort de la fonction proprement sans planter. |
| 49 | `    fprintf(f, "strategie,nb_attente,temps_moyen_sec\n");` | Écrit la ligne d'en-tête du CSV. Les virgules séparent les colonnes. |
| 50 | `    fprintf(f, "semaphore,%d,%.2f\n", nb_attente_sem, ...);` | Ligne de données pour le sémaphore. |
| 52 | `    fprintf(f, "attente_active,%d,%.2f\n", nb_attente_busy, ...);` | Ligne de données pour l'attente active. |
| 54 | `    fclose(f);` | Ferme le fichier et garantit que tout est bien écrit sur le disque. |

---

## main.c

| Ligne | Code | Explication |
|-------|------|-------------|
| 1 | `#include <stdio.h>` | Pour printf utilisé dans les messages d'erreur d'arguments invalides. |
| 2 | `#include <stdlib.h>` | Pour atoi qui convertit les arguments texte en entiers. |
| 3 | `#include <pthread.h>` | Pour pthread_t, pthread_create, pthread_join. |
| 5 | `#include "parking.h"` | Pour NB_VOITURES, nb_places, nb_voitures et les autres variables globales. |
| 6 | `#include "voiture.h"` | Pour la déclaration de voiture_thread. |
| 7 | `#include "affichage.h"` | Pour init_affichage, afficher_parking, end_affichage, getch. |
| 8 | `#include "stats.h"` | Pour init_stats, afficher_stats, exporter_csv. |
| 10 | `extern int strategie;` | Accès à la variable stratégie définie dans parking.c pour pouvoir la modifier avec la touche s. |
| 12 | `int simulation_active = 1;` | Drapeau partagé entre le thread main et le thread d'affichage. 1 = en cours, 0 = terminé. Quand main met 0 le thread d'affichage s'arrête. |
| 14 | `void* thread_affichage(void* arg) {` | Fonction du thread qui gère l'affichage et les touches. Tourne en parallèle pendant toute la simulation. |
| 15 | `    int ch;` | Variable qui stocke la touche captée par getch. |
| 16 | `    while (simulation_active) {` | Boucle tant que la simulation est active. Quand simulation_active passe à 0 la boucle s'arrête et le thread se termine. |
| 17 | `        afficher_parking();` | Redessine tout l'écran avec l'état actuel. |
| 18 | `        usleep(100000);` | Attend 100 000 microsecondes = 100 millisecondes = 0,1 seconde entre chaque rafraîchissement. Donne environ 10 images par seconde. |
| 19 | `        ch = getch();` | Lit une touche si l'utilisateur en a pressé une. Retourne ERR immédiatement si aucune touche (grâce à nodelay). |
| 20 | `        if (ch == 'q') {` | Si l'utilisateur a tapé q. |
| 21 | `            simulation_active = 0;` | Demande l'arrêt de la simulation. Le thread d'affichage s'arrêtera, et main arrêtera aussi d'attendre les voitures. |
| 23 | `        } else if (ch == 's') {` | Si l'utilisateur a tapé s. |
| 24 | `            strategie = (strategie == 0) ? 1 : 0;` | Bascule la stratégie. Si elle était 0 elle devient 1, si elle était 1 elle devient 0. Opérateur ternaire pour faire ça en une ligne. |
| 27 | `    return NULL;` | Le thread d'affichage se termine proprement. |
| 30 | `int main(int argc, char* argv[]) {` | Point d'entrée du programme. argc = nombre d'arguments, argv = tableau des arguments en texte. argv[0] = nom du programme, argv[1] = premier argument, etc. |
| 31 | `    pthread_t threads[NB_VOITURES];` | Tableau pour stocker les identifiants des threads voitures. Chaque pthread_t représente un thread. |
| 32 | `    pthread_t t_affichage;` | Identifiant du thread d'affichage. |
| 33 | `    int ids[NB_VOITURES];` | Tableau des numéros de voitures (1 à 10). Chaque thread reçoit son propre entier en argument. |
| 34 | `    int i;` | Variable de boucle. |
| 37 | `    if (argc >= 3) {` | Si au moins 3 arguments sont passés (nom_programme + 2 args). argc inclut le nom du programme donc argc=3 = ./parking arg1 arg2. |
| 38 | `        nb_places   = atoi(argv[1]);` | atoi = ASCII to Integer : convertit la chaîne argv[1] en entier. "3" devient 3. Si invalide atoi retourne 0. |
| 39 | `        nb_voitures = atoi(argv[2]);` | Même chose pour le nombre de voitures. |
| 41 | `        if (nb_places < 1 || nb_places > NB_PLACES) {` | Validation : au moins 1 place, au maximum NB_PLACES (5). || = OU logique : si l'une OU l'autre condition est vraie. |
| 42 | `            printf("nb_places invalide, max %d\n", NB_PLACES);` | Message d'erreur explicatif. |
| 43 | `            return 1;` | Quitte main avec code d'erreur 1. Par convention 0 = succès, autre valeur = erreur. |
| 51 | `    if (argc >= 4) {` | Si un 3ème argument est passé pour la stratégie initiale. |
| 52 | `        strategie = atoi(argv[3]);` | Lit la stratégie. 0 ou 1. |
| 53 | `        if (strategie != 0 && strategie != 1) {` | Valide que c'est bien 0 ou 1. && = ET logique. |
| 59 | `    init_parking();` | Initialise le sémaphore, le mutex et les places. Doit être fait avant de lancer les threads. |
| 60 | `    init_stats();` | Initialise le mutex des stats. |
| 61 | `    init_affichage();` | Démarre ncurses, configure les couleurs, initialise le buffer journal. |
| 63 | `    pthread_create(&t_affichage, NULL, thread_affichage, NULL);` | Lance le thread d'affichage. Il tourne immédiatement en parallèle de main. Arguments : adresse où stocker l'id, attributs (NULL=défaut), fonction à exécuter, argument à passer (NULL=aucun). |
| 66 | `    for (i = 0; i < nb_voitures; i++) {` | Boucle pour créer tous les threads voitures. |
| 67 | `        ids[i] = i + 1;` | Stocke le numéro de voiture dans le tableau. i+1 pour commencer à V01 et non V00. L'adresse &ids[i] sera passée au thread. |
| 68 | `        pthread_create(&threads[i], NULL, voiture_thread, &ids[i]);` | Lance le thread voiture. On passe &ids[i] et non ids[i] car pthread_create attend un pointeur. Le thread lira la valeur avec *((int*)arg). |
| 72 | `    for (i = 0; i < nb_voitures; i++) {` | Deuxième boucle pour attendre que tous les threads finissent. |
| 73 | `        pthread_join(threads[i], NULL);` | Bloque jusqu'à ce que ce thread soit terminé. Sans ça main continuerait et fermerait le programme avant que les voitures aient fini. NULL = on ne récupère pas la valeur de retour. |
| 76 | `    simulation_active = 0;` | Signale au thread d'affichage de s'arrêter. |
| 77 | `    pthread_join(t_affichage, NULL);` | Attend que le thread d'affichage se termine proprement. |
| 79 | `    end_affichage();` | Ferme ncurses et rend le terminal à l'état normal. |
| 82 | `    afficher_stats();` | Affiche le tableau comparatif dans le terminal (avec printf, ncurses est fermé). |
| 83 | `    exporter_csv();` | Écrit les résultats dans logs/stats.csv. |
| 85 | `    destroy_parking();` | Libère le sémaphore et le mutex. |
| 86 | `    return 0;` | Quitte le programme avec code 0 = succès. |
