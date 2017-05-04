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
read_cap()
{
  long name;
  int bad_dev = FALSE;

  *(dataptr + 1) = 0.0;

  read_error = read_long(nodeptr, "plus node", FALSE);
  read_error = read_long((nodeptr+1), "minus node", FALSE); 
  read_error = read_double(dataptr, "capacitance", FALSE);

  if (read_error != OK)
  {
    printf("## Error -- bad capacitor definition\n");
    no_go = TRUE;
    bad_dev = TRUE;
  }

  read_error = read_string("");
  if (read_error == OK)
  {
    if (get_string_keyword(tempstring) == IC)
      read_error = read_double(dataptr+1, "initial condition", FALSE);
    else printf("## Warning -- illegal keyword %s\n", tempstring);
  }

  if (bad_dev == FALSE)
  {
    if (current_sub_def == NULL) 
    {
      name = add_realname(dev_name);
      cap_to_devlist(name, CAP, nodeptr, dataptr);
    }
    else
      cap_to_deftree(dev_name, CAP, nodeptr, dataptr);
  }

}   /* read_cap */


device *
cap_to_devlist(long name, int type, long *nodeptr, double *dataptr)
{
  int n_plus, n_minus; 
  branch_marker *bmarker;

  switch (type)
  {
    case CAP : 
         bmarker = add_bmarker();
         n_plus = add_node(*nodeptr, name, type, PLUS_NODE, bmarker);
         n_minus = add_node(*(nodeptr+1), name, type, MINUS_NODE,
                            bmarker);
         return(add_cap(name, *dataptr, *(dataptr+1), n_plus, n_minus));
              
  }

  return(NULL);

}   /* cap_to_devlist */   


device *
cap_to_deftree(char *dev_name, int type, long *nodeptr, double *dataptr)
{
  long name;

  name = (long) dev_name;

  switch (type)
  {
    case CAP : 
         return(add_cap(name, *dataptr, *(dataptr+1),
                        *nodeptr, *(nodeptr+1)));
   
  }

  return(NULL);

}   /* cap_to_deftree */   


device *
capsub_to_devlist(char *dev_name, int type, char *data)
{
  int n_plus, n_minus; 
  long name;
  branch_marker *bmarker;

  name = add_realname(combine_string(namestring, dev_name));

  switch (type)
  {
    case CAP : 
         bmarker = add_bmarker(); 
         n_plus = getnode_subnode(((sub_cap *) data)->n_plus,
                                  ((sub_cap *) data)->p_node);
         n_plus = add_node(n_plus, name, type, PLUS_NODE, bmarker);
         n_minus = getnode_subnode(((sub_cap *) data)->n_minus,
                                   ((sub_cap *) data)->m_node);
         n_minus = add_node(n_minus, name, type, MINUS_NODE,
                            bmarker);
         return(add_cap(name, ((sub_cap *) data)->val, 
                        ((sub_cap *) data)->ic, n_plus, n_minus));

  }  /* switch */

  return(NULL);

}   /* capsub_to_devlist */   


device *
add_cap(long name, double val, double ic, long n_plus, long n_minus)
{
  char *data;
  dev_cap *temp;
  sub_cap *temp_sub;
   
  if (current_sub_def == NULL)
  {
    temp = (dev_cap *) mycalloc(1, sizeof(dev_cap));
  
    temp->val = val;
    temp->ic = ic;
    temp->n_plus = n_plus;
    temp->n_minus = n_minus;
  
    data = (char *) temp;  
    return(add_dev(name, CAP, data));
  }
  else 
  {
    temp_sub = (sub_cap *) mycalloc(1, sizeof(sub_cap));
  
    temp_sub->val = val;
    temp_sub->ic = ic;
    temp_sub->n_plus = n_plus;
    temp_sub->n_minus = n_minus;
    temp_sub->p_node = scan_node(n_plus);
    temp_sub->m_node = scan_node(n_minus);
  
    data = (char *) temp_sub;  
    return(add_sub((char *) name, CAP, data));
  }

}  /* add_cap */


void
cap_matrix()
{
  dev_cap *temp;
  device *temp_dev;
  int exist;

  temp_dev = cap;
  
  while (temp_dev != NULL)
  {
    temp = (dev_cap *) temp_dev->data;

    if (temp->n_plus != GROUND )
    {
      temp->mptr_pp = add_i_j(A_matrix, 
                            temp->n_plus, temp->n_plus, &exist);
      temp->mptr_pp->type = ORIGINAL;

/*
      if (x_unknown[temp->n_plus] == NULL)
      {
        add_unknown(x_unknown, temp->n_plus);
      }
*/
      x_unknown[temp->n_plus]->type = VOLT_TIME;

      add_source(b_source, temp->n_plus);

    }

    if ((temp->n_plus != GROUND) && 
        (temp->n_minus != GROUND))
    {
      temp->mptr_pm = add_i_j(A_matrix, 
                            temp->n_plus, temp->n_minus, &exist);
      temp->mptr_pm->type = ORIGINAL;

      temp->mptr_mp = add_i_j(A_matrix, 
                            temp->n_minus, temp->n_plus, &exist);
      temp->mptr_mp->type = ORIGINAL;
    }

    if (temp->n_minus != GROUND)
    {
      temp->mptr_mm = add_i_j(A_matrix, 
                            temp->n_minus, temp->n_minus, &exist);
      temp->mptr_mm->type = ORIGINAL;

/*
      if (x_unknown[temp->n_minus] == NULL)
      {
        add_unknown(x_unknown, temp->n_minus);
      }
*/
      x_unknown[temp->n_minus]->type = VOLT_TIME;

      add_source(b_source, temp->n_minus);
    }

    if (temp->n_plus == GROUND) temp->n_plus = ground_node;
    if (temp->n_minus == GROUND) temp->n_minus = ground_node;

    temp_dev = temp_dev->next_dev;
  }

}  /* cap_matrix */


void
cap_trap(int source_only, double *hptr)
{
  dev_cap *temp;
  device *temp_dev;
  int exist;
  double xn1, xn1d, yn1, yn1d, two_c_hn, sval;

  temp_dev = cap;
  
  while (temp_dev != NULL)
  {
    temp = (dev_cap *) temp_dev->data;

    two_c_hn = (temp->val + temp->val)/(*hptr); 

    xn1 = *(x_unk_copy[temp->n_plus]->xpast);
    xn1d = x_unk_copy[temp->n_plus]->xdpast;

    yn1 = *(x_unk_copy[temp->n_minus]->xpast);
    yn1d = x_unk_copy[temp->n_minus]->xdpast;

    sval = two_c_hn*(xn1 - yn1) + temp->val*(xn1d - yn1d); 

    if (temp->n_plus != ground_node)
    {
      if (source_only == FALSE)
        temp->mptr_pp->val = temp->mptr_pp->val + two_c_hn;

      b_src_copy[temp->n_plus]->val = 
                 b_src_copy[temp->n_plus]->val + sval; 
/*
      b_source[temp->n_plus]->val = b_source[temp->n_plus]->val 
                                    + sval; 
*/
    }

    if ((temp->n_plus != ground_node) && 
        (temp->n_minus != ground_node) &&
        (source_only == FALSE))
    {
      temp->mptr_pm->val = temp->mptr_pm->val - two_c_hn;

      temp->mptr_mp->val = temp->mptr_mp->val - two_c_hn;
    }

    if (temp->n_minus != ground_node)
    {
      if (source_only == FALSE)
        temp->mptr_mm->val = temp->mptr_mm->val + two_c_hn;

      b_src_copy[temp->n_minus]->val = 
                 b_src_copy[temp->n_minus]->val - sval;
/*
      b_source[temp->n_minus]->val = b_source[temp->n_minus]->val 
                                     - sval;
*/
    }

    temp_dev = temp_dev->next_dev;
  }

}  /* cap_trap */


void
cap_tran_print(FILE *fp, double inc_rate, int prtype, device *temp_dev)
{
  long plus, minus; 
  double cap_val;
  double val;
  double plus_val, minus_val, pd_val, md_val;


  plus = ((dev_cap *) temp_dev->data)->n_plus;
  minus = ((dev_cap *) temp_dev->data)->n_minus;
  cap_val = ((dev_cap *) temp_dev->data)->val;

  
  if (prtype == VOLT)
  {
    plus_val = get_pr_val(inc_rate, plus, 0);
    minus_val = get_pr_val(inc_rate, minus, 0);
    fprintf(fp, out_format, plus_val - minus_val);
    return;
  }

  if (prtype == AMP)
  {
    pd_val = get_pr_val(inc_rate, plus, 1);
    md_val = get_pr_val(inc_rate, minus, 1);
    val = (pd_val - md_val)*cap_val;
    fprintf(fp, out_format, val);
    return;

  } 

}  /* cap_tran_print */

