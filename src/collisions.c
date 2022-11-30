#include <math.h>
#include "cheerios.h"
#include "calculs.h"
// Utilise l'intégration de Verlet pour calculer la nouvelle position, vitesse et accélération d'un objet.
void IntegrationDeVerlet(cheerio_t* cheerio, double dt){
    vec2_t new_pos, new_acc, new_vel;
    new_pos = VecteurAdition(VecteurAdition(cheerio->pos, VectorTimesScalar(cheerio->v, dt)), VectorTimesScalar(cheerio->a, dt*dt*0.5));
    new_acc = VectorTimesScalar(cheerio->f_applique, 1/cheerio->m);
    new_vel = VecteurAdition(cheerio->v, VectorTimesScalar(VecteurAdition(cheerio->a, new_acc),(dt*0.5)));
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

// Applique aux objets l'effet de la collision en changenat les vitesses.
void AppliqueCollision(double distance, cheerio_t* cheerios, int i, int j, double dt){
    double speed, impulse;
    vec2_t vCollision, vCollisionNorm, vRelativeVelocity;
    vCollision.x = cheerios[j].pos.x - cheerios[i].pos.x;
    vCollision.y = cheerios[j].pos.y - cheerios[i].pos.y;
    vCollisionNorm.x = vCollision.x/distance,
    vCollisionNorm.y = vCollision.y/distance;
    vRelativeVelocity.x = cheerios[i].v.x - cheerios[j].v.x;
    vRelativeVelocity.y = cheerios[i].v.y - cheerios[j].v.y;
    speed = vRelativeVelocity.x * vCollisionNorm.x + vRelativeVelocity.y * vCollisionNorm.y;
    // TODO trouver cette constante 
    speed *= 7*dt*100;//0.7; // correction* ca depend plus de dt que ca mais quand meme il faux pas le metre trop bas ou trop haut// entre 0.5 et 0.8 car si on met plus haut ca rebondis pas mal et si on mets trop bas ils rentre entre eux// le coefficint qui fait tel que ca robondis pas NE PAS LE METRE TROP BAS CAR CA PEUX ENFONCER DEDANS OU REBONDIR TROP
    if(speed > 0){
        // Avec le conservation de momentum
        impulse = 2 * speed / (cheerios[i].m + cheerios[j].m);            // Basic
        cheerios[i].v.x -= (impulse * cheerios[j].m* vCollisionNorm.x);   // che[i].v.x -= (speed * vCollisionNorm.x);
        cheerios[i].v.y -= (impulse * cheerios[j].m* vCollisionNorm.y);   // che[i].v.y -= (speed * vCollisionNorm.y);
        cheerios[j].v.x += (impulse * cheerios[i].m* vCollisionNorm.x);   // che[j].v.x += (speed * vCollisionNorm.x);
        cheerios[j].v.y += (impulse * cheerios[i].m* vCollisionNorm.y);   // che[j].v.y += (speed * vCollisionNorm.y);
    }
}

// Return autre chose que 0 si la distance entre deux objets est inférieure à l'addition de leurs rayons.
int Collision(double distance, double r1, double r2 ){
    return distance  <= r1+r2;
}