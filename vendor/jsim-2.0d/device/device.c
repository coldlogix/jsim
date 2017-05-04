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


device *
find_dev(char *real_name, int type)
{
  realname_name *temp_name;
  
  temp_name = search_realname(real_name); 

  if (temp_name != NULL)
  switch (type)
  {
    case RESIS : 
         return(search_name_dev(resis_array, resis_count, 
                                temp_name->name)); 

    case CAP   : 
         return(search_name_dev(cap_array, cap_count, temp_name->name));

    case INDUCT : 
         return(search_name_dev(ind_array, ind_count, temp_name->name));

    case V_SOURCE:
    case DC_V:
    case SIN_V:
    case PWL_V : 
         return(search_name_dev(vsource_array, vsource_count, 
                                         temp_name->name));

    case I_SOURCE:
    case SIN_I:
    case PWL_I : 
         return(search_name_dev(isource_array, isource_count, 
                                         temp_name->name)); 
             
    case JJ    : 
         return(search_name_dev(jj_array, jj_count, temp_name->name));

    case MUTUAL_L : 
         return(search_name_dev(mut_array, mut_count, 
                                temp_name->name)); 

    case TRANSLINE :
    case TRAN_NO_LOSS :
         return(search_name_dev(xline_array, xline_count, 
                                temp_name->name));

  }   /* switch */

  return(NULL);

}    /* find_dev */


device *
add_dev(long name, int type, char *data)
{
  device *temp;
  
  switch (type)
  {
    case RESIS : 
         if (resis == NULL) 
         { 
           resis = (device *) mycalloc(1, sizeof(device));
           resis_tail = resis;
         } 
         else
         {
           resis_tail->next_dev = (device *) 
                                     mycalloc(1, sizeof(device));
           resis_tail = resis_tail->next_dev;
         }  
         temp = resis_tail;
         resis_count++;
         break;

    case CAP : 

         if (cap == NULL) 
         { 
           cap = (device *) mycalloc(1, sizeof(device));
           cap_tail = cap;
         } 
         else
         {
           cap_tail->next_dev = (device *) 
                                   mycalloc(1, sizeof(device));
           cap_tail = cap_tail->next_dev;
         }  
         temp = cap_tail;
         cap_count++;
         break;

    case INDUCT : 
         if (ind == NULL) 
         { 
           ind = (device *) mycalloc(1, sizeof(device));
           ind_tail = ind;
         } 
         else
         {
           ind_tail->next_dev = (device *) 
                                  mycalloc(1, sizeof(device));
           ind_tail = ind_tail->next_dev;
         }
         temp = ind_tail;
         ind_count++;
         break;

    case DC_V :
    case SIN_V :
    case PULSE_V :
    case PWL_V : 
    case V_SOURCE :
         if (vsource == NULL) 
         { 
           vsource = (device *) mycalloc(1, sizeof(device));
           vsource_tail = vsource;
         } 
         else
         {
           vsource_tail->next_dev = (device *) 
                                     mycalloc(1, sizeof(device));
           vsource_tail = vsource_tail->next_dev;
         } 
         temp = vsource_tail;
         vsource_count++;
         break;

    case SIN_I :
    case PULSE_I :
    case PWL_I : 
    case I_SOURCE :

         if (isource == NULL) 
         { 
           isource = (device *) mycalloc(1, sizeof(device));
           isource_tail = isource;
         }
         else
         {
           isource_tail->next_dev = (device *) 
                                     mycalloc(1, sizeof(device));
           isource_tail = isource_tail->next_dev;
         }
         temp = isource_tail;
         isource_count++;
         break;
             
    case JJ    : 

         if (jj == NULL) 
         { 
           jj = (device *) mycalloc(1, sizeof(device));
           jj_tail = jj;
         } 
         else
         {
           jj_tail->next_dev = (device *) 
                                  mycalloc(1, sizeof(device));
           jj_tail = jj_tail->next_dev;
         }
         temp = jj_tail;
         jj_count++;
         break;

    case MUTUAL_L : 

         if (mut == NULL) 
         { 
           mut = (device *) mycalloc(1, sizeof(device));
           mut_tail = mut;
         } 
         else
         {
           mut_tail->next_dev = (device *) 
                                  mycalloc(1, sizeof(device));
           mut_tail = mut_tail->next_dev;
         }
         temp = mut_tail;
         mut_count++;
         break;

    case TRAN_NO_LOSS :

         if (xline == NULL)
         {
           xline = (device *) mycalloc(1, sizeof(device));
           xline_tail = xline;
         }
         else
         {
           xline_tail->next_dev = (device *)
                                    mycalloc(1, sizeof(device));
           xline_tail = xline_tail->next_dev;
         }
         temp = xline_tail;
         xline_count++;
         break;

    case SUB_C : 

         if (sub_ckt == NULL) 
         { 
           sub_ckt = (device *) mycalloc(1, sizeof(device));
           sub_ckt_tail = sub_ckt;
         } 
         else
         {
           sub_ckt_tail->next_dev = (device *) 
                                     mycalloc(1, sizeof(device));
           sub_ckt_tail = sub_ckt_tail->next_dev;
         }
         temp = sub_ckt_tail;
         sub_ckt_count++;
         break;

  }   /* switch */

  temp->name = name;
  temp->type = type;
  temp->data = data;
  temp->next_dev = NULL;

  return(temp);

}    /* add_dev */


long
add_realname(char *dev_name)
{
  realname_name *temp_name;

  temp_name = (realname_name *) mycalloc(1, sizeof(realname_name));
  temp_name->real_name = dev_name;
  dev_count++;
  temp_name->name = dev_count;
  temp_name->next_name = NULL;
  if (namelist == NULL)
  {
    namelist = temp_name;
    namelist_tail = temp_name;
  }
  else
  {
    namelist_tail->next_name = temp_name;
    namelist_tail = temp_name;
  }
  return(dev_count);

}  /* add_realname */
  

