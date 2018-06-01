#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOLVE_OK 0
#define SOLVE_ERROR 1
#define EMPTY_STRING 2
#define BYE 3
#define NUMBER_ERROR 4
#define OP_ERROR 5
#define MATH_ERROR 6

int solve(char *str, char *out);

int main(char argc, char *argv[]) {
	char tmp[1024] = { 0, };

	printf("%d : ", solve("SOLVE -1 + -1\n",tmp)); printf("%s",tmp);
	printf("%d : ", solve("SOLVE 1 + -1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE 1 + 1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE -1 * -1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE -1 / -1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE -0 / -1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE 0 / -1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE 0 + 1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE -1d + -1\n", tmp)); printf("%s", tmp);
	printf("%d : ", solve("SOLVE -1 + -1\nhjgjg", tmp)); printf("%s", tmp);
	getchar();
	return 0;
}

int solve(char *str, char *out) {
	if (str == NULL) {
		printf("ERROR empty string");
		return EMPTY_STRING;
	}


	printf("%s", str);

	char *bye = "BYE", *solve = "SOLVE";


	int len = strlen(str);

	if (len < 4) {
		printf("string error len");
		return NULL;
	}

	memset(out, 0, 1024);

	//bye test
	//TODO SECRET jen do \n
	if (str[0] == 'B' && str[1] == 'Y' && str[2] == 'E' && str[3] == ' ') {
		strcpy(out, str + 4);
		return BYE;
	}

	//test solve
	if (str[0] == 'S' && str[1] == 'O' && str[2] == 'L' && str[3] == 'V' && str[4] == 'E' && str[5] == ' ') {
		//first num
		char tmp[50] = { 0, };
		char op;
		long a, b, c;

		int i = 6;
		for (int j = 0; i < len; i++) {
			
			if (str[i] >= '0' && str[i] <= '9') {
				tmp[j++] = str[i];
			}
			else if (str[i] == ' ') { //next part OP
				switch(str[i + 1]) {
					case '+':
					case '-':
					case '/':
					case '*': op = str[i + 1]; break;
					default: return OP_ERROR;
				}
				i += 3; // _(op)_ 3 chars
				a = strtol(tmp, NULL,10);
				memset(tmp, 0, 50);
				break;
			}
			else if (j==0 && str[i]=='-') {
				tmp[j++] = str[i];
			}
			else {
					return NUMBER_ERROR;
				}
		}

		for (int j = 0; i < len; i++) {
			if (str[i] >= '0' && str[i] <= '9') {
				tmp[j++] = str[i];
			}
			else if (str[i] == '\n') { // End
				b = strtol(tmp, NULL, 10);
				break;
			}
			else if (j == 0 && str[i] == '-') {
				tmp[j++] = str[i];
			}
			else {
				return NUMBER_ERROR;
			}
		}

		//solving
		switch (op) {
			case '+': c = a + b; break;
			case '-': c = a - b; break;
			case '/': if (b == 0) { return MATH_ERROR; }
					  else c = a / b; break;
			case '*': c = a * b; break;
			default: c = -1; break;
		}

		//

		sprintf(out, "RESULT %ld %c %ld = %ld\n", a, op, b, c);

		return SOLVE_OK;
	}

	


	return SOLVE_ERROR;
}