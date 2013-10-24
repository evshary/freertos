#ifndef DBG_H
#define DBG_H

#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
#define DBGPRINTF(x)          printf("%s %d: ", __FILE__, __LINE__); printf(x)
#define DBGPRINTF1(x,a)       printf("%s %d: ", __FILE__, __LINE__); printf(x, a)
#define DBGPRINTF2(x,a,b)     printf("%s %d: ", __FILE__, __LINE__); printf(x, a, b)
#define DBGPRINTF3(x,a,b,c)   printf("%s %d: ", __FILE__, __LINE__); printf(x, a, b, c)
#else
#define DBGPRINTF(x)
#define DBGPRINTF1(x,a)
#define DBGPRINTF2(x,a,b)
#define DBGPRINTF3(x,a,b,c)
#endif

#endif
