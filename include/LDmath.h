#ifndef __CYGWIN__

#ifdef _Quad16_

#define sin    __sinq
#define cos    __cosq
#define tan    __tanq
#define asin   __asinq
#define acos   __acosq
#define atan   __atanq
#define atan2  __atan2q
#define exp    __expq
#define log    __logq
#define paw    __pawq
#define sqrt   __sqrtq
#define fabs   __fabsq

extern _Quad   __sinq(_Quad);
extern _Quad   __cosq(_Quad);
extern _Quad   __tanq(_Quad);
extern _Quad   __asinq(_Quad);
extern _Quad   __acosq(_Quad);
extern _Quad   __atanq(_Quad);
extern _Quad   __atan2q(_Quad,_Quad);
extern _Quad   __expq(_Quad);
extern _Quad   __logq(_Quad);
extern _Quad   __pawq(_Quad);
extern _Quad   __sqrtq(_Quad);
extern _Quad   __fabsq(_Quad);

#else

#define sin sinl
#define cos cosl
#define tan tanl
#define asin asinl
#define acos acosl
#define atan atanl
#define atan2 atan2l
#define exp expl
#define log logl
#define paw pawl
#define sqrt sqrtl
#define fabs fabsl

#endif

#endif
