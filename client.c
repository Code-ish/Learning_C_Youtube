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

// char data[100];

int
main(){
	/*
	 * [ Left-Value ] 'clientSocket' CHANGED TO  'fd'   =   Which is our new socket or sock- File-Descriptor.
	 * 
	 * [ Right-Value-1 ] 'PF_INET' or 'AF_INET'...  You can use either: 	PF "Protocol Family" 
	 * 									AF "Address_Family"
	 * 						They are the same in the header file.
	 */
	
	int fd = socket(PF_INET, SOCK_STREAM, 0);

//	↓↓↓ : Additional error handling added.   Later we will initiate a second attempt to create the socket.
	if (fd == -1) {
		printf("Socket creation failed\n");
		return -1; // "/OR/" 'EXIT_FAILURE' instead of -1 for socket creation.
			   // I personally think it is better to just learn the codes...
			   // BUT if your prefer, by all means type EXIT_FAILURE every single time ;)
	}

	/*   YOU DO NOT NEED TO ADD THIS - Here just for clarity of our choices. 
	
	struct sockaddr_in {

           	sa_family_t         sin_family;      "AF_INET"
           	in_port_t           sin_port;        "Port number"
        	struct in_addr      sin_addr;        "IPv4 address"
       	};

	*/

	struct sockaddr_in sin;
	
	bzero(&sin, sizeof(sin));  // Added zeroing out of the structure.
				   //    While this isn't necessary per the IEEE documentation,
				   //    it is recommended for sanity across architectures and future changes. 

	sin.sin_family = AF_INET;
	sin.sin_port = htons(8080);

	/*
	 * '.sin_addr' "STRUCT" Is a nested "UNION" , for which only one data type can be chosen..
	 *
	 * 			s_un_b 	(four 1-byte integers)
	 * 			s_un_w 	(two  2-byte integers)
	 * 			s_addr 	(one  4-byte integers)
	 * 
	 *  This is why there is the additional '.s_addr' on the following line for the "sockaddr_in" struct.
	*/
	

	sin.sin_addr.s_addr = INADDR_ANY ; 	// REMOVED "htonl()", INADDR_ANY binds socket to ALL available interfaces.  
						//
						//  ↓ FOR"PRODUCTION"USE:↓
						//  inet_addr(012.345.67.89);  "DON'T USE MY IP..." ;) 
						//  	         a.b.c.d 
						//  "Alway interpreted from Left->To->Right"
						//    	    Duly "htonl()" removal.
			                        
// Connect() Function ↓↓↓ 
/*  [ Right-Value-2 ] ↓↓↓   Casting data-type "struct sockaddr *" onto "struct sockaddr_in *" 
			       * 	
			       *	sockaddr  is a generic descriptor for any kind of socket operation, 
			       *		  whereas sockaddr_in is a struct specific to IPv4-based communication. 				       			   *		  
			       *		  the "_in" stands for InterNet.
			      
			      
* [ if COMPARISON ] ↓↓↓ 
*		    ↓↓↓    Instead of attaching the chat functionality INSIDE the IF statement,
*	   		   for now let's just error check the connection and move on.
*
*	   		   REMOVED block from IF statement
*	   		   -1 = FAILURE 	0 = SUCCESS 
*	   		   
*	   		   Comparison switched to '>' Operator,
*			   ';' and '{' are not necessary for if statement 
*			   so long as your if statement only requires a 
*			   single subsequent line of code. The subsequent 
*			   line of code should be completed with usual ';' 
*			   statement completion operator.
							       ↓↓       */
	if (connect (fd, (struct sockaddr *) &sin, sizeof(sin)) > 0) 
		perror("Connection to server failed");
		
	printf("Connection established ........ \n");

	
	// Declaring a BUFFER called data, with size of MAX_BUFFER 1024-Bytes
	char data[MAX_BUFFER];
	memset(data, 0, MAX_BUFFER); // memset to make sure the buffer space is empty.

					// ↓↓↓		     ↓↓↓  		↓↓↓ 
					// Flag value set to '0' 
					// you can check out the flags on the 
					// recv function documents for your 
					// personal needs.. You will likely want '0'
					//	 ↓↓
	for(;;) {
		ssize_t read = recv(fd, data, MAX_BUFFER, 0);

		// If 'recv()' value is -1 we have an error, 
		// We added error check so we can know
		// if there was an error, as well as to stop
		// the program from continuing while there is 
		// an error, with 'return'.
		if (read < 0) {
			perror("recv failed");
			return 1;
		}

		/* We changed a lot about what they had going on below this point...
		 *
		 * We DO NOT want to null terminate if the msg is greater than MAX_BUFFER
		 * If it exceeds 1024 bytes "buffer overflow" and writes a null byte, we
		 * have undefined behavior in the program. 
		 *
		 */
		if (read >= 0) {
		//	Use of Ternary Operator '?' is shorthand way of IF-ELSE Statement..
			data[read < MAX_BUFFER ? read : MAX_BUFFER -1] = '\0'; 
			/*
			 *↓↓↓  Longhand visual representation of what is happening ↓↓↓
			 *
			 * 	if (read_bytes < MAX_BUFFER) {
			 * 		data[read_bytes] = '\0';
			 * 	} else {
					data[MAX_BUFFER - 1] = '\0';
			 * 	}
			 *
			 */

			printf("%s\n", data);
		}

		char input[MAX_BUFFER];
		scanf("%s", input);
		
		/* ADDED "exit" Command for the terminal */
		if (strncmp(input, "exit", strlen("exit")) == 0) {
			close(fd);
			return 1;
		}
		// 
		// Added this while statement to clear in the input buffer for subsequent messages 
		// as-well-as for use of "exit" command to get out of the program, which we would
		// otherwise be left use "ctrl-c" interrupt signal and suspend program execution.
		// "getchar()" Discards any leftover characters in the input buffer (e.g., newline)
		//  which would otherwise get left behind from the "scanf()" function. 
		//
		while ((getchar()) != '\n');  // Clear the newline character from the input buffer
		
		send(fd, input, MAX_BUFFER, 0);

	}	
	close(fd); // CLOSE the socket after use
	return 0;
}
	
