#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<unistd.h>
#include<time.h>

#include "event2pyth.h"

void  upinit_(void)
{ int I;

  R_.NPRUP= 1;
  R_.LPRUP[0]=1;
  for(I=0;I<2;I++) { R_.PDFSUP[I]=-1; R_.PDFGUP[I]=-1;}
  E_.AQEDUP=-1;
  E_.AQCDUP=-1;
  R_.IDWTUP = 3;
  R_.XERRUP[0] = 0.;
  R_.XMAXUP[0] = 1.;
}

int main(int argc,char ** argv)
{
  int N,NEV,MAXEVENTS,I,J,K,err;
  double cs;
  FILE *lun1=NULL;

  if(argc<3 || sscanf(argv[1],"%d",&NEV)!=1 || NEV<0) err=1; else err=0;

  if(argc<3) 
  { printf("%s needs arguments: nEvents event_directories ...\n",argv[0]);
    exit(1);
  }
  

  srand48(time(NULL));

  if(argv[1][0]=='?') NEV=-1;
  else
  { if(sscanf(argv[1],"%d",&NEV)!=1 || NEV<0)
    {printf("First argument is wrong.\n"); exit(1);}  
  } 
  if(access("decaySLHA.txt", R_OK)==0) readslha_();
  
  for(I=2;I<argc;I++) scandir_(argv[I],strlen(argv[I]));
            
  if(NEV==-1) { writeInfo(); exit(0);}

  
  eventstat_(&cs, &MAXEVENTS); 
  if(cs==0) 
  {  printf("There are no events \n");
     exit(1);
  }   

  printf("total cross section %.3E\n", cs);
  printf("Max number of events %.3d\n", MAXEVENTS);
  if(NEV==0) exit(0);  

  lun1=fopen("event_mixer.lhe","w");
  
                                                               
     
  upinit_();
      
  if(lun1)
  {
    fprintf(lun1,"<LesHouchesEvents version=\"1.0\">\n");
    fprintf(lun1,"<!--\n");
    fprintf(lun1,"File generated with CalcHEP-PYTHIA interface\n");
    fprintf(lun1,"-->\n");
    fprintf(lun1,"<header>\n");

    fprintf(lun1,"<slha>\n");
    if(access("qNumbers", R_OK)==0)
    {  FILE * f=fopen("qNumbers","r");
       int ch;
       for(;;)
       { ch=fgetc(f);
         if(ch== EOF) break;
         fputc(ch,lun1);
       }
       fclose(f);
    }
    if(access("decaySLHA.txt", R_OK)==0) 
    { FILE * f=fopen("decaySLHA.txt","r");
       int ch;
       for(;;)
       { ch=fgetc(f);
         if(ch== EOF) break;
         fputc(ch,lun1);
       }
       fclose(f);
    }
    
    fprintf(lun1,"</slha>\n");

    if(access("run_details.txt", R_OK)==0) 
    {  FILE * f=fopen("run_details.txt","r");
       int ch;
       for(;;)
       { ch=fgetc(f);
         if(ch== EOF) break;
         fputc(ch,lun1);
       }
      fclose(f);
    }
    
    fprintf(lun1,"</header>\n");	
    fprintf(lun1,"<init>\n");
    fprintf(lun1," %5d %5d %18.11E %18.11E %5d %5d %5d %5d %5d %5d\n",
      R_.IDBMUP[0],R_.IDBMUP[1],R_.EBMUP[0], R_.EBMUP[1], 
      R_.PDFGUP[0],R_.PDFGUP[1],R_.PDFSUP[0],R_.PDFSUP[1],
      R_.IDWTUP,R_.NPRUP);

    fprintf(lun1," %18.11E %18.11E %18.11E %3d\n",
             cs,R_.XERRUP[0],R_.XMAXUP[0], R_.LPRUP[0]); 
    fprintf(lun1,"</init>\n");
  }
 
  for(N=1;N<=NEV;N++)
  {
    while(upevnt_());
    if(E_.NUP==0){printf("Only %d events are generated\n", N-1); break;}
    
    if(lun1)
    {
      fprintf(lun1,"<event>\n");
      
      fprintf(lun1,"%2d %4d %15.7E %15.7E %15.7E %15.7E\n",
          E_.NUP,E_.IDPRUP,E_.XWGTUP,E_.SCALUP,E_.AQEDUP,E_.AQCDUP);
     
      for(I=0;I<E_.NUP;I++)
      {
        fprintf(lun1," %8d %4d",  E_.IDUP[I],E_.ISTUP[I]);
        for(J=0;J<2;J++) fprintf(lun1," %4d", E_.MOTHUP[I][J]);
        for(J=0;J<2;J++) fprintf(lun1," %4d", E_.ICOLUP[I][J]);
        for(J=0;J<5;J++)  fprintf(lun1," %18.11E",E_.PUP[I][J]);
        fprintf(lun1," %11.4E %4.1f\n",E_.VTIMUP[I],E_.SPINUP[I]);
      }
	
      fprintf(lun1,"</event>\n");
    }   
  }       

  if(lun1){ fprintf(lun1,"</LesHouchesEvents>\n");fclose(lun1);}
  closeevents_();
  return 0;
}
