#include<math.h>
#define Real double 

#include"num_in.h"

double Helicity[2]={0.,0.};
double HelicityN[2]={0.,0.};
double Fmax=0; 
int  FError=0;
int  CalcConst=0;
int nMonoms=0;

int indx_(int k,int l)
{
  int i,j;
  if(k<l) {i=k;j=l;} else {i=l;j=k;}
  return i+(j*(j-1))/2;
}


#include "sqme_aux.inc"

#undef Real

#undef __NUM_IN_

#include "num_LD.h"
#include "num_in.h"
#include "sqme_aux.inc"
