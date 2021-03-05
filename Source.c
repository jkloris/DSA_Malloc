#include <stdio.h>
#include <stdlib.h>
#define MEMSIZE 1000

//typedef struct _head {
//	short size;
//	char n;
//}HEAD;

void** memP;

void memory_init(void* ptr, unsigned int size);
void* memory_alloc(unsigned int size);
int memory_free(void* valid_ptr);
int memory_check(void* ptr);
void clearArray(void* ptr, unsigned int size);
int checkPrevCell(void** p);
int checkNextCell(void** p);
void* findPrevCell(void** ptr);
void changePtrsAroundCell(void* ptr);

void z1_testovac(char* region, char** pointer, int minBlock, int maxBlock, int minMemory, int maxMemory, int testFragDefrag);

int main() {

	char region[100000];
	char* pointer[13000];

	
	z1_testovac(region, pointer, 8, 24, 50, 100, 1);
	z1_testovac(region, pointer, 8, 24, 50, 100, 1);
	z1_testovac(region, pointer, 8, 24, 50, 100, 1);
	z1_testovac(region, pointer, 8, 24, 50, 100, 1);


	z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);

	z1_testovac(region, pointer, 8, 35000, 50000, 99000, 0);





	/*char  memory[MEMSIZE];
	memory_init(memory, MEMSIZE);
	void** a = NULL, *b = NULL, *c = NULL, *d = NULL, *e =  NULL, *f = NULL, *g = NULL, *h = NULL;
	a = memP;

	void* testArray[10];



	memory_free(f);*/
	
	return 0;
}


int memory_free(void* valid_ptr) {
	if (memory_check(valid_ptr)) {


		void** p = (char*)valid_ptr - sizeof(unsigned int); //ukazuje na Head
		unsigned int size = *(unsigned int*)p >> 1;

		
		clearArray(valid_ptr, size);
		short prevBool = checkPrevCell(p);
		short nextBool = checkNextCell(p);
		void** next;
		void** prev;
		//unsigned int buffSize;

		if (prevBool && nextBool) {

			
			next = (char*)p + size + 2 * sizeof(unsigned int) + sizeof(void*);
			prev = (char*)p - size -  sizeof(unsigned int) + sizeof(void*);
			*prev = (void*)next; //next predchadzajuceho pola sa rovna next nasledujuceho
			*next = NULL; //vynuluje next nasledujuceho pola
			next = (char*)next - sizeof(void*);
			*next = NULL; //vynuluje prev nasledujuceho pola

			//vynulovanie hlavicky next pola
			next = (char*)next - sizeof(unsigned int);
			unsigned int sizeNext = (unsigned int*)next;
			*(unsigned int*)next = NULL;
			next = (char*)next - sizeof(unsigned int);
			//unsigned int sizeNext = (unsigned int*)next;
			*(unsigned int*)next = NULL;

			*(unsigned int*)p = NULL;
			p = (char*)p - sizeof(unsigned int);
			unsigned int sizePrev = (unsigned int*)prev;
			*(unsigned int*)p = NULL;

			prev = (char*)prev - sizeof(void*) - sizeof(unsigned int);

			//celkova size, zaciatok
			*(unsigned int*)prev = (( size + (sizeNext>> 1) + (sizePrev >> 1) + 4 * sizeof(unsigned int) ) << 1);
			
			//celkova size, koniec
			next = (char*)next + sizeNext + 2 * sizeof(unsigned int);
			//*(unsigned int*)next = (( size + (sizeNext >> 1) + (sizePrev >> 1) + 4 * sizeof(unsigned int) ) << 1);
			*(unsigned int*)next = *(unsigned int*)prev;

			changePtrsAroundCell(valid_ptr);
		}
		else if (prevBool && !nextBool) {
			next = (char*)p + size + sizeof(unsigned int);

			*(unsigned int*)p = NULL;
			p = (char*)p - sizeof(unsigned int);

			prev = (char*)p - (*(unsigned int*)p >> 1) - sizeof(unsigned int);
			*(unsigned int*)prev = ((size + (*(unsigned int*)prev >> 1) + 2 * sizeof(unsigned int)) << 1);
			*(unsigned int*)next = *(unsigned int*)prev ;
			*(unsigned int*)p = NULL;

			//changePtrsAroundCell(valid_ptr);
		}
		else if (!prevBool && nextBool) {
			next = (char*)p + size + sizeof(unsigned int);
			*(unsigned int*)next = NULL;
			
			next = (char*)next + sizeof(unsigned int);
			unsigned int sizeNext = *(unsigned int*)next >> 1;
			*(unsigned int*)next = NULL;

			//velkost bunky begin
			*(unsigned int*)p = (size + sizeNext + 2 * sizeof(unsigned int)) << 1;


			//prehodenie pointrov z next na p
			
			p = (char*)p + sizeof(unsigned int);
			next = (char*)next + sizeof(unsigned int);
			*p = *next;
			*next = NULL;
			p = (char*)p + sizeof(void*);
			next = (char*)next + sizeof(void*);
			*p = *next;
			*next = NULL;


			//velkost bunky end
			next = (char*)next + sizeNext - sizeof(void*);
			*(unsigned int*)next = (size + sizeNext + 2 * sizeof(unsigned int)) << 1;

			changePtrsAroundCell(valid_ptr);
		}
		else if (!prevBool && !nextBool) {

			*(unsigned int*)p = size << 1; //oznacenie ze je free, begin
			prev = findPrevCell(&p); //p ukazuje na next free
			next = p;
			p = valid_ptr;
			
			if (prev == NULL) {
				p = (char*)p - sizeof(unsigned int);
				*memP = p;

				p = (char*)p + sizeof(unsigned int) + sizeof(void*);
				*p = next;
				
				p = (char*)valid_ptr - sizeof(unsigned int);
				
			}
			else {
				*p = prev;
				prev = (char*)prev + sizeof(unsigned int) + sizeof(void*);
				p++;
				*p = *prev; //priradenie next
				p = (char*)valid_ptr - sizeof(unsigned int);
				*prev = p;
				
			}

			if (next != NULL) {
				next = (char*)next + sizeof(unsigned int);
				*next = p;

			}

			p = (char*)valid_ptr + size;
			*(unsigned int*)p = size << 1; //oznacenie ze je free, end
			
			changePtrsAroundCell(valid_ptr);
		}
		
		
		return 0;
	}
	else return 1;
}

//ptr by mal ukazovat na zaciatok payloadu
void changePtrsAroundCell(void* ptr) {
	void** p, **buff;
	buff = (char*)ptr - sizeof(unsigned int);

	//prev
	p = ptr;
	if (*p != NULL) {
		p = *p;
		p = (char*)p + sizeof(void*) + sizeof(unsigned int);
		*p = (unsigned int*)buff;
	}
	else {
		*memP = buff;
	}

	//next
	p = (char*)ptr + sizeof(void*);
	if (*p != NULL) {

		p = *p;
		p = (char*)p  + sizeof(unsigned int);
		*p = (unsigned int*)buff;
	}


}

void* findPrevCell(void** ptr) {
	void** buff;
	void* prev = NULL;

	if(*memP != NULL){
		buff = *memP;
	}
	else {
		*ptr = NULL;
		return NULL;
	}
	
	while (buff < *ptr) {
		prev = buff;
		buff = (char*)buff + sizeof(void*) + sizeof(unsigned int);
		buff = *buff;
	}
	*ptr = buff;
	
	return prev;
}

int checkNextCell(void** p) {
	unsigned int size = *(unsigned int*)p >> 1;
	p = (char*)p + size + 2*sizeof(unsigned int);
	if ((*(unsigned int*)p & 1) == 1 || (*(unsigned int*)p == 0))
		return 0;
	else
		return 1;
}

int checkPrevCell(void** p) {
	p = (char*)p - sizeof(unsigned int);
	if (p < (memP + 1)) {
		//printf("checkPrev overflow\n");
		return 0;
	}
	if ((*(unsigned int*)p & 1) == 1)
		return 0;
	else 
		return 1;
}

void clearArray(void* ptr, unsigned int size) {

	int i;
	void **p = ptr;
	for (i = 0; i < size / sizeof(void*); i++) {
		*p++ = NULL;
	}
	for (i = 0; i < size % sizeof(void*); i++) {
		*(char*)p = NULL;
		p = (char*)p + sizeof(char);
	}
}

int memory_check(void* ptr) {
	if (ptr == NULL) {
		printf("neplatny ptr\n");
		return 0;
	}
	ptr = (char*)ptr - sizeof(unsigned int);
	if ((*(unsigned int*)ptr & 1) == 0) {
		printf("neplatny ptr\n");
		return 0;
	}
	return 1;
}



void* memory_alloc(unsigned int size) {
	void** help = memP; //tmp
	void** p = memP;
	void** buff = NULL, *prev, * next;
	unsigned int buffSize, bestFitSize = 0;

	if (size < 2 * sizeof(void*)) {
		size = 2 * sizeof(void*);
	}

	//best fit
	while (*p != NULL ) {
		p = *p;
		buffSize = *(unsigned int*)p;
		if (buffSize == size) {
			break;
		}

		if (buffSize > size && (bestFitSize == 0 || buffSize - size < bestFitSize)) {
			bestFitSize = buffSize - size;
			buff = p;
		}
		p = (char*)p + sizeof(void*) + sizeof(unsigned int);

		//p = *p;
	}
	if (buff == NULL)
		return NULL;
	p = buff;

	buffSize = *(unsigned int*)p;
	//while (*p != NULL) {


		if ((buffSize >> 1) >= size && (buffSize & 1) == 0) {


			if ( (int)((int)(buffSize >> 1) - (int)size -  (int)sizeof(unsigned int)) <= (int)(2 * sizeof(void*) + 2 * sizeof(unsigned int))) { //zmensena free bunka nie je dost velka

				size = buffSize >> 1;
			}

			*(unsigned int*)p = (size << 1) | 1; //size begin
			p = (char*)p + sizeof(unsigned int); //skok na zaciatok payloadu
			buff = p;
			prev = *p; //ukazuje na prev pointer -> buduci zaciatok payloadu
			*p = NULL; //odstranenie pointrov
			next = *(++p); 
			*(p--) = NULL;
			p = (char*)p + sizeof(char) * size; //skok na koniec payloadu
			*(unsigned int*)p = (size << 1) | 1; //size end
			

			
			p = (char*)p + sizeof(unsigned int);
			if ((buffSize >> 1) != size) {

				*(unsigned int*)p = ((buffSize >> 1) - size - 2 * sizeof(unsigned int) << 1); //velkost novej free bunky
				p = (char*)p + ((buffSize >> 1) - size - 1 * sizeof(unsigned int) );
				*(unsigned int*)p = ((buffSize >> 1) - size - 2 * sizeof(unsigned int) << 1); //velkost novej free bunky
				p = (char*)p - ((buffSize >> 1) - size - 1 * sizeof(unsigned int));
				
				
				if (prev == NULL) { //prev == NULL
					*memP = p; //. zaciatok ukazuje na nove pole
					p = (char*)p + sizeof(unsigned int);
					*p = NULL; //nove prev
				}
				else {
					buff = p;
					p = (char*)p + sizeof(unsigned int); //ukazuje na prev novej free bunky
					*p = prev; 
					p = *p;
					p = (char*)p + sizeof(unsigned int) + sizeof(void*);

					*p = buff;
					p = *p;
					p = (char*)p + sizeof(unsigned int); //ukazuje na prev novej free bunky
					//

				}
				p = p + 1; //mal by ukazovat na next novej free bunky

				if (next == NULL) {
					*p = NULL;
				}
				else {

					buff = (char*)p - sizeof(void*) - sizeof(unsigned int);
					*p = next;
					p = *p;
					p = (char*)p + sizeof(unsigned int); // ukazuje na prev NewFree

					*p = buff; //na adresu kde ukazuje p sa ulozila adresa kde ukazuje buff

					
				}
				
			}
			else {
				if (prev == NULL && next == NULL) {
					*memP = NULL;
				}
				else if (prev == NULL && next != NULL){
					*memP = next;
					p = (char*)next + sizeof(unsigned int); //?
					*p = NULL;
				}
				else if (prev != NULL && next == NULL) {
					p = (char*)prev + sizeof(unsigned int) + sizeof(void*);
					*p = NULL;
				}
				else {
					p = (char*)prev + sizeof(unsigned int) + sizeof(void*);
					*p = next;
					p = (char*)next + sizeof(unsigned int);
					*p = prev;
				}
			}
			
			return buff = (char*)buff;
						
			
		}
		
		

//	}
	//printf("nedostatok miesta!\n");
	return NULL; //TODO kukni co ma vratit
}

void memory_init(void* ptr, unsigned int size) {

	void** p = ptr;
	*p = (void**)ptr + 1; // pointer to pointer ptr[sizeof(void*)]


	//TODO zmenit buffsize na realnu velkost - asi done? 
	p = p + 1;
	unsigned int BuffSize = ((size - sizeof(void*) - 2*sizeof(unsigned int) ) << 1); //velkost s bitom na urcenie zaplnenia

	
	*(unsigned int*)p = BuffSize;

	p = (char*)ptr + sizeof(char) * (size - sizeof(unsigned int)); //poslednych sizeof(int) miest memory pola
	*(unsigned int*)p = BuffSize;
	
	
	//next a last ptr su zatial NULL
	p = (char*)ptr + sizeof(void*) + sizeof(unsigned int) ;
	*p = NULL; //prev
	p = p + 1;
	*p = NULL; //next


	//
	p = ptr;
	BuffSize = **(unsigned int**)p;

	memP = ptr;

}


//potom odstranit
void z1_testovac(char* region, char** pointer, int minBlock, int maxBlock, int minMemory, int maxMemory, int testFragDefrag) {
	unsigned int allocated = 0;
	unsigned int mallocated = 0;
	unsigned int allocated_count = 0;
	unsigned int mallocated_count = 0;
	unsigned int i = 0;
	int random_memory = 0;
	int random = 0;
	memset(region, 0, 100000);
	random_memory = (rand() % (maxMemory - minMemory + 1)) + minMemory;
	memory_init(region, random_memory);
	if (testFragDefrag) {
		do {
			pointer[i] = memory_alloc(8);
			if (pointer[i])
				i++;
		} while (pointer[i]);
		for (int j = 0; j < i; j++) {
			if (memory_check(pointer[j])) {
				memory_free(pointer[j]);
			}
			else {
				printf("Error: Wrong memory check.\n");
			}
		}
	}
	i = 0;
	while (allocated <= random_memory - minBlock) {
		random = (rand() % (maxBlock - minBlock + 1)) + minBlock;
		if (allocated + random > random_memory)
			continue;
		allocated += random;
		allocated_count++;
		pointer[i] = memory_alloc(random);
		if (pointer[i]) {
			i++;
			mallocated_count++;
			mallocated += random;
		}
	}

	for (int j = 0; j < i; j++) {
		if (memory_check(pointer[j])) {
			memory_free(pointer[j]);
		}
		else {
			printf("Error: Wrong memory check.\n");
		}
	}
	memset(region, 0, random_memory);
	for (int j = 0; j < 100000; j++) {
		if (region[j] != 0) {
			region[j] = 0;
			printf("Error: Modified memory outside the managed region. index: %d\n", j );
		}
	}
	float result = ((float)mallocated_count / allocated_count) * 100;
	float result_bytes = ((float)mallocated / allocated) * 100;
	printf("Memory size of %d bytes: allocated %.2f%% blocks (%.2f%% bytes).\n", random_memory, result, result_bytes);
}
