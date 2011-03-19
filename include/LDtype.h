#ifndef __LDTYPE__
#define __LDTYPE__

#ifndef __CYGWIN__
#ifdef _QUAD16_

#define Real _Quad

#else

#define Real long double

#endif

#else 
  #define Real double
#endif  
  
#endif
