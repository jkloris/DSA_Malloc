#include <stdio.h>
#include <stdlib.h>
#define MEMSIZE 50

//typedef struct _head {
//	short size;
//	char n;
//}HEAD;

void** memP;
void show_memory();
void memory_init(void* ptr, unsigned int size);
void* memory_alloc(unsigned int size);

void test();
int main() {

	char  memory[MEMSIZE];
	//*p = &memory[10];
	memory_init(memory, MEMSIZE);

	memory_alloc(sizeof(int) * 4);
	void** p;
	p = memP;
	test();
	return 0;
}
//void show_memory() {
//	char *p = (char*)memP;
//	for (int i = 0; i < MEMSIZE; i++) {
//		if (*(p + i) == NULL) printf("NULL\n");
//		else printf("%c\n", *(p + i));
//	}
//}

void* memory_alloc(unsigned int size) {
	void** p = memP;
	unsigned int buffSize;
	while (1) {
		buffSize = **(unsigned int**)p;
		if ((buffSize >> 1) >= size && (buffSize & 1) == 0) {
			p = *p;
			*(unsigned int*)p = (size << 1) | 1;
			printf("jo");
		}
	}
}

void memory_init(void* ptr, unsigned int size) {

	void** p = ptr;
	*p = (void**)ptr + 1; // pointer to pointer ptr[sizeof(void*)]


	//TODO zmenit buffsize na realnu velkost
	p = p + 1;
	unsigned int BuffSize = ((size - 3*sizeof(void*) - 2*sizeof(unsigned int) ) << 1); //velkost s bitom na urcenie zaplnenia

	
	*(unsigned int*)p = BuffSize;

	p = (char*)ptr + sizeof(char) * (size - sizeof(unsigned int)); //poslednych sizeof(int) miest memory pola
	*(unsigned int*)p = BuffSize;
	
	
	//next a last ptr su zatial NULL
	p = (char*)ptr + sizeof(void*) + sizeof(unsigned int) ;
	*p = NULL; //next
	p = p + 1;
	*p = (void**)ptr; //last


	//
	/*p = ptr;
	BuffSize = **(unsigned int**)p;*/

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