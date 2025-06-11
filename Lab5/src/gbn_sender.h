#pragma once

/* Sends the 26 bytes of the alphabet over the socket described by sockfd to
 * dest. Also, introduces bit error at the rate of ber. */
void gbn_send_alphabet_to(int sockfd, struct sockaddr_in dest, double ber);