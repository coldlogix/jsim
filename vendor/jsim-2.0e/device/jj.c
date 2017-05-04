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
read_jj()
{
  modeldata *modptr;
  long name;
  int bad_dev = FALSE;

  *dataptr = 1.0;
  *(dataptr + 1) = 0.0;
  *(dataptr + 2) = 0.0;
  intptr[1] = (long) NULL;

  read_error = read_long(nodeptr, "plus node", FALSE);
  read_error = read_long((nodeptr+1), "minus node", FALSE); 
  read_error = read_string("model name");
  if (read_error == OK) modptr = (modeldata *) new_string(tempstring);

  if (read_error != OK)
  {
    printf("## Error -- bad jj definition\n");
    no_go = TRUE;
    bad_dev = TRUE;
  }
  
  while ((read_error = read_string("")) == OK)
  {
    switch(get_string_keyword(tempstring))
    {
      case AREA : 

           read_error = read_double(dataptr, "junction area", FALSE);
           if (read_error != OK)
           {
             printf("## Warning -- illegal area\n");
             warned = TRUE;
           }
           break;
      
      case IC : 
      
           read_error = read_double(dataptr+1, "initial voltage",
                                    FALSE);
           read_error = read_double(dataptr+2, "initial phase", 
                                    FALSE);
           if (read_error != OK)
           {
             printf("## Warning -- illegal initial values\n");
             warned = TRUE;
           }
           break;
      
      case CONDEV : 
     
           read_error = read_string("control device");
           if (read_error == OK)
           {
             intptr[1] = (long) new_string(tempstring);
           }
           else 
           {
             printf("## Warning -- illegal condev\n");
             warned = TRUE;
           }
           break;

      default : 
        
           printf("## Warning -- illegal keyword %s\n", tempstring);
           warned = TRUE;
           break;

     }   /* switch */

     read_error = OK;

  }  /* while */

  intptr[0] = LATER;

  if (bad_dev == FALSE)
  {
    if (current_sub_def == NULL)
    {
      name = add_realname(dev_name);
      jj_to_devlist(name, JJ, nodeptr, intptr, dataptr, modptr);
    }
    else
      jj_to_deftree(dev_name, JJ, nodeptr, intptr, dataptr, modptr);
  }

}   /* read_jj */


device *
jj_to_devlist(long name, int type, long *nodeptr, long *intptr,
    double *dataptr, modeldata *modptr)
{
  long n_plus, n_minus; 
  branch_marker *bmarker;

  switch (type)
  {
    case JJ : 

         bmarker = add_bmarker();
         n_plus = add_node(*nodeptr, name, type, PLUS_NODE, bmarker);
         n_minus = add_node(*(nodeptr+1), name, type, MINUS_NODE, 
                            bmarker);

         return(add_jj(name, *dataptr, *(dataptr+1), *(dataptr+2), 
                       n_plus, n_minus, (char *) *(intptr + 1), 
                       modptr));
          
  }

  return(NULL);

}   /* jj_to_devlist */   


device *
jj_to_deftree(char *dev_name, int type, long *nodeptr, long *intptr,
    double *dataptr, modeldata *modptr)
{
  long name;

  name = (long) dev_name;

  switch (type)
  {
    case JJ : 
         
         return(add_jj(name, *dataptr, *(dataptr+1), *(dataptr+2),
                       *nodeptr, *(nodeptr+1), (char*)*(intptr+1),  modptr));
    
  }

  return(NULL);

}   /* jj_to_deftree */   


device *
jjsub_to_devlist(char *dev_name, int type, char *data)
{
  int n_plus, n_minus; 
  long name;
  char *con_dev;
  branch_marker *bmarker;

  name = add_realname(combine_string(namestring, dev_name));

  switch (type)
  {
    case JJ : 

         bmarker = add_bmarker();
         n_plus = getnode_subnode(((sub_jj *) data)->n_plus,
                                  ((sub_jj *) data)->p_node);
         n_plus = add_node(n_plus, name, type, PLUS_NODE, bmarker);
         n_minus = getnode_subnode(((sub_jj *) data)->n_minus,
                                   ((sub_jj *) data)->m_node);
         n_minus = add_node(n_minus, name, type, MINUS_NODE,
                            bmarker);

         if (((sub_jj *) data)->con_dev != NULL)
           con_dev = get_subdev_name(((sub_jj *) data)->con_dev);
         else con_dev = NULL;

         return(add_jj(name, ((sub_jj *) data)->area, 
                       ((sub_jj *) data)->v_ic, 
                       ((sub_jj *) data)->phi_ic, 
                       n_plus, n_minus, con_dev, 
                       ((sub_jj *) data)->mod)); 
    
  }  /* switch */

  return(NULL);

}   /* jjsub_to_devlist */


void
get_jjmodel()
{
  device *temp_dev;
  dev_jj *temp_jj;
  modeldata *modptr;
  realname_name *temp_name;
  

  temp_dev = jj;

  while (temp_dev != NULL)
  {
    temp_jj = (dev_jj *) temp_dev->data;
    modptr = search_model((char *) temp_jj->mod); 
    if (modptr != NULL) 
    {
      /* SRW ** Can't do this.  Strings used more than once get clobbered
       * by free(), so search_model() fails.
      free((char *) temp_jj->mod);
      */
      temp_jj->mod = modptr;
    }
    else
    {
      no_go = TRUE;
      temp_name = search_name(temp_dev->name);
      printf("## Error -- model %s for %s does not exit\n",
              (char *) temp_jj->mod, temp_name->real_name);
      temp_jj->mod = NULL;
    } 
   
    temp_dev = temp_dev->next_dev;
  }

}   /* get_jjmodel */


device *
add_jj(long name, double area, double v_ic, double phi_ic, long n_plus,
    long n_minus, char *con_dev, modeldata *mod)
{
  int i;
  char *data;
  dev_jj *temp;
  sub_jj *temp_sub;
   
  if (current_sub_def == NULL)
  {
    temp = (dev_jj *) mycalloc(1, sizeof(dev_jj));
  
    temp->area = area;
    temp->v_ic = v_ic;
    temp->phi_ic = phi_ic;
    temp->n_plus = n_plus;
    temp->n_minus = n_minus;
    temp->con_dev = con_dev;
    temp->mod = mod;
    temp->which_piece = SUB_GAP;
    for (i = 0; i <= POLY_ORDER; i++) temp->poly_coeff[i] = 0.0;
  
    data = (char *) temp;  
    return(add_dev(name, JJ, data));
  }
  else 
  {
    temp_sub = (sub_jj *) mycalloc(1, sizeof(sub_jj));
 
    temp_sub->area = area;
    temp_sub->v_ic = v_ic;
    temp_sub->phi_ic = phi_ic;
    temp_sub->n_plus = n_plus;
    temp_sub->n_minus = n_minus;
    temp_sub->con_dev = con_dev;
    temp_sub->p_node = scan_node(n_plus);
    temp_sub->m_node = scan_node(n_minus);
    temp_sub->mod = mod;
  
    data = (char *) temp_sub;  
    return(add_sub((char *) name, JJ, data));
  }

}  /* add_jj */


void
jj_dependent()
{
  device *temp_dev, *con_dev;
  dev_jj *temp_jj;
  realname_name *temp_name;
  int type;

  temp_dev = jj;

  while (temp_dev != NULL)
  {
    temp_jj = (dev_jj *) temp_dev->data;
    
    if (temp_jj->con_dev != NULL) 
    {
      type = get_devname_type(temp_jj->con_dev);

      switch (type)
      {
        case INDUCT :
        case I_SOURCE :
        case V_SOURCE : 

             con_dev = find_dev(temp_jj->con_dev, type);
             if (con_dev == NULL)
             {
               temp_name = search_name(temp_dev->name);
               printf("## Warning -- con dev %s for",
                      temp_jj->con_dev);
               printf(" %s not found\n", temp_name->real_name); 
               warned = TRUE;
               free(temp_jj->con_dev);
               temp_jj->con_dev = NULL;
             }
             else
               temp_jj->con_dev = (char *) con_dev;
             break;

        default : 

             temp_name = search_name(temp_dev->name); 
             printf("## Warning -- incompatible con dev %s",
                         temp_jj->con_dev);
             printf(" for %s\n", temp_name->real_name);
             warned = TRUE;
             free(temp_jj->con_dev);
             temp_jj->con_dev = NULL;
             break;

      }  /* switch */ 

    }   /* if */

    temp_dev = temp_dev->next_dev;

  } /* while */
    
}     /* jj_dependent */


void
jj_matrix()
{
  dev_jj *temp;
  m_data *temp_m;
  mod_jj *temp_mod;
  device *temp_dev;
  int exist;

  temp_dev = jj;
  
  while (temp_dev != NULL)
  {
    temp = (dev_jj *) temp_dev->data;
    temp_mod = (mod_jj *) temp->mod->data;

    temp->ic = temp->area * temp_mod->ic;
    temp->ic_current = temp->ic_past = temp->ic;
    temp->cap = temp->area * temp_mod->cap;
    temp->gg = temp->area / temp_mod->rg;
    temp->gn = temp->area / temp_mod->rn;
    temp->glarge = temp->area * temp_mod->glarge;

    if (temp->n_plus != GROUND)
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

    phi_count++;
    temp->n_phi = phi_count + eqn_count + 1; 
/*
    add_unknown(x_unknown, temp->n_phi);
*/
    x_unknown[temp->n_phi]->type = PHI;

    if (temp->n_plus == GROUND) temp->n_plus = ground_node;
    if (temp->n_minus == GROUND) temp->n_minus = ground_node;

    temp_dev = temp_dev->next_dev;
  }

}  /* jj_matrix */ 


double
jj_condev_i(dev_jj *temp_jj, double time)
{
  device *temp_dev;

  temp_dev = (device *) temp_jj->con_dev;

  switch (temp_dev->type)
  {
    case INDUCT :  return(x_unk_copy[
                      ((dev_ind *) temp_dev->data)->branch]->xn);
    case V_SOURCE : return(x_unk_copy[ 
                      ((dev_vsource *) temp_dev->data)->branch]->xn);
   
    case I_SOURCE : return(is_eval(temp_dev, time));
   
    default : return(0.0);

  }  /* switch */

}   /* jj_condev_i */
    

void
jj_quasi(dev_jj *temp_jj, double val_quasi, int *which_piece,
    double *gval_quasi, double *sval_quasi)
{
  mod_jj *temp_mod;

  temp_mod = (mod_jj *) (temp_jj->mod->data);

  if (temp_mod->rtype == JJ_SHUNT)
  {
    *which_piece = SUB_GAP;
    *gval_quasi = 0.0;
    *sval_quasi = 0.0;
    return;
  }

  if (val_quasi <= temp_mod->quasi_bkpoint[0])
  {
    *which_piece = SUB_GAP;
    *gval_quasi = temp_jj->gg;
    *sval_quasi = 0.0;
    return;
  }
  
  if (val_quasi <= temp_mod->quasi_bkpoint[1])
  {
    if ( temp_jj->which_piece == SUB_GAP)
    {
      *which_piece = SUB_GAP;
      *gval_quasi = temp_jj->gg;
      *sval_quasi = 0.0;
      return;
    }
    else
    {
      *which_piece = TRANSITION;
      *gval_quasi = temp_jj->glarge;
      *sval_quasi = val_quasi*(temp_jj->gg - temp_jj->glarge);
      return;
    }
  }

  if (val_quasi <= temp_mod->quasi_bkpoint[2])
  {
    *which_piece = TRANSITION;
    *gval_quasi = temp_jj->glarge;
    *sval_quasi = temp_mod->vgap*(temp_jj->gg - temp_jj->glarge);
    return;
  }

  if (val_quasi <= temp_mod->quasi_bkpoint[3])
  {
    if (temp_jj->which_piece == NORMAL)
    {
      *which_piece = NORMAL;
      *gval_quasi = temp_jj->gn;
      *sval_quasi = 0.0;
      return;
    }
    else 
    {
      *which_piece = TRANSITION;
      *gval_quasi = temp_jj->glarge;
      *sval_quasi = val_quasi * (temp_jj->gn - temp_jj->glarge);
      return;
    }
  }

  *which_piece = NORMAL;
  *gval_quasi = temp_jj->gn;
  *sval_quasi = 0.0;
  return;

}  /* jj_quasi */


void
jj_fix_trap(int source_only, int *need_lu, double *hptr)
{
  dev_jj *temp;
  device *temp_dev;
  mod_jj *temp_moddata;
  double cap_val, ic, icon;
  double current_g, last_g;
  double source_val, phi_guess, sval;
  double xn1, xn1d, yn1, yn1d, two_c_hn;
  int current_piece;
  int tempc;

  temp_dev = jj;

  *need_lu = FALSE;
  
  while (temp_dev != NULL)
  {
    temp = (dev_jj *) temp_dev->data;

    temp_moddata = (mod_jj *) (temp->mod->data);
    ic = temp->ic;
    if (temp->con_dev != NULL)
    if (temp_moddata->cct == SINXX)
    {
      icon = jj_condev_i(temp, last_time + *hptr);

      if (myabs(icon) > 1.0e-12 * myabs(temp_moddata->icon))
        ic = ic * (sin(PI*icon/temp_moddata->icon)/
             (PI*icon/temp_moddata->icon));

      temp->ic_current = ic;

    }

    cap_val = temp->cap;

    if (temp_moddata->rtype == JJ_SHUNT) last_g = 0.0;
    else if (temp_moddata->rtype == JJ_PWL)
    {
      if (temp->which_piece == SUB_GAP) last_g = temp->gg;
      else
      {
        if (temp->which_piece == TRANSITION) last_g = temp->glarge;
        else last_g = temp->gn;
      }
    }

    two_c_hn = (cap_val + cap_val)/(*hptr); 

    xn1 = *(x_unk_copy[temp->n_plus]->xpast);
    xn1d = x_unk_copy[temp->n_plus]->xdpast;

    yn1 = *(x_unk_copy[temp->n_minus]->xpast);
    yn1d = x_unk_copy[temp->n_minus]->xdpast;
 
    phi_guess = x_unknown[temp->n_phi]->yn;

    jj_quasi(temp, myabs(temp->vn_guess), &current_piece, &current_g,
             &source_val);

    if (temp->vn_guess < 0.0) source_val = 0.0 - source_val;

/*
    if (temp_moddata->rtype == JJ_SHUNT)
    {
      current_piece = SUB_GAP;
      current_g = 0.0;
      source_val = ic*sin(phi_guess);
    }
    else if (temp_moddata->rtype == JJ_PWL)
    {
      if (myabs(temp->vn_guess) <= 0.9*(temp_moddata->vgap))
      {
        current_piece = SUB_GAP;
        current_g = gg;
        source_val = ic*sin(phi_guess);
      }
      else if (myabs(temp->vn_guess) >= 1.1*(temp_moddata->vgap) +
               temp_moddata->del_v)
      {
        current_piece = NORMAL;
        current_g = gn;
        source_val = ic*sin(phi_guess);
      }
      else if (myabs(temp->vn_guess) < temp_moddata->vgap)
      {
        if (temp->which_piece == SUB_GAP)
        {
          current_piece = SUB_GAP;
          current_g = gg;
          source_val = ic*sin(phi_guess);
        }
        else
        {
          current_piece = TRANSITION;
          current_g = glarge;
          source_val = temp->vn_guess*(gg - glarge);
          source_val = ic*sin(phi_guess) + temp->vn_guess*(gg - glarge);
        }
      }
      else if (myabs(temp->vn_guess) > temp_moddata->vgap +
               temp_moddata->del_v)
      {
        if (temp->which_piece == NORMAL)
        {
          current_piece = NORMAL;
          current_g = gn;
          source_val = ic*sin(phi_guess);
        }
        else
        {
          current_piece = TRANSITION;
          current_g = glarge;
          source_val = ic*sin(phi_guess) + temp->vn_guess*(gn - glarge);
        }
      }
      else
      {
        current_piece = TRANSITION;
        current_g = glarge;
        source_val = ic*sin(phi_guess) + 
                     temp_moddata->vgap*(gg - glarge);
      }
    }

 */

    source_val = ic*sin(phi_guess) + source_val;

    if ((current_piece != temp->which_piece) && (*need_lu == FALSE))
      *need_lu = TRUE;

    sval = two_c_hn*(xn1 - yn1) + cap_val*(xn1d - yn1d) - source_val;

    if (temp->n_plus != ground_node)
    {
      if (source_only == FALSE) 
         temp->mptr_pp->val = temp->mptr_pp->val + two_c_hn + current_g;
      else if (current_piece != temp->which_piece)
         temp->mptr_pp->val = temp->mptr_pp->val + current_g - last_g;

      b_src_copy[temp->n_plus]->val_nonlin = 
                         b_src_copy[temp->n_plus]->val_nonlin + sval;
/*
      b_source[temp->n_plus]->val_nonlin = 
                             b_source[temp->n_plus]->val_nonlin + sval;
*/
                                  
    }

    if ((temp->n_plus != ground_node) && 
        (temp->n_minus != ground_node))
    {
      if (source_only == FALSE)
      {
        temp->mptr_pm->val = temp->mptr_pm->val - two_c_hn - current_g;
        temp->mptr_mp->val = temp->mptr_mp->val - two_c_hn - current_g;
      }
      else if (current_piece != temp->which_piece)
      {
        temp->mptr_pm->val = temp->mptr_pm->val - current_g + last_g;
        temp->mptr_mp->val = temp->mptr_mp->val - current_g + last_g;
      }
    }

    if (temp->n_minus != ground_node)
    {
      if (source_only == FALSE)
        temp->mptr_mm->val = temp->mptr_mm->val + two_c_hn + current_g;
      else if (current_piece != temp->which_piece)
        temp->mptr_mm->val = temp->mptr_mm->val + current_g - last_g;

      b_src_copy[temp->n_minus]->val_nonlin = 
                        b_src_copy[temp->n_minus]->val_nonlin - sval;
/*
      b_source[temp->n_minus]->val_nonlin = 
                            b_source[temp->n_minus]->val_nonlin - sval;
*/
    }

    temp->which_piece = current_piece;

    temp_dev = temp_dev->next_dev;
  }

}  /* jj_fix_trap */


void
update_jj()
{
  device *temp_dev;
  dev_jj *temp_jj;
  double xn, xnd, yn, ynd; 

  temp_dev = jj;

  while (temp_dev != NULL)
  {
    temp_jj = (dev_jj *) temp_dev->data;

    xn = x_unk_copy[temp_jj->n_plus]->xn;
    xnd = x_unk_copy[temp_jj->n_plus]->yn;

    yn = x_unk_copy[temp_jj->n_minus]->xn;
    ynd = x_unk_copy[temp_jj->n_minus]->yn;

    temp_jj->poly_coeff[0] = xn - yn;
    temp_jj->poly_coeff[1] = (xnd - ynd);

    temp_jj->ic_past = temp_jj->ic_current;
    
    temp_dev = temp_dev->next_dev;

  }

}  /* update_jj */


void
jj_iteration_update()
{
  device *temp_dev;
  dev_jj *temp_jj;
  double xn, yn;
  
  temp_dev = jj;
  
  while (temp_dev != NULL)
  {
    temp_jj = (dev_jj *) temp_dev->data;
    
    xn = x_unk_copy[temp_jj->n_plus]->xn;
  
    yn = x_unk_copy[temp_jj->n_minus]->xn;

    temp_jj->vn_guess = xn - yn;

    temp_dev = temp_dev->next_dev;
  }

}   /* jj_iteration_update */


double
jj_step_limit(double current_step)
{
  device *temp_dev;
  dev_jj *temp_jj;
  double smallest;
  double h;
  double delta_phi;

  smallest = 1.0e30;

  temp_dev = jj;

  while (temp_dev != NULL)
  {
    temp_jj = (dev_jj *) temp_dev->data;

    if (temp_dev->type == JJ)
    { 
      delta_phi = myabs(x_unknown[temp_jj->n_phi]->xn -
                *(x_unknown[temp_jj->n_phi]->xpast));
    

      delta_phi = delta_phi*VN_GUESS_REF/
                (vn_guess_min + myabs(temp_jj->vn_guess));
      
    
      if (delta_phi > 0.1*max_phi_step)
        h = current_step*(max_phi_step / delta_phi);
      else h = 10.0*current_step;

      if (h < smallest) smallest = h;
    }
  
    temp_dev = temp_dev->next_dev;
  }
    
  return(smallest);

}   /* jj_step_limit */


int
jj_apriori_step_limit(double step, double *newstep)
{
  int i;
  dev_jj *temp;
  device *temp_dev;
  double temp_h;
  double xn1, yn1;
  double h;
  double delta_phi;
  double large_delta_phi;
  double allowed_phi_step;


  temp_dev = jj;

  large_delta_phi = 0.0;
  
  while (temp_dev != NULL)
  {
    temp = (dev_jj *) temp_dev->data;

    temp_h = step;
    h = 1.0;
   
    temp->vn_guess = 0.0;

    for (i = 0; i <= POLY_ORDER; i++)
    {
      temp->vn_guess = temp->vn_guess + temp->poly_coeff[i]*h;
      h = h*temp_h;
    }  

    xn1 = *(x_unk_copy[temp->n_plus]->xpast);

    yn1 = *(x_unk_copy[temp->n_minus]->xpast);
    
    x_unknown[temp->n_phi]->yn = *(x_unknown[temp->n_phi]->xpast) +
                E_HBAR*step*(temp->vn_guess + xn1 - yn1);

    delta_phi = myabs(x_unknown[temp->n_phi]->yn -
                      *(x_unknown[temp->n_phi]->xpast));

    delta_phi = delta_phi*VN_GUESS_REF/
                (vn_guess_min + myabs(temp->vn_guess));

    if (delta_phi > large_delta_phi) large_delta_phi = delta_phi;

    temp_dev = temp_dev->next_dev;

  }   /* while */

  allowed_phi_step = 1.9*max_phi_step;

  if (allowed_phi_step > 3.0) allowed_phi_step = 3.0;

  if (large_delta_phi < allowed_phi_step) 
  {
    *newstep = step;
    return(OK);
  }

  if (large_delta_phi > allowed_phi_step + allowed_phi_step) 
  {
    *newstep = step/4.0;
    return(TOO_BIG);
  }
  else
  {
    *newstep = step/2.0;
    return(TOO_BIG);
  }

}  /* jj_apriori_step_limit */


void
jj_tran_print(FILE *fp, double inc_rate, int prtype, int subtype,
    device *temp_dev)
{
  long plus, minus, phi; 
  double jj_area, i_joseph, i_cap, i_resis, i_total, j_phi;
  double plus_val, minus_val, phi_val, pd_val, md_val;
  modeldata *mod;

  plus = ((dev_jj *) temp_dev->data)->n_plus;
  minus = ((dev_jj *) temp_dev->data)->n_minus;
  phi = ((dev_jj *) temp_dev->data)->n_phi;
  jj_area = ((dev_jj *) temp_dev->data)->area;
  mod = ((dev_jj *) temp_dev->data)->mod;
  
  if (prtype == VOLT)
  {
    plus_val = get_pr_val(inc_rate, plus, 0);
    minus_val = get_pr_val(inc_rate, minus, 0);
    fprintf(fp, out_format, plus_val - minus_val);
    return;
  }

  if (prtype == AMP)
  {
    plus_val = get_pr_val(inc_rate, plus, 0);
    minus_val = get_pr_val(inc_rate, minus, 0);
    phi_val = get_pr_val(inc_rate, phi, 0);
    pd_val = get_pr_val(inc_rate, plus, 1);
    md_val = get_pr_val(inc_rate, minus, 1);
    i_joseph = get_pr_jjic(inc_rate, (dev_jj *) temp_dev->data) *
                 sin(phi_val);
    i_cap = (((dev_jj *) temp_dev->data)->cap)* (pd_val - md_val);
    i_resis = jj_area*jjiv(plus_val - minus_val, mod);
    i_total = i_joseph + i_cap + i_resis;

    switch(subtype)
    {
      case JJALL : 

                fprintf(fp, out_format, i_total);
                fprintf(fp, out_format, i_joseph);
                fprintf(fp, out_format, i_cap);
                fprintf(fp, out_format, i_resis);
                break;

      case JJJOSEPH : 

                fprintf(fp, out_format, i_joseph);
                break;
       
      case JJCAP : 

                fprintf(fp, out_format, i_cap);
                break;

      case JJRESIS : 

                fprintf(fp, out_format, i_resis);
                break;
            
      case JJTOTAL :

                fprintf(fp, out_format, i_total);
                break;
                  
      default : 

                fprintf(fp, out_format, i_total);
                break;

    }  /* switch */

    return;

  }

  if (prtype == PHI)
  if (temp_dev->type == JJ)
  {
    j_phi = get_pr_val(inc_rate, phi, 0);
    j_phi = j_phi - *(x_unknown[phi]->xpast) + 
                    *(x_unknown[phi]->xpast + 1);
    fprintf(fp, out_format, j_phi);
    return;
  }

}  /* jj_tran_print */

