#include "syst.h"
#include "crt_util.h"
#include "file_scr.h"

#include "help.h"

char pathtohelp[256]="";
extern char pathtocomphep[];

int show_help(char * fname)
{
   char f_name[STRSIZ];
   FILE* f;
   int z[4];
   sprintf(f_name,"%s%s%s",pathtohelp,fname,".txt");

   f=fopen(f_name,"r"); 
   if (f == NULL) return 0;
   fgets(f_name,STRSIZ,f);
   if(3==sscanf(f_name,"%d %d %d",&z[0],&z[1],&z[2]))
   {
      z[2]=z[2]+z[0]+1;
      showtext (z[0],z[1],z[2],z[1],fname,f);
      fclose(f);
      return 1;
   } else 
   fclose(f);
   {
     char command[500];
     sprintf(command, "%s/bin/showHelp %s &",pathtocomphep,fname);
     system(command);
     return 1;
   }
    
}

