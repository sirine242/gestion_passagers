#ifndef PASSAGER_H_INCLUDED
#define PASSAGER_H_INCLUDED

#include <stdio.h>
#include <gtk/gtk.h>

#define FICHIER_PASSAGERS     "passager.txt"
#define FICHIER_UTILISATEURS  "utilisateurs.txt"
#define MAX_PASSAGERS         500

#define NB_CHAMPS_PASSAGER    16

typedef struct
{
    int jour;
    int mois;
    int annee;
} Date;

typedef struct
{
    char  cin[9];          
    char  nom[30];
    char  prenom[30];
    Date  naissance;
    char  sexe[10];        
    char  nationalite[30];
    char  tel[9];          
    char  email[60];       
    char  login[30];       
    int   points;          
    int   nb_vols;          
    float total_depense;    
    int   bloque;        
    char  motif[120];       
} Passager;

enum { STANDARD, SILVER, GOLD, PLATINUM };

int   lire_passager(FILE *f, Passager *p);
void  ecrire_passager(FILE *f, Passager p);
int   ajouter_passager(Passager p);
int   modifier_passager(char *cin, Passager p);
int   supprimer_passager(char *cin);
int   chercher_passager(char *cin, Passager *p);
int   chercher_passager_login(char *login, Passager *p);
int   email_existe(char *email, char *cin_exclu);
int   charger_passagers(Passager t[], int max);

void  trier_passagers(Passager t[], int n, int croissant);
int   correspond_filtre(Passager p, char *filtre);
int   afficher_passagers(GtkWidget *liste, char *filtre, int croissant);

int   calculer_age(Date d);
int   verifier_cin(char *cin);
int   verifier_nom(char *nom);
int   verifier_date(Date d);
int   verifier_tel(char *tel);
int   verifier_email(char *email);
int   verifier_login(char *login);
int   controler_passager(Passager p, char *erreur);


void  nettoyer_texte(char *s);        
void  texte_affichage(char *dest, char *src);

int   niveau_fidelite(int points);
void  nom_niveau(int niveau, char *nom);
float remise_fidelite(int points);
void  avantages_fidelite(int points, char *texte);
int   seuil_niveau_suivant(int points);
int   calculer_points(float prix_billet);
int   enregistrer_vol(char *cin, float prix_billet, int *points_gagnes, int *ancien_niveau, int *nouveau_niveau);
float prix_apres_remise(char *cin, float prix);


int   bloquer_passager(char *cin, char *motif);
int   reactiver_passager(char *cin);


void  afficher_statistiques(GtkWidget *tv_age, GtkWidget *tv_niveau, GtkWidget *tv_top, char *resume);


int   verifier_utilisateur(char *login, char *mdp, char *role);
int   login_existe(char *login);
int   ajouter_utilisateur(char *login, char *mdp, char *role);
int   modifier_utilisateur(char *ancien_login, char *nouveau_login, char *nouveau_mdp);
int   supprimer_utilisateur(char *login);

#endif 
