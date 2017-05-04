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


void
add_frame_point(frame *myframe, double time, double value)
{
  double *temp_double;
  double delta_time;
  double last_val;
  int i, step_no;

  if (myframe->current_size == 0)
  {
    myframe->time_data[0] = time;
    myframe->value_data[0] = value;
    myframe->current_size = 1;
    return;
  }    

  delta_time = time - myframe->time_data[myframe->current_size-1];
  last_val = myframe->value_data[myframe->current_size-1];

  if ((delta_time < myframe->nominal_step) &&
      (myabs(value - last_val) < 
       myframe->val_min + 
       myframe->val_rel * mymin(myabs(last_val), myabs(value))))
    return; 

  if (delta_time >= myframe->interval) 
    step_no = myframe->current_size;
  else
  {
    step_no = 0;
    while (time - myframe->time_data[step_no] >= myframe->interval)
      step_no++;

  }
  step_no--;
  
  if (step_no > 0)
  {
    for (i = step_no; i < myframe->current_size; i++)
    {
      myframe->time_data[i - step_no] = myframe->time_data[i];
      myframe->value_data[i - step_no] = myframe->value_data[i];
    }
    myframe->current_size = myframe->current_size - step_no;
  }

  if (myframe->current_size == myframe->size)
  {
    temp_double = (double *) 
                  mycalloc(myframe->size + myframe->unit_size,
                           sizeof(double));

    for (i = 0; i < myframe->size; i++)
      temp_double[i] = myframe->time_data[i];

    myframe->time_data = temp_double;

    temp_double = (double *) 
                  mycalloc(myframe->size + myframe->unit_size,
                           sizeof(double));

    for (i = 0; i < myframe->size; i++)
      temp_double[i] = myframe->value_data[i];

    myframe->value_data = temp_double;

    myframe->size = myframe->size + myframe->unit_size;
  }

  myframe->time_data[myframe->current_size] = time;
  myframe->value_data[myframe->current_size] = value;
  myframe->current_size++;

}  /* add_frame_point */


double
delay_frame_point(frame *myframe, double time, int *index)
{

  if (myframe->current_size == 0) 
  {
    *index = -1;
    return(0.0);
  }

  time = time - myframe->interval;

  if (time < myframe->time_data[0]) 
  {
    *index = -1;
    return(0.0);
  }

  if (time > myframe->time_data[myframe->current_size - 1])
  {
    *index = -1;
    return(0.0);
  }

  if (myframe->current_size == 1)
  {
    *index = 0;
    return(myframe->value_data[myframe->current_size - 1]);
  }

  *index = 1;
  while (myframe->time_data[*index] < time) (*index)++;
  
  return(myframe->value_data[*index - 1] +
         (myframe->value_data[*index] -
          myframe->value_data[*index - 1]) *
         (time - myframe->time_data[*index - 1]) /
         (myframe->time_data[*index] -
          myframe->time_data[*index - 1]));
  

} /* delay_frame_point */


