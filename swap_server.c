/* Jimmy Pham
*  T00629354
*  COMP 3271
*  Thompson Rivers University
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdbool.h>

#define	MAXLINE	512	// maximum characters to receive and send at once
#define MAXFRAME 512

extern int swap_accept(unsigned short port);
extern int swap_disconnect(int sd);
extern int sdp_send(int sd, char *buf, int length);
extern int sdp_receive(int sd, char *buf);
extern int sdp_receive_with_timer(int sd, char *buf, unsigned int expiration);
extern int checksum(unsigned char buf[], int length);

int session_id = 0;
int R = 0;	// frame number to receive

int swap_wait(unsigned short port)
{
	/*
	*	if the session is already open, then return error
	*/

	if (session_id != 0)
		return -1;

	/*
	*	accept a connection
	*/

	session_id = swap_accept(port);	// in sdp.o

	/*
	*	return a ssession id
	*/

	return session_id;
}

int swap_read(int sd, char *buf)
{
	int	n;
	char	frame[MAXFRAME];
	bool error = false;

	if (session_id == 0 || sd != session_id){
		return -1;
	}

	while (true){

		/*
		*	receive a frame without a timer
		*/

		n= sdp_receive(sd, frame);
		
		if (n <= 0){
			// Something wrong, return -1 to client
			printf("sdp_recieve returns -1\n");
			return -1;
		}

		/*
		*	several cases
		*/
		if (checksum(frame, strlen(frame))==0){
			// Error in the frame, continue to receive
			continue;
		}

		unsigned short received_checksum = checksum(frame, strlen(frame));
		sscanf(frame, "Received checksum: %hd\n", &received_checksum);

		unsigned short expected_checksum = checksum((unsigned char *)frame, strlen(frame));
		// No errors
		if (received_checksum == expected_checksum){
		
		/*
		*	copy the data field in the frame into buf, and return the length
		*/

			char ack_frame[MAXFRAME];
			sprintf(ack_frame, "ACK"); 
			n = sdp_send(sd, ack_frame, strlen(ack_frame)); // send ACK frame

			// Copy the frame data to buf
            strncpy(buf, frame, MAXFRAME);
            buf[MAXFRAME - 1] = '\0'; // Ensure null-termination

			return strlen(buf);
		}
		else {
			printf("Server: Error - Checksum mismatch in received data.\n");
			continue; //receive again
		}
		
	}
}

void swap_close(int sd)
{
	if (session_id == 0 || sd != session_id)
		return;

	else
		session_id = 0;

	swap_disconnect(sd);	// in sdp.o
}
