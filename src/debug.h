#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#if DF_DEBUG
	#define DF_DBG(__info__,...) { fprintf(stderr, "[DF-DBG]: <%s> %d: " __info__, __FUNCTION__, __LINE__, ##__VA_ARGS__); fprintf(stderr, "\n"); }
#else
	#define DF_DBG(__info__,...)
#endif

#define DF_ERR(__info__,...) { fprintf(stderr, "[DF-ERR]: <%s> %d: " __info__, __FUNCTION__, __LINE__, ##__VA_ARGS__); fprintf(stderr, "\n"); }

#endif /* DEBUG_H */
