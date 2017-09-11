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

extern FILE *fileptr[];
extern FILE *current_fp;
extern int file_count;

extern long lu_opcount;

extern node_to_eqn *node_map[];
extern long ground_node;
extern long eqn_count;
extern long phi_start;
extern long phi_end;
extern long phi_count;
extern long node_count;
extern long subnode_count;
extern long *node_array;
extern long *eqn_array;

extern matrix *A_matrix, my_matrix;
extern unknown *x_unknown;
extern unknown *x_unk_copy;
extern source *b_source;
extern source *b_src_copy;
extern double last_time;
extern double stop_time;
extern double user_step;
extern double user_pr_step;
extern double start_pr_time;
extern double last_pr_time;

extern model jsim_model;
extern model jsim_model_tail;
extern long *jsim_model_array;
extern int mod_count;

extern double vtran_min;
extern double vtran_rel;
extern double itran_min;
extern double itran_rel;
extern double error_tolerance;

extern double max_phi_step;
extern double phi_min;
extern double phi_rel;
extern double phi_quit;

extern double max_flux_step;

extern int nu_digit;
extern char out_format[];

extern double vs_max_inc;
extern double vs_min_inc;
extern double is_max_inc;
extern double is_min_inc;
extern int nu_min_sample;
extern int nu_segment;

extern double vn_guess_min;

extern int no_truncation_error;
extern int no_flux_control;

extern long lu_count;
extern long solve_count;
extern long last_solve_count;

extern printdata *print_list, *print_tail;

extern device *resis, *resis_tail;
extern long resis_count;
extern long *resis_array;

extern device *cap, *cap_tail;
extern long cap_count;
extern long *cap_array;

extern device *ind, *ind_tail;
extern long ind_count;
extern long *ind_array;

extern flux_check_list *fcheck_list, *fcheck_tail;

extern device *jj, *jj_tail;
extern long jj_count;
extern long *jj_array;

extern device *vsource, *vsource_tail;
extern long vsource_count;
extern long *vsource_array;

extern device *isource, *isource_tail;
extern long isource_count;
extern long *isource_array;

extern device *mut, *mut_tail;
extern long mut_count;
extern long *mut_array;

extern device *xline, *xline_tail;
extern long xline_count;
extern long *xline_array;

extern device *sub_ckt, *sub_ckt_tail;
extern long sub_ckt_count;
extern long *sub_ckt_array;

extern sub_def *sub_def_tree, *sub_def_tail;
extern long sub_def_count;
extern long *sub_def_array;
extern sub_def *current_sub_def;

extern realname_name *namelist, *namelist_tail;
extern long dev_count;
extern long *realname_array, *name_array;


extern int no_go;
extern int warned;
extern int igwarn_no_go;
extern int jsim_dbg;
extern int jsim_raw;


extern int read_error;
extern char *dev_name;
extern char linesave[];
extern char *line;
extern char namestring[];
extern char tempstring[];
extern double dataptr[];
extern long nodeptr[];
extern long intptr[];
extern char *filename[];

extern double hptr[];

extern int bkpoint_count;
extern double *bkpoint_time, *bkpoint_step;

extern FILE *rawfp;
extern int raw_cntr;


/* debug.c */
extern void sperror(int);
extern void print_A(matrix);
extern void print_cA(matrix);
extern void print_A_LU(matrix);
extern void print_stat_LU(matrix);
extern void print_x(unknown*,int,int);
extern void print_b(source*,int);

/* function.c */
extern char *read_function(int*);
extern char *read_sin(void);
extern char *read_pulse(void);
extern char *read_pwl(void);
extern double func_eval(int,char*,double);
extern void func_breakpoint(int,char*,double,double);
extern void func_infoprint(FILE*,int,char*);
extern void new_function(int,char**,char*);

/* matrix.c */
extern m_data *find_i_j(matrix*,int,int);
extern m_data *add_i_j(matrix*,int,int,int*);
extern m_data *add_next(matrix*,m_data*,m_data*,int,int);
extern void create_rowptr(matrix*);
extern void Axy(matrix*,int,int,double);
extern void ex_row(matrix*,int,int,int);
extern void ex_col(matrix*,int,int,int);
extern s_data *add_source(source*,int);
extern x_data *add_unknown(unknown*,int);
extern void ex_unknown(unknown*,int,int);
extern void ex_source(source*,int,int);
extern void A_LU(matrix*,int,int);
extern m_data *full_pivot(matrix*,int,int);
extern m_data *row_pivot(matrix*,int,int);
extern m_data *diag_pivot(matrix*,int);
extern void solve_Ly_b(matrix*,unknown*,source*);
extern void solve_Ux_y(matrix*,unknown*);
extern void matrix_clear(void);
extern void LU_refresh(void);
extern void clear_source(void);
extern void clear_nonlin_source(void);

/* misc.c */
extern char *mycalloc(int,int);
extern int  readline(char*);
extern void ignore_separator(char*);
extern int  read_int(int*,char*,int);
extern int  read_long(long*,char*,int);
extern int  read_string(char*);
extern char *new_string(char*);
extern char *combine_string(char*,char*);
extern char *comb_str_seg(char*,int,char*);
extern int  get_multiplier(char*,double*);
extern int  num_multiplier(char*,double*);
extern int  read_double(double*,char*,int);
extern double myabs(double);
extern double mymax(double,double);
extern double mymin(double,double);
extern int  get_string_type(char*);
extern int  get_devname_type(char*);
extern int  get_string_keyword(char*);
extern int  source_type(int);
extern int  is_dc_device(int);
extern int  node_type(int);

/* model.c */
extern void read_model(void);
extern modeldata *add_mod(char*,int);
extern modeldata *add_to_modlist(char*,int,long*,double*);
extern void read_jjmodel(void);
extern void assign_jj_mod(modeldata*,int,double,double,double,double,
                    double,double,int,double);
extern double jjiv(double,modeldata*);

/* morspace.c */
extern void free_devarray(void);
extern void free_name(void);
extern void free_def_tree(void);
extern void free_sub_def(sub_def*);

/* picture.c */
extern void add_frame_point(frame*,double,double);
extern double delay_frame_point(frame*,double,int*);

/* print.c */
extern void read_file(void);
extern void read_print(void);
extern void add_pr_node(FILE*,int,int);
extern void add_pr_device(FILE*,char*,int,int);
extern void do_print(void);

/* setup.c */
extern void init_global(void);
extern void init_default_global(void);
extern void init_dev_global(void);
extern void init_A(matrix*);
extern void init_source(source*);
extern void init_unknown(unknown*);
extern void init_unk_copy(unknown*,unknown*);
extern void init_b_src_copy(source*,source*);
extern void init_node_map(void);
extern void read_deck(void);
extern char *datestring(void);
extern void read_option(void);
extern void process_deck(void);
extern void get_arrays(void);
extern void do_dependent(void);
extern void get_model(void);
extern void free_space(void);
extern void setup_device(void);
extern void setup_x_unknown(void);
extern void setup_A_matrix(void);
extern void setup_matrix(void);
extern void get_breakpoint(void);
extern void topology_check(void);
extern void deckerror_check(void);
extern void print_node_map(void);
extern void print_dev_array(long*,long);
extern void print_devlist(void);
extern void print_deftree(sub_def*);
extern void myprint_deftree(sub_def*,FILE*);
extern void print_modlist(void);
extern void print_realname_name(void);

/* srchsort.c */
extern void merge_copy(long*,long,long);
extern void sort_realname(long,long);
extern void merge_realname(long,long,long);
extern void get_realname_array(void);
extern void get_name_array(void);
extern realname_name *search_realname(char*);
extern realname_name *search_name(long);
extern void sort_dev(long*,long,long);
extern void merge_dev(long*,long,long,long);
extern long *get_dev_array(int,device*,long);
extern device *search_dev(long*,long,char*);
extern device *search_name_dev(long*,long,long);
extern void sort_sub_def(long*,long,long);
extern void merge_sub_def(long*,long,long,long);
extern long *get_sub_def_array(sub_def*,long);
extern sub_def *search_sub_def(long*,long,char*);
extern void get_eqn_array(void);
extern void sort_node(long,long);
extern void merge_node(long,long,long);
extern void get_node_array(void);
extern node_to_eqn *search_node(int);
extern void sort_model(long,long);
extern void merge_model(long,long,long);
extern void get_model_array(void);
extern modeldata *search_model(char*);

/* subckt.c */
extern void read_sub_def(void);
extern void new_sub_def(char*,long*,long*);
extern void read_sub_ckt(void);
extern device *subckt_to_devlist(long,int,long*,long*,modeldata*);
extern device *subckt_to_deftree(char*,int,long*,long*,modeldata*);
extern int  scan_node(int);
extern device *add_sub(char*,int,char*);
extern device *add_sub_ckt(long,long,long*,char*);
extern long getnode_subnode(int,int);
extern char *get_subdev_name(char*);
extern void do_sub_ckt(void);
extern void expand_sub_ckt(void);
extern sub_def *find_sub_def(sub_def*,char*);
extern device *sub_to_devlist(char*,int,char*);

/* topology.c */
extern int  hash_node(int);
extern branch_marker *add_bmarker(void);
extern int  add_node(int,long,int,int,branch_marker*);
extern void clear_marker(void);
extern void cutset_check(void);
extern void trace_subgraph(node_to_eqn*,int);
extern void sourceloop_check(int);
extern void trace_connect(node_to_eqn*,int);
extern void ground_check(int);

/* tran.c */
extern void read_tran(void);
extern void check_breakpoint(double*);
extern int  step_control(double,double*,double*);
extern void time_loop(double*);
extern int  nonlinear_loop(int,double*);
extern int  guess_next(double,double*);
extern int  converge(void);
extern int  in_bkpoint(breakpoint_node*,double);
extern void init_breakpoint(double,double,double);
extern void add_breakpoint(double,double,double);
extern void find_left_bkpoint(void);
extern void bkpttree_to_array(void);
extern void get_bkpt_subcount(breakpoint_node*);
extern void get_bkpt_subarray(breakpoint_node*);
extern void free_bkpt_tree(breakpoint_node**);
extern void print_bkpt_subtree(breakpoint_node*);
extern void print_bkpt_tree(void);
extern void print_bkpt_array(void);
extern void find_dx(double*);
extern void find_phi(double*);
extern void update_unknown(void);
extern void iteration_update(void);
extern int  matrix_iteration_update(int,double*);
extern int  matrix_nonlin_iteration_update(int,double*);
extern void update_device(double*);
extern double get_pr_jjic(double,dev_jj*);
extern double get_pr_val(double,long,int);
extern void print_tran(double,double);

/* cap.c */
extern void read_cap(void);
extern device *cap_to_devlist(long,int,long*,double*);
extern device *cap_to_deftree(char*,int,long*,double*);
extern device *capsub_to_devlist(char*,int,char*);
extern device *add_cap(long,double,double,long,long);
extern void cap_matrix(void);
extern void cap_trap(int,double*);
extern void cap_tran_print(FILE*,double,int,device*);

/* device.c */
extern device *find_dev(char*,int);
extern device *add_dev(long,int,char*);
extern long add_realname(char*);

/* inductor.c */
extern void read_ind(void);
extern device *ind_to_devlist(long,int,long*,long*,double*);
extern device *ind_to_deftree(char*,int,long*,long*,double*);
extern device *indsub_to_devlist(char*,int,char*);
extern device *add_ind(long,double,double,long,long,long,int,double);
extern void ind_matrix(void);
extern void ind_trap(int,double*);
extern double ind_step_limit(double);
extern int  ind_apriori_step_limit(double,double*);
extern void ind_tran_print(FILE*,double,int,device*);

/* isource.c */
extern void read_is(void);
extern device *is_to_devlist(long,dev_isource*);
extern device *is_to_deftree(char*,sub_isource*);
extern device *issub_to_devlist(char*,char*);
extern device *add_isource(long,char*);
extern void is_matrix(void);
extern void is_breakpoint(void);
extern double is_eval(device*,double);
extern void is_advance(double*);
extern void is_tran_print(FILE*,double,int,device*,double);

/* jj.c */
extern void read_jj(void);
extern device *jj_to_devlist(long,int,long*,long*,double*,modeldata*);
extern device *jj_to_deftree(char*,int,long*,long*,double*,modeldata*);
extern device *jjsub_to_devlist(char*,int,char*);
extern void get_jjmodel(void);
extern device *add_jj(long,double,double,double,long,long,char*,modeldata*);
extern void jj_dependent(void);
extern void jj_matrix(void);
extern double jj_condev_i(dev_jj*,double);
extern void jj_quasi(dev_jj*,double,int*,double*,double*);
extern void jj_fix_trap(int,int*,double*);
extern void update_jj(void);
extern void jj_iteration_update(void);
extern double jj_step_limit(double);
extern int  jj_apriori_step_limit(double,double*);
extern void jj_tran_print(FILE*,double,int,int,device*);

/* mutual.c */
extern void read_mut(void);
extern device *mut_to_devlist(long,int,long*,long*,double*);
extern device *mut_to_deftree(char*,int,long*,long*,double*);
extern device *mutsub_to_devlist(char*,int,char*);
extern device *add_mut(long,double,char*,char*);
extern void mut_dependent(void);
extern void mut_matrix(void);
extern void mut_trap(int,double*);

/* resistor.c */
extern void read_resis(void);
extern device *resis_to_devlist(long,int,long*,double*);
extern device *resis_to_deftree(char*,int,long*,double*);
extern device *resissub_to_devlist(char*,int,char*);
extern device *add_resis(long,double,long,long);
extern void resis_matrix(void);
extern void resis_trap(int);
extern void resis_tran_print(FILE*,double,int,device*);

/* vsource.c */
extern void read_vs(void);
extern device *vs_to_devlist(long,dev_vsource*);
extern device *vs_to_deftree(char*,sub_vsource*);
extern device *vssub_to_devlist(char*,char*);
extern device *add_vsource(long,char*);
extern void vs_matrix(void);
extern void vs_breakpoint(void);
extern double vs_eval(device*,double);
extern void vs_advance(double*);
extern void vs_tran_print(FILE*,double,int,device*);

/* xline.c */
extern void read_transline(void);
extern device *xline_to_devlist(long,int,long*,double*);
extern device *xline_to_deftree(char*,int,long*,double*);
extern device *xlinesub_to_devlist(char*,int,char*);
extern device *add_transline(long,int,double*,long,long,long,long);
extern void trans_matrix(void);
extern void setup_transline(void);
extern void transline_trap(int,double*);
extern void update_transline(double*);
extern void xline_tran_print(FILE*,double,int,int,device*);

