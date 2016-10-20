#ifndef NEW_AND_DELETE_H
#define NEW_AND_DELETE_H

#include <stdlib.h>

void * operator new(size_t n)
{
	void * const p = malloc(n);
	// handle p == 0
	return p;
}

void operator delete(void * p) // or delete(void *, std::size_t)
{
	free(p);
}

#endif /* NEW_AND_DELETE_H */