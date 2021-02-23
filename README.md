Zistit si:
* ci pri akolokovanej pamati pri presahu pristupi do inej pamate - Pravdepodobne nie
* free(p+N) prejde a ak hej, co uvolni - NEDA SA

Otazky na cviko:
* ~~staci riesit alokovanie a spol?~~
* ~~pole ma byt char? ANO~~
* ~~ma to byt spravene sposobom ako v priklade? - pole v maine sa potom posiela do mem_init - jo~~
* ~~mame tam ukladat hodnoty? Ak ano, hocijakeho typu (int, char, float,..) - konvergovanie~~
* ~~ako to je s explicitnym a implicitnym riesenim - na plnku treba 3. riesenie~~
* ~~aka je minimalna a max velkost memory pola - nie je~~

Vyriesit:
* kontrola ze som za alokovanou pamatou - stasi pre hlavnu pamat
* kontrola ci okolo uvolnovaneho pola je volne miesto
* ako ulozit pointer na zadanej adrese - DONE
* ako riesit pole < 16

Mozno vyuzi:
*	//**(char**)p = 204;
*	//printf("%x\n", **(char**)p);

*	//void** p = ptr;
*	//*p = (void**)ptr + 1;