#include <stdio.h>

#define DITHER_STEP 1.0;
int get_flat_dither(int iteration, double *ra_dither, double *dec_dither);

main(int argc, char **argv)
{
    int i;
    double ra_dither, dec_dither;
    sscanf(argv[1],"%d",&i);

    get_flat_dither(i,&ra_dither,&dec_dither);

    printf("%d  %7.3f  %7.3f\n",
         i,ra_dither,dec_dither);

    exit(0);
}


get_flat_dither(int iteration, double *ra_dither, double *dec_dither)
{
    int i,square_size,i0,side,step_a,step_b;


    if (iteration == 0){
         *ra_dither=0.0;
         *dec_dither=0.0;
         return(0);
    }

    else if (iteration <= 8){ 
        square_size=3;
        i0=1;
    }
    else if (iteration <= 24 ){ 
       square_size=5;
       i0=9;
    }
    else if (iteration <= 48 ){ 
       square_size=7;
       i0=25;
    }
    else if (iteration <= 80 ){
       square_size=9;
       i0=49;
    }
    else if (iteration <= 120 ){
       square_size=11;
       i0=81;
    }
    else{
       fprintf(stderr,"get_flat_dither: too many iterations: %d\n",iteration);
       *ra_dither=0;
       *dec_dither=0;
       return(-1);
    }

    i=iteration-i0;
    side=i/(square_size-1);
    step_a=square_size/2;
    step_b=i-side*(square_size-1);


    if(side==0){
       *ra_dither=step_a*DITHER_STEP;
       *dec_dither=(step_b-step_a)*DITHER_STEP;
    }
    else if (side==1){
       *ra_dither=(step_b-step_a+1)*DITHER_STEP;
       *dec_dither=step_a*DITHER_STEP;
    }
    else if (side==2){
       *ra_dither=-step_a*DITHER_STEP;
       *dec_dither=(step_b-step_a+1)*DITHER_STEP;
    }
    else{
       *ra_dither=(step_b-step_a)*DITHER_STEP;
       *dec_dither=-step_a*DITHER_STEP;
    }
     
    return(0);
}

