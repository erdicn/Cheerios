#ifndef COLLISIONS_H
#define COLLISIONS_H
#include "cheerios.h"

void AppliqueCollision(double distance, cheerio_t* cheerios, int i, int j);
int Collision(double distance, double r1, double r2 );
int CollisionBord(cheerio_t* cheerio, bord_t bord);
vec2_t CalculRotatedVec(vec2_t vec, double angle);
void AppliqueCollisionBord(cheerio_t* cheerio, bord_t bord);

#endif // COLLISIONS_H