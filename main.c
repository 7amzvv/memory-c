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
    int coups = 0;
    int paires_trouvees = 0;
    int valeurs[32] = {0};
    bool visibles[32] = {false};
    bool trouvees[32] = {false};
    int premiere_carte = -1;
    int deuxieme_carte = -1;
    float attente = 0.0f;

    Color couleurs[16] = {
        {220, 65, 65, 255}, {45, 160, 85, 255},
        {55, 110, 220, 255}, {245, 200, 45, 255},
        {150, 75, 200, 255}, {245, 135, 40, 255},
        {235, 110, 170, 255}, {40, 190, 195, 255},
        {115, 70, 40, 255}, {40, 55, 75, 255},
        {145, 190, 50, 255}, {130, 35, 70, 255},
        {175, 145, 110, 255}, {120, 140, 155, 255},
        {35, 95, 75, 255}, {125, 115, 230, 255}
    };

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
        int ancien_ecran = ecran;

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


        if (ancien_ecran == 0 && ecran == 1) {
            score = 0;
            coups = 0;
            paires_trouvees = 0;
            premiere_carte = -1;
            deuxieme_carte = -1;
            attente = 0.0f;

            for (int i = 0; i < nombre_cartes; i++) {
                valeurs[i] = i / 2;
                visibles[i] = false;
                trouvees[i] = false;
            }

            for (int i = nombre_cartes - 1; i > 0; i--) {
                int hasard = GetRandomValue(0, i);
                int temporaire = valeurs[i];
                valeurs[i] = valeurs[hasard];
                valeurs[hasard] = temporaire;
            }

            clic = false;
        }

        int largeur_grille = nombre_colonnes * largeur_carte;
        largeur_grille = largeur_grille + (nombre_colonnes - 1) * espace_cartes;
        int hauteur_grille = nombre_lignes * hauteur_carte;
        hauteur_grille = hauteur_grille + (nombre_lignes - 1) * espace_cartes;
        int debut_x = (LARGEUR_FENETRE - largeur_grille) / 2;
        int debut_y = 170 + (440 - hauteur_grille) / 2;

        if (ecran == 1) {
            if (deuxieme_carte != -1) {
                attente = attente + GetFrameTime();

                if (attente >= 0.9f) {
                    visibles[premiere_carte] = false;
                    visibles[deuxieme_carte] = false;
                    premiere_carte = -1;
                    deuxieme_carte = -1;
                    attente = 0.0f;
                }

                clic = false;
            }

            if (clic && deuxieme_carte == -1) {
                for (int i = 0; i < nombre_cartes; i++) {
                    int ligne = i / nombre_colonnes;
                    int colonne = i % nombre_colonnes;
                    Rectangle zone;
                    zone.x = (float)(debut_x + colonne * (largeur_carte + espace_cartes));
                    zone.y = (float)(debut_y + ligne * (hauteur_carte + espace_cartes));
                    zone.width = (float)largeur_carte;
                    zone.height = (float)hauteur_carte;

                    if (CheckCollisionPointRec(souris, zone) &&
                        !visibles[i] && !trouvees[i]) {
                        visibles[i] = true;

                        if (premiere_carte == -1) {
                            premiere_carte = i;
                        } else {
                            deuxieme_carte = i;
                            coups = coups + 1;

                            if (valeurs[premiere_carte] == valeurs[deuxieme_carte]) {
                                trouvees[premiere_carte] = true;
                                trouvees[deuxieme_carte] = true;
                                paires_trouvees = paires_trouvees + 1;
                                score = score + 10;
                                premiere_carte = -1;
                                deuxieme_carte = -1;
                            } else {
                                attente = 0.0f;
                            }
                        }

                        break;
                    }
                }
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

            for (int ligne = 0; ligne < nombre_lignes; ligne++) {
                for (int colonne = 0; colonne < nombre_colonnes; colonne++) {
                    int position_x = debut_x + colonne * (largeur_carte + espace_cartes);
                    int position_y = debut_y + ligne * (hauteur_carte + espace_cartes);

                    Rectangle carte;
                    carte.x = (float)position_x;
                    carte.y = (float)position_y;
                    carte.width = (float)largeur_carte;
                    carte.height = (float)hauteur_carte;

                    int numero = ligne * nombre_colonnes + colonne;

                    if (visibles[numero] || trouvees[numero]) {
                        DrawRectangleRec(carte, couleurs[valeurs[numero]]);

                        if (trouvees[numero]) {
                            DrawRectangleLinesEx(carte, 3, accent);
                        }
                    } else {
                        DrawRectangleRec(carte, couleur_carte);
                        int texte_x = position_x + (largeur_carte - MeasureText("?", 38)) / 2;
                        int texte_y = position_y + (hauteur_carte - 38) / 2;
                        DrawText("?", texte_x, texte_y, 38, RAYWHITE);
                    }
                }
            }

            const char *texte_score = TextFormat("Score : %d   Coups : %d", score, coups);
            int largeur_score = MeasureText(texte_score, 26);
            DrawText(texte_score, (LARGEUR_FENETRE - largeur_score) / 2,
                     670, 26, accent);
            if (paires_trouvees == nombre_cartes / 2) {
                DrawText("Bravo ! Toutes les paires sont trouvees.", 255, 130, 22, accent);
            }

            DrawText("Hamza Fadli", 40, 730, 16, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
