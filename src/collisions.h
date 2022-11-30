#ifndef COLLISIONS_H
#define COLLISIONS_H
#include "cheerios.h"

void IntegrationDeVerlet(cheerio_t* cheerio, double dt);
void UpdatePositions(cheerio_t* cheerios, int nb_cheerios, double dt);
void AppliqueCollision(double distance, cheerio_t* cheerios, int i, int j, double dt);
int Collision(double distance, double r1, double r2 );
#endif // COLLISIONS_H