#ifndef __NUM_OUT_ext
#define __NUM_OUT_ext


#include<stdlib.h>
#include<string.h> 
#include<math.h>

extern  int    FError;
extern  double Helicity[2];

#ifndef  __CALCHEP_INTERFACE__
#define  __CALCHEP_INTERFACE__
typedef struct CalcHEP_interface
{
  int hPrec;
  int nvar;
  int nfunc;
  char ** varName;
  void* va;

  int nin;
  int nout;
  int nprc;
  char* (*pinf)(int, int , void*,long *);
  char** polarized;
  int (*calcFunc)(void);
  double * BWrange;
  int    * twidth;    
  int *   gtwidth;
  int *   gswidth;

  double (*sqme)(int , void*, int*);

  char * (*den_info)(int, int, int *, int*);
  void (*build_cb)(int);

  int *cb_pow;
  int *cb_nc;
  int **cb_chains;
  double ** cb_coeff;
  void (*destroy_cb)(void);  
} CalcHEP_interface;
/*
extern int    jobInit(CalcHEP_interface * interface);
extern int    jobInState(int nProc, double P1, double P2, char* strf1, char*strf2);
extern int    jobCut2(char * par,double min,  double max);
extern int    jobCutMin(char * par,double min);
extern int    jobCutMax(char * par,double max);
extern void   jobCutDel(void);
extern int    jobHist(double min, char * par, double max);
extern void   jobHistDel(void);
extern double jobVegas(int nSess,int nCalls,int clear,int*err_,double*dI,double*chi2);
*/
#endif

extern CalcHEP_interface interface_ext;
extern CalcHEP_interface * PtrInterface_ext;

void link_process(CalcHEP_interface * interface);

#ifndef RESTRICT
extern const int nin_ext;
extern const int nout_ext;
extern const int nprc_ext;
extern const int nvar_ext;
extern const int nfunc_ext;

extern char * pinf_ext(int nsub,int nprtcl,Real * pmass,long*num);
extern char * varName_ext[];

extern double sqme_ext(int nsub, Real * momenta, int * err);
extern int calcFunc_ext(void);
extern double BWrange_ext;
extern int twidth_ext, gtwidth_ext, gswidth_ext;
extern Real va_ext[];

extern  char * den_info_ext(int nsub, int n, int * mass, int * width);
extern void build_cb_ext(int nsub); 
extern void destroy_cb_ext(void);    
extern int cb_pow_ext;   
extern int cb_nc_ext; 
extern int * cb_chains_ext;
extern double * cb_coeff_ext;
#endif

#define  DENOMINATOR_ERROR   2
#endif
