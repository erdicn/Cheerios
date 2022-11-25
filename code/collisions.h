#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "cheerios.h"

/**
 * @brief Vérifie s'il y a une collision entre deux objets ou un objet et un bords. Pour tous les objets.
 * 
 * @param cheerios 
 * @param nb_cheerios 
 * @param bords 
 * @param capilary_length
 */
void isThereCollision(cheerio_t* cheerios, int nb_cheerios, bords_t* bords, double capilary_length, double dt);

/**
 * @brief Vérifie si un objet a une collision avec un des bords et annule sa vitesse en fonction de celui-ci. TODO améliorer le réalisme de la chose
 * 
 * @param cheerio 
 * @param bords 
 */
void CheckCollisionBord(cheerio_t* cheerio, bords_t* bords);

#endif // COLLISIONS_H