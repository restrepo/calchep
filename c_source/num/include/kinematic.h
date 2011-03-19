
#ifndef __KINEMATIC__
#define __KINEMATIC__

#include"LDtype.h"

extern double pvect[400];
extern int  mkmom(double *x, double *tfact);
extern int  imkmom(double P1,double P2);

extern Real pvectL[400];
extern int  mkmomL(double *x, double *tfact);
extern int  imkmomL(double P1,double P2);


#endif
