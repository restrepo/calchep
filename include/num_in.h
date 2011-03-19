#ifndef __NUM_IN_
#define __NUM_IN_

#include<stdlib.h>
#include<string.h> 
#include<math.h>

extern double alpha_2(double);

extern  Real sqrMom(int, char*, Real*);
extern  Real DP[]; 
extern  Real *Q0,*Q1,*Q2;
extern  int    prepDen(int nden, double BWrange2,   
                       double s0max, Real * dmass,double * dwidth, Real *q);
extern  Real*DPmonom;
extern  void sprod_(int ntot, Real * momenta);


extern  int    CalcConst;
extern  double Fmax;
extern  int nMonoms;
extern  double HelicityN[2];
extern  int    indx_(int k,int l);
extern double Eps;



#endif
