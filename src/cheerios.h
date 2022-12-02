#ifndef CHEERIOS_H
#define CHEERIOS_H

// Structure de vecteur
typedef struct Vec2{
    double x, y; // coordonnées en x et y.
} vec2_t;

// javais mis ca pour lenergie mecanique mais je pense on peux faire juste avec energie mecanique
typedef struct Energie{
    double Em;
    double Ep;
    double Ec;
}energie_t;

// Structure de nos objets movibles.
typedef struct Cheerio{
    vec2_t pos;           // position cheerio
    vec2_t v;             // vitesse cheerio
    vec2_t a;             // acceleration cheerio
    vec2_t f_applique;    // force applique sur le cheerio
    double diametre_cheerio;             // diametre cheerio
    double masse;             // masse du cheerio
    double rayon_courbure;             // rayon de courbure  cree par cheerio
    double Bond_nb;            // Bond number acorded to the cheerios 
    double angle_contact;         // Angle que le cheerio fait avec le liquide
    double Sigma;         // le Sigma du cheerio
    // energie_t E;            // Energie mecanique du cheerio
} cheerio_t;   

// Structure contenant les informations de nos bords.
typedef struct Bord{
    vec2_t centre;       // position du centre des Bords
    double rayon;        // rayon entre le centre et les bords.
    double rho;          // Masse volumique du bord
    double rayon_courbure;            // Rayon de courbure du bord
    double angle_contact;        // Angle entre le bord et le liquide.
    double Sigma;        // Sigma du Bord.
    double Bond_nb;           // Bond Number du bord.
} bord_t;

#endif // CHEERIOS_H