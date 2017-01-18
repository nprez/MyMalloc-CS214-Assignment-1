#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "mymalloc.h"

//malloc() 1 byte 3000 times, then free() the 3000 1 byte pointers one by one
void A(void* p, int i){
	if(i == 3000){
		return;
	}
	else{
		p = malloc(1);
		i++;
		void* q = NULL;
		A(q,i);
		free(p);
	}
}

//malloc() 1 byte and immediately free it 3000 times in a row
void B(void* p){
	int i;
	for(i = 0; i < 3000; i++){
		p = malloc(1);
		free(p);
	}
}

//randomly decides to either malloc arr[i] or free arr[i-1]
void CHelper(char* arr[], int m, int f, int i){
	if(f == 3000){
		return;
	}
	else if(m == 3000){
		f++;
		free(arr[i-1]);
		i--;
		CHelper(arr, m, f, i);
	}
	else{
		if(f<m){
			int r = rand();
			if(r%2==0){
				arr[i]=malloc(1);
				i++;
				m++;
				CHelper(arr, m, f, i);
			}
			else{
				f++;
				free(arr[i-1]);
				i--;
				CHelper(arr, m, f, i);
			}
		}
		else{
			arr[i]=malloc(1);
			i++;
			m++;
			CHelper(arr, m, f, i);
		}
	}
}

//randomly choose between a 1 byte malloc() or free() 6000 times
void C(){
	char* arr[3000];
	CHelper(arr, 0, 0, 0);
}

//randomly decides to either malloc arr[i] or free arr[i-1]
void DHelper(char* arr[], int m, int f, int i){
	if(f == 3000){
		return;
	}
	else if(m == 3000){
		f++;
		free(arr[i-1]);
		i--;
		DHelper(arr, m, f, i);
	}
	else{
		if(f<m){
			int r1 = rand();
			if(r1%2==0){
				int r2 = rand()%501;
				arr[i]=malloc(r2);
				i++;
				m++;
				DHelper(arr, m, f, i);
			}
			else{
				f++;
				free(arr[i-1]);
				i--;
				DHelper(arr, m, f, i);
			}
		}
		else{
			int r2 = rand()%501;
			arr[i]=malloc(r2);
			i++;
			m++;
			DHelper(arr, m, f, i);
		}
	}
}

//randomly choose between a randomly-sized malloc() or free() 6000 times
void D(){
	char* arr[3000];
	CHelper(arr, 0, 0, 0);
}

//Does i mallocs followed by i frees
void EHelper(void* p, int i, int j){
	if(j==i){
		return;
	}
	p = malloc(1);
	void* q = NULL;
	j++;
	EHelper(q, i, j);
	free(p);
}

//increasing equal numbers of mallocs and frees up to 128 of each
void E(void* p){
	int i;
	for(i = 1; i <= 128; i++){
		EHelper(p, i, 0);
	}
}

//Does i mallocs followed by i frees
void FHelper(void* p, int i, int j){
	if(j==i){
		return;
	}
	p = malloc(1);
	void* q = NULL;
	j++;
	FHelper(q, i, j);
	free(p);
}

//increasing powers of two groups of mallocs followed by frees up to 128 of each
void F(void* p){
	int i;
	for(i = 1; i <= pow(2, 7); i*=2){
		FHelper(p, i, 0);
	}
}

int main(){
	goodMallocs = 0;
	nullMallocs = 0;
	goodFrees = 0;
	nullFrees = 0;
	outsideFrees = 0;
	redundantFrees = 0;
	int i;
	void* p = NULL;

	unsigned int timeA = 0;
	unsigned int timeB = 0;
	unsigned int timeC = 0;
	unsigned int timeD = 0;
	unsigned int timeE = 0;
	unsigned int timeF = 0;
	unsigned int timeAll = 0;
	struct timeval start;
	struct timeval end;
	struct timeval fullStart;
	struct timeval fullEnd;

	//seed rand() based on milliseconds of system time
	struct timeval time;
	gettimeofday(&time, NULL);
	suseconds_t systime = time.tv_usec;
	srand(systime);

	for(i = 0; i < 100; i++){
		gettimeofday(&fullStart, NULL);

		//reset everything
		sizeSet(0, 4998);
		gettimeofday(&start, NULL);
		A(p, 1);
		gettimeofday(&end, NULL);
		timeA += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		
		//reset everything
		sizeSet(0, 4998);
		gettimeofday(&start, NULL);
		B(p);
		gettimeofday(&end, NULL);
		timeB += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		
		//reset everything
		sizeSet(0, 4998);
		gettimeofday(&start, NULL);
		C();
		gettimeofday(&end, NULL);
		timeC += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		
		//reset everything
		sizeSet(0, 4998);
		gettimeofday(&start, NULL);
		D();
		gettimeofday(&end, NULL);
		timeD += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;

		//reset everything
		sizeSet(0, 4998);
		gettimeofday(&start, NULL);
		E(p);
		gettimeofday(&end, NULL);
		timeE += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;

		//reset everything
		sizeSet(0, 4998);
		gettimeofday(&start, NULL);
		F(p);
		gettimeofday(&end, NULL);
		timeF += (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
		
		gettimeofday(&fullEnd, NULL);
		timeAll += (fullEnd.tv_sec - fullStart.tv_sec)*1000000 + fullEnd.tv_usec - fullStart.tv_usec;
	}

	timeA /= 100;
	timeB /= 100;
	timeC /= 100;
	timeD /= 100;
	timeE /= 100;
	timeF /= 100;
	timeAll /= 100;

	printf("Average time (in microseconds) for workload A: %d\n", timeA);
	printf("Average time (in microseconds) for workload B: %d\n", timeB);
	printf("Average time (in microseconds) for workload C: %d\n", timeC);
	printf("Average time (in microseconds) for workload D: %d\n", timeD);
	printf("Average time (in microseconds) for workload E: %d\n", timeE);
	printf("Average time (in microseconds) for workload F: %d\n", timeF);
	printf("Average time (in microseconds) for a full A-F workload: %d\n", timeAll);

	printf("%d Mallocs that successfully returned addresses\n", goodMallocs);
	printf("%d Mallocs that retuned NULL\n", nullMallocs);
	printf("%d Successful frees\n", goodFrees);
	printf("%d Attempts to free a NULL pointer\n", nullFrees);
	printf("%d Attempts to free at an address outside the given memory array\n", outsideFrees);
	printf("%d Attempts to free already unallocated blocks\n", redundantFrees);

	return 0;
}