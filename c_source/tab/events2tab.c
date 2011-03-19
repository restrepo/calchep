#include"e_tools.h"
#define Real double
#include"4_vector.h"
#include"phys_val.h"
#include"interface.h"
/*
#define const
#include"num_out.h"
#undef const
*/
#include"paragraphs.h"


static void wrongParam(int N)
{ 
   if(N) fprintf(stderr,"Wrong parameter %d\n",N);
   else  fprintf(stderr,"Wrong number of parameters \n");  
   fprintf(stderr,  
    "Parameters:\n"
    " 1- name of variable,\n"
    " 2- minimum limit,\n"
    " 3- maximum limit,\n"
    " 4- number of bins(<=300).\n"
    "File with events must be passed to input. For example:\n"
    "   ../bin/events2tab \"M(b,B)\" 1 100 200 < events_1.txt >tab.txt\n");
}


static long nEvents=0;
static double totCS=0;

static int skipHeadLine(FILE* flow) {fscanf(flow,"%*[^\n]\n"); return -1;}
static int readCS(FILE* flow) {fscanf(flow,"%lf",&totCS); return 0;}
static int readNEvents(FILE* flow) {fscanf(flow,"%ld",&nEvents); return 0;}


int  main(int argc,char** argv)
{ 
  char buff[STRSIZE];
  char varName[NAMELEN];
  int i;
    
  double minX, maxX;
  int nbin;
  char  key[4];
  physValRec * plist;
  
  double *hist, *dhist;
  double weight,coef;
  long nPoints;  
  double mass[MAXNP];

  rw_paragraph  rd_array[8]=
  {
    {"CalcHEP",NULL },
    {"Type",           getNinNout},
    {"Initial_state",  NULL},
    {"PROCESS",         getNames   },
    {"MASSES",         getMasses  },
    {"Cross_section(Width)", readCS},
    {"Number_of_events", readNEvents},
    {"Events",          skipHeadLine}
  };
 
  if(argc != 5 ) { wrongParam(0); return 1;} 
  readParagraphs(stdin,8,rd_array); 
  if(!checkPhysValN(argv[1], key, &plist)) { wrongParam(1); return 1;}
    else  strcpy(varName, argv[1]);
       
  if(sscanf(argv[2],"%lf",&minX)!=1){ wrongParam(2); return 1;}
  if(sscanf(argv[3],"%lf",&maxX)!=1 || minX>=maxX){ wrongParam(3); return 1;}
  if(sscanf(argv[4],"%d",&nbin)!=1  || nbin<=0)
    { wrongParam(4); return 1;}

  hist=(double*)malloc(nbin*sizeof(double));
  dhist=(double*)malloc(nbin*sizeof(double));
  for(i=0;i<nbin;i++){hist[i]=0; dhist[i]=0;}

  for(i=0;i<nin_int+nout_int;i++) pinf_int(1,i+1,mass+i,NULL);
  nPoints=0;
  
  while(fscanf(stdin,"%lf",&weight)==1)
  { 
    for(i=0;i<4*nin_int;i++) pvect[i]=0;
    if(nin_int ==2) fscanf(stdin,"%lf %lf",pvect+3,pvect+7); 
    for(i=nin_int;i<nin_int+nout_int;i++)
    {  double *p = pvect+4*i;
       fscanf(stdin,"%lf%lf%lf",p+1,p+2,p+3);
    }
    weight*=totCS/nEvents;
    for(i=0;i<nin_int+nout_int;i++) pvect[4*i]=ENERGY(mass[i],pvect+4*i+1);
    { double z0[100];
      int k,n0=0;
      physValRec * plist0=plist;
      for(;plist0;plist0=plist0->next) z0[n0++]=calcPhysVal(key[0],plist0->pstr);
      switch(key[1])
      { case '^':
         for(k=1;k<n0;k++) if(z0[0]<z0[k]) z0[0]=z0[k];
         n0=1;
         break;
        case '_':
         for(k=1;k<n0;k++) if(z0[0]>z0[k]) z0[0]=z0[k];
         n0=1;
         break;
      }    
      for(k=0;k<n0;k++)
      {
        i=nbin*(z0[k]- minX)/(maxX-minX); 
        if(i>=0 && i<nbin) 
          {hist[i]+=weight; dhist[i]+=weight*weight; nPoints++;}
      }
    }

    fgets(buff,1000,stdin);
  }
  
  coef=nbin/(maxX - minX);
  
  for(i=0;i<nbin;i++)
  { dhist[i]=coef*sqrt( dhist[i] - hist[i]*hist[i]/nPoints);
    hist[i]*=coef;
  }


  { char  xname[200], yname[200], xunits[100];
    fprintf(stdout,"#title ");
    for(i=0;i<nin_int+nout_int;i++) 
    { if(i==nin_int) fprintf(stdout," ->"); else if(i) fprintf(stdout,",");
      fprintf(stdout," %s",pinf_int(1,i+1,NULL,NULL));
    } 
    xUnit(key[0], xunits);
    if(nin_int==2) sprintf(yname,"Diff. cross section [pb/%s]",xunits);
    else        sprintf(yname,"Diff. width [GeV/%s]",xunits);
    fprintf(stdout,"\n");
      

   fprintf(stdout,"#type 1  %%1d-histogram\n");
   fprintf(stdout,"#xName %s\n",argv[1]);
   fprintf(stdout,"#xMin %E\n",minX);
   fprintf(stdout,"#xMax %E\n",maxX);
   fprintf(stdout,"#xDim %d\n",nbin);
   fprintf(stdout,"#yName %s\n",yname);
  }
  for(i=0;i<nbin;i++) fprintf(stdout,"%-12E  %-12E\n",hist[i],dhist[i]);
  return 0;
}
