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

typedef struct Objet{
    long int  len_liste;
    long int* liste;
    double m_tot;
} obj_t;

typedef struct Objects{
    long int len;
    obj_t* l_objects;
} objets_t;

// Marhce sulement avec 2 spheres 
// Calcul et modifie la vitesses comme si ils avait fait une collision inelastic parfait (m1+m2)*vf = m1*v1 + m2*v2 => vf = (m1*v1 + m2*v2) / (m1+m2)
void PerfectInelasticCollision(cheerio_t* c1, cheerio_t* c2, int nb_cheerios){ // ptas = plusieurs tas tableau de tas
    double m1 = 0;
    double m2 = 0;
    int i;
    if(c1->tas_id != c2->tas_id && c1->tas_id >= 0 && c2->tas_id >= 0){ // les deux tas se fussionne
        // on les mets dans le tas de c1 cest une decicion quelquonque que on a choisi
        m1 = c1->masses_tas[c1->tas_id];
        m2 = c2->masses_tas[c2->tas_id];
        c1->masses_tas[c1->tas_id] += c2->masses_tas[c2->tas_id];
        c2->masses_tas[c2->tas_id] -= c2->m;
        c2->tas_id = c1->tas_id;            // TODO pas sur de ici si on change un est que ca change tous avec les contacts ? 
    } else {
        if (c1->tas_id < 0 && c2->tas_id < 0){ // si les cheerio na pas de tas en les mets dans un tas 
            for(i = 0; i < nb_cheerios; i++){
                if(c2->masses_tas[i] < 0 && c1->masses_tas[i] < 0){
                    c2->tas_id = i;
                    c1->tas_id = i;
                }
            }
        } else if (c1->tas_id > 0 && c2->tas_id < 0){ // c1 a un tas mais pas c2
            c2->tas_id = c1->tas_id;
            c1->masses_tas[c1->tas_id] += c2->m;
        } else if(c1->tas_id < 0 && c2->tas_id > 0){ // si c2 a un tas mais pas c1
            c1->tas_id = c2->tas_id;
            c2->masses_tas[c2->tas_id] += c1->m;
        }else { // apartient aux meme tas 
            m1 = c1->m;
            m2 = c2->m;
        }
    }

    vec2_t vf = VectorDiviseScalaire(
                            VecteurAdition(VectorTimesScalar(c1->v, m1 ) , VectorTimesScalar( c2->v, m2))
                            ,(m1 +m2));
    c1->v = vf;
    c2->v = vf;
    //InitialiseVec(&(c1->a), 0, 0);
    //InitialiseVec(&(c2->a), 0, 0);
}

// Ne marche pas 
// void PerfectInelasticCollisionTest(cheerio_t* c1, cheerio_t* c2){
//     vec2_t cdm = VectorTimesScalar(SensEntreC1aC2(*c1, *c2), 
//                                         DistanceEntreDeuxCentreDeCheerios(*c1, *c2)/2.);
//     // vf cest la vitesse finale de un 
//     vec2_t vf = VectorDiviseScalaire(
//                             VecteurAdition(VectorTimesScalar(c1->v, c1->m ) , VectorTimesScalar( c2->v, c2->m))
//                             ,(c1->m + c2->m));
//     // InitialiseVec(&(c1->v),0,0); c1->v = vf;
//     // InitialiseVec(&(c2->v),0,0); c2->v = vf;
// }

void why(cheerio_t* c1, cheerio_t* c2){
    vec2_t vf = VectorDiviseScalaire(
                    VecteurAdition(VectorTimesScalar(c1->v, c1->m ) , VectorTimesScalar( c2->v, c2->m))
                    ,(c1->m +c2->m));
    c1->v = vf;
    c2->v = vf;
}

// Ne marche pas
void TroisiemeLoiDeNewton(cheerio_t* c1, cheerio_t* c2){
    vec2_t f1 = VectorTimesScalar(c1->a, c1->m);//c1->f_applique;
    vec2_t f2 = VectorTimesScalar(c2->a, c2->m);//c2->f_applique;
    c1->f_applique = VecteurAdition(c1->f_applique, f2);
    c2->f_applique = VecteurAdition(c2->f_applique, f1);
}

void isThereCollision(cheerio_t* cheerios, int nb_cheerios){
    int i, c;
    for(i = 0; i < nb_cheerios; i++){
        for(c = i+1; c < nb_cheerios; c++){ // i+1 car les points de avant si ils collide on les prend on compte
            // si la distance entre cheerios est plus petit que leur rayon combine on dis que il ya une collision
            //printf("Distance entre %d et %d = %lf\n", i,c, DistanceEntreDeuxCheerios(cheerios[i], cheerios[c]));
            if (0 >= DistanceEntreDeuxCheerios(cheerios[i], cheerios[c])){
                //PerfectInelasticCollision(cheerios+i, cheerios+c, nb_cheerios);
                //PerfectInelasticCollisionTest(cheerios+i, cheerios+c);
                //TroisiemeLoiDeNewton(cheerios + i, cheerios + c);
                why(cheerios + i, cheerios + c);
                if(PRINT_INFO) printf("COLLISION ENTRE %d et %d\n", i, c);
            }
        }
    }
    //UpdateAll(cheerios, nb_cheerios, dt); // on lapelle dans la fonction simulation dans le main
}

// O((n*(n-1)) / 2 )
// for(i = 0; i < n; i++)
//  for(c = i + 1; c < n; c++)