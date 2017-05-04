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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/* constants */
#define GROUND              -1

/* error defines */
#define TOO_BIG             -1
#define OK                  0

#define FALSE               0
#define TRUE                1
#define NOT_YET             2


#define NO_MEM              1
#define M_SINGULAR          2
#define LU_FAILED           3
#define TYPE_MATCH          4
#define IND_NOTDEF          5
#define EXCESS_MODEL        6
#define PHANTOM_NODE        7
#define MOD_EXIST           8
#define SMALL_TIME_STEP     9

#define PIVOT_MIN 1.0e-12
#define PIVOT_REL 1.0e-5 


/* unknown element type */

#define VOLT       0
#define VOLT_TIME  3
#define AMP        1
#define AMP_TIME   4
#define PHI        2


/* matrix element type */

#define ORIGINAL        0
#define CONST_ORIGINAL  3
#define FILLIN          1
#define NOTUSED         2

/* pivot type */

#define PIVOT_FULL      0
#define PIVOT_ROW       1
#define PIVOT_DIAG      2


/* matrix typedefs */
typedef struct matrixdata
    {
      double val;
      double lu_val;
      int type;
      int row, col;
      struct matrixdata *n_row, *n_col;
    } m_data, *m_data_ptr;


typedef struct matrixdef
    {
      m_data_ptr *rowptr, *colptr, *diagptr;
      int *r_order, *c_order;
      int *r_count, *c_count;
      int dim, lu_stop;
    } matrix; 


typedef struct vardata
    {
      double xn;
      double yn;
      double *xpast;
      double xdpast;
      double xn_guess;
      int type;
      long node_branch; 
    } x_data, *unknown;


typedef struct sourcedata
    {
      double val;
      double val_nonlin;
    } s_data, *source;


typedef struct temppivot
    {
      m_data *pivot_data, *next_data;
    } pivot_row_data;


/* device stuff */

#define MAXFILE                 50

/* device type */

#define LATER                   -4
#define CONTROL                 -3
#define COMMENT                 -2
#define ILLEGAL                 -1

#define RESIS                   0
#define CAP                     1
#define INDUCT                  2
#define PWL_V                   3
#define PWL_I                   4
#define DC_V                    5
#define DC_I                    6
#define SIN_V                   7
#define SIN_I                   8
#define PULSE_V                 9
#define PULSE_I                 10
#define MUTUAL_L                11
#define JJ                      12
#define SUB_C                   13
#define V_SOURCE                14
#define I_SOURCE                15
#define TRANSLINE               16
#define TRAN_NO_LOSS            17
#define LOSSLESS_LINE           18


typedef struct modtype {
    char *name;
    int type;
    char *data;
    struct modtype *next_model;
  } modeldata, *model;

typedef struct devtype {
    int type;
    long name;
    char *data;
    struct devtype *next_dev;
  }  device;

typedef struct sub_d {
    char *name;
    int no_node;
    int *node_list;
    device *the_sub_ckt, *the_sub_tail;
    long sub_dev_count;
    long *sub_dev_array;
    struct sub_d *parent;
    struct sub_d *next_sub;
    struct sub_d *sub_sub, *sub_sub_tail;
    long sub_sub_count;
    long *sub_sub_array;
  }  sub_def;   

typedef struct printtype {
    FILE *fp;
    int plus, minus;
    int prtype;
    int subtype;
    int is_dev;
    char *dev_name;
    device *print_dev;
    struct printtype *next_print;
  }  printdata;


typedef struct MJPS {
    double ic;
    double cap;
    double vgap;
    double rg;
    double del_v;
    double rn;
    double glarge;
    double quasi_bkpoint[4];
    int rtype;
    int cct;
    double icon;
  } mod_jj;


typedef struct func_sin {
    double off;
    double amp;
    double freq;
    double td;
    double theta;
  }  f_sin;


typedef struct func_pulse {
    double plow;
    double phigh;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
  }  f_pulse;


typedef struct func_pwl {
    int size;
    int pre_piece;
    double *timedata;
  }  f_pwl;

#define POLY_ORDER    1
#define TWO_E_HBAR    3.038510188e15
#define E_HBAR        TWO_E_HBAR/2.0
#ifndef PI
#define PI            3.1415926535897932
#endif
#ifdef PI2
#undef PI2
#endif
#define PI2           PI*2.0
#define SUB_GAP       0
#define TRANSITION    1
#define NORMAL        2

#define JJ_SHUNT      0
#define JJ_PWL        1
#define JJ_FERMI      2

#define SINXX         1


/* resistor */

typedef struct d_resis {
    double val;
    long n_plus, n_minus;
    m_data *mptr_pp, *mptr_pm, *mptr_mp, *mptr_mm;
  } dev_resis;  

typedef struct dd_resis {
    double val;
    long n_plus, n_minus;
    int p_node, m_node;
  } sub_resis;


/* capacitor */

typedef struct d_cap {
    double val;
    double ic;
    long n_plus, n_minus;
    m_data *mptr_pp, *mptr_pm, *mptr_mp, *mptr_mm;
  } dev_cap; 

typedef struct dd_cap {
    double val;
    double ic;
    long n_plus, n_minus;
    int p_node, m_node;
  } sub_cap;


/* inductor */

typedef struct d_ind {
    double val;
    double ic;
    long n_plus, n_minus;
    long branch;
    int fcheck;
    double fvalue;
    m_data *mptr_ii;
  } dev_ind;

typedef struct dd_ind {
    double val;
    double ic;
    long n_plus, n_minus;
    int p_node, m_node;
    int fcheck;
    double fvalue;
  }  sub_ind;


/* independent voltage source */

typedef struct d_vsource {
    int function;
    char *function_parm;
    long n_plus, n_minus;
    long branch;
   } dev_vsource;

typedef struct dd_vsource {
    int function;
    char *function_parm;
    long n_plus, n_minus;
    int p_node, m_node;
  }  sub_vsource;


/* independent current source */

typedef struct d_isource {
    int function;
    char *function_parm;
    long n_plus, n_minus;
  } dev_isource;

typedef struct dd_isource {
    int function;
    char *function_parm;
    long n_plus, n_minus;
    int p_node, m_node;
  } sub_isource;


/* dc voltage source */

typedef struct d_dcv {
    double val;
    long n_plus, n_minus;
    long branch;
  } dev_dcv;

typedef struct dd_dcv {
    double val;
    long n_plus, n_minus;
    int p_node, m_node;
  }  sub_dcv;


/* dc current source */

typedef struct d_dci {
    double val;
    long n_plus, n_minus;
  } dev_dci;

typedef struct dd_dci {
    double val;
    long n_plus, n_minus;
    int p_node, m_node;
  } sub_dci;


/* sin voltage source */

typedef struct d_sinv {
    double vo;
    double va;
    double freq;
    double td;
    double theta;
    double allowed_step;
    long n_plus, n_minus;
    long branch;
  } dev_sinv;

typedef struct dd_sinv {
    double vo;
    double va;
    double freq;
    double td;
    double theta;
    long n_plus, n_minus;
    int p_node, m_node;
  }  sub_sinv;


/* sin current source */

typedef struct d_sini {
    double io;
    double ia;
    double freq;
    double td;
    double theta;
    double allowed_step;
    long n_plus, n_minus;
  } dev_sini;

typedef struct dd_sini {
    double io;
    double ia;
    double freq;
    double td;
    double theta;
    long n_plus, n_minus;
    int p_node, m_node;
  } sub_sini;


/* pulse voltage source */

typedef struct d_pulsev {
    double v1;
    double v2;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
    double allowed_tr;
    double allowed_tf;
    double allowed_pw;
    double allowed_restper;
    long n_plus, n_minus;
    long branch;
  } dev_pulsev;

typedef struct dd_pulsev {
    double v1;
    double v2;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
    long n_plus, n_minus;
    int p_node, m_node;
  }  sub_pulsev;


/* pulse current source */

typedef struct d_pulsei {
    double i1;
    double i2;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
    double allowed_tr;
    double allowed_tf;
    double allowed_pw;
    double allowed_restper;
    long n_plus, n_minus;
  } dev_pulsei;

typedef struct dd_pulsei {
    double i1;
    double i2;
    double td;
    double tr;
    double tf;
    double pw;
    double per;
    long n_plus, n_minus;
    int p_node, m_node;
  } sub_pulsei;


/* piece-wise linear voltage source */

typedef struct d_pwlv {
    int size;
    int pre_piece; 
    double *timedata;
    double *allowed_step;
    long n_plus, n_minus;
    long branch;
  } dev_pwlv;

typedef struct dd_pwlv {
    int size;
    int pre_piece;
    double *timedata;
    long n_plus, n_minus;
    int p_node, m_node;
  }  sub_pwlv;


/* piece-wise linear current source */

typedef struct d_pwli {
    int size;
    int pre_piece;
    double *timedata;
    double *allowed_step;
    long n_plus, n_minus;
  } dev_pwli;

typedef struct dd_pwli {
    int size;
    int pre_piece;
    double *timedata;
    long n_plus, n_minus;
    int p_node, m_node;
  } sub_pwli;
    

/* josephson junction */

typedef struct d_jj {
    double area;
    double v_ic;
    double phi_ic;
    double ic;
    double ic_current;
    double ic_past;
    double cap;
    double gg;
    double gn;
    double glarge;
    long n_plus, n_minus;
    long n_phi;
    char *con_dev;
    m_data *mptr_pp, *mptr_pm, *mptr_mp, *mptr_mm;
    modeldata *mod;
    int which_piece;
    double vn_guess;
    double poly_coeff[POLY_ORDER+1];
  }  dev_jj;  

typedef struct dd_jj {
    double area;
    double v_ic;
    double phi_ic;
    long n_plus, n_minus;
    long n_phi;
    char *con_dev;
    modeldata *mod;
    int p_node, m_node;
  } sub_jj;


/* mutual inductance */

typedef struct d_mut {
    double val;
    long n_primary, n_second;
    char *primary_name, *second_name;
    m_data *mptr_primary, *mptr_second;  
  } dev_mut;

typedef struct dd_mut {
    double val;
    char *primary_name, *second_name;
  }  sub_mut;


/* sub circuit */

typedef struct d_sub_ckt {
    char *sub_def_name;
    int no_node;
    long *node_list;
  } dev_sub_ckt;

typedef struct dd_sub_ckt {
    char *sub_def_name;
    int no_node;
    long *node_list;
    long *temp_node_list;
    long *node_node;
  } sub_sub_ckt;


/* picture frame */

typedef struct frametype {
    double interval;
    double nominal_step;
    double val_min;
    double val_rel;
    int unit_size;
    int size;
    int current_size;
    double *time_data;
    double *value_data;
  } frame;


/* transmission line lossless */

typedef struct d_tran_noloss {
    long n1_plus, n1_minus;
    long n2_plus, n2_minus;
    double zo;
    double td;
    double v1o;
    double i1o;
    double v2o;
    double i2o;
    double is1;
    double is2;
    double is1past;
    double is2past;
    frame iport1;
    frame vport1;
    frame iport2;
    frame vport2;
    m_data *mptr_pp1, *mptr_pm1, *mptr_mp1, *mptr_mm1;
    m_data *mptr_pp2, *mptr_pm2, *mptr_mp2, *mptr_mm2;
  } dev_tran_noloss;


typedef struct dd_tran_noloss {
    long n1_plus, n1_minus;
    long n2_plus, n2_minus;
    int p1_node, m1_node;
    int p2_node, m2_node;
    double zo;
    double td;
    double v1o;
    double i1o;
    double v2o;
    double i2o;
  } sub_tran_noloss;


#define RANGE      20
#define SUB_BUFFER_SIZE 100

#define PLUS_NODE       1
#define MINUS_NODE      2
#define PLUS_C_NODE     3
#define MINUS_C_NODE    4

#define POSITIVE        1
#define NEGATIVE        2


typedef struct bmarkertype {
    int marker;
    long pnode, mnode;
} branch_marker;
     

typedef struct connecttype {
    long name;
    int devtype;
    int nodetype;
    branch_marker *bmarker;
    struct connecttype *next_branch;
  } branch_list;
    

typedef struct nodeeqn {
    int n_eq;
    int node;
    int marker;
    branch_list *branch, *branch_tail;    
    struct nodeeqn *next_node;
  } node_to_eqn;

typedef struct subnodenode {
    int subnode;
    int node;
  } subnode_to_node;

typedef struct rn_nametype {
    char *real_name;
    long name;
    struct rn_nametype *next_name;
  }  realname_name;

typedef struct fchecktype {
    device *flux_check;
    struct fchecktype *next_flux_check;
  }  flux_check_list;


#define STEPSAVE   3

#define NONLIN_MAX 20

#define DEF_VTRAN_MIN  1.0e-7
#define DEF_VTRAN_REL  1.0e-4
#define DEF_ITRAN_MIN  1.0e-10
#define DEF_ITRAN_REL  1.0e-4
#define DEF_ERROR_TOLERANCE 10.0

#define DEF_MAX_PHI_STEP   1.5
#define DEF_PHI_MIN        1.0e-2
#define DEF_PHI_REL        1.0e-3
#define DEF_PHI_QUIT       6.00

#define DEF_MAX_FLUX_STEP  0.5e-15

#define DEF_NU_DIGIT       3

#define DEF_VN_GUESS_MIN   0.5e-3
#define VN_GUESS_REF       2.0e-3

#define DEF_VS_MIN_INC     0.2e-3
#define DEF_VS_MAX_INC     0.5e-3
#define DEF_IS_MIN_INC     2.0e-6
#define DEF_IS_MAX_INC     10.0e-6
#define DEF_NU_MIN_SAMPLE  4
#define DEF_SEGMENT_MIN    4

#define MIN_TIME_STEP      1.0e-15
#define DEF_TIME_STEP      1.0e-12


typedef struct d_breakpoint {
    double stime, ftime;
    double step;
    struct d_breakpoint *left, *right;
  }  breakpoint_node;


#define OPTIONS              0
#define TRAN                 1
#define PRINT                2
#define SIN                  3
#define PWL                  4
#define PULSE                5
#define MODEL                6
#define JJMODEL              7
#define TRANMODEL            8
#define FLUX                 9
#define SUBCKT               10
#define ENDS                 11
#define NODEV                12
#define DEVI                 13
#define DEVV                 14
#define PHASE                15
#define AREA                 16
#define IC                   17
#define ICRIT                18
#define CAPMOD               19
#define R0                   20
#define RN                   21
#define RTYPE                22
#define ICON                 23
#define DELV                 24
#define VG                   25
#define CCT                  26
#define VNTOL                27
#define INTOL                28
#define VRELTOL              29
#define NUMDGT               30
#define LTE                  31
#define PHITOL               32
#define IRELTOL              33
#define PHIRELTOL            34
#define MAXPHISTEP           35
#define NEWFILE              36
#define FCHECK               37
#define CONDEV               38
#define PHIQUIT              39
#define NUSAMPLE             40
#define NUSEGMENT            41
#define RELTOL               42
#define JJALL                43
#define JJJOSEPH             44
#define JJRESIS              45
#define JJCAP                46
#define JJTOTAL              47
#define IGWARN               48
#define Z0                   49
#define TD                   50
#define LOSSLESS             51
#define PORT1                52
#define PORT2                53
#define MAXFLUXSTEP          54
#define FVALUE               55

/* default parameters */
#define DEF_RTYPE            0
#define DEF_CCT              0
#define DEF_ICRIT            1.0e-3	
#define DEF_JJCAP            2.5e-12	
#define DEF_VG               2.8e-3
#define DEF_R0               3.0e+1
#define DEF_DELV             0.1e-3
#define DEF_RN               5.0
#define DEF_ICON             1.0e-3
#define DEF_TD_MIN           0.2e-12
#define DEF_ZO_MIN           1.0e-6

