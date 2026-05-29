# Mini Projet — Simulation du Problème de Parking
## Rapport Technique

**Etudiant(s) :** Younes MOUMOU  
**Filière :** Génie Informatique - 1ère année  
**Année universitaire :** 2025-2026  
**Module :** Systèmes d'Exploitation  
**Dépôt GitHub :** https://github.com/younes692/parking-simulator-c  
**Date :** Mai 2026  

---

## Sommaire

1. Introduction
2. Analyse Conceptuelle
3. Architecture du système
4. Planning de réalisation
5. Implémentation par semaine
6. Tests et Résultats *(à compléter)*
7. Difficultés rencontrées
8. Conclusion

---

## 1. Introduction

Ce projet a pour but de simuler la gestion d'un parking avec un nombre limité de places où plusieurs voitures arrivent en même temps. Il s'inscrit dans le cadre du cours de Systèmes d'Exploitation et illustre de façon concrète les mécanismes de synchronisation entre processus.

Le problème est le suivant : chaque voiture est un thread, les places de parking sont une ressource partagée et limitée. Si le parking est plein, une voiture doit attendre qu'une place se libère avant d'entrer. Sans mécanismes de protection, deux voitures pourraient occuper la même place en même temps, ou le programme pourrait se retrouver dans un interblocage.

Les concepts du cours mis en oeuvre dans ce projet sont :

- **Sémaphore compteur** : pour bloquer les voitures quand le parking est plein
- **Mutex** : pour protéger l'accès au tableau des places et les écritures dans les logs
- **Attente active** : implémentée en parallèle pour montrer son inefficacité par rapport au sémaphore
- **Prévention de l'interblocage** : par un ordre d'acquisition fixe des ressources, commenté dans le code

---

## 2. Analyse Conceptuelle

### 2.1 Description du problème

Le parking est une ressource bornée : il possède NB_PLACES places (configurable, défaut 5). Les voitures sont des processus concurrents qui arrivent de façon aléatoire, tentent d'accéder à la ressource, l'utilisent pendant un certain temps, puis la libèrent.

Contraintes à respecter :
- Deux voitures ne peuvent pas occuper la même place en même temps
- Une voiture qui attend ne doit pas consommer du CPU inutilement (mode sémaphore)
- Aucun interblocage ne doit se produire
- Les actions de chaque voiture doivent être enregistrées dans un fichier log

### 2.2 Concepts utilisés

| Concept | Rôle dans le projet |
|---|---|
| Thread (pthread) | Chaque voiture est un thread indépendant créé avec pthread_create() |
| Sémaphore compteur | Initialisé à N places. sem_wait() bloque si plein, sem_post() libère une place |
| Mutex | Protège le tableau places[] et les écritures dans le fichier log |
| Attente active | Boucle while vérifiant l'occupation — montre le gaspillage CPU vs sémaphore |
| Section critique | L'attribution d'une place est protégée : lecture + modification + écriture atomiques |
| Interblocage | Évité par ordre d'acquisition fixe : toujours sémaphore avant mutex, jamais l'inverse |

### 2.3 Identification des acteurs

**Voiture** — Thread qui arrive, attend si besoin, se gare, puis repart. Interagit avec le sémaphore et le tableau des places.

**Utilisateur** — Lance le programme avec des paramètres CLI, peut changer la stratégie de synchronisation en appuyant sur `s`, et quitter avec `q`.

**Système** — Gère les ressources partagées : sémaphore, mutex, tableau des places, fichier log, statistiques.

### 2.4 Diagramme de cas d'utilisation

```
+-------------------------------------------------------+
|                   Système de parking                  |
|                                                       |
|  Voiture ----> Arriver au parking                     |
|           ----> Tenter d'entrer (sem ou busy wait)    |
|           ----> Se garer sur une place libre          |
|           ----> Occuper la place (sleep)              |
|           ----> Libérer la place et partir            |
|                                                       |
|  Utilisateur -> Lancer la simulation avec params      |
|             -> Changer la stratégie (touche 's')      |
|             -> Consulter l'affichage en temps réel    |
|             -> Quitter proprement (touche 'q')        |
+-------------------------------------------------------+
```

### 2.5 Diagramme de classes

```
+------------------+        +------------------+        +------------------+
|     Parking      |        |     Voiture      |        |      Logger      |
+------------------+        +------------------+        +------------------+
| - places[N]: int |<>------| - id: int        |        | - fichier: FILE* |
| - nb_places: int |        | - thread: pthread|        | - mutex: mutex_t |
| - occupes: int   |        | - t_arrivee: long|        +------------------+
| - sem: sem_t     |        | - t_attente: long|        | + ecrire_log()   |
| - mutex: mutex_t |        | - spot_id: int   |        +------------------+
| - strategie: int |        +------------------+
+------------------+        | + voiture_thread()|
| + init_parking() |        +------------------+
| + destroy()      |
| + trouver_place()|        +------------------+        +------------------+
+------------------+        |      Stats       |        |    Affichage     |
                            +------------------+        +------------------+
                            | - nb_attente_sem |        | - mutex: mutex_t |
                            | - temps_sem      |        +------------------+
                            | - nb_attente_busy|        | + init_affichage()|
                            | - temps_busy     |        | + afficher()     |
                            | - occupation_moy |        | + end_affichage()|
                            +------------------+        +------------------+
                            | + calculer_stats()|
                            +------------------+
```

### 2.6 Diagramme d'activité — Cycle de vie d'une voiture

```
                    [DEBUT DU THREAD]
                           |
                    sleep(aléatoire)      <- attente avant arrivée
                           |
              [ARRIVÉE AU PARKING] --> ecrire_log(ARRIVEE)
                           |
              +------------+------------+
              |                         |
      strategie == 0              strategie == 1
      (SEMAPHORE)                (ATTENTE ACTIVE)
              |                         |
         sem_wait()              while(occupes >= N)
   (thread mis en veille          { sleep(1) }    <- gaspille CPU
      par l'OS si plein)
              |                         |
         mutex_lock()              mutex_lock()
         place = trouver()         place = trouver()
         occupes++                 occupes++
         mutex_unlock()            mutex_unlock()
              |                         |
              +------------+------------+
                           |
              [GARÉE place #X] --> ecrire_log(GAREE)
                           |
                    sleep(rand()%3+1)    <- simule le stationnement
                           |
              [DEPART] --> ecrire_log(DEPART)
                           |
              +------------+------------+
              |                         |
         mutex_lock()              mutex_lock()
         places[x] = 0             places[x] = 0
         occupes--                 occupes--
         mutex_unlock()            mutex_unlock()
         sem_post()
              |                         |
              +------------+------------+
                           |
                     [FIN DU THREAD]
```

### 2.7 Diagramme de séquence — Entrée et sortie d'une voiture

```
  main        voiture_thread    semaphore     mutex        logger
   |                |               |            |            |
   |--pthread_create->|             |            |            |
   |                |               |            |            |
   |                |--sleep(rand)->|            |            |
   |                |               |            |            |
   |                |-ecrire_log(ARRIVEE)----------------------------->|
   |                |               |            |            |
   |                |--sem_wait()--->|            |            |
   |                | (bloque si     |            |            |
   |                |  compteur=0)   |            |            |
   |                |<--ok-----------|            |            |
   |                |               |            |            |
   |                |--mutex_lock()-------------->|            |
   |                |  trouver place libre        |            |
   |                |  places[x] = 1              |            |
   |                |  occupes++                  |            |
   |                |--mutex_unlock()------------>|            |
   |                |               |            |            |
   |                |-ecrire_log(GAREE, spot=x)-------------->|
   |                |               |            |            |
   |                |--sleep(rand)->|            |            |
   |                |  (stationnement)           |            |
   |                |               |            |            |
   |                |--mutex_lock()-------------->|            |
   |                |  places[x] = 0              |            |
   |                |  occupes--                  |            |
   |                |--mutex_unlock()------------>|            |
   |                |               |            |            |
   |                |--sem_post()--->|            |            |
   |                |-ecrire_log(DEPART)------------------------------>|
   |                |               |            |            |
   |<--thread termine|              |            |            |
```

---

## 3. Architecture du système

### 3.1 Structure des fichiers

```
parking-simulator-c/
├── Makefile
├── README.md
├── src/
│   ├── main.c          <- point d'entrée, création des threads, thread affichage
│   ├── parking.h       <- constantes, déclarations des variables globales
│   ├── parking.c       <- init/destroy sémaphore et mutex, tableau des places
│   ├── voiture.h
│   ├── voiture.c       <- voiture_thread(), les deux stratégies
│   ├── logger.h
│   ├── logger.c        <- écriture dans parking.log avec timestamp
│   ├── stats.h         <- (semaine 3)
│   ├── stats.c         <- (semaine 3)
│   ├── affichage.h
│   └── affichage.c     <- interface ncurses, thread de rafraîchissement
├── doc/
│   ├── documentation_technique.md
│   └── guide_utilisation.md
└── logs/
    ├── parking.log
    └── stats.csv
```

### 3.2 Variables globales

| Variable | Type | Rôle | Protection |
|---|---|---|---|
| places[NB_PLACES] | int[] | État de chaque place (0=libre, 1=occupé) | mutex_affichage |
| nb_en_attente | int | Voitures qui attendent une place | mutex_affichage |
| places_dispo | sem_t | Sémaphore compteur — nb places libres | sem_wait/post |
| mutex_affichage | pthread_mutex_t | Protège places[] et les printf | N/A |
| strategie | int | Mode actif : 0=sémaphore, 1=attente active | lecture seule |
| simulation_active | int | Flag d'arrêt pour le thread d'affichage | lecture seule |

---

## 4. Planning de réalisation

| Semaine | Objectifs |
|---|---|
| S1 | Structure du projet, Makefile, voiture_thread() basique, création des threads sans synchronisation |
| S2 | Sémaphore + mutex, tableau places[], journalisation dans parking.log en mode append |
| S3 | Interface ncurses, deuxième stratégie (attente active), changement dynamique, statistiques comparatives |
| S4 | Arguments CLI, nettoyage du code, finalisation de ce rapport et rédaction du guide d'utilisation |

---

## 5. Implémentation par semaine

### 5.1 Semaine 1 — Fondations (threads sans synchronisation)

La première semaine a permis de mettre en place la structure complète du projet et de vérifier que les threads fonctionnent correctement. Chaque voiture est un thread créé avec `pthread_create()` qui reçoit son identifiant en argument, affiche son arrivée, dort un moment aléatoire, puis affiche son départ.

À ce stade il n'y a pas encore de synchronisation, ce qui fait que les `printf` se mélangent dans le terminal. C'est normal et voulu — ça illustre bien le problème qu'on va résoudre en semaine 2.

**Code clé — voiture_thread() semaine 1 :**

```c
void* voiture_thread(void* arg) {
    int voiture_id = *((int*)arg);
    int duree_parking;

    printf("[voiture %02d] arrive au parking\n", voiture_id);

    duree_parking = rand() % DUREE_MAX + 1;
    sleep(duree_parking);

    printf("[voiture %02d] quitte le parking\n", voiture_id);
    return NULL;
}
```

**Difficulté rencontrée :** un warning à la compilation sur `sleep()` parce que `<unistd.h>` n'était pas inclus dans `voiture.h`. Corrigé en ajoutant l'include manquant.

### 5.2 Semaine 2 — Synchronisation et journalisation

C'est la semaine principale du projet. Trois choses ont été ajoutées :

**Sémaphore et mutex** — Le sémaphore `places_dispo` est initialisé à `NB_PLACES`. Chaque voiture fait un `sem_wait()` avant d'entrer et un `sem_post()` en partant. Le mutex `mutex_affichage` protège les écritures dans le terminal et l'accès au tableau `places[]`.

```c
// on attend qu'une place se libere
ecrire_log(voiture_id, "ATTENTE", "");
nb_en_attente++;
sem_wait(&places_dispo);
nb_en_attente--;

// section critique : trouver et reserver une place
pthread_mutex_lock(&mutex_affichage);
for (i = 0; i < NB_PLACES; i++) {
    if (places[i] == 0) {
        places[i] = 1;
        ma_place = i;
        break;
    }
}
pthread_mutex_unlock(&mutex_affichage);
```

Pourquoi il n'y a pas d'interblocage : l'ordre d'acquisition est toujours le même — sémaphore d'abord, mutex ensuite. On ne prend jamais le mutex avant le sémaphore, donc pas de cycle possible.

**Tableau des places** — `int places[NB_PLACES]` dans `parking.c` permet de savoir quelle place précise est libre. Quand une voiture entre elle cherche la première case à 0, la met à 1 et stocke l'index. En partant elle remet sa case à 0.

**Logger** — `ecrire_log()` dans `logger.c` ouvre le fichier en mode `"a"` (append), écrit une ligne avec le timestamp via `time()` et `strftime()`, puis referme. Un bug avait été introduit au départ avec le mode `"w"` qui écrasait le fichier à chaque run — corrigé rapidement.

Format du log :
```
[2026-05-29 08:30:01] [voiture 03] [ARRIVEE] 
[2026-05-29 08:30:01] [voiture 03] [ATTENTE] 
[2026-05-29 08:30:02] [voiture 03] [GAREE] place 2
[2026-05-29 08:30:05] [voiture 03] [DEPART] 
```

**Difficulté rencontrée :** au début le `sem_init()` était appelé après `pthread_create()`. Les voitures entraient toutes en même temps sans se bloquer parce que le sémaphore n'était pas encore initialisé quand les threads démarraient. Corrigé en déplaçant `init_parking()` avant la boucle de création des threads.

### 5.3 Semaine 3 — Interface ncurses (en cours)

L'interface ncurses a été démarrée avec `affichage.c`. Un thread dédié appelle `afficher_parking()` toutes les 100ms via `usleep(100000)`. La fonction affiche la grille des places avec `[  V  ]` pour occupée et `[     ]` pour libre, plus le compteur de voitures en attente.

```c
void* thread_affichage(void* arg) {
    while (simulation_active) {
        afficher_parking();
        usleep(100000);
        int ch = getch();
        if (ch == 'q') {
            simulation_active = 0;
        }
    }
    return NULL;
}
```

Reste à faire en semaine 3 : panneau droit avec les dernières lignes du log, mode attente active, touche `s` pour changer de stratégie, et les statistiques comparatives dans `stats.c`.

---

## 6. Tests et Résultats

*(Cette section sera complétée en semaine 4 après finalisation de toutes les fonctionnalités)*

---

## 7. Difficultés rencontrées

**sem_init après pthread_create** — Les threads démarraient avant que le sémaphore soit initialisé, ce qui faisait entrer toutes les voitures en même temps. Résolu en appelant `init_parking()` avant la boucle `pthread_create`.

**Fichier log écrasé à chaque run** — Le fichier était ouvert en mode `"w"` au lieu de `"a"`, donc chaque nouvelle simulation effaçait les logs précédents. Corrigé en changeant le mode d'ouverture.

**Warning sur sleep()** — `<unistd.h>` manquait dans `voiture.h`, le compilateur avertissait d'une déclaration implicite de `sleep()`. Simple oubli d'include corrigé en semaine 1.

*(D'autres difficultés seront ajoutées au fil de l'implémentation)*

---

## 8. Conclusion

Ce rapport documente l'avancement du projet semaine par semaine. Les fonctionnalités minimales demandées sont implémentées : simulation des voitures par threads, gestion de la concurrence par sémaphore et mutex, journalisation dans un fichier log.

Les fonctionnalités avancées (interface ncurses, attente active, statistiques comparatives) sont en cours d'implémentation en semaine 3. La semaine 4 sera consacrée aux paramètres CLI, au nettoyage du code et à la finalisation de ce rapport.

Dépôt GitHub : https://github.com/younes692/parking-simulator-c
