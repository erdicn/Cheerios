// faire 
// gcc -Wall -Wextra from_scratch.c -lm -lgsl -lgslcblas  && ./a.out && python3 visualisation.py
// car pour linstant il nes t pas dans le main
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_errno.h>

typedef struct Vec2{
    double x, y;
} vec2_t;

// TODO peut etre metre des courbures differentes ?
typedef struct Cheerio{
    vec2_t pos;           // position cheerio
    vec2_t v;             // vitesse cheerio
    vec2_t a;             // acceleration cheerio
    vec2_t f_applique;    // force applique sur le cheerio
    double d;             // diametre cheerio
    double m;             // masse du cheerio
} cheerio_t;     

void voirSiNotreLectureABienMarche(cheerio_t* cheerios, int nb_cheerios){
    int nb_print = nb_cheerios > 10 ? 10 : nb_cheerios; // comme ca si on a beaucour de cherios on afiche pas tout on check maximum les 10 premiers et on assume que si les 10 premiers sont bien fonctione les autres marche aussi
    for(int i = 0; i < nb_print; i++){
        printf("%lf %lf %lf %lf %lf\n", cheerios[i].pos.x, cheerios[i].pos.y, cheerios[i].d, cheerios[i].v.x, cheerios[i].a.x);
    }
}

void LectureData(FILE* fichier, cheerio_t *cher){
	double posx = 0,posy = 0 ,d = 0, v_x = 0, v_y = 0, a_x = 0, a_y = 0, m = 0, f_x = 0, f_y = 0;
	fscanf(fichier,"%lf %lf %lf %lf %lf %lf %lf %lf", &posx, &posy, &d, &v_x, &v_y, &a_x, &a_y, &m);//, tmp);
	cher->pos.x = posx; // si on mets cela dans le fscanf ca bug meme si on prends les adresses cest pour ca que cest en dehors
	cher->pos.y = posy;
	cher->d = d;
	cher->v.x = v_x;
	cher->v.y = v_y;
	cher->a.x = a_x;
	cher->a.x = a_y;
	cher->m = m;
	cher->f_applique.x = f_x;
	cher->f_applique.y = f_y;
}
cheerio_t* LectureTouteCheerios(char* nom_fichier, int* nb_cheerios, long int* NT, double* dt){
	FILE* fichier_avec_donnees_initiales_cheerios = fopen(nom_fichier,"r");
	if (fichier_avec_donnees_initiales_cheerios == NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		fscanf(fichier_avec_donnees_initiales_cheerios, "%d %ld %lf", nb_cheerios, NT, dt); // la premiere ligne sur le fichier indique le nombre de cheerios(objets flottants)
		cheerio_t *cheerios = NULL;
		cheerios = malloc(sizeof(cheerio_t)* *nb_cheerios);
		for(int i = 0; i < *nb_cheerios; i++){
			LectureData(fichier_avec_donnees_initiales_cheerios, &cheerios[i]);
		}
		fclose(fichier_avec_donnees_initiales_cheerios);
		printf("Lecture data fin\n");
		return cheerios;
	}
}


void InitialiseFichierDeEcriture(char* nom_fichier){
	FILE* fichier_de_ecriture= fopen(nom_fichier,"w"); //When you open a file with "w" flag it creates an empty file for writing. If a file with the same name already exists its contents are erased and the file is treated as an empty new file. source: https://stackoverflow.com/questions/4815251/how-do-i-clear-the-whole-contents-of-a-file-in-c#:~:text=If%20a%20file%20with%20the,was%20in%20the%20file%20previously. 
	if (fichier_de_ecriture== NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		fclose(fichier_de_ecriture);
	}
}

void EcritureData(char* nom_fichier, cheerio_t* cheerios, int nb_cheerios, long int nt){
	FILE* fichier_de_ecriture= fopen(nom_fichier,"a"); 
	if (fichier_de_ecriture== NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		// on parcour notre tableau et on mets chaque element du tableau dans le fichier 
		for(int c = 0; c < nb_cheerios; c++){
			fprintf(fichier_de_ecriture, "%ld %.16lf %.16lf %.16lf\n",//%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n", //"%ld %g %g %g %g %g %g %g %g %g %g\n",
										nt, cheerios[c].pos.x, cheerios[c].pos.y, cheerios[c].d);
		}
		fclose(fichier_de_ecriture);
	}
}

vec2_t VecteurAdition(vec2_t v1, vec2_t v2){
    vec2_t new_vec;
    new_vec.x = v1.x + v2.x;
    new_vec.y = v1.y + v2.y;
    return new_vec;
}

vec2_t VectorTimesScalar(vec2_t vec, double s){
    vec2_t new_vec;
    new_vec.x = vec.x * s;
    new_vec.y = vec.y * s;
    return new_vec;
}

// Retourne le carré de x.
double sq(double x){
    return x*x;
}

// Retourne le cube de x.
double cb(double x){
    return x*x*x;
}


// TODO je sais pas si le bond number ca marche quand cest plus grand que 0.3
double CalculBondNumber(double rho_liquide, double rho_air, 
                            double R, double gamma ,double g){
    double delta_rho = fabs(rho_liquide-rho_air);   //fabs = valeur absolue pour les doubles 
    double B = (delta_rho * g * sq(R)) / gamma;
    return B;
}

double CalculLinearBondNumber( double R, double L_c){
    double B = sq(R)/sq(L_c);
    return B;
}
double CalculSigma(double rho_flottant, double rho_liquide, double theta, long long int* warning_counter){
    double D = rho_flottant/rho_liquide; // flottant cest notre objet
    if (theta > 2*M_PI){ // message de erreur car si  theta est plus grand que ca ca veux dire que soit la valeur nest pas en bone valeurs
            printf("WARNING ettes vous sur que vouz avez rentre l'angle(thetha) en radian car theta = %lf radians (%lf degrees)",theta, (theta*180)/M_PI);
            printf("(WARRNING de la fonction CalculSigma(%lf, %lf, %lf))\n",rho_flottant, rho_liquide, theta);
        (*warning_counter)++;
    }
    return ((2*D-1) / 3.0) - 0.5*cos(theta) + (1/6.0)*cb(cos(theta));
}
double ForceBetweenTwoInteractingParticles(double gamma, double R, double B, 
                                            double Sigma, double l, double L_c){
    // precondition l > 0 
    return -2*M_PI*gamma*R*sqrt(pow(B,5))*sq(Sigma)*gsl_sf_bessel_K1(l/L_c);
}

int main(){
    gsl_set_error_handler_off();
    long int NT;                                                          // nombre de pas de temps 
    double dt;                     // TODO trouver la masse volumique des cheerios// masses volumiques en kg/m^3 source air https://www.thermexcel.com/french/tables/massair.htm
    int nb_cheerios;
    cheerio_t *cheerios= NULL;                                                  // notre tableaux qui est remplie de cheerios
    cheerios = LectureTouteCheerios("cheerio_donnees_test.txt", &nb_cheerios, &NT, &dt);  
    voirSiNotreLectureABienMarche(cheerios, nb_cheerios);
    InitialiseFichierDeEcriture("donnees.txt");                                 // ca efface tout le fichier donnees.txt pour que on a un fichier vide pour les nouvelles donnees 
    ////////////////////////////////////////////////////////////////////////////
    // TODO faire tel que on lis ces donnees aussi dans un autre fichier
    double rho_liq = 1000., rho_air = 1.1, rho_cheerio = 24.;                  // TODO trouver la masse volumique des cheerios// masses volumiques en kg/m^3 source air https://www.thermexcel.com/french/tables/massair.htm
    long long int warning_counter = 0;                                          // TODO pour linstant les valeurs sont unpeut aliatore il faux les metres dans le meme unite et les bones valeures(peut etre faire tel que on lis les valeures de un fichier? )
    double surface_tension = 71.99 / 1000;                                      // tension de surface (gamma) pour l'eau gamma = 72 mN/m = 72/1000 N/m source (https://www.biolinscientific.com/blog/surface-tension-of-water-why-is-it-so-high#:~:text=The%20surface%20tension%20of%20water,highest%20surface%20tension%20for%20liquid.)
    double g = 9.81;
    double capilary_length = sqrt(surface_tension/(fabs(rho_liq-rho_air)*g)) ;  // capilary lenght = L_c ≡ sqrt(γ/(𝜌*g))  γ = gamma = surface tension//2.7 / 1000; // L_c of water = 2.7 mm https://www.sciencedirect.com/topics/engineering/capillary-length#:~:text=As%20surface%20energy%20is%20related,will%20indeed%20have%20little%20effect.
    double R = 2.7 / 1000  ;                                                                // TODO trouver le R a partir de lobjet// ~ en mm /1000 = en m // The toroidal shape of a Cheerio complicates the notion of Bond number, but if we take the effective radius based on its volume R*⫽(R12R2)1/3⬇2.7 mm 共where R1⬇2 mm and R2⬇5 mm are the two radii of the torus兲 and Lc⫽2.7 mm for an air–water interface, then B⬇1. This value is within the regime where the gravitational energy of the particles dominates the capillary suction due to the meniscus between them, and so it is crucial that we account for the buoyancy effects to correctly interpret the attractive force.//= CalculCurvature
    double B = CalculLinearBondNumber(R,capilary_length); 
    B = CalculBondNumber(rho_liq, rho_air, R, surface_tension, g);
    double theta = fabs(B) < 0.63 ? asin(M_PI_2 * B) : asin(B);    //M_PI_2 *  // dans larticle ils mets ca mais si nous on le mets ca depase la limite de arcsin [-1,1]                                                  // TODO pour linstant ca change pas par rapport a la proximite et cest symetyrique mais en realite ca depend de la proximite des particules source Lattice Boltzmann simulation of capillary interactions among colloidal particles equation27 //(M_PI * 30) /180;                                                        // TODO faire la funtion qui trouve langle (pour linstant on a une valeur au pif il faux ecrire l'equation pour trouver l'angle)
    double Sigma = CalculSigma(rho_cheerio, rho_liq, theta, &warning_counter);
    printf("L_c = %lfm Sigma = %lf B = %lf B_lin = %lf Theta = %lf°\n",capilary_length ,Sigma, B, CalculLinearBondNumber(R,capilary_length), theta/M_PI * 180);
    ////////////////////////////////////////////////////////////////////////////
    int i, j;
    double puissance_force, distance_squared, speed, distance, impulse, l;
    vec2_t forceAvecDirection, vCollision, vCollisionNorm, vRelativeVelocity, sensij, new_pos, new_acc, new_vel;
    // O(NT*(nb*nb+nb*nb+nb)) // pour linstant
    for(long int nt = 0; nt < NT; nt++){                                                    // on itere autant fois que le nombre de pas de temps 
        if (nt % (NT / 100) == 0) printf("%%%ld\n", nt/(NT/100)); // pour voir le progress
        // TODO ici on peux le faire plus court on faisant telle que quand on calcule les deux cheerios en meme temps
        // test de collision
        for(i = 0; i < nb_cheerios; i++){
            for(j = 0; j < nb_cheerios; j++){
                // multiplication est plus vite que sqrt
                distance_squared = (cheerios[j].pos.x - cheerios[i].pos.x) * (cheerios[j].pos.x - cheerios[i].pos.x) + 
                                            (cheerios[j].pos.y - cheerios[i].pos.y) * (cheerios[j].pos.y - cheerios[i].pos.y);
                if(j != i && distance_squared  <= (cheerios[i].d/2 + cheerios[j].d/2)*(cheerios[i].d/2 + cheerios[j].d/2) ){
                    vCollision.x = cheerios[j].pos.x - cheerios[i].pos.x;
                    vCollision.y = cheerios[j].pos.y - cheerios[i].pos.y;
                    distance = sqrt(distance_squared);//sqrt((cheerios[j].pos.x-cheerios[i].pos.x)*(cheerios[j].pos.x-cheerios[i].pos.x)+(cheerios[j].pos.y - cheerios[i].pos.y)*(cheerios[j].pos.y - cheerios[i].pos.y));  
                    vCollisionNorm.x = vCollision.x/distance,
                    vCollisionNorm.y = vCollision.y/distance;
                    vRelativeVelocity.x = cheerios[i].v.x - cheerios[j].v.x;
                    vRelativeVelocity.y = cheerios[i].v.y - cheerios[j].v.y;
                    speed = vRelativeVelocity.x * vCollisionNorm.x + vRelativeVelocity.y * vCollisionNorm.y;
                    // TODO ttrouver cette constante 
                    speed *= 0.4; // correction* ca depend plus de dt que ca mais quand meme il faux pas le metre trop bas ou trop haut// entre 0.5 et 0.8 car si on met plus haut ca rebondis pas mal et si on mets trop bas ils rentre entre eux// le coefficint qui fait tel que ca robondis pas NE PAS LE METRE TROP BAS CAR CA PEUX ENFONCER DEDANS OU REBONDIR TROP
                    if(speed > 0){
                        // Avec les moments 
                        impulse = 2 * speed / (cheerios[i].m + cheerios[j].m);            // Basic
                        cheerios[i].v.x -= (impulse * cheerios[j].m* vCollisionNorm.x);   // che[i].v.x -= (speed * vCollisionNorm.x);
                        cheerios[i].v.y -= (impulse * cheerios[j].m* vCollisionNorm.y);   // che[i].v.y -= (speed * vCollisionNorm.y);
                        cheerios[j].v.x += (impulse * cheerios[i].m* vCollisionNorm.x);   // che[j].v.x += (speed * vCollisionNorm.x);
                        cheerios[j].v.y += (impulse * cheerios[i].m* vCollisionNorm.y);   // che[j].v.y += (speed * vCollisionNorm.y);
                    }
                }
            }
        }
        // trouver les forces et les nouvelles positions
        for(i = 0; i < nb_cheerios; i++){
            forceAvecDirection.x = 0;// initialise chaque fois a 0 pour chaque cheerio
            forceAvecDirection.y = 0;
            puissance_force = 0;
            for(j = 0; j < nb_cheerios; j++){
                l = sqrt(sq(cheerios[j].pos.x - cheerios[i].pos.x) + sq(cheerios[j].pos.y - cheerios[i].pos.y));
                if(l > 0){
                    // TODO je sais pas pq ca se pousse (probablement de notre angle mais pq ?)
                    puissance_force = -ForceBetweenTwoInteractingParticles(surface_tension, R, B, Sigma, l, capilary_length);// enlever le - pour une force de attraction
                    // maintenant trouver le sens
                    sensij.x = cheerios[j].pos.x - cheerios[i].pos.x;
                    sensij.y = cheerios[j].pos.y - cheerios[i].pos.y;
                    forceAvecDirection = VecteurAdition(forceAvecDirection, VectorTimesScalar(sensij, puissance_force));
                }
            }
            cheerios[i].f_applique = forceAvecDirection;
        }

        // Integration de Verlet 
        // On peux lutiliser cellui ci car notre acceleration depend seulement des interactions entre cheerios 
        // et que notre acceleration ne depend pas de la vitesse 
        // on peux pas metre cette boucle dans lautre car sinon ca changerait la position chaque cheerio un par un et les nouvelles positions changerait au cours du temps par rapport ou on comence a calculer 
        for(i = 0; i < nb_cheerios; i++){
            new_pos = VecteurAdition(VecteurAdition(cheerios[i].pos, VectorTimesScalar(cheerios[i].v, dt)), VectorTimesScalar(cheerios[i].a, dt*dt*0.5));
            new_acc = VectorTimesScalar(cheerios[i].f_applique, 1/cheerios[i].m);
            new_vel = VecteurAdition(cheerios[i].v, VectorTimesScalar(VecteurAdition(cheerios[i].a, new_acc),(dt*0.5)));
            cheerios[i].pos= new_pos;
            cheerios[i].v  = new_vel;
            cheerios[i].a  = new_acc;
        }

        // TODO a partir de quel moment on a plus de boost de vitesse pour lecriture 
        // avec 11 1000000 0.001 => chaque iteration ~ 53s, 10 ~ 23s, 100 ~ 23s,  1000 ~ 21s, et si on ecris pas ca prend ~ 20s  
        // mais en mem temps plus on a de iterations on a plus de donnes et ca prendplus de place par example on a eu avant de print seulement les 100 iterations des fichiers de 15 GB et comme on les lis 100 par 100 dans python ou plus on a pas besoin de autant de donnees 
        if(nt%100 == 0) 
            EcritureData("donnees.txt", cheerios, nb_cheerios, nt);                             // On fait l'ecriturechawue fois comme ca on a bas besoin de stocker toute les donnees passees. 
    }                  
    free(cheerios);
    return 0;
}