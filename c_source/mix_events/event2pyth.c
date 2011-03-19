int FError;

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <dirent.h>

#include "event2pyth.h"

hepeup_str E_;
heprup_str R_;

#define MORE
#define CHECK_E_M
static double totCS=0, maxLum=0;

static int SMid[18]=      {1,2,3,4,5,6,   11,12,13,       14, 15,     16,21,22,23,     24,     81,83};
static double SMwidth[18]={0,0,0,0,0,1.59,0, 0, 3.E-19 , 0 , 2.26E-12,0 ,0, 0, 2.49444,2.08895,0,  0};
static int SLHA=0;

extern int slhaRead(char*,int);
extern int slhaDecayExists(int);
extern double slhaWidth(int);


typedef struct eventfile_info
{ 
  struct eventfile_info* next;
  FILE *F;  
  char * fileName;
  int Nin,Nout;
  double cs;
  double inMom[2];
  int inPID[2];
  int PIDs[10];
  char pName[10][6];  
  double pmass[20];
  long nEvents;
  long cEvent;
  long FirstEventPos;
  long CurrentEventPos;  
} eventfile_info;

eventfile_info * All=NULL;

typedef struct decay_info
{ 
  struct decay_info* next;
  int ID;
  double totWidth;
  double  width;
  double  lost;
  eventfile_info * List;
}  decay_info;

decay_info * Decays=NULL;

static void fName2c(char*f_name,char*c_name,int len)
{ int i; for(i=len-1;i>=0 &&f_name[i]==' ';i--);
  c_name[i+1]=0;
  for(;i>=0;i--) c_name[i]=f_name[i];
}

static void confirmEvent_aux(eventfile_info * D, int ok)
{  
   for(;D;D=D->next)
   { long p=ftell(D->F);
     if(p!=D->CurrentEventPos)
     { if(ok) {D->CurrentEventPos=p; D->cEvent++;}
         else   fseek(D->F, D->CurrentEventPos, SEEK_SET);
     }
   }
}

static void  confirmEvent(int ok)
{ decay_info * D;
  
  confirmEvent_aux(All,ok);
         
  for(D=Decays;D;D=D->next)
  { eventfile_info * E=Decays->List;
    confirmEvent_aux(E,ok);
  }   
}

int readslha_(void)
{ int err=slhaRead("decaySLHA.txt",0);
  if(!err) SLHA=1;
  return err;
}  


static eventfile_info * initEventFile(char* fname)
{
   char buff[200];
   eventfile_info * Finfo;
   FILE*F;
   int i,n;
   int ntot;
   
   F=fopen(fname,"r");
   if(F==NULL) return NULL;
   
   Finfo=(eventfile_info *)malloc(sizeof(eventfile_info)); 

   Finfo->F=F;
        
   for(fscanf(F,"%s",buff);!feof(F); )
   {
      if(strcmp(buff,"#Type")==0) 
      { fscanf(F,"%d -> %d", &Finfo->Nin, &Finfo->Nout);
        ntot=Finfo->Nin+Finfo->Nout;        
      } else if(strcmp(buff,"#Initial_state")==0)
      {
        for(i=0;i<2;i++) { Finfo->inPID[i]=0;Finfo->inMom[i]=0;}
        if(Finfo->Nin==2)
        { 
          for(i=0;i<2;i++){fscanf(F," P%d_3=",&n); fscanf(F,"%lf",Finfo->inMom+n-1);}
          for(i=0;i<2;i++) 
          { fscanf(F," StrFun%d=",&n); 
            if(fscanf(F,"\"%*[^\"]\" %d",Finfo->inPID+n-1)==0)Finfo->inPID[n-1]=0;
          }
        }  
      }
      else if(strcmp(buff,"#PROCESS")==0)
      {  int np;
         for(np=0;np<ntot;np++)
         { fscanf(F,"%d(%[^)])",Finfo->PIDs+np,Finfo->pName[np]);
           if(np==Finfo->Nin-1)fscanf(F," -> ");
         }  
      }  else if(strcmp(buff,"#MASSES")==0)
      { int i;
         for(i=0;i<ntot;i++) fscanf(F," %lf",&Finfo->pmass[i]);
      } else if(strcmp(buff,"#Cross_section(Width)")==0)
      {  if( fscanf(F," %lf",&Finfo->cs)==0) Finfo->cs=0;
      } else if(strcmp(buff,"#Number_of_events")==0)
      {  fscanf(F," %ld",&Finfo->nEvents);      
      } else if(strcmp(buff,"#Events")==0)
      {  fscanf(F,"%*[^\n]%*c");
         for(i=0;i<Finfo->Nin;i++) if(Finfo->inPID[i]==0) Finfo->inPID[i]=Finfo->PIDs[i];

         if(Finfo->cs == 0) 
         { 
            fprintf(stderr,"Error: zero cross section/width in %s\n",
            Finfo->fileName);
            exit(5);
         } 
         if(Finfo->cs<0) Finfo->cs*=-1; 
         Finfo->cEvent=1;
         Finfo->FirstEventPos=ftell(F);
         Finfo->CurrentEventPos=Finfo->FirstEventPos;
         Finfo->fileName=malloc(strlen(fname)+1);
         strcpy(Finfo->fileName,fname);
         return Finfo;
      }
      fscanf(F,"%s",buff);
   }
   return NULL; 
}

static eventfile_info* secondCopy(eventfile_info* new, eventfile_info* old)
{ int i;
  for(;old; old=old->next) if(new->Nout==old->Nout)
  { 
     for(i=0;i<new->Nin+new->Nout;i++) if(new->PIDs[i]!=old->PIDs[i]) break;
     if(i>=new->Nin+new->Nout) return  old;
  }
  return NULL;
}

int scandir_( char * dirname, int len)
{
  DIR *dirPtr;
  struct dirent * dp;
  char dirName[200];
  eventfile_info *Finfo, *Copy;
  int count=0;
  int i;
   
  
  fName2c(dirname,dirName,len);
      
  dirPtr=opendir(dirName);
  if(!dirPtr)
  { fprintf(stderr,"ERROR:%s - no such directory\n",dirName);
    exit(1);
  }  
  while((dp=readdir(dirPtr)))
  { char fullName[300];
    int N;
     
    if( sscanf(dp->d_name,"events_%d",&N)==1)
    { sprintf(fullName,"%s/%s", dirName,dp->d_name);  
      Finfo=initEventFile(fullName);
      if(!Finfo) continue;
      if(Finfo->Nin==2) 
      { double lum=Finfo->nEvents/Finfo->cs;

        if(All)
        { for(i=0;i<2;i++) 
          if( All->inMom[i]!=Finfo->inMom[i] ||
              All->inPID[i]!=Finfo->inPID[i]   ) 
          { fprintf(stderr, "Error: different incoming states in\n"
             "%s \n and  %s\n", All->fileName, Finfo->fileName);
            exit(1);
          }
          Copy= secondCopy(Finfo,All); 
          if(Copy) 
          { fprintf(stderr,"Error: Indentical processes in files\n%s\n%s\n",
            Copy->fileName, Finfo->fileName);
            exit(2);
          }
          if(maxLum > lum) maxLum=lum;          
        }else  maxLum=lum; 
          
        Finfo->next=All;
        All=Finfo;
        totCS+=All->cs;      
        count ++;
      }
      if(Finfo->Nin==1) 
      { 
        decay_info * D; 
        for(D=Decays;D;D=D->next)if(D->ID==Finfo->PIDs[0])
        { 
          Copy=secondCopy(Finfo,D->List);
          if(Copy) 
          { fprintf(stderr,"Error: Indentical processes in files\n%s\n%s\n",
             Copy->fileName, Finfo->fileName);
             exit(2);
          }  
          Finfo->next=D->List;
          D->List=Finfo;
          D->width+=Finfo->cs;
          if(D->totWidth<D->width) D->totWidth=D->width;
          break;
        }  
        if(!D)  
        { D=malloc(sizeof(decay_info));
          D->next=Decays;
          D->ID=Finfo->PIDs[0];
          if(SLHA && slhaDecayExists(abs(D->ID))>0) D->totWidth=slhaWidth(abs(D->ID));
          else 
          {int i; for(i=0;i<18;i++) if(abs(D->ID)==SMid[i])
                  { D->totWidth=SMwidth[i]; break;}
             if(i==18) {printf("Total width of %d is not found\n",D->ID); D->totWidth=0;}
          }  
          
          D->width=Finfo->cs;       
          if(D->totWidth < D->width) D->totWidth=D->width;
          D->List=Finfo;
          Decays=D;
          count++;
        }   
      }
    } 
  }  
  closedir(dirPtr);
  if(All)
  {  
     R_.XSECUP[0] = totCS;
     for(i=0;i<2;i++)
     { double m,p;
       R_.IDBMUP[i]=All->inPID[i];
       if(All->inPID[i]!=All->PIDs[i] &&(fabs(All->inPID[i])==2212))  m=0.98;
       else  m=All->pmass[i];
       p=All->inMom[i];          
       R_.EBMUP[i]=sqrt(m*m+p*p);           
     }  
  }
  return count;
}


static void readEvent(eventfile_info *Finfo, int *Nmom, double * mom, int * clr1, int * clr2, double *Q, int * w)
{ int n=0;
  if(Finfo->cEvent >= Finfo->nEvents)
  { Finfo->cEvent=1;
    fseek(Finfo->F, Finfo->FirstEventPos, SEEK_SET);
    Finfo->CurrentEventPos=Finfo->FirstEventPos;  
    fprintf(stderr,"Warning: File %s finishes. Return to the beginning.\n",
    Finfo->fileName);
  } 

  fscanf(Finfo->F,"%d",w);
  for(;1==fscanf(Finfo->F," %lf",mom+n);n++);
  *Nmom=n;
  fscanf(Finfo->F,"| %lf",Q);
  for(n=0;2==fscanf(Finfo->F," (%d %d)",clr1+n,clr2+n);n++);
  clr1[n]=0; clr2[n]=0;
}

static double PQ(double *p)
{ int i; 
  double s;
  for(i=0,s=0; i<3; i++) s+=p[i]*p[i];
  return s;
}


static void  boost(double *p, double shY, double *n)
/* makes Lorentz boost for vector P={p[0],p[1],p[2],p[3]}
   n- normalized 3-vector - direction of boosting
   shY=sh(Y) where Y is the  boost angle
*/   
{  int i;
   double chY=sqrt(1+ shY*shY);
   double np=p[0]*n[0]+p[1]*n[1]+p[2]*n[2];
   double f= p[3]*shY + np*(chY-1);
   for(i=0;i<3;i++) p[i]+=n[i]*f;
   p[3]=p[3]*chY+np*shY;
}

static double Ep(double * p)
{
  int i;
  double s; 
  for(i=0,s=0;i<5;i++) if(i!=3) s+=p[i]*p[i];
  s=sqrt(s);
/*  p[3]=s;*/
  return s; 
}

static double sinH(double p_,double p,double m)
/* finds boost angle to accelerate p to p_; m - particle mass */
{  double E_=sqrt(m*m+p_*p_);
   double E= sqrt(m*m+p*p);
   return (p_-p)*( p+p_ +(E*E_+p*p_+p*p+p_*p_+m*m)/(E_+E))/(p_+E_)/(E+p);
}


static double PCM(double sqrtS, double m1, double m2, int * err)
{  double S=sqrtS*sqrtS;
   double ms=m1+m2;
   double md=m1-m2;
   double lambda2=(S-ms*ms)*(S-md*md);
   
   if(lambda2<=0){ *err=1; return 0;}
   *err=0;
   return sqrt(lambda2)/(2*sqrtS);             
}
  
static int momImproveD(int NP,  double*p, double  m_)
{
  int i,i1,k,err;
  double m1,m2,E1,E2,pcm, pcm_,sh_x1,sh_x2;
  double n[4]; 

  if(NP==2) {i1=0; pcm=PQ(p);}  
  else 
  { pcm=0;
    for(i=0;i<NP;i++)
    { double q2=PQ(p+5*i);
      if(q2>pcm) {i1=i; pcm=q2;}
    }
  }
  pcm=sqrt(pcm);
  m1=p[5*i1+4];
  
  for(i=0,E2=0;i<NP;i++) if(i!=i1) E2+=Ep(p+5*i);
  m2=sqrt(E2*E2-pcm*pcm);
  E1=sqrt(m1*m1-pcm*pcm); 
  pcm_=PCM(m_,m1,m2,&err);
  if(err) return 1;
  sh_x2=sinH(pcm_,pcm,m2);
  sh_x1=sinH(pcm_,pcm,m1);
  for(k=0;k<3;k++) n[k]=p[5*i1+k]/pcm;
  boost(p+5*i1, sh_x1, n);
  for(k=0;k<3;k++) n[k]*=-1;
  for(i=0;i<NP;i++) if(i!=i1) boost(p+5*i,sh_x2,n);

  return 0;
}
  
static int momImproveS(int NP, double*p, int * stat,double m1, int i1)
{
  int i,k,err;
  double q[3]={0,0,0},n0[3]={0,0,0},n1[3]={0,0,0};
  double pp,pcm,pcm_,sh_x2,M,E,E1,E2,m2,shY0;
  
  for(i=0,E=0;i<NP;i++) if(stat[i]==1)
  { E+=Ep(p+5*i);
    for(k=0;k<3;k++) q[k]+=p[5*i+k];
  }
  pp=PQ(q);
  M=sqrt(E*E-pp);
  if(pp)
  { pp=sqrt(pp);
    shY0=pp/M;   
    for(k=0;k<3;k++) n0[k]=-q[k]/pp;
    for(i=0;i<NP;i++) boost(p+5*i,shY0,n0);
  }
#ifdef MORE  
  for(k=0;k<3;k++) q[k]=0;
  for(i=0,E2=0;i<NP;i++) if(i!=i1 && stat[i]==1 )
  { E2+=Ep(p+5*i);
    for(k=0;k<3;k++) q[k]+=p[5*i+k];
  }
  
  m2=E2*E2-PQ(q);
  if(m2<=0) m2=0; else m2=sqrt(m2);
  pcm_=PCM(M,m1,m2,&err);  
  if(err) return 1;
  pcm=sqrt(PQ(p+5*i1));
  for(k=0;k<3;k++) n1[k]=p[k+5*i1]/pcm;
  E1=sqrt(m1*m1 +pcm*pcm);
  sh_x2=sinH(pcm_,pcm,m2);

  for(k=0;k<3;k++) p[k+5*i1]=n1[k]*pcm_;
  p[5*i1+3]=sqrt(m1*m1+PQ(p+5*i1));      
  for(k=0;k<3;k++) n1[k]*=-1;
  for(i=0;i<NP;i++) if(i!=i1) boost(p+5*i,sh_x2,n1);
#endif   
  if(pp) for(i=0;i<NP;i++) boost(p+5*i,-shY0,n0);
  return 0;
}      

static double * randBuff=NULL;
static int randBuffSize=0; 
static int randBuffPos;

static double drandPlus(void)
{ double r; 
  if(randBuffPos==randBuffSize)
  { randBuffSize+=50;
    randBuff=realloc(randBuff,randBuffSize*sizeof(double));
  }
  r=drand48();
  randBuff[randBuffPos++]=r;
  return r;
} 

static double drandBuff()
{ 
  if(randBuffPos==randBuffSize) drandPlus();
  return  randBuff[randBuffPos++];
}

static int upEvent0(void)
{
  int I,N;
  double cs;
  eventfile_info * Finfo;
  
  for(Finfo=All,cs=totCS*drandPlus(); cs>Finfo->cs; )
  { cs-=Finfo->cs;
    if(Finfo->next) Finfo=Finfo->next;
  }

  N=Finfo->Nin+Finfo->Nout;
  E_.NUP=N;
  
  for(I=0;I<N;I++)
  { 
     E_.IDUP[I] =Finfo->PIDs[I];
  }
  
  
  for(I=2;I<E_.NUP;I++)
  { decay_info *D;
    for(D=Decays;D;D=D->next)
    { if(D->ID==E_.IDUP[I]) 
      { double width; 
        eventfile_info *Finfo=D->List;
        int k;
        int shift=E_.NUP-1;
    
        width=D->totWidth*drandPlus();
        if(width>D->width) return 1;

        for(width=D->width*drandPlus(); Finfo->cs <= width;)
        {
          width-=Finfo->cs;        
          if(Finfo->next) Finfo=Finfo->next;
        }
        for(k=1;k<=Finfo->Nout;k++)
        {  
          E_.IDUP[shift+k] =Finfo->PIDs[k];
        }
        E_.NUP+=Finfo->Nout;
        break;
      }
    }      
  }
  return 0;
}


static int upEvent1(void)
{
   int i,I,j,N, CC,Nmom,w, clr1[10],clr2[10];
   double cs, mom[32], Q;
   eventfile_info * Finfo;
   

   for(Finfo=All,cs=totCS*drandBuff(); cs>Finfo->cs; )
   { cs-=Finfo->cs;
     if(Finfo->next) Finfo=Finfo->next;
   }
   E_.IDPRUP=1;
   E_.XWGTUP=1.;
   N=Finfo->Nin+Finfo->Nout;
   E_.NUP=N;
   
   for(I=0;I<N;I++)
   {  E_.PUP[I][4]=Finfo->pmass[I];
      E_.IDUP[I] =Finfo->PIDs[I];
      E_.SPINUP[I]=9;
      E_.VTIMUP[I]=0;
      for(j=0;j<2;j++) E_.ICOLUP[I][j]=0;
   }
   readEvent(Finfo,&Nmom, mom, clr1, clr2, &Q, &w);
   E_.SCALUP=Q;
   
   for(I=0;I<2;I++)
   { for(j=0;j<2;j++) {E_.PUP[I][j]=0;E_.MOTHUP[I][j]=0;}
     E_.PUP[I][2]=mom[I];
     E_.ISTUP[I]=-1;
   } 

   for(I=2;I<N;I++) 
   {  for(j=0;j<2;j++) E_.MOTHUP[I][j]=j+1;
      for(j=0;j<3;j++) E_.PUP[I][j]=mom[2+3*(I-2)+j];
      E_.ISTUP[I]=1;    
   } 

   for(I=0;I<N;I++) E_.PUP[I][3]=Ep(E_.PUP[I]);

   for(i=0,CC=500;clr1[i];i++,CC++)
   { int k1=clr1[i]-1,k2=clr2[i]-1; 
     if(k1<2) E_.ICOLUP[k1][0]=CC; else E_.ICOLUP[k1][1]=CC;
     if(k2<2) E_.ICOLUP[k2][1]=CC; else E_.ICOLUP[k2][0]=CC;
   }
   
   for(I=2;I<E_.NUP;I++) if(E_.ISTUP[I]==1)
   { decay_info *D;
     for(D=Decays;D;D=D->next)
     { if(D->ID==E_.IDUP[I]) 
     { double width,P,shY,M_; 
       eventfile_info *Finfo=D->List;
       double b[4];
       int k,err;
       int shift=E_.NUP-1;
     
       width=D->totWidth*drandBuff();
       if(width>D->width)
       { confirmEvent(0);
         printf("confirmEvent(0)?\n");
         return 1;
       }
       
       for(width=D->width*drandBuff(); Finfo->cs <= width; )
       {
         width-=Finfo->cs;   
         if(Finfo->next) Finfo=Finfo->next;
       }
       readEvent(Finfo,&Nmom, mom, clr1, clr2, &Q, &w);
       E_.VTIMUP[I]=-1.973E-13/(D->totWidth)*log(drand48());
       
       M_=E_.PUP[I][4]*
         (E_.PUP[I][4]+ D->totWidth*tan((M_PI_2-drand48()*M_PI*0.9999999999)));
       if(M_ <=0) {confirmEvent(1); return 2;}
       M_=sqrt(M_);
      
       err=momImproveS(E_.NUP-2, E_.PUP[2],E_.ISTUP+2,M_,I-2);
       if(err) {confirmEvent(1); return 2;}
       
       P=PQ(E_.PUP[I]);
       P=sqrt(P);
       for(k=0;k<3;k++) b[k]=E_.PUP[I][k]/P;
       
       shY=sinH(P,0.,M_); 
       E_.ISTUP[I]=2;

       for(k=1;k<=Finfo->Nout;k++)
       {  
         for(j=0;j<3;j++) E_.PUP[shift+k][j]=mom[3*(k-1)+j];
         E_.PUP[shift+k][4]=Finfo->pmass[k]; 
         E_.PUP[shift+k][3]=Ep(E_.PUP[shift+k]);         
         E_.MOTHUP[shift+k][0]=I+1;
         E_.MOTHUP[shift+k][1]=I+1;
         E_.SPINUP[shift+k]=9;
         E_.VTIMUP[shift+k]=0;
         E_.ICOLUP[shift+k][0]=0;
         E_.ICOLUP[shift+k][1]=0;
         E_.ISTUP[shift+k]=1;
         E_.IDUP[shift+k] =Finfo->PIDs[k];
       }
       err=momImproveD(Finfo->Nout,E_.PUP[shift+1], M_);
       if(err){confirmEvent(1); return 2;}
       for(k=1;k<=Finfo->Nout;k++) boost(E_.PUP[shift+k],shY,b);
       
      
       for(i=0;clr1[i];i++)
       { int k1=clr1[i]-1, k2=clr2[i]-1;
         if(k1 && k2) 
         { E_.ICOLUP[k1+shift][1]=++CC;
           E_.ICOLUP[k2+shift][0]=CC;
         } else if(k1)  E_.ICOLUP[k1+shift][1]=E_.ICOLUP[I][1];
         else   if(k2)  E_.ICOLUP[k2+shift][0]=E_.ICOLUP[I][0];                      
                      
       }
       
       E_.NUP+=Finfo->Nout;
       break;
     }
     }
           
   }
#ifdef CHECK_E_M   
   { double PL= E_.PUP[0][2]+E_.PUP[1][2];
     double Etot= E_.PUP[0][3]+E_.PUP[1][3];
     double p1=0,p2=0;
     for(I=2;I<E_.NUP;I++) if(E_.ISTUP[I]==1)
     { PL -= E_.PUP[I][2];
       Etot-=E_.PUP[I][3];
       p1+=E_.PUP[I][0];
       p2+=E_.PUP[I][1];
     }  
/*     printf("Zero = %E %E %E\n", PL,Etot, sqrt(p1*p1+p2*p2));*/
   }
#endif     
   confirmEvent(1);
   return 0;
}

int upevnt_(void)
{
  if(!All) 
  { fprintf(stderr,"Error:No event file is found\n");
    exit(2);
  }

  for(randBuffPos=0;upEvent0();randBuffPos=0);
  randBuffPos=0;
  return upEvent1();  
}


static void cleanList(eventfile_info * list)
{  eventfile_info * Finfo;
   for(;list;)
   { fclose(list->F); 
     free(list->fileName);
     Finfo=list;
     list=list->next;
     free(Finfo);
   }
}

void closeevents_(void)
{ decay_info * D;

  totCS=0; maxLum=0;
  cleanList(All); All=NULL;
  for(;Decays;)
  { cleanList(Decays->List);
    D=Decays;
    Decays=Decays->next;
    free(D);
  }   
}

static double BrEff(int ID)
{  decay_info * D;

   for(D=Decays;D;D=D->next) if(ID==D->ID)
   {  eventfile_info * F;
      double sum=0;
      int i;
      for(F=D->List;F;F=F->next)
      { double width=F->cs;
        for(i=1;i<=F->Nout;i++) width*=BrEff(F->PIDs[i]);
        sum+=width;
      }
      return sum/D->totWidth;  
   }
   return 1;
}


void eventstat_(double * cs, int * nevents)
{ 
  eventfile_info * Finfo;
   int i;
   double LuMin,csTot;

   LuMin=-1;
   csTot=0;
   for(Finfo=All;Finfo;Finfo=Finfo->next)
   { double Lu, cs=Finfo->cs;
     for(i=0;i<Finfo->Nout;i++) cs*=BrEff(Finfo->PIDs[i+2]);
     Lu=Finfo->nEvents/cs;
     if(LuMin<0) LuMin=Lu; else{ if(LuMin>Lu) LuMin=Lu;}
     csTot+=cs;
   }
   *cs=csTot;  *nevents=LuMin*csTot; 
}


void writeInfo(void)
{  eventfile_info * Finfo;
   int i,MaxEvents;
   double csTot;
   
   if(SLHA) printf("SLHA decay file is found\n");
   else    printf("SLHA decay file is not found\n");
   printf("List of reactions:\n");
   if(!All) printf("            Empty\n");
   for(Finfo=All;Finfo;Finfo=Finfo->next)
   { 
     printf(" Process  : %s %s -> %s", Finfo->pName[0],Finfo->pName[1],Finfo->pName[2]);
                for(i=4;i<=Finfo->Nin+Finfo->Nout;i++) printf(",%s",Finfo->pName[i-1]); 
                printf("    File: %s\n",Finfo->fileName);  
     printf("    Incoming particles and momenta: %d(%.2E) %d(%.2E)\n",Finfo->inPID[0],Finfo->inMom[0],Finfo->inPID[1],Finfo->inMom[1]);
     printf("    Cross section: %.2E[pb]\n",Finfo->cs);
     printf("    Number of events %ld\n",Finfo->nEvents); 
   }

   printf(" Decay channels:\n");
   if( !Decays) printf(" No decays\n");
   else
   {  decay_info * D=Decays;
      for(D=Decays;D;D=D->next)
      { printf("  Incoming particle: %s\n",D->List->pName[0]);
        if(SLHA) printf("    Total width: %.2E[GeV]\n",D->totWidth);
        printf("    Sum of widths of presented channels: %.2E[GeV]\n",D->width); 
        printf("    List of channels with parion widths and number of events:\n");
        for(Finfo=D->List;Finfo;Finfo=Finfo->next)
        {
          printf("           width=%.2E[GeV] N=%ld  Channel: %s -> %s", Finfo->cs,Finfo->nEvents, Finfo->pName[0],Finfo->pName[1]);
          for(i=3;i<=Finfo->Nin+Finfo->Nout;i++) printf(",%s",Finfo->pName[i-1]); 
          printf("  File: %s\n",Finfo->fileName);
        }
      }
   }
   eventstat_(&csTot,&MaxEvents);
   printf("Effective summary cross section %.2E[pb]\n",csTot);  
/*   printf("Maximal Luminocity %.2E[1/pb]\n",LuMin);*/
   printf("Maximal number of events %d\n",MaxEvents);
}

