# Memory en C

Un jeu de mémoire avec une interface graphique simple, développé en **C** avec **raylib**.

## Présentation

Le joueur retourne deux cartes pour retrouver les paires de couleurs. Le jeu propose trois formats :

| Cartes | Paires | Grille |
| --- | --- | --- |
| 8 | 4 | 4 × 2 |
| 16 | 8 | 4 × 4 |
| 32 | 16 | 8 × 4 |

Les cartes sont mélangées à chaque nouvelle partie.

## Règles et fonctionnalités

- Cliquer sur deux cartes pour révéler leurs couleurs.
- Une paire trouvée reste visible et rapporte **10 points**.
- Deux cartes différentes se cachent après **0,9 seconde**.
- Un coup correspond au choix de deux cartes différentes, que la paire soit correcte ou non.
- Le score et le nombre de coups sont affichés sous la grille.
- Après la victoire, choisir **Rejouer** ou **Accueil**.
- Rejouer conserve le nombre de cartes, mélange les paires et remet les compteurs à zéro.

Une seule paire peut être examinée à la fois. Les cartes déjà trouvées ne peuvent plus être sélectionnées.

## Commandes

| Commande | Action |
| --- | --- |
| Clic gauche | Choisir un mode, retourner une carte ou utiliser un bouton |
| Menu | Revenir au choix du nombre de cartes pendant une partie |
| Rejouer | Relancer le même mode après une victoire |
| Accueil | Choisir un autre mode après une victoire |
| Échap | Fermer le jeu |

## Installation et lancement

La configuration fournie est prévue pour **Windows 64 bits**, avec **GCC MinGW-w64** accessible dans le PATH. VS Code est facultatif.

Télécharger ou cloner le dépôt :

```powershell
git clone https://github.com/7amzvv/memory-c.git
cd memory-c
```

Dans PowerShell, installer raylib :

```powershell
.\setup-raylib.ps1
```

Ce script télécharge raylib 6.2 depuis son dépôt officiel, vérifie l'empreinte de l'archive et place la bibliothèque dans `lib/raylib`.

Compiler puis lancer :

```powershell
gcc -std=c11 -Wall -Wextra -Werror -pedantic main.c -Ilib/raylib/include -Llib/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -o memory.exe
.\memory.exe
```

Fermer le jeu avant de recompiler. Si Windows bloque un fichier, consulter le message de sécurité : le projet ne modifie aucune protection du système.

### Avec VS Code

Ouvrir le dossier du projet :

- **Ctrl + Maj + B** : compiler le jeu.
- **Terminal → Exécuter la tâche → Jouer** : compiler et lancer.
- **Terminal → Exécuter la tâche → Tester Memory** : compiler et exécuter les tests.

## Organisation du code

```text
memory-c/
├── main.c
├── tests/
│   └── test_memory.c
├── .vscode/
│   └── tasks.json
├── setup-raylib.ps1
└── .gitignore
```

La structure `Partie` regroupe les cartes et les compteurs. Les fonctions principales sont :

- `InitialiserPartie` : préparer et mélanger les paires.
- `RetournerCarte` : vérifier un clic et comparer les cartes.
- `MettreAJourPartie` : gérer le délai des paires incorrectes.
- `DessinerAccueil`, `DessinerPartie` et `DessinerFinPartie` : afficher les écrans.

## Tests

Les tests vérifient les trois modes, la présence de deux cartes par couleur, les dimensions des grilles, les clics invalides, le délai, le score, la victoire et la navigation de fin de partie. Ils ne remplacent pas une vérification visuelle de l'interface.

Pour les lancer sans VS Code :

```powershell
gcc -std=c11 -Wall -Wextra -Werror -pedantic tests/test_memory.c -Ilib/raylib/include -Llib/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -o test_memory.exe
.\test_memory.exe
```

## Évolutions possibles

- Remplacer les couleurs par des images.
- Ajouter un chronomètre.
- Proposer des thèmes de cartes.

## Crédits

**Hamza FADLI**

Bibliothèque graphique : [raylib](https://github.com/raysan5/raylib), créée par Ramon Santamaria et ses contributeurs. Ses fichiers conservent leurs mentions de licence.
