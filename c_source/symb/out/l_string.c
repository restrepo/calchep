/*
 Copyright (C) 1997, Alexander Pukhov 
*/

#include <limits.h>
#include "syst.h"
#include "syst2.h"
#include "physics.h"
#include "procvar.h"
#include "optimise.h"
#include "out_serv.h" 
#include "l_string.h"
#include "writeF.h"
#include "dpMonoms.h"

#define buffsize MIN(1290,STRSIZ-3)

typedef struct longstr
   {
      int  len;
      char     txt[buffsize+1];
   }  longstr;

typedef longstr *longstrptr;



typedef struct degreerec
   {
      struct degreerec *    next;
      char         name[10];
      int         deg;
   }  degreerec;

static   degreerec  ** degnamesptr=NULL;

static int   degnamecount;
static int   tmpNameNum;
int   tmpNameMax=0;

void  initdegnames(void)
{int   k;
   degnamesptr=(degreerec**)m_alloc(sizeof(degreerec **) * (nProcessVar));
   for (k = 0; k < nProcessVar; k++) degnamesptr[k] = NULL;
   degnamecount=0;
}

int  cleardegnames(void)
{int  k;
 degreerec   * p, * q;
  if(!degnamesptr) return 0;
  for (k = 0; k < nProcessVar; k++)
  {
     q = degnamesptr[k];
     while (q != NULL)
     {
        p = q;
        q = p->next;
        free(p);
     }
  }
  free(degnamesptr);
  degnamesptr=NULL;
  return degnamecount;
}


static void  writelongstr(char* name,longstrptr longs)
{
   writeF("%s=",name);
   if (longs == NULL || longs->len == 0) writeF("0;\n");
   else writeF("%.*s;\n",longs->len,longs->txt);
}


static void   addstring(longstrptr longs,char* s)
{
 int i,l,ll;
 char name[STRSIZ];
   ll = longs->len;
   l  = strlen(s);
   if (ll + l > buffsize)
   {
      sprintf(name,"tmp[%d]",tmpNameNum++);
      writelongstr(name,longs);
      longs->len = 0;
      addstring(longs,name);
      ll = longs->len;
   }
   
   for (i = 0; i < l; i++) longs->txt[ll + i] = s[i];
   longs->len += l;
}


static void*  gorner(char* s,longstrptr pmult,longstrptr psum)
{
 char name[STRSIZ], name2[STRSIZ];
 longstrptr   ans;
 void*      pchange;

   if (pmult == NULL) return (void*)psum;
   ans = (longstrptr)m_alloc(sizeof(longstr));
   ans->len = 0;
   addstring(ans,s);

   if (3 + ans->len + pmult->len > buffsize)
   {
      sprintf(name,"tmp[%d]",tmpNameNum++);
      writelongstr(name,pmult);
      addstring(ans,"*"); addstring(ans,name);
   }
   else
   {
      if (pmult->txt[0] == '+')
      {
         pmult->txt[0] = '(';
         addstring(ans,"*");
      }
      else  addstring(ans,"*(");
     
      memcpy(&(ans->txt[ans->len]),pmult->txt,pmult->len);
      ans->len += pmult->len;
      addstring(ans,")");
   }
   free(pmult);

   if (psum == NULL) return (void*)ans;
   if (ans->len + psum->len > buffsize)
   {
      sprintf(name,"tmp[%d]",tmpNameNum++);
      if (ans->len > psum->len)
      {
         pchange = (void*)ans;
         ans = psum;
         psum = (longstrptr)pchange;
      }
      writelongstr(name,psum);
      if (ans->len + strlen(name) >= buffsize)
      {
         sprintf(name2,"tmp[%d]",tmpNameNum++);
         writelongstr(name2,ans);
         ans->len = 0;
         addstring(ans,"+");addstring(ans,name);
         addstring(ans,"+");addstring(ans,name2);
      }
      else { addstring(ans,"+"); addstring(ans,name);}
          
   }
   else
   {
      memcpy(&(ans->txt[ans->len]),psum->txt,psum->len);
      ans->len += psum->len;
   }
   free(psum);
   return (void*)ans;
}


static char *  writevardeg(int nv,int deg)
{
 degreerec *   p;
 static char namest[21];
 int deg0;
   if (deg == 1)  return   vararr[nv].alias;
   p = degnamesptr[nv];
   while (p != NULL)
      if (p->deg == deg)  return  p->name; 
      else                p = p->next;
   p = (degreerec *)m_alloc(sizeof(degreerec));
   p->deg = deg;
   sprintf(namest,"S[%d]",degnamecount++);
   strcpy(p->name,namest);
   p->next = degnamesptr[nv];
   degnamesptr[nv] = p;

   writeF("%s=%s*%s;\n",namest,vararr[nv].alias,vararr[nv].alias);
   for(deg0=2;2*deg0<=deg; deg0*=2)  writeF("%s*=%s;",namest,namest);
   if(deg0<deg) 
   {  writeF("%s*=%s",namest,vararr[nv].alias);
      for(deg0++;deg0<deg;deg0++)  writeF("*%s",vararr[nv].alias);
      writeF(";");
   } 
   writeF("\n");

   return namest;
}


static void*  smpl_emit(varptr ex)
{longstrptr   ans;
 char     s[STRSIZ];
 int      k, bt, nv, deg;
 int      star;
 varptr       ex_, exbeg;

   if (ex == NULL) return NULL;

   if (ex->sgn == '-')
   {
      ex_ = ex;

      while (ex_->next != NULL && ex_->next->sgn == '-')
      ex_ = ex_->next;

      if (ex_->next != NULL)
      {
         exbeg = ex_->next;
         ex_->next = exbeg->next;
         exbeg->next = ex;
         ex = exbeg;
      }
      
   }

   ans = (longstrptr)m_alloc(sizeof(longstr));
   ans->len = 0; 
   while (ex != NULL) 
   { 
      sprintf(s,"%c",ex->sgn);       
      star = (strcmp((ex->coef)->name,"1") != 0); 
      if (star || ex->vars[0] == 0) strcat(s,(ex->coef)->name); 
      if (ex->vars[0] != 0) 
      { 
         bt = (ex->vars[0]); 
         deg = 1; 
         for (k = 1; ex->vars[k]; k++) 
         { 
            nv = (ex->vars[k]); 
            if (bt != nv) 
            {
               if (star) strcat(s,"*"); else star = 1;     
               strcat(s,writevardeg(bt,deg)); 
               deg = 1; 
               bt = nv; 
            } 
            else ++(deg); 
         } 
         if (star) strcat(s,"*");  else  star = 1;      
         strcat(s,writevardeg(bt,deg)); 
      } 
      addstring(ans,s); 
      ex = ex->next; 
   } 
   return (void*) ans; 
}




static void*  v_gorner(int ch,int deg,void* pmult,void* psum)
{char b[STRSIZ];
   sprintf(b,"+%s",writevardeg(ch,deg));
   return gorner(b,pmult,psum);
}

static void*  c_gorner(infoptr i,void* pmult,void* psum)
{char b[STRSIZ];
   sprintf(b,"+%s",i->name);
   return gorner(b,pmult,psum);
}

void  fortwriter(char* name,varptr fortformula)
{longstrptr   tmp;
   tmpNameNum=0;
   tmp = (longstrptr)emitexpr(fortformula,smpl_emit,v_gorner,c_gorner);
   writelongstr(name,tmp);
   if (tmp != NULL) free(tmp);
   if (tmpNameMax<tmpNameNum) tmpNameMax=tmpNameNum;   
}

int  write_const(void)
{
   infoptr      i;
   int         firstVarTmp;
   int constcount=0;

    
   firstVarTmp = firstVar;
   firstVar   = 1;

   for(i=info; i; i=i->next) if (i->consttype != numb) 
   { constcount++; fortwriter(i->name,(void*) i->const_);}
   
   firstVar = firstVarTmp;

   return constcount;
}   /*  WriteConst  */

static int VarMonomNum( int *varstr)
{
  int i, monom[27];

  for(i=0;i<NofDP;i++)monom[i]=0;

  for(i=0;varstr[i];i++) 
  { int k;
    sscanf(vararr[varstr[i]].alias,"DP[%d",&k);
    monom[k]++;
  } 

  return findMomonPosition(monom);
}


void findPD(varptr sum)
{
  varptr s;
  int n;
  for(s=sum;s;s=s->next)
  { n= VarMonomNum(s->vars);
    if(n>=DPmonomSize) 
    { int i;
      DPmonomMap=realloc(DPmonomMap, sizeof(int)*(n+1));
      for(i=DPmonomSize;i<n;i++)DPmonomMap[i]=0;
      DPmonomSize=n+1;
    }
    DPmonomMap[n]=1;   
  }   
}


int* for_write_const(varptr sum, int * Ntt, int * nCoef)
{  int firstVarTmp = firstVar;
   int constcount=0;
   int TTcount=0;
   varptr s;
   int * posAr,*index,*inc,*reindex;
   int N,i;

   for(s=sum,N=0;s;s=s->next)N++;
   posAr=malloc(sizeof(int)*N);
   index=malloc(sizeof(int)*N);
   reindex=malloc(sizeof(int)*N);
   for(s=sum,i=0;s;s=s->next,i++) 
   { posAr[i]=DPmonomMap[VarMonomNum(s->vars)]; 
     index[i]=i; 
     if(constcount<posAr[i]) constcount=posAr[i];
   } 
   if(constcount>2*N) 
   {
     for(i=0;i<N-1;)
     {  if( posAr[index[i]] > posAr[index[i+1]]) 
        { int in=index[i];index[i]=index[i+1];index[i+1]=in; if(i>0)i--; else i++;}
        else i++;
     }
     for(i=0;i<N;i++) reindex[index[i]]=i;
      inc=malloc(sizeof(int)*N);
      *nCoef=N;  
   }else  
   { writeF("{int i; for(i=0;i<%d;i++) CC[i]=0;}\n",constcount+1);
     inc=NULL;
     *nCoef=constcount+1;
   }
   firstVar   = 1;
   for(i=0;sum; sum=sum->next,i++) 
   {  char name[10];

      if(constcount>2*N)
      { 
        sprintf(name,"CC[%d]",reindex[i]);  inc[reindex[i]]=posAr[i];
      } 
      else sprintf(name,"CC[%d]",posAr[i]);        

      if(sum->coef->nused >1)
      {  if(sum->coef->name[0]==0)
         { sprintf(sum->coef->name,"tt[%d]",TTcount++);             
           fortwriter(sum->coef->name,(void*) sum->coef->const_);          
         }
              if(sum->ncoef==1)  writeF("%s = %s;\n",name,sum->coef->name);
         else if(sum->ncoef==-1)writeF("%s = -%s;\n",name,sum->coef->name);
         else writeF("%s = %" NUM_STR "*%s;\n",name,sum->ncoef,sum->coef->name); 
      } else 
      {
         fortwriter(name,(void*) sum->coef->const_);
         if(sum->ncoef!=1) writeF("%s*=%" NUM_STR ";\n",name,sum->ncoef);
      }
   }
   *Ntt=TTcount;
   
   firstVar = firstVarTmp;
   free(index); free(posAr); free(reindex); 
   return inc;
}
