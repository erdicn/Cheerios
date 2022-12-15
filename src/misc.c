#include "calculs.h"
#include "cheerios.h"
#include <math.h>

// Retourne si notre système a explosé ou non (ça nous dit si notre système est toujours stable ou pas) 
int Explosion(cheerio_t c_i, cheerio_t c_j, double distance){
    return distance < fmin(c_i.diametre_cheerio/4., c_j.diametre_cheerio/4.);
}

// Utilise l'intégration de Verlet pour calculer la nouvelle position, vitesse et accélération d'un objet.
void IntegrationDeVerlet(cheerio_t* cheerio, double dt){
    vec2_t new_pos, new_acc, new_vel; // Nous créons des nouveaux vecteurs de positions, d'accélérations et de vitesse.
    new_pos = VecAdition(VecAdition(cheerio->pos, VecTimesScalar(cheerio->v, dt)), VecTimesScalar(cheerio->a, dt*dt*0.5));
    new_acc = VecTimesScalar(cheerio->f_applique, 1/cheerio->masse);
    //new_vel = VecAdition(cheerio->v, VecTimesScalar(new_acc,(dt)));
    new_vel = VecAdition(cheerio->v, VecTimesScalar(VecAdition(cheerio->a, new_acc),(dt*0.5)));
    cheerio->pos= new_pos;
    cheerio->v  = new_vel;
    cheerio->a  = new_acc;
}

// Met à jour la position des objets en utilisant l'intégration de Verlet.
void UpdatePositions(cheerio_t* cheerios, int nb_cheerios, double dt){
    for(int i = 0; i < nb_cheerios; i++){
        IntegrationDeVerlet(cheerios + i, dt);
    }
}