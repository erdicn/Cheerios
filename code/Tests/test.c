#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NB 100

typedef struct Vec2{
    double x, y;
} vec2_t;

typedef struct Cheerio{
    vec2_t pos;           // position cheerio
    double d;             // diametre cheerio
    vec2_t v;             // vitesse cheerio
    vec2_t a;             // acceleration cheerio
    vec2_t f_applique;    // force applique sur le cheerio
    double m;             // masse du cheerio
} cheerio_t;  


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

void printCheerio(cheerio_t cheerio, long int nt){
    printf("%ld %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf %.16lf\n", //"%ld %g %g %g %g %g %g %g %g %g %g\n",
										nt,
										cheerio.pos.x, cheerio.pos.y,
										cheerio.d,
										cheerio.v.x, cheerio.v.y,
										cheerio.a.x, cheerio.a.y,
										cheerio.f_applique.x, cheerio.f_applique.y,
										cheerio.m);
}

int main(){
    __float128 f = 0.0000000000000000000000000000000000000001;
    f++;
    int i = 0,c, tot = 0;
    cheerio_t* cheerios = malloc(sizeof(cheerio_t)*NB*NB);
    //EcritureData("test.txt", cheerios, NB*NB, i);
    for(i = 0; i < NB; i++)
        printf("%d\n", i);
    //     for(c = 0; c < NB; c++)
    //         //printCheerio(cheerios[c], i);
    //         EcritureData("test.txt", cheerios, NB, i);
    // }
    // for(i = 0; i < NB; i++){
    //     for(int c = i+1; c < NB; c++){
    //         tot++;
    //     }
    // }
    // printf("%d %.0f\n",tot, (NB*(NB - 1))/2.);
    return 0;
}