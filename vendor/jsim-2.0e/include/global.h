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

FILE *fileptr[MAXFILE+1];
FILE *current_fp;
int file_count;

long lu_opcount;

node_to_eqn *node_map[RANGE + 1];
long ground_node;
long eqn_count;
long phi_start;
long phi_end;
long phi_count;
long node_count;
long subnode_count;
long *node_array;
long *eqn_array;

matrix *A_matrix, my_matrix;
unknown *x_unknown;
unknown *x_unk_copy;
source *b_source;
source *b_src_copy;
double last_time;
double stop_time;
double user_step;
double user_pr_step;
double start_pr_time;
double last_pr_time;

model jsim_model;
model jsim_model_tail;
long *jsim_model_array;
int mod_count;

double vtran_min;
double vtran_rel;
double itran_min;
double itran_rel;
double error_tolerance;

double max_phi_step;
double phi_min;
double phi_rel;
double phi_quit;

double max_flux_step;

int nu_digit;
char out_format[20];

double vs_min_inc;
double vs_max_inc;
double is_min_inc;
double is_max_inc;
int nu_min_sample;
int nu_segment;

double vn_guess_min;

int no_truncation_error;
int no_flux_control;

long lu_count;
long solve_count; 
long last_solve_count;

printdata *print_list, *print_tail;

device *resis, *resis_tail;
long resis_count;
long *resis_array;

device *cap, *cap_tail;
long cap_count;
long *cap_array;

device *ind, *ind_tail;
long ind_count;
long *ind_array;

flux_check_list *fcheck_list, *fcheck_tail;

device *jj, *jj_tail;
long jj_count;
long *jj_array;

device *vsource, *vsource_tail;
long vsource_count;
long *vsource_array;

device *isource, *isource_tail;
long isource_count;
long *isource_array;

device *mut, *mut_tail;
long mut_count;
long *mut_array;

device *xline, *xline_tail;
long xline_count;
long *xline_array;

device *sub_ckt, *sub_ckt_tail;
long sub_ckt_count;
long *sub_ckt_array;

sub_def *sub_def_tree, *sub_def_tail;
long sub_def_count;
long *sub_def_array;
sub_def *current_sub_def;

realname_name *namelist, *namelist_tail;
long dev_count;
long *realname_array, *name_array;


int no_go;
int warned;
int igwarn_no_go;
int jsim_dbg;
int jsim_raw;

int read_error;
char *dev_name;
char linesave[500];
char *line;
char namestring[200];
char tempstring[200];
double dataptr[100];
long nodeptr[100];
long intptr[100];
char *filename[MAXFILE];

double hptr[STEPSAVE+1];

int bkpoint_count;
double *bkpoint_time, *bkpoint_step;
