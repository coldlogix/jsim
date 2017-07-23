/* calculate a-h solution. Likharev 116  */

#include <math.h>
#include <stdio.h>

double modb(double z);

double modb(double z)
/* modified bessel fn I0 */

{
double a,t,p,t2,t4,i2,i4;
t = z / 3.75;
if (z>3.75)
{
i2=1.0/t/t; i4=i2*i2;
p = 0.39894228 + 0.01328592/t + 0.00225319*i2 - 0.000157565*i2/t
    + 0.00916281 *i4   - 0.02057706*i4/t 
   + 0.02635537 *i4 *i2 - 0.01647633*i4*i2/t
  +  0.00392377*i4*i4;
a = exp (z) * p /sqrt(z);
}
else
{
t2 = t*t; t4 = t2 *t2;
a = 1 +3.5156229 *t2 + 3.0899224*t4 + 1.2067492*t4*t2 
+ 0.2659732*t4*t4 + 0.0360768*t4*t4*t2 
+ 0.0045813*t4*t4*t4 ;
}
return a;
}

main()

{
double v, integral, dx, x, y ,gam ,i;
int j,jmax;
printf("Enter current, gamma \n");
scanf("%F %F", &i, &gam);
jmax = 100 + 60/gam;
integral = 0;
dx = M_PI/jmax/2;
for (j=0; j<jmax ;j++)
	{
	x = (j+ 0.5) * dx;
        y = cosh ( i*x*2.0/gam ) * modb(2.0*cos(x)/gam);
	integral = integral +y*dx * 2.0 / M_PI ; 
	}
v = gam / M_PI /integral * sinh(M_PI * i/gam);
printf("v = %g \n",v);
}
