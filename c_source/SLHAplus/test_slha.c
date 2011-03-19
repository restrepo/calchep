#include"SLHAplus.h"


int main(int nargs, char** vargs)
{ int nv,err;
  int  i, nChannel;

// Reading SLHA file 
  printf("Reading of spectrum file 'spectr.slha':\n");

#define readStream  
#ifdef readStream
{ FILE*f=fopen("spectr.slha","r"); 

  err=slhaReadStream(f,0,"</slha>");
  fclose(f);
}
#else

  err=slhaRead("spectr.slha",0);
#endif 
  
  printf("              error code is %d\n",err);
  if(err) return err;



// Checking warnings    
  printf("SLHA Warnings:\n");
  nv = slhaWarnings(NULL);  
  if(nv)
  { 
    printf("There are %d warnings\n",nv);
    slhaWarnings(stdout);
  } else  printf("    no  Warnings\n"); 


//Masses 
  printf("Particle masses:\n");
  printf("     lightest neutralino: %.3E\n", slhaVal("Mass",0.,1,1000022 ));
  printf("     first neutral Higgs     : %.3E\n", slhaVal("Mass",0.,1,25));
  printf("     second neutral Higgs    : %.3E\n", slhaVal("Mass",0.,1,35));      
   
//Mixing matrix 
 printf("\n Stau mixing matrix element Zl22: %.3E \n", slhaVal("STAuMIX",0., 2,2,2));

// Running strong coupling:
  printf("Running strong coupling  g3=sqrt(4*pi*alphaS):\n");
  if(slhaValExists("GaUge",1,3))
  { 
    printf("        at 100 GeV = %.3E\n",  slhaVal("GAUGE", 100.,1, 3));
    printf("        at 10^6 GeV = %.3E\n", slhaVal("GAUGE", 1E6,1, 3));
  } else printf("No  data\n");


// Reading of decays information;
  printf("Reading of  decay file 'decay.slha'\n"); 
  
  slhaRead("decay.slha",1);
   
  printf(" Decays of neutral Higgs\n");
  nChannel=slhaDecayExists(35);
  printf("   %d - channels of decay for second neutral Higgs\n",nChannel);
  if(nChannel>=0)
  {
    printf("  H width=%.3E\n",slhaWidth(35)); 
    printf("  Fist 5 decay channels:\n");
    for(i=1;i<=nChannel && i<5;i++)
    { int j;
      double Br;
      int prtcls[10];
      Br=slhaBranch(35,i,prtcls);
      printf(" 35 -> ");
      for(j=0;prtcls[j];j++) printf(" %d ", prtcls[j]);
       printf(" Branching = %.3E\n",Br); 
    }
  } else printf("  no decay information\n");   
  
  printf("Particle quantum numbers:\n");
  { int eQ3,spinDim,cDim,neutral;
    err=findQnumbers(144,&eQ3,&spinDim,&cDim,&neutral); 
    printf("eQ3=%d spinDim=%d  colorDim=%d  neutral=%d\n",eQ3,spinDim,cDim,neutral); 
  } 
// Writing summary SLHA file:   
  printf("Writing summary copy of input files 'copy.slha'\n");  
  slhaWrite("copy.slha");
    
  return 0;
}

