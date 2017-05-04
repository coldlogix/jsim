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


m_data *
find_i_j(matrix *A, int i, int j)
{
   m_data  *temp;

   temp = A->rowptr[i];
   
   while (temp != NULL)
     if (temp->col == j) return (temp);
     else temp = temp->n_col;

   return(temp);

}  /* find_i_j */


m_data *
add_i_j(matrix *A, int i, int j, int *exist)
{
   m_data *d_ptr, *temp1;
   int tempint;

   temp1 = A->rowptr[i];
   
   while (temp1 != NULL)
   {
     if (temp1->col == j) 
     {
       *exist = 1;
       return (temp1);
     }
     else if (temp1->col > j) break;
     else if (temp1->n_col == NULL) break;
     else if (temp1->n_col->col > j) break;
     else temp1 = temp1->n_col;
   }
   

   d_ptr = (m_data *) mycalloc(1, sizeof(m_data)); 
   if (d_ptr == NULL) sperror(NO_MEM);

   d_ptr->row = i;
   d_ptr->col = j;
   d_ptr->val = 0.0;
   d_ptr->lu_val = 0.0;
   d_ptr->n_row = NULL;

   if (temp1 == NULL)
   {
      A->rowptr[i] = d_ptr;
      d_ptr->n_col = NULL;
   } 
   else if (temp1->col > j)
   {
      d_ptr->n_col = temp1;
      A->rowptr[i] = d_ptr;
   }
   else
   {  
     d_ptr->n_col = temp1->n_col;
     temp1->n_col = d_ptr;
   }

   if (i == j) A->diagptr[i] = d_ptr;

   *exist = 0;
   return (d_ptr);

}   /* add_i_j  */


m_data *
add_next(matrix *A, m_data *p_row, m_data *p_col, int i, int j)
{
  m_data *d_ptr;

  d_ptr = (m_data *) mycalloc(1, sizeof(m_data));
  if (d_ptr == NULL) sperror(NO_MEM);
  
  d_ptr->row = i;
  d_ptr->col = j;

  if (p_row != NULL)
  {
    d_ptr->n_row = p_row->n_row;
    p_row->n_row = d_ptr;
  }
  else 
  {
    d_ptr->n_row = A->colptr[j];
    A->colptr[j] = d_ptr;
  }

  if (p_col != NULL)
  {
    d_ptr->n_col = p_col->n_col;
    p_col->n_col = d_ptr;
  }
  else 
  {
    d_ptr->n_col = A->rowptr[i];
    A->rowptr[i] = d_ptr;
  }

  if (i == j) A->diagptr[i] = d_ptr;
  if ((i <= A->lu_stop) && (j <= A->lu_stop))
  {
    A->r_count[i]++;
    A->c_count[j]++;
  }

  return(d_ptr);

}  /* add_next */


void
create_rowptr(matrix *A)
{
  m_data_ptr *temp_colptr;
  m_data *temp_ptr;
  int i;

  temp_colptr = (m_data_ptr *) 
                  mycalloc(eqn_count + 1, sizeof(m_data_ptr));

  for (i = 0; i <= A->dim; i++)
  {
    temp_ptr = A->rowptr[i];
    while (temp_ptr != NULL)
    {
      if ((i <= A->lu_stop) && (temp_ptr->col <= A->lu_stop))
      {
        A->r_count[i]++;
        A->c_count[temp_ptr->col]++;
      }

      if (A->colptr[temp_ptr->col] == NULL)
      {
        A->colptr[temp_ptr->col] = temp_ptr;
        temp_colptr[temp_ptr->col] = temp_ptr;
      } 
      else
      {
        temp_colptr[temp_ptr->col]->n_row = temp_ptr;
        temp_colptr[temp_ptr->col] = temp_ptr;
      }
      temp_ptr = temp_ptr->n_col;
    }   /* while */
  }    /* for */

  free((char *) temp_colptr);

}    /* create_rowptr */


void
Axy(matrix *A, int x, int y, double val)     /* A(x, y) = val */
{
  m_data *temp;
  int exist;

  temp = add_i_j(A, x, y, &exist);
  temp->val = val;
  temp->lu_val = val;
  temp->type = ORIGINAL;
}   /* Axy */


void
ex_row(matrix *A, int ra, int rb, int need_diag)
{
  m_data *r1_ptr, *r2_ptr; 
  m_data *r1_r1, *r1_pre, *r2_r2, *r2_pre;
  m_data *temp;
  int r1, r2, temp_int;
  
  if (ra == rb) return;
  else if (ra < rb) 
  {
    r1 = ra;
    r2 = rb;
  }
  else 
  {
    r1 = rb;
    r2 = ra;
  }

  r1_ptr = A->rowptr[r1];
  r2_ptr = A->rowptr[r2];
 
  if ((r1_ptr == NULL) && (r2_ptr == NULL)) return;

  while ((r1_ptr != NULL) || (r2_ptr != NULL))
  {
    if ((r1_ptr != NULL) && (r2_ptr != NULL))
    {  if (r1_ptr->col <= r2_ptr->col) temp = A->colptr[r1_ptr->col];
       else temp = A->colptr[r2_ptr->col];
    }
    else if (r1_ptr != NULL) temp = A->colptr[r1_ptr->col];
    else temp = A->colptr[r2_ptr->col];

    if (temp->row > r1) r1_r1 = r1_pre = NULL;
    else 
    {  
      while (temp->n_row != NULL)
        if (temp->n_row->row >= r1) break;
        else temp = temp->n_row;

      if (temp->row == r1)
      {
        r1_pre = NULL;
        r1_r1 = temp;
      }
      else 
      {
        r1_pre = temp;
        if (temp->n_row == NULL) r1_r1 = NULL;
        else if (temp->n_row->row > r1) r1_r1 = NULL;
        else r1_r1 = temp->n_row;
      }
    }
     
    if (temp->row > r2) r2_r2 = r2_pre = NULL;
    else
    {  
      while (temp->n_row != NULL)
        if (temp->n_row->row >= r2) break;
        else temp = temp->n_row;

      if (temp->row == r2)
      {
        r2_pre = NULL;
        r2_r2 = temp;
      }
      else 
      {
        r2_pre = temp;
        if (temp->n_row == NULL) r2_r2 = NULL;
        else if (temp->n_row->row > r2) r2_r2 = NULL;
        else r2_r2 = temp->n_row;
      }
    }
     
    if (r2_pre == NULL) goto advance;
    if ((r2_r2 == NULL) && (r2_pre == r1_r1)) goto advance;
    
    if (r2_r2 == NULL)
    {
      if (r1_pre == NULL) A->colptr[r1_r1->col] = r1_r1->n_row;
      else r1_pre->n_row = r1_r1->n_row;
      r1_r1->n_row = r2_pre->n_row;
      r2_pre->n_row = r1_r1;
    }
    else if (r1_r1 == NULL)
    {
      r2_pre->n_row = r2_r2->n_row;
      if (r1_pre == NULL)
      {
        r2_r2->n_row = A->colptr[r2_r2->col];
        A->colptr[r2_r2->col] = r2_r2;
      }
      else 
      {
        r2_r2->n_row = r1_pre->n_row;
        r1_pre->n_row = r2_r2;
      }
    }   
    else if (r2_pre == r1_r1)
    {
      r1_r1->n_row = r2_r2->n_row;
      r2_r2->n_row = r1_r1;
      if (r1_pre != NULL) r1_pre->n_row = r2_r2;
      else A->colptr[r1_r1->col] = r2_r2;
    }
    else
    {
      temp = r1_r1->n_row;
      r1_r1->n_row = r2_r2->n_row;
      r2_r2->n_row = temp;
      r2_pre->n_row = r1_r1;
      if (r1_pre != NULL) r1_pre->n_row = r2_r2;
      else A->colptr[r1_r1->col] = r2_r2;
    }

    advance:

    if (r1_r1 != NULL) r1_r1->row = r2;
    if (r2_r2 != NULL) r2_r2->row = r1; 
     
    if ((r1_ptr != NULL) && (r2_ptr == NULL)) r1_ptr = r1_ptr->n_col;
    else if ((r1_ptr == NULL) && (r2_ptr != NULL))
      r2_ptr = r2_ptr->n_col;
    else if (r1_ptr->col < r2_ptr->col) r1_ptr = r1_ptr->n_col;
    else if (r1_ptr->col > r2_ptr->col) r2_ptr = r2_ptr->n_col;
    else 
    {
      r1_ptr = r1_ptr->n_col;
      r2_ptr = r2_ptr->n_col;
    }      
        
  }    /* while */

  temp = A->rowptr[r1];
  A->rowptr[r1] = A->rowptr[r2];
  A->rowptr[r2] = temp;

  temp_int = A->r_order[r1];
  A->r_order[r1] = A->r_order[r2];
  A->r_order[r2] = temp_int;

  temp_int = A->r_count[r1];
  A->r_count[r1] = A->r_count[r2];
  A->r_count[r2] = temp_int;

  if (need_diag == 1)
  {
    temp = A->rowptr[r1];
    while (temp != NULL)
    {
      if (temp->col >= r1) break;
      temp = temp->n_col;
    }
    if (temp != NULL)
      if (temp->col == r1) A->diagptr[r1] = temp;
      else A->diagptr[r1] = NULL;
    else A->diagptr[r1] = NULL;

    temp = A->rowptr[r2];
    while (temp != NULL)
    {
      if (temp->col >= r2) break;
      temp = temp->n_col;
    }
    if (temp != NULL)
      if (temp->col == r2) A->diagptr[r2] = temp;
      else A->diagptr[r2] = NULL;
    else A->diagptr[r2] = NULL;
  }

}    /* ex_row */


void
ex_col(matrix *A, int ca, int cb, int need_diag)
{
  m_data *c1_ptr, *c2_ptr; 
  m_data *c1_c1, *c1_pre, *c2_c2, *c2_pre;
  m_data *temp;
  int c1, c2, temp_int;
  long temp_long;
  
  if (ca == cb) return;
  else if (ca < cb) 
  {
    c1 = ca;
    c2 = cb;
  }
  else 
  {
    c1 = cb;
    c2 = ca;
  }

  c1_ptr = A->colptr[c1];
  c2_ptr = A->colptr[c2];
 
  if ((c1_ptr == NULL) && (c2_ptr == NULL)) return;

  while ((c1_ptr != NULL) || (c2_ptr != NULL))
  {
    if ((c1_ptr != NULL) && (c2_ptr != NULL))
    {  if (c1_ptr->row <= c2_ptr->row) temp = A->rowptr[c1_ptr->row];
       else temp = A->rowptr[c2_ptr->row];
    }
    else if (c1_ptr != NULL) temp = A->rowptr[c1_ptr->row];
    else temp = A->rowptr[c2_ptr->row];

    if (temp->col > c1) c1_c1 = c1_pre = NULL;
    else 
    {  
      while (temp->n_col != NULL)
        if (temp->n_col->col >= c1) break;
        else temp = temp->n_col;

      if (temp->col == c1)
      {
        c1_pre = NULL;
        c1_c1 = temp;
      }
      else 
      {
        c1_pre = temp;
        if (temp->n_col == NULL) c1_c1 = NULL;
        else if (temp->n_col->col > c1) c1_c1 = NULL;
        else c1_c1 = temp->n_col;
      }
    }
     
    if (temp->col > c2) c2_c2 = c2_pre = NULL;
    else
    {  
      while (temp->n_col != NULL)
        if (temp->n_col->col >= c2) break;
        else temp = temp->n_col;

      if (temp->col == c2)
      {
        c2_pre = NULL;
        c2_c2 = temp;
      }
      else 
      {
        c2_pre = temp;
        if (temp->n_col == NULL) c2_c2 = NULL;
        else if (temp->n_col->col > c2) c2_c2 = NULL;
        else c2_c2 = temp->n_col;
      }
    }
     
    if (c2_pre == NULL) goto advance;
    if ((c2_c2 == NULL) && (c2_pre == c1_c1)) goto advance;
    
    if (c2_c2 == NULL)
    {
      if (c1_pre == NULL) A->rowptr[c1_c1->row] = c1_c1->n_col;
      else c1_pre->n_col = c1_c1->n_col;
      c1_c1->n_col = c2_pre->n_col;
      c2_pre->n_col = c1_c1;
    }
    else if (c1_c1 == NULL)
    {
      c2_pre->n_col = c2_c2->n_col;
      if (c1_pre == NULL)
      {
        c2_c2->n_col = A->rowptr[c2_c2->row];
        A->rowptr[c2_c2->row] = c2_c2;
      }
      else 
      {
        c2_c2->n_col = c1_pre->n_col;
        c1_pre->n_col = c2_c2;
      }
    }   
    else if (c2_pre == c1_c1)
    {
      c1_c1->n_col = c2_c2->n_col;
      c2_c2->n_col = c1_c1;
      if (c1_pre != NULL) c1_pre->n_col = c2_c2;
      else A->rowptr[c1_c1->row] = c2_c2;
    }
    else
    {
      temp = c1_c1->n_col;
      c1_c1->n_col = c2_c2->n_col;
      c2_c2->n_col = temp;
      c2_pre->n_col = c1_c1;
      if (c1_pre != NULL) c1_pre->n_col = c2_c2;
      else A->rowptr[c1_c1->row] = c2_c2;
    }

    advance:

    if (c1_c1 != NULL) c1_c1->col = c2;
    if (c2_c2 != NULL) c2_c2->col = c1; 
     
    if ((c1_ptr != NULL) && (c2_ptr == NULL)) c1_ptr = c1_ptr->n_row;
    else if ((c1_ptr == NULL) && (c2_ptr != NULL))
      c2_ptr = c2_ptr->n_row;
    else if (c1_ptr->row < c2_ptr->row) c1_ptr = c1_ptr->n_row;
    else if (c1_ptr->row > c2_ptr->row) c2_ptr = c2_ptr->n_row;
    else 
    {
      c1_ptr = c1_ptr->n_row;
      c2_ptr = c2_ptr->n_row;
    }      
        
  }    /* while */

  temp = A->colptr[c1];
  A->colptr[c1] = A->colptr[c2];
  A->colptr[c2] = temp;

  temp_int = A->c_order[c1]; 
  A->c_order[c1] = A->c_order[c2];
  A->c_order[c2] = temp_int;

  temp_int = A->c_count[c1];
  A->c_count[c1] = A->c_count[c2];
  A->c_count[c2] = temp_int;

  if (need_diag == 1)
  {
    temp = A->colptr[c1];
    while (temp != NULL)
    {
      if (temp->row >= c1) break;
      temp = temp->n_row;
    }
    if (temp != NULL)
      if (temp->row == c1) A->diagptr[c1] = temp;
      else A->diagptr[c1] = NULL;
    else A->diagptr[c1] = NULL;

    temp = A->colptr[c2];
    while (temp != NULL)
    {
      if (temp->row >= c2) break;
      temp = temp->n_row;
    }
    if (temp != NULL)
      if (temp->row == c2) A->diagptr[c2] = temp;
      else A->diagptr[c2] = NULL;
    else A->diagptr[c2] = NULL;
  }
}    /* ex_col */    


s_data *
add_source(source *b, int i)
{
   if (b[i] != NULL) return(b[i]);
   
   b[i] = (s_data *) mycalloc(1, sizeof(s_data));
   b[i]->val = 0.0;
   b[i]->val_nonlin = 0.0;
   return(b[i]);

}  /* add_source */


x_data *
add_unknown(unknown *x, int i)
{
  int tempint;

  if (x[i] != NULL) return(x[i]);

  x[i] = (x_data *) mycalloc(1, sizeof(x_data));
  return(x[i]);

}  /* add_unknown */


void
ex_unknown(unknown *x, int xa, int xb)
{
  x_data *temp_x;
  
  temp_x = x[xa];
  x[xa] = x[xb];
  x[xb] = temp_x;

}   /* ex_unknown */


void
ex_source(source *b, int ba, int bb)
{
  s_data *temp_s;

  temp_s = b[ba];
  b[ba] = b[bb];
  b[bb] = temp_s;

}  /* ex_source */


/* LU decomposition of A, with 1's on L's diagonal */
void
A_LU(matrix *A, int pivot_type, int allow_fillin)
{
  m_data *pivot, *next_r, *temp_ptr1, *temp_ptr2;
  int i, j, pivot_row_count;
  int d_pivot_failed;
  double temp_val;
  pivot_row_data  *pivot_row;
  int tempint;

  d_pivot_failed = 0;
  pivot_row = (pivot_row_data *) 
               mycalloc(eqn_count + 1, sizeof(pivot_row_data)); 

  for (i = 0; i <= A->lu_stop - 1; i++)
  {
     if (pivot_type == PIVOT_ROW) 
       pivot = row_pivot(A, i, allow_fillin);
     else if (pivot_type == PIVOT_FULL) 
       pivot = full_pivot(A, i, allow_fillin);
     else if (pivot_type == PIVOT_DIAG)
     {
       if (d_pivot_failed == 0) 
       {
         pivot = diag_pivot(A, i);
         if (pivot == NULL)
         {
           d_pivot_failed = 1;
           pivot = row_pivot(A, i, allow_fillin);
         }
       }
       else pivot = row_pivot(A, i, allow_fillin);
     }

     temp_ptr1 = pivot->n_col;
     pivot_row_count = -1;
     while (temp_ptr1 != NULL)
     {
       pivot_row_count++;
       pivot_row[pivot_row_count].pivot_data =
            pivot_row[pivot_row_count].next_data = temp_ptr1;
       temp_ptr1 = temp_ptr1->n_col;
     }

     next_r = pivot->n_row;
     while (next_r != NULL)
     {
       if (next_r->row > A->lu_stop) break;
       temp_val = next_r->lu_val/pivot->lu_val;
/*
       lu_opcount++;
*/
       next_r->lu_val = temp_val;
       temp_val = 0.0 - temp_val;
       temp_ptr2 = next_r;
    
       for (j = 0; j <= pivot_row_count; j++)
       {
         while (pivot_row[j].next_data->n_row != NULL)
         {
           if (pivot_row[j].next_data->n_row->row >= next_r->row) 
               break; 
           pivot_row[j].next_data = pivot_row[j].next_data->n_row;
         }
         if (pivot_row[j].next_data->n_row != NULL)
           if (pivot_row[j].next_data->n_row->row == next_r->row)
           {
             temp_ptr1 = pivot_row[j].next_data->n_row;
             goto skip_A_LU;
            }

         while (temp_ptr2->n_col != NULL)
         {
           if (temp_ptr2->n_col->col > pivot_row[j].pivot_data->col) 
               break;
           temp_ptr2 = temp_ptr2->n_col;
         }
         temp_ptr1 = add_next(A, pivot_row[j].next_data, temp_ptr2, 
                            next_r->row, pivot_row[j].pivot_data->col);
         temp_ptr1->type = FILLIN;
         temp_ptr1->lu_val = 0.0;

    skip_A_LU:

         if (temp_ptr1->type == NOTUSED)
         {
           temp_ptr1->type = FILLIN;
           temp_ptr1->lu_val = 0.0;
           A->r_count[temp_ptr1->row]++;
           A->c_count[temp_ptr1->col]++;
           if (temp_ptr1->row == temp_ptr1->col) 
               A->diagptr[temp_ptr1->row] = temp_ptr1;
         }

         temp_ptr1->lu_val = temp_ptr1->lu_val + 
                             temp_val*pivot_row[j].pivot_data->lu_val;
/*
         lu_opcount++;
*/
       }
       next_r = next_r->n_row;
     }  /* while */
     
     temp_ptr1 = A->diagptr[i]->n_col;
     while (temp_ptr1 != NULL)
     {
       if ((temp_ptr1->col <= A->lu_stop) && (temp_ptr1->col >= 0))
       {
         (A->c_count[temp_ptr1->col])--;
         temp_ptr1 = temp_ptr1->n_col;
       }
       else break;
     }

     temp_ptr1 = A->diagptr[i]->n_row;
     while (temp_ptr1 != NULL)
     if (temp_ptr1->row <= A->lu_stop)
     {
       A->r_count[temp_ptr1->row]--;
       temp_ptr1 = temp_ptr1->n_row;
     }
     else break;

  }  /* for */

  free((char *) pivot_row);

}     /* A_LU */


m_data *
full_pivot(matrix *A, int i, int allow_fillin)
{
  m_data *temp_ptr, *max_ptr, *allmin_ptr, *start_ptr, *min_ptr;
  long min_val, allmin_val, temp_val;
  double temp_max;
  int j;

  allmin_ptr = NULL;

  for (j = i; j <= A->lu_stop; j++)
  {
    temp_ptr = A->colptr[j];
    while (temp_ptr != NULL)
    {
      if ((temp_ptr->row >= i) && 
          (temp_ptr->type != NOTUSED)) break;
      else temp_ptr = temp_ptr->n_row;
    }
    
    if (temp_ptr == NULL) sperror(M_SINGULAR);
    if (temp_ptr->row > A->lu_stop) sperror(M_SINGULAR);

    start_ptr = temp_ptr;
    max_ptr = temp_ptr;
    min_ptr = temp_ptr;
    min_val = (A->r_count[temp_ptr->row] - 1);
/*
 *(A->c_count[j] - 1);
 */
  
    temp_ptr = temp_ptr->n_row; 
    while (temp_ptr != NULL)
    {
      if (temp_ptr->row > A->lu_stop) break;
      if ((myabs(temp_ptr->lu_val) > myabs(max_ptr->lu_val))
          && (temp_ptr->type != NOTUSED)) max_ptr = temp_ptr;
      temp_ptr = temp_ptr->n_row;
    }

    temp_max = PIVOT_MIN + PIVOT_REL*myabs(max_ptr->lu_val);

    temp_ptr = start_ptr->n_row;
    while (temp_ptr != NULL)
    {
      if (temp_ptr->row > A->lu_stop) break;
      temp_val = (A->r_count[temp_ptr->row] - 1);
/*
 *(A->c_count[j] - 1); 
 */

      if ((myabs(temp_ptr->lu_val) >  temp_max) && 
          (temp_ptr->type != NOTUSED))
      {
        if (allow_fillin == FALSE)   
        {
          if ((temp_val <= min_val) && 
            ((temp_ptr->type != FILLIN) || (min_ptr->type == FILLIN)))
          {
            if ((temp_val < min_val) || 
              (myabs(temp_ptr->lu_val) > myabs(min_ptr->lu_val)))
            {
              min_ptr = temp_ptr;
              min_val = temp_val;
            }
          }
          else if ((temp_ptr->type != FILLIN) && (min_ptr->type == FILLIN))
          {
            min_ptr = temp_ptr;
            min_val = temp_val;
          }
        }
        else  /* don't care about pivot on fill-in */
        {
          if ((temp_val < min_val) || ((temp_val == min_val) &&
              (myabs(temp_ptr->lu_val) > myabs(min_ptr->lu_val))))
          {
            min_ptr = temp_ptr;
            min_val = temp_val;
          }
        }    /* end of if-else of allow_fillin == FALSE */
      } /* end of if */

      temp_ptr = temp_ptr->n_row;
     
    }   /* while */

    min_val = min_val*(A->c_count[j] - 1);

    if (myabs(min_ptr->lu_val) >= PIVOT_MIN)
    {
      if (allmin_ptr == NULL)
      {
        allmin_ptr = min_ptr;
        allmin_val = min_val;
      }
      else
      {
        if (allow_fillin == FALSE)   
        {
          if ((min_val <= allmin_val) && 
            ((min_ptr->type != FILLIN) || (allmin_ptr->type == FILLIN)))
          {
            if ((min_val < allmin_val) || 
              (myabs(min_ptr->lu_val) > myabs(allmin_ptr->lu_val)))
            {
              allmin_ptr = min_ptr;
              allmin_val = min_val;
            }
          }
          else if ((min_ptr->type != FILLIN) && (allmin_ptr->type == FILLIN))
          {
            allmin_ptr = min_ptr;
            allmin_val = min_val;
          }
        }
        else  /* don't care about pivot on fill-in */
        {
          if ((min_val < allmin_val) || ((min_val == allmin_val) &&
            (myabs(min_ptr->lu_val) > myabs(allmin_ptr->lu_val))))
          {
            allmin_ptr = min_ptr;
            allmin_val = min_val;
          }
        }    /* end of if-else of allow_fillin == FALSE */
      } /* end of else */
    } /* end of if */

  }  /* for */
    
  if (allmin_ptr == NULL) sperror(M_SINGULAR);
   
  if (allmin_ptr->row != i) 
  { 
/*
    printf("exchange row %d %d %d\n", 
            i, allmin_ptr->row, A->r_order[allmin_ptr->row]);
*/
    ex_source(b_source, i, allmin_ptr->row);
    ex_row(A, i, allmin_ptr->row, 1);
  }
  
  if (allmin_ptr->col != i)
  {
/*
    printf("exchange col %d %d %d\n", 
            i, allmin_ptr->col, A->c_order[allmin_ptr->col]);
*/
    ex_unknown(x_unknown, i, allmin_ptr->col);
    ex_col(A, i, allmin_ptr->col, 1);
  }

  return(allmin_ptr);

}  /* full_pivot */


m_data *
row_pivot(matrix *A, int i, int allow_fillin)
{
  m_data *temp_ptr, *max_ptr, *min_ptr, *start_ptr;
  long min_val, temp_val;
  double temp_max;
  int j;

  if (A->diagptr[i] != NULL) temp_ptr = A->diagptr[i];
  else
  {
    temp_ptr = A->colptr[i];
    while (temp_ptr != NULL)
    {
      if ((temp_ptr->row >= i) && (temp_ptr->type != NOTUSED)) break;
      else temp_ptr = temp_ptr->n_row;
    }
  } 

  if (temp_ptr == NULL) sperror(M_SINGULAR);
  if (temp_ptr->row > A->lu_stop) sperror(M_SINGULAR);

  start_ptr = temp_ptr;
  max_ptr = temp_ptr;
  min_ptr = temp_ptr;
  min_val = (A->r_count[temp_ptr->row] - 1);
/*
 *(A->c_count[i] - 1);
 */
  
  temp_ptr = temp_ptr->n_row; 
  while (temp_ptr != NULL)
  {
    if (temp_ptr->row > A->lu_stop) break;
    if ((myabs(temp_ptr->lu_val) > myabs(max_ptr->lu_val))
        && (temp_ptr->type != NOTUSED)) max_ptr = temp_ptr;
    temp_ptr = temp_ptr->n_row;
  }

  temp_max = PIVOT_MIN + PIVOT_REL*myabs(max_ptr->lu_val);

  temp_ptr = start_ptr->n_row;
  while (temp_ptr != NULL)
  {
    if (temp_ptr->row > A->lu_stop) break;
    temp_val = (A->r_count[temp_ptr->row] - 1);
/*
 *(A->c_count[i] - 1); 
 */

    if ((myabs(temp_ptr->lu_val) > temp_max) && 
         (temp_ptr->type != NOTUSED))
    {
      if (allow_fillin == FALSE)   
      {
        if ((temp_val <= min_val) && 
          ((temp_ptr->type != FILLIN) || (min_ptr->type == FILLIN)))
        {
          if ((temp_val < min_val) || 
            (myabs(temp_ptr->lu_val) > myabs(min_ptr->lu_val)))
          {
            min_ptr = temp_ptr;
            min_val = temp_val;
          }
        }
        else if ((temp_ptr->type != FILLIN) && (min_ptr->type == FILLIN))
        {
          min_ptr = temp_ptr;
          min_val = temp_val;
        }
      }
      else  /* don't care about pivot on fill-in */
      {
        if ((temp_val < min_val) || ((temp_val == min_val) &&
          (myabs(temp_ptr->lu_val) > myabs(min_ptr->lu_val))))
        {
          min_ptr = temp_ptr;
          min_val = temp_val;
        }
      }    /* end of if-else of allow_fillin == FALSE */
    } /* end of if */

    temp_ptr = temp_ptr->n_row;
  }   /* while */

  if (myabs(min_ptr->lu_val) <= PIVOT_MIN)
  {
    j = 0;
    while (A->r_order[j] != min_ptr->row) j++;

    printf("pivot val too small %d %e\n", j, min_ptr->lu_val);
    sperror(M_SINGULAR);
  }
 
  if (min_ptr->row != i) 
  { 
/*
    printf("exchange row %d %d %d\n", 
            i, min_ptr->row, A->r_order[min_ptr->row]);
*/
    ex_source(b_source, i, min_ptr->row);
    ex_row(A, i, min_ptr->row, 1);
  }

  return(min_ptr);

}  /* row_pivot */


m_data *
diag_pivot(matrix *A, int i)
{
  m_data *temp_ptr, *pivot, *max_ptr;
  int j, min; 
  long min_val, temp_val;
 
  if (i == A->lu_stop) return(A->diagptr[i]);
  
  min = -1;
  pivot = NULL;
  
  for (j = i; j <= A->lu_stop; j++)
  if (A->diagptr[j] != NULL) 
/*
  if ((A->diagptr[j]->type == ORIGINAL) || 
      (A->diagptr[i]->type == CONST_ORIGINAL))
*/
  {
    temp_val = (A->r_count[j] - 1)*(A->c_count[j] - 1);
    if (((temp_val < min_val) || (min < 0)) && (temp_val >= 0))
    {
      temp_ptr = A->colptr[j];
      while (temp_ptr != NULL)
      {
        if (temp_ptr->row >= i) break;
        temp_ptr = temp_ptr->n_row;
      }
      max_ptr = temp_ptr;
      temp_ptr = temp_ptr->n_row;
      while (temp_ptr != NULL)
      {
        if (temp_ptr->row > A->lu_stop) break;
        if ((myabs(temp_ptr->lu_val) > myabs(max_ptr->lu_val))
            && (temp_ptr->type != NOTUSED))
            max_ptr = temp_ptr;
        temp_ptr = temp_ptr->n_row;
      } 
      if (myabs(A->diagptr[j]->lu_val) > PIVOT_MIN + 
          PIVOT_REL*myabs(max_ptr->lu_val))
      {
        min_val = temp_val;
        min = j;
      }
    }
  }  /* for if */  
    
  if (min < 0) return(NULL);
  if (min != i)
  {
    ex_source(b_source, i, min);
    ex_row(A, i, min, 0);
    ex_unknown(x_unknown, i, min);
    ex_col(A, i, min, 0);
    temp_ptr = A->diagptr[i];
    A->diagptr[i] = A->diagptr[min];
    A->diagptr[min] = temp_ptr;
/*
    printf("exchange diag %d %d\n", i, min);
*/
  }

  pivot = A->diagptr[i];
  return(pivot);

}  /* diag_pivot */


void
solve_Ly_b(matrix *A, unknown *x, source *b)
{
  int i;
  m_data *temp;
  double temp_x;

  for (i = 0; i <= A->lu_stop; i++)
  {
/*
    if (b[A->r_order[i]] == NULL) temp_x = 0.0;
    else temp_x = b[A->r_order[i]]->val +
                  b[A->r_order[i]]->val_nonlin; 
*/
    if (b[i] == NULL) temp_x = 0.0;
    else temp_x = b[i]->val + b[i]->val_nonlin; 

    temp = A->rowptr[i];

/*
    if (temp == NULL) sperror(LU_FAILED);
*/
    
    while (temp->col < i) 
    {
      if (temp->type != NOTUSED)
        temp_x = temp_x - temp->lu_val*x[temp->col]->yn;
   
      temp = temp->n_col;
    }
   
    x[i]->yn = temp_x;
  }

}    /* solve_Ly_b */


void
solve_Ux_y(matrix *A, unknown *x)
{
  int i;
  m_data *temp;
  double temp_x, divider;

  for (i = A->lu_stop; i >= 0; i--)
  {
    temp_x = x[i]->yn;
    
    temp = A->diagptr[i];

/*
    if (temp == NULL) sperror(LU_FAILED);
*/

    divider =  temp->lu_val;
   
    temp = temp->n_col;
    while (temp != NULL)
    {
      if (temp->col > A->lu_stop) break;
      
      if (temp->type != NOTUSED)
        temp_x = temp_x - temp->lu_val*x[temp->col]->xn;
   
      temp = temp->n_col; 
    }
    
    x[i]->xn = temp_x/divider;

  }    /* for */
  
}    /* solve_Ux_y */


void
matrix_clear()
{
  int i;
  m_data *temp;
  
  for (i = 0; i <= A_matrix->dim; i++)
  {
    temp = A_matrix->rowptr[i];
    while (temp != NULL)
    {
      if (temp->type != CONST_ORIGINAL) temp->val = 0.0; 
      temp = temp->n_col;
    }
  }

}   /* matrix_clear */


void
LU_refresh()
{
  int i;
  m_data *temp;
  
  for (i = 0; i <= A_matrix->lu_stop; i++)
  {
    A_matrix->r_count[i] = 0;
    A_matrix->c_count[i] = 0;
  }
  
  for (i = 0; i <= A_matrix->lu_stop; i++)
  {
    temp = A_matrix->rowptr[i];
    while (temp != NULL)
    {
      if ((temp->type == CONST_ORIGINAL) || (temp->type == ORIGINAL))
      {
        temp->lu_val = temp->val; 
        A_matrix->r_count[i]++;
        A_matrix->c_count[temp->col]++;
      }
      else if (temp->type == FILLIN) 
      {
        temp->type = NOTUSED;
        if (temp->row == temp->col) A_matrix->diagptr[temp->row] = NULL;
      }
      temp = temp->n_col;
    }
  }
  

}   /* LU_refresh */


void
clear_source()
{
  long i;

  for (i = 0; i <= eqn_count; i++)
  if (b_source[i] != NULL) 
  {
    b_source[i]->val = 0.0;
    b_source[i]->val_nonlin = 0.0;
  }

}   /* refresh_source */


void
clear_nonlin_source()
{

  long i;
  
  for (i = 0; i <= eqn_count; i++)
  if (b_source[i] != NULL) b_source[i]->val_nonlin = 0.0;

}  /* clear_nonlin_source */
