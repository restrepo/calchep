/*
 Copyright (C) 1997, Alexander Pukhov, e-mail: pukhov@theory.npi.msu.su
*/
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "chep_crt.h"
#include "syst2.h"
#include "s_files.h"

static void xw_error(void) { sortie(80);}

int main(int argc,char** argv)
{   
  int n;
  if(argc<2) return 0;
  
  blind=0;
  
  strcpy(pathtocomphep,argv[0]);
  for(n=strlen(pathtocomphep)-1; n && pathtocomphep[n]!=f_slash; n--);
  pathtocomphep[n-3]=0;                                         
     
   
   sprintf(pathtohelp,"%shelp%c",pathtocomphep,f_slash);
   outputDir="results/";        
   { char * icon=(char *) malloc(strlen(pathtocomphep)+10);
     sprintf(icon,"%sicon",pathtocomphep);
     { char helpfile[500];
       char title[100];
       FILE*f; 

       sprintf(helpfile,"%shelp/%s.txt",pathtocomphep,argv[1]);
       f=fopen(helpfile,"r");
       if(!f) return 1;
       fscanf(f,"%[^\n]\n",title);
       start1(title,icon,"calchep.ini",&xw_error);
       showtext(1,1,79, 24 , argv[1],  f ); 
       finish();
       exit(0);   
     }   
   }
}   
