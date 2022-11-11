#include <math.h>
#include <stdio.h>
#include <gsl/gsl_sf_bessel.h>

#warning The homamade functions doesnt works they are not accurate enough or i did make a mistake 

// only works for ints 
long long int homemadeGammaFunc(long int n){
    long long int tot = 1;
    for(long long int i = 1; i < n; i++){
        tot *= i;
    }
    return tot;
}

long long int fact(long int n){
    long long int tot = 1;
    for(long long int i = 1; i < n + 1; i++){
        tot *= i;
    }
    return tot;
}


double homemade_I(double alpha, double x, double epsilon){
    // alpha is an integer so we can use limits and easier gamma function calculation
    double tot = 0, new_tot = 1;
    if( alpha - (int)floor(alpha) < 0.0000000001){
        long int m = 0;
        while (new_tot - tot > epsilon){
            tot += (1/(fact(m)*homemadeGammaFunc(m+alpha+1))) * 
                                pow(x/2, 2*m + alpha);
        }
    } else {
        printf("ERROR the homemade_I function is not made yet for non integer alphas\n");
    }

    return tot;
}

double homemade_K(double alpha, double x, double epsilon){
    return (M_PI/2) * 
                ((homemade_I(-alpha, x, epsilon) - homemade_I(alpha, x, epsilon)) 
                            / sin(alpha*M_PI) );
}

int main(){
    gsl_sf_result resultat;
    for(double i = 1; i < 2; i += 0.00001 ){
        gsl_sf_bessel_K1_e(i, &resultat);
        printf("i = %.16lf %.16lf %.16lf ", i,gsl_sf_bessel_K1(i), 1/i);
        printf("error is %.16lf\n", resultat.err);
        printf("MY %.16lf\n", homemade_K(1, i, 0.0000000001));
    }

    return 0;
}