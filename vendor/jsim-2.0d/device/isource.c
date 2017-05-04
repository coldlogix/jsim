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
read_is()
{
  long name;
  int ftype;
  char *temp_parm;
  dev_isource *temp_is;
  sub_isource *temp_subi;

  read_error = read_long(nodeptr, "plus node", FALSE);
  read_error = read_long((nodeptr+1), "minus node", FALSE); 

  if (read_error == OK)
    temp_parm = read_function(&ftype);
  else temp_parm = NULL;

  if (temp_parm == NULL)
  {
    printf("## Error -- bad current source definition\n");
    no_go = TRUE;
    return;
  }

  if (current_sub_def == NULL)
  {
    temp_is = (dev_isource *) mycalloc(1, sizeof(dev_isource));
    temp_is->function = ftype;
    temp_is->function_parm = temp_parm;
    temp_is->n_plus = nodeptr[0];
    temp_is->n_minus = nodeptr[1];
    name = add_realname(dev_name);
    is_to_devlist(name, temp_is);
  }
  else
  {
    temp_subi = (sub_isource *) mycalloc(1, sizeof(sub_isource));
    temp_subi->function = ftype;
    temp_subi->function_parm = temp_parm;
    temp_subi->n_plus = nodeptr[0];
    temp_subi->n_minus = nodeptr[1];
    temp_subi->p_node = scan_node(nodeptr[0]);
    temp_subi->m_node = scan_node(nodeptr[1]);
    is_to_deftree(dev_name, temp_subi);
  }

}   /* read_is */


device *
is_to_devlist(long name, dev_isource *temp_is)
{
  branch_marker *bmarker;

  bmarker = add_bmarker();
  temp_is->n_plus = add_node(temp_is->n_plus, name, I_SOURCE, 
                             PLUS_NODE, bmarker);
  temp_is->n_minus = add_node(temp_is->n_minus, name, I_SOURCE,
                            MINUS_NODE, bmarker);

  return(add_isource(name, (char *) temp_is));

}   /* is_to_devlist */   


device *
is_to_deftree(char *dev_name, sub_isource *temp_subi)
{
  long name;

  name = (long) dev_name;

  return(add_isource(name, (char *) temp_subi));
    
}   /* is_to_deftree */   


device *
issub_to_devlist(char *dev_name, char *data)
{
  int n_plus, n_minus, branch; 
  long name;
  branch_marker *bmarker;
  dev_isource *temp_is;

  name = add_realname(combine_string(namestring, dev_name));


  bmarker = add_bmarker();
  n_plus = getnode_subnode(((sub_isource *) data)->n_plus,
                           ((sub_isource *) data)->p_node);
  n_plus = add_node(n_plus, name, I_SOURCE, PLUS_NODE, bmarker);
  n_minus = getnode_subnode(((sub_isource *) data)->n_minus,
                            ((sub_isource *) data)->m_node);
  n_minus = add_node(n_minus, name, I_SOURCE, MINUS_NODE, bmarker);

  temp_is = (dev_isource *) mycalloc(1, sizeof(dev_isource));

  temp_is->n_plus = n_plus;
  temp_is->n_minus = n_minus;
  temp_is->function = ((sub_isource *) data)->function;
  new_function(temp_is->function, &(temp_is->function_parm),
               ((sub_isource *) data)->function_parm);

  return(add_isource(name, (char *) temp_is));

}   /* issub_to_devlist */


device *
add_isource(long name, char *data)
{
  if (current_sub_def == NULL)
  {
    return(add_dev(name, I_SOURCE, data));
  }
  else
  {
    return(add_sub((char *) name, I_SOURCE, data));
  }
                 
}  /* add_isource */


void
is_matrix()
{
  m_data *temp_m;
  device *temp_dev;
  dev_isource *temp_is;
  int i, exist;
  double val;

  temp_dev = isource;

  while (temp_dev != NULL)
  {
    temp_is = (dev_isource *) temp_dev->data;

    if (temp_is->n_plus != GROUND)
    {
/*
      if (x_unknown[temp_is->n_plus] == NULL)
      {
        add_unknown(x_unknown, temp_is->n_plus);
        x_unknown[temp_is->n_plus]->type = VOLT;
      }
*/
      add_source(b_source, temp_is->n_plus);
    }

    if (temp_is->n_minus != GROUND) 
    {
/*
      if (x_unknown[temp_is->n_minus] == NULL)
      {
        add_unknown(x_unknown, temp_is->n_minus);
        x_unknown[temp_is->n_minus]->type = VOLT;
      }
*/
      add_source(b_source, temp_is->n_minus);
    }

    if (temp_is->n_plus == GROUND) temp_is->n_plus = ground_node;
    if (temp_is->n_minus == GROUND) temp_is->n_minus = ground_node;

    temp_dev = temp_dev->next_dev;
  }

}  /* is_matrix */


void
is_breakpoint()
{
  device *temp_dev;
  dev_isource *temp_isource;

  temp_dev = isource;
  
  while (temp_dev != NULL)
  {
    temp_isource = (dev_isource *) temp_dev->data;

    func_breakpoint(temp_isource->function,
                    temp_isource->function_parm,
                    user_step, stop_time);

    temp_dev = temp_dev->next_dev;
  
  }  /* while */

}  /* is_breakpoint */


double
is_eval(device *temp_dev, double time)
{
  dev_isource *temp_is;
  
  temp_is = (dev_isource *) temp_dev->data;

  return(func_eval(temp_is->function, temp_is->function_parm, time));

}    /* is_eval */


void
is_advance(double *hptr)
{
  device *temp_dev;
  dev_isource *temp_is;
  double temp_val;

  temp_dev = isource;

  while (temp_dev != NULL)
  {
    temp_is = (dev_isource *) temp_dev->data;
 
    temp_val = is_eval(temp_dev, *hptr + last_time);

    if (temp_is->n_plus != ground_node)
      b_src_copy[temp_is->n_plus]->val =
               b_src_copy[temp_is->n_plus]->val - temp_val;
/*
      b_source[temp_is->n_plus]->val =
               b_source[temp_is->n_plus]->val - temp_val;
*/
    
    if (temp_is->n_minus != ground_node)
      b_src_copy[temp_is->n_minus]->val =
               b_src_copy[temp_is->n_minus]->val + temp_val;
/*
      b_source[temp_is->n_minus]->val =
               b_source[temp_is->n_minus]->val + temp_val;
*/

    temp_dev = temp_dev->next_dev;

  }  /* while */

}    /* is_advance */


void
is_tran_print(FILE *fp, double inc_rate, int prtype, device *temp_dev,
    double time)
{
  long plus, minus; 
  double plus_val, minus_val;

  plus = ((dev_isource *) temp_dev->data)->n_plus;
  minus = ((dev_isource *) temp_dev->data)->n_minus;
  
  if (prtype == VOLT)
  {
    plus_val = get_pr_val(inc_rate, plus, 0);
    minus_val = get_pr_val(inc_rate, minus, 0);
    fprintf(fp, out_format, plus_val - minus_val);
    return;
  }

  if (prtype == AMP)
  {
    fprintf(fp, out_format, is_eval(temp_dev, time)); 
    return;
  } 

}  /* is_tran_print */

