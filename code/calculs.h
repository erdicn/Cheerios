#ifndef CALCULS_H
#define CALCULS_H

double sq(double x);
double cb(double x);
double abs_d(double x);
double cot(double x);
void EqualiseVector(vec2_t* vecObjectif, vec2_t vecEgal);
vec2_t VectorTimesScalar(vec2_t vec, double s);
vec2_t VecteurAdition(vec2_t v1, vec2_t v2);
void InitialiseVec(vec2_t* vec, double x_val, double y_val);
double CalculNorme(vec2_t vec);
vec2_t NormaliseVector(vec2_t vec);
vec2_t SensEntreC1aC2(cheerio_t c1, cheerio_t c2);
void UpdateCheerio(cheerio_t* cheerio, double dt);
void UpdateAll(cheerio_t* cheerios, int nb_cheerios, double dt);
double DistanceEntreDeuxCheerios(cheerio_t c1, cheerio_t c2);
double ForcePerUnitLenght(double surface_tension, double theta1, double theta2, double L_c, double d);
double CalculBuoyancyForce(double rho_liquide, double g, double R, 
                            double z_c, double phi_c);
double CalculBondNumber(double rho_liquide, double rho_air, 
                            double R, double gamma ,double g,long long int* warning_counter);
double CalculLinearBondNumber( double R, double L_c,long long int* warning_counter);
double CalculSigma(double rho_flottant, double rho_liquide, double theta, long long int* warning_counter);
double EnergyBetweenTwoInteractingParticles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c);
double ForceBetweenTwoInteractingParticles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c,long long int* warning_counter);
#endif //CALCULS_H       