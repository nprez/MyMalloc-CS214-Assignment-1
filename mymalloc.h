#ifndef _mymalloc_H_
#define _mymalloc_H_

#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )
char memory[5000];
unsigned int goodMallocs;
unsigned int nullMallocs;
unsigned int goodFrees;
unsigned int nullFrees;
unsigned int outsideFrees;
unsigned int redundantFrees;
void sizeSet(unsigned int index, unsigned int num);
unsigned int sizeRead(unsigned int index);
void* mymalloc(size_t size, char* file, int linenum);
void myfree(void* p, char* file, int linenum);

#endif /* _mymalloc_H_ */
