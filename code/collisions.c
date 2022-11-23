/**
 * @file collisions.c
 * @authors Baptiste BRAUN-DELVOYE, Erdi Çan
 * @brief Fichier permettant de calculer et gérer les collisions entre les cheerios.
 * @version 6.0
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <math.h>
#include "cheerios.h"
#include "calculs.h"

// TODO pour linstant ca nous print et ca change rien 
// basic non optimised collision algorithm that checks one by one 

void why(cheerio_t* c1, cheerio_t* c2, double l){
    vec2_t vf = VectorDiviseScalaire(
                    VecteurAdition(VectorTimesScalar(c1->v, c1->m ) , VectorTimesScalar( c2->v, c2->m))
                    ,(c1->m +c2->m));
    c1->v = vf;
    c2->v = vf;

    // TODO ici je pensais qu eca enlevrait le decalemnt mais ca marche pas
    // apres avoir mis les vitesse a jour il faux que on enleve les cheerios de entre eux car si on fait pas ca ils senfonce de plus en plus entre eux
    vec2_t enfoncement = VectorTimesScalar(SensEntreC1aC2(*c1, *c2), l);
    // on va leur devale par rappor a leur masses celui qui a moins de masse ce decale plus 
    vec2_t decalement_par_masse = VectorDiviseScalaire(enfoncement, (c1->m + c2->m));
    // comme le vecteur enfoncement cest de c1  vers c2 c2 va se decaler du meme sens que enfoncement et c1 l'inverse
    // c1->pos = VecteurAdition(c1->pos, VectorTimesScalar(decalement_par_masse, c1->m));
    // c2->pos = VecteurAdition(c2->pos, VectorTimesScalar(decalement_par_masse, c2->m));
}

void isThereCollision(cheerio_t* cheerios, int nb_cheerios){
    int i, c;
    for(i = 0; i < nb_cheerios; i++){
        for(c = i+1; c < nb_cheerios; c++){ // i+1 car les points de avant si ils collide on les prend on compte
            double l = DistanceEntreDeuxCheerios(cheerios[i], cheerios[c]);
            if (0 >= l){
                why(cheerios + i, cheerios + c, l);
                if(PRINT_INFO) printf("COLLISION ENTRE %d et %d\n", i, c);
            }
        }
    }
}
// pour les update on apelle update all dans simulation

// O((n*(n-1)) / 2 )
// for(i = 0; i < n; i++)
//  for(c = i + 1; c < n; c++)