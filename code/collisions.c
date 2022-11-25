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

void PerfectlyInelasticCollision2D(cheerio_t* c1, cheerio_t* c2, double l){
    vec2_t vf = VectorDiviseScalaire(
                    VecteurAdition(VectorTimesScalar(c1->v, c1->m ) , VectorTimesScalar( c2->v, c2->m))
                    ,(c1->m +c2->m));
    c1->v = vf;
    c2->v = vf;

    // TODO ici je pensais qu eca enlevrait le decalemnt mais ca marche pas
    // apres avoir mis les vitesse a jour il faux que on enleve les cheerios de entre eux car si on fait pas ca ils senfonce de plus en plus entre eux
    //vec2_t enfoncement = VectorTimesScalar(SensEntreC1aC2(*c1, *c2), l);
    // on va leur devale par rappor a leur masses celui qui a moins de masse ce decale plus 
    //vec2_t decalement_par_masse = VectorDiviseScalaire(enfoncement, (c1->m + c2->m));
    // comme le vecteur enfoncement cest de c1  vers c2 c2 va se decaler du meme sens que enfoncement et c1 l'inverse
    // c1->pos = VecteurAdition(c1->pos, VectorTimesScalar(decalement_par_masse, c1->m));
    // c2->pos = VecteurAdition(c2->pos, VectorTimesScalar(decalement_par_masse, c2->m));
}

void print(double nb){
    printf("%lf\n",nb);
}
void printvec(vec2_t vec){
    printf("x= %.16lf y= %.16lf\n", vec.x, vec.y);
}



void CorrectionDuEnfoncement(cheerio_t* c1, cheerio_t* c2, double l){
    vec2_t sens12 = SensEntreC1aC2(*c1, *c2);
    vec2_t sens21 = SensEntreC1aC2(*c2, *c1);
    // on veux decaler c1 du sens 21 et c2 du sens de 12 et 
    //print(-l);
    //print(5/100000.);   // TODO  ici je sais pas de ou viens cette constante 
    double l_abs = fabs(l)-5/100000.; // on veux la valeur absolue car on a deja le sens dans le vecteur
    double l_2 = l_abs/2.;
    // print(l);
    // print(sens12.x);
    // print(sens12.y);
    // print(l_2);
    // printvec(VectorTimesScalar(sens12, -l_2));
    // TODO 
    // celui que a plus de energie se decale le moins et lautre se decale plus 
    // pour linstant on les decale du meme facon
    c1->pos = VecteurAdition(c1->pos, VectorTimesScalar(sens21, l_2));
    c2->pos = VecteurAdition(c2->pos, VectorTimesScalar(sens12, l_2));
    //c1->pos = VecteurAdition(c1->pos, VectorTimesScalar(NormaliseVector(c1->v), -l_2));
    //c2->pos = VecteurAdition(c2->pos, VectorTimesScalar(NormaliseVector(c2->v), -l_2));

}

// Vérifie s'il y a une collision entre deux objets ou un objet et un bords. Pour tous les objets.
void isThereCollision(cheerio_t* cheerios, int nb_cheerios, bords_t* bords){
    int i, c;
    double l;
    for(i = 0; i < nb_cheerios; i++){
        for(c = i+1; c < nb_cheerios; c++){ // i+1 car les points de avant si ils collide on les prend on compte
            l = DistanceEntreDeuxCheerios(cheerios[i], cheerios[c]);
            if (0 >= l){
                //print(l);
                CorrectionDuEnfoncement(cheerios + i, cheerios + c, l);
                //PerfectlyInelasticCollision2D(cheerios + i, cheerios + c, l);
                if(PRINT_INFO) printf("COLLISION ENTRE %d et %d\n", i, c);
            }
        }
        CheckCollisionBord(cheerios+i, bords);
    }
}

// Vérifie si un objet a une collision avec un des bords et annule sa vitesse en fonction de celui-ci. TODO améliorer le réalisme de la chose
void CheckCollisionBord(cheerio_t* cheerio, bords_t* bords){
    if ((cheerio->pos.x)-(cheerio->d)/2 <= bords->gauche || (cheerio->pos.x)+(cheerio->d)/2 >= bords->droite)
    {
       cheerio->v.x = 0; 
    }
    if ((cheerio->pos.y)-(cheerio->d)/2 <= bords->bas || (cheerio->pos.y)+(cheerio->d) >= bords->haut)
    {
        cheerio->v.y = 0;
    }
    
    
    
}
// pour les update on apelle update all dans simulation

// O((n*(n-1)) / 2 )
// for(i = 0; i < n; i++)
//  for(c = i + 1; c < n; c++)