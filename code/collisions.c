/**
 * @file collisions.c
 * @authors Baptiste BRAUN-DELVOYE, Erdi Çan
 * @brief Fichier permettant de calculer et gérer les collisions entre les cheerios.
 * @version 5.8
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include "cheerios.h"
#include "calculs.h"

// TODO pour linstant ca nous print et ca change rien 
// basic non optimised collision algorithm that checks one by one 

// Calcul et modifie la vitesses comme si ils avait fait une collision inelastic parfait (m1+m2)*vf = m1*v1 + m2*v2 =>
void PerfectInerlasticCollision(cheerio_t* c1, cheerio_t* c2){
    vec2_t vf = VectorDiviseScalaire(
                            VecteurAdition(VectorTimesScalar(c1->v, c1->m ) , VectorTimesScalar( c2->v, c2->m))
                            ,(c1->m + c2->m));
    c1->v = vf;
    c2->v = vf;
    //InitialiseVec(&(c1->a), 0, 0);
    //InitialiseVec(&(c2->a), 0, 0);
}

void isThereCollision(cheerio_t* cheerios, int nb_cheerios, double dt){
    int i, c;
    for(i = 0; i < nb_cheerios; i++){
        for(c = i+1; c < nb_cheerios; c++){ // i+1 car les points de avant si ils collide on les prend on compte
            // si la distance entre cheerios est plus petit que leur rayon combine on dis que il ya une collision
            //printf("Distance entre %d et %d = %lf\n", i,c, DistanceEntreDeuxCheerios(cheerios[i], cheerios[c]));
            if (0 >= DistanceEntreDeuxCheerios(cheerios[i], cheerios[c])){
                //printf("\n\n\nDistance entre %d et %d = %lf\n\n\n", i,c, DistanceEntreDeuxCheerios(cheerios[i], cheerios[c]));
                // cheerios[i].v          = NormaliseVector(VectorTimesScalar(cheerios[i].v,-1));
                // cheerios[i].a          = NormaliseVector(VectorTimesScalar(cheerios[i].a,-1));
                // cheerios[i].f_applique = NormaliseVector(VectorTimesScalar(cheerios[i].f_applique,-1));
                // cheerios[c].v          = NormaliseVector(VectorTimesScalar(cheerios[c].a,-1));
                // cheerios[c].a          = NormaliseVector(VectorTimesScalar(cheerios[c].a,-1));
                // cheerios[c].f_applique = NormaliseVector(VectorTimesScalar(cheerios[c].f_applique,-1));
                PerfectInerlasticCollision(cheerios+i, cheerios+c);
                //printf("COLLISION ENTRE %d et %d\n", i, c);
                if(PRINT_INFO) printf("COLLISION ENTRE %d et %d\n", i, c);
            }
        }
    }
    //UpdateAll(cheerios, nb_cheerios, dt); // on lapelle dans la fonction simulation dans le main
}

// O((n*(n-1)) / 2 )
// for(i = 0; i < n; i++)
//  for(c = i + 1; c < n; c++)