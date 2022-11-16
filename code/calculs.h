#ifndef CALCULS_H
#define CALCULS_H

/**
 * @brief Retourne le carré de x.
 * 
 * @param x 
 * @return double 
 */
double sq(double x);

/**
 * @brief Retourne le cube de x.
 * 
 * @param x 
 * @return double 
 */
double cb(double x);

/**
 * @brief Retourne le cotangeant d'un angle x.
 * 
 * @param x 
 * @return double 
 */
double cot(double x);

/**
 * @brief Retourne un nouveau vecteur à 2 coordonnées créé à partir de l'addition des coordonnées des vecteurs v1 et v2.
 * 
 * @param v1 
 * @param v2 
 * @return vec2_t 
 */
void EqualiseVector(vec2_t* vecObjectif, vec2_t vecEgal);

/**
 * @brief Retourne le vecteur à 2 dimensions vec avec ses coordonnées initiales divisées par s.
 * 
 * @param vec 
 * @param s 
 * @return vec2_t 
 */
vec2_t VectorDiviseScalaire(vec2_t vec, double s);

/**
 * @brief Retourne le vecteur à 2 dimensions vec avec ses coordonnées initiales multipliées par s.
 * 
 * @param vec 
 * @param s 
 * @return vec2_t 
 */
vec2_t VectorTimesScalar(vec2_t vec, double s);

/**
 * @brief Retourne un nouveau vecteur à 2 coordonnées créé à partir de l'addition des coordonnées des vecteurs v1 et v2.
 * 
 * @param v1 
 * @param v2 
 * @return vec2_t 
 */
vec2_t VecteurAdition(vec2_t v1, vec2_t v2);

/**
 * @brief Initialise un vecteur avec x_val et y_val.
 * 
 * @param vec 
 * @param x_val 
 * @param y_val 
 * */
void InitialiseVec(vec2_t* vec, double x_val, double y_val);

/**
 * @brief Retourne la norme du vecteur à 2 dimensions vec.
 * 
 * @param vec 
 * @return double 
 */
double CalculNorme(vec2_t vec);

/**
 * @brief Retourne la normalisation d'un vecteur, vec, à 2 dimensions en divisant ses coorodnnées par la norme de ce vecteur.
 * 
 * @param vec 
 * @return vec2_t 
 */
vec2_t NormaliseVector(vec2_t vec);

/**
 * @brief Retourne un vecteur normalisé de la direction(sens) de c1 vers c2.
 * 
 * @param c1 
 * @param c2 
 * @return vec2_t 
 */
vec2_t SensEntreC1aC2(cheerio_t c1, cheerio_t c2);

/**
 * @brief Met à jour la position, la vitesse et l'accélération du cheerio en utilisant l'intégratioon de Verlet (TODO) et la force de trainée https://en.wikipedia.org/wiki/Verlet_integration 
 * @param cheerio 
 * @param dt 
 */
void UpdateCheerio(cheerio_t* cheerio, double dt);

/**
 * @brief Parcours chaque cheerio et met à jour leurs positions.
 * 
 * @param cheerios 
 * @param nb_cheerios 
 * @param dt 
 */
void UpdateAll(cheerio_t* cheerios, int nb_cheerios, double dt);

/**
 * @brief Retourne la distance entre le centre de deux cheerios, c1 et c2.
 * 
 * @param c1 
 * @param c2 
 * @return double 
 */
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

// force of interaction is given by F(l) = -dE/dl or :
/**
 * @brief Retourne la force d'intéraction entre deux particules.
 * 
 * @param gamma 
 * @param R 
 * @param B 
 * @param Sigma 
 * @param l 
 * @param L_c 
 * @param warning_counter 
 * @return double 
 */
double ForceBetweenTwoInteractingParticles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c,long long int* warning_counter);
#endif //CALCULS_H       