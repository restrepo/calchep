#ifndef __SLHA_PLUS_
#define __SLHA_PLUS_

#ifdef __cplusplus
extern "C" {
#endif 

#include<stdio.h>
#include<math.h>
#include<stdarg.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>

extern int FError;


extern int slhaBasicReader( int mode, int (*readLn)(int, char*) );

extern int slhaRead(char *fname,int mode);
int slhaReadStream(FILE*f, int mode, char * end );

extern double slhaVal(char * Block, double Q, int nKey, ...);
extern int slhaValExists(char * Block, int nKey, ...);
extern double slhaWidth(int pNum);
extern int slhaWrite(char *fname);
extern int slhaWarnings(FILE*f);
extern int slhaDecayExists(int pNum);
extern double slhaBranch(int pNum,int N, int * nCh);
extern int findQnumbers(int pdg, int *eQ3,int * spinDim,int*cDim,int *neutral);
extern int allQnumbers(int i, int *pdg,int*eQ3,int*spinDim,int*cDim,int*neutral);
extern int allBlocks(int i,int j,char*name,int*Len,int*key, double * val);
extern int allDecays(int i,int j,int* pdg, int*Len,int*decay,double*width,double*br);

extern int initDiagonal(void);
extern int rDiagonal(int nDim,...);
extern int rDiagonalA(int nDim,...);
extern double MassArray(int id,  int i);
extern double MixMatrix(int id, int i,int j);
extern double MixMatrixU(int id, int i,int j);


extern int System(char * format, ...);
extern int openAppend(char * fileName);
extern int aPrintF(char * format,...);

extern double initQCD(double MZalphaS,double McMc,double MbP,double MtP);
extern double alphaQCD(double Q);
extern double MbRun(double Q);
extern double MbEff(double Q);
extern double MtRun(double Q);
extern double MtEff(double Q);
extern double McRun(double Q);
extern double McEff(double Q);
extern double MbPole;

#ifdef __cplusplus
}
#endif 


#endif
