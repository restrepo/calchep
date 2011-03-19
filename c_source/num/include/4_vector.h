#include<stdio.h>
#ifndef __4_VERTOR__
#define __4_VERTOR__

extern Real   pvect[400];

extern Real   vdot4(int i, int j);
extern void   vsum4(int i, int j, int k, int isg);
extern void   vnull4(int i);
extern void   eps4(int n1, int n2, int n3, int n4);
extern void   pvFill(Real mass, Real * mom, int pos); 
extern void   lvtonv(char * lv, int nin, int nvpos);

extern void incomkin(Real m1, Real m2, Real p1, Real p2,  
           Real *sqrt_S_,  Real *Pcm_, Real * rapidity_);
           
#endif
