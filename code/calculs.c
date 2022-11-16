<<<<<<< HEAD
/**
 * @file calculs.c
 * @authors Bapiste BRAUN-DELVOYE, Erdi Çan
 * @brief Fichier où toutes les fonctions de calculs sont.
 * @version 5.5
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <math.h>
#include <gsl/gsl_sf_bessel.h> // pour la fonction de bessel modifiee K_1(x)
#include <stdio.h>
#include "cheerios.h"


/**
 * @brief Retourne le carré de x.
 * 
 * @param x 
 * @return double 
 */
double sq(double x){
    return x*x;
}

/**
 * @brief Retourne le cube de x.
 * 
 * @param x 
 * @return double 
 */
double cb(double x){
    return x*x*x;
}

/**
 * @brief Retourne le cotangeant d'un angle x.
 * 
 * @param x 
 * @return double 
 */
double cot(double x){
    return cos(x)/sin(x);
}

// On peux mettre directement vec = vec2 et ça le fait automatiquement donc on a pas besoin sauf si on va faire des allocations de memoire 
void EqualiseVector(vec2_t* vecObjectif, vec2_t vecEgal){
    vecObjectif->x = vecEgal.x;
    vecObjectif->y = vecEgal.y;
}

/**
 * @brief Retourne le vecteur à 2 dimensions vec avec ses coordonnées initiales divisées par s.
 * 
 * @param vec 
 * @param s 
 * @return vec2_t 
 */
vec2_t VectorDiviseScalaire(vec2_t vec, double s){
    vec2_t new_vec;
    new_vec.x = vec.x / s;
    new_vec.y = vec.y / s;
    return new_vec;
}

/**
 * @brief Retourne le vecteur à 2 dimensions vec avec ses coordonnées initiales multipliées par s.
 * 
 * @param vec 
 * @param s 
 * @return vec2_t 
 */
vec2_t VectorTimesScalar(vec2_t vec, double s){
    vec2_t new_vec;
    new_vec.x = vec.x * s;
    new_vec.y = vec.y * s;
    return new_vec;
}

/**
 * @brief Retourne un nouveau vecteur à 2 coordonnées créé à partir de l'addition des coordonnées des vecteurs v1 et v2.
 * 
 * @param v1 
 * @param v2 
 * @return vec2_t 
 */
vec2_t VecteurAdition(vec2_t v1, vec2_t v2){
    vec2_t vecAditione;
    vecAditione.x = v1.x + v2.x;
    vecAditione.y = v1.y + v2.y;
    return vecAditione;
}

/**
 * @brief Initialise un vecteur avec x_val et y_val.
 * 
 * @param vec 
 * @param x_val 
 * @param y_val 
 * */
void InitialiseVec(vec2_t* vec, double x_val, double y_val){
    vec->x = x_val;
    vec->y = y_val;
}

/**
 * @brief Retourne la norme du vecteur à 2 dimensions vec.
 * 
 * @param vec 
 * @return double 
 */
double CalculNorme(vec2_t vec){
    return sqrt(sq(vec.x) + sq(vec.y));
}

/**
 * @brief Retourne la normalisation d'un vecteur, vec, à 2 dimensions en divisant ses coorodnnées par la norme de ce vecteur.
 * 
 * @param vec 
 * @return vec2_t 
 */
vec2_t NormaliseVector(vec2_t vec){
    double norme = CalculNorme(vec);
    vec2_t normalise;
    normalise.x = vec.x/norme;
    normalise.y = vec.y/norme;
    return normalise;
}

/**
 * @brief Retourne un vecteur normalisé de la direction(sens) de c1 vers c2.
 * 
 * @param c1 
 * @param c2 
 * @return vec2_t 
 */
vec2_t SensEntreC1aC2(cheerio_t c1, cheerio_t c2){     
    vec2_t sens;
    sens.x = c2.pos.x - c1.pos.x;
    sens.y = c2.pos.y - c1.pos.y;
    return NormaliseVector(sens);
}

/**
 * @brief Retourne la distance entre le centre de deux cheerios, c1 et c2.
 * 
 * @param c1 
 * @param c2 
 * @return double 
 */
double DistanceEntreDeuxCheerios(cheerio_t c1, cheerio_t c2){
    return sqrt(sq(c1.pos.x - c2.pos.x) + sq(c1.pos.y - c2.pos.y)) - c1.d/2 - c2.d/2; // on enleve la distance de leur rayons
}

/**
 * @brief Met à jour la position, la vitesse et l'accélération du cheerio en utilisant l'intégratioon de Verlet (TODO) et la force de trainée https://en.wikipedia.org/wiki/Verlet_integration 
 * @todo Intégration de Verlet
 * @param cheerio 
 * @param dt 
 */
void UpdateCheerio(cheerio_t* cheerio, double dt){
    vec2_t new_acc = VectorDiviseScalaire(cheerio->f_applique, cheerio->m); // F = ma => a = F/m
    // TODO faire tel que on prend en compte la force de drag aussi
    vec2_t new_pos = VecteurAdition(VecteurAdition(cheerio->pos, VectorTimesScalar(cheerio->v, dt)), VectorTimesScalar(cheerio->a, (dt * dt * 0.5))); // new_pos = pos + V*dt + A*dt*dt*0.5 // si on etait en C++ on pourait faire des operator overloading et specialiser quand on utilise loperateur + entre deux vecteur qu ca ais ca automatiqueent sans ecrire les fonctions
    vec2_t new_vel = VecteurAdition(cheerio->v, VectorTimesScalar(VecteurAdition(cheerio->a, new_acc), dt*0.5));  // new_vel = V + (A+newA)*dt*0.5
    // on mets les nouveaux positions dans notre cheerios (objet)
    cheerio->a = new_acc;
    cheerio->v = new_vel;
    cheerio->pos = new_pos;
}

/**
 * @brief Parcours chaque cheerio et met à jour leurs positions.
 * 
 * @param cheerios 
 * @param nb_cheerios 
 * @param dt 
 */
void UpdateAll(cheerio_t* cheerios, int nb_cheerios, double dt){
    for(int c = 0; c < nb_cheerios; c++){
        UpdateCheerio(cheerios+c, dt);
        //                 ^ pointer arithmetic meme chose que &(cheerios[c])
    }
}

// gamma = tension de surface, R = longueur caractéristique de courbure, (par exemple les raypons de courbure d'une goutte)
// B = Bond number = (rho * g * R * R) / gamma // le plus important, paramètre sans dimension // TODO source Cherrios 2005
double CalculBondNumber(double rho_liquide, double rho_air, 
                            double R, double gamma ,double g,long long int* warning_counter){
    double delta_rho = fabs(rho_liquide-rho_air);   //fabs = valeur absolue pour les doubles 
    double B = (delta_rho * g * sq(R)) / gamma;
    if (B > 0.3 && WARNING_MESAGES) {                                  // TODO pas sur de ici 
        if(PRINT_WARNING) {
            printf("WARNING Bond Number = %lf > 0.3 et lequation 9 de Cherrios est valable seulement quand B << 1 ", B); // The expression for the slope of the interface in the vicinity of the spherical particle given in Eq. 共9兲 is valid for BⰆ1 共corresponding to a radius of ⬃1mm or smaller for a sphere at an air–water interface兲, in which case surface tension is very important.
            printf("(WARNING de la fonction CalculBondNumber(%g, %g, %g, %g, %g)\n", rho_liquide,  rho_air,  R,  gamma , g );
        }
        (*warning_counter)++;
    }
    return B;
}


double CalculLinearBondNumber( double R, double L_c,long long int* warning_counter){
    double B = sq(R)/sq(L_c);
    if (B > 0.3 && WARNING_MESAGES) {                                  // TODO pas sur de ici 
        if(PRINT_WARNING) {
            printf("WARNING Bond Number = %lf > 0.3 et lequation 9 de Cherrios est valable seulement quand B << 1 ", B); // The expression for the slope of the interface in the vicinity of the spherical particle given in Eq. 共9兲 is valid for BⰆ1 共corresponding to a radius of ⬃1mm or smaller for a sphere at an air–water interface兲, in which case surface tension is very important.
            printf("(WARNING de la fonction CalculLinearisedBondNumber(%g, %g)\n", R, L_c );
        }
        (*warning_counter)++;
    }
    return B;
}

// Poids effectif d'une sphère (sans dimension) = 2*pi*gamma*R*B*Sigma
// z_c'*sin(fi_c) = B*Sigma
// Sigma = ((2*D-1) / 3 ) - 0.5*cos(theta) + (1.0/6)*cos(theta)*cos(theta)*cos(theta)
double CalculSigma(double rho_flottant, double rho_liquide, double theta, long long int* warning_counter){
    double D = rho_flottant/rho_liquide; // flottant cest notre objet
    if (theta > 2*M_PI && WARNING_MESAGES){ // message de erreur car si  theta est plus grand que ca ca veux dire que soit la valeur nest pas en bone valeurs
        if(PRINT_WARNING) {
            printf("WARNING ettes vous sur que vouz avez rentre l'angle(thetha) en radian car theta = %lf radians (%lf degrees)",theta, (theta*180)/M_PI);
            printf("(WARRNING de la fonction CalculSigma(%lf, %lf, %lf))\n",rho_flottant, rho_liquide, theta);
        }
        (*warning_counter)++;
    }
    return ((2*D-1) / 3.0) - 0.5*cos(theta) + (1/6.0)*cb(cos(theta));
}

// force of interaction is given by F(l) = -dE/dl or :
/**
 * @brief Retourne la force d'intéraction entre deux particules.
 * 
 * @param gamma 
 * @param R 
 * @param B 
 * @param Sigma 
 * @param l 
 * @param L_c 
 * @param warning_counter 
 * @return double 
 */
double ForceBetweenTwoInteractingParticles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c,long long int* warning_counter){
    // precondition l > 0 
    double l_over_L_c = l/L_c;
    if(l_over_L_c > 36 && WARNING_MESAGES){
        if(PRINT_INFO && PRINT_INFO) printf("INFO l/L_c = %lf > 36 and modified bessel function K1 goes to 0 so thee is no force at that length ( function not calculated since sometimes it might cause underflow)\n",l_over_L_c);
        //return 0;
    } // if it is smaller than 36 calculates itself 
    return -2*M_PI*gamma*R*sqrt(pow(B,5))*sq(Sigma)*gsl_sf_bessel_K1(l/L_c);
}


// EXTRA des fiqures en asci art :)

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

//         |  P_atm |       ___ |        | ___             
//         |\______/|          \|  P_atm |/                    
//         |        |           |        |             
//         |        |           | ______ |             
//        /|        |\          |/      \|             
//    ___/ |P>P_atm | \__       |P<P_atm |   
//
//     two wetting plates    Two nonwetting plates  

// Dézoomer pour voir la figure
//                                                                                                                                                                                       :^^^^^^^^^^^^:                                                                                                                                                                                            
//                                                                                                                                                                          .::~!?JJJYYPPGPPPPPYJ5PPPPPPPYYJJJ?!~::.                                                                                                                                                                               
//                                                                                                                                                                    :^!JY5PPYJJ?!~:::...             ..:::~!?JYYPP5Y?~^.                                                                                                                                                                         
//                                                                                                                                                               :~?5GP5J7~:.                                      .:~7Y5P557~.                                                                                                                                                                    
//                                                                                                                                                           ^75PPY?!:                                                    :!75PPJ!:                                                                                                                                                                
//                                                                                                                                                       :!YPPY!^                                                              ^!YPPY!:                                                                                                                                                            
//                                                                                                                                                    :?PGY!:                                                                      :!YGP?^                                                                                                                                                         
//                                                                                                                                                 ^?GP?^                                                                             .^JPG?^                                                                                                                                                      
//                                                                                                                                              :7GPJ^                                                                                    ^JGG?:                                                                                                                                                   
//                                                                                                                                            ~YBY~                                                                                          !5BY^                                                                                                                                                 
//                                                                                                                                          7GBJ:                                                                                              .7PP7                                                                                                                                               
//                                                                                                                                       .?BG!                                                                                                    !GB7                                                                                                                                             
//                                                                                                                                      !GB!                                                                                                        ~PG!                                                                                                                                           
//                                                                                                                                    ~PG~                                                                                                            7BG^                                                                                                                                         
//                                                                                                                                  :P&?                                                                                                               .?BY:                                                                                                                                       
//                                                                                                                                 7#5:                                                                                                                  :P#!                                             .                                                                                        
//                                                                                                                               :PB!                                                                                                                      7&Y.                                          ~!                                                                                        
//                                                                                                                              ~#G:                                                                                                                        :G#^                                        !~                                                                                         
//                                                                                                                             7&Y                                                             .                                                             .Y&!                                      ~^                                                                                          
//                                                                                                                            J&7                                                            !7:!?.                                                            ?&?                                                                                                                                 
//                                                                                                                           Y&~                                                            ^B.  G?                                                             !&J                                                                                                                                
//                                                                                                                          J&!                                                             ~@7.^J^                                                              !&?                               ~~                                                                                              
//                                                                                                                         ?@7                                                              ~&:.:  ~~:                                                            7@?                             !~                                                                                               
//                                                                                                                        ~&?                                                               :7     !J!                                                             Y&~                           !^                                                                                                
//                                                                             .J:                                       :&P                                                                       . .                                                              PB.                                                                                                                            
//                        ..     .:.    .^.                                    .P:                                       5&:                                                                                                                                        :&Y                                                                                                                            
//                      7J!~5P .?!!J5.:P77?~                                   .P:                                      !@?                                                                                                                                          J@~                     ^                                                                                                     
//                      #!  !# :J!~?&^.J?!?7                                   .P:                                      P#.                                                                                                                                           BP                   .!^                                                                                                     
//                      JJ^^5# ?#~~?&!:Y~^JP.                                  .P:                                     ~@7                                                                                                                                            J@^                 .7:                                                                                                      
//                      ^Y!~YP  :~^.:: :~!^.                                   .P:                                     5@^                                                                                                                                            :&Y                 ..                                                                                                       
//                      :?77~.                                                 .P:                                    .B5                                                                                                                                              5G                                                                                                                          
//                                                                             .P:                                    ~@7                                                                    .PPGY^                       :7^^~.                                       ?&^             :                                                                                                           
//                                                                             .P:                                    Y@!                                                                    :P7G~~!^:.                   !G..G7                                       ~@7           .!^                                                                                                           
//                                                                             .P:                                    Y@!                                                                    :Y    .:^^!7:.               PJYY~                                        .&5          :?:                                                                                                            
//                                                                             .P:                                    5&:                                                                     .       ~P7:!^^^:          JY 7G:                                         &5          ::                                                                                                             
//                                                                             .P:                                    5&.                                                                     .~::~!?JJ:   ..:~!~^.     ~Y!. !?.                                        &5                                                                                                                         
//                                                                             .P:                                    Y@~                           .                                        .?7~~~~:.   :^7^.  .::~!^:.                                                &5       .                                                                                                                 
//            ^:                                                               .P:                                 .  5@!     ~         .^!!!!~~~?55?^~!!!!!^            :.      :!!!!!!!!!!~7P^~!JY7^  ~P^~^P~      .:^^~!5Y7~~!!!!!!!!!!!!!!^   :!?.            .^!!~?@5~!!!~7Y?~!!!!^                    :!!!!!!!!!!!!!!!!!!!:                    ^~~~~~~~~~~~~~~~~~~~.                         
//    :^~^^^^~^^~^^^^^^^^^^^^^^^^~^:::::::.                                    .P:                                    ~@!     ~.              ^7JJ!.                  :7YJ:                  :5~?Y?^    75.J.P7        ^?5J?!~:..              ^7JJ!:             ^^   7@~    :?:                                                          .   .::::^~~~~~~~~~~~~~~~~~~~~~^^^^~^^^~^^~~^           
//   .?YYYYYYYYYYYYYYYYYYYYYYYYYYYY555555P5YY555YY5555Y?????7!!~^^:::..         P:                                    :#J     ^            ~JJ?~.                  ^7JJ!:                  .!JY7:        ^7@7~:^^:  ~JJ?~.   ..~~^^.        ^7JY!.                ^^   YB.    ~.                                     ....^^^!!7?????Y555Y55PYYY555555YYYYYYYYYYYYYYYYYYYYYYYYYJG@5JYYYY?           
//                                        .....:...^^^~!!!7!77?Y55YY5Y555555J777G!:..                                  5#.    ~.       .~JY?~.                  ^7J?~.                  :7J?!^            .5. ?~.~?J?~.           .:^!::.^7??~.                 .~5?  .BP                               .:^!!!7?555P55YYY55Y?77!!!!!^^^:..:....                               ^5#J:                
//                                                                    ..:^~~!!JJ#P55Y5P5JJ?!^:.                        ?@~    ^     .~JY7^                   ^?JJ~.                 .^7J?~:  .!              .!YY?^                  .~5YJ?^.                .~JJ7!:  !@!                     .^^!??J5P5Y55YY?!!~~~^..                                             .~~!~:     ..J.:                
//                                                                              P:  ..:^!77YYPPPPY?7~^.                 BP    ^  :~JY7^                  .~?J?~.                 .~7J7^.     :5           :!JJ7:                  .~JJ7^   ^^~^::.        :!YY!^  ::  G# .!           .:^7?Y5P5PYJ!!~^:.    .                       ~7                             :!7?7.       J                  
//                                                                             .P:             :^^7?J5PG5Y7^:.          ?@!   ~^JJ!:                  .!J??~.                 .~?J7^         :Y        :!JJ!:                  .!JY7^          .^~~^.  :!JJ!:     ^^ 7@7:7.     .:!JY5P5JJ7^?~~! .?^!.:^!7:^#! ^7:~..7!!?^     ^~~~ ^^                              ::          J                  
//                                                                             .P:                    .:~!JJPGYJ!:.      G#   !..                  :~JY7^                  .~?J7^             .     :7JJ!:                  :!JJ!:                 :~YYP?^        ::.BG:!  .^?5PPYJ?^J^     7^^P :@~.~Y  ^::@: ~!:5. G5 ?G     .^~^                               .J5~ ^ ..:.   J                  
//                                                                             .P:                           :^7YPG5?^.  ~@J  ^                 :~?Y!:                  :!?J7:                   :7Y?~.                  :!JJ!:                  ^7Y?~..:^^^^: :. ^^?&^:!JPGYP!:.    !:    ^P~~G::&^ ^P7:^ :@~ 5Y^P! G5 ?G.   :!.                                  ::!^~!5 .    J                  
//                  ^. ^.               ^.     ^.                              .P:                                :!?PG5!.?@~ ^.             :!JY!:                  :!JY!:                  .?7Y?~.                  :!YJ!:                 .~7JJ~.       .:^7B#7.7@BPGY!:  !      ~!      ~~:~:.^   .~^:  ~^ :^::^ ~^ :~.   :!!~! :.:                                 .~:   ~7P7~                
//                 .B~.J:  ::..  .   .  7^  ...G!                              .P:                                    :75GY##^^           ^7JJ!.                  ^7Y?~.                  .^75G^.                  :7Y?~.                 .^JY?^              ^!!7J##J~:::::~Y     :Y^^^^^.      ^^^^^^       ^^^^^^.     .^^^^^^. ^! ..^^^^^^      :^^^^^^      :^^^^^:       J&!                 
//                 .B~:B^:5J!?B^:G.  P~ P~ YY!7&!                              .P:                                       .!G@B!        ^7JJ~.                  ^7JJ!.                  :!?J7::Y                .^JJ?~.                 .~JY7^                   ?B@P:  .:::^Y^     .::::::.      ^^~~~~.      ^~~~~~.     .^~~~~~  .^  .^~~~~^      :~~~~~^      :~~~~~^        :                  
//                 .B~^@^PJ  .B~^&.  B! B~^&.  P!                             . P:                                          ~#B:   .~JJ?~.                 .^7JJ^.                  :!YJ!:    .             .^?Y?^.                 .~JY7^                  .^7YP@P       ^!:                                                                                                                      
//                 .B~^&^?P!~?&~.GJ^!#~ G~.G7~!B!                             ~J#57.                                         .GB^^?J?~.                 .~7YJ^.                  :?Y?!:                  .!?J7^.                 :!?J!:                  .~?J?:!&P.     ^!~                                                                                                                        
//                  :. :. .^~^#~ .^^::. :. .~7^..                              ^#5                                            .P@5:                   ~JJ?~                   ^7YJ!.         .!       .!?Y7:                  :!JY!:                  .~JJ7^  ~#Y.   ^~~^   .                                                                                                                      
//                            ~.                                                :                                               ?&?               .~JJ7^                  .^7Y?~.            :5    :!JJ!:                  ^7J?!:                  .!JJ7^    ?&J.^^~^^.  ^?^^7^                                                                                                                    
//                                                                                                                               !#G:          :!JY!:                  .~7Y?^.               :J :7JJ!:                  ^?YJ~.                  :!JJ7:     .P#?:~^.      YP  JP                                                                                                                    
//                                                                            .^:::.                                              .P#!      :!Y?!:                  .~JJ7:                   ^?J?!:                 .^?Y?~.                  :75J!:       ?#5:           YP  ?P                                                                                                                    
//                                                                           ~P^.P?.                                                7#5: :7J?!:                  .!?J7:                  .~?J?~.                 .~YY7^.                  :7J?!:        :P#7             ~5..J~                                                                                                                    
//                                                                           .P~~^                                                   .Y#BY~.                  :!J?!:                  .~?J?~..~               .~JJ7^                   ^7??~.         :Y#Y                .::.                                                                                                                     
//                                                                          ~777?J.                                                    ^5G7                :75J!:                  .!?J7:    :5            :~J57:                  .^?YJ~.          .J#G^                                                                                                                                          
//                                                                          ~!:!:!.                                                      ~PBJ.          ^7Y?!:                  :!JJ7:       :J         :!Y?!:                  .~JY7^.           .JB57~                                                                                                                                           
//                                                                                                                                         ~PBJ:     ^!JJ~.                  :7J?~.                  :7J?!.                  .!?J?^             ^JB5^~!                                                                                                                                            
//                                                                                                                                           ^YGP!^7J?~.                  ^7JJ!.                  ^!YJ!:                  :!?J7:             .!GBJ. .^                                                                                                                                             
//                                                                                                                                             .7G&G^                  ^?Y?~.                .7~?YJ~.                  :!JY7:              ^YBP7                                                                                                                                                   
//                                                                                                                                                :?PGJ^           .~7J7~.                 .^Y#?~.                  :7?J~:              ^JGP?:                                                                                                                                                     
//                                                                                                                                                   ^?PGJ~.    .~?J?^                  :!JY?!Y                  ^7JJ!.             .!YGP?^     ~!                                                                                                                                                 
//                                                                                                                                                      :7PG5?^~Y7:                  :!JY7:.  .               ^??J7.            .~JPPY?^       ~!                                                                                                                                                  
//                                                                                                                                  .:::                   .^?JPPY!:              :7JJ!.                   ~JJ?~.          .^!JPG5?~.         ~~                                                                                                                                                   
//                                                                                                                                 ^P^.7P.                     .^7JPP5Y!~:.    .7J?!.        .!        .^?J?^        .:~!Y5PPJ!^.                                                                                                                                                                  
//                                                                                                                                 5B. ^&^                          .^!?YP555?7?J~.          :Y       !Y7^   .^^~!JYP5P5?7^.                                                                                                                                                                       
//                                                                                                                                 PG?:!^                                 .:~!7JYPP5555P55?77?Y7777777555555PPP5?77~:.                    !^                                                                                                                                                       
//                                                                                                                                 57 .                                            ..:~~~~!!!!!!!!!!!!~~~~:..                            ~!                                                                                                                                                        
//                                                                                                                                 ..                                                                                                   7~                                                                                                                                                         
//                                                                                                                                                                                                                                      .                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                                                                
//                                                                                                                                                                                                                                  ^                                                                                                                                                              
//                                                                                                                                                                                                                                 !~                                                                                                                                                              
//                                                                                                                                                                                                                               .!~                                                                                                                                                               
//                                                                                                                                                                                                                               ..                                                                                                                                                                
                                                                                                                                                                                                                                                                                                                                                                                                
//                                                                                                                                                                                                                            .                                                                                                                                                                    
//                                                                                                                                                                                                                          .!~                                                                                                                                                                    
//                                                                                                                                                                                                                         .7:                                                                                                                                                                     
//                                                                                                                                                                                                                         ^:                                                                                                                                                                      


// double ForcePerUnitLenght(double rho_liq, double g, double h1_0, double h2_0){
//     // The value of the horizontal force per unit length, Fh , can be calculated by integrating the hydrostatic pressure along each of the wetted sides of one of the plates (say the one on the left in Fig. 4) and taking the difference as follows
// // TODO Integral numerique
//     // at the end we have
//     return 0.5 * rho_liq * g * (h1_0*h1_0 - h2_0*h2_0);
// }
// OU



// Gives the force per unit length given the surface tension between fluids, tha angle made by the liquid with the border, capilary lenght(L_c) and the distancse between borders
// Return la force par unité de longueur compte tenu de la tension de surface entre les fluides, l'angle créé par le liquide avec les bords, la longueur capillaire (L_c) et la distance entre les bords.
// double ForcePerUnitLenght(double surface_tension, double theta1, double theta2, double L_c, double d){
//     return -(surface_tension/2.0) *
//     ((( (cot(theta1)*cosh(d/L_c) + cot(theta2)) *
//        (cot(theta1)*cosh(d/L_c) + cot(theta2))
//       ) / (sinh(d/L_c)*sinh(d/L_c)))- cot(theta1)*cot(theta1) );
// }
// (ca date du debut quand je metais les fonctions tu veux que on le garde ? lol on peux le metre en commentaire et regarder apres ? )
// cette fonction on la jamais utilise Ah... pourquoi l'as-tu mises ? oh très bien, on la garde ? mdr, oui voila au cas ou on ne aurait besoin
// Retourne la force de flottabilité compte tenu de la masse volumique du liquide, de la gravité terrestre du rayon de z_c et phi_c.
// double CalculBuoyancyForce(double rho_liquide, double g, double R, 
//                             double z_c, double phi_c){
//     return M_PI * rho_liquide * g * cb(R) * 
//             ((z_c / R)*sin(phi_c) + (2/3.0) - cos(phi_c) + (1/3.0) * cb(cos(phi_c))); // TODO pas sur du -cos equation 7 
// }

// A single particle
// see the ascii image or fig 7 for a single doubleing particle                                                                                                                                                                                                                                                                                                                                                                                                                 
// Nicolson approximation ????? finite difference method ???
// TODO Equation 7 8 
// Describing the ratio of gravitational to capillary forces, the Eötvös or Bond number is given by the equation:[7]
// TODO Source notre Cheerio 2008

// TODO resultant weight in linearised aproximation est que on a besoin de qu cest nous qui le prouve ou on peux les utiliser celui de larticle on disant que ils sont prouve
// 2*pi*gamma*z_c'*sin(fi_c) = 2*pi*gamma*R*B*Sigma
// To calculate the interaction energy using the Nicolson approximation, we also must calculate the interfacial displacement caused by an isolated floating sphere, which is determined by the hydrostatic balance 
//gamma*nabla*nabla*h=rho*g*h
// the coordinate invariant statement of Eq. (1). With the assumption of cylindrical symmetry, this generalization of Eq. (1) becomes:
// (1/r)(d/dt)(r*(dh/dr)) = h / (L_c*L_c)
// equations (11)



// Two interacting particles
// double EnergyBetweenTwoInteractingParticles(double gamma, double R, double B, 
//                                             double Sigma, double l, double L_c){
//     return -2*M_PI*gamma*sq(R)*sq(B)*sq(Sigma)*gsl_sf_bessel_K0(l/L_c);   
// }
// on pourait utiliser 
// gsl_sf_result result; // c"est un struct avec resultat et erreur
// int status = gsl_sf_bessel_K0_e (x, &result);
// pour avoir lerreur sur le resultat aussi


// TODO eq 14 et 15








                                                                                                                                                                                                                                                                                                                                                                                                                
=======
/**
 * @file calculs.c
 * @authors Bapiste BRAUN-DELVOYE, Erdi Çan
 * @brief Fichier où toutes les fonctions de calculs sont.
 * @version 5.5
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <math.h>
#include <gsl/gsl_sf_bessel.h> // pour la fonction de bessel modifiee K_1(x)
#include <stdio.h>
#include "cheerios.h"


// Retourne le carré de x.
double sq(double x){
    return x*x;
}

// Retourne le cube de x.
double cb(double x){
    return x*x*x;
}

// Retourne le cotangeant d'un angle x.
double cot(double x){
    return cos(x)/sin(x);
}

// On peux mettre directement vec = vec2 et ça le fait automatiquement donc on a pas besoin sauf si on va faire des allocations de memoire 
// Retourne un nouveau vecteur à 2 coordonnées créé à partir de l'addition des coordonnées des vecteurs v1 et v2.
void EqualiseVector(vec2_t* vecObjectif, vec2_t vecEgal){
    vecObjectif->x = vecEgal.x;
    vecObjectif->y = vecEgal.y;
}

// Retourne le vecteur à 2 dimensions vec avec ses coordonnées initiales divisées par s.
vec2_t VectorDiviseScalaire(vec2_t vec, double s){
    vec2_t new_vec;
    new_vec.x = vec.x / s;
    new_vec.y = vec.y / s;
    return new_vec;
}

// Retourne le vecteur à 2 dimensions vec avec ses coordonnées initiales multipliées par s.
vec2_t VectorTimesScalar(vec2_t vec, double s){
    vec2_t new_vec;
    new_vec.x = vec.x * s;
    new_vec.y = vec.y * s;
    return new_vec;
}

// Retourne un nouveau vecteur à 2 coordonnées créé à partir de l'addition des coordonnées des vecteurs v1 et v2.
vec2_t VecteurAdition(vec2_t v1, vec2_t v2){
    vec2_t vecAditione;
    vecAditione.x = v1.x + v2.x;
    vecAditione.y = v1.y + v2.y;
    return vecAditione;
}

// Initialise un vecteur avec x_val et y_val.
void InitialiseVec(vec2_t* vec, double x_val, double y_val){
    vec->x = x_val;
    vec->y = y_val;
}

// Retourne la norme du vecteur à 2 dimensions vec.
double CalculNorme(vec2_t vec){
    return sqrt(sq(vec.x) + sq(vec.y));
}


// Retourne la normalisation d'un vecteur, vec, à 2 dimensions en divisant ses coorodnnées par la norme de ce vecteur.
vec2_t NormaliseVector(vec2_t vec){
    double norme = CalculNorme(vec);
    vec2_t normalise;
    normalise.x = vec.x/norme;
    normalise.y = vec.y/norme;
    return normalise;
}

// Retourne un vecteur normalisé de la direction(sens) de c1 vers c2.
vec2_t SensEntreC1aC2(cheerio_t c1, cheerio_t c2){     
    vec2_t sens;
    sens.x = c2.pos.x - c1.pos.x;
    sens.y = c2.pos.y - c1.pos.y;
    return NormaliseVector(sens);
}


// Retourne la distance entre le centre de deux cheerios, c1 et c2.
double DistanceEntreDeuxCheerios(cheerio_t c1, cheerio_t c2){
    return sqrt(sq(c1.pos.x - c2.pos.x) + sq(c1.pos.y - c2.pos.y)) - c1.d/2 - c2.d/2; // on enleve la distance de leur rayons
}

// Met à jour la position, la vitesse et l'accélération du cheerio en utilisant l'intégratioon de Verlet (TODO) et la force de trainée https://en.wikipedia.org/wiki/Verlet_integration 
void UpdateCheerio(cheerio_t* cheerio, double dt){
    vec2_t new_acc = VectorDiviseScalaire(cheerio->f_applique, cheerio->m); // F = ma => a = F/m
    // TODO faire tel que on prend en compte la force de drag aussi
    vec2_t new_pos = VecteurAdition(VecteurAdition(cheerio->pos, VectorTimesScalar(cheerio->v, dt)), VectorTimesScalar(cheerio->a, (dt * dt * 0.5))); // new_pos = pos + V*dt + A*dt*dt*0.5 // si on etait en C++ on pourait faire des operator overloading et specialiser quand on utilise loperateur + entre deux vecteur qu ca ais ca automatiqueent sans ecrire les fonctions
    vec2_t new_vel = VecteurAdition(cheerio->v, VectorTimesScalar(VecteurAdition(cheerio->a, new_acc), dt*0.5));  // new_vel = V + (A+newA)*dt*0.5
    // on mets les nouveaux positions dans notre cheerios (objet)
    cheerio->a = new_acc;
    cheerio->v = new_vel;
    cheerio->pos = new_pos;
}

// Parcours chaque cheerio et met à jour leurs positions.
void UpdateAll(cheerio_t* cheerios, int nb_cheerios, double dt){
    for(int c = 0; c < nb_cheerios; c++){
        UpdateCheerio(cheerios+c, dt);
        //                 ^ pointer arithmetic meme chose que &(cheerios[c])
    }
}

// gamma = tension de surface, R = longueur caractéristique de courbure, (par exemple les raypons de courbure d'une goutte)
// B = Bond number = (rho * g * R * R) / gamma // le plus important, paramètre sans dimension // TODO source Cherrios 2005
double CalculBondNumber(double rho_liquide, double rho_air, 
                            double R, double gamma ,double g,long long int* warning_counter){
    double delta_rho = fabs(rho_liquide-rho_air);   //fabs = valeur absolue pour les doubles 
    double B = (delta_rho * g * sq(R)) / gamma;
    if (B > 0.3 && WARNING_MESAGES) {                                  // TODO pas sur de ici 
        if(PRINT_WARNING) {
            printf("WARNING Bond Number = %lf > 0.3 et lequation 9 de Cherrios est valable seulement quand B << 1 ", B); // The expression for the slope of the interface in the vicinity of the spherical particle given in Eq. 共9兲 is valid for BⰆ1 共corresponding to a radius of ⬃1mm or smaller for a sphere at an air–water interface兲, in which case surface tension is very important.
            printf("(WARNING de la fonction CalculBondNumber(%g, %g, %g, %g, %g)\n", rho_liquide,  rho_air,  R,  gamma , g );
        }
        (*warning_counter)++;
    }
    return B;
}


double CalculLinearBondNumber( double R, double L_c,long long int* warning_counter){
    double B = sq(R)/sq(L_c);
    if (B > 0.3 && WARNING_MESAGES) {                                  // TODO pas sur de ici 
        if(PRINT_WARNING) {
            printf("WARNING Bond Number = %lf > 0.3 et lequation 9 de Cherrios est valable seulement quand B << 1 ", B); // The expression for the slope of the interface in the vicinity of the spherical particle given in Eq. 共9兲 is valid for BⰆ1 共corresponding to a radius of ⬃1mm or smaller for a sphere at an air–water interface兲, in which case surface tension is very important.
            printf("(WARNING de la fonction CalculLinearisedBondNumber(%g, %g)\n", R, L_c );
        }
        (*warning_counter)++;
    }
    return B;
}

// Poids effectif d'une sphère (sans dimension) = 2*pi*gamma*R*B*Sigma
// z_c'*sin(fi_c) = B*Sigma
// Sigma = ((2*D-1) / 3 ) - 0.5*cos(theta) + (1.0/6)*cos(theta)*cos(theta)*cos(theta)
double CalculSigma(double rho_flottant, double rho_liquide, double theta, long long int* warning_counter){
    double D = rho_flottant/rho_liquide; // flottant cest notre objet
    if (theta > 2*M_PI && WARNING_MESAGES){ // message de erreur car si  theta est plus grand que ca ca veux dire que soit la valeur nest pas en bone valeurs
        if(PRINT_WARNING) {
            printf("WARNING ettes vous sur que vouz avez rentre l'angle(thetha) en radian car theta = %lf radians (%lf degrees)",theta, (theta*180)/M_PI);
            printf("(WARRNING de la fonction CalculSigma(%lf, %lf, %lf))\n",rho_flottant, rho_liquide, theta);
        }
        (*warning_counter)++;
    }
    return ((2*D-1) / 3.0) - 0.5*cos(theta) + (1/6.0)*cb(cos(theta));
}

// force of interaction is given by F(l) = -dE/dl or :
// Retourne la force d'intéraction entre deux particules.
double ForceBetweenTwoInteractingParticles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c,long long int* warning_counter){
    // precondition l > 0 
    double l_over_L_c = l/L_c;
    if(l_over_L_c > 36 && WARNING_MESAGES){
        if(PRINT_INFO && PRINT_INFO) printf("INFO l/L_c = %lf > 36 and modified bessel function K1 goes to 0 so thee is no force at that length ( function not calculated since sometimes it might cause underflow)\n",l_over_L_c);
        //return 0;
    } // if it is smaller than 36 calculates itself 
    return -2*M_PI*gamma*R*sqrt(pow(B,5))*sq(Sigma)*gsl_sf_bessel_K1(l/L_c);
}


// EXTRA des fiqures en asci art :)

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

//         |  P_atm |       ___ |        | ___             
//         |\______/|          \|  P_atm |/                    
//         |        |           |        |             
//         |        |           | ______ |             
//        /|        |\          |/      \|             
//    ___/ |P>P_atm | \__       |P<P_atm |   
//
//     two wetting plates    Two nonwetting plates  

// Dézoomer pour voir la figure
//                                                                                                                                                                                       :^^^^^^^^^^^^:                                                                                                                                                                                            
//                                                                                                                                                                          .::~!?JJJYYPPGPPPPPYJ5PPPPPPPYYJJJ?!~::.                                                                                                                                                                               
//                                                                                                                                                                    :^!JY5PPYJJ?!~:::...             ..:::~!?JYYPP5Y?~^.                                                                                                                                                                         
//                                                                                                                                                               :~?5GP5J7~:.                                      .:~7Y5P557~.                                                                                                                                                                    
//                                                                                                                                                           ^75PPY?!:                                                    :!75PPJ!:                                                                                                                                                                
//                                                                                                                                                       :!YPPY!^                                                              ^!YPPY!:                                                                                                                                                            
//                                                                                                                                                    :?PGY!:                                                                      :!YGP?^                                                                                                                                                         
//                                                                                                                                                 ^?GP?^                                                                             .^JPG?^                                                                                                                                                      
//                                                                                                                                              :7GPJ^                                                                                    ^JGG?:                                                                                                                                                   
//                                                                                                                                            ~YBY~                                                                                          !5BY^                                                                                                                                                 
//                                                                                                                                          7GBJ:                                                                                              .7PP7                                                                                                                                               
//                                                                                                                                       .?BG!                                                                                                    !GB7                                                                                                                                             
//                                                                                                                                      !GB!                                                                                                        ~PG!                                                                                                                                           
//                                                                                                                                    ~PG~                                                                                                            7BG^                                                                                                                                         
//                                                                                                                                  :P&?                                                                                                               .?BY:                                                                                                                                       
//                                                                                                                                 7#5:                                                                                                                  :P#!                                             .                                                                                        
//                                                                                                                               :PB!                                                                                                                      7&Y.                                          ~!                                                                                        
//                                                                                                                              ~#G:                                                                                                                        :G#^                                        !~                                                                                         
//                                                                                                                             7&Y                                                             .                                                             .Y&!                                      ~^                                                                                          
//                                                                                                                            J&7                                                            !7:!?.                                                            ?&?                                                                                                                                 
//                                                                                                                           Y&~                                                            ^B.  G?                                                             !&J                                                                                                                                
//                                                                                                                          J&!                                                             ~@7.^J^                                                              !&?                               ~~                                                                                              
//                                                                                                                         ?@7                                                              ~&:.:  ~~:                                                            7@?                             !~                                                                                               
//                                                                                                                        ~&?                                                               :7     !J!                                                             Y&~                           !^                                                                                                
//                                                                             .J:                                       :&P                                                                       . .                                                              PB.                                                                                                                            
//                        ..     .:.    .^.                                    .P:                                       5&:                                                                                                                                        :&Y                                                                                                                            
//                      7J!~5P .?!!J5.:P77?~                                   .P:                                      !@?                                                                                                                                          J@~                     ^                                                                                                     
//                      #!  !# :J!~?&^.J?!?7                                   .P:                                      P#.                                                                                                                                           BP                   .!^                                                                                                     
//                      JJ^^5# ?#~~?&!:Y~^JP.                                  .P:                                     ~@7                                                                                                                                            J@^                 .7:                                                                                                      
//                      ^Y!~YP  :~^.:: :~!^.                                   .P:                                     5@^                                                                                                                                            :&Y                 ..                                                                                                       
//                      :?77~.                                                 .P:                                    .B5                                                                                                                                              5G                                                                                                                          
//                                                                             .P:                                    ~@7                                                                    .PPGY^                       :7^^~.                                       ?&^             :                                                                                                           
//                                                                             .P:                                    Y@!                                                                    :P7G~~!^:.                   !G..G7                                       ~@7           .!^                                                                                                           
//                                                                             .P:                                    Y@!                                                                    :Y    .:^^!7:.               PJYY~                                        .&5          :?:                                                                                                            
//                                                                             .P:                                    5&:                                                                     .       ~P7:!^^^:          JY 7G:                                         &5          ::                                                                                                             
//                                                                             .P:                                    5&.                                                                     .~::~!?JJ:   ..:~!~^.     ~Y!. !?.                                        &5                                                                                                                         
//                                                                             .P:                                    Y@~                           .                                        .?7~~~~:.   :^7^.  .::~!^:.                                                &5       .                                                                                                                 
//            ^:                                                               .P:                                 .  5@!     ~         .^!!!!~~~?55?^~!!!!!^            :.      :!!!!!!!!!!~7P^~!JY7^  ~P^~^P~      .:^^~!5Y7~~!!!!!!!!!!!!!!^   :!?.            .^!!~?@5~!!!~7Y?~!!!!^                    :!!!!!!!!!!!!!!!!!!!:                    ^~~~~~~~~~~~~~~~~~~~.                         
//    :^~^^^^~^^~^^^^^^^^^^^^^^^^~^:::::::.                                    .P:                                    ~@!     ~.              ^7JJ!.                  :7YJ:                  :5~?Y?^    75.J.P7        ^?5J?!~:..              ^7JJ!:             ^^   7@~    :?:                                                          .   .::::^~~~~~~~~~~~~~~~~~~~~~^^^^~^^^~^^~~^           
//   .?YYYYYYYYYYYYYYYYYYYYYYYYYYYY555555P5YY555YY5555Y?????7!!~^^:::..         P:                                    :#J     ^            ~JJ?~.                  ^7JJ!:                  .!JY7:        ^7@7~:^^:  ~JJ?~.   ..~~^^.        ^7JY!.                ^^   YB.    ~.                                     ....^^^!!7?????Y555Y55PYYY555555YYYYYYYYYYYYYYYYYYYYYYYYYJG@5JYYYY?           
//                                        .....:...^^^~!!!7!77?Y55YY5Y555555J777G!:..                                  5#.    ~.       .~JY?~.                  ^7J?~.                  :7J?!^            .5. ?~.~?J?~.           .:^!::.^7??~.                 .~5?  .BP                               .:^!!!7?555P55YYY55Y?77!!!!!^^^:..:....                               ^5#J:                
//                                                                    ..:^~~!!JJ#P55Y5P5JJ?!^:.                        ?@~    ^     .~JY7^                   ^?JJ~.                 .^7J?~:  .!              .!YY?^                  .~5YJ?^.                .~JJ7!:  !@!                     .^^!??J5P5Y55YY?!!~~~^..                                             .~~!~:     ..J.:                
//                                                                              P:  ..:^!77YYPPPPY?7~^.                 BP    ^  :~JY7^                  .~?J?~.                 .~7J7^.     :5           :!JJ7:                  .~JJ7^   ^^~^::.        :!YY!^  ::  G# .!           .:^7?Y5P5PYJ!!~^:.    .                       ~7                             :!7?7.       J                  
//                                                                             .P:             :^^7?J5PG5Y7^:.          ?@!   ~^JJ!:                  .!J??~.                 .~?J7^         :Y        :!JJ!:                  .!JY7^          .^~~^.  :!JJ!:     ^^ 7@7:7.     .:!JY5P5JJ7^?~~! .?^!.:^!7:^#! ^7:~..7!!?^     ^~~~ ^^                              ::          J                  
//                                                                             .P:                    .:~!JJPGYJ!:.      G#   !..                  :~JY7^                  .~?J7^             .     :7JJ!:                  :!JJ!:                 :~YYP?^        ::.BG:!  .^?5PPYJ?^J^     7^^P :@~.~Y  ^::@: ~!:5. G5 ?G     .^~^                               .J5~ ^ ..:.   J                  
//                                                                             .P:                           :^7YPG5?^.  ~@J  ^                 :~?Y!:                  :!?J7:                   :7Y?~.                  :!JJ!:                  ^7Y?~..:^^^^: :. ^^?&^:!JPGYP!:.    !:    ^P~~G::&^ ^P7:^ :@~ 5Y^P! G5 ?G.   :!.                                  ::!^~!5 .    J                  
//                  ^. ^.               ^.     ^.                              .P:                                :!?PG5!.?@~ ^.             :!JY!:                  :!JY!:                  .?7Y?~.                  :!YJ!:                 .~7JJ~.       .:^7B#7.7@BPGY!:  !      ~!      ~~:~:.^   .~^:  ~^ :^::^ ~^ :~.   :!!~! :.:                                 .~:   ~7P7~                
//                 .B~.J:  ::..  .   .  7^  ...G!                              .P:                                    :75GY##^^           ^7JJ!.                  ^7Y?~.                  .^75G^.                  :7Y?~.                 .^JY?^              ^!!7J##J~:::::~Y     :Y^^^^^.      ^^^^^^       ^^^^^^.     .^^^^^^. ^! ..^^^^^^      :^^^^^^      :^^^^^:       J&!                 
//                 .B~:B^:5J!?B^:G.  P~ P~ YY!7&!                              .P:                                       .!G@B!        ^7JJ~.                  ^7JJ!.                  :!?J7::Y                .^JJ?~.                 .~JY7^                   ?B@P:  .:::^Y^     .::::::.      ^^~~~~.      ^~~~~~.     .^~~~~~  .^  .^~~~~^      :~~~~~^      :~~~~~^        :                  
//                 .B~^@^PJ  .B~^&.  B! B~^&.  P!                             . P:                                          ~#B:   .~JJ?~.                 .^7JJ^.                  :!YJ!:    .             .^?Y?^.                 .~JY7^                  .^7YP@P       ^!:                                                                                                                      
//                 .B~^&^?P!~?&~.GJ^!#~ G~.G7~!B!                             ~J#57.                                         .GB^^?J?~.                 .~7YJ^.                  :?Y?!:                  .!?J7^.                 :!?J!:                  .~?J?:!&P.     ^!~                                                                                                                        
//                  :. :. .^~^#~ .^^::. :. .~7^..                              ^#5                                            .P@5:                   ~JJ?~                   ^7YJ!.         .!       .!?Y7:                  :!JY!:                  .~JJ7^  ~#Y.   ^~~^   .                                                                                                                      
//                            ~.                                                :                                               ?&?               .~JJ7^                  .^7Y?~.            :5    :!JJ!:                  ^7J?!:                  .!JJ7^    ?&J.^^~^^.  ^?^^7^                                                                                                                    
//                                                                                                                               !#G:          :!JY!:                  .~7Y?^.               :J :7JJ!:                  ^?YJ~.                  :!JJ7:     .P#?:~^.      YP  JP                                                                                                                    
//                                                                            .^:::.                                              .P#!      :!Y?!:                  .~JJ7:                   ^?J?!:                 .^?Y?~.                  :75J!:       ?#5:           YP  ?P                                                                                                                    
//                                                                           ~P^.P?.                                                7#5: :7J?!:                  .!?J7:                  .~?J?~.                 .~YY7^.                  :7J?!:        :P#7             ~5..J~                                                                                                                    
//                                                                           .P~~^                                                   .Y#BY~.                  :!J?!:                  .~?J?~..~               .~JJ7^                   ^7??~.         :Y#Y                .::.                                                                                                                     
//                                                                          ~777?J.                                                    ^5G7                :75J!:                  .!?J7:    :5            :~J57:                  .^?YJ~.          .J#G^                                                                                                                                          
//                                                                          ~!:!:!.                                                      ~PBJ.          ^7Y?!:                  :!JJ7:       :J         :!Y?!:                  .~JY7^.           .JB57~                                                                                                                                           
//                                                                                                                                         ~PBJ:     ^!JJ~.                  :7J?~.                  :7J?!.                  .!?J?^             ^JB5^~!                                                                                                                                            
//                                                                                                                                           ^YGP!^7J?~.                  ^7JJ!.                  ^!YJ!:                  :!?J7:             .!GBJ. .^                                                                                                                                             
//                                                                                                                                             .7G&G^                  ^?Y?~.                .7~?YJ~.                  :!JY7:              ^YBP7                                                                                                                                                   
//                                                                                                                                                :?PGJ^           .~7J7~.                 .^Y#?~.                  :7?J~:              ^JGP?:                                                                                                                                                     
//                                                                                                                                                   ^?PGJ~.    .~?J?^                  :!JY?!Y                  ^7JJ!.             .!YGP?^     ~!                                                                                                                                                 
//                                                                                                                                                      :7PG5?^~Y7:                  :!JY7:.  .               ^??J7.            .~JPPY?^       ~!                                                                                                                                                  
//                                                                                                                                  .:::                   .^?JPPY!:              :7JJ!.                   ~JJ?~.          .^!JPG5?~.         ~~                                                                                                                                                   
//                                                                                                                                 ^P^.7P.                     .^7JPP5Y!~:.    .7J?!.        .!        .^?J?^        .:~!Y5PPJ!^.                                                                                                                                                                  
//                                                                                                                                 5B. ^&^                          .^!?YP555?7?J~.          :Y       !Y7^   .^^~!JYP5P5?7^.                                                                                                                                                                       
//                                                                                                                                 PG?:!^                                 .:~!7JYPP5555P55?77?Y7777777555555PPP5?77~:.                    !^                                                                                                                                                       
//                                                                                                                                 57 .                                            ..:~~~~!!!!!!!!!!!!~~~~:..                            ~!                                                                                                                                                        
//                                                                                                                                 ..                                                                                                   7~                                                                                                                                                         
//                                                                                                                                                                                                                                      .                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                                                                
//                                                                                                                                                                                                                                  ^                                                                                                                                                              
//                                                                                                                                                                                                                                 !~                                                                                                                                                              
//                                                                                                                                                                                                                               .!~                                                                                                                                                               
//                                                                                                                                                                                                                               ..                                                                                                                                                                
                                                                                                                                                                                                                                                                                                                                                                                                
//                                                                                                                                                                                                                            .                                                                                                                                                                    
//                                                                                                                                                                                                                          .!~                                                                                                                                                                    
//                                                                                                                                                                                                                         .7:                                                                                                                                                                     
//                                                                                                                                                                                                                         ^:                                                                                                                                                                      


// double ForcePerUnitLenght(double rho_liq, double g, double h1_0, double h2_0){
//     // The value of the horizontal force per unit length, Fh , can be calculated by integrating the hydrostatic pressure along each of the wetted sides of one of the plates (say the one on the left in Fig. 4) and taking the difference as follows
// // TODO Integral numerique
//     // at the end we have
//     return 0.5 * rho_liq * g * (h1_0*h1_0 - h2_0*h2_0);
// }
// OU



// Gives the force per unit length given the surface tension between fluids, tha angle made by the liquid with the border, capilary lenght(L_c) and the distancse between borders
// Return la force par unité de longueur compte tenu de la tension de surface entre les fluides, l'angle créé par le liquide avec les bords, la longueur capillaire (L_c) et la distance entre les bords.
// double ForcePerUnitLenght(double surface_tension, double theta1, double theta2, double L_c, double d){
//     return -(surface_tension/2.0) *
//     ((( (cot(theta1)*cosh(d/L_c) + cot(theta2)) *
//        (cot(theta1)*cosh(d/L_c) + cot(theta2))
//       ) / (sinh(d/L_c)*sinh(d/L_c)))- cot(theta1)*cot(theta1) );
// }
// (ca date du debut quand je metais les fonctions tu veux que on le garde ? lol on peux le metre en commentaire et regarder apres ? )
// cette fonction on la jamais utilise Ah... pourquoi l'as-tu mises ? oh très bien, on la garde ? mdr, oui voila au cas ou on ne aurait besoin
// Retourne la force de flottabilité compte tenu de la masse volumique du liquide, de la gravité terrestre du rayon de z_c et phi_c.
// double CalculBuoyancyForce(double rho_liquide, double g, double R, 
//                             double z_c, double phi_c){
//     return M_PI * rho_liquide * g * cb(R) * 
//             ((z_c / R)*sin(phi_c) + (2/3.0) - cos(phi_c) + (1/3.0) * cb(cos(phi_c))); // TODO pas sur du -cos equation 7 
// }

// A single particle
// see the ascii image or fig 7 for a single doubleing particle                                                                                                                                                                                                                                                                                                                                                                                                                 
// Nicolson approximation ????? finite difference method ???
// TODO Equation 7 8 
// Describing the ratio of gravitational to capillary forces, the Eötvös or Bond number is given by the equation:[7]
// TODO Source notre Cheerio 2008

// TODO resultant weight in linearised aproximation est que on a besoin de qu cest nous qui le prouve ou on peux les utiliser celui de larticle on disant que ils sont prouve
// 2*pi*gamma*z_c'*sin(fi_c) = 2*pi*gamma*R*B*Sigma
// To calculate the interaction energy using the Nicolson approximation, we also must calculate the interfacial displacement caused by an isolated floating sphere, which is determined by the hydrostatic balance 
//gamma*nabla*nabla*h=rho*g*h
// the coordinate invariant statement of Eq. (1). With the assumption of cylindrical symmetry, this generalization of Eq. (1) becomes:
// (1/r)(d/dt)(r*(dh/dr)) = h / (L_c*L_c)
// equations (11)



// Two interacting particles
// double EnergyBetweenTwoInteractingParticles(double gamma, double R, double B, 
//                                             double Sigma, double l, double L_c){
//     return -2*M_PI*gamma*sq(R)*sq(B)*sq(Sigma)*gsl_sf_bessel_K0(l/L_c);   
// }
// on pourait utiliser 
// gsl_sf_result result; // c"est un struct avec resultat et erreur
// int status = gsl_sf_bessel_K0_e (x, &result);
// pour avoir lerreur sur le resultat aussi


// TODO eq 14 et 15








                                                                                                                                                                                                                                                                                                                                                                                                                
>>>>>>> a9b07e1 (Co-authored-by: Arthasee <baptiste.braun.delvoye@gmail.com>)
