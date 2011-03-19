#ifndef   __DP_MONOMS__
#define  __DP_MONOMS__

extern int   NofDP;
extern int *DPmonomMap;
extern int  DPmonomSize;

extern void  initDPmonoms(void);
extern void  write_calcDPmonoms(FILE*f);
extern int   findMomonPosition(int * monom);
extern void  cleanDPmonoms(void);

#endif
