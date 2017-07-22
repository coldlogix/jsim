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
free_devarray()
{
  if (resis_array != NULL)
  {
    free((char *) resis_array);
    resis_array = NULL;
  }

  if (cap_array != NULL)
  {
    free((char *) cap_array);
    cap_array = NULL;
  }

  if (ind_array != NULL)
  {
    free((char *) ind_array);
    ind_array = NULL;
  }

  if (jj_array != NULL)
  {
    free((char *) jj_array);
    jj_array = NULL;
  }

  if (mut_array != NULL)
  {
    free((char *) mut_array);
    mut_array = NULL;
  }

  if (sub_ckt_array != NULL)
  {
    free((char *) sub_ckt_array);
    sub_ckt_array = NULL;
  }

  if (vsource_array != NULL)
  {
    free((char *) vsource_array);
    vsource_array = NULL;
  }

  if (isource_array != NULL)
  {
    free((char *) isource_array);
    isource_array = NULL;
  }

}  /* free_devarray */


void
free_name()
{
  long i;

  for (i = 0; i < dev_count; i++)
  {
    free((char *) realname_array[i]);
    realname_array[i] = (long)NULL;
  }

  if (realname_array != NULL)
  {
    free((char *) realname_array);
    realname_array = NULL;
  }

  if (name_array != NULL)
  {
    free((char *) name_array);
    name_array = NULL;
  }

}  /* free_name */


void
free_def_tree()
{
  int i;

  for (i=0; i < sub_def_count; i++)
  {
    free_sub_def((sub_def *) sub_def_array[i]);
    free((char *) sub_def_array[i]);
    sub_def_array[i] = (long)NULL;
  }

  if (sub_def_array != NULL)
  {
    free((char *) sub_def_array);
    sub_def_array = NULL;
  }

}  /* free_def_tree */


void
free_sub_def(sub_def*the_sub)
{
  int i;
  device *temp_dev;

  free((char *) the_sub->node_list);
  the_sub->node_list = NULL;
  
  for (i = 0; i < the_sub->sub_dev_count; i++)
  {
    temp_dev = (device *) the_sub->sub_dev_array[i];
    free(temp_dev->data);
    temp_dev->data = NULL;
    free((char *) the_sub->sub_dev_array[i]);
    the_sub->sub_dev_array[i] = (long)NULL;
  }
  
  if (the_sub->sub_dev_array != NULL)
  {
    free((char *) the_sub->sub_dev_array);
    the_sub->sub_dev_array = NULL;
  }

  for (i = 0; i < the_sub->sub_sub_count; i++)
  {
    free_sub_def((sub_def *) the_sub->sub_sub_array[i]);
    free((char *) the_sub->sub_sub_array[i]);
    the_sub->sub_sub_array[i] = (long)NULL;
  }

  if (the_sub->sub_sub_array != NULL)
  {
    free((char *) the_sub->sub_sub_array);
    the_sub->sub_sub_array = NULL;
  }

}  /* free_sub_def */
