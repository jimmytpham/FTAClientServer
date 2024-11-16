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

#define MAXLINE 512 // maximum bytes to receive and send at once

// External functions
extern int swap_open(unsigned int addr, unsigned short port);
extern int swap_write(int sd, unsigned char buf[], int length);
extern void swap_close(int sd);

int main(int argc, char *argv[])
{
    unsigned short server_port;
    unsigned int server_address;
    unsigned char buf[MAXLINE];
    int sd, n, in;
	char file_buffer[MAXLINE];

    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s address port input-filename\n", argv[0]);
        exit(1);
    }

    // set values for server address and port number
    server_address = inet_addr(argv[1]); // server address
    server_port = htons(atoi(argv[2]));  // port number

    // connect to the swap server
    sd = swap_open(server_address, server_port);
    if (sd < 0)
    {
        fprintf(stderr, "swap_open error\n");
        exit(0);
    }

    // open input file for reading
    in = open(argv[3], O_RDONLY);
    if (in < 0)
    {
        perror("open");
        fprintf(stderr, "Error opening input file\n");
        exit(1);
    }
    printf("input file open for reading.\n");

    // send input file name to the server
    strcpy(buf, argv[3]);
    n = swap_write(sd, buf, strlen(buf) + 1);
    if (n < 0)
    {
        fprintf(stderr, "Error sending destination file name\n");
        fprintf(stderr, "swap_write error\n");
        exit(1);
    }
    printf("input file sent to server.\n");

	// read input file contents
    ssize_t bytes_read = read(in, file_buffer, MAXLINE);
    if (bytes_read < 0)
    {
        perror("read");
        fprintf(stderr, "Error reading input file\n");
        close(in);
        exit(1);
    }

    // send input file contents to the server
    n = swap_write(sd, file_buffer, bytes_read);
    if (n < 0)
    {
        fprintf(stderr, "Error sending input file contents\n");
        fprintf(stderr, "swap_write error\n");
        close(in);
        exit(1);
    }
    printf("input file contents sent to server\n");

    // close the connection to the swap server
    swap_close(sd);

    // close connection to input file
    close(in);

    return 0;
}
