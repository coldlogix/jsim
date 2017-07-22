/*************************************************************************
 *
 *  JSIM Release 2.0 10/24/92
 *
 *  JSIM is a simulator for circuits containing Josephson Junctions.
 *
 *  Author:         Emerson Fang, 1991
 *                  University of California, Berkeley
 *                  Dept. of Electrical Engineering and Computer Sciences
 *                  Cryoelectronics Group, Professor Ted Van Duzer
 *                  
 *
 *  Cleanup Crew:   Jay Fleischman    (jef@swordfish.berkeley.edu)
 *                  Steve Whiteley    (stevew@landau.conductus.com)
 *
 *************************************************************************/

#include "jsim.h"
#include "extern.h"


char *
read_function(int *ftype)
{
  read_error = read_string("source function type");

  if (read_error == OK)
  switch ((*ftype = get_string_keyword(tempstring)))
  {
    case SIN : return(read_sin());

    case PULSE : return(read_pulse());

    case PWL : return(read_pwl());

    default : *ftype = ILLEGAL;
              printf("## Error -- illegal source type %s", tempstring); 
              return(NULL);
  }

  *ftype = ILLEGAL;
  return(NULL);

}  /* read_function */


char *
read_sin()
{
  f_sin *temp_sin;

  read_error = read_double(dataptr, "offset", FALSE);
  read_error = read_double(dataptr+1, "amplitude", FALSE);
  read_error = read_double(dataptr+2, "frequency", FALSE);
  read_error = read_double(dataptr+3, "delay", FALSE);
  read_error = read_double(dataptr+4, "damping factor", FALSE);    

  if (read_error != OK)
  {
    printf("## Error -- illegal sin source definition\n");
    return(NULL);
  }
  
  if (dataptr[0] != 0.0)
  {
    printf("## Error -- offset has to be zero\n");
    return(NULL);
  }
  
  temp_sin = (f_sin *) mycalloc(1, sizeof(f_sin));

  temp_sin->off = dataptr[0];
  temp_sin->amp = dataptr[1];
  temp_sin->freq = dataptr[2];
  temp_sin->td = dataptr[3];
  temp_sin->theta = dataptr[4];

  return((char *) temp_sin);

}   /* read_sin */


char *
read_pulse()
{
  f_pulse *temp_pulse;

  read_error = read_double(dataptr, "initial val", FALSE);
  read_error = read_double(dataptr+1, "pulse val", FALSE);
  read_error = read_double(dataptr+2, "delay", FALSE);
  read_error = read_double(dataptr+3, "rise time", FALSE);
  read_error = read_double(dataptr+4, "fall time", FALSE);
  read_error = read_double(dataptr+5, "pulse width", FALSE);    
  read_error = read_double(dataptr+6, "pulse period", FALSE);    

  if (read_error != OK)
  {
    printf("## Error -- illegal pulse source definition\n");
    return(NULL);
  }
  
  if (dataptr[0] != 0.0)
  {
    printf("## Error -- initial value has to be zero\n");
    return(NULL);
  }
  
  temp_pulse = (f_pulse *) mycalloc(1, sizeof(f_pulse));

  temp_pulse->plow = dataptr[0]; 
  temp_pulse->phigh = dataptr[1];
  temp_pulse->td = dataptr[2];
  temp_pulse->tr = dataptr[3];
  temp_pulse->tf = dataptr[4];
  temp_pulse->pw = dataptr[5];
  temp_pulse->per = dataptr[6];

  return((char *) temp_pulse);

}   /* read_pulse */


char *
read_pwl()
{
  f_pwl *temp_pwl;
  int i, j;

  i = 0;
  do
  {
     read_error = read_double(dataptr+2*i, "time", TRUE);
     read_error = read_double(dataptr+2*i+1, "value", TRUE);
     if (read_error == OK) i++;
  }
  while (read_error == OK);

  if (i < 1)
  {
    printf("## Error -- bad pwl source definition, ");
    printf("too few argument\n");
    return(NULL);
  }

  if ((dataptr[0] != 0.0) || (dataptr[1] != 0.0))
  {
    if (dataptr[0] != 0.0)
      printf("## Error -- starting time has to be zero\n");

    if (dataptr[1] != 0.0)
      printf("## Error -- starting value has to be zero\n");

    return(NULL);
  }

  temp_pwl = (f_pwl *) mycalloc(1, sizeof(f_pwl));

  temp_pwl->size = i;
  temp_pwl->timedata = (double *) mycalloc(i+i, sizeof(double));  

  for (j = 0; j < i+i; j++)
    temp_pwl->timedata[j] = dataptr[j]; 

  return((char *) temp_pwl);

}   /* read_pwl */


double
func_eval(int ftype, char *fparm, double time)
{
  f_sin *temp_sin;
  f_pulse *temp_pulse;
  f_pwl *temp_pwl;
  double temp_val;
  int index, index2;


  switch (ftype)
  {
    case SIN : 

         temp_sin = (f_sin *) fparm;
 
         if (time < temp_sin->td)
           temp_val = temp_sin->off;
         else if (temp_sin->theta == 0.0)
           temp_val = temp_sin->off + temp_sin->amp * 
                      sin(PI2*temp_sin->freq* (time - temp_sin->td));
         else
           temp_val = temp_sin->off + temp_sin->amp * 
                      exp((temp_sin->td - time)*temp_sin->theta)*
                      sin(PI2*temp_sin->freq*(time - temp_sin->td));

         return(temp_val);

    case PULSE : 

         temp_pulse = (f_pulse *) fparm;
 
         if (time < temp_pulse->td) return(temp_pulse->plow);
         index = (time - temp_pulse->td) / temp_pulse->per;
         time = time - temp_pulse->td - index * temp_pulse->per;
         if (time <= temp_pulse->tr)
           temp_val = temp_pulse->plow +
                      (temp_pulse->phigh - temp_pulse->plow) * 
                      time / temp_pulse->tr;
         else if (time <= temp_pulse->tr + temp_pulse->pw)
           temp_val = temp_pulse->phigh;
         else if (time <= temp_pulse->tr + temp_pulse->pw +
                          temp_pulse->tf)
           temp_val = temp_pulse->plow + 
                      (temp_pulse->phigh - temp_pulse->plow) *
                      (temp_pulse->tr + temp_pulse->pw +
                       temp_pulse->tf - time) / temp_pulse->tf;
         else temp_val = temp_pulse->plow;

         return(temp_val);

    case PWL :

         temp_pwl = (f_pwl *) fparm;

         index = 0;
         while (*(temp_pwl->timedata + index + index) < time)
         {
           index++;
           if (index == temp_pwl->size) break;
         }

         if (index == 0) return(0.0);

         index2 = index + index;

         if (index == temp_pwl->size)
           temp_val = *(temp_pwl->timedata + temp_pwl->size +
                        temp_pwl->size - 1);
         else
         { 
           temp_val = *(temp_pwl->timedata + index2 - 1) +
                      (time - *(temp_pwl->timedata + index2 - 2))*
                      (*(temp_pwl->timedata + index2 + 1) -
                       *(temp_pwl->timedata + index2 - 1))/
                      (*(temp_pwl->timedata + index2) -
                       *(temp_pwl->timedata + index2 - 2)); 

         }

         return(temp_val);
       
  }  /* switch */

  return (0.0); /* shouldn't get here */
}    /* func_eval */


void
func_breakpoint(int ftype, char *fparm, double max_step, double ftime)
{
  f_sin *temp_sin;
  f_pulse *temp_pulse;
  f_pwl *temp_pwl;
  double temp_val, temp_time;
  int index, index2;


  switch (ftype)
  {
    case SIN : 

         temp_sin = (f_sin *) fparm;
 
         temp_val = 0.25 / temp_sin->freq;
         if (temp_val < max_step) 
           add_breakpoint(temp_sin->td, ftime, temp_val);

         return;

    case PULSE : 

         temp_pulse = (f_pulse *) fparm;
 
         index = 0;

         while ((temp_time = index*temp_pulse->per + temp_pulse->td) <= 
                 ftime)
         {
           temp_val = 0.25*temp_pulse->tr;
           if (temp_val < max_step)
           {
             if (temp_time + temp_pulse->tr <= ftime)
               add_breakpoint(temp_time, temp_time + temp_pulse->tr, 
                            temp_val);
             else 
               add_breakpoint(temp_time, ftime, temp_val);
           }

           temp_time = temp_time + temp_pulse->tr;
           temp_val = 0.25*temp_pulse->pw;
           if (temp_val < max_step)
           {
             if (temp_time + temp_pulse->pw <= ftime)
               add_breakpoint(temp_time, temp_time + temp_pulse->pw, 
                            temp_val);
             else 
               add_breakpoint(temp_time, ftime, temp_val);
           }

           temp_time = temp_time + temp_pulse->pw;
           temp_val = 0.25*temp_pulse->tf;
           if (temp_val < max_step)
           {
             if (temp_time + temp_pulse->tf <= ftime)
               add_breakpoint(temp_time, temp_time + temp_pulse->tf, 
                            temp_val);
             else 
               add_breakpoint(temp_time, ftime, temp_val);
           }

           temp_time = temp_time + temp_pulse->tf;
           temp_val = temp_pulse->per - temp_pulse->tr -
                            temp_pulse->pw - temp_pulse->tf;
           if (0.25*temp_val < max_step)
           {
             if (temp_time + temp_val <= ftime)
               add_breakpoint(temp_time, temp_time + temp_val, 
                            0.25*temp_val);
             else 
               add_breakpoint(temp_time, ftime, 0.25*temp_val);
           }

           index++;

         }  /* while */

         return;

    case PWL :

         temp_pwl = (f_pwl *) fparm;

         index = 0;
         while ((index + 1 < temp_pwl->size) &&
                (temp_pwl->timedata[index + index] < ftime))
         {
           index2 = index + index;
           temp_val = 0.25*(temp_pwl->timedata[index2 + 2] - 
                            temp_pwl->timedata[index2]);
           if (temp_val < max_step)
           {
             if (temp_pwl->timedata[index2 + 2] <= ftime)
               add_breakpoint(temp_pwl->timedata[index2], 
                            temp_pwl->timedata[index2 + 2],
                            temp_val);
             else
               add_breakpoint(temp_pwl->timedata[index2], ftime,
                            temp_val);
           }
           index++;

         }  /* while */

         return;
       
  }  /* switch */ 

}    /* func_breakpoint */


void
func_infoprint(FILE *fp, int ftype, char *fparm)
{
  int i;
  f_sin *temp_sin;
  f_pulse *temp_pulse;
  f_pwl *temp_pwl;
  
  switch(ftype)
  {
    case SIN :
               temp_sin = (f_sin *) fparm;
               fprintf(fp, "SIN");
               fprintf(fp, " off %.2e amp %.2e",
                           temp_sin->off, temp_sin->amp);
               fprintf(fp, " freq %.2e td %.2e",
                           temp_sin->freq, temp_sin->td);
               fprintf(fp, " theta %.2e",
                           temp_sin->theta);
                   
               break;

    case PULSE :
                 temp_pulse = (f_pulse *) fparm;
                 fprintf(fp, "PULSE");
                 fprintf(fp, " plow %.2e phigh %.2e",
                           temp_pulse->plow, temp_pulse->phigh);
                 fprintf(fp, " td %.2e tr %.2e",
                           temp_pulse->td, temp_pulse->tr);
                 fprintf(fp, " tf %.2e", temp_pulse->tf);
                 fprintf(fp, " pw %.2e per %.2e",
                           temp_pulse->pw, temp_pulse->per);
                   
                 break;

    case PWL :
               temp_pwl = (f_pwl *) fparm;
  
               fprintf(fp, "PWL"); 

               for (i = 0; i < temp_pwl->size; i++)
                 fprintf(fp, " time %.2e val %.2e", 
                             *(temp_pwl->timedata + 2*i),
                             *(temp_pwl->timedata + 2*i + 1));
    
               break;

  }   /* switch */

}  /* func_infoprint */


void
new_function(int ftype, char **f1, char *f2)
{
  int i;
  f_sin *temp_sin;
  f_pulse *temp_pulse;
  f_pwl *temp_pwl;

  switch(ftype)
  {
    case SIN : 

          *f1 = mycalloc(1, sizeof(f_sin));
          temp_sin = (f_sin *) (*f1);
          temp_sin->off = ((f_sin *) f2)->off;
          temp_sin->amp = ((f_sin *) f2)->amp;
          temp_sin->freq = ((f_sin *) f2)->freq;
          temp_sin->td = ((f_sin *) f2)->td;
          temp_sin->theta = ((f_sin *) f2)->theta;
          return;
   
    case PULSE : 
            *f1 = mycalloc(1, sizeof(f_pulse));
            temp_pulse = (f_pulse *) (*f1);
            temp_pulse->plow = ((f_pulse *) f2)->plow;
            temp_pulse->phigh = ((f_pulse *) f2)->phigh;
            temp_pulse->td = ((f_pulse *) f2)->td;
            temp_pulse->tr = ((f_pulse *) f2)->tr;
            temp_pulse->tf = ((f_pulse *) f2)->tf;
            temp_pulse->pw = ((f_pulse *) f2)->pw;
            temp_pulse->per = ((f_pulse *) f2)->per;
            return;
 
    case PWL : 
          *f1 = mycalloc(1, sizeof(f_pwl));
          temp_pwl = (f_pwl *) (*f1);
          temp_pwl->size = ((f_pwl *) f2)->size;
          temp_pwl->timedata = (double *)
                               mycalloc(2*temp_pwl->size,
                                        sizeof(double));
          for (i = 0; i < temp_pwl->size + temp_pwl->size; i++)
             temp_pwl->timedata[i] = ((f_pwl *) f2)->timedata[i];

          return;

  }  /* switch */

}  /* new_function */
