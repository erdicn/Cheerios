#include <stdio.h>
#include <stdlib.h>

int main(){
    int nb_cheerios = 7;
    double* liste = NULL;
    liste = malloc(sizeof(double)*(nb_cheerios));
    printf("Liste malloce\n");
    for(int i = 0; i < nb_cheerios; i++){
        printf("Boucle %d pas initialise %lf\n", i, liste[i]);
        liste[i] = -1;
        printf("Initialise %lf\n", liste[i]);
    }
    printf("Sortie de la boucle\n");
    free(liste);
    return 0;
}