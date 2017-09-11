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
sperror(int code)
{
  switch (code)
  {
    case NO_MEM : printf("no memory\n");
                  break;

    case M_SINGULAR : printf("matrix singular\n");
                      break;

    case LU_FAILED : printf("LU factorization failed\n");
                     break;

    case TYPE_MATCH : 
         printf("jj phi node cannot have other connection\n");
         break;

    case IND_NOTDEF : printf("inductor not defined\n");
                      break;

    case EXCESS_MODEL : printf("too many jj models\n");
                        break;

    case PHANTOM_NODE : 
     printf("print node or current does not exist in circuit\n");
     break;

    case MOD_EXIST : printf("modal already exist\n");
                       break;

    case SMALL_TIME_STEP :
     printf("transient internal time step too small\n");
     break;
  }
  exit(0);
}   /* sperror */


void
print_A(matrix A)
{
  m_data *temp;
  int i, element_count;

  element_count = 0;

  for (i = 0; i <= A.dim; i++)
  {
    temp = A.rowptr[i];
    printf("%d row count %d, col count %d\n", i,
            A.r_count[i], A.c_count[i]);

    if (A.diagptr[i] != NULL) printf("%d %f\n", i, A.diagptr[i]->val); 
    else printf("%d NULL\n", i);

    while (temp != NULL)
    {
      if (temp->type != NOTUSED)
      {
        element_count++;
        printf("(%d, %d) %e\n", temp->row, temp->col, temp->val);
      }
      temp = temp->n_col;
    }
  }

  if (A.lu_stop >= 0)
  printf("%f percent filled\n",
          100.0*element_count/((A.lu_stop + 1)*(A.lu_stop+1)));
}    /* print_A */


void
print_cA(matrix A)
{
  m_data *temp;
  int i;

  for (i = 0; i <= A.dim; i++)
  {
    temp = A.colptr[i];
    printf("%d row count %d, col count %d\n", i,
            A.r_count[i], A.c_count[i]);

    if (A.diagptr[i] != NULL) printf("%d %f\n", i, A.diagptr[i]->val); 
    else printf("%d NULL\n", i);

    while (temp != NULL)
    {
      if (temp->type != NOTUSED)
      printf("(%d, %d) %e\n", temp->row, temp->col, temp->val);
      temp = temp->n_row;
    }
  }
}  /* print_cA */


void
print_A_LU(matrix A)
{
  m_data *temp;
  int i, element_count;

  element_count = 0;

  for (i = 0; i <= A.dim; i++)
  {
    if (A.diagptr[i] != NULL)
      printf("%d %f\n", i, A.diagptr[i]->lu_val);
    else printf("%d NULL\n", i);
    temp = A.rowptr[i];
    while (temp != NULL)
    {
      if (temp->type != NOTUSED)
      {
        element_count++;
        printf("(%d, %d) %e\n", temp->row, temp->col, temp->lu_val);
      }
      temp = temp->n_col;
    }
  }
  printf("%f LU percent filled\n",
          100.0*element_count/((A.lu_stop + 1)*(A.lu_stop + 1)));
}    /* print_A_LU */


void
print_stat_LU(matrix A)
{
  m_data *temp;
  int i, element_count, fillin_count;

  element_count = 0;
  fillin_count = 0;

  for (i = 0; i <= A.lu_stop; i++)
  {
    temp = A.rowptr[i];
    while (temp != NULL)
    {
      if (temp->type != NOTUSED)
      {
        element_count++;
      }

      if (temp->type == FILLIN) fillin_count++;
      temp = temp->n_col;
    }
  }
  printf("%-19s%6d     %-19s%6.3f\n","LU size",A.lu_stop+1,
    "LU percent filled",
    100.0*element_count/((A.lu_stop + 1)*(A.lu_stop + 1)));

  printf("%-19s%6d     %-19s%6d\n","Non-zero elements",element_count,
    "Fill-ins",fillin_count);

}    /* print_stat_LU */


void
print_x(unknown *x, int stop, int stop2)
{
  int i;
  
  for (i = 0; i <= stop; i++)
  if (x[i] != NULL) printf("(%d) %e %e\n", i, x[i]->xn, x[i]->yn);

  for (i = stop2; i <= eqn_count + phi_count; i++)
  if (x[i] != NULL) printf("(%d) %e %e\n", i, x[i]->xn, x[i]->yn);

}   /* print_x */


void
print_b(source *b, int stop)
{
  int i;
 
  for (i = 0; i <= stop; i++)
  if (b[i] != NULL) printf("(%d) %e\n", i, b[i]->val);

}   /* print_b */
