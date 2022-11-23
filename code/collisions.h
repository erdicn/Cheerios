#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "cheerios.h"
void PerfectInelasticCollision(cheerio_t* c1, cheerio_t* c2);
void isThereCollision(cheerio_t* cheerios, int nb_cheerios, bords_t* bords);
void CheckCollisionBord(cheerio_t* cheerio, bords_t* bords);

#endif // COLLISIONS_H