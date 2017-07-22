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
read_model()
{
  int type;
  int bad_mod = FALSE;

  read_error = read_string("model name");
  if (read_error == OK) dev_name = new_string(tempstring);

  read_error = read_string("model type");
  if (read_error == OK) 
  switch(type = get_string_keyword(tempstring))
  {
    case JJMODEL : read_jjmodel();
                   break;

    default : printf("## Error -- illegal model type %s\n", tempstring);
              no_go = TRUE;
              bad_mod = TRUE;
              break;

  }  /* switch */
  else
  {
    printf("## Error -- bad model definition\n");
    no_go = TRUE;
    bad_mod = TRUE;
  }

  if (bad_mod == FALSE)
  {
    add_to_modlist(dev_name, type, intptr, dataptr);
  }
  else free(dev_name);

}  /* read_model */


modeldata *
add_mod(char *name, int type)
{
  modeldata *temp_model;

  temp_model = (modeldata *) calloc(1, sizeof(modeldata));
  mod_count++;
  temp_model->name = name;
  temp_model->type = type;
  temp_model->data = NULL;
  temp_model->next_model = NULL;

  if (jsim_model == NULL)
  {
    jsim_model = jsim_model_tail = temp_model;
  }
  else
  {
    jsim_model_tail->next_model = temp_model;
    jsim_model_tail = temp_model;
  }

  return(temp_model);

}   /* add_mod */


modeldata *
add_to_modlist(char *name, int type, long *intptr, double *dataptr)
{
  modeldata *add_mod();
  modeldata *modptr;

  modptr = add_mod(name, type);

  if (type == JJMODEL)
  {
    assign_jj_mod(modptr, *intptr, *dataptr, *(dataptr+1),
                  *(dataptr+2), *(dataptr+3), *(dataptr+4),
                  *(dataptr+5), *(intptr+1), *(dataptr+6));
  }
  return(modptr);

}  /* add_to_modlist */


void
read_jjmodel()
{

  *intptr = DEF_RTYPE;
  *(intptr + 1) = DEF_CCT;
  *dataptr = DEF_ICRIT;
  *(dataptr + 1) = DEF_JJCAP;
  *(dataptr + 2) = DEF_VG;
  *(dataptr + 3) = DEF_R0;
  *(dataptr + 4) = DEF_DELV;
  *(dataptr + 5) = DEF_RN;
  *(dataptr + 6) = DEF_ICON;

  while ((read_error = read_string("")) == OK)
  {
    switch(get_string_keyword(tempstring))
    {
      case RTYPE : 
           read_error = read_long(intptr, "IV type", FALSE);
           break;
      case CCT : 
           read_error = read_long(intptr+1, "CCT type", FALSE);
           break;
      case ICRIT : 
           read_error = read_double(dataptr, "critical current", FALSE);
           break;
      case CAPMOD : 
           read_error = read_double(dataptr+1, "junction cap", FALSE);
           break;
      case VG : 
           read_error = read_double(dataptr+2, "gap voltage", FALSE);
           break;
      case R0 :  
           read_error = read_double(dataptr+3, "subgap resistance", 
                                    FALSE);
           break;
      case DELV : 
           read_error = read_double(dataptr+4, "delta voltage", FALSE);
           break;
      case RN : 
           read_error = read_double(dataptr+5, "normal resistance", 
                                    FALSE);
           break;
      case ICON : 
           read_error = read_double(dataptr+6, "ICON val", FALSE);
           break;

      default : 
           printf("## Warning -- illegal keyword %s\n", tempstring);
           warned = TRUE;
           break;

    }  /* switch */  

  }    /* while */

}  /* read_jjmodel */


void
assign_jj_mod(modeldata *modptr, int rtype, double ic, double cap,
    double vgap, double rg, double del_v, double rn, int cct, double icon)
{
   mod_jj *temp_mod;

   modptr->data = mycalloc(1, sizeof(mod_jj));
   temp_mod = (mod_jj *) modptr->data;
   temp_mod->rtype = rtype;
   temp_mod->ic = ic;
   temp_mod->cap = cap;
   temp_mod->vgap = vgap;
   temp_mod->rg = rg;
   temp_mod->del_v = del_v;
   temp_mod->rn = rn;
   temp_mod->glarge = ((1.0/rn)*(vgap + del_v) - (1.0/rg)*vgap)
                      /del_v;
   temp_mod->cct = cct;
   temp_mod->icon = icon;
   temp_mod->quasi_bkpoint[0] = vgap - mymin(0.1*vgap, del_v);
   temp_mod->quasi_bkpoint[1] = vgap;
   temp_mod->quasi_bkpoint[2] = vgap + del_v;
   temp_mod->quasi_bkpoint[3] = vgap + del_v + mymin(0.1*vgap, del_v);

}    /* assign_jj_mod */


double
jjiv(double val, modeldata *mod)
{
  mod_jj *temp_jj;
  double temp_val, abs_val;

  abs_val = myabs(val);

  temp_jj = (mod_jj *) mod->data;

  if (temp_jj->rtype == JJ_PWL)
  {
    temp_jj = (mod_jj *) mod->data;
    if (abs_val <= temp_jj->vgap) return(val/temp_jj->rg);
    else if (abs_val <= temp_jj->vgap + temp_jj->del_v)
    {
      temp_val = temp_jj->vgap/temp_jj->rg;
      temp_val = temp_val + 
                 ((temp_jj->vgap + temp_jj->del_v)/temp_jj->rn
                  - temp_jj->vgap/temp_jj->rg)*
                 ((abs_val - temp_jj->vgap)/temp_jj->del_v);
      if (val >= 0.0) return(temp_val);
      else return(0.0 - temp_val);
    }
    else return(val/temp_jj->rn);
  }
  
  return(0.0);

}  /* jjiv */
