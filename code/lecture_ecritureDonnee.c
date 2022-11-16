/**
 * @file lecture_ecritureDonnee.c
 * @authors Baptiste BRAUN-DELVOYE, Erdi Çan
 * @brief Fichier permettant l'écriture et la lecture de nos données.
 * @version 5.5
 * @date 2022-11-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cheerios.h"
#define BUFFER_TAILLE_MAX 1000

void LectureData(FILE* fichier, cheerio_t *cher);
 
cheerio_t* LectureTouteCheerios(char* nom_fichier, int* nb_cheerios, long int* NT, double* dt){
	FILE* fichier_avec_donnees_initiales_cheerios = fopen(nom_fichier,"r");
	if (fichier_avec_donnees_initiales_cheerios == NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		fscanf(fichier_avec_donnees_initiales_cheerios, "%d %ld %lf", nb_cheerios, NT, dt); // la premiere ligne sur le fichier indique le nombre de cheerios(objets flottants)
		cheerio_t *cheerios= NULL;
		cheerios = malloc(sizeof(cheerio_t)* *nb_cheerios);
		for(int i = 0; i < *nb_cheerios; i++){
			LectureData(fichier_avec_donnees_initiales_cheerios, &cheerios[i]);
		}
		fclose(fichier_avec_donnees_initiales_cheerios);
		return cheerios;
	}
}

void LectureData(FILE* fichier, cheerio_t *cher){
    //char tmp[BUFFER_TAILLE_MAX];
	double posx = 0,posy = 0 ,d = 0, v_x = 0, v_y = 0, a_x = 0, a_y = 0, m = 0, f_x = 0, f_y = 0;
	// %[^\n]%*c  si il ya pas autre chose il faux pas le metre car sinon ca marche pas autrement si il ya des extra if faux le metre 
	fscanf(fichier,"%lf %lf %lf %lf %lf %lf %lf %lf", &posx, &posy, &d, &v_x, &v_y, &a_x, &a_y, &m);//, tmp);
	//printf("%lf %lf %lf %lf %lf\n", x,y,d,v,a);
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
    // The [] is the scanset character. [^\n] tells that while the input is not a newline ('\n') take input. 
    //Then with the %*c it reads the newline character from the input buffer (which is not read), and the * indicates 
    //that this read in input is discarded (assignment suppression), as you do not need it, and this newline in the 
    //buffer does not create any problem for next inputs that you might take.
}

/**
 * @brief Efface toutes les données dans le fichier pour faire tel qu'on commence avec un fichier frais sans encombrement.
 * 
 * @param nom_fichier 
 */
void InitialiseFichierDeEcriture(char* nom_fichier){
	FILE* fichier_de_ecriture= fopen(nom_fichier,"w"); //When you open a file with "w" flag it creates an empty file for writing. If a file with the same name already exists its contents are erased and the file is treated as an empty new file. source: https://stackoverflow.com/questions/4815251/how-do-i-clear-the-whole-contents-of-a-file-in-c#:~:text=If%20a%20file%20with%20the,was%20in%20the%20file%20previously. 
	if (fichier_de_ecriture== NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		fclose(fichier_de_ecriture);
	}
}

/**
 * @brief Ecrit le tableau de cheerios dans un fichier texte au fil du temps. Cela permets de connaître à quel moment il y a un problème qui surgit.
 * 
 * @param nom_fichier 
 * @param cheerios 
 * @param nb_cheerios 
 * @param nt 
 */
void EcritureData(char* nom_fichier, cheerio_t* cheerios, int nb_cheerios, long int nt){
	FILE* fichier_de_ecriture= fopen(nom_fichier,"a"); 
	if (fichier_de_ecriture== NULL){
		printf("Error opening file!\n");
		exit(1);
	} else{
		// on parcour notre tableau et on mets chaque element du tableau dans le fichier 
		for(int c = 0; c < nb_cheerios; c++){
			fprintf(fichier_de_ecriture, "%ld %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n", //"%ld %g %g %g %g %g %g %g %g %g %g\n",
										nt,
										cheerios[c].pos.x, cheerios[c].pos.y,
										cheerios[c].d,
										cheerios[c].v.x, cheerios[c].v.y,
										cheerios[c].a.x, cheerios[c].a.y,
										cheerios[c].f_applique.x, cheerios[c].f_applique.y,
										cheerios[c].m);
		}
		fclose(fichier_de_ecriture);
	}
}