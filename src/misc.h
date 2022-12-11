#ifndef MISC_H
#define MISC_H

#include "cheerios.h"

int Explosion(cheerio_t c_i, cheerio_t c_j, double distance);
void IntegrationDeVerlet(cheerio_t* cheerio, double dt);
void UpdatePositions(cheerio_t* cheerios, int nb_cheerios, double dt);

#endif //MISC_H