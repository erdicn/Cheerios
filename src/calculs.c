#include "cheerios.h"
#include <math.h>
#include <gsl/gsl_sf_bessel.h>


// Retourne le carré de x.
double sq(double x){
    return x*x;
}

// Retourne le cube de x.
double cb(double x){
    return x*x*x;
}

// Retourne un nouveau vecteur qui est l'addition des 2 vecteurs v1 et v2.
vec2_t VecteurAdition(vec2_t v1, vec2_t v2){
    vec2_t new_vec = { .x = v1.x + v2.x,
                       .y = v1.y + v2.y};
    return new_vec;
}

vec2_t VecteurSubstraction(vec2_t v1, vec2_t v2){
    vec2_t new_vec = { .x = v1.x - v2.x,
                       .y = v1.y - v2.y};
    return new_vec;
}

// Retourne le vecteur vec avec ses coordonnées multipliés par la valeur s.
vec2_t VectorTimesScalar(vec2_t vec, double s){
    vec2_t new_vec = { .x = vec.x * s,
                       .y = vec.y * s};
    return new_vec;
}

// Retourne la norme du vecteur à 2 dimensions vec.
double CalculNorme(vec2_t vec){
    return sqrt(sq(vec.x) + sq(vec.y));
}

double CalculProduitScalaire(vec2_t v1, vec2_t v2){
    return v1.x*v2.x + v1.y*v2.y;
}

// retourne le vecteur unitaire du sens entre 1 et 2
vec2_t SensEntre1et2(vec2_t pos1, vec2_t pos2, double distance){
    vec2_t sens12 = {.x = (pos2.x - pos1.x)/distance,
                     .y = (pos2.y - pos1.y)/distance};
    return sens12;
}

// Retourne la distance entre deux objets.
double CalculDistance(vec2_t pos1, vec2_t pos2){
    return sqrt(sq(pos2.x - pos1.x) + sq(pos2.y - pos1.y)) ;
}

// TODO je sais pas si le bond number ca marche quand cest plus grand que 0.3
double CalculBondNumber(double rho_liquide, double rho_air, 
                            double R, double gamma ,double g){
    double delta_rho = fabs(rho_liquide-rho_air);   //fabs = valeur absolue pour les doubles 
    double B = (delta_rho * g * sq(R)) / gamma;
    return B;
}

// Retourne le Bond Number calculé de façon linéaire.
double CalculLinearBondNumber( double R, double L_c){
    double B = sq(R)/sq(L_c);
    return B;
}

// Retourne le calcul appelé Sigma.
double CalculSigma(double rho_flottant, double rho_liquide, double theta){
    double D = rho_flottant/rho_liquide; // flottant cest notre objet
    return ((2*D-1) / 3.0) - 0.5*cos(theta) + (1/6.0)*cb(cos(theta));
}

// Retourne l'énergie potentielle entre deux objets.
double EnergiePotentielleEntreDeuxParicles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c){
    return -2*M_PI*gamma*R*sqrt(pow(B,5))*sq(Sigma)*gsl_sf_bessel_K0(l/L_c);
}

/**
 * @brief Retourne la valeur de la force d'intéraction entre deux particules. -dE/dl
 * 
 * @param gamma Tension de Surface
 * @param R Rayon de courbure
 * @param B Bond Number
 * @param Sigma 
 * @param l Distance entre les deux particules
 * @param L_c Longueur Capillaire
 * @return double: la force entre deux particles 
 */
double ForceBetweenTwoInteractingParticles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c){
    // precondition l > 0 
    return -2*M_PI*gamma*R*sqrt(pow(B,5))*sq(Sigma)*gsl_sf_bessel_K1(l/L_c);
}


// Retourne la force émise par les bords sur une particule. // TODO Actuellement pas utilisée, à vérifier si cela est utile.
double ForceBord(bord_t bord, cheerio_t cheerio, double gamma, double lc, double rho_l, double rho_air, double g){
    double distCh_Ce = CalculDistance(cheerio.pos, bord.centre);
    double BN = CalculBondNumber(rho_l, rho_air, bord.R, gamma, g);
    double Sigma = CalculSigma(bord.rho, rho_l, bord.theta);
    double Force = ForceBetweenTwoInteractingParticles(gamma, bord.R, BN, Sigma, distCh_Ce, lc);
    return Force;
}



// // Retourne l'énergie cinétique d'un objet
// double CalculEnergieCinetique(cheerio_t* cheerio){
//     cheerio->E.Ec = 0.5*cheerio->m*sq(CalculNorme(cheerio->v));
//     return cheerio->E.Ec;
// }

// // Retourne l'énergie mécanique d'un objet.
// double CalculEnergieMecanique(cheerio_t* cheerio){
//     cheerio->E.Em = cheerio->E.Ec + cheerio->E.Ep;
//     return cheerio->E.Em;
// }

// Initialise les valeurs de Bond Number et de Sigma pour tous les objets.
void InitialiseBondEtSigma(cheerio_t* cheerios, int nb_cheerios, double capilary_length, double rho_liq, double rho_cheerio){
    double tmp_B, tmp_theta;
    // on mets les donnees calcule dans les cheerios 
    for(int i = 0; i < nb_cheerios; i++){
        cheerios[i].Bo = CalculLinearBondNumber(cheerios[i].R, capilary_length);//CalculBondNumber(rho_liq, rho_air, cheerios[i].R, surface_tension, g); 
        tmp_B = cheerios[i].Bo;
        cheerios[i].theta = fabs(tmp_B) < 0.63 ? asin(M_PI_2 * tmp_B) : asin(tmp_B);    //M_PI_2 *  // dans larticle ils mets ca mais si nous on le mets ca depase la limite de arcsin [-1,1]                                                  // TODO pour linstant ca change pas par rapport a la proximite et cest symetyrique mais en realite ca depend de la proximite des particules source Lattice Boltzmann simulation of capillary interactions among colloidal particles equation27 //(M_PI * 30) /180;                                                        // TODO faire la funtion qui trouve langle (pour linstant on a une valeur au pif il faux ecrire l'equation pour trouver l'angle)
        tmp_theta = cheerios[i].theta;
        cheerios[i].Sigma = CalculSigma(rho_cheerio, rho_liq, tmp_theta);
    }
}