/*
* IZP projekt 1 - Prace s textem
* Patrik Sztefek
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

int strLenght(char *str);
int palindrom(char *str);
int isNum(char *str);
int testInt(char *str);
int isPrime(int num);
int isDate(char *str, char *_day);
int inputTest(char *str);

int main(int argc, char *argv[]){
	(void)argc;
	(void)argv;

	char str[101]; // Vstupni string
	int tmpInt;
	char tmpStr[101];

	while (scanf("%100s", str) != EOF){

		// Test vstupu
		if (inputTest(str)){
			printf("Input error.");
			return 1;
		}

		if (isNum(str) == 0 && (tmpInt=testInt(str)) != -1 ){ // Pokud se jedna o cislo a pokud se povede prevod na int
			if (isPrime(tmpInt))
				printf("number: %d\n",tmpInt);
			else
				printf("number: %d (prime)\n", tmpInt);
		}
		else if (isDate(str, tmpStr) == 0){ // Pokud je retezec ve formatu data
			printf("date: %s %s\n", tmpStr, str);
		}
		else{ // Pokud str neni ani datum ani kladne cislo v rozmezi INT_MAX, jedna se o slovo
			if (palindrom(str))
				printf("word: %s\n", str);
			else
				printf("word: %s (palindrome)\n", str);
		}
	}

	return 0;
}

int strLenght(char *str){
	int size = 0;
	while (1){
		if (str[size] == '\0')
			return size;
		size++;
	}
}

/*
	Funkce pro zjisteni palindromu.
	Vstup: string, nactene slovo
	Vystup:
		1 = Neni palindrom
		0 = Je palindrom
*/
int palindrom(char *str){
	int size = strLenght(str);

	if (size == 1) return 0;

	for (int i = 0; i < size / 2; i++){
		if (str[i] != str[size - 1 - i]) //Pakud se znak od sebe lisi
			return 1;
	}
	return 0;
}

/*
	Funkce pro zjisteni zda zadany str je cele kladne cislo resp. zda obsahuje pouze cislice.
	Vstup: string, nactene slovo
	Vystup:
		1: Nejedna se o cislo
		0: Retezec je cislo
*/
int isNum(char *str){
	int size = strLenght(str);

	for (int i = 0; i < size; i++){
		if (str[i]<'0' || str[i]>'9') // Pokud znak nepatri do rozsahu 0-9
			return 1;
	}
	return 0;
}

/*
	Funkce pro zjisteni zda retezec, resp. cislo je v povolem rozsahu INT_MAX.
	Vstup: string, nactene slovo, predem otestovano ve fci isNum
	Vystup:
		-1: Chyba prevodu/ cislo je mimo rozsah
		>= 0: Prevedene cislo
*/
int testInt(char *str){
	int foo;

	foo = (int)strtol(str, (char **)NULL, 10);
	if (foo == INT_MAX && errno == ERANGE) // Test prevodu
		return -1; // Error, neprevede/mimo rozsah
	else
		return foo; // Uspesny prevod, vraci prevednou hodnotu v int
}

/*
	Funkce pro zjisteni zda je zadane cislo prvocislo.
	Vstup: Jakekoli cislo (kladne int - testovani pred volanim)
	Vystup:
		1: Nejedna se o prvocislo
		0: Cislo je prvocislo
*/
int isPrime(int num){
	int foo;

	if (num <= 1) return 1;  // Neni prime
	for (foo = 2; foo < num; foo++){
		if (num % foo == 0) return 1; // Pokud je delitelne cislem mensim, nez je ono samo
	}
	return 0;
}

/*
	Funkce pro test data a ziskani nazvu dne.
	Vstup: String, nactene slovo
	Vystup:
		_day : V/v promenna pro ulozeni nazvu dne
		-1: Chyba retezce, retezec je priliz dlouhy
		-2: Chyba retezce, nejedna se o format DDDD-DD-DD
		-3: Chyba mktime()
		-4: Chyba hodnot
		0: Uspesny prevod, do promenne _day se ulozi nazev dne
*/
int isDate(char *str, char *_day){
	struct tm time_str;
	int day, month, year;

	if (strLenght(str) == 10){ // Zda odpovida delka retezce
		if (str[0] >= '1' && str[0] <= '9' &&
			str[1] >= '0' && str[1] <= '9' &&
			str[2] >= '0' && str[2] <= '9' &&
			str[3] >= '0' && str[3] <= '9' &&
			str[4] == '-' &&
			str[5] >= '0' && str[5] <= '9' &&
			str[6] >= '0' && str[6] <= '9' &&
			str[7] == '-' &&
			str[8] >= '0' && str[8] <= '9' &&
			str[9] >= '0' && str[9] <= '9'){ // Kontrola formatu DDDD-DD-DD

			sscanf(str, "%d-%d-%d", &year, &month, &day); // Rozdeleni str na cisla

			if (year < 1900 && month == 0 && day == 0) return -4; 

			time_str.tm_year = year - 1900;
			time_str.tm_mon = month - 1;
			time_str.tm_mday = day;
			time_str.tm_hour = 1;
			time_str.tm_min = 1;
			time_str.tm_sec = 1;
			time_str.tm_isdst = -1;
			mktime(&time_str);

			if (mktime(&time_str) == -1)
				return -3;
			else {
				(void)strftime(_day, sizeof(char[5]), "%a", &time_str);
				return 0;
			}
		}return -2;
	}
	return -1;
}


int inputTest(char *str){
	for (int i = 0; i < strLenght(str); i++){
		if (str[i] < 32) return 1;
	}

	return 0;
}