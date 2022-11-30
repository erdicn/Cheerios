#ifndef LECTURE_ECRITURE_H
#define LECTURE_ECRITURE_H
#include "cheerios.h"
#include "stdio.h"
#include "stdlib.h"
void voirSiNotreLectureABienMarche(cheerio_t* cheerios, int nb_cheerios, long int NT, 
                                     double dt, double rho_liq, double rho_air, 
                                        double rho_cheerio, double surface_tension, double g);
void LectureData(FILE* fichier, cheerio_t *cher);
cheerio_t* LectureTouteCheerios(char* nom_fichier, int* nb_cheerios, long int* NT, double* dt,
                                    double* rho_liq, double* rho_air, double* rho_cheerio, double* surface_tension, double* g);
void InitialiseFichierDeEcriture(char* nom_fichier);
void EcritureData(char* nom_fichier, cheerio_t* cheerios, int nb_cheerios, long int nt);
#endif //LECTURE_ECRITURE_H