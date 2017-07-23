#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Define generator and scaling factor*/
#if defined USERAND
#define  RANDGEN rand() /2048.0/1024.0/1024.0
#elif defined USERAND48
#define  RANDGEN lrand48()/2048.0/1024.0/1024.0 
#else
#define  RANDGEN random()/2048.0/1024.0/1024.0 
#endif

void main(void)
{
double x,vals[128],corr[128],d[128],mom[4],dmom[3];
int i,j,k,n;
for (i=0;i<128;i++) 
	{
  	vals[i] = RANDGEN ;
	corr[i] = 0.0; d[i] = 0.0;
        }
n= 1000000;
for (k=0;k<n;k++)
     {
     x = RANDGEN ;
     for(j=127;j>0;j--)
          vals[j] = vals[j-1];
    vals[0] = x; mom[0]=mom[0]+x/n;
mom[1]=mom[1]+x*x/n;mom[2]=mom[2]+x*x*x/n;mom[3]=mom[3]+x*x*x*x/n;
dmom[0]=mom[1]+(1-x)*(1-x)/n;dmom[1]=mom[2]+(1-x)*(1-x)*(1-x)/n;
dmom[2]=mom[3]+(1-x)*(1-x)*(1-x)*(1-x)/n;
    i = floor(x*128.0); 
d[i] = 128.0/n + d[i];
    for(j=0;j<128;j++)
          corr[j] = corr[j] + x*vals[j];
    }
printf(" %d numbers generated. SEM +/- %8.5f \n",n,sqrt(1.0/12.0/n));
printf("Moments    %8.5f    %8.5f %8.5f %8.5f \n",mom[0],mom[1],mom[2],mom[3]);
printf("Complement Moments  %8.5f %8.5f %8.5f \n",dmom[0],dmom[1],dmom[2]);
for (j=0;j<16;j++) 
	{
for (i=0;i<8;i++) 
	printf("%8.4f ",corr[8*j+i]/n);
      printf("\n");
       }

      printf("\nDistribution \n");
for (j=0;j<16;j++) 
	{
for (i=0;i<8;i++) 
	printf("%8.4f ",d[8*j+i]);
      printf("\n");
       }
}