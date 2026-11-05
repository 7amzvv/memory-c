#include "raylib.h"
#include <stddef.h>

#define LARGEUR_FENETRE 1000
#define HAUTEUR_FENETRE 760
#define IMAGES_PAR_SECONDE 60
#define MAX_CARTES 32
#define MAX_PAIRES (MAX_CARTES / 2)
#define AUCUNE_CARTE (-1)
#define POINTS_PAR_PAIRE 10
#define DELAI_RETOURNEMENT 0.9f

#define LARGEUR_CARTE 104
#define HAUTEUR_CARTE 96
#define ESPACE_CARTES 16
#define HAUT_ZONE_GRILLE 170
#define HAUTEUR_ZONE_GRILLE 440
#define COLONNES_PETITE_GRILLE 4
#define COLONNES_GRANDE_GRILLE 8
#define BORDURE_PAIRE 3

#define DEBUT_MODES_X 155
#define DEBUT_MODES_Y 320
#define LARGEUR_MODE 210
#define HAUTEUR_MODE 130
#define ESPACE_MODES 30
#define TAILLE_TITRE 42
#define TAILLE_TEXTE 22
#define TAILLE_NOMBRE 48
#define TAILLE_QUESTION 38
#define TAILLE_SCORE 26

/* Les trois ecrans du jeu. */
typedef enum {
    ACCUEIL,
    PARTIE,
    FIN_PARTIE
} Ecran;

/* Les donnees d'une partie sont regroupees dans cette structure. */
typedef struct {
    int nombre_cartes;
    int nombre_colonnes;
    int nombre_lignes;
    int valeurs[MAX_CARTES];
    bool visibles[MAX_CARTES];
    bool trouvees[MAX_CARTES];
    int premiere_carte;
    int deuxieme_carte;
    int score;
    int coups;
    int paires_trouvees;
    float attente;
} Partie;

static const int MODES[] = {8, 16, MAX_CARTES};
#define NOMBRE_MODES ((int)(sizeof MODES / sizeof MODES[0]))

static const Color FOND = {246, 247, 250, 255};
static const Color PANNEAU = {224, 232, 242, 255};
static const Color SURVOL = {198, 216, 237, 255};
static const Color DOS_CARTE = {74, 116, 170, 255};
static const Color ACCENT = {40, 73, 112, 255};
static const Color VOILE = {30, 40, 55, 150};

static const Rectangle BOUTON_MENU = {820, 35, 140, 45};
static const Rectangle BOUTON_REJOUER = {300, 400, 180, 50};
static const Rectangle BOUTON_ACCUEIL = {520, 400, 180, 50};
static const Rectangle FENETRE_FIN = {250, 230, 500, 270};

static const Color COULEURS[] = {
    {220, 65, 65, 255}, {45, 160, 85, 255},
    {55, 110, 220, 255}, {245, 200, 45, 255},
    {150, 75, 200, 255}, {245, 135, 40, 255},
    {235, 110, 170, 255}, {40, 190, 195, 255},
    {115, 70, 40, 255}, {40, 55, 75, 255},
    {145, 190, 50, 255}, {130, 35, 70, 255},
    {175, 145, 110, 255}, {120, 140, 155, 255},
    {35, 95, 75, 255}, {125, 115, 230, 255}
};

_Static_assert(MAX_CARTES % 2 == 0, "Le nombre maximal de cartes doit etre pair.");
_Static_assert(sizeof COULEURS / sizeof COULEURS[0] == MAX_PAIRES,
               "Il faut une couleur par paire.");

bool NombreCartesValide(int nombre_cartes)
{
    for (int i = 0; i < NOMBRE_MODES; i++) {
        if (nombre_cartes == MODES[i]) {
            return true;
        }
    }
    return false;
}

/* Remet la partie a zero et prepare deux cartes par couleur. */
bool InitialiserPartie(Partie *partie, int nombre_cartes)
{
    if (partie == NULL || !NombreCartesValide(nombre_cartes)) {
        return false;
    }

    *partie = (Partie){0};
    partie->nombre_cartes = nombre_cartes;
    partie->nombre_colonnes = COLONNES_PETITE_GRILLE;

    if (nombre_cartes == MAX_CARTES) {
        partie->nombre_colonnes = COLONNES_GRANDE_GRILLE;
    }

    partie->nombre_lignes = nombre_cartes / partie->nombre_colonnes;
    partie->premiere_carte = AUCUNE_CARTE;
    partie->deuxieme_carte = AUCUNE_CARTE;

    for (int i = 0; i < nombre_cartes; i++) {
        partie->valeurs[i] = i / 2;
    }

    /* Melange de Fisher-Yates. */
    for (int i = nombre_cartes - 1; i > 0; i--) {
        int hasard = GetRandomValue(0, i);
        int temporaire = partie->valeurs[i];
        partie->valeurs[i] = partie->valeurs[hasard];
        partie->valeurs[hasard] = temporaire;
    }

    return true;
}

bool PartieTerminee(const Partie *partie)
{
    return partie->nombre_cartes > 0 &&
           partie->paires_trouvees == partie->nombre_cartes / 2;
}

/* Ignore les clics invalides et compare les deux cartes choisies. */
bool RetournerCarte(Partie *partie, int numero)
{
    if (numero < 0 || numero >= partie->nombre_cartes ||
        PartieTerminee(partie) ||
        partie->deuxieme_carte != AUCUNE_CARTE ||
        partie->visibles[numero] || partie->trouvees[numero]) {
        return false;
    }

    partie->visibles[numero] = true;

    if (partie->premiere_carte == AUCUNE_CARTE) {
        partie->premiere_carte = numero;
        return true;
    }

    partie->deuxieme_carte = numero;
    partie->coups++;
    int premiere = partie->premiere_carte;
    int deuxieme = partie->deuxieme_carte;

    if (partie->valeurs[premiere] == partie->valeurs[deuxieme]) {
        partie->trouvees[premiere] = true;
        partie->trouvees[deuxieme] = true;
        partie->paires_trouvees++;
        partie->score += POINTS_PAR_PAIRE;
        partie->premiere_carte = AUCUNE_CARTE;
        partie->deuxieme_carte = AUCUNE_CARTE;
    } else {
        partie->attente = 0.0f;
    }

    return true;
}

/* Laisse les cartes differentes visibles avant de les cacher. */
void MettreAJourPartie(Partie *partie, float temps_ecoule)
{
    if (partie->deuxieme_carte == AUCUNE_CARTE || temps_ecoule <= 0.0f) {
        return;
    }

    partie->attente += temps_ecoule;

    if (partie->attente >= DELAI_RETOURNEMENT) {
        partie->visibles[partie->premiere_carte] = false;
        partie->visibles[partie->deuxieme_carte] = false;
        partie->premiere_carte = AUCUNE_CARTE;
        partie->deuxieme_carte = AUCUNE_CARTE;
        partie->attente = 0.0f;
    }
}

Rectangle ZoneMode(int numero)
{
    return (Rectangle){
        DEBUT_MODES_X + numero * (LARGEUR_MODE + ESPACE_MODES),
        DEBUT_MODES_Y, LARGEUR_MODE, HAUTEUR_MODE
    };
}

Rectangle ZoneCarte(const Partie *partie, int numero)
{
    int largeur_grille = partie->nombre_colonnes * LARGEUR_CARTE;
    largeur_grille += (partie->nombre_colonnes - 1) * ESPACE_CARTES;
    int hauteur_grille = partie->nombre_lignes * HAUTEUR_CARTE;
    hauteur_grille += (partie->nombre_lignes - 1) * ESPACE_CARTES;
    int debut_x = (LARGEUR_FENETRE - largeur_grille) / 2;
    int debut_y = HAUT_ZONE_GRILLE + (HAUTEUR_ZONE_GRILLE - hauteur_grille) / 2;

    int ligne = numero / partie->nombre_colonnes;
    int colonne = numero % partie->nombre_colonnes;

    return (Rectangle){
        debut_x + colonne * (LARGEUR_CARTE + ESPACE_CARTES),
        debut_y + ligne * (HAUTEUR_CARTE + ESPACE_CARTES),
        LARGEUR_CARTE, HAUTEUR_CARTE
    };
}

int CarteSousSouris(const Partie *partie, Vector2 souris)
{
    for (int i = 0; i < partie->nombre_cartes; i++) {
        if (CheckCollisionPointRec(souris, ZoneCarte(partie, i))) {
            return i;
        }
    }
    return AUCUNE_CARTE;
}

/* Fonctions d'affichage partagees par les ecrans. */
void DessinerTexteCentre(const char *texte, Rectangle zone, int taille, Color couleur)
{
    int x = (int)(zone.x + (zone.width - MeasureText(texte, taille)) / 2);
    int y = (int)(zone.y + (zone.height - taille) / 2);
    DrawText(texte, x, y, taille, couleur);
}

void DessinerBouton(Rectangle zone, const char *texte, Vector2 souris)
{
    Color couleur = PANNEAU;
    if (CheckCollisionPointRec(souris, zone)) {
        couleur = SURVOL;
    }
    DrawRectangleRec(zone, couleur);
    DessinerTexteCentre(texte, zone, TAILLE_TEXTE, ACCENT);
}

void DessinerAccueil(Vector2 souris)
{
    DessinerTexteCentre("Memory",
                       (Rectangle){0, 170, LARGEUR_FENETRE, TAILLE_TITRE},
                       TAILLE_TITRE, ACCENT);
    DessinerTexteCentre("Choisis le nombre de cartes",
                       (Rectangle){0, 235, LARGEUR_FENETRE, TAILLE_SCORE},
                       TAILLE_SCORE, DARKGRAY);

    for (int i = 0; i < NOMBRE_MODES; i++) {
        Rectangle zone = ZoneMode(i);
        Color couleur = PANNEAU;
        if (CheckCollisionPointRec(souris, zone)) {
            couleur = SURVOL;
        }
        DrawRectangleRec(zone, couleur);

        Rectangle zone_nombre = {zone.x, zone.y + 25, zone.width, TAILLE_NOMBRE};
        DessinerTexteCentre(TextFormat("%d", MODES[i]), zone_nombre,
                           TAILLE_NOMBRE, ACCENT);
        Rectangle zone_paires = {zone.x, zone.y + 90, zone.width, TAILLE_TEXTE};
        DessinerTexteCentre(TextFormat("%d paires", MODES[i] / 2), zone_paires,
                           TAILLE_TEXTE, DARKGRAY);
    }

    DrawText("Hamza Fadli", 30, 720, 18, GRAY);
    DrawText("Echap : quitter", 810, 720, 18, GRAY);
}

void DessinerPartie(const Partie *partie, Vector2 souris)
{
    DrawText("Memory", 40, 36, 30, ACCENT);
    DrawText(TextFormat("%d cartes - %d paires",
             partie->nombre_cartes, partie->nombre_cartes / 2),
             40, 100, 24, DARKGRAY);
    DessinerBouton(BOUTON_MENU, "Menu", souris);

    for (int i = 0; i < partie->nombre_cartes; i++) {
        Rectangle zone = ZoneCarte(partie, i);

        if (partie->visibles[i] || partie->trouvees[i]) {
            DrawRectangleRec(zone, COULEURS[partie->valeurs[i]]);
            if (partie->trouvees[i]) {
                DrawRectangleLinesEx(zone, BORDURE_PAIRE, ACCENT);
            }
        } else {
            DrawRectangleRec(zone, DOS_CARTE);
            DessinerTexteCentre("?", zone, TAILLE_QUESTION, RAYWHITE);
        }
    }

    DessinerTexteCentre(TextFormat("Score : %d   Coups : %d",
                       partie->score, partie->coups),
                       (Rectangle){0, 670, LARGEUR_FENETRE, TAILLE_SCORE},
                       TAILLE_SCORE, ACCENT);
    DrawText("Hamza Fadli", 40, 730, 16, GRAY);
}

void DessinerFinPartie(const Partie *partie, Vector2 souris)
{
    DrawRectangleRec((Rectangle){0, 0, LARGEUR_FENETRE, HAUTEUR_FENETRE}, VOILE);
    DrawRectangleRec(FENETRE_FIN, FOND);
    DrawRectangleLinesEx(FENETRE_FIN, 2, ACCENT);

    DessinerTexteCentre("Bravo ! Partie terminee",
                       (Rectangle){FENETRE_FIN.x, 260, FENETRE_FIN.width, 28},
                       28, ACCENT);
    DessinerTexteCentre(TextFormat("%d points - %d coups", partie->score, partie->coups),
                       (Rectangle){FENETRE_FIN.x, 310, FENETRE_FIN.width, 24},
                       24, DARKGRAY);
    DessinerTexteCentre("Que veux-tu faire ?",
                       (Rectangle){FENETRE_FIN.x, 355, FENETRE_FIN.width, TAILLE_TEXTE},
                       TAILLE_TEXTE, DARKGRAY);
    DessinerBouton(BOUTON_REJOUER, "Rejouer", souris);
    DessinerBouton(BOUTON_ACCUEIL, "Accueil", souris);
}

void GererClicAccueil(Partie *partie, Ecran *ecran, Vector2 souris)
{
    for (int i = 0; i < NOMBRE_MODES; i++) {
        if (CheckCollisionPointRec(souris, ZoneMode(i))) {
            if (InitialiserPartie(partie, MODES[i])) {
                *ecran = PARTIE;
            }
            return;
        }
    }
}

void GererClicFinPartie(Partie *partie, Ecran *ecran, Vector2 souris)
{
    if (CheckCollisionPointRec(souris, BOUTON_REJOUER)) {
        if (InitialiserPartie(partie, partie->nombre_cartes)) {
            *ecran = PARTIE;
        }
    } else if (CheckCollisionPointRec(souris, BOUTON_ACCUEIL)) {
        *ecran = ACCUEIL;
    }
}

int main(void)
{
    Partie partie = {0};
    Ecran ecran = ACCUEIL;

    InitWindow(LARGEUR_FENETRE, HAUTEUR_FENETRE, "Memory");
    SetTargetFPS(IMAGES_PAR_SECONDE);

    /* Traite les entrees, met a jour la partie puis dessine l'ecran. */
    while (!WindowShouldClose()) {
        Vector2 souris = GetMousePosition();
        bool clic = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (ecran == ACCUEIL) {
            if (clic) {
                GererClicAccueil(&partie, &ecran, souris);
            }
        } else if (ecran == PARTIE) {
            if (clic && CheckCollisionPointRec(souris, BOUTON_MENU)) {
                ecran = ACCUEIL;
            } else {
                bool attendait = partie.deuxieme_carte != AUCUNE_CARTE;
                MettreAJourPartie(&partie, GetFrameTime());
                if (clic && !attendait) {
                    RetournerCarte(&partie, CarteSousSouris(&partie, souris));
                }
                if (PartieTerminee(&partie)) {
                    ecran = FIN_PARTIE;
                }
            }
        } else if (ecran == FIN_PARTIE) {
            if (clic) {
                GererClicFinPartie(&partie, &ecran, souris);
            }
        }

        BeginDrawing();
        ClearBackground(FOND);
        if (ecran == ACCUEIL) {
            DessinerAccueil(souris);
        } else {
            DessinerPartie(&partie, souris);
            if (ecran == FIN_PARTIE) {
                DessinerFinPartie(&partie, souris);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
