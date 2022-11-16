#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "cheerios.h"
void PerfectInerlasticCollision(cheerio_t* c1, cheerio_t* c2);
void isThereCollision(cheerio_t* cheerios, int nb_cheerios, double dt);

#endif // COLLISIONS_H