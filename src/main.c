/**
 * @file main.c
 * @authors Baptiste BRAUN-DELVOYE, Erdi ÇAN
 * @brief Version source de notre projet.
 * @version 8.0
 * @date 2022-11-29
 * 
 * @copyright Copyright (c) 2022 // TODO le faire une vrai licence MIT our GNU ca laire de etre pas mal
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
#include "misc.h"

void Simulate(char* fichier_donnees_initiales, char* fichier_donnees);

int main(){
    gsl_set_error_handler_off(); // Car si on ne le désactive pas, on peux avoir des underflow et ça veux juste dire que notre résultat est plus précis que ce que nous pouvons afficher, mais pour notre application une précision double suffit donc on peux l'arrondir.
    char fichier_donnees_initiales[30] = "donnees_initiales.txt";
    char fichier_donnees[30]            = "donnees.txt";
    Simulate(fichier_donnees_initiales, fichier_donnees);
    return 0;
}

void Simulate(char* fichier_donnees_initiales, char* fichier_donnees){
    // Initialisation des variables 
    double rho_liq = 1000., rho_air = 1.1, rho_cheerio = 24.;                   // masses volumiques en kg/m^3
    double surface_tension_liq_air = 71.99 / 1000.;                             // tension de surface (gamma) pour l'eau : gamma = 72 mN/m = 72/1000 N/m 
    double g = 9.81;                                                            // Constante de l'intensité de la pesanteur terrestre.
    long int NT;                                                                // nombre de pas de temps 
    double dt;
    int nb_cheerios;
    bord_t bord;
    bord.rayon_courbure = 2.7/3000.0; // TODO metre un rayon de courbure trouver

    cheerio_t *cheerios= NULL;                                                  // notre tableaux de cheerios.
    // Lecture du fichier de données initiales, fichier de données finales et initialisation des cheerios.
    cheerios = LectureTouteCheerios(fichier_donnees_initiales, 
                                        &nb_cheerios, &NT, &dt, 
                                            &rho_liq, &rho_air, &rho_cheerio, &surface_tension_liq_air, &g, 
                                                &bord);  

    double capilary_length =  CalculLongeurCapilaire(surface_tension_liq_air, 
                                                        rho_liq, rho_air, g);// capilary lenght = L_c ≡ sqrt(γ/(𝜌*g))  γ = gamma = surface tension//2.7 / 1000; // L_c of water = 2.7 mm
    InitialiseBondEtSigma(cheerios, nb_cheerios, capilary_length, rho_liq, rho_cheerio, &bord); // On initialise le nopmbre de Bond et Sigma pour le bord et les cheerios.
    
    VoirSiNotreLectureABienMarche(cheerios, nb_cheerios, NT, dt, rho_liq, rho_air, rho_cheerio, surface_tension_liq_air, g, &bord); // Test pour vérifier si la lecture a bien fonctionné.
    InitialiseFichierDeEcriture(fichier_donnees);                                 // Efface tout le fichier donnees.txt pour qu'on ait un fichier vide pour les nouvelles données.
    printf("L_c = %lf m\n",capilary_length);
    
    // Début du calcul de positions.
    int i, j;
    double puissance_force, distance;                      
    vec2_t forceAvecDirection, sens;
    // O(NT*(nb*nb+nb*nb+nb)) => O(NT*nb*nb)// pour linstant
    for(long int nt = 0; nt < NT; nt++){                      // on itère autant fois que le nombre de pas de temps 
        ProgressBar(nt, NT, dt);
        // test les collision et update des forces applique 
        for(i = 0; i < nb_cheerios; i++){                     // parcour des cheerios dans le quel on cherche la force applique
            forceAvecDirection.x = 0;                         // initialise chaque fois a 0 pour chaque cheerio car a chaque instant on a besoin de recalculer les forces
            forceAvecDirection.y = 0;                         
            for(j = 0; j < nb_cheerios; j++){                 // parcourir cahque cheerio pour calculer la 
                if (j != i){                                  // si ce n'est pas le même objet car lobjet n'applique pas de force sur lui même.
                    distance = CalculDistance(cheerios[i].pos, cheerios[j].pos);
                    // Si nos objets sont trop en contact(enfonce entre eux), cela veux dire qu'après un moment notre simulation n'est plus stable donc on termine la simulation
                    if (Explosion(cheerios[i], cheerios[j], distance)){
                        printf("\nExplossion a pas temps %ld (%.2lfs)\n", 
                                                                    nt, dt*nt);
                        return;
                    }
                    // On applique les collisions s'il y en a.
                    if( Collision(distance, cheerios[i].rayon_courbure,
                                                cheerios[j].rayon_courbure) ){
                        AppliqueCollision(distance, cheerios, i, j);
                    } else { // les cheerios ne se touchent pas pas donc on applique les forces de j qui sont applique sur i 
                        puissance_force = ForceBetweenTwoInteractingParticles(surface_tension_liq_air, cheerios[j].rayon_courbure,
                                                                                 cheerios[j].Bond_nb, cheerios[j].Sigma, distance, capilary_length);
                        sens = SensEntre1et2(cheerios[j].pos, cheerios[i].pos, distance);  // maintenant trouver le sens
                        forceAvecDirection = VecAdition(forceAvecDirection, VecTimesScalar(sens, puissance_force)); // on ajoute la nouvelle force a la précédente
                    }
                }
            }
            // On applique les collisions de bord si il ya des collisons bord
            if( CollisionBord(cheerios+i, bord) ){
                AppliqueCollisionBord(cheerios+i, bord);
            }
            // On ajoute la force des bords sur notre objet
            forceAvecDirection = VecAdition(forceAvecDirection, ForceBord(bord, cheerios[i], surface_tension_liq_air, capilary_length));
            cheerios[i].f_applique = forceAvecDirection; // Initialisation des force des cheerios a la force que on a calcule
        }

        // Integration de Verlet  
        // On peux lutiliser cellui ci car notre acceleration depend seulement des interactions entre cheerios  
        // et que notre acceleration ne depend pas de la vitesse  
        // on peux pas metre cette boucle dans lautre car sinon ca changerait la position chaque cheerio un par un et les nouvelles positions changerait au cours du temps par rapport ou on comence a calculer 
        UpdatePositions(cheerios, nb_cheerios, dt);

        // A partir de quel moment on a plus de boost de vitesse pour lecriture 
        // avec 11 1000000 0.001 => chaque iteration ~ 53s, 10 ~ 23s, 100 ~ 23s,  1000 ~ 21s, et si on ecris pas ca prend ~ 20s pour executer
        if(nt%10 == 0) 
            EcritureData(fichier_donnees, cheerios, nb_cheerios, nt);                             // On fait l'ecriturechawue fois comme ca on a bas besoin de stocker toute les donnees passees. 
    }                  
    free(cheerios);
    return;
}