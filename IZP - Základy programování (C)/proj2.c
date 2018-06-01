#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <float.h>
#include <string.h>

double taylor_log(double x, unsigned int n);
double cfrac_log(double x, unsigned int n);

double power(double num, unsigned int e);
int getIters(double min, double max, double eps);
int getLog(double x, int n);
double fAbs(double min);
double _taylor_log(double x, unsigned int n);
void help();

int main(int args, char **argv){
	(void)args;
	(void)argv;
	double x; //x resp. min u --iter
	double max; //max u --iter
	double eps; //presnost
	int n; //pocet iteraci
	char *error;

	if (args > 0){
		if (args == 4){ 
			if (!strcmp(argv[1], "--log")){
				x = strtod(argv[2], &error); 
				if ((x == DBL_MAX && errno == ERANGE) || *error != '\0'){ //kontrola prevodu
					printf("Chyba paremetru 1\n");
					return -1;
				}
				else{
					n = (int)strtol(argv[3], (char **)NULL, 10);
					if (n == INT_MAX && errno == ERANGE){ 
						printf("Chyba paremetru 2\n");
						return -1;
					}
					else{
						// vykonai prikazu --log	
						getLog(x, n);
						return 0;
					}
				}
			}
			else {
				help();
				return -1;
			}
		}
		else if (args == 5){
			if (!strcmp(argv[1], "--iter")){
				x = strtod(argv[2], &error);
				if ((x == DBL_MAX && errno == ERANGE) || *error != '\0'){
					printf("Chyba paremetru 1\n");
					return -1;
				}
				else{
					max = strtod(argv[3], &error);
					if ((max == DBL_MAX && errno == ERANGE) || *error != '\0'){
						printf("Chyba paremetru 2\n");
						return -1;
					}
					else{
						eps = strtod(argv[4], &error);
						if ((eps == DBL_MAX && errno == ERANGE) || *error != '\0'){
							printf("Chyba paremetru 3\n");
							return -1;
						}
						else{
							// vykonani prikazu --iter
							getIters(x, max, eps);
							return 0;
						}
					}
				}
			}
			else{
				help();
				return -1;
			}
		}
		else {
			help();
			return -1;
		}
	}
	else {
		help();
	}

	//printf("\nStopCLK: %d\n", clock() * 1000 / CLOCKS_PER_SEC);

	//system("pause");
	return 0;
}

/*
Funkce pro vypocet logaritmu taylor
*/
double taylor_log(double x, unsigned int n){
	unsigned int i;
	double step=1; //aktualni krok
	double actual = 0; // postupny vysledek
	double base = 1;

	if (x == 0 || x == 1) return 0;

	if (x > 1){
		for (i = 1; i <= n; i++){ 
			base *= (x - 1)/x;  
			step = base / i; 
			if (step == 0) break; // pokud vypoctena hodnota je mensi mensi nez minimalni hodnota double
			actual += step; 
		}
	}
	else {
		double tmp = 1 - x;
		for (i = 1; i <= n; i++){
			base *= tmp;
			step = base / i;
			if (step == 0) break;
			actual -= step;
		}
	}

	return actual;
}

/*
	Funkce pro vypocet logaritmu cfrac
*/
double cfrac_log(double x, unsigned int n){
	unsigned int i = n - 1;// pocet iteraci
	double tmp = (1 - x) / (-1 - x); // z
	double prev = 0.0; // aktualni prvek resp. posledni

	if (x == 0 || x == 1) return 0;

	while (i > 0){
		prev = (tmp*tmp*i*i) / ((i * 2 + 1) - prev);
		i--;
	}

	return 2 * tmp / (1 - prev);
}


/*
	Funkce pro vypocet absolutni hodnoty
*/
double fAbs(double num){
	if (num >= 0) return num;
	return -1.0 * num;
}

/*
	Funkce pro zpracovani --iter min max eps. Urceni poctu iteracai pro zadanou presnost.
	Implementován algoritmus BISEKCE.
	Vstup:
		min - zacatek intervalu
		max - konec intervalu
		eps - pozadovana presnost
	Vystup:
		null
*/
int getIters(double min, double max, double eps) {
	int itersMax; // maximalni pocet iteraci
	int itersMin; // minimalni pocet iteraci
	int half; // polovicni hodnota mezi itersMin a itersMax
	double _eps; // aktualni presnost

	//changs
	double x; // log(x)
	double logMath; // = log(x) math.h
	double(*_log)(double, unsigned int); // ukazatel na funkce cf_log a tylor_log
	int itersOfMin; // pocet iteraci pro hodnotu log(min)
	int itersOfMax; // pocet iteraci pro hodnotu log(max)
	int itersOfCfrac; // vypocteny pocet iteraci pro cf_log
	int itersOfTaylor; // vypoctney pcet iteraci pro tylor_log
	int *itersTmp; // ukazatel na promenne itersOfMax a Min
	int *itersCount; // ukayatel na vyslenou promennou itersOfCf / Tylor

	if (min > max){
		printf("Spatne zadan MIN a MAX"); 
		return -1;
	}
		

	itersTmp = &itersOfMin; 
	_log = &cfrac_log; 
	itersCount = &itersOfCfrac; 
	/*
		Prvni dva pruchody forse pocitaji hodoty pro cfrac, dalsi dva pruchody pro taylor
		ukazalete jsu na staveny na min/max resp. cf/tylor, podle toho
		se kterou funkci zrovna pracujeme.
	*/
	for (int i = 0; i < 4; i++){
		//set
		if (i == 0){ //1. pruchod, nastaveni x zacatkem intervalu, ukazalatele jsou nastaveny pred smyckou
			x = min; 
		}
		else if (i == 1 || i == 3){ // 2. a 4. pruchod, pracuje se s MAX
			x = max;
			itersTmp = &itersOfMax;
		}
		else if (i == 2){ // 3. pruchod, x je min, meni se funkce na tylor
			x = min;
			itersTmp = &itersOfMin;
			_log = &taylor_log;
			itersCount = &itersOfTaylor;
		}
		logMath = log(x); // vypocteni logaritmu pro porovnavani presnosti
		itersMin = 2; // minimalni pocet iteraci
		itersMax = INT_MAX; // maximalni pocet iteraci


		// Smycka pro vypocitani iteraci, konci pokud sme nasli pozadovanou presnot
		while (itersMax - itersMin > 1){
			half = ((itersMax - itersMin) / 2 + itersMin); // vypocteni strdni hodnoty poctu iteraci

			_eps = fAbs(fAbs((*_log)(x, half)) - fAbs(logMath)); // vypocet aktualni odchylky od hodnoty log z math.h

			if (_eps >= eps) // pokud je aktualni presnost mensi nez pozadovana
				itersMin = half; // minumum iteraci je nastaveno na polovinu
			else
				itersMax = half; // pokud ne tak max je nataveno na polovicni hodnotu
		}
		*itersTmp = itersMax;// ???!!!
		printf(".. %g - %d\n", (*_log)(x, half),itersMax);
		// po skonceni cf nebo tylor se pocita ktera hodnota je vyssi
		if (i % 2){ 
			if (itersOfMin > itersOfMax) *itersCount = itersOfMin; // podle nastaveni ukazatele se hodnota iteraci uklada do itersOfCf / Tylor
			else *itersCount = itersOfMax;
        }
	}


	// vypis vysledku
	printf("       log(%g) = %.12g\n", min, log(min));
	printf("       log(%g) = %.12g\n", x, log(max));

	printf("continued fraction iterations = %d\n", itersOfCfrac);
	printf("    cf_log(%g) = %.12g\n", min, cfrac_log(min, itersOfCfrac));
	printf("    cf_log(%g) = %.12g\n", x, cfrac_log(max, itersOfCfrac));

	printf("taylor polynomail iteratuons = %d\n", itersOfTaylor);
	printf("taylor_log(%g) = %.12g\n", min, taylor_log(min, itersOfTaylor));
	printf("taylor_log(%g) = %.12g\n", x, taylor_log(max, itersOfTaylor));

	return 0;
}

/*
	Funkce pro --log x n
	Vstup:
		x - cislo, pro ktere pocitame logaritmus
		n - pocet iteraci
	Vystup:
		null
*/
int getLog(double x, int n){

	printf("       log(%g) = %.12g\n", x, log(x));
	printf("    cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
	printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));

	return 0;
}
/*
Funkce pro help
*/
void help(){
	printf("Help\n");
	printf("Program slouzi v vypoctu logaritnu z cisla x resp."
		   "poctu iteraci potrebny k zadane presnosti, pro urcity interval min-max\n");
	printf("Parametry programu:\n");
	printf("  --log (double)x (int)pocet_iteraci\n");
	printf("  --iters (double)min (double)max (double)presnost\n");
}
