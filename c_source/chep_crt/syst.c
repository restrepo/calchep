/*
 Copyright (C) 1997, Alexander Pukhov 
*/

#include<stdarg.h>
#include"syst.h"



char *  trim(char* p)
{  int n1=0, n2, k=-1;
   n2=(int)strlen(p)-1;
   while(!isgraph(p[n1]) && n1 <= n2) n1++;
   while(!isgraph(p[n2]) && n1 <  n2) n2--;
   while(++k < n2-n1+1)  p[k] = p[k+n1];
   p[k] = '\0';
   return p;
}


void  (*diskerror)(void) = NULL;

int f_printf(FILE *fp,char * format, ... )
{ va_list args;
  char dump[STRSIZ];
  int  r;
  va_start(args, format);

        vsprintf(dump,format,args);
	va_end(args);
	r = fputs(dump,fp);
	if (r == EOF)
	{ if(diskerror)(*diskerror)(); else sortie(65); }
	return r;
}


size_t f_write(void* ptr,  size_t size,  size_t n,  FILE * fp)
{ size_t nn;
   if ((size == 0)||(n == 0)) return 0;
   nn= fwrite(ptr,size,n,fp);
   if (nn != n) {if(diskerror) (*diskerror)() ; else sortie(65);}
   return nn;
}


void sortie(int code) { exit(code); }

