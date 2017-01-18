#include <stdlib.h>
#include <stdio.h>

#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )
#define free( x ) myfree( x, __FILE__, __LINE__ )

/* Blocks are allocated as follows:
 * Data (X bytes)
 * Size (2 bytes, metadata)
 *		-Block allocation is determined by most significant bit in Size bytes
 */
unsigned char memory[5000]; //array used for memory allocation
unsigned int goodMallocs;
unsigned int nullMallocs;
unsigned int goodFrees;
unsigned int nullFrees;
unsigned int outsideFrees;
unsigned int redundantFrees;

/* Sets the size of a block, where index is the start of that block's metadata
 * Note: Always set the size before setting the allocation bit, as it is implicitly
 * set to unallocated
 */
void sizeSet(unsigned int index, unsigned int num){
	memory[index] = (num<<16)>>24;
	memory[index+1] = (num<<24)>>24;
}

/* Reads the size of a block, where index is the start of that block's metadata
 * Ignores the allocation bit when calculating block size
 */
unsigned int sizeRead(unsigned int index){
	unsigned int result = 0;
	result |= memory[index]<<8;
	result |= memory[index+1];
	//ignore bit used for allocation
	return (result<<17)>>17;
}

/* Replacement for malloc
 * Uses First Fit algorithm to find an unallocated block
 * Splits the found block in two: the amount requested is allocated, and the rest is left free
 */
void* mymalloc(size_t size, char* file, int linenum){
	//find first unallocated block with enough space
	unsigned int i = 0;
	while(i+2<5000){
		if((memory[i]>>7)==0 && sizeRead(i)>=size)
			break;
		else
			i+=2+sizeRead(i);
	}
	if(i+2>=5000){
		nullMallocs++;
		return NULL;
	}
	
	unsigned int startSize = sizeRead(i);
	sizeSet(i, size);
	//mark as allocated
	memory[i] |= (1<<7);
	//check if space remaining
	if((startSize-sizeRead(i))>0){
		//divide leftover space into new block
		sizeSet(i+2+size, (startSize-size)-2);
	}
	goodMallocs++;
	return &memory[i+2];
}

/* Unallocates blocks allocated by mymalloc()
 * Merges with adjacent free blocks if applicable
 * Zero's out the data within free'd blocks
 */
void myfree(void* p, char* file, int linenum){
	if(p==NULL){
		//fprintf(stderr, "MALLOC ERROR in %s:%d: Cannot free NULL pointer\n", file, linenum);
		nullFrees++;
		return;
	}
	int adr = -1;
	unsigned int i;
	//make sure p points to a real block
	for(i = 0; i<5000; i+=2+sizeRead(i)){
		if(p==&memory[i+2]){
			adr = i;
			break;
		}
	}
	if(adr==-1){
		//fprintf(stderr, "MALLOC ERROR in %s:%d: Pointer not malloc'd\n", file, linenum);
		outsideFrees++;
		return;
	}
	if(memory[adr]>>7==0){
		//fprintf(stderr, "MALLOC ERROR in %s:%d: Pointer already free\n", file, linenum);
		redundantFrees++;
		return;
	}
	//mark as unallocated
	memory[adr] = (memory[adr]<<25)>>25;
	//merge with adjacent free blocks
	//check if next block free
	if(sizeRead(adr)+2+adr<5000 && memory[adr+2+sizeRead(adr)]>>7==0){
		unsigned int nextBlock = adr+2+sizeRead(adr);
		sizeSet(adr, sizeRead(adr)+sizeRead(nextBlock)+2);
	}
	//check if prev block free
	int prev = -1;
	for(i = 0; i<adr; i+=(2+sizeRead(i))){
		if(i+2+sizeRead(i)==adr){
			prev=i;
			break;
		}
	}
	if(prev!=-1 && memory[prev]>>7==0){
		sizeSet(prev, sizeRead(prev)+sizeRead(adr)+2);
	}
	goodFrees++;
}