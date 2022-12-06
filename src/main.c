/**
 * @file main.c
 * @authors Baptiste BRAUN-DELVOYE, Erdi ÇAN
 * @brief Version source de notre projet.
 * @version 8.0
 * @date 2022-11-29
 * 
 * @copyright Copyright (c) 2022 // TODO le faire une vrai licence MIT our GNU ca laire de etre pas mal // TODO demander au profle quel et meilleur 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include "cheerios.h"
#include "calculs.h"
#include "collisions.h"
#include "lecture_ecriture.h"

int main(){
    gsl_set_error_handler_off(); // Car si on le desactive pas on peux avoir des underflow et ca ca veux juste dire que notre resultat est plus precis que on peux representer mais pour notre application une precision double sa souffit donc on peux l'arondir 
    // Initialisation des variables en lisant le fichier 
    double rho_liq = 1000., rho_air = 1.1, rho_cheerio = 24.;                  // TODO trouver la masse volumique des cheerios// masses volumiques en kg/m^3 source air https://www.thermexcel.com/french/tables/massair.htm
    double surface_tension_liq_air = 71.99 / 1000.;                                      // tension de surface (gamma) pour l'eau gamma = 72 mN/m = 72/1000 N/m source (https://www.biolinscientific.com/blog/surface-tension-of-water-why-is-it-so-high#:~:text=The%20surface%20tension%20of%20water,highest%20surface%20tension%20for%20liquid.)
    double g = 9.81;
    long int NT;                                                          // nombre de pas de temps 
    double dt;                     // TODO trouver la masse volumique des cheerios// masses volumiques en kg/m^3 source air https://www.thermexcel.com/french/tables/massair.htm
    int nb_cheerios;
    bord_t bord; bord.angle_contact = M_PI/4; bord.rayon_courbure = 2.7/1000.0;
    cheerio_t *cheerios= NULL;                                                  // notre tableaux qui est remplie de cheerios
    cheerios = LectureTouteCheerios("donnees_initiales.txt", &nb_cheerios, &NT, &dt, &rho_liq, &rho_air, &rho_cheerio, &surface_tension_liq_air, &g, &bord);  
    double capilary_length = sqrt(surface_tension_liq_air/(fabs(rho_liq-rho_air)*g)) ;  // capilary lenght = L_c ≡ sqrt(γ/(𝜌*g))  γ = gamma = surface tension//2.7 / 1000; // L_c of water = 2.7 mm https://www.sciencedirect.com/topics/engineering/capillary-length#:~:text=As%20surface%20energy%20is%20related,will%20indeed%20have%20little%20effect.
    InitialiseBondEtSigma(cheerios, nb_cheerios, capilary_length, rho_liq, rho_cheerio, &bord);
    
    VoirSiNotreLectureABienMarche(cheerios, nb_cheerios, NT, dt, rho_liq, rho_air, rho_cheerio, surface_tension_liq_air, g, &bord);
    InitialiseFichierDeEcriture("donnees.txt");                                 // ca efface tout le fichier donnees.txt pour que on a un fichier vide pour les nouvelles donnees 
    printf("L_c = %lf \n",capilary_length);
    
    int i, j;
    double puissance_force, distance;                      
    vec2_t forceAvecDirection, sens;
    // O(NT*(nb*nb+nb*nb+nb)) => O(NT*nb*nb)// pour linstant
    for(long int nt = 0; nt < NT; nt++){                                                    // on itere autant fois que le nombre de pas de temps 
        ProgressBar(nt, NT);
        // printf("\renergie dans le systeme = %lf", energie_totale_dans_le_systeme); fflush(stdout); 
        // TODO ici on peux le faire plus court on faisant telle que quand on calcule les deux cheerios en meme temps ?
        // test de collision et update des forces applique avant on calculait les collisions et apres les forces 
        // en pensant que on pourait reduire la complexite de la fonction collisions a nlog(n) mais comme on a besoin de calculer linteraction entre kes 
        // cheerios on a besoin de calculer en n*n donc notre complexite ne dimunait pas 
        // TODO faire une test de collision en nlogn et en meme temps upload les cheerios en nlogn aussi
        for(i = 0; i < nb_cheerios; i++){
            forceAvecDirection.x = 0;// initialise chaque fois a 0 pour chaque cheerio
            forceAvecDirection.y = 0;
            for(j = 0; j < nb_cheerios; j++){
                if (j != i){ // si cest pas le meme objet car si lobjet ne applique pas de force sur lui meme
                    distance = CalculDistance(cheerios[i].pos, cheerios[j].pos);
                    // On applique les collisions 
                    if( Collision(distance, cheerios[i].rayon_courbure,cheerios[j].rayon_courbure) ){
                        AppliqueCollision(distance, cheerios, i, j);
                    } else { // les cheerios ne se intersect pas donc on applique les forces 
                        // On prend les forces de j qui applique sur i le 
                        puissance_force = ForceBetweenTwoInteractingParticles(surface_tension_liq_air, cheerios[j].rayon_courbure, cheerios[j].Bond_nb, cheerios[j].Sigma, distance, capilary_length);// enlever le - pour une force de attraction
                        sens = SensEntre1et2(cheerios[j].pos, cheerios[i].pos, distance); // maintenant trouver le sens
                        forceAvecDirection = VecteurAdition(forceAvecDirection, VectorTimesScalar(sens, puissance_force)); // on ajoute la nouvelle force a la precedente
                    }
                }
                if( CollisionBord(cheerios+i, bord) ){
                    //printf("Collision bord\n");
                    AppliqueCollisionBord(cheerios+i, bord);
                }
            }
            forceAvecDirection = VecteurAdition(forceAvecDirection, ForceBord(bord, cheerios[i], surface_tension_liq_air, capilary_length));
            cheerios[i].f_applique = forceAvecDirection;
        }

        // Integration de Verlet  // On peux lutiliser cellui ci car notre acceleration depend seulement des interactions entre cheerios  // et que notre acceleration ne depend pas de la vitesse  // on peux pas metre cette boucle dans lautre car sinon ca changerait la position chaque cheerio un par un et les nouvelles positions changerait au cours du temps par rapport ou on comence a calculer 
        UpdatePositions(cheerios, nb_cheerios, dt);

        // TODO a partir de quel moment on a plus de boost de vitesse pour lecriture 
        // avec 11 1000000 0.001 => chaque iteration ~ 53s, 10 ~ 23s, 100 ~ 23s,  1000 ~ 21s, et si on ecris pas ca prend ~ 20s  
        // mais en mem temps plus on a de iterations on a plus de donnes et ca prendplus de place par example on a eu avant de print seulement les 100 iterations des fichiers de 15 GB et comme on les lis 100 par 100 dans python ou plus on a pas besoin de autant de donnees 
        if(nt%10 == 0) 
            EcritureData("donnees.txt", cheerios, nb_cheerios, nt);                             // On fait l'ecriturechawue fois comme ca on a bas besoin de stocker toute les donnees passees. 
    }                  
    free(cheerios);
    return 0;
}