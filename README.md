# parking simulator

projet de programmation systeme - semestre 2

## comment compiler

```
make
```

## comment lancer

```
./parking
```

ou avec des arguments pour choisir le nombre de places et de voitures :

```
./parking 3 8
```

premier argument = nb places, deuxieme = nb voitures
max 5 places et 10 voitures

## touches

- `s` : changer de strategie (semaphore / attente active)
- `q` : quitter

## ce que ca fait

le programme simule un parking avec plusieurs voitures qui arrivent en meme temps.
quand le parking est plein les voitures attendent dehors.

deux modes de synchronisation :
- **semaphore** : le thread dort en attendant une place (efficient)
- **attente active** : le thread tourne en boucle (gaspille le CPU)

a la fin on voit les stats comparant les deux strategies.

les logs sont dans `logs/parking.log`
