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
read_mut()
{
  long name;
  char *primary_name, *second_name;
  int tempint;
  int bad_dev;

  bad_dev = FALSE;

  read_error = read_string("primary inductor");
  if (read_error == OK) primary_name = new_string(tempstring);
  read_error = read_string("secondary inductor"); 
  if (read_error == OK) second_name = new_string(tempstring);
  read_error = read_double(dataptr, "mutual coupling", FALSE);

  if (read_error != OK)
  {
    printf("## Error -- bad mutual inductance card\n");
    no_go = TRUE;
    bad_dev = TRUE;
  }

  if (bad_dev == FALSE)
  {
    if (current_sub_def == NULL)
    {
      name = add_realname(dev_name);
      mut_to_devlist(name, MUTUAL_L, (long *) primary_name, 
                   (long *) second_name, dataptr);
    }
    else
      mut_to_deftree(dev_name, MUTUAL_L, (long *) primary_name, 
                   (long *) second_name, dataptr);
  }

}   /* read_mut */


device *
mut_to_devlist(long name, int type, long *nodeptr, long *intptr,
    double *dataptr)
{
  switch (type)
  {
    case MUTUAL_L : 

         return(add_mut(name, *dataptr, (char *) nodeptr, 
                        (char *) intptr));
    
  }

  return(NULL);

}   /* mut_to_devlist */   


device *
mut_to_deftree(char *dev_name, int type, long *nodeptr, long *intptr,
    double *dataptr)
{
  long name;

  name = (long) dev_name;

  switch (type)
  {
    case MUTUAL_L : 

         return(add_mut(name, *dataptr, (char *) nodeptr, 
                        (char *) intptr));
    
  }

  return(NULL);

}   /* mut_to_deftree */   


device *
mutsub_to_devlist(char *dev_name, int type, char *data)
{
  long name;
  char *primary_name, *second_name, *con_dev;

  name = add_realname(combine_string(namestring, dev_name));

  switch (type)
  {
    case MUTUAL_L : 

         primary_name = 
                 get_subdev_name(((sub_mut *) data)->primary_name);
         second_name = 
                 get_subdev_name(((sub_mut *) data)->second_name);
         return(add_mut(name, ((sub_mut *) data)->val, 
                        primary_name, second_name));
    
  }  /* switch */

  return(NULL);

}   /* mutsub_to_devlist */   


device *
add_mut(long name, double val, char *primary_name, char *second_name)
{
  char *data;
  dev_mut *temp;
  sub_mut *temp_sub;
  int tempint;
   
  if (current_sub_def == NULL)
  {
    temp = (dev_mut *) mycalloc(1, sizeof(dev_mut));
  
    temp->val = val;
    temp->primary_name = primary_name;
    temp->second_name = second_name;
  
    data = (char *) temp;  
    return(add_dev(name, MUTUAL_L, data));
  }
  else
  {
    temp_sub = (sub_mut *) mycalloc(1, sizeof(sub_mut));
  
    temp_sub->val = val;
    temp_sub->primary_name = primary_name;
    temp_sub->second_name = second_name;
  
    data = (char *) temp_sub;  
    return(add_sub((char *) name, MUTUAL_L, data));
  }

}  /* add_mut */


void
mut_dependent()
{
  device *temp_dev, *prim_dev, *second_dev;
  dev_mut *temp_mut;
  realname_name *temp_name;

  temp_dev = mut;

  while (temp_dev != NULL)
  {
    temp_mut = (dev_mut *) temp_dev->data; 

    prim_dev = find_dev(temp_mut->primary_name, INDUCT);
    second_dev = find_dev(temp_mut->second_name, INDUCT);

    if ((prim_dev == NULL) || (second_dev == NULL))
    {
      temp_name = search_name(temp_dev->name);

      if (prim_dev == NULL) 
      {
        printf("## Error -- %s for %s not defined\n",
               temp_mut->primary_name, temp_name->real_name);
        no_go = TRUE;
      }

      if (second_dev == NULL)
      {
        printf("## Error -- %s for %s not defined\n",
               temp_mut->second_name, temp_name->real_name);
        no_go = TRUE;
      }
    }
    else 
    {

      temp_mut->n_primary = ((dev_ind *) (prim_dev->data))->branch;
      temp_mut->n_second = ((dev_ind *) (second_dev->data))->branch; 
      temp_mut->val = sqrt((((dev_ind *) (prim_dev->data))->val)*
                           (((dev_ind *) (second_dev->data))->val))*
                      temp_mut->val;
    }
    
    temp_dev = temp_dev->next_dev;
  }
    
}     /* mut_dependent */


void
mut_matrix()
{
  dev_mut *temp;
  device *temp_dev;
  int exist;

  temp_dev = mut;
  
  while (temp_dev != NULL)
  {
    temp = (dev_mut *) temp_dev->data;

    temp->mptr_primary = add_i_j(A_matrix, temp->n_primary, 
                                 temp->n_second, &exist);
    temp->mptr_primary->type = ORIGINAL;

    temp->mptr_second = add_i_j(A_matrix, temp->n_second, 
                                temp->n_primary, &exist);
    temp->mptr_second->type = ORIGINAL;

    temp_dev = temp_dev->next_dev;
  }

}  /* mut_matrix */


void
mut_trap(int source_only, double *hptr)
{
  dev_mut *temp;
  device *temp_dev;
  int exist, tempint;
  double xn1, xn1d, yn1, yn1d, two_l_hn;

  temp_dev = mut;
  
  while (temp_dev != NULL)
  {
    temp = (dev_mut *) temp_dev->data;

    two_l_hn = (temp->val + temp->val)/(*hptr); 

    xn1 = *(x_unk_copy[temp->n_primary]->xpast);
    xn1d = x_unk_copy[temp->n_primary]->xdpast;

    yn1 = *(x_unk_copy[temp->n_second]->xpast);
    yn1d = x_unk_copy[temp->n_second]->xdpast;

    if (source_only == FALSE)
    {
      temp->mptr_primary->val = temp->mptr_primary->val - 
                              two_l_hn;

      temp->mptr_second->val = temp->mptr_second->val - 
                             two_l_hn;
    }

    b_src_copy[temp->n_primary]->val = 
                                  b_src_copy[temp->n_primary]->val 
                                  - two_l_hn*yn1 - temp->val*yn1d;

    b_src_copy[temp->n_second]->val = 
                                  b_src_copy[temp->n_second]->val 
                                  - two_l_hn*xn1 - temp->val*xn1d;
/*
    b_source[temp->n_primary]->val = b_source[temp->n_primary]->val 
                                  - two_l_hn*yn1 - temp->val*yn1d;

    b_source[temp->n_second]->val = b_source[temp->n_second]->val 
                                  - two_l_hn*xn1 - temp->val*xn1d;
*/
      
    temp_dev = temp_dev->next_dev;
  }

}  /* mut_trap */ 
