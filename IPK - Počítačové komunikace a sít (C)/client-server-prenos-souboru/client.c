/*
	IPK2018 proj1
	Patrik Sztefek xsztef02

	./ipk-client -h host -p port [-r|-w] file
	• host (IP adresa nebo fully-qualified DNS name) identifikace serveru jakožto koncového
	bodu komunikace klienta;
	• port (číslo) cílové číslo portu;
	• -r značí, že klient bude ze serveru soubor číst;
	• -w značí, že klient bude na server soubor zapisovat;
	• file (cesta) určuje cestu k souboru, se kterým se bude manipulovat (buď se soubor
	odkazovaný cestou uploadne do pracovního adresáře serveru, nebo se obsah souboru
	stejného jména v pracovním adresáři serveru downloadne na místo specifikované cestou).
*/

#define _POSIX_C_SOURCE 2
#include <unistd.h> // getopt
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include <libgen.h>


#define USAGE  "Usage: ./ipk-client -h host -p port [-r|-w] file\n"
#define FILE_404 -2 // file not found
#define FILE_ERR -1 // file in use
#define BUFSIZE 255

int getip(char *str); // fce pro zjisteni typu adresy
int hostnameToIp(char * hostname , char* ip); // prevod name na ip
int sendFile(int socket, char *filename); // odesilani souboru
int receiveFile(int socket, char *filename); // prijem souboru

int main( int argc, char **argv){
	char c;
	char mode = '\0';
	int hflag = 0, pflag = 0,wflag = 0, rflag = 0;
	char host[255],port[12],file[255];

	if(argc != 7){
		fprintf(stderr, USAGE);
		return 1;
	}

	// arg parsing
	while(( c = getopt( argc, argv, "h:p:r:w:" )) != -1 ){
		switch ( c ){
			case 'h':
			hflag++;
			strcpy(host,optarg);
			break;
			case 'p':
			pflag++;
			strcpy(port,optarg);
			break;
			case 'r':
			if(mode == 'w'){
				fprintf(stderr, USAGE);
				return 1;
			}
			rflag++;
			mode = 'r';
			strcpy(file,optarg);
			break;
			case 'w':
			if(mode == 'r'){
				fprintf(stderr, USAGE);
				return 1;
			}
			wflag++;
			mode = 'w';
			strcpy(file,optarg);
			break;
			default:
			fprintf(stderr, USAGE);
			return 1;
			break;
		}
	}

	if(hflag == 1 && pflag == 1 && (wflag == 1 || rflag == 1)){
		//valid;
		printf("Connecting to %s:%s with %c to file %s\n",host,port,mode,file);
	}else{
		fprintf(stderr, USAGE);
		return 1;
	}


	int sockfd = 0, code, ip_t;
	char msg[256],tmp[256];
	char ip[128];
	struct sockaddr_in6 serv_addr6;
	struct sockaddr_in serv_addr;

	ip_t = getip(host);

	// hostname convert to ip
	if(ip_t == -1){
		hostnameToIp(host, ip);
		ip_t = AF_INET;
	}else{
		strcpy(ip, host);
	}


	// ip convert
	if(ip_t == AF_INET){
		serv_addr.sin_family = ip_t;
		serv_addr.sin_port = htons(atoi(port));

		if (inet_pton(ip_t, ip, &serv_addr.sin_addr) <= 0)
		{
			fprintf(stderr,"Error IP format\n");
			return 1;
		}
	}
	else{
		serv_addr6.sin6_family = ip_t;
		serv_addr6.sin6_port = htons(atoi(port));

		if (inet_pton(ip_t, ip, &serv_addr6.sin6_addr) <= 0)
		{
			fprintf(stderr,"Error IP6 format\n");
			return 1;
		}
	}

	// create socket
	if ((sockfd = socket(ip_t, SOCK_STREAM, 0)) < 0){
		fprintf(stderr,"Error : Could not create socket \n");
		return 1;
	}

	// connection
	if(ip_t == AF_INET){
		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
			fprintf(stderr,"Error Connect Failed \n");
			return 1;
		}
	}
	else{
		if (connect(sockfd, (struct sockaddr *)&serv_addr6, sizeof(serv_addr6)) < 0){
			fprintf(stderr,"Error Connect Failed \n");
			return 1;
		}
	}

	memset(msg, 0, 256);
	sprintf(msg,"%c %s",mode, basename(file));

	// osending command to server
	if (send(sockfd, msg, 255, 0) < 0){
		fprintf(stderr, "Error sending command\n");
		return -1;
	}

	int rw_state = 0;

	if(mode == 'w'){
		rw_state = sendFile(sockfd, file);
	}else{
		rw_state = receiveFile(sockfd, file);
	}

	if(rw_state > 0)
		fprintf(stderr,"Transfer NOT completed. Connection lost\n");
	else if(rw_state == 0)
		printf("Transfer completed\n");

	close(sockfd);
	return 0;
}

int getip(char *str){
	if((void*)strchr(str,':') != NULL) return AF_INET6; // ipv6
	if(str[0]>='0' && str[0]<='9') return AF_INET; // ipv4
	return -1; //hostname
}

int hostnameToIp(char * hostname , char* ip){
	struct hostent *he;
	struct in_addr **addr_list;
	int i;

	if ((he = gethostbyname(hostname)) == NULL){
		return 1;
	}

	addr_list = (struct in_addr **) he->h_addr_list;

	for(i = 0; addr_list[i] != NULL; i++){
		strcpy(ip , inet_ntoa(*addr_list[i]) );
		return 0;
	}

	return 1;
}

int sendFile(int socket, char *filename){
	off_t offset = 0,remaining = 0;
	int sent = 0;
	struct stat file_stat;
	int total = 0;
	char filesize[255];

	// if file exist
	if(access( filename, F_OK ) == -1 ){
		fprintf(stderr, "Error file not exist\n");
		if(send(socket, "-2", 2, 0) < 0){
			fprintf(stderr, "Error sending error state\n");
			return -1;
		}
		return -1;
	}

	int file = open(filename, O_RDONLY);
	if(file == -1){
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

	if (fstat(file, &file_stat) < 0){
		fprintf(stderr, "Error fstat\n");
		return -1;
	}

	printf("File size: %d Bytes\n", (int)file_stat.st_size);

	sprintf(filesize, "%d", (int)file_stat.st_size);

	// send filesize
	if (send(socket, filesize, sizeof(filesize), 0) < 0){
		fprintf(stderr, "Error on sending filesize\n");
		return -1;
	}

	remaining = file_stat.st_size;

	while (((sent = sendfile(socket, file, &offset, BUFSIZE)) > 0) && (remaining > 0)){
		remaining -= sent;
		printf("Sending %d B of %d B, remaining %d B\n", sent, (int)file_stat.st_size, (int)remaining);
	}

	total = (int)file_stat.st_size - (int)remaining;
	printf("Total bytes sent: %d\n", total);

	return remaining;
}

int receiveFile(int socket, char *filename){
	char buffer[BUFSIZE];
	FILE *f;
	off_t remaining = 0;
	int received = 0,total = 0;
	int filesize = 0;

	recv(socket, buffer, BUFSIZE, 0);
	filesize = atoi(buffer);

	// server error on open
	if(filesize == -1){
		fprintf(stderr, "Error opening file at server\n");
		return -1;
	}
	// server  error file 404
	if(filesize == -2){
		fprintf(stderr, "Error file does not exist at server\n");
		return -1;
	}

	f = fopen(filename,"w");
	if(f == NULL){
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

	remaining = filesize;
	//cteni socketu do ukonceni spojeni/konce souboru
	while(((received = recv(socket, buffer, BUFSIZE, 0)) > 0) && (remaining > 0)){
		fwrite(buffer, sizeof(char), received, f);
		remaining -= received;
		printf("Received %d B of %d B, remaining %d B\n", received, (int)filesize, (int)remaining);
	}

	fclose(f);

	total = filesize-remaining;
	printf("Total bytes received: %d\n", total);

	return remaining;
}
