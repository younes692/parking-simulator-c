# Simulateur de Parking

Projet universitaire — Systemes d'exploitation / Programmation concurrente
Auteur : Younes MOUMOU

## Description

Simulation d'un parking a acces concurrent. Plusieurs threads representent des voitures qui arrivent, attendent une place libre, se garent, puis repartent. Le projet compare deux strategies de synchronisation : semaphores POSIX et attente active (busy-waiting).

## Prerequis

- Linux (GCC + make)
- libncurses : `sudo apt-get install libncurses5-dev`
- libpthread (inclus par defaut sous Linux)

## Compilation

```bash
make          # compile et genere l executalbe ./parking
make clean    # supprime les fichiers compiles
```

## Lancer le programme

```bash
./parking
```

ou avec des arguments pour choisir le nombre de places et de voitures :

```bash
./parking 3 8
```

premier argument = nb places, deuxieme = nb voitures
max 5 places et 10 voitures

## Touches

- `q` : quitter

## Ce que ca fait

Au demarrage un menu permet de choisir la strategie de synchronisation. Le programme simule ensuite un parking avec plusieurs voitures qui arrivent en meme temps. Quand le parking est plein les voitures attendent dehors.

Deux modes disponibles :
- **semaphore** : le thread dort en attendant une place
- **attente active** : le thread tourne en boucle en verifiant regulierement si une place est libre

A la fin les stats s affichent avec le temps moyen d attente et le taux d utilisation du parking.

Les logs sont dans `logs/parking.log`
