#include"SLHAplus.h"

int main(void)
{ int i,j,k,l;
  double a11,a12,a21,a22;
  int pos1,pos2;
  a11=0.1; a12=3.0; a21=3.0,a22=2; 
//  a11=1026081.918241,a12=-175950.000000, a22=117642.874690;
printf("=========================\ndiagonazising of symmetry matrix:\n");
printf("%E  %E\n%E  %E\n",a11,a12,a12,a22); 

  pos1=rDiagonal(2,a11,a12,a22);
  
  printf("masses : %E %E\n",   MassArray(pos1,1), MassArray(pos1,2) );
  printf("rotation :\n%E  %E\n%E  %E\n", MixMatrix(pos1,1,1), MixMatrix(pos1,1,2),
                                         MixMatrix(pos1,2,1), MixMatrix(pos1,2,2) );
printf("restoring of original matrix:\n"); 
printf("sqrt(mass^2)=%e %e\n", sqrt(MassArray(pos1,1)),
sqrt(MassArray(pos1,2)));
 
  for(j=1;j<=2;j++) for(i=1;i<=2;i++) 
  { double s=0;
     for(k=1;k<=2;k++) s+=MixMatrix(pos1,k,i)*MassArray(pos1,k)*MixMatrix(pos1,k,j);
     printf("%E",s); if(i==1)printf("  "); else printf("\n");
  }
printf("=========================\ndiagonazising of asymmetry matrix:\n");  

a11=1; a12=0.01; a21=0.05,a22=-1;
printf("%E  %E\n%E  %E\n",a11,a12,a21,a22); 

  pos2=rDiagonalA(2,a11,a12,a21,a22);

printf("masses : %E %E\n",   MassArray(pos2,1), MassArray(pos2,2) );
  
  printf("U rotation :\n%E  %E\n%E  %E\n", MixMatrixU(pos2,1,1), MixMatrixU(pos2,1,2),
                                         MixMatrixU(pos2,2,1), MixMatrixU(pos2,2,2) );
  
  printf("V rotation :\n%E  %E\n%E  %E\n", MixMatrix(pos2,1,1), MixMatrix(pos2,1,2),
                                           MixMatrix(pos2,2,1), MixMatrix(pos2,2,2) );

printf("restoring of original matrix:\n");  
  for(i=1;i<=2;i++) for(j=1;j<=2;j++) 
  { double s=0;
     for(k=1;k<=2;k++) s+=MixMatrixU(pos2,k,i)*MassArray(pos2,k)*MixMatrix(pos2,k,j);
     printf("%E",s); if(j==1)printf("  "); else printf("\n");
  }

}
