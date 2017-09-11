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
read_ind()
{
  long name;
  int bad_dev = FALSE;

  intptr[0] = FALSE;
  dataptr[1] = 0.0;
  dataptr[2] = 1.0;

  read_error = read_long(nodeptr, "plus node", FALSE);
  read_error = read_long((nodeptr+1), "minus node", FALSE); 
  read_error = read_double(dataptr, "inductance", FALSE);

  if (read_error != OK) 
  {
    printf("## Error -- bad inductor definition\n");
    no_go = TRUE;
    bad_dev = TRUE;
  }

  while ((read_error = read_string("")) == OK)
  switch (get_string_keyword(tempstring))
  {
    case IC :
      read_error = read_double(dataptr+1, "initial condition", FALSE);
      break;

    case FCHECK : intptr[0] = TRUE;
                  break;

    case FVALUE : 
     read_error = read_double(dataptr+2, "proportion of phi 0", FALSE); 
     break;
      
    default :
      printf("## Warning -- illegal keyword %s\n", tempstring);
      break;
  }

  if (bad_dev == FALSE)
  {
    if (current_sub_def == NULL)
    {
      name = add_realname(dev_name);
      ind_to_devlist(name, INDUCT, nodeptr, intptr, dataptr);
    }
    else 
     ind_to_deftree(dev_name, INDUCT, nodeptr, intptr, dataptr);
  }

}   /* read_ind */


device *
ind_to_devlist(long name, int type, long *nodeptr, long *intptr,
    double *dataptr)
{
  int n_plus, n_minus, branch; 
  branch_marker *bmarker;
  flux_check_list *temp_fcheck;
  device *temp_dev;

  switch (type)
  {
    case INDUCT : 

         bmarker = add_bmarker();
         n_plus = add_node(*nodeptr, name, type, PLUS_NODE, bmarker);
         n_minus = add_node(*(nodeptr+1), name, type, MINUS_NODE, 
                            bmarker);
         eqn_count++;
         branch = eqn_count;
         temp_dev = add_ind(name, dataptr[0], dataptr[1], n_plus, 
                            n_minus, branch, intptr[0], dataptr[2]);

         if (*intptr == TRUE)
         {
           if (fcheck_list == NULL)
           {
             temp_fcheck = (flux_check_list *) 
                          mycalloc(1, sizeof(flux_check_list)); 
             temp_fcheck->flux_check = temp_dev;
             temp_fcheck->next_flux_check = NULL;
             fcheck_list = fcheck_tail = temp_fcheck;
           }
           else
           {
             temp_fcheck = (flux_check_list *) 
                          mycalloc(1, sizeof(flux_check_list)); 
             temp_fcheck->flux_check = temp_dev;
             temp_fcheck->next_flux_check = NULL;
             fcheck_tail->next_flux_check = temp_fcheck;
             fcheck_tail = temp_fcheck;
           }
         }

         return(temp_dev);

  }  /* switch */

  return(NULL);

}   /* ind_to_devlist */   


device *
ind_to_deftree(char *dev_name, int type, long *nodeptr, long *intptr,
    double *dataptr)
{
  int branch;
  long name;

  name = (long) dev_name;

  switch (type)
  {
    case INDUCT : 
         return(add_ind(name, dataptr[0], dataptr[1], *nodeptr, 
                        *(nodeptr+1), branch, intptr[0], dataptr[2]));

  }

  return(NULL);

}   /* ind_to_deftree */   


device *
indsub_to_devlist(char *dev_name, int type, char *data)
{
  int n_plus, n_minus, branch; 
  int i, temp_int;
  double temp_data[7];
  long name;
  char *primary_name, *second_name, *con_dev;
  branch_marker *bmarker;

  name = add_realname(combine_string(namestring, dev_name));

  switch (type)
  {
    case INDUCT : 

         bmarker = add_bmarker(); 
         n_plus = getnode_subnode(((sub_ind *) data)->n_plus,
                                  ((sub_ind *) data)->p_node);
         n_plus = add_node(n_plus, name, type, PLUS_NODE, bmarker);
         n_minus = getnode_subnode(((sub_ind *) data)->n_minus,
                                   ((sub_ind *) data)->m_node);
         n_minus = add_node(n_minus, name, type, MINUS_NODE,
                            bmarker);
         eqn_count++;
         branch = eqn_count;
         return(add_ind(name, ((sub_ind *) data)->val, 
                        ((sub_ind *) data)->ic, n_plus, n_minus, 
                        branch, ((sub_ind *) data)->fcheck,
                        ((sub_ind *) data)->fvalue));

  }  /* switch */

  return(NULL);

}   /* indsub_to_devlist */   


device *
add_ind(long name, double val, double ic, long n_plus, long n_minus,
    long branch, int fcheck, double fvalue)
{
  char *data;
  dev_ind *temp;
  sub_ind *temp_sub;
   
  if (current_sub_def == NULL)
  {
    temp = (dev_ind *) mycalloc(1, sizeof(dev_ind));
  
    temp->val = val;
    temp->ic = ic;
    temp->n_plus = n_plus;
    temp->n_minus = n_minus;
    temp->branch = branch;
    temp->fcheck = fcheck;
    temp->fvalue = fvalue;
  
    data = (char *) temp;  
    return(add_dev(name, INDUCT, data));
  }
  else
  {
    temp_sub = (sub_ind *) mycalloc(1, sizeof(sub_ind));
  
    temp_sub->val = val;
    temp_sub->ic = ic;
    temp_sub->n_plus = n_plus;
    temp_sub->n_minus = n_minus;
    temp_sub->p_node = scan_node(n_plus);
    temp_sub->m_node = scan_node(n_minus);
    temp_sub->fcheck = fcheck;
    temp_sub->fvalue = fvalue;
  
    data = (char *) temp_sub;  
    return(add_sub((char *) name, INDUCT, data));
  }

}  /* add_ind */


void
ind_matrix()
{
  dev_ind *temp;
  m_data  *temp_m;
  device *temp_dev;
  int exist;

  temp_dev = ind;
  
  while (temp_dev != NULL)
  {
    temp = (dev_ind *) temp_dev->data;

    temp->mptr_ii = add_i_j(A_matrix, 
                            temp->branch, temp->branch, &exist);
    temp->mptr_ii->type = ORIGINAL;

    if (temp->n_plus != GROUND)
    {
      temp_m = add_i_j(A_matrix, 
                     temp->n_plus, temp->branch, &exist);
      temp_m->val = 1.0;
      temp_m->type = CONST_ORIGINAL;

      temp_m = add_i_j(A_matrix, 
                     temp->branch, temp->n_plus, &exist);
      temp_m->val = 1.0;
      temp_m->type = CONST_ORIGINAL;

    }

    if (temp->n_minus != GROUND)
    {
      temp_m = add_i_j(A_matrix, 
                     temp->n_minus, temp->branch, &exist);
      temp_m->val = temp_m->val - 1.0;
      temp_m->type = CONST_ORIGINAL;

      temp_m = add_i_j(A_matrix, 
                     temp->branch, temp->n_minus, &exist);
      temp_m->val = temp_m->val - 1.0;
      temp_m->type = CONST_ORIGINAL;

    }

    x_unknown[temp->branch]->type = AMP_TIME;

    add_source(b_source, temp->branch);

    if (temp->n_plus == GROUND) temp->n_plus = ground_node;
    if (temp->n_minus == GROUND) temp->n_minus = ground_node;
    temp->fvalue = temp->fvalue*max_flux_step;

    temp_dev = temp_dev->next_dev;
  }

}  /* ind_matrix */


void
ind_trap(int source_only, double *hptr)
{
  dev_ind *temp;
  device *temp_dev;
  int exist;
  double xn1, xn1d, two_l_hn;

  temp_dev = ind;
  
  while (temp_dev != NULL)
  {
    temp = (dev_ind *) temp_dev->data;

    two_l_hn = (temp->val + temp->val)/(*hptr); 

    xn1 = *(x_unk_copy[temp->branch]->xpast);
    xn1d = x_unk_copy[temp->branch]->xdpast;

    if (source_only == FALSE)
      temp->mptr_ii->val = temp->mptr_ii->val - two_l_hn;

    b_src_copy[temp->branch]->val = 
                                  b_src_copy[temp->branch]->val 
                                  - two_l_hn*xn1 - temp->val*xn1d;
/*
    b_source[temp->branch]->val = b_source[temp->branch]->val 
                                  - two_l_hn*xn1 - temp->val*xn1d;
*/
      
    temp_dev = temp_dev->next_dev;
  }

}  /* ind_trap */ 


double
ind_step_limit(double current_step)
{
  flux_check_list *temp_fcheck;
  device *temp_dev;
  dev_ind *temp_ind;
  double smallest;
  double h;
  double delta_flux;

  smallest = 1.0e30;

  temp_fcheck = fcheck_list;

  while (temp_fcheck != NULL)
  {
    temp_ind = (dev_ind *) temp_fcheck->flux_check->data;
    
    delta_flux = myabs(x_unk_copy[temp_ind->branch]->xn -
                       *(x_unk_copy[temp_ind->branch]->xpast));

    delta_flux = delta_flux * temp_ind->val;

    if ((delta_flux > temp_ind->fvalue) || (delta_flux > 0.0))
      h = current_step*(temp_ind->fvalue/delta_flux);  
    else h = smallest;

    if (h < smallest) smallest = h;

    temp_fcheck = temp_fcheck->next_flux_check;

  }

  return(smallest);

}   /* ind_step_limit */


int
ind_apriori_step_limit(double step, double *newstep)
{
  dev_ind *temp_ind;
  device *temp_dev;
  flux_check_list *temp_flux;
  double temp_h;
  double xn1, yn1;
  double h, smallest;
  double delta_flux;
  double large_delta_flux;


  temp_flux = fcheck_list;
  smallest = step;

  while (temp_flux != NULL)
  {
    temp_ind = (dev_ind *) temp_flux->flux_check->data;

    delta_flux = (x_unk_copy[temp_ind->branch]->xdpast) * step;

    delta_flux = myabs(delta_flux * temp_ind->val);
   
    if (delta_flux > temp_ind->fvalue)
    {
      h = step * (temp_ind->fvalue/delta_flux);
      if (h < smallest) smallest = h;
    }
    
    temp_flux = temp_flux->next_flux_check;
  }

  if (smallest >= step)
  {
    *newstep = step;
    return(OK);
  }
  
  if (smallest + smallest >= step)
  {
    *newstep = smallest;
    return(TOO_BIG);
  }
  
  *newstep = 0.5*step;
  return(TOO_BIG);

}  /* ind_apriori_step_limit */


void
ind_tran_print(FILE *fp, double inc_rate, int prtype, device *temp_dev)
{
  long plus, minus, branch; 
  double plus_val, minus_val, branch_val;

  plus = ((dev_ind *) temp_dev->data)->n_plus;
  minus = ((dev_ind *) temp_dev->data)->n_minus;
  branch = ((dev_ind *) temp_dev->data)->branch;
  
  if (prtype == VOLT)
  {
    plus_val = get_pr_val(inc_rate, plus, 0);
    minus_val = get_pr_val(inc_rate, minus, 0);
    fprintf(fp, out_format, plus_val - minus_val);
    return;
  }

  if (prtype == AMP)
  {
    branch_val = get_pr_val(inc_rate, branch, 0);
    fprintf(fp, out_format, branch_val);
    return;

  }

}  /* ind_tran_print */

