#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_BUFFER 1024

int
main(){
	
	int fd = socket(PF_INET, SOCK_STREAM, 0);

	if (fd == -1) {
		printf("Socket creation failed\n");
		return -1; 	  
	}

	struct sockaddr_in sin;
	bzero(&sin, sizeof(sin));
				  
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8080);

	sin.sin_addr.s_addr = INADDR_ANY ; 	
	
	if (connect (fd, (struct sockaddr *) &sin, sizeof(sin)) > 0) 
		perror("Connection to server failed");
		
	printf("Connection established ........ \n");

	char data[MAX_BUFFER];
	memset(data, 0, MAX_BUFFER); 

	for(;;) {
		ssize_t read = recv(fd, data, MAX_BUFFER, 0);

		if (read < 0) {
			perror("recv failed");
			return 1;
		}

		if (read >= 0) {

			data[read < MAX_BUFFER ? read : MAX_BUFFER -1] = '\0'; 

			printf("%s\n", data);
		}

		char input[MAX_BUFFER];
		scanf("%s", input);
		

		if (strncmp(input, "exit", strlen("exit")) == 0) {
			close(fd);
			return 1;
		}
	
		while ((getchar()) != '\n');  // Clear the newline character from the input buffer
		
		send(fd, input, MAX_BUFFER, 0);

	}	
	
	return 0;
}
