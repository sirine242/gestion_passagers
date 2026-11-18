
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <gtk/gtk.h>
#include "passager.h"

enum
{
    COL_CIN,
    COL_NOM,
    COL_PRENOM,
    COL_AGE,
    COL_SEXE,
    COL_TEL,
    COL_EMAIL,
    COL_NATIONALITE,
    COL_POINTS,
    COL_NIVEAU,
    COL_STATUT,
    NB_COLONNES
};


int lire_passager(FILE *f, Passager *p)
{
    return fscanf(f, "%8s %29s %29s %d %d %d %9s %29s %8s %59s %29s %d %d %f %d %119s",
                  p->cin, p->nom, p->prenom,
                  &p->naissance.jour, &p->naissance.mois, &p->naissance.annee,
                  p->sexe, p->nationalite, p->tel, p->email, p->login,
                  &p->points, &p->nb_vols, &p->total_depense, &p->bloque, p->motif);
}

void ecrire_passager(FILE *f, Passager p)
{
    if (strlen(p.motif) == 0)
        strcpy(p.motif, "aucun");
    fprintf(f, "%s %s %s %d %d %d %s %s %s %s %s %d %d %.3f %d %s\n",
            p.cin, p.nom, p.prenom,
            p.naissance.jour, p.naissance.mois, p.naissance.annee,
            p.sexe, p.nationalite, p.tel, p.email, p.login,
            p.points, p.nb_vols, p.total_depense, p.bloque, p.motif);
}

int chercher_passager(char *cin, Passager *p)
{
    FILE *f;
    Passager x;
    int trouve = 0;

    f = fopen(FICHIER_PASSAGERS, "r");
    if (f == NULL)
        return 0;
    while (!trouve && lire_passager(f, &x) == NB_CHAMPS_PASSAGER)
    {
        if (strcmp(x.cin, cin) == 0)
        {
            *p = x;
            trouve = 1;
        }
    }
    fclose(f);
    return trouve;
}

int chercher_passager_login(char *login, Passager *p)
{
    FILE *f;
    Passager x;
    int trouve = 0;

    f = fopen(FICHIER_PASSAGERS, "r");
    if (f == NULL)
        return 0;
    while (!trouve && lire_passager(f, &x) == NB_CHAMPS_PASSAGER)
    {
        if (strcmp(x.login, login) == 0)
        {
            *p = x;
            trouve = 1;
        }
    }
    fclose(f);
    return trouve;
}

int email_existe(char *email, char *cin_exclu)
{
    FILE *f;
    Passager x;
    int existe = 0;

    f = fopen(FICHIER_PASSAGERS, "r");
    if (f == NULL)
        return 0;
    while (!existe && lire_passager(f, &x) == NB_CHAMPS_PASSAGER)
    {
        if (g_ascii_strcasecmp(x.email, email) == 0 &&
            (cin_exclu == NULL || strcmp(x.cin, cin_exclu) != 0))
            existe = 1;
    }
    fclose(f);
    return existe;
}

int ajouter_passager(Passager p)
{
    FILE *f;
    Passager x;

    if (chercher_passager(p.cin, &x))
        return -1;
    if (chercher_passager_login(p.login, &x) || login_existe(p.login))
        return -2;
    if (email_existe(p.email, NULL))
        return -3;

    f = fopen(FICHIER_PASSAGERS, "a");
    if (f == NULL)
        return 0;
    ecrire_passager(f, p);
    fclose(f);
    return 1;
}

int modifier_passager(char *cin, Passager nouv)
{
    FILE *f, *f2;
    Passager p;
    int trouve = 0;

    f = fopen(FICHIER_PASSAGERS, "r");
    if (f == NULL)
        return 0;
    f2 = fopen("tmp_passager.txt", "w");
    if (f2 == NULL)
    {
        fclose(f);
        return 0;
    }
    while (lire_passager(f, &p) == NB_CHAMPS_PASSAGER)
    {
        if (strcmp(p.cin, cin) == 0)
        {
            ecrire_passager(f2, nouv);
            trouve = 1;
        }
        else
            ecrire_passager(f2, p);
    }
    fclose(f);
    fclose(f2);
    remove(FICHIER_PASSAGERS);
    rename("tmp_passager.txt", FICHIER_PASSAGERS);
    return trouve;
}

int supprimer_passager(char *cin)
{
    FILE *f, *f2;
    Passager p;
    int trouve = 0;

    f = fopen(FICHIER_PASSAGERS, "r");
    if (f == NULL)
        return 0;
    f2 = fopen("tmp_passager.txt", "w");
    if (f2 == NULL)
    {
        fclose(f);
        return 0;
    }
    while (lire_passager(f, &p) == NB_CHAMPS_PASSAGER)
    {
        if (strcmp(p.cin, cin) == 0)
            trouve = 1;
        else
            ecrire_passager(f2, p);
    }
    fclose(f);
    fclose(f2);
    remove(FICHIER_PASSAGERS);
    rename("tmp_passager.txt", FICHIER_PASSAGERS);
    return trouve;
}

int charger_passagers(Passager t[], int max)
{
    FILE *f;
    int n = 0;

    f = fopen(FICHIER_PASSAGERS, "r");
    if (f == NULL)
        return 0;
    while (n < max && lire_passager(f, &t[n]) == NB_CHAMPS_PASSAGER)
        n++;
    fclose(f);
    return n;
}

void nettoyer_texte(char *s)
{
    int i;
    g_strstrip(s);
    for (i = 0; s[i] != '\0'; i++)
        if (s[i] == ' ')
            s[i] = '_';
}


void texte_affichage(char *dest, char *src)
{
    int i;
    for (i = 0; src[i] != '\0'; i++)
        dest[i] = (src[i] == '_') ? ' ' : src[i];
    dest[i] = '\0';
}


static int comparer_textes(char *a, char *b)
{
    char ta[64], tb[64];
    gchar *ca, *cb;
    int r;

    texte_affichage(ta, a);
    texte_affichage(tb, b);
    ca = g_utf8_casefold(ta, -1);
    cb = g_utf8_casefold(tb, -1);
    r = g_utf8_collate(ca, cb);
    g_free(ca);
    g_free(cb);
    return r;
}

static int comparer_noms(Passager a, Passager b)
{
    int r = comparer_textes(a.nom, b.nom);
    if (r == 0)
        r = comparer_textes(a.prenom, b.prenom);
    return r;
}


void trier_passagers(Passager t[], int n, int croissant)
{
    int i, j, k;
    Passager aux;

    for (i = 0; i < n - 1; i++)
    {
        k = i;
        for (j = i + 1; j < n; j++)
        {
            int c = comparer_noms(t[j], t[k]);
            if ((croissant && c < 0) || (!croissant && c > 0))
                k = j;
        }
        if (k != i)
        {
            aux = t[i];
            t[i] = t[k];
            t[k] = aux;
        }
    }
}


int correspond_filtre(Passager p, char *filtre)
{
    char texte[200], f[100];
    gchar *t_min, *f_min;
    int ok;

    if (filtre == NULL || filtre[0] == '\0')
        return 1;

    sprintf(texte, "%s %s %s %s %s %s", p.cin, p.nom, p.prenom, p.prenom, p.nom, p.login);
    texte_affichage(texte, texte);
    texte_affichage(f, filtre);
    t_min = g_utf8_casefold(texte, -1);
    f_min = g_utf8_casefold(f, -1);
    ok = (strstr(t_min, f_min) != NULL);
    g_free(t_min);
    g_free(f_min);
    return ok;
}

static void ajouter_colonne(GtkWidget *liste, char *titre, int col, int markup, float xalign)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "xalign", xalign, NULL);
    column = gtk_tree_view_column_new_with_attributes(titre, renderer,
                                                      markup ? "markup" : "text", col, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(liste), column);
}

static void niveau_markup(int points, char *dest)
{
    char nom[20];
    char *couleur[] = { "#6b7a90", "#7d8a99", "#b8901c", "#3b4a6b" };
    int n = niveau_fidelite(points);
    nom_niveau(n, nom);
    sprintf(dest, "<span foreground='%s'><b>%s</b></span>", couleur[n], nom);
}

int afficher_passagers(GtkWidget *liste, char *filtre, int croissant)
{
    GtkListStore *store;
    GtkTreeIter iter;
    static Passager t[MAX_PASSAGERS];
    int n, i, nb = 0;
    char nom[30], prenom[30], email[60], nat[30], age[8], points[12], niveau[120], statut[400], motif[120];

    if (gtk_tree_view_get_column(GTK_TREE_VIEW(liste), 0) == NULL)
    {
        ajouter_colonne(liste, "CIN", COL_CIN, 0, 0.0);
        ajouter_colonne(liste, "Nom", COL_NOM, 0, 0.0);
        ajouter_colonne(liste, "Prénom", COL_PRENOM, 0, 0.0);
        ajouter_colonne(liste, "Âge", COL_AGE, 0, 1.0);
        ajouter_colonne(liste, "Sexe", COL_SEXE, 0, 0.0);
        ajouter_colonne(liste, "Téléphone", COL_TEL, 0, 0.0);
        ajouter_colonne(liste, "E-mail", COL_EMAIL, 0, 0.0);
        ajouter_colonne(liste, "Nationalité", COL_NATIONALITE, 0, 0.0);
        ajouter_colonne(liste, "Points", COL_POINTS, 0, 1.0);
        ajouter_colonne(liste, "Niveau", COL_NIVEAU, 1, 0.0);
        ajouter_colonne(liste, "Statut", COL_STATUT, 1, 0.0);
    }

    store = gtk_list_store_new(NB_COLONNES,
                               G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                               G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                               G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    n = charger_passagers(t, MAX_PASSAGERS);
    trier_passagers(t, n, croissant);

    for (i = 0; i < n; i++)
    {
        if (!correspond_filtre(t[i], filtre))
            continue;

        texte_affichage(nom, t[i].nom);
        texte_affichage(prenom, t[i].prenom);
        texte_affichage(email, t[i].email);
        texte_affichage(nat, t[i].nationalite);
        sprintf(age, "%d", calculer_age(t[i].naissance));
        sprintf(points, "%d", t[i].points);
        niveau_markup(t[i].points, niveau);

        if (t[i].bloque)
        {
            gchar *m;
            texte_affichage(motif, t[i].motif);
            m = g_markup_escape_text(motif, -1);
            g_snprintf(statut, sizeof(statut), "<span foreground='#c0392b'><b>Bloqué</b></span> <small>(%s)</small>", m);
            g_free(m);
        }
        else
            strcpy(statut, "<span foreground='#1e8449'><b>Actif</b></span>");

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COL_CIN, t[i].cin, COL_NOM, nom, COL_PRENOM, prenom,
                           COL_AGE, age, COL_SEXE, t[i].sexe, COL_TEL, t[i].tel,
                           COL_EMAIL, email, COL_NATIONALITE, nat, COL_POINTS, points,
                           COL_NIVEAU, niveau, COL_STATUT, statut, -1);
        nb++;
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(liste), GTK_TREE_MODEL(store));
    g_object_unref(store);
    return nb;
}


static void date_aujourdhui(Date *d)
{
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    d->jour = now->tm_mday;
    d->mois = now->tm_mon + 1;
    d->annee = now->tm_year + 1900;
}

int calculer_age(Date d)
{
    Date auj;
    int age;

    date_aujourdhui(&auj);
    age = auj.annee - d.annee;
    if (auj.mois < d.mois || (auj.mois == d.mois && auj.jour < d.jour))
        age--;
    return age;
}

static int jours_dans_mois(int mois, int annee)
{
    int jours[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (mois == 2 && ((annee % 4 == 0 && annee % 100 != 0) || annee % 400 == 0))
        return 29;
    return jours[mois - 1];
}


int verifier_cin(char *cin)
{
    int i;
    if (strlen(cin) != 8)
        return 0;
    for (i = 0; i < 8; i++)
        if (!isdigit((unsigned char)cin[i]))
            return 0;
    return 1;
}

int verifier_nom(char *nom)
{
    int i, lettres = 0;
    int n = strlen(nom);

    if (n < 2 || n > 29)
        return 0;
    for (i = 0; i < n; i++)
    {
        unsigned char c = nom[i];
        if (isalpha(c) || c >= 128)
            lettres++;
        else if (c != '_' && c != '-' && c != '\'')
            return 0;
    }
    return lettres >= 2;
}

int verifier_date(Date d)
{
    int age;
    if (d.mois < 1 || d.mois > 12 || d.jour < 1)
        return 0;
    if (d.jour > jours_dans_mois(d.mois, d.annee))
        return 0;
    age = calculer_age(d);
    return age >= 0 && age <= 120;
}

int verifier_tel(char *tel)
{
    int i;
    if (strlen(tel) != 8)
        return 0;
    if (strchr("234579", tel[0]) == NULL)
        return 0;
    for (i = 0; i < 8; i++)
        if (!isdigit((unsigned char)tel[i]))
            return 0;
    return 1;
}

int verifier_email(char *email)
{
    char *arobase, *point;
    int i, n = strlen(email);

    if (n < 6 || n > 59)
        return 0;
    arobase = strchr(email, '@');
    if (arobase == NULL || arobase == email || strchr(arobase + 1, '@') != NULL)
        return 0;
    for (i = 0; email + i < arobase; i++)
    {
        unsigned char c = email[i];
        if (!isalnum(c) && c != '.' && c != '_' && c != '-' && c != '+')
            return 0;
    }
    if (email[0] == '.' || *(arobase - 1) == '.')
        return 0;
    for (i = (arobase - email) + 1; i < n; i++)
    {
        unsigned char c = email[i];
        if (!isalnum(c) && c != '.' && c != '-')
            return 0;
    }
    if (strstr(email, "..") != NULL)
        return 0;
    point = strrchr(arobase, '.');
    if (point == NULL || point == arobase + 1 || strlen(point + 1) < 2)
        return 0;
    for (i = 1; point[i] != '\0'; i++)
        if (!isalpha((unsigned char)point[i]))
            return 0;
    return 1;
}

int verifier_login(char *login)
{
    int i, n = strlen(login);
    if (n < 3 || n > 29 || !isalpha((unsigned char)login[0]))
        return 0;
    for (i = 1; i < n; i++)
    {
        unsigned char c = login[i];
        if (!isalnum(c) && c != '.' && c != '_' && c != '-')
            return 0;
    }
    return 1;
}

int controler_passager(Passager p, char *erreur)
{
    if (!verifier_cin(p.cin))
        strcpy(erreur, "Le CIN doit contenir exactement 8 chiffres.");
    else if (!verifier_nom(p.nom))
        strcpy(erreur, "Nom invalide : 2 à 29 lettres (espace, tiret et apostrophe acceptés).");
    else if (!verifier_nom(p.prenom))
        strcpy(erreur, "Prénom invalide : 2 à 29 lettres (espace, tiret et apostrophe acceptés).");
    else if (!verifier_date(p.naissance))
        strcpy(erreur, "Date de naissance invalide (date inexistante ou dans le futur).");
    else if (strcmp(p.sexe, "Homme") != 0 && strcmp(p.sexe, "Femme") != 0)
        strcpy(erreur, "Veuillez choisir le sexe.");
    else if (!verifier_tel(p.tel))
        strcpy(erreur, "Téléphone invalide : 8 chiffres commençant par 2, 3, 4, 5, 7 ou 9.");
    else if (!verifier_email(p.email))
        strcpy(erreur, "Adresse e-mail invalide (exemple : nom.prenom@domaine.tn).");
    else if (strlen(p.nationalite) == 0)
        strcpy(erreur, "Veuillez choisir la nationalité.");
    else if (!verifier_login(p.login))
        strcpy(erreur, "Login invalide : 3 à 29 caractères, commence par une lettre (lettres, chiffres, . _ -).");
    else
    {
        erreur[0] = '\0';
        return 1;
    }
    return 0;
}

int niveau_fidelite(int points)
{
    if (points >= 600) return PLATINUM;
    if (points >= 300) return GOLD;
    if (points >= 100) return SILVER;
    return STANDARD;
}

void nom_niveau(int niveau, char *nom)
{
    switch (niveau)
    {
    case SILVER:   strcpy(nom, "Silver");   break;
    case GOLD:     strcpy(nom, "Gold");     break;
    case PLATINUM: strcpy(nom, "Platinum"); break;
    default:       strcpy(nom, "Standard");
    }
}

float remise_fidelite(int points)
{
    switch (niveau_fidelite(points))
    {
    case SILVER:   return 5;
    case GOLD:     return 10;
    case PLATINUM: return 15;
    default:       return 0;
    }
}

void avantages_fidelite(int points, char *texte)
{
    switch (niveau_fidelite(points))
    {
    case SILVER:
        strcpy(texte, "•  5 % de remise sur vos prochaines réservations\n"
                      "•  Enregistrement prioritaire au comptoir\n"
                      "•  Cumul de points sur chaque vol");
        break;
    case GOLD:
        strcpy(texte, "•  10 % de remise sur vos prochaines réservations\n"
                      "•  Enregistrement et embarquement prioritaires\n"
                      "•  Bagage prioritaire + 10 kg de franchise offerts");
        break;
    case PLATINUM:
        strcpy(texte, "•  15 % de remise sur vos prochaines réservations\n"
                      "•  Accès au salon VIP dans nos aéroports\n"
                      "•  Surclassement selon disponibilité\n"
                      "•  Bagage prioritaire + 20 kg de franchise offerts");
        break;
    default:
        strcpy(texte, "•  Cumul de 1 point par tranche de 10 DT dépensés\n"
                      "•  Atteignez 100 points pour passer Silver (5 % de remise)");
    }
}

int seuil_niveau_suivant(int points)
{
    switch (niveau_fidelite(points))
    {
    case STANDARD: return 100;
    case SILVER:   return 300;
    case GOLD:     return 600;
    default:       return -1;
    }
}

int calculer_points(float prix_billet)
{
    if (prix_billet <= 0)
        return 0;
    return (int)((prix_billet + 0.0001) / 10);
}

int enregistrer_vol(char *cin, float prix_billet, int *points_gagnes, int *ancien_niveau, int *nouveau_niveau)
{
    Passager p;

    if (prix_billet <= 0)
        return -1;
    if (!chercher_passager(cin, &p))
        return 0;

    *ancien_niveau = niveau_fidelite(p.points);
    *points_gagnes = calculer_points(prix_billet);
    p.points += *points_gagnes;
    p.nb_vols++;
    p.total_depense += prix_billet;
    *nouveau_niveau = niveau_fidelite(p.points);
    modifier_passager(cin, p);
    return 1;
}

float prix_apres_remise(char *cin, float prix)
{
    Passager p;
    if (!chercher_passager(cin, &p))
        return prix;
    return prix * (100 - remise_fidelite(p.points)) / 100;
}

int bloquer_passager(char *cin, char *motif)
{
    Passager p;
    if (!chercher_passager(cin, &p))
        return 0;
    p.bloque = 1;
    strncpy(p.motif, motif, sizeof(p.motif) - 1);
    p.motif[sizeof(p.motif) - 1] = '\0';
    nettoyer_texte(p.motif);
    if (p.motif[0] == '\0')
        strcpy(p.motif, "non_precise");
    return modifier_passager(cin, p);
}

int reactiver_passager(char *cin)
{
    Passager p;
    if (!chercher_passager(cin, &p))
        return 0;
    p.bloque = 0;
    strcpy(p.motif, "aucun");
    return modifier_passager(cin, p);
}


static void colonne_texte(GtkWidget *tv, char *titre, int col, float xalign)
{
    ajouter_colonne(tv, titre, col, 0, xalign);
}

static void colonne_barre(GtkWidget *tv, char *titre, int col_valeur, int col_texte)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_progress_new();
    column = gtk_tree_view_column_new_with_attributes(titre, renderer,
                                                      "value", col_valeur,
                                                      "text", col_texte, NULL);
    gtk_tree_view_column_set_expand(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tv), column);
}

static void remplir_repartition(GtkWidget *tv, char *titre, char *libelles[], int nb[], int n, int total)
{
    GtkListStore *store;
    GtkTreeIter iter;
    char nombre[12], pourcent[16];
    int i, pct;

    if (gtk_tree_view_get_column(GTK_TREE_VIEW(tv), 0) == NULL)
    {
        colonne_texte(tv, titre, 0, 0.0);
        colonne_texte(tv, "Passagers", 1, 1.0);
        colonne_barre(tv, "Part", 2, 3);
    }
    store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
    for (i = 0; i < n; i++)
    {
        pct = (total > 0) ? (nb[i] * 100 + total / 2) / total : 0;
        sprintf(nombre, "%d", nb[i]);
        sprintf(pourcent, "%d %%", pct);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, libelles[i], 1, nombre, 2, pct, 3, pourcent, -1);
    }
    gtk_tree_view_set_model(GTK_TREE_VIEW(tv), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void afficher_statistiques(GtkWidget *tv_age, GtkWidget *tv_niveau, GtkWidget *tv_top, char *resume)
{
    static Passager t[MAX_PASSAGERS];
    char *tranches[] = { "Moins de 18 ans", "18 – 25 ans", "26 – 35 ans", "36 – 50 ans", "51 – 65 ans", "Plus de 65 ans" };
    char *niveaux[] = { "Standard", "Silver", "Gold", "Platinum" };
    int nb_age[6] = { 0 }, nb_niv[4] = { 0 };
    int n, i, j, k, age, bloques = 0, somme_age = 0, somme_points = 0;
    GtkListStore *store;
    GtkTreeIter iter;
    Passager aux;

    n = charger_passagers(t, MAX_PASSAGERS);
    for (i = 0; i < n; i++)
    {
        age = calculer_age(t[i].naissance);
        somme_age += age;
        somme_points += t[i].points;
        if (t[i].bloque) bloques++;
        if (age < 18) nb_age[0]++;
        else if (age <= 25) nb_age[1]++;
        else if (age <= 35) nb_age[2]++;
        else if (age <= 50) nb_age[3]++;
        else if (age <= 65) nb_age[4]++;
        else nb_age[5]++;
        nb_niv[niveau_fidelite(t[i].points)]++;
    }

    remplir_repartition(tv_age, "Tranche d'âge", tranches, nb_age, 6, n);
    remplir_repartition(tv_niveau, "Niveau", niveaux, nb_niv, 4, n);

    for (i = 0; i < n - 1; i++)
    {
        k = i;
        for (j = i + 1; j < n; j++)
            if (t[j].nb_vols > t[k].nb_vols ||
                (t[j].nb_vols == t[k].nb_vols && t[j].points > t[k].points))
                k = j;
        aux = t[i]; t[i] = t[k]; t[k] = aux;
    }

    if (gtk_tree_view_get_column(GTK_TREE_VIEW(tv_top), 0) == NULL)
    {
        colonne_texte(tv_top, "Rang", 0, 0.5);
        colonne_texte(tv_top, "Passager", 1, 0.0);
        colonne_texte(tv_top, "CIN", 2, 0.0);
        colonne_texte(tv_top, "Vols effectués", 3, 1.0);
        colonne_texte(tv_top, "Points", 4, 1.0);
        ajouter_colonne(tv_top, "Niveau", 5, 1, 0.0);
    }
    store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    for (i = 0; i < n && i < 5; i++)
    {
        char rang[8], nom[70], vols[12], pts[12], niv[120], tmp[70];
        sprintf(rang, "%d", i + 1);
        sprintf(tmp, "%s %s", t[i].prenom, t[i].nom);
        texte_affichage(nom, tmp);
        sprintf(vols, "%d", t[i].nb_vols);
        sprintf(pts, "%d", t[i].points);
        niveau_markup(t[i].points, niv);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, rang, 1, nom, 2, t[i].cin, 3, vols, 4, pts, 5, niv, -1);
    }
    gtk_tree_view_set_model(GTK_TREE_VIEW(tv_top), GTK_TREE_MODEL(store));
    g_object_unref(store);

    if (n == 0)
        strcpy(resume, "<span foreground='#6b7a90'>Aucun passager enregistré.</span>");
    else
        sprintf(resume,
                "<b>%d</b> passagers   ·   <span foreground='#1e8449'><b>%d</b> actifs</span>   ·   "
                "<span foreground='#c0392b'><b>%d</b> bloqués</span>   ·   âge moyen <b>%d</b> ans   ·   "
                "<b>%d</b> points en moyenne",
                n, n - bloques, bloques, (somme_age + n / 2) / n, (somme_points + n / 2) / n);
}

int verifier_utilisateur(char *login, char *mdp, char *role)
{
    FILE *f;
    char l[30], m[30], r[20];
    int trouve = 0;

    f = fopen(FICHIER_UTILISATEURS, "r");
    if (f == NULL)
        return 0;
    while (!trouve && fscanf(f, "%29s %29s %19s", l, m, r) == 3)
    {
        if (strcmp(l, login) == 0 && strcmp(m, mdp) == 0)
        {
            strcpy(role, r);
            trouve = 1;
        }
    }
    fclose(f);
    return trouve;
}

int login_existe(char *login)
{
    FILE *f;
    char l[30], m[30], r[20];
    int trouve = 0;

    f = fopen(FICHIER_UTILISATEURS, "r");
    if (f == NULL)
        return 0;
    while (!trouve && fscanf(f, "%29s %29s %19s", l, m, r) == 3)
        if (strcmp(l, login) == 0)
            trouve = 1;
    fclose(f);
    return trouve;
}

int ajouter_utilisateur(char *login, char *mdp, char *role)
{
    FILE *f = fopen(FICHIER_UTILISATEURS, "a");
    if (f == NULL)
        return 0;
    fprintf(f, "%s %s %s\n", login, mdp, role);
    fclose(f);
    return 1;
}

int modifier_utilisateur(char *ancien_login, char *nouveau_login, char *nouveau_mdp)
{
    FILE *f, *f2;
    char l[30], m[30], r[20];
    int trouve = 0;

    f = fopen(FICHIER_UTILISATEURS, "r");
    if (f == NULL)
        return 0;
    f2 = fopen("tmp_utilisateurs.txt", "w");
    if (f2 == NULL)
    {
        fclose(f);
        return 0;
    }
    while (fscanf(f, "%29s %29s %19s", l, m, r) == 3)
    {
        if (strcmp(l, ancien_login) == 0)
        {
            fprintf(f2, "%s %s %s\n", nouveau_login, strlen(nouveau_mdp) > 0 ? nouveau_mdp : m, r);
            trouve = 1;
        }
        else
            fprintf(f2, "%s %s %s\n", l, m, r);
    }
    fclose(f);
    fclose(f2);
    remove(FICHIER_UTILISATEURS);
    rename("tmp_utilisateurs.txt", FICHIER_UTILISATEURS);
    return trouve;
}

int supprimer_utilisateur(char *login)
{
    FILE *f, *f2;
    char l[30], m[30], r[20];
    int trouve = 0;

    f = fopen(FICHIER_UTILISATEURS, "r");
    if (f == NULL)
        return 0;
    f2 = fopen("tmp_utilisateurs.txt", "w");
    if (f2 == NULL)
    {
        fclose(f);
        return 0;
    }
    while (fscanf(f, "%29s %29s %19s", l, m, r) == 3)
    {
        if (strcmp(l, login) == 0)
            trouve = 1;
        else
            fprintf(f2, "%s %s %s\n", l, m, r);
    }
    fclose(f);
    fclose(f2);
    remove(FICHIER_UTILISATEURS);
    rename("tmp_utilisateurs.txt", FICHIER_UTILISATEURS);
    return trouve;
}
