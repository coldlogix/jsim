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
read_transline()
{
  long name;
  modeldata *modptr;
  int bad_dev;
  int type;

  bad_dev = FALSE;

  intptr[1] = (long) NULL;

  read_error = read_long(nodeptr, "plus1 node", FALSE);
  read_error = read_long((nodeptr+1), "minus1 node", FALSE); 
  read_error = read_long((nodeptr+2), "plus2 node", FALSE);
  read_error = read_long((nodeptr+3), "minus2 node", FALSE); 
  read_error = read_string("line type");

  if (read_error != OK)
  {
    printf("## Error -- bad transmission line definition\n");
    no_go = TRUE;
    bad_dev = TRUE;
  }
  
  if (read_error == OK)
  switch(get_string_keyword(tempstring))
  {
    case LOSSLESS : 

         type = TRAN_NO_LOSS;

         *dataptr = 50.0;
         *(dataptr + 1) = 1.0;
         *(dataptr + 2) = 0.0;
         *(dataptr + 3) = 0.0;
         *(dataptr + 4) = 0.0;
         *(dataptr + 5) = 0.0;

         while ((read_error = read_string("")) == OK)
         {
           switch(get_string_keyword(tempstring))
           {
             case Z0 : 

                  read_error = read_double(dataptr, "Z0", FALSE);
                  if (read_error != OK)
                  {
                    printf("## Warning -- bad Z0\n");
                    warned = TRUE;
                  }
                  else if (*dataptr < DEF_ZO_MIN)
                  { 
                    printf("## Warning -- zo too small\n");
                    warned = TRUE;
                    *dataptr = DEF_ZO_MIN;
                  }
                  break;
    
             case TD : 

                  read_error = read_double(dataptr+1, "TD", FALSE);
                  if (read_error != OK)
                  {
                    printf("## Warning -- bad TD\n");
                    warned = TRUE;
                  }
                  else if (*(dataptr+1) < DEF_TD_MIN)
                  {
                    printf("## Warning -- td too small\n");
                    warned = TRUE;
                    *(dataptr+1) = DEF_TD_MIN;
                  }
                  break;

             case IC : 

                  read_error = read_double(dataptr+2, "V1", FALSE);
                  read_error = read_double(dataptr+3, "I1", FALSE);
                  read_error = read_double(dataptr+4, "V2", FALSE);
                  read_error = read_double(dataptr+5, "I2", FALSE);
                  if (read_error != OK)
                  {
                    printf("## Warning -- bad IC's\n");
                    warned = TRUE;
                  }
                  break;
      
             default : 

                  printf("## Warning -- illegal keyword %s\n", 
                          tempstring);
                  warned = TRUE;
                  break;

           }   /* switch */

           read_error = OK;

         }  /* while */

         break;

    default : 

         printf("## Error -- illegal line type %s\n", tempstring);
         no_go = TRUE;
         bad_dev = TRUE;
         break;
   
  }  /* switch */


  intptr[0] = LATER;

  if (bad_dev == FALSE)
  {
    if (current_sub_def == NULL)
    {
      name = add_realname(dev_name);
      xline_to_devlist(name, type, nodeptr, dataptr);
    }
    else
      xline_to_deftree(dev_name, type, nodeptr, dataptr);
  }

}   /* read_transline */


device *
xline_to_devlist(long name, int type, long *nodeptr, double *dataptr)
{
  long n1_plus, n2_plus, n1_minus, n2_minus;
  branch_marker *bmarker;
  device *temp_dev;

  switch (type)
  {
    case TRAN_NO_LOSS : 

         bmarker = add_bmarker();
         n1_plus = add_node(*nodeptr, name, type, PLUS_NODE, bmarker);
         n1_minus = add_node(*(nodeptr+1), name, type, MINUS_NODE, 
                             bmarker);

         bmarker = add_bmarker();
         n2_plus = add_node(*(nodeptr+2), name, type, PLUS_NODE,
                            bmarker);
         n2_minus = add_node(*(nodeptr+3), name, type, MINUS_NODE, 
                             bmarker);

         bmarker = add_bmarker();
         if (*nodeptr != *(nodeptr+2))
         {
           if (*nodeptr != *(nodeptr+2))
           {
             add_node(*nodeptr, name, LOSSLESS_LINE, PLUS_NODE,
                      bmarker);
             add_node(*(nodeptr+2), name, LOSSLESS_LINE, MINUS_NODE, 
                      bmarker);
           }
         }

         if (*(nodeptr+1) != *(nodeptr+3))
         {
           bmarker = add_bmarker();
           if (type == TRAN_NO_LOSS)
           {
             add_node(*(nodeptr+1), name, LOSSLESS_LINE, PLUS_NODE,
                      bmarker);
             add_node(*(nodeptr+3), name, LOSSLESS_LINE, MINUS_NODE, 
                      bmarker);
           }
         }

         return(add_transline(name, type, dataptr, n1_plus, 
                              n1_minus, n2_plus, n2_minus));
          
  }  /* switch */

  return(NULL);

}   /* xline_to_devlist */ 


device *
xline_to_deftree(char *dev_name, int type, long *nodeptr, double *dataptr)
{
  long name;

  name = (long) dev_name;

  switch (type)
  {
    case TRAN_NO_LOSS : 
         return(add_transline(name, type, dataptr, *nodeptr, 
                              *(nodeptr+1), *(nodeptr+2), 
                              *(nodeptr+3)));
    
  }

  return(NULL);

}   /* xline_to_deftree */   


device *
xlinesub_to_devlist(char *dev_name, int type, char *data)
{
  int n1_plus, n1_minus, n2_plus, n2_minus;
  int n1_p_save, n1_m_save, n2_p_save, n2_m_save;
  double temp_data[7];
  long name;
  branch_marker *bmarker;

  name = add_realname(combine_string(namestring, dev_name));

  switch (type)
  {
    case TRAN_NO_LOSS  :
         bmarker = add_bmarker(); 
         n1_plus = n1_p_save =
           getnode_subnode(((sub_tran_noloss *) data)->n1_plus,
                           ((sub_tran_noloss *) data)->p1_node);
         n1_plus = add_node(n1_plus, name, type, PLUS_NODE, bmarker);

         n1_minus = n1_m_save =
           getnode_subnode(((sub_tran_noloss *) data)->n1_minus,
                           ((sub_tran_noloss *) data)->m1_node);
         n1_minus = add_node(n1_minus, name, type, MINUS_NODE, bmarker);

         bmarker = add_bmarker(); 
         n2_plus = n2_p_save =
           getnode_subnode(((sub_tran_noloss *) data)->n2_plus,
                           ((sub_tran_noloss *) data)->p2_node);
         n2_plus = add_node(n2_plus, name, type, PLUS_NODE, bmarker);
         n2_minus = n2_m_save =
           getnode_subnode(((sub_tran_noloss *) data)->n2_minus,
                           ((sub_tran_noloss *) data)->m2_node);
         n2_minus = add_node(n2_minus, name, type, MINUS_NODE, bmarker);

        if (n1_p_save != n2_p_save)
        {
          bmarker = add_bmarker();
          add_node(n1_p_save, name, LOSSLESS_LINE, PLUS_NODE, bmarker);
          add_node(n2_p_save, name, LOSSLESS_LINE, MINUS_NODE, bmarker);
        }

        if (n1_m_save != n2_m_save)
        {
          bmarker = add_bmarker();
          add_node(n1_m_save, name, LOSSLESS_LINE, PLUS_NODE, bmarker);
          add_node(n2_m_save, name, LOSSLESS_LINE, MINUS_NODE, bmarker);
        }

        temp_data[0] = ((sub_tran_noloss *) data)->zo;
        temp_data[1] = ((sub_tran_noloss *) data)->td;
        temp_data[2] = ((sub_tran_noloss *) data)->v1o;
        temp_data[3] = ((sub_tran_noloss *) data)->i1o;
        temp_data[4] = ((sub_tran_noloss *) data)->v2o;
        temp_data[5] = ((sub_tran_noloss *) data)->i2o;

        return(add_transline(name, TRAN_NO_LOSS, temp_data, 
                 n1_plus, n1_minus, n2_plus, n2_minus));

  }  /* switch */

  return(NULL);

}   /* xlinesub_to_devlist */


device *
add_transline(long name, int type, double *dataptr, long n1_plus,
    long n1_minus, long n2_plus, long n2_minus)
{
  int i;
  char *data;
  dev_tran_noloss *temp;
  sub_tran_noloss *temp_sub;
   
  if (current_sub_def == NULL)
  {
    if (type == TRAN_NO_LOSS)
    {
      temp = (dev_tran_noloss *) mycalloc(1, sizeof(dev_tran_noloss));
  
      temp->zo = *dataptr;
      temp->td = *(dataptr + 1);
      temp->v1o = *(dataptr + 2);
      temp->i1o = *(dataptr + 3);
      temp->v1o = *(dataptr + 4);
      temp->i1o = *(dataptr + 5);
      temp->n1_plus = n1_plus;
      temp->n1_minus = n1_minus;
      temp->n2_plus = n2_plus;
      temp->n2_minus = n2_minus;
    
      data = (char *) temp;  
    }
    return(add_dev(name, type, data));
  }
  else 
  {
    if (type == TRAN_NO_LOSS)
    {
      temp_sub = (sub_tran_noloss *) 
                 mycalloc(1, sizeof(sub_tran_noloss));
 
      temp_sub->zo = *(dataptr);
      temp_sub->td = *(dataptr + 1);
      temp_sub->v1o = *(dataptr + 2);
      temp_sub->i1o = *(dataptr + 3);
      temp_sub->v2o = *(dataptr + 4);
      temp_sub->i2o = *(dataptr + 5);
      temp_sub->n1_plus = n1_plus;
      temp_sub->n1_minus = n1_minus;
      temp_sub->n2_plus = n2_plus;
      temp_sub->n2_minus = n2_minus;
      temp_sub->p1_node = scan_node(n1_plus);
      temp_sub->m1_node = scan_node(n1_minus);
      temp_sub->p2_node = scan_node(n2_plus);
      temp_sub->m2_node = scan_node(n2_minus);
  
      data = (char *) temp_sub;  
    }
    return(add_sub((char *) name, type, data));
  }

}  /* add_transline */


void
trans_matrix()
{
  dev_tran_noloss *temp_noloss;
  m_data *temp_m;
  device *temp_dev;
  int exist;

  temp_dev = xline;

  while (temp_dev != NULL)
  {
    if (temp_dev->type == TRAN_NO_LOSS)
    {
      temp_noloss = (dev_tran_noloss *) temp_dev->data;

      if (temp_noloss->n1_plus != GROUND)
      {
        temp_noloss->mptr_pp1 = 
          add_i_j(A_matrix, temp_noloss->n1_plus, 
                  temp_noloss->n1_plus, &exist);
        temp_noloss->mptr_pp1->type = ORIGINAL;

/*
        if (x_unknown[temp_noloss->n1_plus] == NULL)
        {
          add_unknown(x_unknown, temp_noloss->n1_plus);
          x_unknown[temp_noloss->n1_plus]->type = VOLT;
        }
*/

        add_source(b_source, temp_noloss->n1_plus);
      }

      if ((temp_noloss->n1_plus != GROUND) && 
          (temp_noloss->n1_minus != GROUND))
      {
        temp_noloss->mptr_pm1 = 
          add_i_j(A_matrix, temp_noloss->n1_plus, 
                  temp_noloss->n1_minus, &exist);
        temp_noloss->mptr_pm1->type = ORIGINAL;

        temp_noloss->mptr_mp1 = 
          add_i_j(A_matrix, temp_noloss->n1_minus, 
                  temp_noloss->n1_plus, &exist);
        temp_noloss->mptr_mp1->type = ORIGINAL;
      }

      if (temp_noloss->n1_minus != GROUND)
      {
        temp_noloss->mptr_mm1 = 
          add_i_j(A_matrix, temp_noloss->n1_minus, 
                  temp_noloss->n1_minus, &exist);
        temp_noloss->mptr_mm1->type = ORIGINAL;

/*
        if (x_unknown[temp_noloss->n1_minus] == NULL)
        {
          add_unknown(x_unknown, temp_noloss->n1_minus);
          x_unknown[temp_noloss->n1_minus]->type = VOLT;
        }
*/
        add_source(b_source, temp_noloss->n1_minus);
      }

      if (temp_noloss->n2_plus != GROUND)
      {
        temp_noloss->mptr_pp2 = 
          add_i_j(A_matrix, temp_noloss->n2_plus, 
                  temp_noloss->n2_plus, &exist);
        temp_noloss->mptr_pp2->type = ORIGINAL;

/*
        if (x_unknown[temp_noloss->n2_plus] == NULL)
        {
          add_unknown(x_unknown, temp_noloss->n2_plus);
          x_unknown[temp_noloss->n2_plus]->type = VOLT;
        }
*/

        add_source(b_source, temp_noloss->n2_plus);
      }

      if ((temp_noloss->n2_plus != GROUND) && 
          (temp_noloss->n2_minus != GROUND))
      {
        temp_noloss->mptr_pm2 = 
          add_i_j(A_matrix, temp_noloss->n2_plus, 
                  temp_noloss->n2_minus, &exist);
        temp_noloss->mptr_pm2->type = ORIGINAL;

        temp_noloss->mptr_mp2 = 
          add_i_j(A_matrix, temp_noloss->n2_minus, 
                  temp_noloss->n2_plus, &exist);
        temp_noloss->mptr_mp2->type = ORIGINAL;
      }

      if (temp_noloss->n2_minus != GROUND)
      {
        temp_noloss->mptr_mm2 = 
          add_i_j(A_matrix, temp_noloss->n2_minus, 
                  temp_noloss->n2_minus, &exist);
        temp_noloss->mptr_mm2->type = ORIGINAL;

/*
        if (x_unknown[temp_noloss->n2_minus] == NULL)
        {
          add_unknown(x_unknown, temp_noloss->n2_minus);
          x_unknown[temp_noloss->n2_minus]->type = VOLT;
        }
*/
        add_source(b_source, temp_noloss->n2_minus);
      }
  
      if (temp_noloss->n1_plus == GROUND) 
        temp_noloss->n1_plus = ground_node;
      if (temp_noloss->n1_minus == GROUND) 
        temp_noloss->n1_minus = ground_node;
      if (temp_noloss->n2_plus == GROUND) 
        temp_noloss->n2_plus = ground_node;
      if (temp_noloss->n2_minus == GROUND) 
        temp_noloss->n2_minus = ground_node;

    }  /* if */

    temp_dev = temp_dev->next_dev;

  }  /* while */

}  /* trans_matrix */


void
setup_transline()
{
  device *temp_dev;
  dev_tran_noloss *temp_noloss;

  temp_dev = xline;
 
  while (temp_dev != NULL)
  {
    if (temp_dev->type == TRAN_NO_LOSS)
    {
      temp_noloss = (dev_tran_noloss *) temp_dev->data;

      if (user_step + user_step > temp_noloss->td)
        user_step = temp_noloss->td/2.0;
    }
    
    temp_dev = temp_dev->next_dev;
  
  }

  temp_dev = xline;
 
  while (temp_dev != NULL)
  {
    if (temp_dev->type == TRAN_NO_LOSS)
    {
      temp_noloss = (dev_tran_noloss *) temp_dev->data;

      temp_noloss->iport1.interval = temp_noloss->td; 
      temp_noloss->vport1.interval = temp_noloss->td; 
      temp_noloss->iport2.interval = temp_noloss->td; 
      temp_noloss->vport2.interval = temp_noloss->td; 

      temp_noloss->iport1.val_min = itran_min;
      temp_noloss->iport1.val_rel = itran_rel;
      temp_noloss->vport1.val_min = vtran_min;
      temp_noloss->vport1.val_rel = vtran_rel;
      temp_noloss->iport2.val_min = itran_min;
      temp_noloss->iport2.val_rel = itran_rel;
      temp_noloss->vport2.val_min = vtran_min;
      temp_noloss->vport2.val_rel = vtran_rel;

      temp_noloss->iport1.unit_size = 
                          (temp_noloss->td/user_step + 1.0);
      temp_noloss->iport1.size = temp_noloss->iport1.unit_size;
      temp_noloss->iport1.current_size = 0;
      temp_noloss->iport1.time_data = (double *) 
             mycalloc(temp_noloss->iport1.unit_size, sizeof(double));
      temp_noloss->iport1.value_data = (double *) 
             mycalloc(temp_noloss->iport1.unit_size, sizeof(double));

      temp_noloss->vport1.unit_size = temp_noloss->iport1.unit_size;
      temp_noloss->vport1.size = temp_noloss->vport1.unit_size;
      temp_noloss->vport1.current_size = 0;
      temp_noloss->vport1.time_data = (double *) 
             mycalloc(temp_noloss->vport1.unit_size, sizeof(double));
      temp_noloss->vport1.value_data = (double *) 
             mycalloc(temp_noloss->vport1.unit_size, sizeof(double));

      temp_noloss->iport2.unit_size = temp_noloss->iport1.unit_size;
      temp_noloss->iport2.size = temp_noloss->iport2.unit_size;
      temp_noloss->iport2.current_size = 0;
      temp_noloss->iport2.time_data = (double *) 
             mycalloc(temp_noloss->iport2.unit_size, sizeof(double));
      temp_noloss->iport2.value_data = (double *) 
             mycalloc(temp_noloss->iport2.unit_size, sizeof(double));

      temp_noloss->vport2.unit_size = temp_noloss->iport1.unit_size;
      temp_noloss->vport2.size = temp_noloss->vport2.unit_size;
      temp_noloss->vport2.current_size = 0;
      temp_noloss->vport2.time_data = (double *) 
             mycalloc(temp_noloss->vport2.unit_size, sizeof(double));
      temp_noloss->vport2.value_data = (double *) 
             mycalloc(temp_noloss->vport2.unit_size, sizeof(double));

    }  /* if */

    temp_dev = temp_dev->next_dev;
  
  }  /* while */

}   /* setup_transline */


void
transline_trap(int source_only, double *hptr)
{
  dev_tran_noloss *temp_noloss;
  device *temp_dev;
  int index, exist;
  double g_val;


  temp_dev = xline;
  
  while (temp_dev != NULL)
  {
    if (temp_dev->type == TRAN_NO_LOSS)
    {
      temp_noloss = (dev_tran_noloss *) temp_dev->data;

      g_val = 1.0/temp_noloss->zo;

      temp_noloss->is1 = delay_frame_point(&(temp_noloss->iport2),
                            last_time + *hptr, &index) +
                         delay_frame_point(&(temp_noloss->vport2),
                            last_time + *hptr, &index) * g_val;

      temp_noloss->is2 = delay_frame_point(&(temp_noloss->iport1),
                            last_time + *hptr, &index) +
                         delay_frame_point(&(temp_noloss->vport1),
                            last_time + *hptr, &index) * g_val;

      if (temp_noloss->n1_plus != ground_node)
      {
        if (source_only == FALSE)
          temp_noloss->mptr_pp1->val = 
            temp_noloss->mptr_pp1->val + g_val;
       
        b_src_copy[temp_noloss->n1_plus]->val =
            b_src_copy[temp_noloss->n1_plus]->val + temp_noloss->is1; 
/*
        b_source[temp_noloss->n1_plus]->val =
            b_source[temp_noloss->n1_plus]->val + temp_noloss->is1; 
*/
      }

      if ((temp_noloss->n1_plus != ground_node) && 
          (temp_noloss->n1_minus != ground_node) &&
          (source_only == FALSE))
      {
        temp_noloss->mptr_pm1->val = 
          temp_noloss->mptr_pm1->val - g_val;

        temp_noloss->mptr_mp1->val = 
          temp_noloss->mptr_mp1->val - g_val;
      }

      if (temp_noloss->n1_minus != ground_node)
      {
        if (source_only == FALSE)
          temp_noloss->mptr_mm1->val = 
            temp_noloss->mptr_mm1->val + g_val;
          
        b_src_copy[temp_noloss->n1_minus]->val =
            b_src_copy[temp_noloss->n1_minus]->val - temp_noloss->is1;
/*
        b_source[temp_noloss->n1_minus]->val =
            b_source[temp_noloss->n1_minus]->val - temp_noloss->is1;
*/
      }

      if (temp_noloss->n2_plus != ground_node)
      {
        if (source_only == FALSE)
          temp_noloss->mptr_pp2->val = 
            temp_noloss->mptr_pp2->val + g_val;
       
        b_src_copy[temp_noloss->n2_plus]->val =
          b_src_copy[temp_noloss->n2_plus]->val + temp_noloss->is2;
/*
        b_source[temp_noloss->n2_plus]->val =
          b_source[temp_noloss->n2_plus]->val + temp_noloss->is2;
*/
      }

      if ((temp_noloss->n2_plus != ground_node) && 
          (temp_noloss->n2_minus != ground_node) &&
          (source_only == FALSE))
      {
        temp_noloss->mptr_pm2->val = 
          temp_noloss->mptr_pm2->val - g_val;

        temp_noloss->mptr_mp2->val = 
          temp_noloss->mptr_mp2->val - g_val;
      }

      if (temp_noloss->n2_minus != ground_node)
      {
        if (source_only == FALSE)
          temp_noloss->mptr_mm2->val = 
            temp_noloss->mptr_mm2->val + g_val;
          
        b_src_copy[temp_noloss->n2_minus]->val =
          b_src_copy[temp_noloss->n2_minus]->val - temp_noloss->is2;
/*
        b_source[temp_noloss->n2_minus]->val =
          b_source[temp_noloss->n2_minus]->val - temp_noloss->is2;
*/
      }

    }  /* if */ 

    temp_dev = temp_dev->next_dev;

  }  /* while */

}  /* transline_trap */


void
update_transline(double *hptr)
{
  dev_tran_noloss *temp_noloss;
  device *temp_dev;
  int index, exist;
  double g_val, vval, ival;


  temp_dev = xline;
  
  while (temp_dev != NULL)
  {
    if (temp_dev->type == TRAN_NO_LOSS)
    {
      temp_noloss = (dev_tran_noloss *) temp_dev->data;

      g_val = 1.0 / temp_noloss->zo;

      temp_noloss->is1past = temp_noloss->is1;
      temp_noloss->is2past = temp_noloss->is2;

      vval = x_unk_copy[temp_noloss->n1_plus]->xn;
      vval = vval - x_unk_copy[temp_noloss->n1_minus]->xn;

      ival = vval * g_val - temp_noloss->is1;

      add_frame_point(&(temp_noloss->iport1), last_time + *hptr,
                      ival);
      add_frame_point(&(temp_noloss->vport1), last_time + *hptr,
                      vval);

      vval = x_unk_copy[temp_noloss->n2_plus]->xn;
      vval = vval - x_unk_copy[temp_noloss->n2_minus]->xn;

      ival = vval * g_val - temp_noloss->is2;

      add_frame_point(&(temp_noloss->iport2), last_time + *hptr,
                      ival);
      add_frame_point(&(temp_noloss->vport2), last_time + *hptr,
                      vval);

    }  /* if */ 

    temp_dev = temp_dev->next_dev;

  }  /* while */

}  /* update_transline */


void
xline_tran_print(FILE *fp, double inc_rate, int prtype, int subtype,
    device *temp_dev)
{
  long plus1, minus1, plus2, minus2; 
  double i1, i2;
  double plus1_val, minus1_val, plus2_val, minus2_val;
  double zo, is1past, is2past, is1, is2;
  modeldata *mod;

  switch (temp_dev->type)
  {
    case TRAN_NO_LOSS : 

         plus1 = ((dev_tran_noloss *) temp_dev->data)->n1_plus;
         minus1 = ((dev_tran_noloss *) temp_dev->data)->n1_minus;
         plus2 = ((dev_tran_noloss *) temp_dev->data)->n2_plus;
         minus2 = ((dev_tran_noloss *) temp_dev->data)->n2_minus;
         zo = ((dev_tran_noloss *) temp_dev->data)->zo;
         is1 = ((dev_tran_noloss *) temp_dev->data)->is1;
         is2 = ((dev_tran_noloss *) temp_dev->data)->is2;
         is1past = ((dev_tran_noloss *) temp_dev->data)->is1past;
         is2past = ((dev_tran_noloss *) temp_dev->data)->is2past;
         plus1_val = get_pr_val(inc_rate, plus1, 0);
         minus1_val = get_pr_val(inc_rate, minus1, 0);
         plus2_val = get_pr_val(inc_rate, plus2, 0);
         minus2_val = get_pr_val(inc_rate, minus2, 0);
  
         if (prtype == VOLT)
         switch(subtype)
         {
           case PORT1 :
                fprintf(fp, out_format, plus1_val - minus1_val);
                return;
           
           case PORT2 :
                fprintf(fp, out_format, plus2_val - minus2_val);
                return;
           
           default :
                fprintf(fp, out_format, plus1_val - minus1_val);
                fprintf(fp, out_format, plus2_val - minus2_val);
                return;
         }
         else if (prtype == AMP)
         {
           is1 = is1past + (is1 - is1past) * inc_rate;
           is1 = (plus1_val - minus1_val) / zo - is1;

           is2 = is2past + (is2 - is2past) * inc_rate;
           is2 = (plus2_val - minus2_val) / zo - is2;

           switch(subtype)
           {
             case PORT1 : 

                fprintf(fp, out_format, is1);
                return;

             case PORT2 : 

                fprintf(fp, out_format, is2);
                return;
       
             default : 

                fprintf(fp, out_format, is1);
                fprintf(fp, out_format, is2);
                return;

           }  /* switch */

         }  /* if */

  }     /* switch */

}  /* xline_tran_print */

