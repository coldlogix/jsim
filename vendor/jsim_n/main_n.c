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
/** noise hack by J Satchell, DRA Malvern **/
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


#include "jsim_n.h"
#include "global.h"
#include <sys/types.h>
#include <sys/timeb.h>

#if __STDC__
static void run_jsim(void);
static void do_opts(char*);
#else
static void run_jsim();
static void do_opts();
#endif

FILE *fp;


int
main(argc,argv)

int argc;
char **argv;
{
  int i;
  int tried = FALSE;
  struct timeb now;
  int seed;

#if (__TURBOC__)
  _control87(0xffff,0x003f);
#endif
/* use real time to set random number generator to non repeatable state */
  ftime(&now);
  seed = now.time*1000+now.millitm;
#ifdef NORANDOM
  srand(seed);
#else
  srandom(seed);
#endif
  jsim_dbg = FALSE;
  jsim_raw = FALSE;
  jsim_mout = FALSE;

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
        if (*(argv[i]+1) == 'm') {
          jsim_mout ^= 1;
          jsim_raw = FALSE;
          mfilename = argv[i+1];  /* argv should not vanish during program execution */
          printf("Filename for exported variables: %s\n", mfilename);
          i++;
        } else {
          do_opts(argv[i]+1);
        }
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

} /* main */


static void
run_jsim()

{
  FILE *fopen();
  struct timeb now;
  long t;

  init_global();
  read_deck();
  process_deck();
  deckerror_check();
  topology_check();
  free_space();

  if (jsim_mout) {
    mfile = fopen(mfilename, "w");
    if (!mfile) {
      printf("## Error -- cannot open output file: %s\n", mfilename);
      no_go = TRUE;
    } else {
      print_header(mfile);
    }
  } else {
    print_header(stdout);
  }

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
    
    ftime(&now);
    t = now.time*1000 + now.millitm;
    time_loop(hptr);
    ftime(&now);
    t -= now.time*1000 + now.millitm;
    print_stat_LU(my_matrix);
    printf("\nSimulation ran %.2f seconds.\n",-t/1000.0);
    printf("This Stochastic extension to JSIM  bought to you by\n");
    printf("Julian Satchell, DRA(Malvern), UK. \n");
    printf("satchell@dra.hmg.gb\nOur WWW page is at: www.dera.hmg.gb\n");
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
do_opts(s)

char *s;
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
      if (!jsim_mout) jsim_raw ^= 1;
      break;
    }
  }

}  /* do_opts */


