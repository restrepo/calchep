#ifndef __L_STRING_
#define __L_STRING_

#include"syst2.h"

 extern void  fortwriter(char * name,varptr fortformula);
 extern void  for_writer(char * name,varptr fortformula);
 
 extern int   write_const(void);
 extern int *  for_write_const(varptr fortformula,int *Ntt,int *nCoef);
 extern int   cleardegnames(void);
 extern void  initdegnames(void);
 extern int tmpNameMax;

#endif
