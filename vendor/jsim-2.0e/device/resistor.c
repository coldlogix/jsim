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
read_resis()
{
  long name;
  int bad_dev = FALSE;

  read_error = read_long(nodeptr, "plus node", FALSE);
  read_error = read_long((nodeptr+1), "minus node", FALSE); 
  read_error = read_double(dataptr, "resistance", FALSE);

  if (read_error != OK)
  {
    printf("## Error -- bad resistor definition\n");
    no_go = TRUE;
    bad_dev = TRUE;
  }

  if (bad_dev == FALSE)
  {
    if (current_sub_def == NULL)
    {
      name = add_realname(dev_name);
      resis_to_devlist(name, RESIS, nodeptr, dataptr);
    }
    else
    {
      resis_to_deftree(dev_name, RESIS, nodeptr, dataptr);
    }
  }

}   /* read_resis */


device *
resis_to_devlist(long name, int type, long *nodeptr, double *dataptr)
{
  int n_plus, n_minus; 
  branch_marker *bmarker;

  switch (type)
  {
    case RESIS : 
         bmarker = add_bmarker();
         n_plus = add_node(*nodeptr, name, type, PLUS_NODE, bmarker);
         n_minus = add_node(*(nodeptr+1), name, type, MINUS_NODE, 
                            bmarker);
         return(add_resis(name, *dataptr, n_plus, n_minus));

  }

  return(NULL);

}   /* resis_to_devlist */   


device *
resis_to_deftree(char *dev_name, int type, long *nodeptr, double *dataptr)
{
  long name;

  name = (long) dev_name;

  switch (type)
  {
    case RESIS : 
         return(add_resis(name, *dataptr, *nodeptr, *(nodeptr+1)));

  }

  return(NULL);

}   /* resis_to_deftree */   


device *
resissub_to_devlist(char *dev_name, int type, char *data)
{
  int n_plus, n_minus; 
  long name;
  branch_marker *bmarker;

  name = add_realname(combine_string(namestring, dev_name));

  switch (type)
  {
    case RESIS : 
         bmarker = add_bmarker(); 
         n_plus = getnode_subnode(((sub_resis *) data)->n_plus,
                                  ((sub_resis *) data)->p_node);
         n_plus = add_node(n_plus, name, type, PLUS_NODE, bmarker);
         n_minus = getnode_subnode(((sub_resis *) data)->n_minus,
                                   ((sub_resis *) data)->m_node);
         n_minus = add_node(n_minus, name, type, MINUS_NODE,
                            bmarker);
         return(add_resis(name, ((sub_resis *) data)->val, 
                          n_plus, n_minus));

  }  /* switch */

  return(NULL);

}   /* resissub_to_devlist */


device *
add_resis(long name, double val, long n_plus, long n_minus)
{
  char *data;
  dev_resis *temp;
  sub_resis *temp_sub;
  
  if (current_sub_def == NULL)
  { 
    temp = (dev_resis *) mycalloc(1, sizeof(dev_resis));
  
    temp->val = val;
    temp->n_plus = n_plus;
    temp->n_minus = n_minus;
  
    data = (char *) temp;  
    return(add_dev(name, RESIS, data));
  }
  else
  {
    temp_sub = (sub_resis *) mycalloc(1, sizeof(sub_resis)); 

    temp_sub->val = val;
    temp_sub->n_plus = n_plus;
    temp_sub->n_minus = n_minus;
    temp_sub->p_node = scan_node(n_plus);
    temp_sub->m_node = scan_node(n_minus); 
  
    data = (char *) temp_sub;  
    return(add_sub((char *) name, RESIS, data));
  }
    
}  /* add_resis */


void
resis_matrix()
{
  dev_resis *temp;
  device *temp_dev;
  double g;
  int exist;

  temp_dev = resis;
  
  while (temp_dev != NULL)
  {
    temp = (dev_resis *) temp_dev->data;

    if (temp->n_plus != GROUND) 
    {
      temp->mptr_pp = add_i_j(A_matrix, temp->n_plus, 
                            temp->n_plus, &exist);
      temp->mptr_pp->type = ORIGINAL;

/*
      if (x_unknown[temp->n_plus] == NULL)
      {
        add_unknown(x_unknown, temp->n_plus);
        x_unknown[temp->n_plus]->type = VOLT;
      }
*/
    }

    if ((temp->n_plus != GROUND) && 
        (temp->n_minus != GROUND))
    {
    temp->mptr_pm = add_i_j(A_matrix, temp->n_plus, 
                            temp->n_minus, &exist);
    temp->mptr_pm->type = ORIGINAL;

    temp->mptr_mp = add_i_j(A_matrix, temp->n_minus, 
                            temp->n_plus, &exist);
    temp->mptr_mp->type = ORIGINAL; 
    }

    if (temp->n_minus != GROUND)
    {
      temp->mptr_mm = add_i_j(A_matrix, temp->n_minus, 
                            temp->n_minus, &exist);
      temp->mptr_mm->type = ORIGINAL;

/*
      if (x_unknown[temp->n_minus] == NULL)
      {
        add_unknown(x_unknown, temp->n_minus);
        x_unknown[temp->n_minus]->type = VOLT;
      }
*/
    }

    if (temp->n_plus == GROUND) temp->n_plus = ground_node;
    if (temp->n_minus == GROUND) temp->n_minus = ground_node;

    temp_dev = temp_dev->next_dev;
  }

}  /* resis_matrix */


void
resis_trap(int source_only)
{
  dev_resis *temp;
  device *temp_dev;
  int exist;
  double g_val;

  if (source_only == TRUE) return;

  temp_dev = resis;
  
  while (temp_dev != NULL)
  {
    temp = (dev_resis *) temp_dev->data;

    g_val = 1.0/temp->val;

    if (temp->n_plus != ground_node)
    {
      temp->mptr_pp->val = temp->mptr_pp->val + g_val;
    }

    if ((temp->n_plus != ground_node) && 
        (temp->n_minus != ground_node))
    {

      temp->mptr_pm->val = temp->mptr_pm->val - g_val;

      temp->mptr_mp->val = temp->mptr_mp->val - g_val;

    }

    if (temp->n_minus != ground_node)
    {
      temp->mptr_mm->val = temp->mptr_mm->val + g_val;
    }

    temp_dev = temp_dev->next_dev;
  }

}  /* resis_trap */


void
resis_tran_print(FILE *fp, double inc_rate, int prtype, device *temp_dev)
{
  long plus, minus; 
  double r_val;
  double val;
  double plus_val, minus_val;

  plus = ((dev_resis *) temp_dev->data)->n_plus;
  minus = ((dev_resis *) temp_dev->data)->n_minus;
  r_val = ((dev_resis *) temp_dev->data)->val;

  plus_val = get_pr_val(inc_rate, plus, 0);
  minus_val = get_pr_val(inc_rate, minus, 0);

  if (prtype == VOLT)
  {
    fprintf(fp, out_format, plus_val - minus_val);
    return;
  }

  if (prtype == AMP)
  {
    val = (plus_val - minus_val)/r_val;
    fprintf(fp, out_format, val);
  }   /* switch */

}  /* resis_tran_print */

