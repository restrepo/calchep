
#include"SLHAplus.h"

extern int FError;
#define  STRSIZ 1000

int System( char * format, ...) 
{  
   va_list args;
   char command[STRSIZ];
   int err;
   
   va_start(args,format);
   vsprintf(command,format,args);
   va_end(args);
   err=system(command);
   if(err==-1||WIFSIGNALED(err) ||WEXITSTATUS(err) ) {FError=1; return -1;}
   return WEXITSTATUS(err);  
} 

static char * FName=NULL;

int openAppend(char * fileName)
{ FILE*f;
  if(access(fileName,F_OK)==0) unlink(fileName);
  FName=realloc(FName, strlen(fileName)+1);
  sprintf(FName,"%s",fileName);
  f=fopen(fileName,"a");
  if(!f) { FError=1; return -1;}
  fclose(f);  
  return 0;
} 

int aPrintF(char * format, ...)
{  int err;
   FILE* f; 
   va_list args;
   char text[2*STRSIZ];
   if(!FName) {FError=1; return 0;}  
   f=fopen(FName,"a");
   if(f==NULL) {FError=1;return -1;}   
   va_start(args,format);
   vsprintf(text,format,args);
   va_end(args);
   err=fprintf(f,"%s",text);
   fclose(f);
   return err;
}
