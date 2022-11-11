/**
 * @file main.c
 * @authors Baptiste BRAUN-DELVOYE, Erdi Çan
 * @brief Fichier principale de notre simulation.
 * @version 5.5
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lecture_ecritureDonnee.h"
#include "calculs.h"
#include "cheerios.h"
#include "collisions.h"

// IMPORTANT!!! TO RUN THIS PROJECT THE GNU SCIENTIFIC LIBRARY IS NEEDED install it here: https://www.gnu.org/software/gsl/ and then suivez les instructions dans le fichier INSTALL pour linstaller si cest installe dans le path standart ca devrait fonctioner sans probleme si le pathe de instalation est change vous devrez besoin de changer le linker pour le makefile aussi
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_errno.h>

// TODO CECI CEST SEULEMENT POUR VOIR DE CE QUE ON A BESOIN ET POUR VOIR SI CA MARCHRAIT MAIS JE NE SUIS PAS SUR DE CE QUE JE FAIS 
// cest pour tester tqt pas baptiste :)
// on comence notre simulation des cheerios 
void Simulate(cheerio_t* cheerios, int nb_cheerios, double rho_liq, double rho_air,
                double rho_cheerio, double surface_tension, double g, 
                    double capilary_length, long int NT, double dt, long long int* warning_counter){
    
    double R = 2.7 / 1000  ;                                                                // TODO trouver le R a partir de lobjet// ~ en mm /1000 = en m // The toroidal shape of a Cheerio complicates the notion of Bond number, but if we take the effective radius based on its volume R*⫽(R12R2)1/3⬇2.7 mm 共where R1⬇2 mm and R2⬇5 mm are the two radii of the torus兲 and Lc⫽2.7 mm for an air–water interface, then B⬇1. This value is within the regime where the gravitational energy of the particles dominates the capillary suction due to the meniscus between them, and so it is crucial that we account for the buoyancy effects to correctly interpret the attractive force.//= CalculCurvature
    double B = CalculLinearBondNumber(R,capilary_length, warning_counter);                  // = CalculBondNumber(rho_cheerio ,rho_liq, R, surface_tension, g, warning_counter);
    //B = CalculBondNumber(rho_cheerio ,rho_liq, R, surface_tension, g, warning_counter);
    double theta = fabs(B) < 0.63 ? asin(M_PI_2 * B) : asin(B);    //M_PI_2 *  // dans larticle ils mets ca mais si nous on le mets ca depase la limite de arcsin [-1,1]                                                  // TODO pour linstant ca change pas par rapport a la proximite et cest symetyrique mais en realite ca depend de la proximite des particules source Lattice Boltzmann simulation of capillary interactions among colloidal particles equation27 //(M_PI * 30) /180;                                                        // TODO faire la funtion qui trouve langle (pour linstant on a une valeur au pif il faux ecrire l'equation pour trouver l'angle)
    //printf("B = %lf \nB_lin = %lf\npi/2 * B = %lf\n%lf\n",CalculBondNumber(rho_cheerio ,rho_liq, R, surface_tension, g, warning_counter),B, M_PI_2 * B ,theta);
    // printf("L_theta = %lf\ntheta = %lf\nT_theta = %lf\n", asin(CalculBondNumber(rho_cheerio ,rho_liq, R, surface_tension, g, warning_counter)), asin(CalculLinearBondNumber(R,capilary_length, warning_counter)),
    //                                                     acos((cheerios[0].m * g)/(2)));
    double Sigma = CalculSigma(rho_cheerio, rho_liq, theta, warning_counter);               // TODO on a besoin de langle de logjet
    double l = 0;                                                                           // lonqueur entre particules (on le calcule dans la boucle)
    int c, c_obj;                                                                           // cheerio, cheerio_objective
    double force = 0;
    vec2_t force_avec_direction, sensDeLaForce;
    // TODO peut etre paraleliser cette fonction 
    for(long int nt = 0; nt < NT; nt++){                                                    // on itere autant fois que le nombre de pas de temps 
        for(c_obj = 0; c_obj < nb_cheerios; c_obj++){                                       // on parcour chaque cheerio pour calculer les forces qui sont excerce sur lui
            InitialiseVec(&force_avec_direction, 0, 0);                                     // on mets 0 au debut car il ya pas de force pour linstant car on a pas fait de calcul 
            for(c = 0; c < nb_cheerios; c++){                                               // on calcul tout les forces par rapport tous les autres cheerios
                if(c != c_obj){                                                             // il exerce pas de force sur lui meme car sinon il essaye de prendre la distance entre lui meme et ca fait tel que l = 0 et l/L_c = 0 => et K1 ne marhe pas a 0
                    l = DistanceEntreDeuxCheerios(cheerios[c_obj], cheerios[c]);            // prints pour voir si ca marche bien// printf("longueur entre %d et %d = %g m = %g cm\n", c_obj, c, l ,l*100);//printf("l = %.16lf L_c = %lf\n", l, capilary_length);//printf("l = %lf L_c = %lf l/L_c = %lf\n", l, capilary_length, l/capilary_length);
                    if(l > 0){                                                              // car si cest 0 notre K1 est 0 et il est pas defini en 0 
                        force = ForceBetweenTwoInteractingParticles(surface_tension, R, B, Sigma, l, capilary_length, warning_counter);
                    } else if (WARNING_MESAGES){
                        if(PRINT_WARNING) printf("WARNING l = %lf and K1 is not defined in %lf so skipped cheerrio nb %d (WARNING de la function Simulate contre obj = %d et c = %d nt = %ld) if the error is shown once it might be when we detect the collision afterwards it corrects itself\n",l, l/capilary_length, c, c_obj, c, nt);
                        (*warning_counter)++;
                    }
                    // sensDeLaForce = SensEntreC1aC2(cheerios[c_obj], cheerios[c]);        // TODO je sais pas pq mais ici cest celui la qui devreit marcher mais si on fait ca on a une force de repulsion ??? cest normal ??? peut etre jai mis le mauvais sens? 
                    sensDeLaForce = SensEntreC1aC2(cheerios[c], cheerios[c_obj]);           // TODO faire tel que a partir de langle et la curvature dde lobjet ca fait tel que ils sais si la force cest repulsive ou atractive
                    force_avec_direction = VecteurAdition(force_avec_direction,  VectorTimesScalar(sensDeLaForce, force));
                }
            }
            cheerios[c_obj].f_applique = force_avec_direction;                              // on mets la force applique dans notre structure //printf("Force aplique au %d cheerio = %g\n", c_obj, CalculNorme(force_avec_direction));
        } 
        isThereCollision(cheerios, nb_cheerios, dt);                                            // TODO faire un e fonction de check  colision pour voire si il ya des colisions il fait tel que il concerve son momentum totale et que ils reste ensemble ou pas si il ya il faux faire des choses (jsp encore) pour linstant ca print seulement si il ya des colllisions ou pas mais ca fait rien
        UpdateAll(cheerios, nb_cheerios, dt);
        EcritureData("donnees.txt", cheerios, nb_cheerios, nt);                             // On fait l'ecriturechawue fois comme ca on a bas besoin de stocker toute les donnees passees. 
    }
}

// pour voir si on a bien fait la lecture du fichier 
void voirSiNotreLectureABienMarche(cheerio_t* cheerios, int nb_cheerios);

//pour tester gsl
void testGsl();


int main(){
    gsl_set_error_handler_off();
    // TODO peut etre faire tel que on prends les variables nescesaire de le meme fichier de donnees ? 
    // On initialise les variables nescesaire 
    long long int warning_counter = 0;                                          // TODO pour linstant les valeurs sont unpeut aliatore il faux les metres dans le meme unite et les bones valeures(peut etre faire tel que on lis les valeures de un fichier? )
    double rho_liq = 1000., rho_air = 1.1, rho_cheerio = 24.;                  // TODO trouver la masse volumique des cheerios// masses volumiques en kg/m^3 source air https://www.thermexcel.com/french/tables/massair.htm
    //double surface_tension = enthalphy_of_vaporisation/(6*sq(atom_diameter)*N_A);            // pas sur que ca soit le meilleur moyen car chaque fois on a besoin de chercher 2 donnees a la place de 1                  // tension de surface (gamma) pour l'eau gamma = 72 mN/m source page 422 Microﬂuidics: Modeling, Mechanics, and Mathematics
    // soit on le mets directement soin on le calcule nous meme
    double surface_tension = 71.99 / 1000;                                      // tension de surface (gamma) pour l'eau gamma = 72 mN/m = 72/1000 N/m source (https://www.biolinscientific.com/blog/surface-tension-of-water-why-is-it-so-high#:~:text=The%20surface%20tension%20of%20water,highest%20surface%20tension%20for%20liquid.)
    double g = 9.81;
    double capilary_lenght = sqrt(surface_tension/(fabs(rho_liq-rho_air)*g));  // capilary lenght = L_c ≡ sqrt(γ/(𝜌*g))  γ = gamma = surface tension//2.7 / 1000; // L_c of water = 2.7 mm https://www.sciencedirect.com/topics/engineering/capillary-length#:~:text=As%20surface%20energy%20is%20related,will%20indeed%20have%20little%20effect.
    printf("cappilary length = %lf\n", capilary_lenght);
    long int NT = 300;                                                          // nombre de pas de temps 
    double dt = 1./600;                                                         // notre pas de temps  // TODO peut etre le metre en const ???
    
    int nb_cheerios = 0;
    cheerio_t *cheerios= NULL;                                                  // notre tableaux qui est remplie de cheerios
    cheerios = LectureTouteCheerios("cheerio_donnees_test.txt", &nb_cheerios, &NT, &dt);  
    
    if(WARNING_MESAGES){
        printf("nb_cheerios = %d\nNT = %ld\ndt = %g\n", nb_cheerios, NT, dt);
        voirSiNotreLectureABienMarche(cheerios, nb_cheerios);
    }
    
    InitialiseFichierDeEcriture("donnees.txt");                                 // ca efface tout le fichier donnees.txt pour que on a un fichier vide pour les nouvelles donnees 

    Simulate(cheerios, nb_cheerios, rho_liq, rho_air, rho_cheerio, 
                surface_tension, g, capilary_lenght, NT, dt, &warning_counter);

    if(WARNING_MESAGES) printf("total warnings = %lld\n", warning_counter); // on print le nombre de warnings pour voir si les conditions que on a choisi sont bien et si le program a bien fonctione
    
    free(cheerios);
    
    return 0;
}

// print les 10 ou tout les donnees des cheerios pour voir si la fonction de lecture de donnee a bien marche ou pas
void voirSiNotreLectureABienMarche(cheerio_t* cheerios, int nb_cheerios){
    int nb_print = nb_cheerios > 10 ? 10 : nb_cheerios; // comme ca si on a beaucour de cherios on afiche pas tout on check maximum les 10 premiers et on assume que si les 10 premiers sont bien fonctione les autres marche aussi
    //          (ternary operator)  ^ ceci est egale a: // if(nb_cheerios > 10) nb_print = 10 ;else  nb_print = nb_cheerios; 
    for(int i = 0; i < nb_print; i++){
        printf("%lf %lf %lf %lf %lf\n", cheerios[i].pos.x, cheerios[i].pos.y, cheerios[i].d, cheerios[i].v.x, cheerios[i].a.x);
    }
}

// pour tester gsl
void testGsl(){
    gsl_sf_result resultat;
    for(double i = 0.00001; i < 0.001; i += 0.00001 ){
        gsl_sf_bessel_K1_e(i, &resultat);
        printf("i = %.16lf %.16lf %.16lf\n", i,gsl_sf_bessel_K1(i), 1/i);
        printf("error is %.16lf\n", resultat.err);
    }
}

// Objects that float at the interface between a liquid and a gas interact because of interfacial deformation and the effect of gravity.
// Despite being a subject with enormous potential for simple, reliable party tricks, the technological implications of the Cheerios effect are far from frivolous. Much research is currently being done to investigate the possibility of using surface tension to induce the self-assembly of smallscale structures. Understanding the way in which particles aggregate at an interface, and hence being able to control the form of the aggregate as well as the dynamics of its formation, may one day enable much simplified manufacture of components of micro-electromechanical systems.
// For floating objects in equilibrium, we must consider the balance of linear momentum both in the flotation plane as well as out of the plane, and in addition the balance of angular momentum in all three directions. Many of the misconceptions in the field arise from considering only some but not all of these balance equations. In particular, neglecting the condition of vertical force balance leads to an underestimation of the importance of the particle’s buoyancy in determining the nature of the interaction.

// TODO 1er
// Collisions entre les objets

// TODO au millieu
// metre un cas pour les bords

// TODO a la fin
// Peut etre voir ou est les forces sur le liquide avec un graph en couleur qui change par rapport au temps


//                         |           |                       
//                         |           |                
//                         |           |                
//                         |           |                
//               theta1 <- |\         /| -> theta2               
//                         |  \....../ |                
//                         |           |                
//                        /|           |\                gas
//            theta1 <- /  |           | \  ->theta2            
// .................../....|...........|..\...................                
//                         |           |                liquid
//                         |           |                
//            1            |     2     |       3               ^ z
//                         |           |                       |
//                         |           |                       |
//                                                             |------> x
//                         <----------->
//                               d          
//  v capilary lenght
// L_c ≡ sqrt(γ/(𝜌*g))   
//            ^ surface tension
// L_c = sqrt(γ/(△𝜌*g))
//                ^ mass density difference of the fluids


// hagën das :)