#include <stdio.h>
#include <stdlib.h>
#include "cheerios.h"

// Print les 10 premières valeurs de nos objets pour vérifier que notre lecture fut fonctionnelle.
void voirSiNotreLectureABienMarche(cheerio_t* cheerios, int nb_cheerios, long int NT, 
                                     double dt, double rho_liq, double rho_air, 
                                        double rho_cheerio, double surface_tension, double g,
											bord_t *bord){
    printf("%ld %d %lf\n", NT, nb_cheerios, dt);
    printf("%lf %lf %lf %lf %lf\n", rho_liq, rho_air, rho_cheerio, surface_tension, g);
	printf("%lf %lf %lf %lf %lf %lf %lf %lf\n", bord->rayon, bord->centre.x, bord->centre.y, bord->rho, bord->theta, bord->Bo, bord->R, bord->sigma);
    int nb_print = nb_cheerios > 10 ? 10 : nb_cheerios; // comme ca si on a beaucoup de cherios on afiche pas tout on check maximum les 10 premiers et on assume que si les 10 premiers sont bien fonctione les autres marche aussi
    for(int i = 0; i < nb_print; i++){
        printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", cheerios[i].pos.x, cheerios[i].pos.y, cheerios[i].d, cheerios[i].v.x, cheerios[i].a.x,
                                                    cheerios[i].m, cheerios[i].R, cheerios[i].Bo, cheerios[i].theta, cheerios[i].Sigma);
    }
}

// Permet de lire le fichier donnees_initiales.txt afin de récupérer les informations de nos objets.
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
    cher->R = d/2.;//1./(d/2.);  // TODO on est bien daccord ceci est le rayon de la courbure ? aparament non car ca marche que quand on prend R
}

// Retourne un tableau de cheerio avec à l'intérieur chaque cheerio avec leurs caractéristique données dans le fichier donnees_initiales.txt
// Returne ladresse aloue pour le tableau cheerios
cheerio_t* LectureTouteCheerios(char* nom_fichier, int* nb_cheerios, long int* NT, double* dt,
                                    double* rho_liq, double* rho_air, double* rho_cheerio, double* surface_tension, double* g,
									bord_t *bord){
	FILE* fichier_avec_donnees_initiales_cheerios = fopen(nom_fichier,"r");
	if (fichier_avec_donnees_initiales_cheerios == NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		double tmp_bord_rayon, tmp_bord_x, tmp_bord_y, tmp_bord_rho, tmp_bord_angle_radian; // TODO ajouter lecture bord 
		fscanf(fichier_avec_donnees_initiales_cheerios, "%d %ld %lf", nb_cheerios, NT, dt); // la premiere ligne sur le fichier indique le nombre de cheerios(objets flottants)
		fscanf(fichier_avec_donnees_initiales_cheerios, "%lf %lf %lf %lf %lf", rho_liq, rho_air, rho_cheerio, surface_tension, g);
        fscanf(fichier_avec_donnees_initiales_cheerios, "%lf %lf %lf %lf %lf", &tmp_bord_rayon, &tmp_bord_x, &tmp_bord_y, &tmp_bord_rho, &tmp_bord_angle_radian);
		bord->rayon = tmp_bord_rayon; 
		bord->centre.x = tmp_bord_x; 
		bord->centre.y = tmp_bord_y; 
		bord->rho = tmp_bord_rho; 
		bord->theta = tmp_bord_angle_radian;
		*surface_tension = *surface_tension/1000.; // car on prend les valeurs mN/m
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
										nt, cheerios[c].pos.x, cheerios[c].pos.y, cheerios[c].d);
		}
		fclose(fichier_de_ecriture);
	}
}