#ifndef __NUM_OUT_int
#define __NUM_OUT_int

#include<stdlib.h>
#include<string.h> 
#include<math.h>

extern  int hPrec_int;
extern  int nin_int;
extern  int nout_int;
extern  int nprc_int;
extern  int nvar_int;
extern  int nfunc_int;

extern char * (*pinf_int)(int nsub, int nprtcl, void * pmass, long*pnum);
extern char ** polarized_int;
extern char ** varName_int;

extern double (*sqme_int)(int nsub, void * momenta, int * err);
extern int (*calcFunc_int)(void);
extern int *twidth_int, *gswidth_int, *gtwidth_int;
extern double *BWrange_int;
extern void *va_int;
extern char*hiddenf;

extern void (*build_cb_int)(int nsub); 
extern void (*destroy_cb_int)(void);    
extern int *cb_pow_int;   
extern int *cb_nc_int; 
extern int ** cb_chains_int;
extern double ** cb_coeff_int;

extern double readVar(int k);
extern void writeVar(int k, double val);

extern void * saveVar(void);
extern void restoreVar(void* mem);
extern double doublePmass(int nsub,int nprctl);

#define  DENOMINATOR_ERROR   2
#endif

