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


int
hash_node(int node)
{
  if (node == 0) return(RANGE);
  if (node > 0) return(node % RANGE);
  return((0 - node) % RANGE);

}  /* hash_node */


branch_marker *
add_bmarker()
{
  branch_marker *temp_marker;

  temp_marker = (branch_marker *) mycalloc(1, sizeof(branch_marker));
  temp_marker->marker = 0;

  return(temp_marker);

}  /* add_bmarker */


int
add_node(int node, long name, int devtype, int nodetype,
    branch_marker *bmarker)
{
  int r;
  node_to_eqn *temp;
  branch_list *temp_branch;

  temp_branch = (branch_list *) mycalloc(1, sizeof(branch_list));
  temp_branch->name = name;
  temp_branch->devtype = devtype;
  temp_branch->nodetype = nodetype;
  temp_branch->bmarker = bmarker;
  temp_branch->next_branch = NULL;

  r = hash_node(node);
  
  if (node_map[r] == NULL)
  {
    if (r < RANGE)
    {
      eqn_count++;
      node_count++;
    }

    node_map[r] = (node_to_eqn *) mycalloc(1, sizeof(node_to_eqn));
    if (r < RANGE)
    {
      node_map[r]->n_eq = eqn_count;
      node_map[r]->node = node;
    }
    else 
    {
      node_map[r]->n_eq = GROUND;
      node_map[r]->node = node;
    }
    node_map[r]->marker = 0;
    node_map[r]->branch = node_map[r]->branch_tail = temp_branch;
    node_map[r]->next_node = NULL;

    if ((nodetype == PLUS_NODE) || (nodetype == PLUS_C_NODE))
      bmarker->pnode = (long) node_map[r];
    else bmarker->mnode = (long) node_map[r]; 

    return(node_map[r]->n_eq);
  }

  temp = node_map[r];

  if (temp->node == node) 
  {
    temp->branch_tail->next_branch = temp_branch;
    temp->branch_tail = temp_branch;

    if ((nodetype == PLUS_NODE) || (nodetype == PLUS_C_NODE))
      bmarker->pnode = (long) temp;
    else bmarker->mnode = (long) temp; 

    return(temp->n_eq);
  }

  while (temp->next_node != NULL)
  {
    if (temp->next_node->node == node) 
    {
      temp->next_node->branch_tail->next_branch = temp_branch;
      temp->next_node->branch_tail = temp_branch;

      if ((nodetype == PLUS_NODE) || (nodetype == PLUS_C_NODE))
        bmarker->pnode = (long) temp->next_node;
      else bmarker->mnode = (long) temp->next_node; 

      return(temp->next_node->n_eq);
    }
   
    temp = temp->next_node;
  }

  eqn_count++;
  node_count++;

  temp->next_node = (node_to_eqn *) mycalloc(1, sizeof(node_to_eqn));
  temp = temp->next_node;

  temp->n_eq = eqn_count;
  temp->node = node;
  temp->marker = 0;
  temp->branch = temp->branch_tail = temp_branch;
  temp->next_node = NULL;

  if ((nodetype == PLUS_NODE) || (nodetype == PLUS_C_NODE))
    bmarker->pnode = (long) temp;
  else bmarker->mnode = (long) temp; 

  return(temp->n_eq);

}   /* add_node */


void
clear_marker()
{
  int i;
  node_to_eqn *temp;
  branch_list *temp_branch;

  for (i = 0; i <= RANGE; i++)
  {
    temp = node_map[i];
    
    while (temp != NULL)
    {
      temp->marker = 0;
      temp_branch = temp->branch;
      
      while (temp_branch != NULL)
      {
        temp_branch->bmarker->marker = 0;
        temp_branch = temp_branch->next_branch;
      }
      temp = temp->next_node;
    }   /* while */
  }  /* for */

}   /* clear_marker */


void
cutset_check()
{

  int i;
  int capacitor;
  int others;
  int element_count;
  realname_name *temp_name;
  node_to_eqn *temp;
  branch_list *temp_branch;


  for (i = 0; i <= RANGE; i++)
  {
    temp = node_map[i];
    while (temp != NULL)
    {
      capacitor = FALSE;
      others = FALSE;
      element_count = 0;
      temp_branch = temp->branch;

      while (temp_branch != NULL)
      {
        switch (temp_branch->devtype)
        {
          case I_SOURCE :
          case SIN_I :
          case PULSE_I :
          case PWL_I : break;

          case CAP : capacitor = TRUE;
                     break;
          default : others = TRUE;
                    break;
        } 

        element_count++;
        temp_branch = temp_branch->next_branch;

      }   /* while */
     
      if (element_count == 1)
      {
        temp_name = search_name(temp->branch->name);
        printf("## Error -- node %1d has only one", temp->node);
        printf(" element %s attached\n", temp_name->real_name);
        no_go = TRUE;
      }

      if (others == FALSE)
      {
        if (capacitor == FALSE) 
        {
          printf("## Error -- node %1d has only current", temp->node);
          printf(" source(s) attached\n"); 
          no_go = TRUE;
        }
        else
        {
          printf("## Warning -- node %1d has only cap(s)", temp->node);
          printf(" and/or current source(s) attached\n");             
          warned = TRUE;
        }
      }

      temp = temp->next_node;
    }   /* while */
  }   /* for */

}  /* cutset_check */
    

void
trace_subgraph(node_to_eqn *nodeptr, int source_only)
{
  branch_list *temp_branch;
  node_to_eqn *temp2;
  realname_name *temp_name;


  temp_branch = nodeptr->branch;

  while (temp_branch != NULL)
  {
    if (((source_type(temp_branch->devtype) == V_SOURCE) ||
         ((source_only == FALSE) && 
          ((temp_branch->devtype == INDUCT) ||
           (temp_branch->devtype == LOSSLESS_LINE)))) &&
        (temp_branch->bmarker->marker == 0))
    { 

       temp_branch->bmarker->marker = 1;

       if (node_type(temp_branch->nodetype) == POSITIVE)
         temp2 = (node_to_eqn *) temp_branch->bmarker->mnode;
       else temp2 = (node_to_eqn *) temp_branch->bmarker->pnode;


       if (temp2->marker == 1)
       {
         temp_name = search_name(temp_branch->name);
         if (source_only == TRUE)
         {
           printf("## Error -- voltage source loop found");
           no_go = TRUE;
         }
         else
         {
           printf("## Warning -- voltage source/inductor loop found");
           warned = TRUE;
         }
         printf(" including %s\n", temp_name->real_name); 
       }

       temp2->marker = 1;

       trace_subgraph(temp2, source_only);

    }

    temp_branch = temp_branch->next_branch;

  }   /* while */

} /* trace_subgraph */


void
sourceloop_check(int source_only)
{
  int i;
  node_to_eqn *temp, *temp2;
  branch_list *temp_branch;
  realname_name *temp_name;

  for(i = 0; i <= RANGE; i++)
  {
    temp = node_map[i];
    while (temp != NULL)
    {

      if (temp->marker == 1)
      {
        temp = temp->next_node;
        continue;
      }

      temp_branch = temp->branch;

      while (temp_branch != NULL)
      {

        if (((source_type(temp_branch->devtype) == V_SOURCE) ||
             ((source_only == FALSE) && 
              ((temp_branch->devtype == INDUCT) ||
               (temp_branch->devtype == LOSSLESS_LINE)))) &&
            (temp_branch->bmarker->marker == 0))
        {

          temp_branch->bmarker->marker = 1;

          temp->marker = 1;

          if (node_type(temp_branch->nodetype) == POSITIVE)
            temp2 = (node_to_eqn *) temp_branch->bmarker->mnode;
          else temp2 = (node_to_eqn *) temp_branch->bmarker->pnode;
          temp2->marker = 1;

          trace_subgraph(temp, source_only);
          trace_subgraph(temp2, source_only);

          break;
        }

        temp_branch = temp_branch->next_branch;

      }  /* while */
      
      temp = temp->next_node;

    }   /* while */
  }  /* for */

  clear_marker();

}  /* sourceloop_check */
 

void
trace_connect(node_to_eqn *nodeptr, int dcpath_only)
{
  branch_list *temp_branch;
  node_to_eqn *temp2;

  temp_branch = nodeptr->branch;

  while (temp_branch != NULL)
  {
    if ((dcpath_only == FALSE) ||
         (is_dc_device(temp_branch->devtype) == TRUE))
    { 

       if (node_type(temp_branch->nodetype) == POSITIVE)
         temp2 = (node_to_eqn *) temp_branch->bmarker->mnode;
       else temp2 = (node_to_eqn *) temp_branch->bmarker->pnode;

       if (temp2->marker == 0)
       {
         temp2->marker = 1;
         trace_connect(temp2, dcpath_only);
       }

    }

    temp_branch = temp_branch->next_branch;

  }   /* while */

} /* trace_connect */


void
ground_check(int dcpath_only)
{
  int i;
  node_to_eqn *temp, *temp2;
  branch_list *temp_branch;

  temp = node_map[RANGE];
  if (temp != NULL)
  {
    temp->marker = 1;
    temp_branch = temp->branch;
    while (temp_branch != NULL)
    {
      if ((dcpath_only == FALSE) ||
          (is_dc_device(temp_branch->devtype) == TRUE))
      {
        if (node_type(temp_branch->nodetype) == POSITIVE)
          temp2 = (node_to_eqn *) temp_branch->bmarker->mnode;
        else 
          temp2 = (node_to_eqn *) temp_branch->bmarker->pnode;

        if (temp2->marker == 0)
        { 
          temp2->marker = 1;
          trace_connect(temp2, dcpath_only);
        }
      }
       
      temp_branch = temp_branch->next_branch;

    }  /* while */

  }  /* if */


  for (i = 0; i < RANGE; i++)
  {
    temp = node_map[i];
    while (temp != NULL)
    {
      if (temp->marker == 0) 
      {
        if (dcpath_only == FALSE)
        {
          printf("## Error -- node %1d has no path to ground\n",
                  temp->node);
          no_go = TRUE;
        }
        else
        {
          printf("## Warning -- node %1d has no dc path to ground\n",
                  temp->node);
          warned = TRUE;
        }
      }

      temp = temp->next_node;
    }   /* while */     
  }  /* for */

  clear_marker();

}  /* ground_check */

