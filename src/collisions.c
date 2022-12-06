#include <math.h>
#include "cheerios.h"
#include "calculs.h"

#define COLLISION_ABSORBTION 0.7

// Utilise l'intégration de Verlet pour calculer la nouvelle position, vitesse et accélération d'un objet.
void IntegrationDeVerlet(cheerio_t* cheerio, double dt){
    vec2_t new_pos, new_acc, new_vel;
    new_pos = VecteurAdition(VecteurAdition(cheerio->pos, VectorTimesScalar(cheerio->v, dt)), VectorTimesScalar(cheerio->a, dt*dt*0.5));
    new_acc = VectorTimesScalar(cheerio->f_applique, 1/cheerio->masse);
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
void AppliqueCollision(double distance, cheerio_t* cheerios, int i, int j){
    double vitesse_collision, impulse;
    vec2_t norme_collision, vitesse_relative;
    norme_collision = SensEntre1et2(cheerios[i].pos, cheerios[j].pos, distance);
    vitesse_relative = VecteurSubstraction(cheerios[i].v, cheerios[j].v);
    vitesse_collision = CalculProduitScalaire(vitesse_relative, norme_collision);//vRelativeVelocity.x * vCollisionNorm.x + vRelativeVelocity.y * vCollisionNorm.y;
    // TODO trouver cette constante 
    vitesse_collision *= COLLISION_ABSORBTION;//0.7; // correction* ca depend plus de dt que ca mais quand meme il faux pas le metre trop bas ou trop haut// entre 0.5 et 0.8 car si on met plus haut ca rebondis pas mal et si on mets trop bas ils rentre entre eux// le coefficint qui fait tel que ca robondis pas NE PAS LE METRE TROP BAS CAR CA PEUX ENFONCER DEDANS OU REBONDIR TROP
    // TODO je sais pas pq ici on a ca 
    if(vitesse_collision > 0){
        // Avec le conservation de momentum
        impulse = 2 * vitesse_collision / (cheerios[i].masse + cheerios[j].masse);                                                               // Basic
        cheerios[i].v.x -= (impulse * cheerios[j].masse* norme_collision.x);                                                     // che[i].v.x -= (speed * vCollisionNorm.x);
        cheerios[i].v.y -= (impulse * cheerios[j].masse* norme_collision.y);                                                     // che[i].v.y -= (speed * vCollisionNorm.y);
        cheerios[j].v.x += (impulse * cheerios[i].masse* norme_collision.x);                                                     // che[j].v.x += (speed * vCollisionNorm.x);
        cheerios[j].v.y += (impulse * cheerios[i].masse* norme_collision.y);                                                     // che[j].v.y += (speed * vCollisionNorm.y);
        // On peux aussi utiliser ces fonctions mais ca diminue la lisibilite sans faire gagner quelque chose 
        // cheerios[i].v = VecteurSubstraction(cheerios[i].v, VectorTimesScalar(norme_collision, impulse*cheerios[j].m));
        // cheerios[j].v = VecteurSubstraction(cheerios[j].v, VectorTimesScalar(norme_collision, impulse*cheerios[i].m));
    }
}

// Return autre chose que 0 si la distance entre deux objets est inférieure à l'addition de leurs rayons.
int Collision(double distance, double r1, double r2 ){
    return distance <= r1+r2;
}

int CollisionBord(cheerio_t* cheerio, bord_t bord){
    return CalculDistance(cheerio->pos, bord.centre) + cheerio->diametre_cheerio/2 >=  bord.rayon; 
}

// Ca rotate le vecteur de ange au sens trigonometrique
void RotateVec(vec2_t* vec, double angle){
    vec2_t new_vec = {.x = vec->x * cos(angle) - vec->y * sin(angle),
                      .y = vec->x * sin(angle) + vec->y * cos(angle)};
    *vec = new_vec;
}

void AppliqueCollisionBord(cheerio_t* cheerio, bord_t bord){
    double val = CalculProduitScalaire(cheerio->v, SensEntre1et2(cheerio->pos, bord.centre, CalculDistance(cheerio->pos, bord.centre))) / CalculNorme(cheerio->v);
    //cheerio->v = VectorTimesScalar(cheerio->v, COLLISION_ABSORBTION);
    if(val > 1 || val < -1){  // Si acos nest pas definie on fait seulement inverser le vecteur 
        cheerio->v = VectorTimesScalar(cheerio->v, -1);
    } else{
        double angle = val;
        double angle2 = acos(CalculProduitScalaire(cheerio->v, SensEntre1et2(bord.centre, cheerio->pos,CalculDistance(cheerio->pos, bord.centre))) / (CalculNorme(cheerio->v)*COLLISION_ABSORBTION));
        // TODO ici on a un proble pour vertains angles ca ne marche pas pour certaines ca marche
        if(angle < angle2)
            RotateVec(&(cheerio->v), -angle+angle2);
        else
            RotateVec(&(cheerio->v), angle-angle2);
    }
}