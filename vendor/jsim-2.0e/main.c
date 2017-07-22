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

/*************************************************************************
 *
 *  jsim [-options] [filenames] [-options] [filenames] ...
 *
 *    Options can be d, r, or nothing, following a '-'.
 *      d    toggle debugging, which dumps a file jsim.dbg (initially off).
 *      r    toggle use of a rawfile jsim.raw for output (initially off).
 *      -    read the standard input.
 *      
 *    Otherwise, tokens on the command line are assumed to be file names
 *    to read for input.  If no such tokens are found, the standard input
 *    is read.
 *
 *    The options operate on files listed to the right of the option
 *    list, and are active until changed with another invocation.  If
 *    no files are listed, the options are read before the standard
 *    input is read.
 *
 *  examples:
 *
 *    jsim file1 -d file2 -d file3
 *      will dump debug information while simulating file2.
 *     
 *    jsim file1 -rd - file3 <anotherfile
 *      will simulate file1, turn on debugging and rawfile creation,
 *      then simulate anotherfile, then file3.
 *
 *    jsim -r <inputfile
 *      will simulate inputfile and create a rawfile.
 *
 *************************************************************************/


#include "jsim.h"
#include "global.h"
#include "extern.h"
#include <sys/types.h>
#include <sys/time.h>

#if __STDC__
static void run_jsim(void);
static void do_opts(char*);
#else
static void run_jsim();
static void do_opts();
#endif

FILE *fp;


int
main(int argc, char **argv)
{
  int i;
  int tried = FALSE;

#if (__TURBOC__)
  _control87(0xffff,0x003f);
#endif
  
  jsim_dbg = FALSE;
  jsim_raw = FALSE;

  for (i = 1; i < argc; i++) {
    if (*argv[i] == '-') {
      if (*(argv[i]+1) == '\0') {
        fp = stdin;
        if (jsim_raw)
          printf("\nReading stdin\n");
        run_jsim();
        tried = TRUE;
      }
      else
        do_opts(argv[i]+1);
    }
    else {
      fp = fopen(argv[i],"r");
      if (fp == NULL)
        printf("\nCan't open %s\n",argv[i]);
      else {
        if (jsim_raw)
          printf("\nReading %s\n",argv[i]);
        run_jsim();
      }  
      tried = TRUE;
    }
  }
  if (tried == FALSE) {
    fp = stdin;
    if (jsim_raw)
      printf("\nReading stdin\n");
    run_jsim();
  }

  return (0);
} /* main */


static void
run_jsim()
{
  FILE *fopen();
  struct timeval now;
  double t;


  init_global();
  read_deck();
  process_deck();
  deckerror_check();
  topology_check();
  free_space();

  if (stop_time < 0.0)
  {
    printf("## Error -- no transient analysis specified\n");
    no_go = TRUE;
  }

  if ((no_go == FALSE) && 
      ((warned == FALSE) || (igwarn_no_go == TRUE)))
  {
    setup_device();
    setup_matrix();
    get_breakpoint();
    
    gettimeofday(&now, NULL);
    t = now.tv_sec + now.tv_usec*1e-6;
    time_loop(hptr);
    gettimeofday(&now, NULL);
    t -= now.tv_sec + now.tv_usec*1e-6;
    print_stat_LU(my_matrix);
    printf("\nSimulation ran %.2lf seconds.\n",-t);

/*
    printf("lu op count %d\n", lu_opcount);
*/

  }

  else if (no_go == TRUE)
    printf("\n\n#### JOB ABORTED due to error in input deck\n");

  else if (warned == TRUE)
    printf("\n\n#### JOB ABORTED due to warning in input deck\n");

}  /* run_jsim */


static void
do_opts(char *s)
{
  char *c;

  for (c = s; *c; c++) {
    if (*c == 'd') {
      jsim_dbg ^= 1;
      break;
    }
  }
  for (c = s; *c; c++) {
    if (*c == 'r') {
      jsim_raw ^= 1;
      break;
    }
  }

}  /* do_opts */
