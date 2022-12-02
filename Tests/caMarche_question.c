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
    double diametre_cheerio;             // diametre cheerio
    double masse;             // masse du cheerio
} cheerio_t;     

void voirSiNotreLectureABienMarche(cheerio_t* cheerios, int nb_cheerios){
    int nb_print = nb_cheerios > 10 ? 10 : nb_cheerios; // comme ca si on a beaucour de cherios on afiche pas tout on check maximum les 10 premiers et on assume que si les 10 premiers sont bien fonctione les autres marche aussi
    for(int i = 0; i < nb_print; i++){
        printf("%lf %lf %lf %lf %lf\n", cheerios[i].pos.x, cheerios[i].pos.y, cheerios[i].diametre_cheerio, cheerios[i].v.x, cheerios[i].a.x);
    }
}

void LectureData(FILE* fichier, cheerio_t *cher){
	double posx = 0,posy = 0 ,d = 0, v_x = 0, v_y = 0, a_x = 0, a_y = 0, m = 0, f_x = 0, f_y = 0;
	fscanf(fichier,"%lf %lf %lf %lf %lf %lf %lf %lf", &posx, &posy, &d, &v_x, &v_y, &a_x, &a_y, &m);//, tmp);
	cher->pos.x = posx; // si on mets cela dans le fscanf ca bug meme si on prends les adresses cest pour ca que cest en dehors
	cher->pos.y = posy;
	cher->diametre_cheerio = d;
	cher->v.x = v_x;
	cher->v.y = v_y;
	cher->a.x = a_x;
	cher->a.x = a_y;
	cher->masse = m;
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
										nt, cheerios[c].pos.x, cheerios[c].pos.y, cheerios[c].diametre_cheerio);
		}
		fclose(fichier_de_ecriture);
	}
}

vec2_t VecteurAdition(vec2_t v1, vec2_t v2){
    vec2_t new_vec = {.x = v1.x + v2.x,
                      .y = v1.y + v2.y};
    return new_vec;
}

vec2_t VectorTimesScalar(vec2_t vec, double s){
    vec2_t new_vec = {.x = vec.x * s,
                      .y = vec.y * s};
    return new_vec;
}

int main(){
    long int NT;                                                          // nombre de pas de temps 
    double dt;                     // TODO trouver la masse volumique des cheerios// masses volumiques en kg/m^3 source air https://www.thermexcel.com/french/tables/massair.htm
    int nb_cheerios;
    cheerio_t *che= NULL;                                                  // notre tableaux qui est remplie de cheerios
    che = LectureTouteCheerios("cheerio_donnees_test.txt", &nb_cheerios, &NT, &dt);  
    voirSiNotreLectureABienMarche(che, nb_cheerios);
    InitialiseFichierDeEcriture("donnees.txt");                                 // ca efface tout le fichier donnees.txt pour que on a un fichier vide pour les nouvelles donnees 

    int i;
    for(long int nt = 0; nt < NT; nt++){                                                    // on itere autant fois que le nombre de pas de temps 
        if (nt % (NT / 100) == 0) printf("%%%ld\n", nt/(NT/100)); // pour voir le progress
        
        // test de collision
        for(i = 0; i < nb_cheerios; i++){
            for(int j = 0; j < nb_cheerios; j++){
                if(j != i && 
                            (che[j].pos.x - che[i].pos.x) * (che[j].pos.x - che[i].pos.x) + 
                                    (che[j].pos.y - che[i].pos.y) * (che[j].pos.y - che[i].pos.y)  <= (che[i].diametre_cheerio/2 + che[j].diametre_cheerio/2)*(che[i].diametre_cheerio/2 + che[j].diametre_cheerio/2) ){
                    vec2_t vCollision = {.x = che[j].pos.x - che[i].pos.x,
                                         .y = che[j].pos.y - che[i].pos.y};
                    double distance = sqrt((che[j].pos.x-che[i].pos.x)*(che[j].pos.x-che[i].pos.x)+
                                            (che[j].pos.y - che[i].pos.y)*(che[j].pos.y - che[i].pos.y));  
                    vec2_t vCollisionNorm = {.x = vCollision.x/distance,
                                             .y = vCollision.y/distance};
                    vec2_t vRelativeVelocity = {.x = che[i].v.x - che[j].v.x,
                                                .y = che[i].v.y - che[j].v.y};
                    double speed = vRelativeVelocity.x * vCollisionNorm.x + 
                                   vRelativeVelocity.y * vCollisionNorm.y;
                    if(speed > 0){
                        double impulse = 2 * speed / (che[i].masse + che[j].masse);
                        che[i].v.x -= (impulse * che[j].masse* vCollisionNorm.x);
                        che[i].v.y -= (impulse * che[j].masse* vCollisionNorm.y);
                        che[j].v.x += (impulse * che[i].masse* vCollisionNorm.x);
                        che[j].v.y += (impulse * che[i].masse* vCollisionNorm.y);
                        // Basic
                        // che[i].v.x -= (speed * vCollisionNorm.x);
                        // che[i].v.y -= (speed * vCollisionNorm.y);
                        // che[j].v.x += (speed * vCollisionNorm.x);
                        // che[j].v.y += (speed * vCollisionNorm.y);
                    }
                    // che[i].v = VectorTimesScalar(che[i].v, -1);
                    // che[j].v = VectorTimesScalar(che[j].v, -1);
                }
            }
        }
        // changements de positions 
        for(i = 0; i < nb_cheerios; i++){
            che[i].pos = VecteurAdition(che[i].pos, VectorTimesScalar(che[i].v, dt));
        }


        EcritureData("donnees.txt", che, nb_cheerios, nt);                             // On fait l'ecriturechawue fois comme ca on a bas besoin de stocker toute les donnees passees. 
    }                  
    free(che);
    return 0;
}