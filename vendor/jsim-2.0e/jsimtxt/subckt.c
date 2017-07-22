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

long name;
modeldata *modptr;


void
read_sub_def()
{
  read_error = read_string("subckt definition name");
  if (read_error != OK) 
  {
    printf("## Error -- bad subcircuit definition\n");
    no_go = TRUE;
    return;
  }

  dev_name = new_string(tempstring);
  intptr[0] = 0;
  while ((read_error = read_long(nodeptr+intptr[0], "node", TRUE)) 
          == OK) 
     intptr[0]++;
  if (intptr[0] < 1) 
  {
    printf("## Error -- bad subcircuit definition,");
    printf(" too few node\n");
    no_go = TRUE;
    free(dev_name);
    return;
  }

  new_sub_def(dev_name, intptr, nodeptr); 

}  /* read_sub_def */


void
new_sub_def(char *name, long *intptr, long *nodeptr)
{
  int i;
  sub_def *temp_sub_def;

  temp_sub_def = (sub_def *) mycalloc(1, sizeof(sub_def));
  temp_sub_def->name = name;
  temp_sub_def->no_node = *intptr;
  temp_sub_def->node_list = (int *) mycalloc(*intptr, sizeof(int));
  for (i = 0; i < *intptr; i++)
    *(temp_sub_def->node_list + i) = *(nodeptr + i);
  temp_sub_def->the_sub_ckt = temp_sub_def->the_sub_tail = NULL;
  temp_sub_def->sub_dev_count = 0;
  temp_sub_def->sub_dev_array = NULL;
  temp_sub_def->parent = NULL;
  temp_sub_def->next_sub = NULL;
  temp_sub_def->sub_sub = temp_sub_def->sub_sub_tail = NULL;
  temp_sub_def->sub_sub_count = 0;
  temp_sub_def->sub_sub_array = NULL;

  if (current_sub_def == NULL)
  {
    if (sub_def_tree == NULL)
    {
      sub_def_tree = temp_sub_def; 
      sub_def_tail = temp_sub_def; 
    }
    else
    {
      sub_def_tail->next_sub = temp_sub_def;
      sub_def_tail = temp_sub_def;
    }
    sub_def_count++;
  }
  else
  {
    temp_sub_def->parent = current_sub_def;
    if (current_sub_def->sub_sub == NULL)
    {
      current_sub_def->sub_sub = temp_sub_def; 
      current_sub_def->sub_sub_tail = temp_sub_def; 
    }
    else
    {
      current_sub_def->sub_sub_tail->next_sub = temp_sub_def;
      current_sub_def->sub_sub_tail = temp_sub_def;
    }
    current_sub_def->sub_sub_count++;
  }
  current_sub_def = temp_sub_def; 
        
}   /* new_sub_def */
  

void
read_sub_ckt()
{
  int bad_dev = FALSE;

  read_error = read_string("subckt definition name");
  if (read_error != OK)
  {
    printf("## Error -- bad subckt call\n");
    no_go = TRUE;
    bad_dev = TRUE;
  }
  else modptr = (modeldata *) new_string(tempstring);

  intptr[0] = 0;
  while ((read_error = read_long(nodeptr+intptr[0], "node", TRUE))
          == OK) 
     intptr[0]++;

  if (bad_dev == FALSE)
  {
    if (current_sub_def == NULL)
      subckt_to_devlist((long) dev_name, SUB_C, nodeptr, intptr, modptr);
    else
      subckt_to_deftree(dev_name, SUB_C, nodeptr, intptr, modptr);
  }

}  /* read_sub_ckt */


device *
subckt_to_devlist(long name, int type, long *nodeptr, long *intptr,
    modeldata *modptr)
{
  switch (type)
  {
    case SUB_C : 

         return(add_sub_ckt(name, *intptr, nodeptr, (char *) modptr));
              
  }

  return(NULL);

}   /* subckt_to_devlist */   


device *
subckt_to_deftree(char *dev_name, int type, long *nodeptr, long *intptr,
    modeldata *modptr)
{
  long name;

  name = (long) dev_name;

  switch (type)
  {
    case SUB_C : 

         return(add_sub_ckt(name, *intptr, nodeptr, (char *) modptr));
              
  }

  return(NULL);

}   /* subckt_to_deftree */   


device *subckt_call[100];
sub_def *subckt_def_call[100];
int current_call;
subnode_to_node *subnode_buffer;
int sub_buffer_index;


int
scan_node(int node)
{
  int i;

  i = 0;
  while (i < current_sub_def->no_node) 
  {
    if (*(current_sub_def->node_list + i) == node) return(i+1);
    i++;
  }

  return(0);

}  /* scan_node */
  

device *
add_sub(char *name, int type, char *data)
{
  device *temp;

  temp = (device *) mycalloc(1, sizeof(device));
  temp->name = (long) name;
  temp->type = type;
  temp->data = data;
  temp->next_dev = NULL;

  if (current_sub_def->the_sub_ckt == NULL)
  {
    current_sub_def->the_sub_ckt = temp;
    current_sub_def->the_sub_tail = temp;
  }
  else 
  {
    current_sub_def->the_sub_tail->next_dev = temp;
    current_sub_def->the_sub_tail = temp;
  }
  current_sub_def->sub_dev_count++;
  
  return(temp);

}  /* add_sub */


device *
add_sub_ckt(long name, long no_node, long *node_list, char *sub_def_name)
{
  int i;
  char *data;
  dev_sub_ckt *temp;
  sub_sub_ckt *temp_sub;
   
  if (current_sub_def == NULL)
  {
    temp = (dev_sub_ckt *) mycalloc(1, sizeof(dev_sub_ckt));
  
    temp->sub_def_name = sub_def_name;
    temp->no_node = no_node;
    temp->node_list = (long *) mycalloc(no_node, sizeof(long));
    for (i = 0; i < no_node; i++)
      *(temp->node_list + i) = *(node_list + i); 
  
    data = (char *) temp;  
    return(add_dev(name, SUB_C, data));
  }
  else
  {
    temp_sub = (sub_sub_ckt *) mycalloc(1, sizeof(sub_sub_ckt));

    temp_sub->sub_def_name = sub_def_name;
    temp_sub->no_node = no_node;
    temp_sub->node_list = (long *) mycalloc(no_node, sizeof(long));
    temp_sub->temp_node_list = (long *) mycalloc(no_node, sizeof(long));
    temp_sub->node_node = (long *) mycalloc(no_node, sizeof(long));
    for (i = 0; i < no_node; i++)
    {
      *(temp_sub->node_list + i) = *(node_list + i); 
      *(temp_sub->node_node + i) = scan_node(*(node_list + i));
    }
  
    data = (char *) temp_sub;  
    return(add_sub((char *) name, SUB_C, data));
  }

}  /* add_sub_ckt */


long
getnode_subnode(int subnode, int subnode_index)
{
  int i;

  if (subnode == 0) return(0);

  if (subnode_index <= 0)
  {
    i = 0;
    while (i <= sub_buffer_index)
    {
      if ((subnode_buffer + i)->subnode == subnode)  
        return((subnode_buffer + i)->node);
      i++;
    }
    sub_buffer_index++;
    subnode_count--;
    (subnode_buffer + sub_buffer_index)->subnode = subnode;
    (subnode_buffer + sub_buffer_index)->node = subnode_count;
    return(subnode_count);
  }
  else
  {
    subnode_index--;
    if (current_call > 0) 
      return(*(((sub_sub_ckt *) 
               subckt_call[current_call]->data)->temp_node_list 
               + subnode_index));
    else 
      return(*(((dev_sub_ckt *)
               subckt_call[current_call]->data)->node_list 
               + subnode_index));
  }

}    /* getnode_subnode */


char *
get_subdev_name(char *name)
{
  int temp_call;
  int temp_count;
  int found;
  device *temp_dev;

  temp_call = current_call;
  temp_count = strlen(namestring) - 1;

  found = FALSE;

  while ((found == FALSE) && (temp_call >= 0))
  {
    temp_dev = search_dev(subckt_def_call[temp_call]->sub_dev_array,
                          subckt_def_call[temp_call]->sub_dev_count, 
                          name);
    if (temp_dev != NULL) found = TRUE;
    else 
    {
      if (temp_call > 0)
      {
        temp_count--; 
        while (namestring[temp_count] != '_') temp_count--;
      }
      temp_call--;
    }
  }  /* while */

  if (temp_call < 0) return(new_string(name));
  return(comb_str_seg(namestring, temp_count, name));

}  /* get_subdev_name */
    
   
void
do_sub_ckt()
{
  int tempint;
  char *sub_def_name;
  char *sub_ckt_name;
  device *temp_dev;
  
  current_sub_def = NULL;
  subnode_buffer = (subnode_to_node *) 
                    mycalloc(SUB_BUFFER_SIZE, sizeof(subnode_to_node));
  sub_buffer_index = -1;

  temp_dev = sub_ckt;
  while (temp_dev != NULL)
  {
    current_call = 0;
    sub_ckt_name = (char *) temp_dev->name;
    sub_def_name = ((dev_sub_ckt *) temp_dev->data)->sub_def_name;

    subckt_def_call[0] = find_sub_def(NULL, sub_def_name); 

    subckt_call[0] = temp_dev; 
    temp_dev = temp_dev->next_dev;
    if (subckt_def_call[0] == NULL)
    {
      no_go = TRUE;
      printf("subckt call %s subdef %s not defined\n", 
              sub_ckt_name, sub_def_name);
    }
    else
    {
      strcpy(namestring, sub_ckt_name);
      strcat(namestring, "_");
      expand_sub_ckt();
    }
  }

}  /* do_sub_ckt */
     

void
expand_sub_ckt()
{
  int tempint;
  device *temp_dev;
  device *sub_to_devlist();

  sub_buffer_index = -1;
  temp_dev = subckt_def_call[current_call]->the_sub_ckt;
  while (temp_dev != NULL)
  {
    sub_to_devlist((char *) temp_dev->name, temp_dev->type,
                   temp_dev->data);
    temp_dev = temp_dev->next_dev;

  } /* while */

  temp_dev = subckt_def_call[current_call]->the_sub_ckt;
  while (temp_dev != NULL)
  {
    if (temp_dev->type == SUB_C)
    {
      current_call++;
      subckt_def_call[current_call] = 
        find_sub_def(subckt_def_call[current_call - 1], 
                     ((sub_sub_ckt *) temp_dev->data)->sub_def_name); 

      subckt_call[current_call] = temp_dev; 
      if (subckt_def_call[current_call] == NULL)
      {
        no_go = TRUE;
        printf("subckt call %s subdef %s not defined\n", 
               (char *) temp_dev->name, 
               ((sub_sub_ckt *) temp_dev->data)->sub_def_name);
      }
      else
      {
        strcat(namestring, (char *) temp_dev->name);
        strcat(namestring, "_");
        expand_sub_ckt();
        current_call--; 
        namestring[strlen(namestring) - 
                   strlen((char *) temp_dev->name) - 1] = '\0';
      }
    }  /* else */  
    temp_dev = temp_dev->next_dev;

  }  /* while */

}   /* expand_sub_ckt */
      

sub_def *
find_sub_def(sub_def *the_sub_def, char *name)
{
  sub_def *temp_sub;

  if (the_sub_def == NULL)
  {
    temp_sub = search_sub_def(sub_def_array, sub_def_count, name);
    return(temp_sub);
  }
  else 
  {
    temp_sub = search_sub_def(the_sub_def->sub_sub_array,
                              the_sub_def->sub_sub_count, name);  
    if (temp_sub != NULL) return(temp_sub);
    else return(find_sub_def(the_sub_def->parent, name));  
  }

}  /* find_sub_def */


device *
sub_to_devlist(char *dev_name, int type, char *data)
{
  int i;

  switch (type)
  {
    case RESIS : 

         return(resissub_to_devlist(dev_name, type, data));

    case CAP : 
   
         return(capsub_to_devlist(dev_name, type, data));

    case INDUCT : 

         return(indsub_to_devlist(dev_name, type, data));

    case DC_V  :
    case SIN_V  :
    case PULSE_V  :
    case PWL_V :
    case V_SOURCE :
             
         return(vssub_to_devlist(dev_name, data));

    case SIN_I :
    case PULSE_I  :
    case PWL_I :
    case I_SOURCE :
    
         return(issub_to_devlist(dev_name, data));

    case JJ : 
    
         return(jjsub_to_devlist(dev_name, type, data));

    case MUTUAL_L : 
    
         return(mutsub_to_devlist(dev_name, type, data));

    case TRAN_NO_LOSS  :

         return(xlinesub_to_devlist(dev_name, type, data));

    case SUB_C : 

         for (i = 0; i < ((sub_sub_ckt *) data)->no_node; i++)
         {
           *(((sub_sub_ckt *) data)->temp_node_list + i) =
             getnode_subnode(*(((sub_sub_ckt *) data)->node_list + i), 
                             *(((sub_sub_ckt *) data)->node_node + i)); 
         }
         return(NULL);

  }  /* switch */

  return(NULL);

}   /* sub_to_devlist */   
