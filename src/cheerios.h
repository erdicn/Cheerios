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
    vec2_t pos;                         // Position du cheerio.
    vec2_t v;                           // Vitesse du cheerio.
    vec2_t a;                           // Accélération du cheerio.
    vec2_t f_applique;                  // Force appliquée sur le cheerio.
    double diametre_cheerio;            // Diamètre du cheerio.
    double masse;                       // Masse du cheerio.
    double rayon_courbure;              // Rayon de courbure créé par le cheerio.
    double Bond_nb;                     // Nombre de Bond accordé aux cheerios. 
    double angle_contact;               // Angle entre le liquide et le cheerio.
    double Sigma;                       // Sigma du cheerio => Voir CalculSigma dans calculs.c pour plus d'explications.
    // energie_t E;                     // Énergie mécanique du cheerio.
} cheerio_t;   

// Structure contenant les informations de nos bords.
typedef struct Bord{
    vec2_t centre;                      // Position du centre des Bords
    double rayon;                       // Rayon entre le centre et les bords.
    double rho;                         // Masse volumique du bord
    double rayon_courbure;              // Rayon de courbure du bord
    double angle_contact;               // Angle entre le bord et le liquide.
    double Sigma;                       // Sigma du Bord.
    double Bond_nb;                     // Nombre de Bond du bord.
} bord_t;

#endif // CHEERIOS_H