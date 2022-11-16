#ifndef LECTURE_ECRITUREDONNEE_H
#define LECTURE_ECRITUREDONNEE_H
#include <stdio.h>
#include "cheerios.h"

cheerio_t* LectureTouteCheerios(char* nom_fichier, int* nb_cheerios,long int* NT, double* dt);
void LectureData(FILE* fichier, cheerio_t *cher);
void InitialiseFichierDeEcriture(char* nom_fichier);
void EcritureData(char* nom_fichier, cheerio_t* cheerios, int nb_cheerios, long int nt);
#endif // LECTURE_ECRITUREDONNEE_H