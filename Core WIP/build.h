#ifndef _RDEBUG_
#define _RDEBUG_
#define _DEBPRINT(args) (printf("DEBUG: "), printf args)
#define TIMESTAMP "Built On " + __DATE__ + " At: " __TIME__
#endif 
