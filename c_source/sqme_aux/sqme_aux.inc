
Real *Q0=NULL;
Real *Q1=NULL;
Real *Q2=NULL;

Real DP[28]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                   /* for 2->6 */

Real *DPmonom=NULL;


void sprod_(int ntot, Real * momenta)
{
   int k, i,j;
   for (i = 0; i < ntot-1; ++i)
   {
      for (j = i + 1; j < ntot; ++j)
      {  Real *sum=DP+indx_(i,j);
         Real *v1=momenta+4*(i);
         Real *v2=momenta+4*(j);
         *sum=*v1**v2;
         for(k=1;k<=3;k++) (*sum)-=v1[k]*v2[k];
      }
   }
}


Real sqrMom(int nin, char * momnum, Real * lv)
{  char * ii;
   Real s[4]={0,0,0,0};
   char nin_char;
   nin_char=nin;

   ii=momnum;
   while(*ii)
   {  int k;
      if(*ii>nin_char) for(k=0;k<4;k++) s[k]-=lv[k+4*(*ii-1)];
      else             for(k=0;k<4;k++) s[k]+=lv[k+4*(*ii-1)];
      ii++;
   }
   return (s[0]-s[3])*(s[0]+s[3])-s[1]*s[1]-s[2]*s[2];
}

int prepDen(int nden, double BWrange2, double s0max, Real * dmass,double * dwidth, Real *q)
{ static int ndenmem=0;
  static Real computer_eps=1;

  int i;
  int err=0;

  if(computer_eps>0.5)
  {  Real one=1,  one_plus_eps;
     do{ computer_eps=computer_eps/2; one_plus_eps=one+computer_eps;}
     while( one_plus_eps !=one); computer_eps*=2;
  }

  s0max=computer_eps*s0max*s0max;

  if(ndenmem<nden+1)
  {
     ndenmem=nden+1;
     Q0=(Real*)realloc(Q0,sizeof(Real)*ndenmem);
     Q1=(Real*)realloc(Q1,sizeof(Real)*ndenmem);
     Q2=(Real*)realloc(Q2,sizeof(Real)*ndenmem);
  }

  for(i=1;i<=nden;i++) Q1[i]=q[i];

  for(i=1;i<= nden;i++)
  if(dwidth[i])
  {  Real w,w2, q2=Q1[i]*Q1[i];
     w=dmass[i]*dwidth[i];
     w2=w*w;
     if(q2>BWrange2*w2) {if(q2<(BWrange2+1)*w2) q2=(BWrange2+1) *w2; w2=0; }
     Q2[i]=1/(q2+w2);
     Q0[i]=Q2[i]*Q1[i]*Q1[i];
     Q1[i]*=Q2[i];
  } else
  {  if((Q1[i]>0? Q1[i]:-Q1[i]) < 10*s0max) err=2;
     if((Q1[i]>0? Q1[i]:-Q1[i]) < s0max) Q1[i]=s0max;
/*     if(!Q1[i]) Q1[i]=s0max; */
     Q1[i]=1/Q1[i];
     Q2[i]=Q1[i]*Q1[i];
     Q0[i]=1;
  }
  return err;
}
