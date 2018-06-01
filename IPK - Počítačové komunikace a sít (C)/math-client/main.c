#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <limits.h>

#define TEST_IP "147.229.13.210"
#define TEST_IP6 "2001:67c:1220:80c::93e5:dd2"
#define PORT 55555
#define HASH "75a7ac0db1b26ff8f3f06f02f4543f42"

/*
1. Create a socket with the socket() system call.
2. Connect the socket to the address of the server using the connect() system call.
3. Send and receive data. Use the read() and write() system calls.
*/

#define SOLVE_OK 0		// uspesne reseni
#define SOLVE_ERROR 1	// nespec. chyba
#define EMPTY_STRING 2 	// prazdna zprava
#define BYE 3			// konec komunikace
#define NUMBER_ERROR 4	// spatne zadane cislo
#define OP_ERROR 5 		// chyba operatora
#define MATH_ERROR 6 	// deleni nulou
#define NUMBER_SIZE 7 	// preteceni rozsahu zadaneho cisla

/*
	Fce pro zpracovani prichozi zpravy serveru
	str - vstupni retezec
	out - vystupni retezec
	return: ok / chybovy kod
*/
int solve(char *str, char *out);
int getip(char *str); // fce pro zjisteni typu adresy


int main(int argc,char *argv[]) {
	int sockfd = 0, code, ip_t;
	char msg[256],tmp[256];
	struct sockaddr_in6 serv_addr6;
	struct sockaddr_in serv_addr;


//vypocet MD5 loginu
    char hash[33];
	unsigned char h[MD5_DIGEST_LENGTH];
    char string[] = "xsztef02";
    MD5((unsigned char*)&string, strlen(string), (unsigned char*)&h);
    for(int i = 0; i < 16; i++)
         sprintf(&hash[i*2], "%02x", (unsigned int)h[i]);

//zpracovani argumentu
	if (argc != 2){
		fprintf(stderr,"Usage: %s <IP ADRESS>\n", argv[0]);
		return 1;
	}else{
		ip_t = getip(argv[1]);
		if(ip_t == -1){
			fprintf(stderr,"Error : IP format\n");
			return 0;
		}
	}


// prevod retezce ip adresy na cislo
	if(ip_t == AF_INET){
		serv_addr.sin_family = ip_t;
		serv_addr.sin_port = htons(PORT);

		if (inet_pton(ip_t, argv[1], &serv_addr.sin_addr) <= 0)
		{
			fprintf(stderr,"Error  IP format\n");
			return 1;
		}
	}else{
		serv_addr6.sin6_family = ip_t;
		serv_addr6.sin6_port = htons(PORT);

		if (inet_pton(ip_t, argv[1], &serv_addr6.sin6_addr) <= 0)
		{
			fprintf(stderr,"Error : IP format\n");
			return 1;
		}
	}

// vytvoreni soketu
	if ((sockfd = socket(ip_t, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr,"Error : Could not create socket \n");
		return 1;
	}

// pripojeni na server
	if(ip_t == AF_INET){
		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			fprintf(stderr,"Error : Connect Failed \n");
			return 1;
		}
	}else{
		if (connect(sockfd, (struct sockaddr *)&serv_addr6, sizeof(serv_addr6)) < 0)
		{
			fprintf(stderr,"Error : Connect Failed \n");
			return 1;
		}
	}


// zaslani uvitaci zpravy
	memset(msg, 0, sizeof(msg));
	strcpy(msg, "HELLO ");
	strcat(msg,hash);
	strcat(msg,"\n");

	//printf("-> %s",msg);

	if (write(sockfd, msg, strlen(msg)) < 0) {
		fprintf(stderr,"ERROR writing to socket");
		return 1;
	}

// solving
	do{
		memset(msg,0, 256);
		memset(tmp,0, 256);

		// cteni zpravy
		if (read(sockfd, msg, 255) < 0) {
			fprintf(stderr,"ERROR reading from socket");
			return 1;
		}

		code = solve(msg,tmp);

		switch(code){
			case BYE: break; //konec komunikace

			//ignorovane chyby
			case EMPTY_STRING: break;
			case OP_ERROR: break;
			case NUMBER_ERROR: break;
			case SOLVE_ERROR: break;

			case SOLVE_OK:
				if (write(sockfd, tmp, strlen(tmp)) < 0){
					fprintf(stderr,"ERROR write to socket");
					return 1;
				}
				//printf("-> %s",tmp);
				break;

			case MATH_ERROR:
			case NUMBER_SIZE:
				if(write(sockfd, "RESULT ERROR\n", 12) < 0){
					fprintf(stderr,"ERROR write to socket");
					return 1;
				}
			break;
			default: break;
		}


	}while(code != BYE);


	if(code == BYE){
		fprintf(stderr,"%s",tmp);
	}

	return 0;
}

int solve(char *str, char *out) {
	if (str == NULL) {
		fprintf(stderr,"ERROR empty string");
		return EMPTY_STRING;
	}


	//printf("<- %s", str);

	//char *bye = "BYE", *solve = "SOLVE";


	int len = strlen(str);

	if (len < 4) {
		fprintf(stderr,"string error len");
		return SOLVE_ERROR;
	}

	//bye test
	if (str[0] == 'B' && str[1] == 'Y' && str[2] == 'E' && str[3] == ' ') {
		for (int i = 4; i < len; i++){
			if(str[i]=='\n') break;
			out[i-4] = str[i];
		}
		return BYE;
	}

	//test solve
	if (str[0] == 'S' && str[1] == 'O' && str[2] == 'L' && str[3] == 'V' && str[4] == 'E' && str[5] == ' ') {
		//first num
		char tmp[128] = { 0, };
		char op;
		long long a=0, b=1;
		long double c;
		int have_all = 0;

		int i = 6; // zacaten retezce bez "SOLVE "
		for (int j = 0; i < len-3; i++) {

			if (str[i] >= '0' && str[i] <= '9') { // [cislo]
				tmp[j++] = str[i];
			}
			else if (str[i] == ' ') { //next part is OP
				switch (str[i + 1]) {
					case '+':
					case '-':
					case '/':
					case '*': op = str[i + 1]; have_all++; break;
					default: return OP_ERROR; // chybna operace
				}
				i += 3; // _(op)_ 3 characters
				a = strtoll(tmp, NULL, 10);
				if(a==LLONG_MAX || a==LLONG_MIN){
					if(errno==ERANGE)
						return NUMBER_SIZE;
				}
				memset(tmp, 0, sizeof(tmp));
				have_all++;
				break;
			}
			else if (j == 0 && str[i] == '-') { // znamenko pred cislem
				tmp[j++] = str[i];
			}
			else {
				return NUMBER_ERROR; // chybne cislo
			}
		}

		//dalsi cislo
		for (int j = 0; i < len; i++) {
			if (str[i] >= '0' && str[i] <= '9') {
				tmp[j++] = str[i];
			}
			else if (str[i] == '\n') { // End
				b = strtoll(tmp, NULL, 10);
				if(b==LLONG_MAX || b==LLONG_MIN){
					if(errno==ERANGE)
						return NUMBER_SIZE;
				}
				have_all++;
				break;
			}
			else if (j == 0 && str[i] == '-') {
				tmp[j++] = str[i];
			}else {
					return NUMBER_ERROR;
				}
		}

		if(have_all!=3){
			return SOLVE_ERROR;
		}

		//solving
		switch (op) {
			case '+': c = (long double)a + b; break;
			case '-': c = (long double)a - b; break;
			case '/': if (b == 0) { return MATH_ERROR; }
					  else c = (long double)a / b; break;
			case '*': c = (long double)a * b; break;
			default: c = -1; break;
		}

		//
		char foo[128]={0,};
		sprintf(foo, "%Lf", c);

		for (int i = 0; i < strlen(foo); ++i){
			if(foo[i]=='.'){
				foo[i+3] = '\0';
			}
		}

		sprintf(out, "RESULT %s\n", foo);


		return SOLVE_OK;
	}

	return SOLVE_ERROR;
}

int getip(char *str){
	int len=strlen(str);

	for(int i = 0; i<len;i++){
		if (str[i]=='.')
		{
			return AF_INET;
		}else if (str[i]==':')
		{
			return AF_INET6;
		}
	}

	return -1;
}
