#include <stdio.h>

void** memP; //ukazuje na zaciatok pamate region

void memory_init(void* ptr, unsigned int size);
void* memory_alloc(unsigned int size);
int memory_free(void* valid_ptr);
int memory_check(void* ptr);
void clearArray(void* ptr, unsigned int size);
int checkPrevCell(void** p);
int checkNextCell(void** p);
void* findPrevCell(void** ptr);
void changePtrsAroundCell(void* ptr);

void test1(char* region, char** pointer, unsigned int initSize, unsigned int blockSize);
void test2(char* region, char** pointer, unsigned int initSize, unsigned int minBlockSize, unsigned int maxBlockSize);


int main() {

	char region[100000];
	char* pointer[10000];


	int i,j;

	for (j = 50; j <= 200; j *= 2) {

		for (i = 8; i <= 24; i++) {
			test1(region, pointer, j, i);
		}
		test2(region, pointer, j, 8, 24);
	}

	test2(region, pointer, 10000, 500, 5000);
	test2(region, pointer, 10000, 500, 5000);
	test2(region, pointer, 10000, 500, 5000);
	test2(region, pointer, 10000, 500, 5000);
	test2(region, pointer, 10000, 500, 5000);

	test2(region, pointer, 100000, 8, 50000);
	test2(region, pointer, 100000, 8, 50000);
	test2(region, pointer, 100000, 8, 50000);
	test2(region, pointer, 100000, 8, 50000);
	test2(region, pointer, 100000, 8, 50000);
	test2(region, pointer, 100000, 8, 50000);
	
	return 0;
}




void test2(char* region, char** pointer, unsigned int initSize, unsigned int minBlockSize, unsigned int maxBlockSize) {
	unsigned int allocAll = 0, allocSucces = 0, blocksAll = 0;
	int i=0, e, randSize, randIndex, count = 0; 
	char* tempP;
	
	memory_init(region, initSize);
	//alokuje bloky s nahodnymi velkostami
	while (initSize > allocAll) {
		randSize = rand() % (maxBlockSize - minBlockSize) + minBlockSize;
		if ((pointer[i++] = memory_alloc(randSize)) != NULL) {
			allocSucces += randSize;
			count++;
		}
		allocAll += randSize;
		blocksAll++;
	}

	//uvolni nahodny bloky
	for (e = 0; e < count ; e++) {
		randIndex = (rand() % (count - e));
		if (memory_free(pointer[randIndex]) == 1) { // vo funkcii memory_free je memory check, ktory skontroluje ci je pointer validny
			printf("ERROR[memory_free], non-valid pointer\n");
		}

		tempP = pointer[count - e - 1];
		pointer[count - e - 1] = pointer[randIndex];
		pointer[randIndex] = tempP;

	}

	memset(region, 0, initSize); 
	
	printf("memory size: %u bytov, alokovanych: %.2f%% bytov, %.2f%% blokov\n", initSize, (float)allocSucces / allocAll * 100, (float)count / blocksAll * 100);
}


void test1(char* region, char** pointer, unsigned int initSize, unsigned int blockSize) {
	unsigned int allocAll = 0, allocSucces = 0, blocksAll = 0, freeCounter = 0;
	int i, e, randIndex, count = 0; ;
	char* tempP;
	void** buff;
	memory_init(region, initSize);

	//alokuje bloky so zadanou velkostou
	for (i = 0; i < (initSize / blockSize ); i++) {
		if ( (pointer[i] = memory_alloc(blockSize))!= NULL ) {
			allocSucces += blockSize;
			count++;
		}
		blocksAll++;
		allocAll += blockSize;
	}

	//uvolni nahodne bloky
	for (e = 0; e < count ; e++) {
		randIndex = (rand() % (count - e ));
		if (memory_free(pointer[randIndex]) == 1 ) { // vo funkcii memory_free je memory check, ktory skontroluje ci je pointer validny
			printf("ERROR[memory_free], non-valid pointer\n");
		}
		
		tempP = pointer[count - e - 1];
		pointer[count - e - 1] = pointer[randIndex];
		pointer[randIndex] = tempP;

	}

	
	memset(region, 0, initSize);
	printf("memory size: %u bytov, alokovanych: %.2f%% bytov, %.2f%% blokov \n",initSize, (float)allocSucces / allocAll * 100, (float)count / blocksAll * 100);
}


int memory_free(void* valid_ptr) {
	if (memory_check(valid_ptr)) {//kontrola ci je pointer validny

		void** p = (char*)valid_ptr - sizeof(unsigned int); //ukazuje na Head
		unsigned int size = *(unsigned int*)p >> 1;

		clearArray(valid_ptr, size);
		//skontroluje ci je pred a za bunkou ina volna bunka
		short prevBool = checkPrevCell(p); 
		short nextBool = checkNextCell(p);
		void** next;
		void** prev;
		//unsigned int buffSize;

		if (prevBool && nextBool) {

			p = (char*)p - sizeof(unsigned int);
			unsigned int sizePrev = *(unsigned int*)p >> 1;
			p = (char*)p + sizeof(unsigned int);
				
			next = (char*)p + size + 3 * sizeof(unsigned int) + sizeof(void*);
			prev = (char*)p - sizePrev -  sizeof(unsigned int) + sizeof(void*);
			*prev = *next; //next predchadzajuceho pola sa rovna next nasledujuceho
			*next = NULL; //vynuluje next nasledujuceho pola
			next = (char*)next - sizeof(void*);
			*next = NULL; //vynuluje prev nasledujuceho pola

			//vynulovanie hlavicky next pola
			next = (char*)next - sizeof(unsigned int);
			unsigned int sizeNext = *(unsigned int*)next;
			sizeNext >>= 1;
			*(unsigned int*)next = NULL;
			next = (char*)next - sizeof(unsigned int);
			*(unsigned int*)next = NULL;

			*(unsigned int*)p = NULL;
			p = (char*)p - sizeof(unsigned int);
			*(unsigned int*)p = NULL;

			prev = (char*)prev - sizeof(void*) - sizeof(unsigned int);

			//celkova size, zaciatok
			*(unsigned int*)prev = (( size + (sizeNext) + (sizePrev) + 4 * sizeof(unsigned int) ) << 1);
			
			//celkova size, koniec
			next = (char*)next + sizeNext + 2 * sizeof(unsigned int);
			*(unsigned int*)next = *(unsigned int*)prev;

			prev = (char*)prev + sizeof(unsigned int);
			changePtrsAroundCell(prev);
		}
		else if (prevBool && !nextBool) {
			next = (char*)p + size + sizeof(unsigned int);

			*(unsigned int*)p = NULL;
			p = (char*)p - sizeof(unsigned int);

			prev = (char*)p - (*(unsigned int*)p >> 1) - sizeof(unsigned int);
			*(unsigned int*)prev = ((size + (*(unsigned int*)prev >> 1) + 2 * sizeof(unsigned int)) << 1);
			*(unsigned int*)next = *(unsigned int*)prev ;
			*(unsigned int*)p = NULL;

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
//zmeni pointre v okolitych volnych blokoch
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

//najde predchadzajucu volnu bunku
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
		if (*buff != NULL)
			buff = *buff;
		else {
			*ptr = NULL;
			return prev;
		}
			
	}
	*ptr = buff;
	
	return prev;
}

int checkNextCell(void** p) {
	unsigned int size = *(unsigned int*)p >> 1;
	p = (char*)p + size + 2*sizeof(unsigned int);
	if ((*(unsigned int*)p & 1) == 1 || (*(unsigned int*)p == 0) || (*(unsigned int*)p == NULL))
		return 0;
	else
		return 1;
}

int checkPrevCell(void** p) {
	p = (char*)p - sizeof(unsigned int);
	if (p < (memP + 1)) {
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
	void** p = memP;
	void** buff = NULL, *prev, * next, **buff2= NULL;
	unsigned int buffSize, bestFitSize = 0;

	if (size < 2 * sizeof(void*)) {
		size = 2 * sizeof(void*);
	}

	//best fit, najde najvhodnejsi volny block
	while (*p != NULL) {
		p = *p;
		buffSize = *(unsigned int*)p >> 1;
		if (buffSize == size) {
			break;
		}

		if (buffSize > size && (bestFitSize == 0 || buffSize - size < bestFitSize)) {
			bestFitSize = buffSize - size;
			buff = p;
		}
		p = (char*)p + sizeof(void*) + sizeof(unsigned int);
	}

	if (buff == NULL)
		return NULL;
	p = buff;

	buffSize = *(unsigned int*)p;



	if ((buffSize >> 1) >= size && ((buffSize & 1) == 0) && (buffSize != 0) ) {

		//ak nova bunka, ktora by vznikla nie je dost velka, pozadovana velkost sa zaokruhli
		if ( (int)((int)(buffSize >> 1) - (int)size -  (int)sizeof(unsigned int)) <= (int)(2 * sizeof(void*) + 2 * sizeof(unsigned int))) { 

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
		
		//ak vznikla nova volna bunka
		if ((buffSize >> 1) != size) {

			*(unsigned int*)p = ((buffSize >> 1) - size - 2 * sizeof(unsigned int) << 1); //velkost novej free bunky
			p = (char*)p + ((buffSize >> 1) - size - 1 * sizeof(unsigned int) );
			*(unsigned int*)p = ((buffSize >> 1) - size - 2 * sizeof(unsigned int) << 1); //velkost novej free bunky
			p = (char*)p - ((buffSize >> 1) - size - 1 * sizeof(unsigned int));
			
			//priradovanie pointrov novej volnej bunke
			if (prev == NULL) { 
				*memP = p; //. zaciatok ukazuje na nove pole
				p = (char*)p + sizeof(unsigned int);
				*p = NULL; //nove prev
			}
			else {
				buff2 = p;
				p = (char*)p + sizeof(unsigned int); //ukazuje na prev novej free bunky
				*p = prev; 
				p = *p;
				p = (char*)p + sizeof(unsigned int) + sizeof(void*);

				*p = buff2;
				p = *p;
				p = (char*)p + sizeof(unsigned int); //ukazuje na prev novej free bunky
				//

			}
			p = p + 1; //mal by ukazovat na next novej free bunky

			if (next == NULL) {
				*p = NULL;
			}
			else {

				buff2 = (char*)p - sizeof(void*) - sizeof(unsigned int);
				*p = next;
				p = *p;
				p = (char*)p + sizeof(unsigned int); // ukazuje na prev NewFree

				*p = buff2; //na adresu kde ukazuje p sa ulozila adresa kde ukazuje buff

			}
			
		}
		else {
			if (prev == NULL && next == NULL) {
				*memP = NULL;
			}
			else if (prev == NULL && next != NULL){
				*memP = next;
				p = (char*)next + sizeof(unsigned int); 
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
		
	
	return NULL; 
}

void memory_init(void* ptr, unsigned int size) {

	void** p = ptr;
	*p = (void**)ptr + 1; // pointer to pointer ptr[sizeof(void*)]

	p = p + 1;
	unsigned int BuffSize = ((size - sizeof(void*) - 3*sizeof(unsigned int) ) << 1); //velkost s bitom na urcenie zaplnenia

	//tvorba hlavicky a paty celej pamate
	*(unsigned int*)p = BuffSize;
	p = (char*)ptr + size - 2*sizeof(unsigned int); //poslednych sizeof(int) miest memory pola
	*(unsigned int*)p = BuffSize;
	p = (char*)p + sizeof(unsigned int);
	*(unsigned int*)p = NULL;
	
	
	//next a last ptr su zatial NULL
	p = (char*)ptr + sizeof(void*) + sizeof(unsigned int) ;
	*p = NULL; //prev
	p = p + 1;
	*p = NULL; //next

	memP = ptr;
}

