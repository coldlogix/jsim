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
read_vs()
{
  long name;
  int ftype;
  char *temp_parm;
  dev_vsource *temp_vs;
  sub_vsource *temp_subv;

  read_error = read_long(nodeptr, "plus node", FALSE);
  read_error = read_long((nodeptr+1), "minus node", FALSE); 
  if (read_error == OK)
    temp_parm = read_function(&ftype);
  else temp_parm = NULL;
 
  if (temp_parm == NULL)
  {
    printf("## Error -- bad voltage source definition\n");
    no_go = TRUE;
    return;
  }

  if (current_sub_def == NULL)
  {
    temp_vs = (dev_vsource *) mycalloc(1, sizeof(dev_vsource)); 
    temp_vs->function = ftype;
    temp_vs->function_parm = temp_parm;
    temp_vs->n_plus = nodeptr[0];
    temp_vs->n_minus = nodeptr[1];
    name = add_realname(dev_name);
    vs_to_devlist(name, temp_vs);
  }
  else
  {
    temp_subv = (sub_vsource *) mycalloc(1, sizeof(sub_vsource));
    temp_subv->function = ftype;
    temp_subv->function_parm = temp_parm;
    temp_subv->n_plus = nodeptr[0];
    temp_subv->n_minus = nodeptr[1];
    temp_subv->p_node = scan_node(nodeptr[0]);
    temp_subv->m_node = scan_node(nodeptr[1]);
    vs_to_deftree(dev_name, temp_subv);
  }

}   /* read_vs */
    

device *
vs_to_devlist(long name, dev_vsource *temp_vs)
{
  branch_marker *bmarker;

  bmarker = add_bmarker();
  temp_vs->n_plus = add_node(temp_vs->n_plus, name, V_SOURCE, 
                             PLUS_NODE, bmarker);
  temp_vs->n_minus = add_node(temp_vs->n_minus, name, V_SOURCE, 
                              MINUS_NODE, bmarker);
  eqn_count++;
  temp_vs->branch = eqn_count;

  return(add_vsource(name, (char *) temp_vs));

}   /* vs_to_devlist */   


device *
vs_to_deftree(char *dev_name, sub_vsource *temp_subv)
{
  long name;

  name = (long) dev_name;

  return(add_vsource(name, (char *) temp_subv));

}   /* vs_to_deftree */   


device *
vssub_to_devlist(char *dev_name, char *data)
{
  int n_plus, n_minus, branch; 
  long name;
  branch_marker *bmarker;
  dev_vsource *temp_vs;

  name = add_realname(combine_string(namestring, dev_name));

  bmarker = add_bmarker(); 
  n_plus = getnode_subnode(((sub_vsource *) data)->n_plus,
                           ((sub_vsource *) data)->p_node);
  n_plus = add_node(n_plus, name, V_SOURCE, PLUS_NODE, bmarker);
  n_minus = getnode_subnode(((sub_vsource *) data)->n_minus,
                            ((sub_vsource *) data)->m_node);
  n_minus = add_node(n_minus, name, V_SOURCE, MINUS_NODE, bmarker);
  eqn_count++;
  branch = eqn_count;
 
  temp_vs = (dev_vsource *) mycalloc(1, sizeof(dev_vsource));
  
  temp_vs->n_plus = n_plus;
  temp_vs->n_minus = n_minus;
  temp_vs->branch = branch;
  temp_vs->function = ((sub_vsource *) data)->function;  
  new_function(temp_vs->function, &(temp_vs->function_parm),
                ((sub_vsource *) data)->function_parm);

  return(add_vsource(name, (char *) temp_vs));

}   /* vssub_to_devlist */


device *
add_vsource(long name, char *data)
{
  if (current_sub_def == NULL)
  {
    return(add_dev(name, V_SOURCE, data));
  }
  else
  {
    return(add_sub((char *) name, V_SOURCE, data));
  }

}  /* add_vsource */


void
vs_matrix()
{
  m_data *temp_m;
  device *temp_dev;
  dev_vsource *temp_vs;
  int exist;
  double val;

  temp_dev = vsource;

  while (temp_dev != NULL)
  {
    temp_vs = (dev_vsource *) temp_dev->data;

    if (temp_vs->n_plus != GROUND)
    {
      temp_m = add_i_j(A_matrix, temp_vs->branch, 
                                 temp_vs->n_plus, &exist);
      temp_m->val =  1.0;
      temp_m->type = CONST_ORIGINAL;

      temp_m = add_i_j(A_matrix, temp_vs->n_plus, 
                                 temp_vs->branch, &exist);
      temp_m->val =  1.0;
      temp_m->type = CONST_ORIGINAL;

/*
      if (x_unknown[temp_vs->n_plus] == NULL)
      {
        add_unknown(x_unknown, temp_vs->n_plus);
        x_unknown[temp_vs->n_plus]->type = VOLT;
      }
*/
    }

    if (temp_vs->n_minus != GROUND)
    {
      temp_m = add_i_j(A_matrix, temp_vs->branch, 
                                 temp_vs->n_minus, &exist);
      temp_m->val = 0.0 - 1.0;
      temp_m->type = CONST_ORIGINAL;

      temp_m = add_i_j(A_matrix, temp_vs->n_minus, 
                                 temp_vs->branch, &exist);
      temp_m->val = 0.0 - 1.0;
      temp_m->type = CONST_ORIGINAL;

/*
      if (x_unknown[temp_vs->n_minus] == NULL)
      {
        add_unknown(x_unknown, temp_vs->n_minus);
        x_unknown[temp_vs->n_minus]->type = VOLT;
      }
*/
    }

/*
    add_unknown(x_unknown, temp_vs->branch);
*/
    x_unknown[temp_vs->branch]->type = AMP;

    add_source(b_source, temp_vs->branch);
  
    if (temp_vs->n_plus == GROUND) temp_vs->n_plus = ground_node;
    if (temp_vs->n_minus == GROUND) temp_vs->n_minus = ground_node;

    temp_dev = temp_dev->next_dev;
  }

}  /* vs_matrix */


void
vs_breakpoint()
{
  device *temp_dev;
  dev_vsource *temp_vsource;

  
  temp_dev = vsource;

  while (temp_dev != NULL)
  {

    temp_vsource = (dev_vsource *) temp_dev->data;


    func_breakpoint(temp_vsource->function,
                    temp_vsource->function_parm,
                    user_step, stop_time);
                   
    temp_dev = temp_dev->next_dev;
   
  }  /* while */

}  /* vs_breakpoint */


double
vs_eval(device *temp_dev, double time)
{

  dev_vsource *temp_vs;

  temp_vs = (dev_vsource *) temp_dev->data;

  return(func_eval(temp_vs->function, temp_vs->function_parm, time));

}    /* vs_eval */


void
vs_advance(double *hptr)
{

  device *temp_dev;
  dev_vsource *temp_vs;
  double temp_val;
  int index;

  
  temp_dev = vsource;

  while (temp_dev != NULL)
  {
    temp_vs = (dev_vsource *) temp_dev->data;
 
    temp_val = vs_eval(temp_dev, *hptr + last_time);

    b_src_copy[temp_vs->branch]->val =
                     b_src_copy[temp_vs->branch]->val + temp_val;
/*
    b_source[temp_vs->branch]->val =
                     b_source[temp_vs->branch]->val + temp_val;
*/

    temp_dev = temp_dev->next_dev;

  }  /* while */

}    /* vs_advance */


void
vs_tran_print(FILE *fp, double inc_rate, int prtype, device *temp_dev)
{
  long plus, minus, branch; 
  double plus_val, minus_val, branch_val;

  plus = ((dev_vsource *) temp_dev->data)->n_plus;
  minus = ((dev_vsource *) temp_dev->data)->n_minus;
  branch = ((dev_vsource *) temp_dev->data)->branch;
  
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

}  /* vs_tran_print */

