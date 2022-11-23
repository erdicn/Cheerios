#ifndef CHEERIOS_H
#define CHEERIOS_H

//#define EXTREME_WARNING_MESSAGES 1
#define WARNING_MESAGES 0 // Si on veux voir toute les messages de warrning (si on veux pas les voir faire ca 0)
#define PRINT_INFO 0
#define PRINT_WARNING 0 // Si on veux juste compter les warning et que on veux pas voire les printf on mets 0 si on veux les voires on mets autre 

typedef struct Vec2{
    double x, y;
} vec2_t;

// TODO peut etre metre des courbures differentes ?
typedef struct Cheerio{
    vec2_t pos;           // position cheerio
    vec2_t v;             // vitesse cheerio
    vec2_t a;             // acceleration cheerio
    vec2_t f_applique;    // force applique sur le cheerio
    double d;             // diametre cheerio
    double m;             // masse du cheerio
    long int cher_id;     // identifiant du particule
    long int tas_id;
    double* masses_tas;
} cheerio_t;     

// Un struct qui definit un rectangle le que lon utilise le bord de notre systeme
typedef struct Bords{
    double haut, bas, droite, gauche;
} bords_t;


#endif // CHEERIOS_H