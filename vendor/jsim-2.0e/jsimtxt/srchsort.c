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

long *temp_array;


void
merge_copy(long *the_array, long lower, long upper)
{
  long i;

  for (i = lower; i <= upper; i++)
     *(temp_array + i - lower) = *(the_array + i);

}   /* merge_copy */


void
sort_realname(long lower, long upper)
{
  long mid;

  if (lower == upper) return;
  mid = (lower + upper) / 2;
  sort_realname(lower, mid);
  sort_realname(mid+1, upper);
  merge_realname(lower, mid, upper); 
  return;

}  /* sort_realname */


void
merge_realname(long lower, long mid, long upper)
{
  long i, j, k;
  int temp;

  merge_copy(realname_array, lower, mid);  
  i = 0;
  j = mid + 1;
  k = lower;
  while ((i <= mid - lower) && (j <= upper))
  {
    temp = strcmp(((realname_name *) (*(temp_array + i)))->real_name,
               ((realname_name *) (*(realname_array + j)))->real_name); 
    if (temp == 0)
    {
      *(realname_array + k) = *(temp_array + i);
      k++; i++;
      *(realname_array + k) = *(realname_array + j);
      k++; j++;
    }
    else if (temp < 0)
    {
      *(realname_array + k) = *(temp_array + i);
      k++; i++;
    }
    else
    {
      *(realname_array + k) = *(realname_array + j);
      k++; j++;
    }
  }   /* while */

  if (i <= mid - lower)
  while (i <= mid - lower)
  {
    *(realname_array + k) = *(temp_array + i);
    k++; i++;
  }

}    /* merge_realname */
    
  
void
get_realname_array()
{
  long i;
  realname_name *temp_name;
  
  if (dev_count <= 0) return;

  realname_array = (long *) mycalloc(dev_count, sizeof(long));
   
  temp_name = namelist;
  i = 0;
  while (temp_name != NULL)
  {
    *(realname_array + i) = (long) temp_name;
    i++;
    temp_name = temp_name->next_name;
  }

  if (dev_count > 1)
  {
    temp_array = (long *) mycalloc(dev_count/2 + 1, sizeof(long));
    sort_realname(0, dev_count - 1);
    free((char *) temp_array);
  }

}  /* get_realname_array */
  

void
get_name_array()
{
  long i;
  realname_name *temp_name;
  
  if (dev_count <= 0) return;

  name_array = (long *) mycalloc(dev_count, sizeof(long));
   
  temp_name = namelist;
  i = 0;
  while (temp_name != NULL)
  {
    *(name_array + i) = (long) temp_name;
    i++;
    temp_name = temp_name->next_name;
  }

}  /* get_name_array */


realname_name *
search_realname(char *real_name)
{
  int lower, mid, upper;
  int temp;

  lower = 0;
  upper = dev_count - 1;
  
  while (lower <= upper)
  {
    mid = (lower + upper)/2;
    temp = strcmp(real_name,
              ((realname_name *) (*(realname_array + mid)))->real_name);
    if (temp == 0)
      return((realname_name *) (*(realname_array + mid)));
    else if (temp < 0) upper = mid - 1;
    else lower = mid + 1;
  }
  return(NULL);

}  /* search_realname */
    
  
realname_name *
search_name(long name)
{

  if ((name > dev_count) || (name <= 0)) return(NULL); 
  return((realname_name *) (*(name_array + name - 1)));

}  /* search_name */


void
sort_dev(long *dev_array, long lower, long upper)
{
  long mid;

  if (lower == upper) return;
  mid = (lower + upper) / 2;
  sort_dev(dev_array, lower, mid);
  sort_dev(dev_array, mid+1, upper);
  merge_dev(dev_array, lower, mid, upper); 
  return;

}  /* sort_dev */


void
merge_dev(long *dev_array, long lower, long mid, long upper)
{
  long i, j, k;
  int temp;

  merge_copy(dev_array, lower, mid);  
  i = 0;
  j = mid + 1;
  k = lower;
  while ((i <= mid - lower) && (j <= upper))
  {
    temp = strcmp((char*)((device *) (*(temp_array + i)))->name,
               (char*)((device *) (*(dev_array + j)))->name); 
    if (temp == 0)
    {
      *(dev_array + k) = *(temp_array + i);
      k++; i++;
      *(dev_array + k) = *(dev_array + j);
      k++; j++;
    }
    else if (temp < 0)
    {
      *(dev_array + k) = *(temp_array + i);
      k++; i++;
    }
    else
    {
      *(dev_array + k) = *(dev_array + j);
      k++; j++;
    }
  }   /* while */

  if (i <= mid - lower)
  while (i <= mid - lower)
  {
    *(dev_array + k) = *(temp_array + i);
    k++; i++;
  }

}    /* merge_dev */
    
  
long *
get_dev_array(int no_sort, device *dev_list, long dev_count)
{
  long i, *temp_dev_array;
  device *temp_dev;
  int tempint;
  
  if (dev_count <= 0) return(NULL);

  temp_dev_array = (long *) mycalloc(dev_count, sizeof(long));
   
  temp_dev = dev_list;
  i = 0;
  while (temp_dev != NULL)
  {
    *(temp_dev_array + i) = (long) temp_dev;
    i++;
    temp_dev = temp_dev->next_dev;
  }

  if ((no_sort == FALSE) && (dev_count > 1))
  {
    temp_array = (long *) mycalloc(dev_count/2 + 1, sizeof(long));
    sort_dev(temp_dev_array, 0, dev_count - 1);
    free((char *) temp_array);
  }

  return(temp_dev_array);

}  /* get_dev_array */


device *
search_dev(long *dev_array, long dev_count, char *name)
{
  long lower, mid, upper;
  int temp;

  lower = 0;
  upper = dev_count - 1;
  
  while (lower <= upper)
  {
    mid = (lower + upper)/2;
    temp = strcmp(name,
              (char *) ((device *) (*(dev_array + mid)))->name);
    if (temp == 0)
      return((device *) (*(dev_array + mid)));
    else if (temp < 0) upper = mid - 1;
    else lower = mid + 1;
  }
  return(NULL);

}  /* search_dev */


device *
search_name_dev(long *dev_array, long dev_count, long name)
{
  long lower, mid, upper;
  long temp;

  lower = 0;
  upper = dev_count - 1;
  
  while (lower <= upper)
  {
    mid = (lower + upper)/2;
    temp = name - ((device *) (*(dev_array + mid)))->name;
    if (temp == 0)
      return((device *) (*(dev_array + mid)));
    else if (temp < 0) upper = mid - 1;
    else lower = mid + 1;
  }
  return(NULL);

}  /* search_name_dev */


void
sort_sub_def(long *sub_def_array, long lower, long upper)
{
  long mid;

  if (lower == upper) return;
  mid = (lower + upper) / 2;
  sort_sub_def(sub_def_array, lower, mid);
  sort_sub_def(sub_def_array, mid+1, upper);
  merge_sub_def(sub_def_array, lower, mid, upper); 
  return;

}  /* sort_sub_def */


void
merge_sub_def(long *sub_def_array, long lower, long mid, long upper)
{
  long i, j, k;
  int temp;

  merge_copy(sub_def_array, lower, mid);  
  i = 0;
  j = mid + 1;
  k = lower;
  while ((i <= mid - lower) && (j <= upper))
  {
    temp = strcmp(((sub_def *) (*(temp_array + i)))->name,
               ((sub_def *) (*(sub_def_array + j)))->name); 
    if (temp == 0)
    {
      *(sub_def_array + k) = *(temp_array + i);
      k++; i++;
      *(sub_def_array + k) = *(sub_def_array + j);
      k++; j++;
    }
    else if (temp < 0)
    {
      *(sub_def_array + k) = *(temp_array + i);
      k++; i++;
    }
    else
    {
      *(sub_def_array + k) = *(sub_def_array + j);
      k++; j++;
    }
  }   /* while */

  if (i <= mid - lower)
  while (i <= mid - lower)
  {
    *(sub_def_array + k) = *(temp_array + i);
    k++; i++;
  }

}    /* merge_sub_def */
    
  
long *
get_sub_def_array(sub_def *sub_def_list, long sub_def_count)
{
  long i, *temp_sub_def_array, *get_dev_array();
  sub_def *temp_sub_def;
  int tempint;
  
  if (sub_def_count <= 0) return(NULL);

  temp_sub_def_array = (long *) mycalloc(sub_def_count, sizeof(long));
   
  temp_sub_def = sub_def_list;
  i = 0;
  while (temp_sub_def != NULL)
  {
    *(temp_sub_def_array + i) = (long) temp_sub_def;
    i++;
    temp_sub_def = temp_sub_def->next_sub;
  }

  if (sub_def_count > 1)
  {
    temp_array = (long *) mycalloc(sub_def_count/2 + 1, sizeof(long));
    sort_sub_def(temp_sub_def_array, 0, sub_def_count - 1);
    free((char *) temp_array);
  }

  temp_sub_def = sub_def_list;
  while (temp_sub_def != NULL)
  {
    temp_sub_def->sub_dev_array = 
      get_dev_array(FALSE, temp_sub_def->the_sub_ckt,
                        temp_sub_def->sub_dev_count);
    temp_sub_def = temp_sub_def->next_sub;
  }

  temp_sub_def = sub_def_list;
  while (temp_sub_def != NULL)
  {
    temp_sub_def->sub_sub_array = 
      get_sub_def_array(temp_sub_def->sub_sub,
                        temp_sub_def->sub_sub_count);
    temp_sub_def = temp_sub_def->next_sub;
  }

  return(temp_sub_def_array);

}  /* get_sub_def_array */
  

sub_def *
search_sub_def(long *sub_def_array, long sub_def_count, char *name)
{
  int lower, mid, upper;
  int temp;
  int tempint;

  lower = 0;
  upper = sub_def_count - 1;
  
  while (lower <= upper)
  {
    mid = (lower + upper)/2;
    temp = strcmp(name,
              (char *) ((sub_def *) (*(sub_def_array + mid)))->name);
    if (temp == 0)
      return((sub_def *) (*(sub_def_array + mid)));
    else if (temp < 0) upper = mid - 1;
    else lower = mid + 1;
  }
  return(NULL);

}  /* search_sub_def */


void
get_eqn_array()
{
  int i;
  node_to_eqn *temp_node_eqn;

  if (eqn_count < 0) return;

  eqn_array = (long *) mycalloc(eqn_count + 1, sizeof(long));
  
  for (i = 0; i < RANGE; i++)
  {
    temp_node_eqn = node_map[i];

    while (temp_node_eqn != NULL)
    {
      *(eqn_array + temp_node_eqn->n_eq) = (long) temp_node_eqn;
      temp_node_eqn = temp_node_eqn->next_node;
    }
  }

}   /* get_eqn_array */


void
sort_node(long lower, long upper)
{
  long mid;

  if (lower == upper) return;
  mid = (lower + upper) / 2;
  sort_node(lower, mid);
  sort_node(mid+1, upper);
  merge_node(lower, mid, upper); 
  return;

}  /* sort_node */


void
merge_node(long lower, long mid, long upper)
{
  long i, j, k;
  int temp;

  merge_copy(node_array, lower, mid);  
  i = 0;
  j = mid + 1;
  k = lower;
  while ((i <= mid - lower) && (j <= upper))
  {
    temp = ((node_to_eqn *) (*(temp_array + i)))->node -
           ((node_to_eqn *) (*(node_array + j)))->node;
    if (temp == 0)
    {
      *(node_array + k) = *(temp_array + i);
      k++; i++;
      *(node_array + k) = *(node_array + j);
      k++; j++;
    }
    else if (temp < 0)
    {
      *(node_array + k) = *(temp_array + i);
      k++; i++;
    }
    else
    {
      *(node_array + k) = *(node_array + j);
      k++; j++;
    }
  }   /* while */

  if (i <= mid - lower)
  while (i <= mid - lower)
  {
    *(node_array + k) = *(temp_array + i);
    k++; i++;
  }

}    /* merge_node */
    
  
void
get_node_array()
{
  long i;
  int j;
  node_to_eqn *temp_node_eqn;
  
  if (node_count <= 0) return;

  node_array = (long *) mycalloc(node_count, sizeof(long));
   
  i = 0;
  for (j = 0; j < RANGE; j++)
  {
    temp_node_eqn = node_map[j];
    while (temp_node_eqn != NULL)
    {
      *(node_array + i) = (long) temp_node_eqn;
      i++;
      temp_node_eqn = temp_node_eqn->next_node;
    }
  }

  if (node_count > 1)
  {
    temp_array = (long *) mycalloc(node_count/2 + 1, sizeof(long));
    sort_node(0, node_count - 1);
    free((char *) temp_array);
  }

}  /* get_node_array */
  

node_to_eqn *
search_node(int node)
{
  long lower, mid, upper;
  int temp;

  lower = 0;
  upper = node_count - 1;
  
  while (lower <= upper)
  {
    mid = (lower + upper)/2;
    temp = node -
              ((node_to_eqn *) (*(node_array + mid)))->node;
    if (temp == 0)
      return((node_to_eqn *) (*(node_array + mid)));
    else if (temp < 0) upper = mid - 1;
    else lower = mid + 1;
  }
  return(NULL);

}  /* search_node */


void
sort_model(long lower, long upper)
{
  long mid;

  if (lower == upper) return;
  mid = (lower + upper) / 2;
  sort_model(lower, mid);
  sort_model(mid+1, upper);
  merge_model(lower, mid, upper); 
  return;

}  /* sort_model */


void
merge_model(long lower, long mid, long upper)
{
  long i, j, k;
  int temp;

  merge_copy(jsim_model_array, lower, mid);  
  i = 0;
  j = mid + 1;
  k = lower;
  while ((i <= mid - lower) && (j <= upper))
  {
    temp = strcmp(((modeldata *) (*(temp_array + i)))->name,
           ((modeldata *) (*(jsim_model_array + j)))->name);
    if (temp == 0)
    {
      *(jsim_model_array + k) = *(temp_array + i);
      k++; i++;
      *(jsim_model_array + k) = *(jsim_model_array + j);
      k++; j++;
    }
    else if (temp < 0)
    {
      *(jsim_model_array + k) = *(temp_array + i);
      k++; i++;
    }
    else
    {
      *(jsim_model_array + k) = *(jsim_model_array + j);
      k++; j++;
    }
  }   /* while */

  if (i <= mid - lower)
  while (i <= mid - lower)
  {
    *(jsim_model_array + k) = *(temp_array + i);
    k++; i++;
  }

}    /* merge_model */
    
  
void
get_model_array()
{
  int i;
  modeldata *temp_model;
  
  if (mod_count <= 0) return;

  jsim_model_array = (long *) mycalloc(mod_count, sizeof(long));
   
  i = 0;
  temp_model = jsim_model;
  while (temp_model != NULL)
  {
    *(jsim_model_array + i) = (long) temp_model;
    i++;
    temp_model = temp_model->next_model;
  }

  if (mod_count > 1)
  {
    temp_array = (long *) mycalloc(mod_count/2 + 1, sizeof(long));
    sort_model(0, mod_count - 1);
    free((char *) temp_array);
  }

}  /* get_model_array */
  

modeldata *
search_model(char *name)
{
  int lower, mid, upper;
  int temp;

  lower = 0;
  upper = mod_count - 1;
  
  while (lower <= upper)
  {
    mid = (lower + upper)/2;
    temp = strcmp(name,
              ((modeldata *) (*(jsim_model_array + mid)))->name);
    if (temp == 0)
      return((modeldata *) (*(jsim_model_array + mid)));
    else if (temp < 0) upper = mid - 1;
    else lower = mid + 1;
  }
  return(NULL);

}  /* search_model */
