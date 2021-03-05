#include <stdio.h>
#include <stdlib.h>
#define MEMSIZE 300

//typedef struct _head {
//	short size;
//	char n;
//}HEAD;

void** memP;
//void show_memory();
void memory_init(void* ptr, unsigned int size);
void* memory_alloc(unsigned int size);
int memory_free(void* valid_ptr);
int memory_check(void* ptr);
void clearArray(void* ptr, unsigned int size);
int checkPrevCell(void** p);
int checkNextCell(void** p);
void* findPrevCell(void** ptr);
void changePtrsAroundCell(void* ptr);

void test();
int main() {

	char  memory[MEMSIZE];
	memory_init(memory, MEMSIZE);
	void** a = NULL, *b = NULL, *c = NULL, *d = NULL, *e =  NULL, *f = NULL, *g = NULL, *h = NULL;


	memory_free(c);
	a = memP;
	b = memory_alloc(sizeof(char) * 60);
	c = memory_alloc(sizeof(int) * 4);
	d = memory_alloc(sizeof(int) * 7);
	e = memory_alloc(sizeof(int) * 4);
	f = memory_alloc(sizeof(int) * 5);
	g = memory_alloc(sizeof(char) * 21);
	h = memory_alloc(sizeof(char) * 35);


	memory_free(f);
	memory_free(e);
	memory_free(g);
	memory_free(b);
	
	f = memory_alloc(sizeof(int) * 5);
	
	e = memory_alloc(sizeof(char) * 43); //F-F?
	
	memory_free(d);

	d = memory_alloc(sizeof(char) * 20);

	
	


	//test();
	return 0;
}
//void show_memory() {
//	char *p = (char*)memP;
//	for (int i = 0; i < MEMSIZE; i++) {
//		if (*(p + i) == NULL) printf("NULL\n");
//		else printf("%c\n", *(p + i));
//	}
//}

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

			next = (char*)next + sizeof(unsigned int);
			*next = p;

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
	void** buff = *memP;
	void* prev = NULL;
	
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
	if ((*(unsigned int*)p & 1) == 1)
		return 0;
	else
		return 1;
}

int checkPrevCell(void** p) {
	p = (char*)p - sizeof(unsigned int);
	if (p < (memP + 1)) {
		printf("checkPrev overflow\n");
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
	void** buff, *prev, * next;
	unsigned int buffSize;

	if (size < 2 * sizeof(void*)) {
		size = 2 * sizeof(void*);
	}

	while (*p != NULL) {
		buffSize = **(unsigned int**)p;

		p = *p;

		if ((buffSize >> 1) >= size && (buffSize & 1) == 0) {

			if (((buffSize >> 1) - size -  sizeof(unsigned int)) <= (2 * sizeof(void*) + 2 * sizeof(unsigned int))) { //zmensena free bunka nie je dost velka

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
		
		p = (char*)p + sizeof(void*) + sizeof(unsigned int);

	}
	printf("nedostatok miesta!\n");
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

void test() {
	
	char pole[20];
	
	//void** p;
	void *ptr;
	ptr = pole ;
	*(unsigned int*)ptr = 50001;

	int a =  * (unsigned int*)ptr;
	a = (a << 1) | 1;

	*(unsigned int*)ptr = a;
	a = *(unsigned int*)ptr >> 1;
	
	printf("%d\n", a);
	////init
	//*(short*)ptr = 20;
	//ptr = (char*)ptr + 20 - sizeof(short);
	//*(short*)ptr = 20;

	////malloc
	//short a = 10;

	//ptr = pole;
	//*(short*)ptr = a;
	//ptr = (char*)ptr + sizeof(short);
	//*(short*)ptr = a + sizeof(short)*2 - 1;
	//ptr = (char*)ptr + (a + sizeof(short) ) ;
	//*(short*)ptr = a;

	//ptr = pole + sizeof(short);
	//short b = *(short*)ptr;
	//ptr = (char*)ptr + (a + sizeof(short)*2);
	//*(short*)ptr = b;

	//HEAD* ph;
//	p = pole;
//	*(unsigned short*)p = 99;
//	*(unsigned short*)p = *(unsigned short*)p & 61440;
//	
//	printf("%hu\n",  (1 << (sizeof(unsigned short)*7) + 1111));
//
///*
//	HEAD h;
//	h.size = 10;
//	h.n = 'a';
//*/
//	p = pole; //p ukazuje na zaciatok ptr
//	//*ph = h;//vytvori pointer na miesto kde ukazuje p odkazujuci na adresu pravej strany '='
//
//	//*ph->next = pole[10];
//
//	//*p = &pole[10];
//	//ptr = ptr + 5;
//	**(short**)p= 65000;
//	*p = &pole[2];
//	**(char**)p = 'c';
//	ptr = (void**)ptr + 1;
//	*p = &ptr;
//
//
//	*(char*)ptr = 'a';
//	ptr= (char*)ptr + 1;
//	*(char*)ptr= 'a';
//	ptr = (char*)ptr + 2;
//
//	*(char*)ptr = 'a';
//
//	//*p = (char)ptr + 8;
//	**(char**)p = 'b';
//
//	/*char* buff = malloc(sizeof(char) * 100);
//	p = pole;
//

	//pole[0] = 'A';
	//printf("%c %d\n", pole[0], pole[0]);
	//*(int*)p = 1000;
	//printf("%c %d\n", *(char*)p,pole[0]);
	//p = (int*)p + 2;
	//*(char*)p = 'a';
	//printf("%c %c\n",*(char*)p, pole[0] );

}