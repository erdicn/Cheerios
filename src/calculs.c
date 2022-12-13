#include <stdio.h>
#include <math.h>
#include <gsl/gsl_sf_bessel.h>
#include "cheerios.h"


// Retourne le carré de x.
double sq(double x){
    return x*x;
}

// Retourne le cube de x.
double cb(double x){
    return x*x*x;
}

// Retourne un nouveau vecteur qui est l'addition des 2 vecteurs v1 et v2.
vec2_t VecAdition(vec2_t v1, vec2_t v2){
    vec2_t new_vec = { .x = v1.x + v2.x,
                       .y = v1.y + v2.y};
    return new_vec;
}

// Retourne un nouveau vecteur qui est la soustraction des 2 vecteurs v1 et v2.
vec2_t VecSubstraction(vec2_t v1, vec2_t v2){
    vec2_t new_vec = { .x = v1.x - v2.x,
                       .y = v1.y - v2.y};
    return new_vec;
}

// Retourne le vecteur vec avec ses coordonnées multipliés par la valeur s.
vec2_t VecTimesScalar(vec2_t vec, double s){
    vec2_t new_vec = { .x = vec.x * s,
                       .y = vec.y * s};
    return new_vec;
}

// Retourne la norme du vecteur à 2 dimensions vec.
double CalculNorme(vec2_t vec){
    return sqrt(sq(vec.x) + sq(vec.y));
}

// Retourne le produit scalaire de v1 avec v2
double ProduitScalaire(vec2_t v1, vec2_t v2){
    return v1.x*v2.x + v1.y*v2.y;
}

// Retourne le vecteur vec tourné d'un angle "angle" en radian dans le sens trigonomètrique.
vec2_t CalculRotatedVec(vec2_t vec, double angle){
    vec2_t new_vec = {.x = vec.x * cos(angle) - vec.y * sin(angle),
                      .y = vec.x * sin(angle) + vec.y * cos(angle)};
    return new_vec;
}

// Retourne le vecteur unitaire du sens entre 1 et 2.
vec2_t SensEntre1et2(vec2_t pos1, vec2_t pos2, double distance){
    vec2_t sens12 = {.x = (pos2.x - pos1.x)/distance,
                     .y = (pos2.y - pos1.y)/distance};
    return sens12;
}

// Retourne la distance entre deux objets.
double CalculDistance(vec2_t pos1, vec2_t pos2){
    return sqrt(sq(pos2.x - pos1.x) + sq(pos2.y - pos1.y)) ;
}

// retourne la longueur capilaire du liquide 
double CalculLongeurCapilaire(double surface_tension_liq_air, double rho_liq, double rho_air, double g){
    return sqrt(surface_tension_liq_air/(fabs(rho_liq-rho_air)*g));
}

/**
 * @brief Retourne le nombre de Bond.
 * 
 * @param rho_liquide Masse volumique du liquide
 * @param rho_air Masse volumique de l'air
 * @param R Rayon de courbure entre le liquide et l'objet
 * @param gamma Tension de surface
 * @param g acceleration de la pesanteur 
 * @return double
 */
double CalculBondNumber(double rho_liquide, double rho_air, 
                            double R, double gamma ,double g){
    double delta_rho = fabs(rho_liquide-rho_air);   //fabs = valeur absolue pour les doubles 
    return (delta_rho * g * sq(R)) / gamma;    
}

/**
 * @brief Retourne le nombre de Bond calculé de façon linéaire.
 * 
 * @param R Rayon de courbure entre le liquide et l'objet
 * @param L_c Longueur capillaire
 * @return double 
 */
double CalculLinearBondNumber( double R, double L_c){
    return sq(R)/sq(L_c);
}

// Retourne le calcul appelé Sigma.
/**
 * @brief Retourne le calcul appelé Sigma. Ce calcul provient de l'article The ‘Cheerios effect’ par Dominic Vella and L. Mahadevan, équation 9. L'article est retrouvable dans notre bibliographie.
 * 
 * @param rho_flottant Masse volumique de l'objet
 * @param rho_liquide Masse volumique du liquide
 * @param theta Angle de courbure
 * @return double 
 */
double CalculSigma(double rho_flottant, double rho_liquide, double theta){
    double D = rho_flottant/rho_liquide; 
    return ((2*D-1) / 3.0) - 0.5*cos(theta) + (1/6.0)*cb(cos(theta));
}

/**
 * @brief Retourne fa puissance de la force applique par l'objet
 * 
 * @param surface_tension 
 * @param rayon_courbure 
 * @param Bond_nb 
 * @param Sigma 
 * @param distance 
 * @param capilary_length 
 * @return double 
 */
double ForceBetweenTwoInteractingParticles(double surface_tension, double rayon_courbure, double Bond_nb, 
                                            double Sigma, double distance, double capilary_length){
    // précondition l > 0 
    return -2*M_PI*surface_tension*rayon_courbure*sqrt(pow(Bond_nb,5))*sq(Sigma)*gsl_sf_bessel_K1(distance/capilary_length);
}

// Retourne la force émise par les bords sur une particule. 
vec2_t ForceBord(bord_t bord, cheerio_t cheerio, double surface_tension, double capilary_length){

    double dist_cheerio_centre = CalculDistance(cheerio.pos, bord.centre);
    vec2_t Force1 =VecTimesScalar( SensEntre1et2(cheerio.pos, bord.centre, dist_cheerio_centre),
                            ForceBetweenTwoInteractingParticles(surface_tension, bord.rayon_courbure, bord.Bond_nb, bord.Sigma, bord.rayon + dist_cheerio_centre, capilary_length));
    vec2_t Force2 =VecTimesScalar( SensEntre1et2(bord.centre, cheerio.pos, dist_cheerio_centre),
                            ForceBetweenTwoInteractingParticles(surface_tension, bord.rayon_courbure, bord.Bond_nb, bord.Sigma, bord.rayon - dist_cheerio_centre, capilary_length));
    return VecAdition(Force1, Force2);
}

// retourne le volume dun sphere avec un diametre d
double CalculVolumeSphere(double d){
    return 4./3. * M_PI * sq(d/2.);
}


// Initialise les valeurs du nombre de Bond et de Sigma pour tous les objets.
void InitialiseBondEtSigma(cheerio_t* cheerios, int nb_cheerios, double capilary_length, double rho_liq, double rho_cheerio, bord_t* bord){
    double tmp_B, tmp_theta, tmp_abs_B;
    int error = 0;
    // Nous calculons d'abord pour le bord.
    bord->Bond_nb = CalculLinearBondNumber(bord->rayon_courbure, capilary_length);
    bord->Sigma = CalculSigma(bord->rho, rho_liq, bord->angle_contact);
    // Maintenant nous le faisons pour tous les cheerios.
    for(int i = 0; i < nb_cheerios; i++){                                                         // linBondnb = BondNumber dans le cas de nos simulations, si ils sont pas egale ces cas sont hors du sujet de la simulation
        cheerios[i].Bond_nb = CalculLinearBondNumber(cheerios[i].rayon_courbure, capilary_length);//CalculBondNumber(rho_liq, rho_air, cheerios[i].R, surface_tension, g); 
        tmp_B = cheerios[i].Bond_nb;
        tmp_abs_B = fabs(tmp_B);
        cheerios[i].angle_contact = fabs(tmp_B) < 0.63 ? asin(M_PI_2 * tmp_B) : asin(tmp_B);     // TODO expliquer pq on prends comme ca pour linstant ca change pas par rapport a la proximite et cest symetyrique mais en realite ca depend de la proximite des particules source Lattice Boltzmann simulation of capillary interactions among colloidal particles equation27 //(M_PI * 30) /180;    faire la funtion qui trouve langle (pour linstant on a une valeur au pif il faux ecrire l'equation pour trouver l'angle)
        if(isnan(cheerios[i].angle_contact)){
            cheerios[i].angle_contact = 0;
        }
        cheerios[i].Sigma = CalculSigma(rho_cheerio, rho_liq, cheerios[i].angle_contact);
    }
    if(error){
        printf("WARNING angle de contact experimental\n");
    }
}

// Pour linstant nous ne l'utilison pas
// Retourne l'énergie potentielle entre deux objets.
double EnergiePotentielleEntreDeuxParicles(double surface_tension, double rayon_courbure, double Bond_nb, 
                                            double Sigma, double distance, double capilary_length){
    return -2*M_PI*surface_tension*rayon_courbure*sqrt(pow(Bond_nb,5))*sq(Sigma)*gsl_sf_bessel_K0(distance/capilary_length);
}