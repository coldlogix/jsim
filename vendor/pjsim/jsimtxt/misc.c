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

extern FILE *fp;


char *
mycalloc(int num, int size)
{
  char *temp;

  temp = calloc(num, size);
  if (temp == NULL) sperror(NO_MEM); 
  return(temp);

}  /* mycalloc */


int
readline(char *line)
{
  char c;
  int i;
  int line_end;

  i = 0;
  line_end = FALSE;

  read_error = OK;

  do
  {
    c = getc(fp);
    if ((c != '\n') && (c != EOF))
    {
      if ((c >= 'a') && (c <= 'z')) c = 'A' + c - 'a';
      *(line + i) = c;
      printf("%c", c);
      i++;
    }
    else if (c == '\n')
    {
      printf("%c", c);
      c = getc(fp);
      if (c != '+') 
      {
        ungetc(c, fp);
        c = '\n';
        line_end = TRUE;
      }
      else 
      {
        printf("%c", c);
        *(line + i) = ' ';
        i++;
      }
    }
    else line_end = TRUE;
  }
  while (line_end == FALSE);

  
  if (c == '\n')
  {
    *(line + i) = '\0';
    if (!strcmp(line,".END"))
      return (EOF);
    return(EOF + 1);
  }
  else return(EOF);

}  /* readline */
   

void
ignore_separator(char *line)
{
  while(*line != '\0')
  {
    if ((*line == ',') || (*line == '=') || (*line == '(')
        || (*line == ')')) *line = ' ';
    line++; 
  }

}  /* ignore_separator */


int
read_int(int *val, char *error_message, int no_mess_if_blank)
{
  int i;

  while (*line == ' ') line++;
   
  tempstring[0] = '\0';
  sscanf(line, "%s", tempstring);

  if (sscanf(tempstring, "%d", val) != 1) 
  {
    if ((strlen(tempstring) > 0) || (no_mess_if_blank == FALSE))
    {
      for (i = 0; i < line - linesave; i++) printf("#");
      if (strlen(tempstring) == 0)
        printf("%s expected, none found\n", error_message);
      else printf("illegal %s value %s\n",  error_message, tempstring);
    }

    line = line + strlen(tempstring);
    return(OK - 1);
  }
  line = line + strlen(tempstring);

  return(read_error);

}   /* read_int */


int
read_long(long *val, char *error_message, int no_mess_if_blank)
{
  int i;

  while (*line == ' ') line++;
   
  tempstring[0] = '\0';
  sscanf(line, "%s", tempstring);

  if (sscanf(tempstring, "%ld", val) != 1) 
  {
    if ((strlen(tempstring) > 0) || (no_mess_if_blank == FALSE))
    {
      for (i = 0; i < line - linesave; i++) printf("#");
      if (strlen(tempstring) == 0)
        printf("%s expected, none found\n", error_message);
      else printf("illegal %s value %s\n",  error_message, tempstring);
    }

    line = line + strlen(tempstring);
    return(OK - 1);
  }
  line = line + strlen(tempstring);

  return(read_error);

}   /* read_long */


int
read_string(char *error_message)
{
  int i;

  tempstring[0] = '\0';

  while (*line == ' ') line++;
  if (*line == '\0')
  {
    if (strlen(error_message) > 0)
    {
      for (i = 0; i < linesave - line; i++) printf("#");
      printf("%s expected, none found\n", error_message);
    }
         
    return(OK - 1);
  }
   
  sscanf(line, "%s", tempstring);
  line = line + strlen(tempstring);

  return(read_error);

}   /* read_string */


char *
new_string(char *tempstring)
{
  char *str;

  str = mycalloc(strlen(tempstring)+1, sizeof(char));
  strcpy(str, tempstring);
  return(str);

}   /* new_string */


char *
combine_string(char *str1, char *str2)
{
  int length;
  char *str;

  length = strlen(str1) + strlen(str2) + 1 ;
  str = mycalloc(length, sizeof(char));
  strcpy(str, str1);
  strcat(str, str2);
  return(str);

}   /* combine_string */ 
        

char *
comb_str_seg(char *str1, int count1, char *str2)
{
  int i, length;
  char *str;

  length = count1 + 1 + strlen(str2) + 1;
  str = mycalloc(length, sizeof(char));
  for (i = 0; i <= count1; i++) str[i] = str1[i];
  str[count1+1] = '\0';
  strcat(str, str2);
  return(str);

}  /* comb_str_seg */


int
get_multiplier(char *str, double *val)
{
  *val = 1.0;

  switch(*str)
  {
    case 'T' : *val = 1.0e12; return(OK);

    case 'G' : *val = 1.0e9; return(OK);
  
    case 'K' : *val = 1.0e3; return(OK);

    case 'U' : *val = 1.0e-6; return(OK);
 
    case 'N' : *val = 1.0e-9; return(OK);
  
    case 'P' : *val = 1.0e-12; return(OK);
   
    case 'F' : *val = 1.0e-15; return(OK);
  
    case 'M' : if (*(str + 1) == 'E')
               if (*(str + 2) == 'G')
               {
                 *val = 1.0e6; return(OK);
               };

               if (*(str + 1) == 'I')
               if (*(str + 2) == 'L')
               {
                 *val = 25.4e-6; return(OK);
               };

               *val = 1.0e-3; return(OK);
 
    default : return(OK - 1);
    
                 
  }  /* switch */
  
}  /* get_multiplier */


int
num_multiplier(char *str, double *val)
{
  while (*str != '\0')
    if ((*str >= '0') && (*str <= '9')) str++;
    else if ((*str == '+') || (*str == '-') || (*str == '.')
             || (*str == 'E')) str++;
    else break;

  if (*str == '\0') return(OK - 2);
  return(get_multiplier(str, val));

}  /* num_multiplier */


int
read_double(double *val, char *error_message, int no_mess_if_blank)
{
  int i;
  double multiplier;

  while (*line == ' ') line++;
   
  tempstring[0] = '\0';
  sscanf(line, "%s", tempstring);

  if (sscanf(tempstring, "%lf", val) != 1) 
  {
    if ((strlen(tempstring) > 0) || (no_mess_if_blank == FALSE))
    {    
      for (i = 0; i < line - linesave; i++) printf("#");
      if (strlen(tempstring) < 1) 
        printf("%s expected, none found\n", error_message);
      else printf("illegal %s value %s\n", error_message, tempstring);
    }

    line = line + strlen(tempstring);
    return(OK - 1);
  } 
  line = line + strlen(tempstring);
  if (num_multiplier(tempstring, &multiplier) == OK)
    *val = *val * multiplier;

  return(read_error);

}   /* read_double */


double
myabs(double x)
{
  if (x < 0.0) return(0.0 - x);
  else return(x);
}  /* myabs */


double
mymax(double x, double y)
{

  if (x >= y) return (x);
  else return(y);

}  /* mymax */


double
mymin(double x, double y)
{
  if (x <= y) return(x);
  else return(y);

}   /* mymin */


int
get_string_type(char *str)
{
  int length;
  int type;
  
  length = strlen(str);

  switch (*str)
  {
    case 'R' : type = RESIS; break;
    case 'C' : type = CAP; break;
    case 'L' : type = INDUCT; break;
    case 'K' : type = MUTUAL_L; break;
    case 'B' : type = JJ; break;
    case 'X' : type = SUB_C; break;
    case 'V' : type = V_SOURCE; break;
    case 'I' : type = I_SOURCE; break;
    case 'T' : type = TRANSLINE; break;
    case '.' : type = CONTROL; break;
    case '*' : type = COMMENT; break;
    default : type = ILLEGAL; break;
  }

  if ((length < 2) && (type != COMMENT)) type = ILLEGAL;
  
  return(type);

}   /* get_string_type */
    

int
get_devname_type(char *str)
{
  char c;
  int i, length;
  
  length = strlen(str);
 
  if (*str == 'X') 
  {
    i = length - 1; 
    while (i >= 0)
    {
      if (*(str + i) == '_') break;
      i--;
    }
    if (i < 0) return(ILLEGAL);
    if (i >= length - 2) return(ILLEGAL);
    c = *(str + i + 1);
  }
  else c = *str;
    
  switch (c)
  {
    case 'R' : return(RESIS);
    case 'C' : return(CAP);
    case 'L' : return(INDUCT);
    case 'K' : return(MUTUAL_L);
    case 'B' : return(JJ);
    case 'V' : return(V_SOURCE);
    case 'I' : return(I_SOURCE);
    case 'T' : return(TRANSLINE);
    default : return(ILLEGAL);
  }

}   /* get_devname_type */
    

int
get_string_keyword(char *keyword)
{
  
  if (strcmp(keyword, ".PRINT") == 0) return(PRINT);
  if (strcmp(keyword, ".MODEL") == 0) return(MODEL);
  if (strcmp(keyword, "AREA") == 0) return(AREA);
  if (strcmp(keyword, "IC") == 0) return(IC);
  if (strcmp(keyword, "FCHECK") == 0) return(FCHECK);
  if (strcmp(keyword, "FVALUE") == 0) return(FVALUE);
  if (strcmp(keyword, ".OPTIONS") == 0) return(OPTIONS);
  if (strcmp(keyword, ".TRAN") == 0) return(TRAN);
  if (strcmp(keyword, ".FILE") == 0) return(NEWFILE);
  if (strcmp(keyword, ".SUBCKT") == 0) return(SUBCKT);
  if (strcmp(keyword, ".ENDS") == 0) return(ENDS);
  if (strcmp(keyword, "PWL") == 0) return(PWL);
  if (strcmp(keyword, "PULSE") == 0) return(PULSE);
  if (strcmp(keyword, "SIN") == 0) return(SIN);
  if (strcmp(keyword, "JJ") == 0) return(JJMODEL);
  if (strcmp(keyword, "XMISSION") == 0) return(TRANMODEL);
  if (strcmp(keyword, "NODEV") == 0) return(NODEV);
  if (strcmp(keyword, "DEVI") == 0) return(DEVI);
  if (strcmp(keyword, "DEVV") == 0) return(DEVV);
  if (strcmp(keyword, "PHASE") == 0) return(PHASE);
  if (strcmp(keyword, "CAP") == 0) return(CAPMOD);
  if (strcmp(keyword, "ICRIT") == 0) return(ICRIT);
  if (strcmp(keyword, "R0") == 0) return(R0);
  if (strcmp(keyword, "RN") == 0) return(RN);
  if (strcmp(keyword, "RTYPE") == 0) return(RTYPE);
  if (strcmp(keyword, "CCT") == 0) return(CCT);
  if (strcmp(keyword, "ICON") == 0) return(ICON);
  if (strcmp(keyword, "DELV") == 0) return(DELV);
  if (strcmp(keyword, "VG") == 0) return(VG);
  if (strcmp(keyword, "VNTOL") == 0) return(VNTOL);
  if (strcmp(keyword, "INTOL") == 0) return(INTOL);
  if (strcmp(keyword, "PHITOL") == 0) return(PHITOL);
  if (strcmp(keyword, "VRELTOL") == 0) return(VRELTOL);
  if (strcmp(keyword, "IRELTOL") == 0) return(IRELTOL);
  if (strcmp(keyword, "PHIRELTOL") == 0) return(PHIRELTOL);
  if (strcmp(keyword, "RELTOL") == 0) return(RELTOL);
  if (strcmp(keyword, "PHIQUIT") == 0) return(PHIQUIT);
  if (strcmp(keyword, "NUMDGT") == 0) return(NUMDGT);
  if (strcmp(keyword, "LTE") == 0) return(LTE);
  if (strcmp(keyword, "MAXPHISTEP") == 0) return(MAXPHISTEP);
  if (strcmp(keyword, "MAXFLUXSTEP") == 0) return(MAXFLUXSTEP);
  if (strcmp(keyword, "CONDEV") == 0) return(CONDEV);
  if (strcmp(keyword, "NUSAMPLE") == 0) return(NUSAMPLE);
  if (strcmp(keyword, "NUSEGMENT") == 0) return(NUSEGMENT);
  if (strcmp(keyword, "JJALL") == 0) return(JJALL);
  if (strcmp(keyword, "JJJOSEPH") == 0) return(JJJOSEPH);
  if (strcmp(keyword, "JJRESIS") == 0) return(JJRESIS);
  if (strcmp(keyword, "JJCAP") == 0) return(JJCAP);
  if (strcmp(keyword, "JJTOTAL") == 0) return(JJTOTAL);
  if (strcmp(keyword, "IGWARN") == 0) return(IGWARN);
  if (strcmp(keyword, "Z0") == 0) return(Z0);
  if (strcmp(keyword, "TD") == 0) return(TD);
  if (strcmp(keyword, "LOSSLESS") == 0) return(LOSSLESS);
  if (strcmp(keyword, "PORT1") == 0) return(PORT1);
  if (strcmp(keyword, "PORT2") == 0) return(PORT2);
  
  return(ILLEGAL);

}  /* get_string_keyword */


int
source_type(int devtype)
{
  switch(devtype)
  {
    case V_SOURCE :
    case PWL_V :
    case SIN_V :
    case PULSE_V :
    case DC_V :   return(V_SOURCE);
    
    case I_SOURCE :
    case SIN_I :
    case PULSE_I :
    case PWL_I :  return(I_SOURCE);

    default : return(ILLEGAL);
  }
}   /* source_type */


int
is_dc_device(int devtype)
{
  switch (devtype)
  {
    case JJ :
    case LOSSLESS_LINE :
    case V_SOURCE :
    case PWL_V :
    case SIN_V :
    case PULSE_V :
    case DC_V :
    case INDUCT :
    case RESIS :  return(TRUE);
   
    default : return(FALSE);
  }
}   /* is_dc_device */
    

int
node_type(int nodetype)
{
  switch(nodetype)
  {
    case PLUS_NODE :
    case PLUS_C_NODE :  return(POSITIVE);
   
    case MINUS_NODE :
    case MINUS_C_NODE : return(NEGATIVE);

    default : return(ILLEGAL);
  }
}   /* node_type */
