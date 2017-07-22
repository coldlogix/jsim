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
#include <time.h>

static FILE *dbfp;
FILE *rawfp;
int raw_cntr;


void
init_global()
{
  int i;
  FILE *fopen();

  for (i = 0; i <= MAXFILE; i++) fileptr[i] = NULL;
  file_count = -1;
  lu_opcount = 0;
  mod_count = 0;
  eqn_count = -1;
  phi_count = 0;
  lu_count = 0;
  solve_count = 0;
  node_count = 0;
  subnode_count = 0;
  user_step = 0.0 - 1.0;
  user_pr_step = 0.0 - 1.0;
  start_pr_time = 0.0;
  stop_time = 0.0 - 1.0; 
  last_time = 0.0;
  last_pr_time = 0.0;


  no_truncation_error = TRUE;
  no_flux_control = TRUE;
  igwarn_no_go = FALSE;

  hptr[0] = hptr[1] = -1.0;

  ground_node = GROUND;

  init_default_global();
  init_dev_global();

}  /* init_global */


void
init_default_global()
{

  vtran_min = DEF_VTRAN_MIN;
  vtran_rel = DEF_VTRAN_REL;

  itran_min = DEF_ITRAN_MIN;
  itran_rel = DEF_ITRAN_REL;

  max_phi_step = DEF_MAX_PHI_STEP;
  phi_min = DEF_PHI_MIN;
  phi_rel = DEF_PHI_REL;
  phi_quit = DEF_PHI_QUIT;

  max_flux_step = DEF_MAX_FLUX_STEP;

  error_tolerance = DEF_ERROR_TOLERANCE;

  nu_digit = DEF_NU_DIGIT;

  vs_max_inc = DEF_VS_MAX_INC;
  vs_min_inc = DEF_VS_MIN_INC;
  is_max_inc = DEF_IS_MAX_INC;
  is_min_inc = DEF_IS_MIN_INC;
  nu_min_sample = DEF_NU_MIN_SAMPLE;
  nu_segment = DEF_SEGMENT_MIN;

  vn_guess_min = DEF_VN_GUESS_MIN;

}  /* init_default_global */


void
init_dev_global()
{

  resis = resis_tail = NULL;
  resis_count = 0;
  resis_array = NULL;

  cap = cap_tail = NULL;
  cap_count = 0;
  cap_array = NULL;

  ind = ind_tail = NULL;
  ind_count = 0;
  ind_array = NULL;
  fcheck_list = fcheck_tail = NULL;

  isource = isource_tail = NULL;
  isource_count = 0;
  isource_array = NULL;

  jj = jj_tail = NULL;
  jj_count = 0;
  jj_array = NULL;

  mut = mut_tail = NULL;
  mut_count = 0;
  mut_array = NULL;

  vsource = vsource_tail = NULL;
  vsource_count = 0;
  vsource_array = NULL;

  xline = xline_tail = NULL;
  xline_count = 0;
  xline_array = NULL;

  sub_ckt = sub_ckt_tail = NULL;
  sub_ckt_count = 0;
  sub_ckt_array = NULL;

  sub_def_tree = sub_def_tail = current_sub_def = NULL;
  sub_def_count = 0;
  sub_def_array = NULL;

  namelist = namelist_tail = NULL;
  dev_count = 0;
  realname_array = name_array = NULL;

  jsim_model = NULL;
  mod_count = 0;
  jsim_model_array = NULL;
  jsim_model_tail = NULL;

  print_list = print_tail = NULL;

  init_node_map();

}  /* init_dev_global */


void
init_A(matrix *A)
{
  long i;
  
  A->rowptr = (m_data_ptr *) mycalloc(eqn_count + 1, sizeof(m_data_ptr));
  A->colptr = (m_data_ptr *) mycalloc(eqn_count + 1, sizeof(m_data_ptr));
  A->diagptr = (m_data_ptr *) mycalloc(eqn_count + 1, sizeof(m_data_ptr));
  A->r_order = (int *) mycalloc(eqn_count + 1, sizeof(int));
  A->c_order = (int *) mycalloc(eqn_count + 1, sizeof(int));
  A->r_count = (int *) mycalloc(eqn_count + 1, sizeof(int));
  A->c_count = (int *) mycalloc(eqn_count + 1, sizeof(int));

  for (i = 0; i <= eqn_count; i++)
  {
    A->rowptr[i] = A->colptr[i] = NULL;
    A->r_order[i] = A->c_order[i] = i;
    A->r_count[i] = A->c_count[i] = 0;
  }

  A->dim = 0;
  A->lu_stop = 0;

}    /* init_A */


void
init_source(source *b)
{
  long i;
 
  for (i = 0; i <= eqn_count; i++) b[i] = NULL;

}   /* init_source */


void
init_unknown(unknown *x)
{
  long i;
   
  for (i = 0; i <= eqn_count + jj_count + 1; i++) 
  {
    x[i] = (x_data *) mycalloc(1, sizeof(x_data));
    x[i]->type = VOLT;
  }

}   /* init_unknown */


void
init_unk_copy(unknown *x, unknown *xc)
{
  long i;
   
  for (i = 0; i <= eqn_count + 1; i++) xc[i] = x[i];

}   /* init_unk_copy */


void
init_b_src_copy(source *b, source *bc)
{
  long i;
   
  for (i = 0; i <= eqn_count; i++) bc[i] = b[i];

}   /* init_b_src_copy */


void
init_node_map()
{
  int i;

  for (i = 0; i <= RANGE; i++) node_map[i] = NULL;

}  /* init_node_map */


extern FILE *fp;


void
read_deck()
{
  int i;
  char c;
  long np, vp, place;
  printdata *pl;

  current_fp = stdout;

  if (jsim_raw) {
    raw_cntr = 1;
    if (!rawfp) {
      rawfp = fopen("jsim.raw","w");
      if (!rawfp) {
  	    printf("## Error -- can't open output file\n");
  	    no_go = TRUE;
        return;
      }
    }

    if (readline(linesave) == EOF) {
  	  printf("## Error -- premature EOF in input\n");
  	  no_go = TRUE;
      return;
    }
    if (*linesave == '*')
  	  fprintf(rawfp,"Title: %s\n", linesave+1);
    else
  	  fprintf(rawfp,"Title: %s\n", "Generic JSIM run");
    fprintf(rawfp,"Date: %s\n",datestring());
    fprintf(rawfp,"Plotname: %s\n","transient analysis");
    fprintf(rawfp,"Flags: real\n");
    fprintf(rawfp,"No. Variables: ");
    vp = ftell(rawfp);
    fprintf(rawfp,"0       \n");
    fprintf(rawfp,"No. Points: ");
    np = ftell(rawfp);
    fprintf(rawfp,"0       \n");
    fprintf(rawfp,"Variables: 0 time time\n");
  }
  else {
    if (readline(linesave) == EOF) {
  	  printf("## Error -- premature EOF in input\n");
      no_go = TRUE;
      return;
    }
  }

  do {

    ignore_separator(linesave);
    line = (char *) linesave;

    read_error = read_string("");
    if ((read_error == OK) && (*tempstring != '.'))
      dev_name = new_string(tempstring); 
    if (read_error == OK)
    switch(get_string_type(tempstring))
    {
      case RESIS :    read_resis(); break;
      case CAP :      read_cap(); break;
      case INDUCT :   read_ind(); break;
      case JJ :       read_jj(); break;
      case MUTUAL_L : read_mut(); break;
      case V_SOURCE : read_vs(); break;
      case I_SOURCE : read_is(); break;
      case TRANSLINE : read_transline(); break;
      case SUB_C :    read_sub_ckt(); break;
      case CONTROL :

           switch(get_string_keyword(tempstring))
           {
             case MODEL :  read_model(); break;
             case TRAN :   read_tran(); break;
             case PRINT :  read_print(); break;
             case OPTIONS : read_option(); break;
             case NEWFILE : read_file(); break;
             case SUBCKT : read_sub_def(); break;
             case ENDS : 

                  if (current_sub_def == NULL)
                    printf("mismatching end\n");  
                  else if (current_sub_def->parent != NULL)
                    current_sub_def = current_sub_def->parent;
                  else current_sub_def = NULL;
                  break;

             case ILLEGAL : 

                  printf("## Error -- illegal control card\n");
                  no_go = TRUE;
                  break;
           }
           break;

      case ILLEGAL : 

           printf("## Error -- illegal input card\n");
           no_go = TRUE;
           break;
    }

  } while (readline(linesave) != EOF);

  if (jsim_raw)
    strcpy(out_format,"\t%.15e\n");
  else
    sprintf(out_format, "%c.%1de %c", '%', nu_digit,'\0');


  for (i = 0; i <= file_count; i++) free(filename[i]);

  if (jsim_raw) {

    for (pl = print_list; pl; pl = pl->next_print, raw_cntr++) {

      if (pl->is_dev == FALSE) {
        fprintf(rawfp," %d V(%d,%d) voltage\n",
          raw_cntr,pl->plus,pl->minus);
        continue;
      }

      if (*pl->dev_name == 'b' || *pl->dev_name =='B') {

        if (pl->prtype == VOLT)  {
          fprintf(rawfp," %d %s.V voltage\n",raw_cntr,pl->dev_name);
          continue;
        }
        if (pl->prtype == PHI) {
          fprintf(rawfp," %d %s.P phase\n",raw_cntr,pl->dev_name);
          continue;
        }
        if (pl->subtype == JJALL) {
          fprintf(rawfp," %d %s.I current\n",raw_cntr,pl->dev_name);
          raw_cntr++;
          fprintf(rawfp," %d %s.Ij current\n",raw_cntr,pl->dev_name);
          raw_cntr++;
          fprintf(rawfp," %d %s.Ic current\n",raw_cntr,pl->dev_name);
          raw_cntr++;
          fprintf(rawfp," %d %s.Ir current\n",raw_cntr,pl->dev_name);
          continue;
        }
        if (pl->subtype == JJJOSEPH) {
          fprintf(rawfp," %d %s.Ij current\n",raw_cntr,pl->dev_name);
          continue;
        }
        if (pl->subtype == JJCAP) {
          fprintf(rawfp," %d %s.Ic current\n",raw_cntr,pl->dev_name);
          continue;
        }
        if (pl->subtype == JJRESIS)  {
          fprintf(rawfp," %d %s.Ir current\n",raw_cntr,pl->dev_name);
          continue;
        }
        fprintf(rawfp," %d %s.I current\n",
          raw_cntr,pl->dev_name);
        continue;
      }

      if (*pl->dev_name == 't' || *pl->dev_name =='T') {

        if (pl->subtype == PORT1) {
          if (pl->prtype == VOLT)
            fprintf(rawfp," %d %s.V1 voltage\n",raw_cntr,pl->dev_name);
          else
            fprintf(rawfp," %d %s.I1 current\n",raw_cntr,pl->dev_name);
          continue;
        }
        if (pl->subtype == PORT2) {
          if (pl->prtype == VOLT)
            fprintf(rawfp," %d %s.V2 voltage\n",raw_cntr,pl->dev_name);
          else
            fprintf(rawfp," %d %s.I2 current\n",raw_cntr,pl->dev_name);
          continue;
        }
        if (pl->prtype == VOLT)  {
          fprintf(rawfp," %d %s.V1 voltage\n",raw_cntr,pl->dev_name);
          raw_cntr++;
          fprintf(rawfp," %d %s.V2 voltage\n",raw_cntr,pl->dev_name);
        }
        else {
          fprintf(rawfp," %d %s.I2 current\n",raw_cntr,pl->dev_name);
          raw_cntr++;
          fprintf(rawfp," %d %s.I2 current\n",raw_cntr,pl->dev_name);
        }
        continue;
      }

      if (pl->prtype == VOLT)
        fprintf(rawfp," %d %s.V voltage\n",raw_cntr,pl->dev_name);
      else
        fprintf(rawfp," %d %s.I current\n",raw_cntr,pl->dev_name);
    }

    place = ftell(rawfp);
    fseek(rawfp,vp,0);
    fprintf(rawfp,"%-7d",raw_cntr);

    i = 1.5 + (stop_time - start_pr_time)/user_pr_step;

    fseek(rawfp,np,0);
    fprintf(rawfp,"%-7d",i);

    fseek(rawfp,place,0);
    raw_cntr = 0;
    fprintf(rawfp,"Values:\n");

  } /* jsim_raw */

}   /* read_deck */


char *
datestring()
{
	static char s[20];
	struct tm *t;
	time_t secs;

	time(&secs);
	t = localtime(&secs);

	sprintf(s,"%02d-%02d-%4d %02d:%02d:%02d",t->tm_mon+1,t->tm_mday,
		t->tm_year+1900,t->tm_hour,t->tm_min,t->tm_sec);
	return s;
}


void
read_option()
{
  int type;

  while ((read_error = read_string("")) == OK)
  {
    switch(get_string_keyword(tempstring))
    {
      case VNTOL : read_error = read_double(&vtran_min, 
                                "voltage tolerance", FALSE);
                   break;

      case INTOL : read_error = read_double(&itran_min,
                                "current tolerance", FALSE);
                   break;

      case PHITOL : read_error = read_double(&phi_min,
                                "phase tolerance", FALSE);
                   break;

      case VRELTOL : read_error = read_double(&vtran_rel,
                                 "voltage relative tolerance", FALSE);
                     break;
                     
      case IRELTOL : read_error = read_double(&itran_rel,
                                 "current relative tolerance", FALSE);
                     break;

      case PHIRELTOL : read_error = read_double(&phi_rel,
                                 "phase relative tolerance", FALSE);
                       break;

      case RELTOL : read_error = read_double(&phi_rel,
                                "relative tolerance", FALSE);
                    if (read_error == OK)
                    {
                      itran_rel = vtran_rel = phi_rel;
                    }
                    break;
  
      case MAXPHISTEP : read_error = read_double(&max_phi_step,
                                     "max phase increment", FALSE);
                        break;

      case PHIQUIT : read_error = read_double(&phi_quit,
                                    "phin quit", FALSE);
                     break;

      case MAXFLUXSTEP : read_error = read_double(&max_flux_step,
                                      "max flux increment", FALSE);
                         break;

      case LTE : no_truncation_error = FALSE; break;
   
      case NUMDGT : read_error = read_int(&nu_digit, 
                                 "number of digit", FALSE);
                    break;

      case NUSAMPLE : read_error = read_int(&nu_min_sample,
                                            "number of sample", 
                                            FALSE);
                      break;

      case NUSEGMENT : read_error = read_int(&nu_segment,
                                             "number of segment", 
                                             FALSE);
                       break;

      case IGWARN : igwarn_no_go = TRUE;
                    break;

      default : 
           printf("## Warning -- Illegal keyword %s\n", tempstring);
           break;

    }  /* switch */  

  }    /* while */

}  /* read_option */


void
process_deck()
{
  if (jsim_dbg) {
    if (!dbfp)
      dbfp = fopen("jsim.dbg","w");
  }

  print_deftree(sub_def_tree);

  sub_def_array = get_sub_def_array(sub_def_tree, sub_def_count);
  do_sub_ckt();

  get_arrays();

  print_realname_name();
  print_modlist();

  do_dependent();

  get_model();

  print_node_map();

  get_node_array();

  ground_node = eqn_count + 1;
  do_print();

  print_devlist();

}  /* process_deck */


void
get_arrays()
{
  get_realname_array();
  get_name_array();

  resis_array = get_dev_array(TRUE, resis, resis_count); 
  cap_array = get_dev_array(TRUE, cap, cap_count); 
  ind_array = get_dev_array(TRUE, ind, ind_count); 
  jj_array = get_dev_array(TRUE, jj, jj_count); 
  vsource_array = get_dev_array(TRUE, vsource, vsource_count); 
  isource_array = get_dev_array(TRUE, isource, isource_count); 
  mut_array = get_dev_array(TRUE, mut, mut_count); 
  xline_array = get_dev_array(TRUE, xline, xline_count);
  sub_ckt_array = get_dev_array(FALSE, sub_ckt, sub_ckt_count); 
    
  get_model_array();

}  /* get_arrays */


void
do_dependent()
{

  mut_dependent();
  jj_dependent(); 
    
}     /* do_dependent */


void
get_model()
{

  get_jjmodel();

}   /* get_model */


void
free_space()
{

  free_devarray();
  free_name();
  free_def_tree();

}  /* free_space */


void
setup_device()
{

  setup_transline();

}   /* setup_device */


void
setup_x_unknown()
{
  int i, j;
  int tempint;

  for (i = 0; i <= eqn_count; i++)
  {
    x_unknown[i]->xpast = (double *) mycalloc(STEPSAVE, sizeof(double));

    for (j = 0; j < STEPSAVE; j++)
    {
      *(x_unknown[i]->xpast + j) = 0.0;
    }
    x_unknown[i]->xdpast = 0.0;
  }
  

  for (i = phi_start; i <= phi_end; i++)
  {
    x_unknown[i]->xpast = (double *) mycalloc(2, sizeof(double));
    *(x_unknown[i]->xpast) = 0.0;
    *(x_unknown[i]->xpast + 1) = 0.0;
  }

  x_unknown[ground_node]->xpast = (double *)
             mycalloc(STEPSAVE, sizeof(double));

  for (j = 0; j < STEPSAVE; j++)
  {
    *(x_unknown[ground_node]->xpast + j) = 0.0;
  }
  x_unknown[ground_node]->xdpast = 0.0;

}  /* setup_x_unknown */


void
setup_A_matrix()
{

  resis_matrix();
  cap_matrix();
  ind_matrix();
  jj_matrix();
  mut_matrix();
  trans_matrix();
  vs_matrix();
  is_matrix();

  create_rowptr(A_matrix);

}  /* setup_A_matrix */


void
setup_matrix()
{
  A_matrix = &my_matrix;
  init_A(A_matrix);
  x_unknown = (unknown *) mycalloc(eqn_count + jj_count + 2,
                                   sizeof(unknown));
  x_unk_copy = (unknown *) mycalloc(eqn_count + 2,
                                    sizeof(unknown));
  init_unknown(x_unknown);
  init_unk_copy(x_unknown, x_unk_copy);

  b_source = (source *) mycalloc(eqn_count + 1, sizeof(source));
  init_source(b_source);
  b_src_copy = (source *) mycalloc(eqn_count + 1, sizeof(source));

  A_matrix->dim = eqn_count;
  A_matrix->lu_stop = eqn_count;

  ground_node = eqn_count + 1;
  phi_start = ground_node + 1;
  phi_end = eqn_count + jj_count + 1;

  setup_A_matrix();
  setup_x_unknown();
  init_b_src_copy(b_source, b_src_copy);

}  /* setup_matrix */


void
get_breakpoint()
{

  init_breakpoint(0.0, stop_time, user_step);
  vs_breakpoint();
  is_breakpoint();

  bkpttree_to_array();

}  /* get_breakpoint */


void
topology_check()
{
  cutset_check();
  sourceloop_check(TRUE);
  sourceloop_check(FALSE);
  ground_check(FALSE);
  ground_check(TRUE);

}  /* topology_check */


void
deckerror_check()
{
  long i;
  int repeat_count;

  i = 1;

  while (i < dev_count)
  {
    repeat_count = 0;

    while (i + 1 <= dev_count)
    {
      if (strcmp(((realname_name *) realname_array[i-1])->real_name,
             ((realname_name *) realname_array[i])->real_name) != 0)
        break;
      repeat_count++;
      i++;
    }
    if (repeat_count > 0)
    {
      printf("## Warning -- %s defined %d time(s)\n",
              ((realname_name *) realname_array[i-1])->real_name,
              repeat_count + 1);
      warned = TRUE;
    }

    i++;

  }

}  /* deckerror_check */


void
print_node_map()
{
  int i;
  node_to_eqn *temp;
  branch_list *temp_branch;
  realname_name *temp_name;

  if (jsim_dbg == FALSE) return;
  if (dbfp == NULL) return;

  fprintf(dbfp,"\nnodes:\n");

  for (i = 0; i <= RANGE; i++)
  {
    temp = node_map[i];
    
    while (temp != NULL) 
    {
      fprintf(dbfp, "node %d  eqn %d \n", 
                            temp->node, temp->n_eq);
      temp_branch = temp->branch;
      while (temp_branch != NULL)
      {
        temp_name = search_name(temp_branch->name);
        fprintf(dbfp, "    %s", temp_name->real_name);
        if (temp_branch->devtype == LOSSLESS_LINE)
          fprintf(dbfp, " lossless line");
        fprintf(dbfp, "\n");
        temp_branch = temp_branch->next_branch;
      }
      temp = temp->next_node;
    }
  }
}    /* print_node_map */


void
print_dev_array(long *dev_array, long dev_count)
{

  long i;

  for (i = 0; i < dev_count; i++)
    printf("dev %s %d\n", 
           (char*)((device *) *(dev_array + i))->name,
           strlen((char*)((device *) *(dev_array + i))->name));

}  /* print_dev_array */


void
print_devlist()
{
  int i;
  device *temp;
  dev_resis *temp_resis;
  dev_cap *temp_cap;
  dev_ind *temp_ind;
  dev_jj *temp_jj;
  dev_vsource *temp_vs;
  dev_isource *temp_is;
  dev_mut *temp_mut;
  dev_tran_noloss *temp_tran_noloss;
  dev_sub_ckt *temp_sub_ckt;
  realname_name *temp_name;

  if (jsim_dbg == FALSE) return;
  if (dbfp == NULL) return;
  fprintf(dbfp,"\ndevices:\n");

  temp = resis;

  while (temp != NULL)
  {
    temp_resis = (dev_resis *) temp->data;
    temp_name = search_name(temp->name);

    fprintf(dbfp, "%s %ld %ld %.2e\n", temp_name->real_name, 
            temp_resis->n_plus, temp_resis->n_minus,
                           temp_resis->val);
    temp = temp->next_dev;
  }

  temp = cap;

  while (temp != NULL)
  {
    temp_cap = (dev_cap *) temp->data;
    temp_name = search_name(temp->name);

    fprintf(dbfp, "%s %ld %ld %.2e %.2e\n", temp_name->real_name,
            temp_cap->n_plus, temp_cap->n_minus,
                           temp_cap->val, temp_cap->ic);
    temp = temp->next_dev;
  }

  temp = ind;

  while (temp != NULL)
  {
    temp_ind = (dev_ind *) temp->data;
    temp_name = search_name(temp->name);

    fprintf(dbfp, "%s %ld %ld %ld %.2e %.2e", 
            temp_name->real_name, 
            temp_ind->n_plus, temp_ind->n_minus,
            temp_ind->branch, temp_ind->val, temp_ind->ic);

    if (temp_ind->fcheck == TRUE) 
      fprintf(dbfp, " FCHECK %.2e\n", temp_ind->fvalue);
    else fprintf(dbfp, "\n");
    temp = temp->next_dev;
  }

  temp = jj;

  while (temp != NULL)
  {
    temp_jj = (dev_jj *) temp->data;
    temp_name = search_name(temp->name);

    fprintf(dbfp, "%s %ld %ld %ld %.2e %.2e %.2e ", 
            temp_name->real_name, temp_jj->n_plus, temp_jj->n_minus, 
            temp_jj->n_phi, temp_jj->area, 
            temp_jj->v_ic, temp_jj->phi_ic);

    if (temp_jj->mod != NULL)
      fprintf(dbfp, "model %s", temp_jj->mod->name);
    else
      fprintf(dbfp, "model UNDEFINED");

    if (temp_jj->con_dev != NULL) 
    {
      temp_name = search_name(((device *) temp_jj->con_dev)->name);
      fprintf(dbfp, " condev %s\n", temp_name->real_name);
    }
    else fprintf(dbfp, "\n");

    temp = temp->next_dev;
  }

  temp = vsource;
  
  while (temp != NULL)
  {
    temp_vs = (dev_vsource *) temp->data;
    temp_name = search_name(temp->name);
    fprintf(dbfp, "%s %ld %ld %ld ", temp_name->real_name,
                temp_vs->n_plus, temp_vs->n_minus, temp_vs->branch);
    func_infoprint(dbfp, temp_vs->function, temp_vs->function_parm);
    fprintf(dbfp, "\n");
                    
    temp = temp->next_dev;

  } /* while */

  temp = isource;
  
  while (temp != NULL)
  {
    temp_is = (dev_isource *) temp->data;
    temp_name = search_name(temp->name);
    fprintf(dbfp, "%s %ld %ld ", temp_name->real_name,
                temp_is->n_plus, temp_is->n_minus);
    func_infoprint(dbfp, temp_is->function, temp_is->function_parm);
    fprintf(dbfp, "\n");

    temp = temp->next_dev;
  }

  temp = mut;
  
  while (temp != NULL)
  {
    temp_mut = (dev_mut *) temp->data;
    temp_name = search_name(temp->name);

    fprintf(dbfp, "%s %s %ld %s %ld %.2e\n", 
                                    temp_name->real_name, 
                                    temp_mut->primary_name,
                                    temp_mut->n_primary,
                                    temp_mut->second_name,
                                    temp_mut->n_second, 
                                    temp_mut->val);    
    temp = temp->next_dev;
  }

  temp = xline;

  while (temp != NULL)
  {
    if (temp->type == TRAN_NO_LOSS)
    {
      temp_tran_noloss = (dev_tran_noloss *) temp->data;
      temp_name = search_name(temp->name);

      fprintf(dbfp, "%s LOSSLESS %ld %ld %ld %ld zo %e td %e ",
                  temp_name->real_name,
                  temp_tran_noloss->n1_plus,
                  temp_tran_noloss->n1_minus,
                  temp_tran_noloss->n2_plus,
                  temp_tran_noloss->n2_minus,
                  temp_tran_noloss->zo,
                  temp_tran_noloss->td);
      fprintf(dbfp, "IC %e %e %e %e\n",
                  temp_tran_noloss->v1o,
                  temp_tran_noloss->i1o,
                  temp_tran_noloss->v2o,
                  temp_tran_noloss->i2o);
    }
   
    temp = temp->next_dev;

  }   /* while */
}     /* print_devlist */


void
print_deftree(sub_def *the_sub_def)
{
  if (jsim_dbg == FALSE) return;
  if (dbfp == NULL) return;
  fprintf(dbfp,"\nsubcircuits:\n");
 
  myprint_deftree(the_sub_def, dbfp);

}  /* print_deftree */


void
myprint_deftree(sub_def *the_sub_def, FILE *fp)
{
  int i;
  device *temp;
  sub_def *temp_sub;
  sub_resis *temp_resis;
  sub_cap *temp_cap;
  sub_ind *temp_ind;
  sub_jj *temp_jj;
  sub_vsource *temp_vs;
  sub_isource *temp_is;
  sub_mut *temp_mut;
  sub_tran_noloss *temp_tran_noloss;
  sub_sub_ckt *temp_sub_ckt;


  if (the_sub_def == NULL) return;

  fprintf(fp, "SUBDEF %s ", the_sub_def->name);
  for (i = 0; i < the_sub_def->no_node; i++)
    fprintf(fp, "%d ", *(the_sub_def->node_list + i));
  fprintf(fp, "\n");
  
  temp = the_sub_def->the_sub_ckt;
  while (temp != NULL)
  {
    switch (temp->type)
    {
      case RESIS : temp_resis = (sub_resis *) temp->data;
                   fprintf(fp, "%s %ld (%d) %ld (%d) %.2e\n", 
                           (char *) temp->name, 
                           temp_resis->n_plus, temp_resis->p_node, 
                           temp_resis->n_minus, temp_resis->m_node,
                           temp_resis->val);
                   break;

      case CAP : temp_cap = (sub_cap *) temp->data;
                 fprintf(fp, "%s %ld (%d) %ld (%d) %.2e %.2e\n", 
                         (char *) temp->name,
                         temp_cap->n_plus, temp_cap->p_node,
                         temp_cap->n_minus, temp_cap->m_node,
                         temp_cap->val, temp_cap->ic);
                 break;

      case INDUCT : temp_ind = (sub_ind *) temp->data;
                    fprintf(fp, "%s %ld (%d) %ld (%d) %.2e %.2e\n", 
                            (char *) temp->name, 
                            temp_ind->n_plus, temp_ind->p_node,
                            temp_ind->n_minus, temp_ind->m_node,
                            temp_ind->val, temp_ind->ic);
                    break;

      case JJ : temp_jj = (sub_jj *) temp->data;
                fprintf(fp, "%s %ld (%d) %ld (%d) %ld ",
                        (char *) temp->name,
                        temp_jj->n_plus, temp_jj->p_node,
                        temp_jj->n_minus, temp_jj->m_node,
                        temp_jj->n_phi); 
                fprintf(fp, "%.2e %.2e %.2e model %s",  
                        temp_jj->area,
                        temp_jj->v_ic, temp_jj->phi_ic,
                        (char *) temp_jj->mod);
                if (temp_jj->con_dev != NULL)
                  fprintf(fp, " condev %s\n", 
                          temp_jj->con_dev);
                else fprintf(fp, "\n");
                break;

      case V_SOURCE : 

                  temp_vs = (sub_vsource *) temp->data;
                  fprintf(fp, "%s %ld (%d) %ld (%d) ", 
                          (char *) temp->name,
                          temp_vs->n_plus, temp_vs->p_node,
                          temp_vs->n_minus, temp_vs->m_node);
                  func_infoprint(fp, temp_vs->function,
                                     temp_vs->function_parm);
                  fprintf(fp, "\n");

                  break;
    
      case I_SOURCE : 

                  temp_is = (sub_isource *) temp->data;
                  fprintf(fp, "%s %ld (%d) %ld (%d) ", 
                          (char *) temp->name,
                          temp_is->n_plus, temp_is->p_node,
                          temp_is->n_minus, temp_is->m_node);
                  func_infoprint(fp, temp_is->function,
                                     temp_is->function_parm);
                  fprintf(fp, "\n");

                  break;
    
      case MUTUAL_L : temp_mut = (sub_mut *) temp->data;
                      fprintf(fp, "%s %s %s %.2e\n", 
                              (char *) temp->name,
                              temp_mut->primary_name,
                              temp_mut->second_name,
                              temp_mut->val);
                      break;

      case TRAN_NO_LOSS :
        temp_tran_noloss = (sub_tran_noloss *) temp->data; 
        fprintf(fp, "%s LOSSLESS %ld (%d) %ld (%d) %ld (%d) %ld (%d) ",
                (char *) temp->name,
                temp_tran_noloss->n1_plus, temp_tran_noloss->p1_node,
                temp_tran_noloss->n1_minus, temp_tran_noloss->m1_node,
                temp_tran_noloss->n2_plus, temp_tran_noloss->p2_node,
                temp_tran_noloss->n2_minus, temp_tran_noloss->m2_node);
        fprintf(fp, "zo %e td %e ", 
                    temp_tran_noloss->zo, temp_tran_noloss->td);
        fprintf(fp, "IC %e %e %e %e\n",
                    temp_tran_noloss->v1o, temp_tran_noloss->i1o,
                    temp_tran_noloss->v2o, temp_tran_noloss->i2o);
        break;

      case SUB_C : temp_sub_ckt = (sub_sub_ckt *) temp->data;
                   fprintf(fp, "%s %s ", (char *) temp->name,
                           temp_sub_ckt->sub_def_name);

                   for (i = 0; i < temp_sub_ckt->no_node; i++)
                     fprintf(fp, "%ld (%ld) ", 
                             *(temp_sub_ckt->node_list + i),
                             *(temp_sub_ckt->node_node + i));
                   fprintf(fp, "\n"); 
                   break;
    } /* switch */  

    temp = temp->next_dev; 

  } /* while */
    
  temp_sub = the_sub_def->sub_sub;
  if (temp_sub != NULL) myprint_deftree(temp_sub, fp);

  fprintf(fp, "ENDSUB %s\n", the_sub_def->name);

  if (the_sub_def->next_sub != NULL)
    myprint_deftree(the_sub_def->next_sub, fp);

}     /* myprint_deftree */


void
print_modlist()
{
  mod_jj *temp_jj;
  modeldata *temp_model;
  int i;

  if (jsim_dbg == FALSE) return;
  if (dbfp == NULL) return;
  fprintf(dbfp,"\nmodels:\n");

  temp_model = jsim_model;
  while (temp_model != NULL) 
  {
    if (temp_model->type == JJMODEL)
    {
      fprintf(dbfp, "JJ model %s\n", temp_model->name);

      temp_jj = (mod_jj *) temp_model->data;
      fprintf(dbfp, " rtype %1d", temp_jj->rtype);
      if (temp_jj != NULL)
      fprintf(dbfp, 
             "  ic %.2e cap %.2e vgap %.2e rg %.2e delv %.2e rn %.2e",
             temp_jj->ic, temp_jj->cap, temp_jj->vgap,
             temp_jj->rg, temp_jj->del_v, temp_jj->rn); 
      fprintf(dbfp, " cct %1d icon %.2e\n", 
              temp_jj->cct, temp_jj->icon);
    }

    temp_model = temp_model->next_model;

  }  /* while */

}    /* print_modlist */


void
print_realname_name()
{
  long i;

  if (jsim_dbg == FALSE) return;
  if (dbfp == NULL) return;
  fprintf(dbfp,"\nnames:\n");
 
  for (i = 0; i < dev_count; i++)
    fprintf(dbfp, "%s %ld\n", 
           ((realname_name *) (*(realname_array + i)))->real_name,
           ((realname_name *) (*(realname_array + i)))->name);

}  /* print_realname_name */

