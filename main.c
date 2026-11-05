#include "raylib.h"

#define LARGEUR_FENETRE 1000
#define HAUTEUR_FENETRE 760

int main(void)
{
    int ecran = 0;
    int nombre_cartes = 0;
    int nombre_colonnes = 0;
    int nombre_lignes = 0;
    int largeur_carte = 104;
    int hauteur_carte = 96;
    int espace_cartes = 16;
    int score = 0;

    Color fond = {246, 247, 250, 255};
    Color panneau = {224, 232, 242, 255};
    Color couleur_carte = {74, 116, 170, 255};
    Color accent = {40, 73, 112, 255};

    Rectangle bouton_8 = {155, 320, 210, 130};
    Rectangle bouton_16 = {395, 320, 210, 130};
    Rectangle bouton_32 = {635, 320, 210, 130};
    Rectangle bouton_menu = {820, 35, 140, 45};

    InitWindow(LARGEUR_FENETRE, HAUTEUR_FENETRE, "Memory");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 souris = GetMousePosition();
        bool clic = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (ecran == 0) {
            if (clic) {
                if (CheckCollisionPointRec(souris, bouton_8)) {
                    nombre_cartes = 8;
                    nombre_colonnes = 4;
                    nombre_lignes = 2;
                    score = 0;
                    ecran = 1;
                } else if (CheckCollisionPointRec(souris, bouton_16)) {
                    nombre_cartes = 16;
                    nombre_colonnes = 4;
                    nombre_lignes = 4;
                    score = 0;
                    ecran = 1;
                } else if (CheckCollisionPointRec(souris, bouton_32)) {
                    nombre_cartes = 32;
                    nombre_colonnes = 8;
                    nombre_lignes = 4;
                    score = 0;
                    ecran = 1;
                }
            }
        } else {
            if (clic && CheckCollisionPointRec(souris, bouton_menu)) {
                ecran = 0;
            }
        }

        BeginDrawing();
        ClearBackground(fond);

        if (ecran == 0) {
            int largeur_titre = MeasureText("Memory", 42);
            DrawText("Memory", (LARGEUR_FENETRE - largeur_titre) / 2,
                     170, 42, accent);

            const char *consigne = "Choisis le nombre de cartes";
            int largeur_consigne = MeasureText(consigne, 26);
            DrawText(consigne, (LARGEUR_FENETRE - largeur_consigne) / 2,
                     235, 26, DARKGRAY);

            Color couleur_bouton_8 = panneau;
            Color couleur_bouton_16 = panneau;
            Color couleur_bouton_32 = panneau;

            if (CheckCollisionPointRec(souris, bouton_8)) {
                couleur_bouton_8 = (Color){198, 216, 237, 255};
            }
            if (CheckCollisionPointRec(souris, bouton_16)) {
                couleur_bouton_16 = (Color){198, 216, 237, 255};
            }
            if (CheckCollisionPointRec(souris, bouton_32)) {
                couleur_bouton_32 = (Color){198, 216, 237, 255};
            }

            DrawRectangleRec(bouton_8, couleur_bouton_8);
            DrawRectangleRec(bouton_16, couleur_bouton_16);
            DrawRectangleRec(bouton_32, couleur_bouton_32);

            DrawText("8", 155 + (210 - MeasureText("8", 48)) / 2,
                     345, 48, accent);
            DrawText("16", 395 + (210 - MeasureText("16", 48)) / 2,
                     345, 48, accent);
            DrawText("32", 635 + (210 - MeasureText("32", 48)) / 2,
                     345, 48, accent);

            DrawText("4 paires", 155 + (210 - MeasureText("4 paires", 22)) / 2,
                     410, 22, DARKGRAY);
            DrawText("8 paires", 395 + (210 - MeasureText("8 paires", 22)) / 2,
                     410, 22, DARKGRAY);
            DrawText("16 paires", 635 + (210 - MeasureText("16 paires", 22)) / 2,
                     410, 22, DARKGRAY);

            DrawText("Hamza Fadli", 30, 720, 18, GRAY);
            DrawText("Echap : quitter", 810, 720, 18, GRAY);
        } else {
            DrawText("Memory", 40, 36, 30, accent);
            DrawText(TextFormat("%d cartes - %d paires",
                     nombre_cartes, nombre_cartes / 2), 40, 100, 24, DARKGRAY);

            Color couleur_menu = panneau;
            if (CheckCollisionPointRec(souris, bouton_menu)) {
                couleur_menu = (Color){198, 216, 237, 255};
            }
            DrawRectangleRec(bouton_menu, couleur_menu);
            DrawText("Menu", 820 + (140 - MeasureText("Menu", 22)) / 2,
                     47, 22, DARKGRAY);

            int largeur_grille = nombre_colonnes * largeur_carte;
            largeur_grille = largeur_grille + (nombre_colonnes - 1) * espace_cartes;
            int hauteur_grille = nombre_lignes * hauteur_carte;
            hauteur_grille = hauteur_grille + (nombre_lignes - 1) * espace_cartes;

            int debut_x = (LARGEUR_FENETRE - largeur_grille) / 2;
            int debut_y = 170 + (440 - hauteur_grille) / 2;

            for (int ligne = 0; ligne < nombre_lignes; ligne++) {
                for (int colonne = 0; colonne < nombre_colonnes; colonne++) {
                    int position_x = debut_x + colonne * (largeur_carte + espace_cartes);
                    int position_y = debut_y + ligne * (hauteur_carte + espace_cartes);

                    Rectangle carte;
                    carte.x = (float)position_x;
                    carte.y = (float)position_y;
                    carte.width = (float)largeur_carte;
                    carte.height = (float)hauteur_carte;

                    DrawRectangleRec(carte, couleur_carte);
                    int texte_x = position_x + (largeur_carte - MeasureText("?", 38)) / 2;
                    int texte_y = position_y + (hauteur_carte - 38) / 2;
                    DrawText("?", texte_x, texte_y, 38, RAYWHITE);
                }
            }

            const char *texte_score = TextFormat("Score : %d", score);
            int largeur_score = MeasureText(texte_score, 26);
            DrawText(texte_score, (LARGEUR_FENETRE - largeur_score) / 2,
                     670, 26, accent);
            DrawText("Hamza Fadli", 40, 730, 16, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
