#ifndef CALCULS_H
#define CALCULS_H

#include "cheerios.h"

double sq(double x);
double cb(double x);
vec2_t VecAdition(vec2_t v1, vec2_t v2);
vec2_t VecSubstraction(vec2_t v1, vec2_t v2);
vec2_t VecTimesScalar(vec2_t vec, double s);
double CalculNorme(vec2_t vec);
double ProduitScalaire(vec2_t v1, vec2_t v2);
vec2_t CalculRotatedVec(vec2_t vec, double angle);
vec2_t SensEntre1et2(vec2_t pos1, vec2_t pos2, double distance);
double CalculDistance(vec2_t pos1, vec2_t pos2);
double CalculLongeurCapilaire(double surface_tension_liq_air, double rho_liq, double rho_air, double g);
double CalculBondNumber(double rho_liquide, double rho_air, 
                            double R, double gamma ,double g);
double CalculLinearBondNumber( double R, double L_c);
double CalculSigma(double rho_flottant, double rho_liquide, double theta);
double EnergiePotentielleEntreDeuxParicles(double surface_tension, double rayon_courbure, double Bond_nb, 
                                            double Sigma, double distance, double capilary_length);
double ForceBetweenTwoInteractingParticles(double surface_tension, double rayon_courbure, double Bond_nb, 
                                            double Sigma, double distance, double capilary_length);
vec2_t ForceBord(bord_t bord, cheerio_t cheerio, double surface_tension, double capilary_length);
void InitialiseBondEtSigma(cheerio_t* cheerios, int nb_cheerios, double capilary_length, double rho_liq, double rho_cheerio, bord_t* bord);

#endif // CALCULS_H