/*#define MAIN*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "dpMonoms.h"
#include "writeF.h"

int NofDP=0;
int *DPmonomMap=NULL;
int  DPmonomSize=0;
  
void initDPmonoms(void)
{ if(DPmonomMap) free(DPmonomMap); 
  DPmonomMap=NULL;
  DPmonomSize=0;  
}

 
 static int C(int d, int n)
 { int i,r=1; 
   for(i=0;i<d;i++){ r*=(n-i);r/=(i+1);}
   return r;
 }    

static int num2monom( int NN, int dim, int *monom)
{
  int i,d,d0,dim0,k;
  int N=NN;
  for(i=0;i<dim;i++) monom[i]=0;
  if(N==0) return 0;
  N++;

  for(d=1;N>C(d,dim+d);d++) continue;
  N-=C(d-1,dim+d-1);
  
  d0=d;
  dim0=dim;
  dim--;

  for(k=0;k<dim0;k++,dim--)
  {
    for(i=0; N>C(i,dim+i); i++) continue;
    monom[k]=d-i;
    d=i;
    if(d==0) break;
    N-=C(d-1,dim+d-1);
  }
  return d0;
} 

static int monom2num(int * monom, int dim)
{ int d,i,k,s,pos;
  for(d=0,i=0;i<dim;i++) d+=monom[i];
  if(d==0) return 0;
  
  s=d-1;
  pos=C(s,NofDP+s);      
  for(k=0;k<NofDP;k++)
  { s-=monom[k];
    if(s<0) break;
    pos+=C(s,NofDP-k-1+s);
  }
  return pos;
}



int findMomonPosition(int * monom)
{ int i,d;
  int pos=monom2num(monom,NofDP);
  return  pos;
}

void write_calcDPmonoms(FILE*f)
{ 
  int i;
  int N;
  int Na;      
  fprintf(f,"{\n");
  if(DPmonomSize>1)
  {   
    int * arr1=(int*) malloc(sizeof(int)*DPmonomSize);
    int * arr2=(int*) malloc(sizeof(int)*DPmonomSize);
    int * buff=(int*) malloc(sizeof(int)*NofDP);

    DPmonomMap[0]=1;
    
    for(i=1;i<DPmonomSize;i++) arr1[i]=arr2[i]=-1;
    for(;;)
    { int nplus=0;
       for(N=1;N<DPmonomSize;N++) if(DPmonomMap[N]&&(arr2[N]==-1))
       { int r=  num2monom(N,NofDP,buff);
         
         for(i=0;i<NofDP;i++) if(buff[i]>0)
         { int K;
           buff[i]--;
           K=monom2num(buff,NofDP);
           if(DPmonomMap[K]) 
           { arr1[N]=i;
             arr2[N]=K;
             break;
           }else  buff[i]++;  
         }
         if(i==NofDP) for(i=0;i<NofDP;i++) if(buff[i]>0) 
         { int K;
           buff[i]--;
           K=monom2num(buff,NofDP);
           arr1[N]=i;
           arr2[N]=K;
           DPmonomMap[K]=1;
           nplus++; 
           break;
         }    
       }
       if(nplus==0) break;
    } 
    DPmonomMap[0]=0;
    for(N=1,Na=1;N<DPmonomSize;N++) if(DPmonomMap[N]) DPmonomMap[N]=Na++;
    fprintf(f," static const int NDP[%d]={",Na-1);
    for(N=1;N<DPmonomSize && DPmonomMap[N]==0 ;N++) continue;
    fprintf(f,"%d",arr1[N]);
    for(N++;N<DPmonomSize;N++) if(DPmonomMap[N])fprintf(f,",%d",arr1[N]);
    fprintf(f,"};\n");
   
    fprintf(f," static const int NMN[%d]={",Na-1);
    for(N=1;N<DPmonomSize && DPmonomMap[N]==0 ;N++) continue;

    fprintf(f,"%d",DPmonomMap[arr2[N]]); 

    for(N++;N<DPmonomSize;N++)if(DPmonomMap[N]) fprintf(f,",%d",DPmonomMap[arr2[N]]);
    fprintf(f,"};\n");
    fprintf(f," int i;\n");
    fprintf(f," if(nMonoms < %d)\n  {",Na);
    fprintf(f,"DPmonom=(Real*)realloc(DPmonom, sizeof(Real)*%d);\n",Na);
    fprintf(f,"   nMonoms=%d;\n  }\n",Na); 
    fprintf(f," for(i=0,DPmonom[0]=1;i<%d;i++) DPmonom[i+1]=DP[NDP[i]]*DPmonom[NMN[i]];\n",Na-1);
    free(arr1);free(arr2); free(buff);
  } else fprintf(f," DPmonom[0]=1;\n");
  fprintf(f,"}\n");
} 

#ifdef MAIN

int main(void)
{
  int i;
  int xx[20];
 
 num2monom(15,15,xx);
 for(i=0;i<15;i++) printf(" %d",xx[i]);
 printf("\n"); 

}
#endif
