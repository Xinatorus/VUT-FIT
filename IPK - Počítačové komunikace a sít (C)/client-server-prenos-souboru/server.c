/*
	IPK2018 proj1
	Patrik Sztefek xsztef02


	./ipk-server -p port
	• port (číslo) číslo portu, na kterém server naslouchá na připojení od klientů.
	např.
	./ipk-server -p 55555
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

#define USAGE  "Usage: ./ipk-server -p port\n"
#define BUFSIZE 255

static volatile int running = 1;
void intHandler(int dummy) {
	running = 0;
}

int sendFile(int socket, char *filename); // odesilani souboru
int receiveFile(int socket, char *filename); // prijem souboru

int main( int argc, char **argv){
	char c_tmp;
	char mode = '\0';
	int pflag = 0;
	short port;

	if(argc != 3){
		fprintf(stderr, USAGE);
		return 1;
	}

	// arg parse
	while(( c_tmp = getopt( argc, argv, "p:" )) != -1 ){
		switch ( c_tmp ){
			case 'p':
			pflag++;
			port = atoi(optarg);
			break;
			default:
			fprintf(stderr, USAGE);
			return 1;
			break;
		}
	}

	if(pflag == 1){
		//valid;
		printf("Opening server at port %i\n",port);
	}else{
		fprintf(stderr, USAGE);
		return 1;
	}


	int serverSocket , clientSocket , c;
	struct sockaddr_in server , client;
	char *message;

	// create socket
	serverSocket = socket(AF_INET , SOCK_STREAM , 0);
	if (serverSocket == -1){
		fprintf(stderr, "Error creating socket\n");
		return 1;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	// port bind
	if(bind(serverSocket,(struct sockaddr *)&server , sizeof(server)) < 0){
		fprintf(stderr, "Error binding port\n");
		return 1;
	}

	printf("Waiting for clients...\n");

	// ctrl + c
	signal(SIGINT, intHandler);
	while(running){

		// lisening
		listen(serverSocket , 3);

		// accept connection
		c = sizeof(struct sockaddr_in);
		clientSocket = accept(serverSocket, (struct sockaddr *)&client, (socklen_t*)&c);
		if (clientSocket<0){
			fprintf(stderr, "Error accepting connection\n");
			break;
		}

		printf("Client %s connected\n", inet_ntoa(client.sin_addr));

		char buffer[BUFSIZE];
		char filename[BUFSIZE];

		// reading command froom client
		if(recv(clientSocket, buffer, BUFSIZE, 0) < 0){
			fprintf(stderr, "Error reading command\n");
			break;
		}
		// [r/w] filename
		printf("Received command: %s\n",buffer);
		mode = buffer[0];
		strcpy(filename,buffer+2);

		int rw_state;

		if(mode == 'w'){
			rw_state =  receiveFile(clientSocket, filename);
		}else{
			rw_state = sendFile(clientSocket, filename);
		}

		if(rw_state > 0)
			fprintf(stderr,"Transfer NOT completed. Connection lost\n");
		else if(rw_state == 0)
			printf("Transfer completed. Client disconected\n");

		close(clientSocket);
	}

	printf("\nClosing server...\n");
	close(clientSocket);
	close(serverSocket);

	return 0;
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
		fprintf(stderr, "Error opening file at client\n");
		return -1;
	}
	// server  error file 404
	if(filesize == -2){
		fprintf(stderr, "Error file does not exist at client\n");
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
