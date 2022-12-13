#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "cheerios.h"
#include "calculs.h"


// Permet d'afficher une barre de progression lorsque le programme s'exécute. 
void ProgressBar(long int nt, long int NT, double dt){
	if (nt % (NT / 100) == 0){
		printf("\r%%%ld nt = %ld (%.2lfs)", nt/(NT/100), nt, nt*dt); // pour voir l'avancement
		// TODO voir avec erdi "flush"
		fflush(stdout); // le \r écrit par dessus la ligne et il ne faut pas mettre \n car ça fait tun flush implicitement mais nous on a besoin de flush apres pour reecrir
	}
}

// Print les 10 premières valeurs de nos objets pour vérifier que notre lecture fut fonctionnelle.
void VoirSiNotreLectureABienMarche(cheerio_t* cheerios, int nb_cheerios, long int NT, 
                                     double dt, double rho_liq, double rho_air, 
                                        double rho_cheerio, double surface_tension, double g,
											bord_t *bord){
    printf("%ld %d %lf\n", NT, nb_cheerios, dt);
    printf("rho_l=%lf rho_a=%lf rho_c=%lf surface tension=%lf g=%lf\n", rho_liq, rho_air, rho_cheerio, surface_tension, g);
	printf("bord:\nr=%lf x=%lf y=%lf rho=%lf %lf° B=%lf R=%lf Sig=%lf\n", bord->rayon, bord->centre.x, bord->centre.y, bord->rho, bord->angle_contact*180/M_PI, bord->Bond_nb, bord->rayon_courbure, bord->Sigma);
    int nb_print = nb_cheerios > 10 ? 10 : nb_cheerios; // Si on a beaucoup de cheerios on n'afiche pas tout, on check maximum les 10 premiers et on assume que si les 10 premiers ont bien fonctioné les autres fonctionnent aussi.
    for(int i = 0; i < nb_print; i++){
        printf("%lf %lf dia=%lf v_x=%lf v_y=%lf m=%lf R_c=%lf B=%lf %lf° Sig=%lf\n", cheerios[i].pos.x, cheerios[i].pos.y, cheerios[i].diametre_cheerio, cheerios[i].v.x, cheerios[i].v.y,
                                                    cheerios[i].masse, cheerios[i].rayon_courbure, cheerios[i].Bond_nb, cheerios[i].angle_contact*180/M_PI, cheerios[i].Sigma);
    }
}

// Permet de lire le fichier donnees_initiales.txt afin de récupérer les informations de nos objets.
void LectureData(FILE* fichier, cheerio_t *cher, double diametre_cheerio, double masse_cheerio){
	int success_scanning = 0;
	double posx = 0,posy = 0 , v_x = 0, v_y = 0, a_x = 0, a_y = 0, f_x = 0, f_y = 0;
	success_scanning = fscanf(fichier,"%lf %lf %lf %lf", &posx, &posy, &v_x, &v_y);
	if (!success_scanning){
		printf("Error scanning\n");
		exit(1);
	}
	cher->pos.x = posx; // si on met cela dans le fscanf ca bug même si on prends les adresses c'est pour ca que cela est en dehors.
	cher->pos.y = posy;
	cher->diametre_cheerio = diametre_cheerio;
	cher->v.x = v_x;
	cher->v.y = v_y;
	cher->a.x = a_x;
	cher->a.x = a_y;
	cher->masse = masse_cheerio;
	cher->f_applique.x = f_x;
	cher->f_applique.y = f_y;
    cher->rayon_courbure = diametre_cheerio/2.;
}

// returne un double random dans linterval [min, max]
double RandomAtAInterval( double min, double max ){
    double echelle = rand() / (double) RAND_MAX; 
    return min + echelle * ( max - min );   
}

typedef struct rectangle{
	double xmin, ymin, xmax, ymax;
}rectangle_t;
                                 
// ca mets des cheerios randomise a partir dun cheerios exemple avec une deviation de ce cheerios de +-0.5 masse, diametre du cheerio
void PutRandomData(bord_t bord,cheerio_t cheerio_moyenne , cheerio_t *cheerios, int nb_cheerios){
	// on prend le plus grand carree qui poeux rentrer dans notre cercle et on randomise dedans.
	rectangle_t carre = {.xmin = bord.centre.x - (bord.rayon/sqrt(2) + cheerio_moyenne.diametre_cheerio),
						 .xmax = bord.centre.x + (bord.rayon/sqrt(2) - cheerio_moyenne.diametre_cheerio),
						 .ymin = bord.centre.y - (bord.rayon/sqrt(2) + cheerio_moyenne.diametre_cheerio),
						 .ymax = bord.centre.y + (bord.rayon/sqrt(2) - cheerio_moyenne.diametre_cheerio)};
	time_t t1;				
	vec2_t vec_nulle = {.x = 0, .y = 0};     
	srand((unsigned) time (&t1));
	double rho_cheerio = cheerio_moyenne.masse/CalculVolumeSphere(cheerio_moyenne.diametre_cheerio);
	for(int i = 0; i < nb_cheerios; i++){
		cheerios[i].diametre_cheerio = cheerio_moyenne.diametre_cheerio;
		cheerios[i].rayon_courbure = cheerios[i].diametre_cheerio/2;
		cheerios[i].masse = rho_cheerio*CalculVolumeSphere(cheerios[i].diametre_cheerio);
		cheerios[i].pos.x = RandomAtAInterval(carre.xmin, carre.xmax);
		cheerios[i].pos.y = RandomAtAInterval(carre.ymin, carre.ymax); 
		cheerios[i].v          = vec_nulle;//cheerio_moyenne.v;//vec_nulle;
		cheerios[i].a          = vec_nulle;//cheerio_moyenne.a;//vec_nulle;
		cheerios[i].f_applique = vec_nulle;//cheerio_moyenne.f_applique;//vec_nulle;
	}
}

// Retourne un tableau de cheerio avec à l'intérieur chaque cheerio avec leurs caractéristique données dans le fichier donnees_initiales.txt
// Returne l'adresse alouée pour le tableau de cheerios.
cheerio_t* LectureTouteCheerios(char* nom_fichier, int* nb_cheerios, long int* NT, double* dt,
                                    double* rho_liq, double* rho_air, double* surface_tension, double* g,
									bord_t *bord){
	FILE* fichier_avec_donnees_initiales_cheerios = fopen(nom_fichier,"r");
	int success_scanning = 0;
	char typeSimulation[100];	// si on va prendre les cheerios aleatoires ou celles que on a mis si on a mis Random ou Standard
	double masse_cheerios, diametre_cheerios;
	if (fichier_avec_donnees_initiales_cheerios == NULL) {
		printf("Error opening file!\n");
		exit(1);
	} else {
		success_scanning = fscanf(fichier_avec_donnees_initiales_cheerios, "%s", typeSimulation);
		double tmp_bord_rayon, tmp_bord_x, tmp_bord_y, tmp_bord_rho, tmp_angle_contact_radian; // TODO ajouter lecture bord 
		success_scanning = fscanf(fichier_avec_donnees_initiales_cheerios, "%d %ld %lf", nb_cheerios, NT, dt); // la premiere ligne sur le fichier indique le nombre de cheerios(objets flottants)
		success_scanning = fscanf(fichier_avec_donnees_initiales_cheerios, "%lf %lf %lf %lf", rho_liq, rho_air, surface_tension, g);
        success_scanning = fscanf(fichier_avec_donnees_initiales_cheerios, "%lf %lf %lf %lf", &tmp_bord_rayon, &tmp_bord_x, &tmp_bord_y, &tmp_bord_rho);
		success_scanning = fscanf(fichier_avec_donnees_initiales_cheerios, "%lf %lf %lf", &diametre_cheerios, &masse_cheerios,&tmp_angle_contact_radian);
		if (!success_scanning){ 
			printf("Error scanning\n");
			exit(1);
		}
		bord->rayon = tmp_bord_rayon; 
		bord->centre.x = tmp_bord_x; 
		bord->centre.y = tmp_bord_y; 
		bord->rho = tmp_bord_rho; 
		*surface_tension = *surface_tension/1000.; // car on prend les valeurs mN/m
        cheerio_t *cheerios = NULL;
		cheerios = malloc(sizeof(cheerio_t)* *nb_cheerios);
		if (typeSimulation[0] == 'R' || typeSimulation[0] == 'r') {
			cheerio_t cheerio_moyenne;
			LectureData(fichier_avec_donnees_initiales_cheerios, &cheerio_moyenne, diametre_cheerios, masse_cheerios);
			cheerio_moyenne.angle_contact = tmp_angle_contact_radian;
			PutRandomData(*bord, cheerio_moyenne, cheerios, *nb_cheerios);
			
		} else {
			for(int i = 0; i < *nb_cheerios; i++){
				LectureData(fichier_avec_donnees_initiales_cheerios, &cheerios[i], diametre_cheerios, masse_cheerios);
				cheerios[i].angle_contact = tmp_angle_contact_radian;
			}
		}
		fclose(fichier_avec_donnees_initiales_cheerios);
		printf("Lecture data fin\n");
		return cheerios;
	}
}

// Initialise le fichier où les coordonnées des Cheerios seront transmises.
void InitialiseFichierDeEcriture(char* nom_fichier){
	FILE* fichier_de_ecriture= fopen(nom_fichier,"w"); //When you open a file with "w" flag it creates an empty file for writing. If a file with the same name already exists its contents are erased and the file is treated as an empty new file. source: https://stackoverflow.com/questions/4815251/how-do-i-clear-the-whole-contents-of-a-file-in-c#:~:text=If%20a%20file%20with%20the,was%20in%20the%20file%20previously. 
	if (fichier_de_ecriture== NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		fclose(fichier_de_ecriture);
	}
}

// Écrit les données des cheerios dans le fichier d'écriture.
void EcritureData(char* nom_fichier, cheerio_t* cheerios, int nb_cheerios, long int nt){
	FILE* fichier_de_ecriture= fopen(nom_fichier,"a"); 
	if (fichier_de_ecriture== NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		// on parcour notre tableau et on mets chaque element du tableau dans le fichier 
		for(int c = 0; c < nb_cheerios; c++){
			fprintf(fichier_de_ecriture, "%ld %.16lf %.16lf %.16lf\n",//%.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n", //"%ld %g %g %g %g %g %g %g %g %g %g\n",
										nt, cheerios[c].pos.x, cheerios[c].pos.y, cheerios[c].diametre_cheerio);
		}
		fclose(fichier_de_ecriture);
	}
}