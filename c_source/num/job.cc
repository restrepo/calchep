/*
 Copyright (C) 2004, Alexander Pukhov, e-mail pukhov@theory.npi.msu.su 
*/

#include"subproc.h"
#include"runVegas.h"
#include"kininpt.h"
#include"alphas2.h"
#include"cut.h"
#include"regul.h"
#include"strfun.h"
#include"mc_menu.h"
#include"phys_val.h"
#include"chep_crt.h"
#include"files.h" 
#include"rw_sess.h"
#include"interface.h"
#include"rootDir.h"    
#include"num_out.h"
#include"histogram.h"                                                                    

int jobInit(CalcHEP_interface * interface)
{ int i;
  
  nin_int=0;
  link_process(interface);
  if(nin_int !=2) return 1;
  sprintf(pathtocomphep,"%s%c",rootDir,f_slash);
  sprintf(pathtohelp,"%shelp%c",pathtocomphep,f_slash);
  proces_1.nsub=1;
  wrtprc_();

  for(i=0;i<2;i++){  sf_num[i]=0; sf_be[i]=0;} 
  inP1=100; inP2=100;

  stdkin_();
  i_qcd();
  
  *rwidth_int=0;
  *gwidth_int=0;
  *twidth_int=0;

  clearGrid();
  cleartab(&cutTab);
  cleartab(&regTab);
  cleartab(&histTab);
  newSession();
  
  return 0;
}  

int jobInState(int nProc, double P1, double P2, char* strf1, char*strf2)
{
  int i,err=0,r=0;
  static  char sf_txt[2][200]={"OFF","OFF"};
  if(nProc <=0 && nProc >nprc_int) return -1; 
  if(nProc != proces_1.nsub){ r=r|2; proces_1.nsub=nProc; wrtprc_();} 
  if(inP1 != P1)       { r=1|1; inP1=P1;}
  if(inP2 != P2)       { r=1|1; inP2=P2;}
  if(strf1 && strcmp(strf1, sf_txt[0])) {r=r|2;strcpy(sf_txt[0],strf1);}
  if(strf2 && strcmp(strf2, sf_txt[1])) {r=r|2;strcpy(sf_txt[1],strf2);} 

  loadStrFun(sf_txt[0],sf_txt[1]);
  for(i=0;i<2;i++) 
  { if(strcmp(sf_txt[i],"OFF") && sf_num[i]==0) 
    { err=i+1; 
      printf("Wrong structure function '%s'\n",sf_txt[i]);
      strcpy(sf_txt[i],"OFF");
    }
  }
  clearGrid();
  newSession();           
}

int jobCut2( char * par,double min, double max)
{ char buff[200];
  char key;
  int err;

  if(!checkPhysVal(par, &key, buff)) return 1;
  if(min>=max) return 2;

  sprintf(buff,"[[[[[[{{%s\\09%E\\09%E}}",par,min,max);
  inkeyString=buff;
  err=monte_carlo_menu();
}

int jobCutMin( char * par,double min)
{ char buff[200];
  char key;
  int err;

  if(!checkPhysVal(par, &key, buff)) return 1;
  sprintf(buff,"[[[[[[{{%s\\09%E}}",par,min);
  inkeyString=buff;
  err=monte_carlo_menu();
}

int jobCutMax( char * par,double max)
{ char buff[200];
  char key;
  int err;

  if(!checkPhysVal(par, &key, buff)) return 1;
  sprintf(buff,"[[[[[[{{%s\\09\\09%E}}",par,max);
  inkeyString=buff;
  err=monte_carlo_menu();
}


void jobCutDel(void){cleartab(&cutTab); clearGrid(); newSession();}

int jobHist(double min, char * par, double max)
{ char buff[200];
  char key;
  int err;

  if(!checkPhysVal(par, &key, buff)) return 1;
  if(min>=max) return 2;
  sprintf(buff,"[[[[[[[[{[[{{%s\\09%E\\09%E}}}",par,min,max);
  inkeyString=buff;
  err=monte_carlo_menu();
}

void jobHistDel(void){ cleartab(&histTab);}

double jobVegas(int nSess,int nCalls,int clear,int*err_,double*dI,double*chi2)
{ char buff[200];
  int err;

  if(clear)sprintf(buff,"[[[[[[[[{{%d{[{%d{[[[[{]]{}}",nSess,nCalls);
  else     sprintf(buff,"[[[[[[[[{{%d{[{%d{[[{}}",nSess,nCalls);

  err=calcFunc_int();
  if(err>0) { if(err_) *err_=1; return -1;}
  if(err==0) newSession();
  inkeyString=buff;
  err=monte_carlo_menu();
  if(err){if(err_) *err_=1; return -1;}
  
  if(err_) *err_=0; 
  if(dI) *dI=integral.dI;
  if(chi2) *chi2=integral.khi2;
  return integral.I;
}
