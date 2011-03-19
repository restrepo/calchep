
#include"interface.h"

#include"subproc.h"
#include"rw_sess.h"
#include"kininpt.h"
#include"alphas2.h"
#include"runVegas.h"

#include"LDtype.h"
 

int hPrec_int;
int nin_int;
int nout_int;
int nprc_int;
int nvar_int;
int nfunc_int;

char * (*pinf_int)(int nsub, int nprtcl, void* pmass,long * num);
char ** polarized_int;
char ** varName_int;

double (*sqme_int)(int nsub, void * momenta, int * err);
int (*calcFunc_int)(void);
double *BWrange_int;
int *twidth_int, *gtwidth_int, *gswidth_int;
void *va_int;



void (*build_cb_int)(int nsub); 
void (*destroy_cb_int)(void);    
int *cb_pow_int;   
int *cb_nc_int; 
int ** cb_chains_int;
double ** cb_coeff_int;

#include"../../include/num_out.h"

char * hiddenf=NULL;

void link_process( CalcHEP_interface * interface)
{ int i;
  hPrec_int=      interface->hPrec;
  nin_int =       interface->nin;
  nout_int=       interface->nout;
  nprc_int=       interface->nprc;
  nvar_int=       interface->nvar;
  nfunc_int=      interface->nfunc;

  pinf_int=       interface->pinf;
  polarized_int=  interface->polarized;
  varName_int=    interface->varName;

  sqme_int=       interface->sqme;
  calcFunc_int=   interface->calcFunc;
  BWrange_int=    interface->BWrange;
  gtwidth_int=    interface->gtwidth;
  twidth_int=     interface->twidth;
  gswidth_int=    interface->gswidth;
  va_int=         interface->va;

  build_cb_int=   interface->build_cb;
  destroy_cb_int= interface->destroy_cb;
  cb_pow_int=     interface->cb_pow;
  cb_nc_int=      interface->cb_nc;
  cb_chains_int=  interface->cb_chains;
  cb_coeff_int=   interface->cb_coeff;

  hiddenf=realloc(hiddenf,nfunc_int);
  for(i=0; i<nfunc_int; i++) if(readVar(1+i+nvar_int)>0.5)hiddenf[i]=1; else hiddenf[i]=0;

  Nsub=1;
  wrtprc_();

  stdkin_();
  i_qcd();
  nSess = 1;

  integral.n_it=0;

  *BWrange_int=2.7;
  *gswidth_int=0;
  *gtwidth_int=0;
  *twidth_int=0;
  if(nin_int==1) inP1=0;

}

double readVar(int k)
{
  double * varD=(double *) va_int;
  Real * varLD=(Real *) va_int;
  if(k==-1) return Helicity[0];
  else if(k==-2) return Helicity[1];
  else if(hPrec_int) return  (double)varLD[k]; 
  else return varD[k];    
}

void writeVar(int k, double val)
{
  double * varD=(double *) va_int;
  Real * varLD=(Real *) va_int;
  if(k==-1) Helicity[0]=val;
  else if(k==-2)Helicity[1]=val;
  else if(hPrec_int) varLD[k]=(Real)val; 
  else  varD[k]=val;
}

void * saveVar(void)
{ void * mem;
  int size=1+nvar_int+nfunc_int;
  if(hPrec_int) size*=sizeof(Real); else size *=sizeof(double);
  mem=malloc(size);
  memcpy(mem, va_int,size);
  return mem;
}

void restoreVar(void* mem)
{
  int size=1+nvar_int+nfunc_int;
  if(hPrec_int) size*=sizeof(Real); else size *=sizeof(double);
  memcpy(va_int,mem,size);
}

double doublePmass(int nsub,int nprctl)
{ double m;
  Real hm;
  if(hPrec_int) { pinf_int(nsub,nprctl,&hm,NULL); return (double)(hm);}
  else          { pinf_int(nsub,nprctl,&m,NULL);  return m;}
}
