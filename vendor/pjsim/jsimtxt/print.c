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
read_file()
{
  int i;
  FILE *temp_fp, *fopen();

  if (jsim_raw) {
    (void) read_string("");
    return;
  }

  read_error = read_string("file name to open");

  if (read_error != OK)
  {
    printf("## Warning -- illegal file specification\n");
    printf("## Warning -- output directed to last file specified\n");
    return;
  }

  if (strcmp(tempstring, "STDOUT") == 0)
  {
    fileptr[MAXFILE] = stdout;
    current_fp = stdout;
    return;
  } 
  
  for (i = 0; i <= file_count; i++)
  if (strcmp(tempstring, filename[i]) == 0)
  {
    current_fp = fileptr[i];
    return;
  } 

  if (file_count == MAXFILE - 1)
  {
    printf("## Warning -- cannot specify more than %d files\n",
            MAXFILE);
    printf("## Warning -- output directed to last file specified\n");
    return;
  }

  temp_fp = fopen(tempstring, "w");

  if (temp_fp == NULL)
  {
    printf("## Warning -- cannot open file %s\n", tempstring);
    printf("## Warning -- output directed to last file specified\n");
    return;
  }
  
  file_count++;
  filename[file_count] = new_string(tempstring);
  fileptr[file_count] = temp_fp;
  current_fp = temp_fp;
  
}    /* read_file */


void
read_print()
{
  int first_node, second_node, whichpart;
  char *dev_name;


  if (current_fp == stdout) fileptr[MAXFILE] = stdout;

  read_error = read_string("print type");

  if (read_error == OK)
  switch(get_string_keyword(tempstring))
  {
    case NODEV :
         second_node = 0;
         read_error = read_int(&first_node, "plus node", FALSE);
         read_error = read_int(&second_node, "minus node", FALSE);
         if (read_error == OK) 
           add_pr_node(current_fp, first_node, second_node);
         break;
    
    case DEVV :
         read_error = read_string("device name");
         dev_name = new_string(tempstring);
         if (read_error == OK)
         {
           read_error = read_string("");
           if (read_error == OK)
             whichpart = get_string_keyword(tempstring);
           else whichpart = ILLEGAL;

           add_pr_device(current_fp, dev_name, VOLT, whichpart);
         }
         break;
    
    case DEVI :
         read_error = read_string("device name");
         dev_name = new_string(tempstring);
         if (read_error == OK)
         {
           read_error = read_string("");
           if (read_error == OK)
             whichpart = get_string_keyword(tempstring);
           else whichpart = ILLEGAL;

           add_pr_device(current_fp, dev_name, AMP, whichpart);
         }
         break;
      
    case PHASE :
         read_error = read_string("device name");
         dev_name = new_string(tempstring);
         add_pr_device(current_fp, dev_name, PHI, whichpart);
         break;

    default:
         printf("## Error -- illegal print type %s\n", tempstring);
         no_go = TRUE;
         break;

  }   /* switch */
  else 
  {
    printf("## Error -- bad print request\n");
    no_go = TRUE;
  }

}  /* read_print */


void
add_pr_node(FILE *fp, int plus, int minus)
{
  printdata *temp_print;

  temp_print = (printdata *) mycalloc(1, sizeof(printdata));

  temp_print->fp = fp;
  temp_print->plus = plus;
  temp_print->minus = minus;
  temp_print->is_dev = FALSE;
  temp_print->next_print = NULL;
 
  if (print_list == NULL) print_list = print_tail = temp_print;
  else
  {
    print_tail->next_print = temp_print;
    print_tail = temp_print;
  }
}   /* add_pr_node */


void
add_pr_device(FILE *fp, char *name, int prtype, int subtype)
{
  printdata *temp_print;

  temp_print = (printdata *) mycalloc(1, sizeof(printdata));

  temp_print->fp = fp;
  temp_print->dev_name = name;
  temp_print->prtype = prtype;
  temp_print->subtype = subtype;
  temp_print->is_dev = TRUE;
  temp_print->next_print = NULL;
 
  if (print_list == NULL) print_list = print_tail = temp_print;
  else
  {
    print_tail->next_print = temp_print;
    print_tail = temp_print;
  }
}   /* add_pr_device */


void
do_print()
{
  printdata *temp_print;
  node_to_eqn *temp_node_eqn;
  int type;
  device *temp_dev;
  
  temp_print = print_list;

  while (temp_print != NULL)
  {
    if (temp_print->is_dev == FALSE)
    {
      if (temp_print->plus == 0) temp_print->plus = ground_node;
      else
      {
        temp_node_eqn = search_node(temp_print->plus);
        if (temp_node_eqn != NULL)
          temp_print->plus = temp_node_eqn->n_eq;
        else
        {
          printf("## Error -- print node %d not defined\n", 
                  temp_print->plus);
          no_go = TRUE;
        }
      }

      if (temp_print->minus == 0) temp_print->minus = ground_node;
      else
      {
        temp_node_eqn = search_node(temp_print->minus);
        if (temp_node_eqn != NULL)
          temp_print->minus = temp_node_eqn->n_eq;
        else
        {
          printf("## Error -- print node %d not defined\n", 
                  temp_print->minus);
          no_go = TRUE;
        }
      }
    }
    else     /* is_dev is TURE */
    {
      type = get_devname_type(temp_print->dev_name);
      if ((type != ILLEGAL) && (type != COMMENT))
      {
        temp_dev = find_dev(temp_print->dev_name, type);
        if (temp_dev != NULL)
          temp_print->print_dev = temp_dev;
        else
        {
          printf("## Error -- print device %s not found\n", 
                  temp_print->dev_name);
          no_go = TRUE;
        }
      }
      else
      {
        printf("## Error -- illegal print device name %s\n", 
                temp_print->dev_name);
        no_go = TRUE;
      }
    }

    temp_print = temp_print->next_print;

  }   /* while */

}  /* do_print */


