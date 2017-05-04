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
read_tran()
{
  read_error = read_double(&user_pr_step, "print step", FALSE);
  read_error = read_double(&stop_time, "stop time", FALSE);
  if (read_double(&start_pr_time, "start print time", TRUE) != OK)
  {
    start_pr_time = 0.0;
    user_step = DEF_TIME_STEP;
  }
  else if (read_double(&user_step, "max internal time step", TRUE) 
           != OK)
    user_step = DEF_TIME_STEP;

  if (read_error != OK)
  {
    printf("## Error -- bad transient analysis call\n"); no_go = TRUE;
  }
  if ((user_pr_step <= 0.0) || (stop_time <= 0.0) ||
      (user_step <= 0.0))  
  {
    printf("## Error -- bad transient analysis call,");
    printf(" cannot have zero/negative value for time step or");
    printf(" stop time \n");
    no_go = TRUE;
  }
  if (stop_time < user_pr_step)
  {
    printf("## Error -- bad transient analysis call,");
    printf(" stop time is less than print step\n");
    no_go = TRUE;
  }
  if (user_step > user_pr_step) user_step = user_pr_step;

}   /* read_tran */
  

double lu_solve_ratio, two_lu_solve;
int last_bkpoint;
double last_bktime;
double next_bktime;
double last_bkstep;
double next_bkstep;


void
check_breakpoint(double *step)
{
  if (last_time >= next_bktime)
  {
    last_bktime = next_bktime;
    last_bkstep = next_bkstep;
    last_bkpoint++;
    if (last_bkpoint < bkpoint_count - 1)
    {
      next_bktime = bkpoint_time[last_bkpoint+1];
      next_bkstep = bkpoint_step[last_bkpoint+1];
    }
    else
    {
      next_bktime = stop_time + stop_time;
      next_bkstep = stop_time;
    }
  }

  if (*step > last_bkstep) *step = last_bkstep;

  if (last_time + *step > next_bktime)
  {
    if (*step <= next_bkstep) return;
    else 
    {
      *step = next_bkstep;
      return;
    }
  }

}  /* check_breakpoint */


int
step_control(double current_step, double *new_step, double *hptr)
{
  double rel_error, error_limit;  
  double smallest;
  double h;
  double xnd, xn1d, xn2d, xnddd;
  int status;
  int i;
  int tempc;

  if (current_step <= 0.0) 
  {
    *new_step = user_step;
    return(OK);
  }  

  smallest = 1.0e30;

  if (no_truncation_error == TRUE) goto flux_limit;

  for (i = 0; i <= eqn_count; i++)
  if ((x_unknown[i]->type == VOLT_TIME) || 
      (x_unknown[i]->type == AMP_TIME))
  {
    xnd = (x_unknown[i]->xn - *(x_unknown[i]->xpast))/current_step;

    if (*hptr > 0.0)
    xn1d = (*(x_unknown[i]->xpast) - *(x_unknown[i]->xpast+1))/(*hptr);

    if (*(hptr+1) > 0.0)
    xn2d = (*(x_unknown[i]->xpast+1) - *(x_unknown[i]->xpast+2))
           /(*(hptr+1));

    if ((*hptr > 0.0) && (*(hptr + 1) > 0.0))
    {
      xnddd = (xnd - xn1d)/(current_step + *hptr) -
              (xn1d - xn2d)/(*hptr + *(hptr+1));
      xnddd = xnddd/(current_step + *hptr + *(hptr+1));
    }
    else if (*hptr > 0.0)
    {
      xnddd = (xnd - xn1d)/(current_step + *hptr) -
              (xn1d - 0.0)/(*hptr + *hptr);
      xnddd = xnddd/(current_step + *hptr + *hptr);
    }
    else 
    {
      xnddd = (xnd - 0.0)/(current_step + current_step);
      xnddd = xnddd/(current_step + current_step + current_step);
    }
    
    xnddd = myabs(xnddd);

    if (x_unknown[i]->type == VOLT_TIME)  
    {
      error_limit = vtran_min + vtran_rel*myabs(x_unknown[i]->yn); 
      rel_error = vtran_rel*myabs(x_unknown[i]->yn)/current_step;
      error_limit = mymax(error_limit, rel_error);
    }
    else if (x_unknown[i]->type == AMP_TIME)
    {
      error_limit = itran_min + itran_rel*myabs(x_unknown[i]->yn); 
      rel_error = itran_rel*myabs(x_unknown[i]->yn)/current_step;
      error_limit = mymax(error_limit, rel_error);
    }
  
    if (xnddd > 0.0) xnddd = sqrt(xnddd);

    error_limit = sqrt(error_tolerance*error_limit);

    if (xnddd > 0.0) h = error_limit/xnddd; 
    else h = user_step;

    if (h < smallest) smallest = h;

  }    

  flux_limit:

  h = ind_step_limit(current_step);

  if (h < smallest) smallest = h;

  h = jj_step_limit(current_step);


  if (h < smallest) smallest = h;
  
  if (smallest > current_step + current_step)
  {
     if (last_solve_count > lu_solve_ratio) 
     {
       *new_step = current_step + current_step;
       if (*new_step > user_step) *new_step = user_step;
     }
     else *new_step = current_step;
     return(OK);
  }
  else if (smallest > 1.5*current_step)
  {
     if (last_solve_count > two_lu_solve)
     {
       *new_step = current_step + current_step;
       if (*new_step > user_step) *new_step = user_step;
     }
     else *new_step = current_step;
     return(OK);
  }
  else if (smallest > 0.9*current_step)
  { 
    *new_step = current_step;
    return(OK);
  }
  else if (smallest + smallest > current_step)
  { 
    *new_step = current_step/2.0;
    return(TOO_BIG);
  }
  else
  {
    *new_step = current_step/4.0;
    return(TOO_BIG);
  }

}   /* step_control */


void
time_loop(double *hptr)
{
  int i;
  double step, newstep, temp_step, temp_newstep;
  double temp_hptr[STEPSAVE+1];
  int source_only;
  int timestep_count;
  int loop_count;
  int guess_step_count;
  int converged;
  int output_cnt = 0;
  int numsteps;
  double temp_time;
  FILE *fopen();

  source_only = FALSE;

  numsteps = 1.5 + (stop_time - start_pr_time)/user_pr_step;
  if (jsim_raw)
    stop_time += user_pr_step;
  lu_solve_ratio = 50.0*log(eqn_count + 5.0)/log(100.0);
  two_lu_solve = 2.0 * lu_solve_ratio;

  last_bkpoint = -1;
  next_bktime = 0.0;
  next_bkstep = bkpoint_step[0];

  if (start_pr_time <= 0.0)
  { 
    print_tran(0.0, 0.0);
    output_cnt++;
    last_pr_time = 0.0;
  }
  else last_pr_time = start_pr_time - user_pr_step;

  step_control(-1.0, &newstep, hptr);

  timestep_count = 0;
  loop_count = 0;
  guess_step_count = 0;

  temp_step = newstep;
  guess_step_count++;

  while (guess_next(temp_step, &temp_newstep) != OK)
  {
    temp_step = temp_newstep;
    guess_step_count++; 
  }
  newstep = temp_step;

  check_breakpoint(&newstep);

  while (output_cnt < numsteps)
  {
    if (last_time >= stop_time)
      break;

    for (i = 0; i < STEPSAVE; i++)
      temp_hptr[i + 1] = *(hptr + i);


    repeat:

    if (newstep < MIN_TIME_STEP) 
    {
      printf("step %e time %e\n", newstep, last_time);
      sperror(SMALL_TIME_STEP);
    }

    step = newstep;
    temp_hptr[0] = step;
    
    loop_count++;

    converged = nonlinear_loop(source_only, temp_hptr);  

    if (converged == FALSE)
    {
      newstep = step/2.0;
      guess_step_count++;
      guess_next(newstep, &temp_newstep);
      goto repeat;
    }

    find_dx(temp_hptr);

    if (step_control(step, &newstep, hptr) == OK)
    {

/*
      last_pr_time = last_time;
      user_pr_step = step;
*/

      temp_time = last_pr_time + user_pr_step;

      if ((last_time + step >= temp_time) && (step > 0.0) &&
          (temp_time <= stop_time)) 
      {
        print_tran(temp_time, (temp_time - last_time)/step);
        last_pr_time = temp_time;
        output_cnt++;
      }    

      update_device(&step);

      last_time = last_time + step;

      for (i = STEPSAVE; i > 0; i--)
         *(hptr + i) = *(hptr + i - 1);

      *hptr = step;

      update_unknown();

      temp_step = newstep;
      guess_step_count++;

      while (guess_next(temp_step, &temp_newstep) != OK)
      {
        temp_step = temp_newstep;
        guess_step_count++; 
      }

      if ((temp_step > 0.9*step) && (temp_step < 1.9*step)) 
        newstep = step;
      else newstep = temp_step;

      check_breakpoint(&newstep);

      if (newstep == step) 
      {
        source_only = TRUE;
      }
      else source_only = FALSE;

      timestep_count++;
    }
    else
    {
      source_only = FALSE;
      guess_step_count++;
      guess_next(newstep, &temp_newstep);
    } 

  }    /* while */

  printf("%-19s%6d     %-19s%6d\n","loop count",loop_count,
        "predictor count",guess_step_count);
  printf("%-19s%6d     %-19s%6ld\n","timestep count",timestep_count,
        "LU count",lu_count);
  printf("%-19s%6ld     %-19s%6.3f\n","Solve count",solve_count,
        "solve ratio",lu_solve_ratio);

}    /* time_loop */


int
nonlinear_loop(int source_only, double *hptr)
{
  int converged, nonlinear_count;
  int temp;
  int need_lu;

  nonlinear_count = 0;
  converged = NOT_YET;
  
  while (converged == NOT_YET)
  {
    if (source_only == FALSE) matrix_clear();
    
    if ((source_only == FALSE) || (nonlinear_count == 0))
    {
      clear_source();
      need_lu = matrix_iteration_update(source_only, hptr);
    }
    else
    {
      clear_nonlin_source();
      need_lu = matrix_nonlin_iteration_update(source_only, hptr);
    }

    if ((source_only == FALSE) || (need_lu == TRUE))
    {
      lu_count++;
      last_solve_count = 0;
      LU_refresh();

      if (lu_count <= 1)
      {
        A_LU(A_matrix, PIVOT_FULL, FALSE);
      }
      else 
        A_LU(A_matrix, PIVOT_ROW, FALSE);
    }
    
    solve_Ly_b(A_matrix, x_unknown, b_source);
    solve_Ux_y(A_matrix, x_unknown);
    
    last_solve_count++;
    solve_count++;
  
    find_phi(hptr);

    converged = converge();
    nonlinear_count++;
    if ((converged == NOT_YET) && (nonlinear_count <= NONLIN_MAX))
    {
      source_only = TRUE;
      iteration_update();
    }
    else if (converged == NOT_YET)
    {
      converged = FALSE;
    }
  }     /* while */

  return(converged);

}    /* nonlinear_loop */


int
guess_next(double step, double *newstep)
{
  long i;
  int jj_status, flux_status;
  double jj_newstep, flux_newstep;

  jj_status = jj_apriori_step_limit(step, &jj_newstep);

  flux_status = ind_apriori_step_limit(step, &flux_newstep);

  if ((jj_status == OK) && (flux_status == OK))
  {
    *newstep = step;

    for (i=0; i <= eqn_count; i++)
    if ((x_unk_copy[i]->type == VOLT_TIME) ||
        (x_unk_copy[i]->type == AMP_TIME))
    {
      x_unk_copy[i]->xn_guess = *(x_unk_copy[i]->xpast) +
                                x_unk_copy[i]->xdpast * step;  
    }

    return(OK);
  }
  else
  {
    *newstep = mymin(jj_newstep, flux_newstep);
    return(TOO_BIG);
  }

}  /* guess_next */


int
converge()
{
  long i;
  int converged;
  double delta, xn, xguess;
  device *temp_dev;
  dev_jj *temp_jj;
  double temp_vn;

/* SRW ** Note:  Only junction phase is convergence tested, unlike
 * jspice3 which also checks junction voltage.  Setting the following
 * parameters and using the first block commented out below
 * is equivalent to the default jspice3	test.  This about doubles
 * the solve count.
 *
 *	vtran_min = 1e-12;
 *	vtran_rel = 1e-3;
 *
 * defaults:
 *  vtran_min = 1e-7
 *  vtran_rel = 1e-4
 *  phi_min	= 1e-2
 *  phi_rel = 1e-3
 */

/*

  temp_dev = jj;  

  while (temp_dev != NULL)
  {
    temp_jj = (dev_jj *) temp_dev->data;
    if ((temp_jj->n_plus >= 0) && (temp_jj->n_minus >= 0))
    {
      xn = x_unk_copy[temp_jj->n_plus]->xn - 
           x_unk_copy[temp_jj->n_minus]->xn;
      xn = myabs(xn);
      xguess = myabs(temp_jj->vn_guess);
      delta = myabs(xn - xguess);

      if (delta > vtran_min + vtran_rel*mymin(xn, xguess))
      {
        return(NOT_YET);
      }
    }
 
    temp_dev = temp_dev->next_dev;

  }

  for (i = 0; i <= eqn_count; i++)
  if ((x_unk_copy[i]->type == VOLT_TIME) ||
      (x_unk_copy[i]->type == AMP_TIME))
  {
    xn = myabs(x_unk_copy[i]->xn);
    xguess = myabs(x_unk_copy[i]->xn_guess);
    delta = myabs(xn - xguess);

    if (x_unk_copy[i]->type == VOLT_TIME)
    {
      if (delta > vtran_min + vtran_rel*mymin(xn, xguess))
      {
        return(NOT_YET);
       }
    }
    else if (delta > itran_min + itran_rel*mymin(xn, xguess))
    {
        return(NOT_YET);
    }
  }

*/
    

  for (i = phi_start; i <= phi_end; i++)
  {
    xn = (x_unknown[i]->xn);
    xguess = (x_unknown[i]->yn);
    delta = myabs(xn - xguess);

    if (delta > phi_quit) return(FALSE);
    if (delta > phi_min + phi_rel*mymin(myabs(xn), myabs(xguess)))
    {
      return(NOT_YET);
    }
  }


  return(TRUE);

}  /* converge */
   

breakpoint_node *bkpoint_tree;
int temp_bkpt_count;

double temp_stime, temp_ftime, temp_step;
int donot_add;
breakpoint_node *current_bkpt, *parent_bkpt;
double last_bkpt_step;


int
in_bkpoint(breakpoint_node *current, double time)
{

  if ((time >= current->stime) && (time < current->ftime))
    return(TRUE);
  return(FALSE);

}  /* in_bkpoint */


void
init_breakpoint(double stime, double ftime, double step)
{
  bkpoint_tree = (breakpoint_node *) 
                   mycalloc(1, sizeof(breakpoint_node));

  bkpoint_tree->stime = stime;
  bkpoint_tree->ftime = ftime;
  bkpoint_tree->step = step;
  bkpoint_tree->left = NULL;
  bkpoint_tree->right = NULL;

  bkpoint_count = 1;

}  /* init_breakpoint */


void
add_breakpoint(double stime, double ftime, double step)
{
  int finished;
  breakpoint_node *temp_node;
  
  finished = FALSE;
  temp_stime = stime;
  temp_ftime = ftime;
  temp_step = step;

  while (finished == FALSE)
  {
    parent_bkpt = NULL;
    current_bkpt = bkpoint_tree;

    find_left_bkpoint();

    if (donot_add == TRUE)
    {
      if (temp_stime >= temp_ftime) finished = TRUE;
    }
    else 
    {
      if (temp_stime > current_bkpt->stime)
      {
        temp_node = (breakpoint_node *) 
                      mycalloc(1, sizeof(breakpoint_node));

        temp_node->stime = temp_stime;
        temp_node->ftime = current_bkpt->ftime;
        temp_node->step = current_bkpt->step;
        temp_node->left = current_bkpt;
        temp_node->right = current_bkpt->right;

        current_bkpt->ftime = temp_stime;
        current_bkpt->right = NULL;

        if (parent_bkpt != NULL) 
        {
          if (temp_node->ftime <= parent_bkpt->stime)
            parent_bkpt->left = temp_node;
          else parent_bkpt->right = temp_node;
        }
        else bkpoint_tree = temp_node;

        current_bkpt = temp_node;

      }

      if (temp_ftime == current_bkpt->ftime)
      {
        current_bkpt->step = temp_step;
        finished = TRUE;
      }
      else if (temp_ftime > current_bkpt->ftime)
      {
        current_bkpt->step = temp_step;
        stime = current_bkpt->ftime;
      }
      else
      {
        temp_node = (breakpoint_node *)
                      mycalloc(1, sizeof(breakpoint_node));
          
        temp_node->stime = temp_ftime;
        temp_node->ftime = current_bkpt->ftime;
        temp_node->step = current_bkpt->step;
        temp_node->left = NULL;
        temp_node->right = current_bkpt->right;

        current_bkpt->ftime = temp_ftime;
        current_bkpt->step = temp_step;
        current_bkpt->right = temp_node;

        finished = TRUE;
 
      }  
 
    }

  }  /* while */
        

}  /* add_breakpoint */


void
find_left_bkpoint()
{

  int found;
 
  found = FALSE;
  donot_add = FALSE;
 
  while (found == FALSE)
  if (in_bkpoint(current_bkpt, temp_stime) == TRUE) 
  {
    if (temp_step >= current_bkpt->step)
    {
      temp_stime = current_bkpt->ftime;
      donot_add = TRUE;
    }
    found = TRUE;
  }
  else
  { 
    /* SRW */
    if (temp_stime < current_bkpt->stime)
    {
      if (current_bkpt->left != NULL)
      {
        parent_bkpt = current_bkpt;
        current_bkpt = current_bkpt->left;
      }
      else break;
    }
    else
    {
      if (current_bkpt->right != NULL)
      {
        parent_bkpt = current_bkpt;
       current_bkpt = current_bkpt->right;
      }
      else break;
    }
  }

}  /* find_left_bkpoint */


void
bkpttree_to_array()
{
  bkpoint_count = 0;
  last_bkpt_step = -1.0;
  get_bkpt_subcount(bkpoint_tree);

  bkpoint_time = (double *) mycalloc(bkpoint_count, sizeof(double));
  bkpoint_step = (double *) mycalloc(bkpoint_count, sizeof(double));

  temp_bkpt_count = 0;
  last_bkpt_step = -1.0;
  get_bkpt_subarray(bkpoint_tree);

  free_bkpt_tree(&bkpoint_tree);

}


void
get_bkpt_subcount(breakpoint_node *temp_node)
{

  if (temp_node->left != NULL) get_bkpt_subcount(temp_node->left); 
  if (temp_node->step != last_bkpt_step)
  {
    bkpoint_count++;
    last_bkpt_step = temp_node->step;
  }
  if (temp_node->right != NULL) get_bkpt_subcount(temp_node->right);

}  /* get_bkpt_subcount */
  

void
get_bkpt_subarray(breakpoint_node *temp_node)
{

  if (temp_node->left != NULL) get_bkpt_subarray(temp_node->left); 

  if (temp_node->step != last_bkpt_step)
  {
    bkpoint_time[temp_bkpt_count] = temp_node->stime;
    bkpoint_step[temp_bkpt_count] = temp_node->step;
    temp_bkpt_count++;
    last_bkpt_step = temp_node->step;
  }

  if (temp_node->right != NULL) get_bkpt_subarray(temp_node->right);

}  /* get_bkpt_subarray */
  

void
free_bkpt_tree(breakpoint_node **temp_node)
{

  if ((*temp_node)->left != NULL) 
    free_bkpt_tree(&((*temp_node)->left)); 
  if ((*temp_node)->right != NULL) 
    free_bkpt_tree(&((*temp_node)->right));

  free((char *) *temp_node);
  *temp_node = NULL;

}  /* get_bkpt_subarray */
  

void
print_bkpt_subtree(breakpoint_node *temp_node)
{
 
  if (temp_node->left != NULL) print_bkpt_subtree(temp_node->left); 
  printf("%6.1f %6.1f %6.1f\n", temp_node->stime, temp_node->ftime,
                       temp_node->step);
  if (temp_node->right != NULL) print_bkpt_subtree(temp_node->right);
}


void
print_bkpt_tree()
{
  print_bkpt_subtree(bkpoint_tree);
}


void
print_bkpt_array()
{
  int i;

  for (i = 0; i < bkpoint_count; i++)
    printf("%6.1f %6.1f\n", bkpoint_time[i], bkpoint_step[i]);

}


void
find_dx(double *hptr)
{
  int i;

  if (*(hptr + 1) > 0.0)
  for (i = 0; i <= eqn_count; i++)
  {
    if ((x_unknown[i]->type == VOLT_TIME) ||
        (x_unknown[i]->type == AMP_TIME)) 
    x_unknown[i]->yn = 
     2.0*(x_unknown[i]->xn - *(x_unknown[i]->xpast))/(*hptr) - 
          x_unknown[i]->xdpast;
  }
  else
  for (i = 0; i <= eqn_count; i++)
  {
    if ((x_unknown[i]->type == VOLT_TIME) ||
        (x_unknown[i]->type == AMP_TIME)) 
    x_unknown[i]->yn = 
     (x_unknown[i]->xn - *(x_unknown[i]->xpast))/(*hptr); 
  }
}    /* find_dx */


void
find_phi(double *hptr)
{
  int i;
  device *temp_dev;
  dev_jj *temp_jj;
  double xn, xn1, yn, yn1;
  
  temp_dev = jj;

  while (temp_dev != NULL)
  {
    temp_jj = (dev_jj *) temp_dev->data;
    
    xn = x_unk_copy[temp_jj->n_plus]->xn;
    xn1 = *(x_unk_copy[temp_jj->n_plus]->xpast);

    yn = x_unk_copy[temp_jj->n_minus]->xn;
    yn1 = *(x_unk_copy[temp_jj->n_minus]->xpast);

    x_unknown[temp_jj->n_phi]->xn = *(x_unknown[temp_jj->n_phi]->xpast)
         + E_HBAR*(*hptr)*(xn - yn + xn1 - yn1);  
   
    temp_dev = temp_dev->next_dev;
  }

}  /* find_phi */


void
update_unknown()
{
  int i, j;
  double temp;

  for (i = 0; i <= eqn_count; i++)
  if ((x_unknown[i]->type == VOLT_TIME) ||
      (x_unknown[i]->type == AMP_TIME)) 
  {
    for (j = STEPSAVE - 1; j > 0; j--)
    {
      *(x_unknown[i]->xpast + j) = *(x_unknown[i]->xpast + j - 1);
    }
    
    *(x_unknown[i]->xpast) = x_unknown[i]->xn;
    x_unknown[i]->xdpast = x_unknown[i]->yn;
  }
  else 
  {
    *(x_unknown[i]->xpast) = x_unknown[i]->xn;
  }

  for (i = phi_start; i <= phi_end; i++)
  {
    j = 1;
    temp = x_unknown[i]->xn; 
    *(x_unknown[i]->xpast + 1) = *(x_unknown[i]->xpast + 1) +
                                 (temp - *(x_unknown[i]->xpast));

    if (temp > PI2)
    {
      while (temp > j*PI2) j++;
      temp = temp - (j - 1)*PI2;
    }
    else if (temp < 0.0 - PI2)
    {
      while (temp < 0.0 - j*PI2) j++;
      temp = temp + (j - 1)*PI2;
    }

    *(x_unknown[i]->xpast) = temp;
  }

}   /* update_unknown */


void
iteration_update()
{
  long i;

  jj_iteration_update();

  for (i = 0; i <= eqn_count; i++)
  {
    x_unk_copy[i]->xn_guess = x_unk_copy[i]->xn;
  } 

  for (i = phi_start; i <= phi_end; i++)
    x_unknown[i]->yn = x_unknown[i]->xn;

}   /* iteration_update */


int
matrix_iteration_update(int source_only, double *hptr)
{
  int temp;
  int need_lu;
  
  jj_fix_trap(source_only, &need_lu, hptr);
  resis_trap(source_only);
  cap_trap(source_only, hptr);
  ind_trap(source_only, hptr);
  mut_trap(source_only, hptr);
  transline_trap(source_only, hptr);
  vs_advance(hptr);
  is_advance(hptr);

  return(need_lu);

}  /* matrix_iteration_update */


int
matrix_nonlin_iteration_update(int source_only, double *hptr)
{
  int need_lu;
  
  jj_fix_trap(source_only, &need_lu, hptr);
  return(need_lu);


}  /* matrix_iteration_update */


void
update_device(double *hptr)
{

  update_transline(hptr);
  update_jj();

}  /* update_device */


double
get_pr_jjic(double inc_rate, dev_jj *temp_jj)
{
  
  return(temp_jj->ic_past + 
         (temp_jj->ic_current - temp_jj->ic_past) * inc_rate);

}  /* get_pr_jjic */


double
get_pr_val(double inc_rate, long which_eqn, int which_val)
{
  double valn, valn1;
  unknown *x;

  if (which_eqn <= eqn_count) x = x_unk_copy;
  else x = x_unknown;

  if (which_val == 0)
  {
    valn = x[which_eqn]->xn;
    valn1 = *(x[which_eqn]->xpast);
  }
  else  
  {
    valn = x[which_eqn]->yn;
    valn1 = x[which_eqn]->xdpast;
  }

  return(valn1 + (valn - valn1) * inc_rate);

}  /* get_pr_val */


void
print_tran(double the_time, double inc_rate)
{
  int i;
  printdata *temp;
  double plus_val, minus_val;
  FILE *fptmp;

  temp = print_list;

  if (temp == NULL) return;

  if (jsim_raw) {
    fprintf(rawfp,"%d",raw_cntr);
    fprintf(rawfp,out_format,the_time);
    raw_cntr++;
  }
  else {
    for (i = 0; i <= file_count; i++)
        fprintf(fileptr[i], out_format, the_time);

    if (fileptr[MAXFILE] != NULL)
        fprintf(stdout, out_format, the_time);
  }

  while (temp != NULL) {

    if (jsim_raw)
      fptmp = rawfp;
    else
      fptmp = temp->fp;

    if (temp->is_dev == FALSE) {

      plus_val = get_pr_val(inc_rate, temp->plus, 0);
      minus_val = get_pr_val(inc_rate, temp->minus, 0);
      fprintf(fptmp, out_format, plus_val - minus_val);
    }
    else {
      /* is_dev is TRUE */ 

      switch(temp->print_dev->type) {
        case RESIS :
 
             resis_tran_print(fptmp, inc_rate, temp->prtype, 
                              temp->print_dev);
             break;

        case CAP :
 
             cap_tran_print(fptmp, inc_rate, temp->prtype, 
                            temp->print_dev);
             break;

        case INDUCT :
 
             ind_tran_print(fptmp, inc_rate, temp->prtype, 
                            temp->print_dev);
             break;

        case DC_V :
        case SIN_V :
        case PULSE_V :
        case PWL_V :
        case V_SOURCE :
 
             vs_tran_print(fptmp, inc_rate, temp->prtype, 
                            temp->print_dev);
             break;

        case SIN_I :
        case PULSE_I :
        case PWL_I :
        case I_SOURCE :
 
             is_tran_print(fptmp, inc_rate, temp->prtype, 
                            temp->print_dev, the_time);
             break;

        case JJ :
 
             jj_tran_print(fptmp, inc_rate, temp->prtype, 
                            temp->subtype, temp->print_dev);
             break;

        case TRAN_NO_LOSS :
 
             xline_tran_print(fptmp, inc_rate, temp->prtype, 
                            temp->subtype, temp->print_dev);
             break;

      }  /* switch */
    }

    temp = temp->next_print;

  }   /* while */

  if (jsim_raw == FALSE) {
    for (i = 0; i <= file_count; i++)
      fprintf(fileptr[i], "\n");

    if (fileptr[MAXFILE] != NULL) fprintf(stdout, "\n");
  }

}   /* print_tran */
