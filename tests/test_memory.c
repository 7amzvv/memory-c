#define main MemoryApplicationMain
#include "../main.c"
#undef main

#include <assert.h>
#include <stdio.h>

void TesterInitialisation(void)
{
    Partie partie = {0};
    assert(!InitialiserPartie(NULL, 8));
    assert(!InitialiserPartie(&partie, 0));
    assert(!InitialiserPartie(&partie, 10));
    assert(!InitialiserPartie(&partie, MAX_CARTES + 2));

    for (int mode = 0; mode < NOMBRE_MODES; mode++) {
        for (int essai = 0; essai < 100; essai++) {
            assert(InitialiserPartie(&partie, MODES[mode]));
            assert(partie.score == 0 && partie.coups == 0);
            assert(partie.paires_trouvees == 0);
            assert(partie.premiere_carte == AUCUNE_CARTE);
            assert(partie.deuxieme_carte == AUCUNE_CARTE);
            assert(partie.nombre_lignes * partie.nombre_colonnes == MODES[mode]);
            int frequences[MAX_PAIRES] = {0};
            for (int i = 0; i < partie.nombre_cartes; i++) {
                assert(partie.valeurs[i] >= 0 && partie.valeurs[i] < MODES[mode] / 2);
                frequences[partie.valeurs[i]]++;
                assert(!partie.visibles[i] && !partie.trouvees[i]);
                Rectangle zone = ZoneCarte(&partie, i);
                assert(zone.x >= 0 && zone.x + zone.width <= LARGEUR_FENETRE);
                assert(zone.y >= HAUT_ZONE_GRILLE);
                assert(zone.y + zone.height <= HAUT_ZONE_GRILLE + HAUTEUR_ZONE_GRILLE);
                Vector2 centre = {zone.x + zone.width / 2, zone.y + zone.height / 2};
                assert(CarteSousSouris(&partie, centre) == i);
            }
            for (int i = 0; i < MODES[mode] / 2; i++) assert(frequences[i] == 2);
            assert(CarteSousSouris(&partie, (Vector2){0, 0}) == AUCUNE_CARTE);
        }
    }
}

void TesterRetournement(void)
{
    Partie partie;
    assert(InitialiserPartie(&partie, 8));
    for (int i = 0; i < partie.nombre_cartes; i++) partie.valeurs[i] = i / 2;

    assert(!RetournerCarte(&partie, -1));
    assert(!RetournerCarte(&partie, partie.nombre_cartes));
    assert(RetournerCarte(&partie, 0));
    assert(!RetournerCarte(&partie, 0));
    assert(partie.coups == 0);
    assert(RetournerCarte(&partie, 2));
    assert(partie.coups == 1 && partie.score == 0);
    assert(!RetournerCarte(&partie, 3));
    MettreAJourPartie(&partie, -1.0f);
    assert(partie.attente == 0);
    MettreAJourPartie(&partie, DELAI_RETOURNEMENT / 2);
    assert(partie.visibles[0] && partie.visibles[2]);
    MettreAJourPartie(&partie, DELAI_RETOURNEMENT);
    assert(!partie.visibles[0] && !partie.visibles[2]);
    assert(partie.premiere_carte == AUCUNE_CARTE);
    assert(partie.deuxieme_carte == AUCUNE_CARTE);

    assert(RetournerCarte(&partie, 0));
    assert(RetournerCarte(&partie, 1));
    assert(partie.score == POINTS_PAR_PAIRE && partie.coups == 2);
    assert(partie.trouvees[0] && partie.trouvees[1]);
    assert(!RetournerCarte(&partie, 0));
    MettreAJourPartie(&partie, DELAI_RETOURNEMENT * 2);
    assert(partie.visibles[0] && partie.visibles[1]);
    assert(partie.score == POINTS_PAR_PAIRE);
}

void TesterVictoireEtNavigation(void)
{
    for (int mode = 0; mode < NOMBRE_MODES; mode++) {
        Partie partie;
        Ecran ecran = ACCUEIL;
        Rectangle zone_mode = ZoneMode(mode);
        Vector2 centre_mode = {zone_mode.x + 10, zone_mode.y + 10};
        GererClicAccueil(&partie, &ecran, centre_mode);
        assert(ecran == PARTIE && partie.nombre_cartes == MODES[mode]);

        for (int couleur = 0; couleur < partie.nombre_cartes / 2; couleur++) {
            for (int i = 0; i < partie.nombre_cartes; i++) {
                if (partie.valeurs[i] == couleur) assert(RetournerCarte(&partie, i));
            }
        }
        assert(PartieTerminee(&partie));
        assert(partie.score == MODES[mode] / 2 * POINTS_PAR_PAIRE);
        assert(partie.coups == MODES[mode] / 2);
        assert(!RetournerCarte(&partie, 0));

        ecran = FIN_PARTIE;
        GererClicFinPartie(&partie, &ecran, (Vector2){0, 0});
        assert(ecran == FIN_PARTIE);
        Vector2 rejouer = {BOUTON_REJOUER.x + 10, BOUTON_REJOUER.y + 10};
        GererClicFinPartie(&partie, &ecran, rejouer);
        assert(ecran == PARTIE && partie.nombre_cartes == MODES[mode]);
        assert(partie.score == 0 && partie.coups == 0 && !PartieTerminee(&partie));
        for (int i = 0; i < partie.nombre_cartes; i++)
            assert(!partie.visibles[i] && !partie.trouvees[i]);

        ecran = FIN_PARTIE;
        Vector2 accueil = {BOUTON_ACCUEIL.x + 10, BOUTON_ACCUEIL.y + 10};
        GererClicFinPartie(&partie, &ecran, accueil);
        assert(ecran == ACCUEIL);
    }
}

int main(void)
{
    TesterInitialisation();
    TesterRetournement();
    TesterVictoireEtNavigation();
    puts("OK : initialisation, melange, grilles, paires, attente, score, victoire et navigation.");
    return 0;
}
