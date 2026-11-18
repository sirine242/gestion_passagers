/*
 * main.c - programme de test (console) pour passager.c
 * Cree passager.txt avec quelques passagers puis affiche son contenu.
 */

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "passager.h"

static void remplir(Passager *p, char *cin, char *nom, char *prenom,
                    int j, int m, int a, char *sexe, char *nat,
                    char *tel, char *email, char *login)
{
    memset(p, 0, sizeof(*p));
    strcpy(p->cin, cin);
    strcpy(p->nom, nom);
    strcpy(p->prenom, prenom);
    p->naissance.jour = j;
    p->naissance.mois = m;
    p->naissance.annee = a;
    strcpy(p->sexe, sexe);
    strcpy(p->nationalite, nat);
    strcpy(p->tel, tel);
    strcpy(p->email, email);
    strcpy(p->login, login);
    p->points = 0;
    p->nb_vols = 0;
    p->total_depense = 0;
    p->bloque = 0;
    strcpy(p->motif, "aucun");
}

int main(void)
{
    Passager p, t[MAX_PASSAGERS];
    char erreur[200], nom[30], prenom[30], niveau[20];
    int n, i, r, pts, anc, nouv;

    /* 1. ajout de quelques passagers */
    Passager liste[3];
    remplir(&liste[0], "12345678", "Ben_Ali", "Ahmed", 14, 3, 2000, "Homme", "Tunisienne",
            "22123455", "ahmed@gmail.com", "ahmed");
    remplir(&liste[1], "87654321", "Trabelsi", "Sarra", 2, 7, 1994, "Femme", "Tunisienne",
            "55987654", "sarra@yahoo.fr", "sarra");
    remplir(&liste[2], "11992451", "Jhinaoui", "Sirine", 9, 5, 2002, "Femme", "Tunisienne",
            "25176190", "sirine.jhinaoui@esprit.tn", "sirine.jhinaoui");

    for (i = 0; i < 3; i++)
    {
        if (!controler_passager(liste[i], erreur))
        {
            printf("Saisie refusee pour %s : %s\n", liste[i].cin, erreur);
            continue;
        }
        r = ajouter_passager(liste[i]);
        if (r == 1)       printf("Ajoute : %s %s\n", liste[i].prenom, liste[i].nom);
        else if (r == -1) printf("CIN %s deja utilise\n", liste[i].cin);
        else if (r == -2) printf("Login %s deja utilise\n", liste[i].login);
        else if (r == -3) printf("E-mail %s deja utilise\n", liste[i].email);
        else              printf("Erreur d'ouverture du fichier\n");
    }

    /* 2. un vol pour Sirine : points de fidelite */
    if (enregistrer_vol("11992451", 450.0, &pts, &anc, &nouv) == 1)
        printf("Vol enregistre pour Sirine : +%d points\n", pts);

    /* 3. affichage trie par nom */
    n = charger_passagers(t, MAX_PASSAGERS);
    trier_passagers(t, n, 1);
    printf("\n%-10s %-15s %-15s %-4s %-7s %-9s\n", "CIN", "Nom", "Prenom", "Age", "Points", "Niveau");
    printf("--------------------------------------------------------------\n");
    for (i = 0; i < n; i++)
    {
        texte_affichage(nom, t[i].nom);
        texte_affichage(prenom, t[i].prenom);
        nom_niveau(niveau_fidelite(t[i].points), niveau);
        printf("%-10s %-15s %-15s %-4d %-7d %-9s\n",
               t[i].cin, nom, prenom, calculer_age(t[i].naissance), t[i].points, niveau);
    }

    /* 4. test d'une recherche */
    if (chercher_passager("87654321", &p))
        printf("\nRecherche CIN 87654321 : %s %s trouve(e)\n", p.prenom, p.nom);

    printf("\nLe fichier %s a ete cree / mis a jour.\n", FICHIER_PASSAGERS);
    return 0;
}
