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
    double d;             // diametre cheerio
    double m;             // masse du cheerio
    double R;             // rayon de courbure  cree par cheerio
    double Bo;            // Bond number acorded to the cheerios 
    double theta;         // Angle que le cheerio fait avec le liquide
    double Sigma;         // le Sigma du cheerio
    energie_t E;            // Energie mecanique du cheerio
} cheerio_t;   

// Structure contenant les informations de nos bords.
typedef struct Bord{
    vec2_t centre;       // position du centre des Bords
    double rayon;        // rayon entre le centre et les bords.
    double rho;          // Masse volumique du bord
    double R;            // Rayon de courbure du bord
    double theta;        // Angle entre le bord et le liquide.
    double sigma;        // Sigma du Bord.
    double Bo;           // Bond Number du bord.
} bord_t;

#endif // CHEERIOS_H