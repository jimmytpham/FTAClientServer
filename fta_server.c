/* Jimmy Pham
*  T00629354
*  COMP 3271
*  Thompson Rivers University
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#define	MAXLINE	512	// maximum bytes to receive and send at once

// External functions
extern int swap_wait(unsigned short port);
extern int swap_read(int sd, unsigned char buf[]);
extern void swap_close(int sd);

int main(int argc, char *argv[])
{
	unsigned short server_port;
	unsigned int server_address;
	unsigned char message[MAXLINE];
	char buffer[MAXLINE];
	int sd, n, out;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(1);
	}

	// set values for server address and port number
	server_port = htons(atoi(argv[1]));

	// connect to the port allocated to the swap server
	sd = swap_wait(server_port);
	if (sd < 0) {
		fprintf(stderr, "swap_open error\n");
		exit(0);
	}
	printf("Connected to port allocated to swap server\n");

	//Students to code the following:
	
	//read in the first message as the new name of the file
	n = swap_read(sd, message);
    if (n <= 0) {
        fprintf(stderr, "swap_read error\n");
        exit(1);
    }
    message[n] = '\0';  // Null-terminate the string
	printf("Read first message as the new name of the file: %s\n", message);

	//open output file for writing
	out = open("output-filename", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR| S_IWUSR);
    if (out == -1) {
		fprintf(stderr, "Error opening output file\n");
        perror("open");
        exit(1);
    }
	printf("Open output file.\n");

	//begin-loop
	while ((n = swap_read(sd, message)) > 0)
    {
        // Write the received data to the output file
        if (write(out, message, n) == -1)
        {
            perror("write");
            fprintf(stderr, "Error writing data to file\n");
            close(out);
            exit(1);
        }
        printf("Data written to output file\n");
    }
	//end-loop
	
	// close the connection to the swap servfer
	swap_close(sd);
	
	//close connection to output file
	close(out);

	return 0;
}
