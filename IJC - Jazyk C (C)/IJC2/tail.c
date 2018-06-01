/**
	IJC DU2

	xsztef02
	Patrik Sztefek
**/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#define MAX_SIZE 512

int get_line(char *buff, int max_size, FILE *f);


int main(int argc, char *argv[]){
	//printf("start");
	//char filename[255] = "stdin"; // nazev souboru
	int lines = 10; // pocet radku
	FILE *f = stdin;
	char buffer[MAX_SIZE]; // buffer pro radek
	char **buffArray; // rotacni buffer pro n radku

	if (argc == 1){
		//none
	}
	else if (argc == 2){ //name file?
		if ((f = fopen(argv[1], "r"))){ // otevreni souboru
			//strcpy(filename, argv[1]);
		}
		else{
			fprintf(stderr, "File error!\n");
			exit(1);
		}
	}
	else if (argc == 3){ // -n num ??
		if (!strcmp(argv[1], "-n")){
			/// TODO: 5asdfa
			lines = (int)strtol(argv[2], (char **)NULL, 10);
			if (lines == INT_MAX && errno == ERANGE){
				fprintf(stderr, "Arg error 0!\nUSE:n\n  tail -n NUM_OF_LINES \n  tail FILENAME\n");
				exit(1);
			}
		}
		else{
			fprintf(stderr, "Arg error 1!\nUSE:n\n  tail -n NUM_OF_LINES \n  tail FILENAME\n");
			exit(1);
		}
	}
	else{
		fprintf(stderr, "Arg error 2!\nUSE:n\n  tail -n NUM_OF_LINES \n  tail FILENAME\n");
		exit(1);
	}
	

//MALLOCS	
	buffArray = malloc(lines*sizeof(buffArray)); // malloc radku
	if (buffArray == NULL){
		fprintf(stderr, "Malloc error!\n");
		exit(2);
	}

	for (int i = 0; i < lines; i++){ // pole n radku 
		buffArray[i] = malloc(MAX_SIZE*sizeof(buffArray[0]));
		if (buffArray[i] == NULL){
			fprintf(stderr, "Malloc error!\n");
			exit(2);
		}
		memset(buffArray[i], 0, MAX_SIZE); // vyprazdneni
	}

// READ LINES
	int line = 0;
	int err = 0;
	int was_error = 0; 
	while ((err = get_line(buffer, MAX_SIZE, f)) != EOF){ //input
		if (err>0 && !was_error){ //pouze prvni prekroceni rozsahu radku 
			fprintf(stderr, "Prekrocen limit delky radku(%d): %d\n", MAX_SIZE, err);
			was_error++;
		}

		strcpy(buffArray[line], buffer);
		line++;

		if (line == lines){ // rotacni buffer
			line = 0;
		}

	}
	strcpy(buffArray[line], buffer); // posledni radek

	line++; 

//OUTPUT
	for (int i = 0; i < lines; i++){
		if (line == lines) line = 0; //rotovat
		if (buffArray[line][0] == 0){ //pokud nebyl buffer zaplnen
			line++;
			continue;
		}	
		printf("%s\n", buffArray[line]); 
		line++;
	}

	for (int i = 0; i < lines; i++)
		free(buffArray[i]);
	free(buffArray);

	if (f != stdin) fclose(f); // zavreni souboru pokud to neni stdin

	system("pause");
	return 0;
}

/*
Funkce pro nacteni radku
*/
int get_line(char *buff, int max_size, FILE *f){
	char c; //nacteny znak
	int i = 0; //pozice na radku

	c = getc(f);
	while (c != EOF && c!='\n'){ // do konce radku/souboru
		//printf(".");
		if (i < (max_size-1)){  
			buff[i++] = c;
			buff[i] = '\0';
		}
		else{
			i++; // delka prekroceneho radku
		}
		c = getc(f); 
	}

	if (i < (max_size) && c != EOF) return 0; // pokud nepresahla delka radku max

	if (c == '\n') return i; // pokud presahla a neni to posledni radek
	
	return EOF;
}
