#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#define MAX_SIZE 512

int get_line(char *buff, int max_size, FILE *f);


int main(int argc, char *argv[]){

	//char filename[255] = "stdin"; // nazev souboru
	int lines = 10; // pocet radku
	FILE *f = stdin;
	int buffer[MAX_SIZE]; // buffer pro radek
	char **buffArray; // rotacni buffer pro n radku

	if (argc == 1){
		//none
	}
	else if (argc == 2){ //name file?
		if (f = fopen(argv[1], "r")){ //otevreni souboru
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
				//exit(1);
			}
		}
		else{
			fprintf(stderr, "Arg error 1!\nUSE:n\n  tail -n NUM_OF_LINES \n  tail FILENAME\n");
			//exit(1);
		}
	}
	else{
		fprintf(stderr, "Arg error 2!\nUSE:n\n  tail -n NUM_OF_LINES \n  tail FILENAME\n");
		//exit(1);
	}
	

	// READ LINES
	
	//printf("lines: %d FILE: %s\nfile: %d\nSTDIN: %d\n",lines,filename,f,stdin);


	buffArray = malloc(lines*sizeof(buffArray));
	if (buffArray == NULL){
		fprintf(stderr, "Malloc error!\n");
		exit(2);
	}

	for (int i = 0; i < lines; i++){ // pole n radku 
		//printf("malloc %d\n", i);
		buffArray[i] = malloc(MAX_SIZE*sizeof(buffArray[0]));
		//printf("%d\n", buffArray[i]);
		if (buffArray[i] == NULL){
			fprintf(stderr, "Malloc error!\n");
			exit(2);
		}
		memset(buffArray[i], 0, MAX_SIZE); // vyprazdneni
	}


	int line = 0;
	int err = 0;
	int was_error = 0;
	while ((err = get_line(buffer, MAX_SIZE, f)) != EOF){ //input
		if (err>0 && !was_error){
			fprintf(stderr, "Prekrocen limit delky radku(%d): %d\n", MAX_SIZE, err);
			was_error++;
		}

		strcpy(buffArray[line], buffer);
		line++;

		if (line == lines){
			line = 0;
		}
	}
	strcpy(buffArray[line], buffer);
	line++;

	//OUTPUT
	for (int i = 0; i < lines; i++){
		if (line == lines) line = 0;
		if (buffArray[line][0] == 0){
			line++;
			continue;
		}	
		printf("%s\n", buffArray[line]);
		line++;
	}

	for (int i = 0; i < lines; i++)
		free(buffArray[i]);
	free(buffArray);
	//printf("%d,%d", f, stdin);

	if (f != stdin) fclose(f);


	//getchar();

	return 0;
}

/*
Funkce pro nacteni radku
*/
int get_line(char *buff, int max_size, FILE *f){
	char c; //nacteny znak
	int i = 0; //pozice na radku

	c = getc(f);
	while (c != EOF){ // do konce radku
		if (c == '\n'){ // konec souboru
			buff[i] = '\0'; // ukonceni stringu
			return 0; // vraci EOF - skoncilo cteni souboru
		}
		else if (i >= max_size){ //mimo rozsah max_size
			buff[i] = '\0'; // ukonceni str
			while (getc(f) != '\n') i++; // nastaveni cursoru na novy radek
			return i; // vraci skutecnou delku radku
		}

		buff[i] = c; // vlozeni znaku do buff
		i++; //inkrementace pozice
		c = getc(f);
	}
	return EOF; // radek je v rozsahu
}


