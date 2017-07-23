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

#ifndef __SETUP_H__
#define __SETUP_H__


#define MAX_INCLUDE_FILES 16


void init_global();
void init_default_global();
void init_dev_global();
void init_A(matrix *A);
void init_source(source *b);
void init_unknown(unknown *x);
void init_unk_copy(unknown *x, unknown *xc);
void init_b_src_copy(source *b, source *bc);
void init_node_map();


extern FILE *fp;
void read_includefile();
void read_deck();
char * datestring();



extern int current_input_file;

typedef struct  {
    const char *filename;
    int line_number;
    FILE *fp;
} included_file;

extern included_file included_files[MAX_INCLUDE_FILES];


void print_file_stack();
int add_input_file(char *filename, FILE *fp);
int open_input_file(char *filename);
void read_includefile();
void read_option();
void process_deck();
void get_arrays();
void do_dependent();
void get_model();
void free_space();
void setup_device();
void setup_x_unknown();
void setup_A_matrix();
void setup_matrix();
void get_breakpoint();
void topology_check();
void deckerror_check();
void print_node_map();
void print_dev_array(long *dev_array, long dev_count);
void print_devlist();
void print_deftree(sub_def *the_sub_def);
void myprint_deftree(sub_def *the_sub_def, FILE *fp);
void print_modlist();
void print_realname_name();

#endif /* __SETUP_H__ */

